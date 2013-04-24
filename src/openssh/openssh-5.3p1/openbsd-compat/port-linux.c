/* $Id: port-linux.c,v 1.5 2008/03/26 20:27:21 dtucker Exp $ */

/*
 * Copyright (c) 2005 Daniel Walsh <dwalsh@redhat.com>
 * Copyright (c) 2006 Damien Miller <djm@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Linux-specific portability code
 */

#include "includes.h"

#include <errno.h>
#include <stdarg.h>
#include <string.h>

#ifdef OOM_ADJUST
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "log.h"

#ifdef WITH_SELINUX
#include "key.h"
#include "hostfile.h"
#include "auth.h"
#ifdef HAVE_GETSEUSERBYNAME
#include "xmalloc.h"
#endif
#include "port-linux.h"

#include <selinux/selinux.h>
#include <selinux/flask.h>
#include <selinux/get_context_list.h>

extern Authctxt *the_authctxt;

/* Wrapper around is_selinux_enabled() to log its return value once only */
int
ssh_selinux_enabled(void)
{
	static int enabled = -1;

	if (enabled == -1) {
		enabled = is_selinux_enabled();
		debug("SELinux support %s", enabled ? "enabled" : "disabled");
	}

	return (enabled);
}

/* Return the default security context for the given username */
static security_context_t
ssh_selinux_getctxbyname(char *pwname)
{
	security_context_t sc = NULL;
	char *sename = NULL, *role = NULL, *lvl = NULL;
	int r;

#ifdef HAVE_GETSEUSERBYNAME
	if (getseuserbyname(pwname, &sename, &lvl) != 0)
		return NULL;
#else
	sename = pwname;
	lvl = NULL;
#endif
	if (the_authctxt)
		role = the_authctxt->role;

#ifdef HAVE_GET_DEFAULT_CONTEXT_WITH_LEVEL
	if (role != NULL && role[0])
		r = get_default_context_with_rolelevel(sename, role, lvl, NULL,
						       &sc);
	else
		r = get_default_context_with_level(sename, lvl, NULL, &sc);
#else
	if (role != NULL && role[0])
		r = get_default_context_with_role(sename, role, NULL, &sc);
	else
		r = get_default_context(sename, NULL, &sc);
#endif

	if (r != 0) {
		switch (security_getenforce()) {
		case -1:
			fatal("%s: ssh_selinux_getctxbyname: "
			    "security_getenforce() failed", __func__);
		case 0:
			error("%s: Failed to get default SELinux security "
			    "context for %s", __func__, pwname);
			break;
		default:
			fatal("%s: Failed to get default SELinux security "
			    "context for %s (in enforcing mode)",
			    __func__, pwname);
		}
	}

#ifdef HAVE_GETSEUSERBYNAME
	if (sename != NULL)
		xfree(sename);
	if (lvl != NULL)
		xfree(lvl);
#endif

	return (sc);
}

/* Set the execution context to the default for the specified user */
void
ssh_selinux_setup_exec_context(char *pwname)
{
	security_context_t user_ctx = NULL;

	if (!ssh_selinux_enabled())
		return;

	debug3("%s: setting execution context", __func__);

	user_ctx = ssh_selinux_getctxbyname(pwname);
	if (setexeccon(user_ctx) != 0) {
		switch (security_getenforce()) {
		case -1:
			fatal("%s: security_getenforce() failed", __func__);
		case 0:
			error("%s: Failed to set SELinux execution "
			    "context for %s", __func__, pwname);
			break;
		default:
			fatal("%s: Failed to set SELinux execution context "
			    "for %s (in enforcing mode)", __func__, pwname);
		}
	}
	if (user_ctx != NULL)
		freecon(user_ctx);

	debug3("%s: done", __func__);
}

/* Set the TTY context for the specified user */
void
ssh_selinux_setup_pty(char *pwname, const char *tty)
{
	security_context_t new_tty_ctx = NULL;
	security_context_t user_ctx = NULL;
	security_context_t old_tty_ctx = NULL;

	if (!ssh_selinux_enabled())
		return;

	debug3("%s: setting TTY context on %s", __func__, tty);

	user_ctx = ssh_selinux_getctxbyname(pwname);

	/* XXX: should these calls fatal() upon failure in enforcing mode? */

	if (getfilecon(tty, &old_tty_ctx) == -1) {
		error("%s: getfilecon: %s", __func__, strerror(errno));
		goto out;
	}

	if (security_compute_relabel(user_ctx, old_tty_ctx,
	    SECCLASS_CHR_FILE, &new_tty_ctx) != 0) {
		error("%s: security_compute_relabel: %s",
		    __func__, strerror(errno));
		goto out;
	}

	if (setfilecon(tty, new_tty_ctx) != 0)
		error("%s: setfilecon: %s", __func__, strerror(errno));
 out:
	if (new_tty_ctx != NULL)
		freecon(new_tty_ctx);
	if (old_tty_ctx != NULL)
		freecon(old_tty_ctx);
	if (user_ctx != NULL)
		freecon(user_ctx);
	debug3("%s: done", __func__);
}
#endif /* WITH_SELINUX */

#ifdef OOM_ADJUST
/* Get the out-of-memory adjustment file for the current process */
static int
oom_adj_open(int oflag)
{
	int fd = open("/proc/self/oom_adj", oflag);
	if (fd < 0)
		logit("error opening /proc/self/oom_adj: %s", strerror(errno));
	return fd;
}

/* Get the current OOM adjustment */
int
oom_adj_get(char *buf, size_t maxlen)
{
	ssize_t n;
	int fd = oom_adj_open(O_RDONLY);
	if (fd < 0)
		return -1;
	n = read(fd, buf, maxlen);
	if (n < 0)
		logit("error reading /proc/self/oom_adj: %s", strerror(errno));
	else
		buf[n] = '\0';
	close(fd);
	return n < 0 ? -1 : 0;
}

/* Set the current OOM adjustment */
int
oom_adj_set(const char *buf)
{
	ssize_t n;
	int fd = oom_adj_open(O_WRONLY);
	if (fd < 0)
		return -1;
	n = write(fd, buf, strlen(buf));
	if (n < 0)
		logit("error writing /proc/self/oom_adj: %s", strerror(errno));
	close(fd);
	return n < 0 ? -1 : 0;
}
#endif
