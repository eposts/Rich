#include	"calld.h"

static FILE	*fpdial = NULL;
static int	diallineno;			/* for error messages */
static char	dialline[MAXLINE];
		/* can't be automatic; dial_next() returns pointers into here */

/*
 * Read and break apart a line in the Dialers file.
 */
int
dial_next(Dialers *dialptr)	/* pointers in structure are filled in */
{
	if (fpdial == NULL) {
		if ((fpdial = fopen(DIALERS, "r")) == NULL)
			log_sys("can't open %s", DIALERS);
		diallineno = 0;
	}

again:
	if (fgets(dialline, MAXLINE, fpdial) == NULL)
		return(-1);		/* EOF */
	diallineno++;
	if ((dialptr->dialer = strtok(dialline, WHITE)) == NULL) {
		if (dialline[0] == '\n')
			goto again;		/* ignore empty line */
		log_quit("missing `dialer' in Dialers file, line %d",
		  diallineno);
	}
	if (dialptr->dialer[0] == '#')
		goto again;			/* ignore comment line */
	if ((dialptr->sub = strtok(NULL, WHITE)) == NULL)
		log_quit("missing `sub' in Dialers file, line %d",
		  diallineno);
	if ((dialptr->expsend = strtok(NULL, "\n")) == NULL)
		log_quit("missing `expsend' in Dialers file, line %d",
		  diallineno);

	return(0);
}

void
dial_rew(void)
{
	if (fpdial != NULL)
		rewind(fpdial);
	diallineno = 0;
}

/*
 * Find a dialer match.
 */
int
dial_find(Dialers *dialptr, const Devices *devptr)
{
	dial_rew();
	while (dial_next(dialptr) >= 0) {
		if (strcmp(dialptr->dialer, devptr->dialer) == 0)
			return(0);		/* found a dialer match */
	}
	sprintf(errmsg, "dialer `%s' not found\n", dialptr->dialer);
	return(-1);
}
