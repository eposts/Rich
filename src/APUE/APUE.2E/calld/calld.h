#include "apue.h"
#include <errno.h>

#define	CS_CALL	"/home/sar/calld"	/* well-known name */
#define	CL_CALL	"call"
#define	MAXSYSNAME	256
#define	MAXSPEEDSTR	256

/*
 * Number of structures to alloc/realloc for Client structs
 * (client.c) and Lock structs (lock.c).
 */
#define	NALLOC	10

#define	WHITE		" \t\n"				/* for separating tokens */
#define	SYSTEMS		"./Systems"			/* my own copies for now */
#define	DEVICES		"./Devices"
#define	DIALERS		"./Dialers"

extern int    clifd;
extern int    Debug;	/* nonzero for dialing debug output */
extern char   errmsg[];	/* error message string to return to client */
extern char   *speed;	/* speed (actually "class") to use */
extern char   *sysname;	/* name of system to call */
extern uid_t  uid;      /* client's uid */
extern volatile sig_atomic_t  chld_flag;	/* when SIGCHLD occurs */
extern enum parity { NONE, EVEN, ODD } parity;	/* specified by client */

typedef struct {	/* one Client struct per connected client */
  int	fd;			/* fd, or -1 if available */
  pid_t	pid;		/* child pid while dialing */
  uid_t	uid;		/* client's user ID */
  int	childdone;	/* nonzero when SIGCHLD from dialing child recvd:
					   1 means exit(0), 2 means exit(1) */
  long	sysftell;	/* next line to read in Systems file */
  long	foundone;	/* true if we find a matching sysfile entry */
  int	Debug;				/* option from client */
  enum parity parity;		/* option from client */
  char	speed[MAXSPEEDSTR];	/* option from client */
  char	sysname[MAXSYSNAME];/* option from client */
} Client;

/* both manipulated by client_XXX() functions */
extern Client *client;     /* ptr to malloc'ed array of Client structs */
extern int    client_size; /* # entries in client[] array */

typedef struct {	/* everything for one entry in Systems file */
  char	*name;		/* system name */
  char	*time;		/* (e.g., "Any") time to call (ignored) */
  char	*type;		/* (e.g., "ACU") or system name if direct connect */
  char	*class;		/* (e.g., "9600") speed */
  char	*phone;		/* phone number or "-" if direct connect */
  char	*login;		/* uucp login chat (ignored) */
} Systems;

typedef struct {	/* everything for one entry in Devices file */
  char	*type;		/* (e.g., "ACU") matched by type in Systems */
  char	*line;		/* (e.g., "cua0") without preceding "/dev/" */
  char	*line2;		/* (ignored) */
  char	*class;		/* matched by class in Systems */
  char	*dialer;	/* name of dialer in Dialers */
} Devices;

typedef struct {	/* everything for one entry in Dialers file */
  char	*dialer;	/* matched by dialer in Devices */
  char	*sub;		/* phone number substitution string (ignored) */
  char	*expsend;	/* expect/send chat */
} Dialers;

extern Systems	systems;	/* filled in by sys_next() */
extern Devices	devices;	/* filled in by dev_next() */
extern Dialers	dialers;	/* filled in by dial_next() */

void  child_dial(Client *);					/* childdial.c */
int   cli_args(int, char **);				/* cliargs.c */
int   client_add(int, uid_t);				/* client.c */
void  client_del(int);
void  client_sigchld(pid_t, int);
void  loop(void);							/* loop.c */
char  *ctl_str(unsigned char);				/* ctlstr.c */
int   dev_find(Devices *, const Systems *);	/* devfile.c */
int   dev_next(Devices *);
void  dev_rew(void);
int   dial_find(Dialers *, const Devices *); /* dialfile.c */
int   dial_next(Dialers *);
void  dial_rew(void);
int   expect_str(int, char *);				/* expectstr.c */
int   request(Client *);					/* request.c */
int   send_str(int, char *, char *, int);	/* sendstr.c */
void  sig_chld(int);						/* sigchld.c */
long  sys_next(Systems *);					/* sysfile.c */
void  sys_posn(long);
void  sys_rew(void);
int   tty_open(char *, char *, enum parity, int);	/* ttyopen.c */
int   tty_dial(int, char *, char *, char *, char *);	/* ttydial.c */
pid_t is_locked(char *);					/* lock.c */
void  lock_set(char *, pid_t);
void  lock_rel(pid_t);
void  DEBUG(char *, ...);					/* debug.c */
void  DEBUG_NONL(char *, ...);
