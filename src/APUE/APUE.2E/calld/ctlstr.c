#include	"calld.h"

/*
 * Make a printable string of the character "c", which may be a
 * control character.  Works only with ASCII.
 */
char *
ctl_str(unsigned char c)
{
	static char	tempstr[6];		/* biggest is "\177" + null */

	c &= 255;
	if (c == 0)
		return("\\0");			/* really shouldn't see a null */
	else if (c < 040)
		sprintf(tempstr, "^%c", c + 'A' - 1);
	else if (c == 0177)
		return("DEL");
	else if (c > 0177)
		sprintf(tempstr, "\\%03o", c);
	else
		sprintf(tempstr, "%c", c);
	return(tempstr);
}
