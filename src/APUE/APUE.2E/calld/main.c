#include	"calld.h"
#include	<syslog.h>

int			clifd, log_to_stderr, client_size;
int			Debug;	/* Debug controlled by client, not cmd line */
char		errmsg[MAXLINE];
char		*speed, *sysname;
uid_t		uid;
Client		*client = NULL;
Systems		systems;
Devices		devices;
Dialers		dialers;
volatile sig_atomic_t chld_flag;
enum parity parity = NONE;

int
main(int argc, char *argv[])
{
	int		c;

	log_open("calld", LOG_PID, LOG_USER);

	opterr = 0;		/* don't want getopt() writing to stderr */
	while ((c = getopt(argc, argv, "d")) != EOF) {
		switch (c) {
		case 'd':		/* debug */
			log_to_stderr = 1;
			break;

		case '?':
			log_quit("unrecognized option: -%c", optopt);
		}
	}

	if (log_to_stderr == 0)
		daemonize("calld");

	loop();		/* never returns */
}
