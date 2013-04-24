#include "apue.h"
#include <stropts.h>

/*
 * Pass a file descriptor to another process.
 * If fd<0, then -fd is sent back instead as the error status.
 */
int
send_fd(int fd, int fd_to_send)
{
	char	buf[2];		/* send_fd()/recv_fd() 2-byte protocol */

	buf[0] = 0;			/* null byte flag to recv_fd() */
	if (fd_to_send < 0) {
		buf[1] = -fd_to_send;	/* nonzero status means error */
		if (buf[1] == 0)
			buf[1] = 1;	/* -256, etc. would screw up protocol */
	} else {
		buf[1] = 0;		/* zero status means OK */
	}

	if (write(fd, buf, 2) != 2)
		return(-1);

	if (fd_to_send >= 0)
		if (ioctl(fd, I_SENDFD, fd_to_send) < 0)
			return(-1);
	return(0);
}
