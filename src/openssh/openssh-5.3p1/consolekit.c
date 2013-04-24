/*
 * Copyright (c) 2008 Colin Watson.  All rights reserved.
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
 * Loosely based on pam-ck-connector, which is:
 *
 * Copyright (c) 2007 David Zeuthen <davidz@redhat.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "includes.h"

#ifdef USE_CONSOLEKIT

#include <ck-connector.h>

#include "xmalloc.h"
#include "channels.h"
#include "key.h"
#include "hostfile.h"
#include "auth.h"
#include "log.h"
#include "servconf.h"
#include "canohost.h"
#include "session.h"
#include "consolekit.h"

extern ServerOptions options;
extern u_int utmp_len;

void
set_active(const char *cookie)
{
	DBusError err;
	DBusConnection *connection;
	DBusMessage *message = NULL, *reply = NULL;
	char *sid;
	DBusMessageIter iter, subiter;
	const char *interface, *property;
	dbus_bool_t active;

	dbus_error_init(&err);
	connection = dbus_bus_get_private(DBUS_BUS_SYSTEM, &err);
	if (!connection) {
		if (dbus_error_is_set(&err)) {
			error("unable to open DBus connection: %s",
			    err.message);
			dbus_error_free(&err);
		}
		goto out;
	}
	dbus_connection_set_exit_on_disconnect(connection, FALSE);

	message = dbus_message_new_method_call("org.freedesktop.ConsoleKit",
	    "/org/freedesktop/ConsoleKit/Manager",
	    "org.freedesktop.ConsoleKit.Manager",
	    "GetSessionForCookie");
	if (!message)
		goto out;
	if (!dbus_message_append_args(message, DBUS_TYPE_STRING, &cookie,
	    DBUS_TYPE_INVALID)) {
		if (dbus_error_is_set(&err)) {
			error("unable to get current session: %s",
			    err.message);
			dbus_error_free(&err);
		}
		goto out;
	}

	dbus_error_init(&err);
	reply = dbus_connection_send_with_reply_and_block(connection, message,
	    -1, &err);
	if (!reply) {
		if (dbus_error_is_set(&err)) {
			error("unable to get current session: %s",
			    err.message);
			dbus_error_free(&err);
		}
		goto out;
	}

	dbus_error_init(&err);
	if (!dbus_message_get_args(reply, &err,
	    DBUS_TYPE_OBJECT_PATH, &sid,
	    DBUS_TYPE_INVALID)) {
		if (dbus_error_is_set(&err)) {
			error("unable to get current session: %s",
			    err.message);
			dbus_error_free(&err);
		}
		goto out;
	}
	dbus_message_unref(reply);
	dbus_message_unref(message);
	message = reply = NULL;

	message = dbus_message_new_method_call("org.freedesktop.ConsoleKit",
	    sid, "org.freedesktop.DBus.Properties", "Set");
	if (!message)
		goto out;
	interface = "org.freedesktop.ConsoleKit.Session";
	property = "active";
	if (!dbus_message_append_args(message,
	    DBUS_TYPE_STRING, &interface, DBUS_TYPE_STRING, &property,
	    DBUS_TYPE_INVALID))
		goto out;
	dbus_message_iter_init_append(message, &iter);
	if (!dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT,
	    DBUS_TYPE_BOOLEAN_AS_STRING, &subiter))
		goto out;
	active = TRUE;
	if (!dbus_message_iter_append_basic(&subiter, DBUS_TYPE_BOOLEAN,
	    &active))
		goto out;
	if (!dbus_message_iter_close_container(&iter, &subiter))
		goto out;

	dbus_error_init(&err);
	reply = dbus_connection_send_with_reply_and_block(connection, message,
	    -1, &err);
	if (!reply) {
		if (dbus_error_is_set(&err)) {
			error("unable to make current session active: %s",
			    err.message);
			dbus_error_free(&err);
		}
		goto out;
	}

out:
	if (reply)
		dbus_message_unref(reply);
	if (message)
		dbus_message_unref(message);
}

/*
 * We pass display separately rather than using s->display because the
 * latter is not available in the monitor when using privsep.
 */

char *
consolekit_register(Session *s, const char *display)
{
	DBusError err;
	const char *tty = s->tty;
	const char *remote_host_name;
	dbus_bool_t is_local = FALSE;
	const char *cookie = NULL;

	if (s->ckc) {
		debug("already registered with ConsoleKit");
		return xstrdup(ck_connector_get_cookie(s->ckc));
	}

	s->ckc = ck_connector_new();
	if (!s->ckc) {
		error("ck_connector_new failed");
		return NULL;
	}

	if (!tty)
		tty = "";
	if (!display)
		display = "";
	remote_host_name = get_remote_name_or_ip(utmp_len, options.use_dns);
	if (!remote_host_name)
		remote_host_name = "";

	dbus_error_init(&err);
	if (!ck_connector_open_session_with_parameters(s->ckc, &err,
	    "unix-user", &s->pw->pw_uid,
	    "display-device", &tty,
	    "x11-display", &display,
	    "remote-host-name", &remote_host_name,
	    "is-local", &is_local,
	    NULL)) {
		if (dbus_error_is_set(&err)) {
			debug("%s", err.message);
			dbus_error_free(&err);
		} else {
			debug("insufficient privileges or D-Bus / ConsoleKit "
			    "not available");
		}
		return NULL;
	}

	debug("registered uid=%d on tty='%s' with ConsoleKit",
	    s->pw->pw_uid, s->tty);

	cookie = ck_connector_get_cookie(s->ckc);
	set_active(cookie);
	return xstrdup(cookie);
}

void
consolekit_unregister(Session *s)
{
	if (s->ckc) {
		debug("unregistering ConsoleKit session %s",
		    ck_connector_get_cookie(s->ckc));
		ck_connector_unref(s->ckc);
		s->ckc = NULL;
	}
}

#endif /* USE_CONSOLEKIT */
