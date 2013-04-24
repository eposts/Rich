#include	"calld.h"

int
send_str(int fd, char *ptr, char *phone, int echocheck)
{
	char	c, tempc;

	/*
	 * Go though send string, converting escape sequences on the fly.
	 */
	while ((c = *ptr++) != 0) {
		if (c == '\\') {
			if (*ptr == 0) {
				sprintf(errmsg, "backslash at end of send string\n");
				return(-1);
			}
			c = *ptr++;		/* char following backslash */

			switch (c) {
			case 'c':		/* no CR, if at end of string */
				if (*ptr == 0)
					goto returnok;
				continue;	/* ignore if not at end of string */

			case 'd':		/* 2 second delay */
				DEBUG_NONL("<delay>");
				sleep(2);
				continue;

			case 'p':		/* 0.25 second pause */
				DEBUG_NONL("<pause>");
				sleep_us(250000);	/* {Ex sleepus} */
				continue;

			case 'e':
				DEBUG_NONL("<echo check off>");
				echocheck = 0;
				continue;

			case 'E':
				DEBUG_NONL("<echo check on>");
				echocheck = 1;
				continue;

			case 'T':		/* output phone number */
				send_str(fd, phone, phone, echocheck);	/* recursive */
				continue;

			case 'r':
				c = '\r';
				break;

			case 's':
				c = ' ';
				break;
				/* room for lots more case statements ... */

			default:
				sprintf(errmsg, "unknown send escape char: \\%s\n",
				  ctl_str(c));
				return(-1);
			}
		}

		DEBUG_NONL("%s", ctl_str(c));
		if (write(fd, &c, 1) != 1)
			log_sys("write error");
		if (echocheck) {		/* wait for char to be echoed */
			do {
				if (read(fd, &tempc, 1) != 1)
					log_sys("read error");
				DEBUG_NONL("{%s}", ctl_str(tempc));
			} while (tempc != c);
		}
	}
	c = '\r';	/* if no \c at end of string, CR written at end */
	DEBUG_NONL("%s", ctl_str(c));
	if (write(fd, &c, 1) != 1)
		log_sys("write error");

returnok:
	DEBUG("");
	return(0);
}
