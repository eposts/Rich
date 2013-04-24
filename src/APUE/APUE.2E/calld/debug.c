#include	"calld.h"
#include	<stdarg.h>

/*
 * Note that all debug output goes back to the client.
 */
void
DEBUG(char *fmt, ...)		/* debug output, newline at end */
{
	va_list	args;
	char	line[MAXLINE];
	int		n;

	if (Debug == 0)
		return;
	va_start(args, fmt);
	vsprintf(line, fmt, args);
	strcat(line, "\n");
	va_end(args);
	n = strlen(line);
	if (writen(clifd, line, n) != n)
		log_sys("writen error");
}

void
DEBUG_NONL(char *fmt, ...)	/* debug output, NO newline at end */
{
	va_list	args;
	char	line[MAXLINE];
	int		n;

	if (Debug == 0)
		return;
	va_start(args, fmt);
	vsprintf(line, fmt, args);
	va_end(args);
	n = strlen(line);
	if (writen(clifd, line, n) != n)
		log_sys("writen error");
}
