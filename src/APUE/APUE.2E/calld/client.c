#include	"calld.h"

static void
client_alloc(void)		/* alloc more entries in the client[] array */
{
	int		i;

	if (client == NULL)
		client = malloc(NALLOC * sizeof(Client));
	else
		client = realloc(client,
		  (client_size + NALLOC) * sizeof(Client));
	if (client == NULL)
		err_sys("can't alloc for client array");

	/*
	 * Have to initialize the new entries.
	 */
	for (i = client_size; i < client_size + NALLOC; i++)
		client[i].fd = -1;	/* fd of -1 means entry available */
	client_size += NALLOC;
}

/*
 * Called by loop() when connection request from a new client arrives.
 */
int
client_add(int fd, uid_t uid)
{
	int		i;

	if (client == NULL)		/* first time we're called */
		client_alloc();
again:
	for (i = 0; i < client_size; i++) {
		if (client[i].fd == -1) {	/* find an available entry */
			client[i].fd = fd;
			client[i].uid = uid;
			return(i);	/* return index in client[] array */
		}
	}

	/*
	 * Client array full, time to realloc for more.
	 */
	client_alloc();
	goto again;		/* and search again (will work this time) */
}

/*
 * Called by loop() when we're done with a client.
 */
void
client_del(int fd)
{
	int		i;

	for (i = 0; i < client_size; i++) {
		if (client[i].fd == fd) {
			client[i].fd = -1;
			return;
		}
	}
	log_quit("can't find client entry for fd %d", fd);
}

/*
 * Find the client entry corresponding to a process ID.
 * This function is called by the sig_chld() signal
 * handler only after a child has terminated.
 */
void
client_sigchld(pid_t pid, int stat)
{
	int		i;

	for (i = 0; i < client_size; i++) {
		if (client[i].pid == pid) {
			client[i].childdone = stat;	/* child's exit() status +1 */
			return;
		}
	}
	log_quit("can't find client entry for pid %d", pid);
}
