#include	"calld.h"

/*
 * This function is called by buf_args(), which is called by loop().
 * buf_args() has broken up the client's buffer into an argv[] style
 * array, which is now processed.
 */
int
cli_args(int argc, char **argv)
{
	int		c;

	if (argc < 2 || strcmp(argv[0], CL_CALL) != 0) {
		strcpy(errmsg, "usage: call <options> <hostname>");
		return(-1);
	}
	Debug = 0;		/* option defaults */
	parity = NONE;
	speed = NULL;
	opterr = 0;		/* don't want getopt() writing to stderr */
	optind = 1;		/* since we call getopt() multiple times */
	while ((c = getopt(argc, argv, "des:o")) != EOF) {
		switch (c) {
		case 'd':
			Debug = 1;	/* client wants DEBUG() output */
			break;

		case 'e':		/* even parity */
			parity = EVEN;
			break;

		case 'o':		/* odd parity */
			parity = ODD;
			break;

		case 's':		/* speed */
			speed = optarg;
			break;

		case '?':
			sprintf(errmsg, "unrecognized option: -%c\n", optopt);
			return(-1);
		}
	}
	if (optind >= argc) {
		sprintf(errmsg, "missing <hostname> to call\n");
		return(-1);
	}
	sysname = argv[optind];		/* name of host to call */
	return(0);
}
