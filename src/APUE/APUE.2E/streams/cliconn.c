#include "apue.h"
#include <fcntl.h>
#include <stropts.h>

/*
 * Create a client endpoint and connect to a server.
 * Returns fd if all OK, <0 on error.
 */
int
cli_conn(const char *name)
{
	int		fd;

	/* open the mounted stream */
	if ((fd = open(name, O_RDWR)) < 0)
		return(-1);
	if (isastream(fd) == 0) {
		close(fd);
		return(-2);
	}
	return(fd);
}
