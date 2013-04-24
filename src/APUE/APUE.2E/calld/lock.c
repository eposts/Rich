#include	"calld.h"

typedef struct {
  char	*line;	/* points to malloc()ed area */
  				/* we lock by line (device name) */
  pid_t	pid;	/* but unlock by process ID */
				/* pid of 0 means available */
} Lock;
static Lock	*lock = NULL;	/* the malloc'ed/realloc'ed array */
static int	 lock_size;		/* #entries in lock[] */
static int	 nlocks;		/* #entries currently used in lock[] */

/*
 * Find the entry in lock[] for the specified device (line).
 * If we don't find it, create a new entry at the end of the
 * lock[] array for the new device.  This is how all the possible
 * devices get added to the lock[] array over time.
 */
static Lock *
find_line(char *line)
{
	int		i;
	Lock	*lptr;

	for (i = 0; i < nlocks; i++) {
		if (strcmp(line, lock[i].line) == 0)
			return(&lock[i]);	/* found entry for device */
	}

	/*
	 * Entry not found.  This device has never been locked before.
	 * Add a new entry to lock[] array.
	 */
	if (nlocks >= lock_size) {	/* lock[] array is full */
		if (lock == NULL)		/* first time through */
			lock = malloc(NALLOC * sizeof(Lock));
		else
			lock = realloc(lock, (lock_size + NALLOC) * sizeof(Lock));
		if (lock == NULL)
			err_sys("can't alloc for lock array");
		lock_size += NALLOC;
	}

	lptr = &lock[nlocks++];
	if ((lptr->line = malloc(strlen(line) + 1)) == NULL)
		log_sys("malloc error");
	strcpy(lptr->line, line);	/* copy caller's line name */
	lptr->pid  = 0;
	return(lptr);
}

void
lock_set(char *line, pid_t pid)
{
	Lock	*lptr;

	log_msg("locking %s for pid %d", line, pid);
	lptr = find_line(line);
	lptr->pid  = pid;
}

void
lock_rel(pid_t pid)
{
	Lock	*lptr;

	for (lptr = &lock[0]; lptr < &lock[nlocks]; lptr++) {
		if (lptr->pid == pid) {
			log_msg("unlocking %s for pid %d", lptr->line, pid);
			lptr->pid  = 0;
			return;
		}
	}
	log_msg("can't find lock for pid = %d", pid);
}

pid_t
is_locked(char *line)
{
	return(find_line(line)->pid);	/* nonzero pid means locked */
}
