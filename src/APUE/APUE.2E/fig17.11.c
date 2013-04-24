#include "apue.h"
#include <stropts.h>

/*
 * Wait for a client connection to arrive, and accept it.
 * We also obtain the client's user ID.
 * Returns new fd if all OK, <0 on error.
 */
int
serv_accept(int listenfd, uid_t *uidptr)
{
	struct strrecvfd	recvfd;

	if (ioctl(listenfd, I_RECVFD, &recvfd) < 0)
		return(-1);		/* could be EINTR if signal caught */
	if (uidptr != NULL)
		*uidptr = recvfd.uid;	/* effective uid of caller */
	return(recvfd.fd);	/* return the new descriptor */
}
