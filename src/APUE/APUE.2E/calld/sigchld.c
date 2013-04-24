#include	"calld.h"
#include	<sys/wait.h>

/*
 * SIGCHLD handler, invoked when a child terminates.
 * Probably interrupts accept() in serv_accept().
 */
void
sig_chld(int signo)
{
	int		stat, errno_save;
	pid_t	pid;

	errno_save = errno;		/* log_msg() might change errno */
	chld_flag = 1;
	if ((pid = waitpid(-1, &stat, 0)) <= 0)
		log_sys("waitpid error");

	if (WIFEXITED(stat) != 0) /* set client childdone status for loop */
		client_sigchld(pid, WEXITSTATUS(stat)+1);
	else
		log_msg("child %d terminated abnormally: %04x", pid, stat);

	errno = errno_save;
}
