#include	"calld.h"

int							/* return 0 if OK, -1 on error */
request(Client *cliptr)
{
	pid_t	pid;

	/*
	 * Position where this client left off last (or rewind).
	 */
	errmsg[0] = 0;
	sys_posn(cliptr->sysftell);
	while ((cliptr->sysftell = sys_next(&systems)) >= 0) {
		if (strcmp(cliptr->sysname, systems.name) == 0) {
			/*
			 * System match;
			 * if client specified a speed, it must match too.
			 */
			if (cliptr->speed[0] != 0 &&
				strcmp(cliptr->speed, systems.class) != 0)
					continue;	/* speeds don't match */

			DEBUG("trying sys: %s, %s, %s, %s", systems.name,
			  systems.type, systems.class, systems.phone);
			cliptr->foundone++;
			if (dev_find(&devices, &systems) < 0)
				break;

			DEBUG("trying dev: %s, %s, %s, %s", devices.type,
			  devices.line, devices.class, devices.dialer);
			if ((pid = is_locked(devices.line)) != 0) {
				sprintf(errmsg, "device `%s' already locked by pid %d\n",
				  devices.line, pid);
				continue;	/* look for another entry in Systems file */
			}

			/*
			 * We've found a device that's not locked.
			 * fork() a child to to the actual dialing.
			 */
			TELL_WAIT();
			if ((cliptr->pid = fork()) < 0) {
				log_sys("fork error");
			} else if (cliptr->pid == 0) {	/* child */
				WAIT_PARENT();		/* let parent set lock */
				child_dial(cliptr);	/* never returns */
			}

			/* parent */
			lock_set(devices.line, cliptr->pid);

			/*
			 * Let child resume, now that lock is set.
			 */
			TELL_CHILD(cliptr->pid);
			return(0);	/* we've started a child */
		}
	}

	/*
	 * Reached EOF on Systems file.
	 */
	if (cliptr->foundone == 0)
		sprintf(errmsg, "system `%s' not found\n", cliptr->sysname);
	else if (errmsg[0] == 0)
		sprintf(errmsg, "unable to connect to system `%s'\n",
		  cliptr->sysname);
	return(-1);		/* also, cliptr->sysftell is -1 */
}
