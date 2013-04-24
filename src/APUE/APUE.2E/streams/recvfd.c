#include "apue.h"
#include <stropts.h>

/*
 * Receive a file descriptor from another process (a server).
 * In addition, any data received from the server is passed
 * to (*userfunc)(STDERR_FILENO, buf, nbytes).  We have a
 * 2-byte protocol for receiving the fd from send_fd().
 */
int
recv_fd(int fd, ssize_t (*userfunc)(int, const void *, size_t))
{
	int					newfd, nread, flag, status;
	char				*ptr;
	char				buf[MAXLINE];
	struct strbuf		dat;
	struct strrecvfd	recvfd;

	status = -1;
	for ( ; ; ) {
		dat.buf = buf;
		dat.maxlen = MAXLINE;
		flag = 0;
		if (getmsg(fd, NULL, &dat, &flag) < 0)
			err_sys("getmsg error");
		nread = dat.len;
		if (nread == 0) {
			err_ret("connection closed by server");
			return(-1);
		}

		/*
		 * See if this is the final data with null & status.
		 * Null must be next to last byte of buffer, status
		 * byte is last byte.  Zero status means there must
		 * be a file descriptor to receive.
		 */
		for (ptr = buf; ptr < &buf[nread]; ) {
			if (*ptr++ == 0) {
				if (ptr != &buf[nread-1])
					err_dump("message format error");
 				status = *ptr & 0xFF;	/* prevent sign extension */
 				if (status == 0) {
					if (ioctl(fd, I_RECVFD, &recvfd) < 0)
						return(-1);
					newfd = recvfd.fd;	/* new descriptor */
				} else {
					newfd = -status;
				}
				nread -= 2;
			}
		}
		if (nread > 0)
			if ((*userfunc)(STDERR_FILENO, buf, nread) != nread)
				return(-1);

		if (status >= 0)	/* final data has arrived */
			return(newfd);	/* descriptor, or -status */
	}
}
