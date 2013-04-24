#include	"calld.h"
#include	<fcntl.h>

/*
 * Open the terminal line.
 */
int
tty_open(char *class, char *line, enum parity parity, int modem)
{
	int				fd, baud;
	char			devname[100];
	struct termios	term;

	/*
	 * First open the device.
	 */
	strcpy(devname, "/dev/");
	strcat(devname, line);
	if ((fd = open(devname, O_RDWR | O_NONBLOCK)) < 0) {
		sprintf(errmsg, "can't open %s: %s\n",
		  devname, strerror(errno));
		return(-1);
	}
	if (isatty(fd) == 0) {
		sprintf(errmsg, "%s is not a tty\n", devname);
		return(-1);
	}

	/*
	 * Fetch then set modem's terminal status.
	 */
	if (tcgetattr(fd, &term) < 0)
		log_sys("tcgetattr error");

	if (parity == NONE)
		term.c_cflag = CS8;
	else if (parity == EVEN)
		term.c_cflag = CS7 | PARENB;
	else if (parity == ODD)
		term.c_cflag = CS7 | PARENB | PARODD;
	else
		log_quit("unknown parity");
	term.c_cflag |= CREAD |			/* enable receiver */
					HUPCL;			/* lower modem lines on last close */
									/* 1 stop bit (since CSTOPB off) */
	if (modem == 0)
		term.c_cflag |= CLOCAL;		/* ignore modem status lines */

	term.c_oflag  = 0;				/* turn off all output processing */
	term.c_iflag  = IXON | IXOFF |	/* Xon/Xoff flow control (default) */
					IGNBRK |		/* ignore breaks */
					ISTRIP |		/* strip input to 7 bits */
					IGNPAR;			/* ignore input parity errors */
	term.c_lflag  = 0;		/* everything off in local flag:
							   disables canonical mode, disables
							   signal generation, disables echo */
	term.c_cc[VMIN]  = 1;	/* 1 byte at a time, no timer */
	term.c_cc[VTIME] = 0;

	if (strcmp(class, "38400") == 0) {
		baud = B38400;
	} else if (strcmp(class, "19200") == 0) {
		baud = B19200;
	} else if (strcmp(class, "9600") == 0) {
		baud = B9600;
	} else if (strcmp(class, "4800") == 0) {
		baud = B4800;
	} else if (strcmp(class, "2400") == 0) {
		baud = B2400;
	} else if (strcmp(class, "1800") == 0) {
		baud = B1800;
	} else if (strcmp(class, "1200") == 0) {
		baud = B1200;
	} else if (strcmp(class, "600") == 0) {
		baud = B600;
	} else if (strcmp(class, "300") == 0) {
		baud = B300;
	} else if (strcmp(class, "200") == 0) {
		baud = B200;
	} else if (strcmp(class, "150") == 0) {
		baud = B150;
	} else if (strcmp(class, "134") == 0) {
		baud = B134;
	} else if (strcmp(class, "110") == 0) {
		baud = B110;
	} else if (strcmp(class, "75") == 0) {
		baud = B75;
	} else if (strcmp(class, "50") == 0) {
		baud = B50;
	} else {
		sprintf(errmsg, "invalid baud rate: %s\n", class);
		return(-1);
	}
	cfsetispeed(&term, baud);
	cfsetospeed(&term, baud);

	if (tcsetattr(fd, TCSANOW, &term) < 0)	/* set attributes */
		log_sys("tcsetattr error");

	DEBUG("tty open");
	clr_fl(fd, O_NONBLOCK);		/* turn off nonblocking */
	return(fd);
}
