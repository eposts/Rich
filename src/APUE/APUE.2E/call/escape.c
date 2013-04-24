#include	"call.h"

/*
 * Called when first character of a line is the escape character
 * (tilde).  Read the next character and process.  Return -1
 * if next character is "terminate" character, 0 if next character
 * is valid command character (that's been processed), or next
 * character itself (if the next character is not special).
 */
int
doescape(int remfd)
{
	char	c;

	if (read(STDIN_FILENO, &c, 1) != 1)		/* next input char */
		err_sys("read error from stdin");

	if (c == escapec) {		/* two in a row -> process as one */
		return(escapec);
	} else if (c == '.') {	/* terminate */
		write(STDOUT_FILENO, "~.\n\r", 4);
		return(-1);

#ifdef	VSUSP
	} else if (c == tty_termios()->c_cc[VSUSP]) { /* suspend client */
		tty_reset(STDIN_FILENO);	/* restore tty mode */
		kill(getpid(), SIGTSTP);	/* suspend ourself */
		tty_raw(STDIN_FILENO);		/* and reset tty to raw */
		return(0);
#endif

	} else if (c == '#') {	/* generate break */
		tcsendbreak(remfd, 0);
		return(0);
	} else if (c == 't') {	/* take a file from remote host */
		take(remfd);
		return(0);
	} else if (c == 'p') {	/* put a file to remote host */
		put(remfd);
		return(0);
	}

	return(c);			/* not a special character */
}
