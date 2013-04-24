#include	"calld.h"
#include	<errno.h>

static void	cli_done(int);
static void child_done(int);

/*
 * One bit per client cxn, plus one for listenfd;
 * modified by loop() and cli_done()
 */
static fd_set	allset;

void
loop(void)
{
	int		 i, n, maxfd, maxi, listenfd, nread;
	char	 buf[MAXLINE];
	Client	*cliptr;
	uid_t	 uid;
	fd_set	 rset;

	if (signal_intr(SIGCHLD, sig_chld) == SIG_ERR)
		log_sys("signal error");

	/*
	 * Obtain descriptor to listen for client requests on.
	 */
	if ((listenfd = serv_listen(CS_CALL)) < 0)
		log_sys("serv_listen error");

	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	maxfd = listenfd;
	maxi = -1;

	for ( ; ; ) {
		if (chld_flag)
			child_done(maxi);
		rset = allset;		/* rset gets modified each time around */
		if ((n = select(maxfd + 1, &rset, NULL, NULL, NULL)) < 0) {
			if (errno == EINTR) {
				/*
				 * Caught SIGCHLD, find entry with childdone set.
				 */
				child_done(maxi);
				continue;		/* issue the select again */
			} else {
				log_sys("select error");
			}
		}

		if (FD_ISSET(listenfd, &rset)) {
			/*
			 * Accept new client request.
			 */
			if ((clifd = serv_accept(listenfd, &uid)) < 0)
				log_sys("serv_accept error: %d", clifd);
			i = client_add(clifd, uid);
			FD_SET(clifd, &allset);
			if (clifd > maxfd)
				maxfd = clifd;	/* max fd for select() */
			if (i > maxi)
				maxi = i;		/* max index in client[] array */
			log_msg("new connection: uid %d, fd %d", uid, clifd);
			continue;
		}

		/*
		 * Go through client[] array.
		 * Read any client data that has arrived.
		 */
		for (cliptr = &client[0]; cliptr <= &client[maxi]; cliptr++) {
			if ((clifd = cliptr->fd) < 0)
				continue;
			if (FD_ISSET(clifd, &rset)) {
				/*
				 * Read argument buffer from client.
				 */
				if ((nread = read(clifd, buf, MAXLINE)) < 0) {
					log_sys("read error on fd %d", clifd);
				} else if (nread == 0) {
					/*
					 * The client has terminated or closed
					 * the stream pipe.  Now we can release
					 * its device lock.
					 */
					log_msg("closed: uid %d, fd %d",
					  cliptr->uid, clifd);
					lock_rel(cliptr->pid);
					cli_done(clifd);
					continue;
				}

				/*
				 * Data has arrived from the client.
				 * Process the client's request.
				 */
				if (buf[nread-1] != 0) {
					log_quit("request from uid %d not null terminated:"
					  " %*.*s", uid, nread, nread, buf);
					cli_done(clifd);
					continue;
				}
				log_msg("starting: %s, from uid %d", buf, uid);

				/*
				 * Parse the arguments, set options.  Since
				 * we may need to try calling again for this
				 * client, save options in client[] array.
				 */
				if (buf_args(buf, cli_args) < 0)
					log_quit("command line error: %s", buf);
				cliptr->Debug = Debug;
				cliptr->parity = parity;
				strcpy(cliptr->sysname, sysname);
				strcpy(cliptr->speed, (speed == NULL) ? "" : speed);
				cliptr->childdone = 0;
				cliptr->sysftell = 0;
				cliptr->foundone = 0;

				if (request(cliptr) < 0) {
					/*
					 * System not found, or unable to connect.
					 */
					if (send_err(cliptr->fd, -1, errmsg) < 0)
						log_sys("send_err error");
					cli_done(clifd);
					continue;
				}
				/*
				 * At this point request() has forked a child that is
				 * trying to dial the remote system.  We'll find
				 * out the child's status when it terminates.
				 */
			}
		}
	}
}

/*
 * Go through the client[] array looking for clients whose dialing
 * children have terminated.  This function is called by loop() when
 * chld_flag (the flag set by the SIGCHLD handler) is nonzero.
 */
static void
child_done(int maxi)
{
	Client	*cliptr;

again:
	chld_flag = 0;	/* to check when done with loop for more SIGCHLDs */
	for (cliptr = &client[0]; cliptr <= &client[maxi]; cliptr++) {
		if ((clifd = cliptr->fd) < 0)
			continue;
		if (cliptr->childdone) {
			log_msg("child done: pid %d, status %d",
			  cliptr->pid, cliptr->childdone-1);

			/*
			 * If the child was successful (exit(0)), just clear the
			 * flag.  When the client terminates, we'll read the EOF
			 * on the stream pipe above and release the device lock.
			 */
			if (cliptr->childdone == 1) {	/* child did exit(0) */
				cliptr->childdone = 0;
				continue;
			}

			/*
			 * Unsuccessful: child did exit(1).  Release the device
			 * lock and try again from where we left off.
			 */
			cliptr->childdone = 0;
			lock_rel(cliptr->pid);	/* unlock the device entry */
			if (request(cliptr) < 0) {
				/*
				 * Still unable, time to give up.
				 */
				if (send_err(cliptr->fd, -1, errmsg) < 0)
					log_sys("send_err error");
				cli_done(clifd);
				continue;
			}
			/*
			 * request() has forked another child for this client.
			 */
		}
	}
	if (chld_flag)	/* additional SIGCHLDs have been caught */
		goto again;	/* need to check all childdone flags again */
}

/*
 * Clean up when we're done with a client.
 */
static void
cli_done(int clifd)
{
	client_del(clifd);		/* delete entry in client[] array */
	FD_CLR(clifd, &allset);	/* turn off bit in select() set */
	close(clifd);			/* close our end of stream pipe */
}
