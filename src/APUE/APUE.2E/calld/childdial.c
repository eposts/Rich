#include	"calld.h"

/*
 * The child does the actual dialing and sends the fd back to
 * the client.  This function can't return to caller; it must exit.
 * If successful, exit(0), else exit(1).
 * The child uses the following global variables, which are just
 * in the copy of the data space from the parent:
 *		cliptr->fd (to send DEBUG() output and fd back to client),
 *		cliptr->Debug (for all DEBUG() output), cliptr->parity,
 *		systems, devices, dialers.
 */
void
child_dial(Client *cliptr)
{
	int		fd, n;

	Debug = cliptr->Debug;
	DEBUG("child, pid %d", getpid());

	if (strcmp(devices.dialer, "direct") == 0) { /* direct tty line */
		fd = tty_open(systems.class, devices.line, cliptr->parity, 0);
		if (fd < 0)
			goto die;
	} else {			/* else assume dialing is needed */
		if (dial_find(&dialers, &devices) < 0)
			goto die;
		fd = tty_open(systems.class, devices.line, cliptr->parity, 1);
		if (fd < 0)
			goto die;
		if (tty_dial(fd, systems.phone, dialers.dialer,
		  dialers.sub, dialers.expsend) < 0)
			goto die;
	}
	DEBUG("done");

	/*
	 * Send the open descriptor to client.
	 */
	if (send_fd(cliptr->fd, fd) < 0)
		log_sys("send_fd error");
	exit(0);	/* parent will see this */

die:
	/*
	 * The child can't call send_err() as that would send the final
	 * 2-byte protocol to the client.  We just send our error message
	 * back to the client.  If the parent finally gives up, it'll
	 * call send_err().
	 */
	n = strlen(errmsg);
	if (writen(cliptr->fd, errmsg, n) != n)	/* send error to client */
		log_sys("writen error");
	exit(1);	/* parent will see this, release lock, and try again */
}
