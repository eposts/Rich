#include	"calld.h"

static FILE	*fpdev = NULL;
static int	devlineno;			/* for error messages */
static char	devline[MAXLINE];
		/* can't be automatic; dev_next() returns pointers into here */

/*
 * Read and break apart a line in the Devices file.
 */
int
dev_next(Devices *devptr)	/* pointers in structure are filled in */
{
	if (fpdev == NULL) {
		if ((fpdev = fopen(DEVICES, "r")) == NULL)
			log_sys("can't open %s", DEVICES);
		devlineno = 0;
	}

again:
	if (fgets(devline, MAXLINE, fpdev) == NULL)
		return(-1);		/* EOF */
	devlineno++;
	if ((devptr->type = strtok(devline, WHITE)) == NULL) {
		if (devline[0] == '\n')
			goto again;		/* ignore empty line */
		log_quit("missing `type' in Devices file, line %d",
		  devlineno);
	}
	if (devptr->type[0] == '#')
		goto again;			/* ignore comment line */
	if ((devptr->line = strtok(NULL, WHITE)) == NULL)
		log_quit("missing `line' in Devices file, line %d",
		  devlineno);
	if ((devptr->line2 = strtok(NULL, WHITE)) == NULL)
		log_quit("missing `line2' in Devices file, line %d",
		  devlineno);
	if ((devptr->class = strtok(NULL, WHITE)) == NULL)
		log_quit("missing `class' in Devices file, line %d",
		  devlineno);
	if ((devptr->dialer = strtok(NULL, WHITE)) == NULL)
		log_quit("missing `dialer' in Devices file, line %d",
		  devlineno);

	return(0);
}

void
dev_rew(void)
{
	if (fpdev != NULL)
		rewind(fpdev);
	devlineno = 0;
}

/*
 * Find a match of type and class.
 */
int
dev_find(Devices *devptr, const Systems *sysptr)
{
	dev_rew();
	while (dev_next(devptr) >= 0) {
		if (strcmp(sysptr->type, devptr->type) == 0 &&
			strcmp(sysptr->class, devptr->class) == 0)
				return(0);		/* found a device match */
	}
	sprintf(errmsg, "device `%s'/`%s' not found\n", sysptr->type,
	  sysptr->class);
	return(-1);
}
