#include	"call.h"

/*
 * Process the argv-style arguments for take or put commands.
 */
int
take_put_args(int argc, char **argv)
{
	if (argc == 1) {
		src = dst = argv[0];
		return(0);
	} else if (argc == 2) {
		src = argv[0];
		dst = argv[1];
		return(0);
	}
	return(-1);
}

/*
 * Can't be automatic; src/dst point into here.
 */
static char	cmdargs[MAXLINE];

/*
 * Read a line from the user.  Call our buf_args() function to
 * break it into an argv-style array, and call userfunc() to
 * process the arguments.
 */
int
prompt_read(char *prompt, int (*userfunc)(int, char **))
{
	int		n;
	char	c, *ptr;

	tty_reset(STDIN_FILENO);	/* allow user's editing chars */

	n = strlen(prompt);
	if (write(STDOUT_FILENO, prompt, n) != n)
		err_sys("write error");

	ptr = cmdargs;
	for ( ; ; ) {
		if ((n = read(STDIN_FILENO, &c, 1)) < 0)
			err_sys("read error");
		else if (n == 0)
			break;
		if (c == '\n')
			break;
		if (ptr < &cmdargs[MAXLINE-2])
			*ptr++ = c;
	}
	*ptr = 0;		/* null terminate */

	tty_raw(STDIN_FILENO);		/* reset tty mode to raw */

	/*
	 * Return whatever userfunc() returns.
	 */
	return(buf_args(cmdargs, userfunc));
}
