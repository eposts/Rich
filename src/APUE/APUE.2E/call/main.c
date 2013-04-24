#include	"call.h"

char	 escapec = '~';
char	*src;
char	*dst;

static void	usage(char *);

int
main(int argc, char *argv[])
{
	int			c, remfd, debug;
	char		args[MAXLINE];

	args[0] = 0;	/* build arg list for conn server here */
	opterr = 0;		/* don't want getopt() writing to stderr */
	while ((c = getopt(argc, argv, "des:o")) != EOF) {
		switch (c) {
		case 'd':		/* debug */
			debug = 1;
			strcat(args, "-d ");
			break;

		case 'e':		/* even parity */
			strcat(args, "-e ");
			break;

		case 'o':		/* odd parity */
			strcat(args, "-o ");
			break;

		case 's':		/* speed */
			strcat(args, "-s ");
			strcat(args, optarg);
			strcat(args, " ");
			break;

		case '?':
			usage("unrecognized option");
		}
	}
	if (optind < argc)
		strcat(args, argv[optind]);	/* name of host to call */
	else
		usage("missing <hostname> to call");

	if ((remfd = call(args)) < 0)	/* place the call */
		exit(1);	/* call() prints reason for failure */
	printf("Connected\n");
	if (tty_raw(STDIN_FILENO) < 0)	/* user's tty to raw mode */
		err_sys("tty_raw error");
	if (atexit(tty_atexit) < 0)		/* reset user's tty on exit */
		err_sys("atexit error");

	loop(remfd);					/* and do it */

	printf("Disconnected\n\r");
	exit(0);
}

static void
usage(char *msg)
{
	err_quit("%s\nusage: call -d -e -o -s<speed> <hostname>", msg);
}
