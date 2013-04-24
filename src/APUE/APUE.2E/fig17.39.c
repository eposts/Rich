#include	"opend.h"
#include	<poll.h>
#if !defined(BSD) && !defined(MACOS)
#include	<stropts.h>
#endif

void
loop(void)
{
	int				i, maxi, listenfd, clifd, nread;
	char			buf[MAXLINE];
	uid_t			uid;
	struct pollfd	*pollfd;

	if ((pollfd = malloc(open_max() * sizeof(struct pollfd))) == NULL)
		err_sys("malloc error");

	/* obtain fd to listen for client requests on */
	if ((listenfd = serv_listen(CS_OPEN)) < 0)
		log_sys("serv_listen error");
	client_add(listenfd, 0);	/* we use [0] for listenfd */
	pollfd[0].fd = listenfd;
	pollfd[0].events = POLLIN;
	maxi = 0;

	for ( ; ; ) {
		if (poll(pollfd, maxi + 1, -1) < 0)
			log_sys("poll error");

		if (pollfd[0].revents & POLLIN) {
			/* accept new client request */
			if ((clifd = serv_accept(listenfd, &uid)) < 0)
				log_sys("serv_accept error: %d", clifd);
			i = client_add(clifd, uid);
			pollfd[i].fd = clifd;
			pollfd[i].events = POLLIN;
			if (i > maxi)
				maxi = i;
			log_msg("new connection: uid %d, fd %d", uid, clifd);
		}

		for (i = 1; i <= maxi; i++) {
			if ((clifd = client[i].fd) < 0)
				continue;
			if (pollfd[i].revents & POLLHUP) {
				goto hungup;
			} else if (pollfd[i].revents & POLLIN) {
				/* read argument buffer from client */
				if ((nread = read(clifd, buf, MAXLINE)) < 0) {
					log_sys("read error on fd %d", clifd);
				} else if (nread == 0) {
hungup:
					log_msg("closed: uid %d, fd %d",
					  client[i].uid, clifd);
					client_del(clifd);	/* client has closed conn */
					pollfd[i].fd = -1;
					close(clifd);
				} else {		/* process client's request */
					request(buf, nread, clifd, client[i].uid);
				}
			}
		}
	}
}
