#include	"calld.h"

static FILE	*fpsys = NULL;
static int	 syslineno;			/* for error messages */
static char	 sysline[MAXLINE];
		/* can't be automatic; sys_next() returns pointers into here */

/*
 * Read and break apart a line in the Systems file.
 */
long						/* return >0 if OK, -1 on EOF */
sys_next(Systems *sysptr)	/* structure is filled in with pointers */
{
	if (fpsys == NULL) {
		if ((fpsys = fopen(SYSTEMS, "r")) == NULL)
			log_sys("can't open %s", SYSTEMS);
		syslineno = 0;
	}

again:
	if (fgets(sysline, MAXLINE, fpsys) == NULL)
		return(-1);		/* EOF */
	syslineno++;
	if ((sysptr->name = strtok(sysline, WHITE)) == NULL) {
		if (sysline[0] == '\n')
			goto again;		/* ignore empty line */
		log_quit("missing `name' in Systems file, line %d", syslineno);
	}
	if (sysptr->name[0] == '#')
		goto again;			/* ignore comment line */
	if ((sysptr->time = strtok(NULL, WHITE)) == NULL)
		log_quit("missing `time' in Systems file, line %d", syslineno);
	if ((sysptr->type = strtok(NULL, WHITE)) == NULL)
		log_quit("missing `type' in Systems file, line %d", syslineno);
	if ((sysptr->class = strtok(NULL, WHITE)) == NULL)
		log_quit("missing `class' in Systems file, line %d", syslineno);
	if ((sysptr->phone = strtok(NULL, WHITE)) == NULL)
		log_quit("missing `phone' in Systems file, line %d", syslineno);
	if ((sysptr->login = strtok(NULL, "\n")) == NULL)
		log_quit("missing `login' in Systems file, line %d", syslineno);

	return(ftell(fpsys));	/* return the position in Systems file */
}

void
sys_rew(void)
{
	if (fpsys != NULL)
		rewind(fpsys);
	syslineno = 0;
}

void
sys_posn(long posn)		/* position Systems file */
{
	if (posn == 0)
		sys_rew();
	else if (fseek(fpsys, posn, SEEK_SET) != 0)
		log_sys("fseek error");
}
