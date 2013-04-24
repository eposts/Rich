#include	"calld.h"

#define	EXPALRM		45			/* alarm time to read expect string */

static int		expalarm = EXPALRM;
static volatile sig_atomic_t	caught_alrm;

size_t			/* read one byte, handle timeout errors & DEBUG */
exp_read(int fd, char *buf)
{
	if (caught_alrm) {	/* test flag before blocking in read */
		DEBUG("\nread timeout");
		return(-1);
	}
	if (read(fd, buf, 1) == 1) {
		DEBUG_NONL("%s", ctl_str(*buf));
		return(1);
	}
	if (errno == EINTR && caught_alrm)
		DEBUG("\nread timeout");
	else
		log_sys("read error");
	return(-1);
}

static void
sig_alrm(int signo)
{
	caught_alrm = 1;
}

int						/* return 0 if got it, -1 if not */
expect_str(int fd, char *ptr)
{
	char	expstr[MAXLINE], inbuf[MAXLINE];
	char	c;
	char	*src, *dst, *inptr, *cmpptr;
	int		i, matchlen;

	if (strcmp(ptr, "\"\"") == 0)
		goto returnok;		/* special case of "" (expect nothing) */

	/*
	 * Copy expect string, converting escape sequences.
	 */
	for (src = ptr, dst = expstr; (c = *src++) != 0; ) {
		if (c == '\\') {
			if (*src == 0) {
				sprintf(errmsg, "invalid expect string: %s\n", ptr);
				return(-1);
			}
			c = *src++;		/* char following backslash */
			switch (c) {
			case 'r':
				c = '\r';
				break;

			case 's':
				c = ' ';
				break;
				/* room for lots more case statements ... */

			default:
				sprintf(errmsg, "unknown expect escape char: \\%s\n",
				  ctl_str(c));
				return(-1);
			}
		}
		*dst++ = c;
	}
	*dst = 0;
	matchlen = strlen(expstr);
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		log_quit("signal error");
	caught_alrm = 0;

	alarm(expalarm);
	do {
		if (exp_read(fd, &c) < 0)
			return(-1);
	} while (c != expstr[0]);	/* skip until first chars equal */
	cmpptr = inptr = inbuf;
	*inptr = c;
	for (i = 1; i < matchlen; i++) {	/* read matchlen chars */
		inptr++;
		if (exp_read(fd, inptr) < 0)
			return(-1);
	}
	for ( ; ; ) {		/* keep reading until we have a match */
		if (strncmp(cmpptr, expstr, matchlen) == 0)
			break;		/* have a match */
		inptr++;
		if (exp_read(fd, inptr) < 0)
			return(-1);
		cmpptr++;
	}

returnok:
	alarm(0);
	DEBUG("\nexpect: got it");
	return(0);
}
