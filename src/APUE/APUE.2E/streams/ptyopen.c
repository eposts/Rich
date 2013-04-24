#include "apue.h"
#include <errno.h>
#include <fcntl.h>
#include <stropts.h>

int
ptym_open(char *pts_name, int pts_namesz)
{
	char	*ptr;
	int		fdm;

	/*
	 * Return the name of the master device so that on failure
	 * the caller can print an error message.  Null terminate
	 * to handle case where strlen("/dev/ptmx") > pts_namesz.
	 */
	strncpy(pts_name, "/dev/ptmx", pts_namesz);
	pts_name[pts_namesz - 1] = '\0';
	if ((fdm = open(pts_name, O_RDWR)) < 0)
		return(-1);
	if (grantpt(fdm) < 0) {		/* grant access to slave */
		close(fdm);
		return(-2);
	}
	if (unlockpt(fdm) < 0) {	/* clear slave's lock flag */
		close(fdm);
		return(-3);
	}
	if ((ptr = ptsname(fdm)) == NULL) {	/* get slave's name */
		close(fdm);
		return(-4);
	}

	/*
	 * Return name of slave.  Null terminate to handle
	 * case where strlen(ptr) > pts_namesz.
	 */
	strncpy(pts_name, ptr, pts_namesz);
	pts_name[pts_namesz - 1] = '\0';
	return(fdm);			/* return fd of master */
}

int
ptys_open(char *pts_name)
{
	int		fds, setup;

	/*
	 * The following open should allocate a controlling terminal.
	 */
	if ((fds = open(pts_name, O_RDWR)) < 0)
		return(-5);

	/*
	 * Check if stream is already set up by autopush facility.
	 */
	if ((setup = ioctl(fds, I_FIND, "ldterm")) < 0) {
		close(fds);
		return(-6);
	}
	if (setup == 0) {
		if (ioctl(fds, I_PUSH, "ptem") < 0) {
			close(fds);
			return(-7);
		}
		if (ioctl(fds, I_PUSH, "ldterm") < 0) {
			close(fds);
			return(-8);
		}
		if (ioctl(fds, I_PUSH, "ttcompat") < 0) {
			close(fds);
			return(-9);
		}
	}
	return(fds);
}
