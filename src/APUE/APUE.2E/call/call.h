#include "apue.h"
#include <errno.h>
#include <sys/time.h>

#define	CS_CALL	"/home/sar/calld"	/* well-known server name */
#define	CL_CALL	"call"				/* command for server */

extern char	 escapec;	/* tilde for local commands */
extern char	*src;		/* for take and put commands */
extern char	*dst;		/* for take and put commands */

int		call(const char *);
int		doescape(int);
void	loop(int);
int		prompt_read(char *, int (*)(int, char **));
void	put(int);
void	take(int);
int		take_put_args(int, char **);
