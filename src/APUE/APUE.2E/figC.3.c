#include "apue.h"
#include <fcntl.h>

#define	DEPTH	100			/* directory depth */
#define	MYHOME	"/home/sar"
#define	NAME	"alonglonglonglonglonglonglonglonglonglongname"
#define MAXSZ	8192

int
main(void)
{
	int		i, size;
	char	*path;

	if (chdir(MYHOME) < 0)
		err_sys("chdir error");

	for (i = 0; i < DEPTH; i++) {
		if (mkdir(NAME, DIR_MODE) < 0)
			err_sys("mkdir failed, i = %d", i);
		if (chdir(NAME) < 0)
			err_sys("chdir failed, i = %d", i);
	}
	if (creat("afile", FILE_MODE) < 0)
		err_sys("creat error");

	/*
	 * The deep directory is created, with a file at the leaf.
	 * Now let's try to obtain its pathname.
	 */
	path = path_alloc(&size);
	for ( ; ; ) {
		if (getcwd(path, size) != NULL) {
			break;
		} else {
			err_ret("getcwd failed, size = %d", size);
			size += 100;
			if (size > MAXSZ)
				err_quit("giving up");
			if ((path = realloc(path, size)) == NULL)
				err_sys("realloc error");
		}
	}
	printf("length = %d\n%s\n", strlen(path), path);

	exit(0);
}

