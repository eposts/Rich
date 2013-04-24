#include	"call.h"

#define	CTRLA	001		/* eof designator for take */

static int		 rem_read(int);
static char		 rem_buf[MAXLINE];
static char		*rem_ptr;
static int		 rem_cnt = 0;

/*
 * Copy a file from remote to local.
 */
void
take(int remfd)
{
	int		n, linecnt;
	char	c;
	char	cmd[MAXLINE];
	FILE	*fpout;

	if (prompt_read("~[take] ", take_put_args) < 0) {
		printf("usage: [take] <sourcefile> <destfile>\n\r");
		return;
	}

	/*
	 * Open local output file.
	 */
	if ((fpout = fopen(dst, "w")) == NULL) {
		err_ret("can't open %s for writing", dst);
		putc('\r', stderr);
		fflush(stderr);
		return;
	}

	/*
	 * Send cat/echo command to remote host.
	 */
	sprintf(cmd, "cat %s; echo %c\r", src, CTRLA);
	n = strlen(cmd);
	if (write(remfd, cmd, n) != n)
		err_sys("write error");

	/*
	 * Read echo of cat/echo command line from remote host
	 */
	rem_cnt = 0;		/* initialize rem_read() */
	for ( ; ; ) {
		if ((c = rem_read(remfd)) == 0)
			return;		/* line has dropped */
		if (c == '\n')
			break;		/* end of echo line */
	}

	/*
	 * Read file from remote host.
	 */
	linecnt = 0;
	for ( ; ; ) {
		if ((c = rem_read(remfd)) == 0)
			break;			/* line has dropped */
		if (c == CTRLA)
			break;			/* all done */
		if (c == '\r')
			continue;		/* ignore returns */
		if (c == '\n')		/* but newlines are written to file */
			printf("\r%d", ++linecnt);
		if (putc(c, fpout) == EOF)
			break;			/* output error */
	}
	if (ferror(fpout) || fclose(fpout) == EOF) {
		err_msg("output error to local file");
		putc('\r', stderr);
		fflush(stderr);
	}
	c = '\n';
	write(remfd, &c, 1);
}

/*
 * Read from remote.  Read up to MAXLINE, but parcel out one
 * character at a time.
 */
int
rem_read(int remfd)
{
	if (rem_cnt <= 0) {
		if ((rem_cnt = read(remfd, rem_buf, MAXLINE)) < 0)
			err_sys("read error");
		else if (rem_cnt == 0)
			return(0);
		rem_ptr = rem_buf;
	}
	rem_cnt--;
	return(*rem_ptr++ & 0177);
}
