#include	"calld.h"

int
tty_dial(int fd, char *phone, char *dialer, char *sub, char *expsend)
{
	char	*ptr;

	ptr = strtok(expsend, WHITE);	/* first expect string */
	for ( ; ; ) {
		DEBUG_NONL("expect = %s\nread: ", ptr);
		if (expect_str(fd, ptr) < 0)
			return(-1);

		if ((ptr = strtok(NULL, WHITE)) == NULL)
			return(0);		/* at the end of the expect/send */
		DEBUG_NONL("send = %s\nwrite: ", ptr);
		if (send_str(fd, ptr, phone, 0) < 0)
			return(-1);

		if ((ptr = strtok(NULL, WHITE)) == NULL)
			return(0);		/* at the end of the expect/send */
	}
}
