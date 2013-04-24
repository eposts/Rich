#include "apue.h"
#include <fcntl.h>
#include <stropts.h>

/* pipe permissions: user rw, group rw, others rw */
#define	FIFO_MODE  (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

/*
 * Establish an endpoint to listen for connect requests.
 * Returns fd if all OK, <0 on error
 */
int
serv_listen(const char *name)
{
	int		tempfd;
	int		fd[2];

	/*
	 * Create a file: mount point for fattach().
	 */
	unlink(name);
	if ((tempfd = creat(name, FIFO_MODE)) < 0)
		return(-1);
	if (close(tempfd) < 0)
		return(-2);
	if (pipe(fd) < 0)
		return(-3);
	/*
	 * Push connld & fattach() on fd[1].
	 */
	if (ioctl(fd[1], I_PUSH, "connld") < 0) {
		close(fd[0]);
		close(fd[1]);
		return(-4);
	}
	if (fattach(fd[1], name) < 0) {
		close(fd[0]);
		close(fd[1]);
		return(-5);
	}
	close(fd[1]);	/* fattach holds this end open */

	return(fd[0]);	/* fd[0] is where client connections arrive */
}
