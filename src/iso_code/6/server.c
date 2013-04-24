
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <stat.h>
#include <ctype.h>
#include <dir.h>
#include <dos.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include "pport.h"

int receive_file(int handle, long size);

#define R_BLOCK   2048          
#define DIR_BUF_SIZE 30000      

char buffer[R_BLOCK];
char dir_buffer[DIR_BUF_SIZE];

#define MAX_WAIT  1200

main()
{
	int i, j, k, count, h;
	long size, free, total;
	char name[121], *endptr, b[131], *dn, *p, *limit;
	struct ffblk fb;
	struct dfree df;
	
	
	printf("\n\n\n                            File transfer server\n\n");
	
	pp_initialize(0);		
	
	while(1)
	{  j = pp_read_data_block(&count, buffer);	
	
	if (j == CHAR1_TIMEOUT)	
	{  if (kbhit())		
	{  k = getch();
	if (k == 'q' || k == 'Q')
	{  pp_initialize(0);
	exit(0);
	}
	}
	continue;
	}
	if (j != OK) barfr(j);	
	
	buffer[count] = 0;              
	
	if (!strnicmp(buffer, "quit", 4))
	{  printf("QUIT command received\n");
	pp_initialize(0);
	exit(0);
	}
	
	else if(!strnicmp(buffer, "HI", 2))
	{  pp_delay();                     
	pp_send("HI");			
	}
	
	else if(!strnicmp(buffer, "PWD", 3))  
	{  getcwd(b, 121);		    
	pp_send(b);
	}
	
	else if(!strnicmp(buffer, "CD", 2))   
	{  dn = buffer + 3;                   
	
	
	if (dn[1] == ':') 		    
	{  if (strlen(dn) == 2) strcat(dn, "\\");
	j = toupper(dn[0]) - 'A';
	k = setdisk(j);		    
	if (j >= k)
	{  pp_send("BAD DISK");
	continue;
	}
	}
	k = chdir(dn);			   
	if (k != 0)
	{  pp_send("BAD DIRECTORY");
	continue;
	}
	pp_send("OK");
	}
	
	else if(!strnicmp(buffer, "DIR", 3))   
	{  dn = buffer + 4;                    
	j = strlen(dn);
	
	if (j == 0 || dn[j-1] == ':' || dn[j-1] == '\\') strcat(dn, "*.*");
	
	
	p = dir_buffer;
	limit = dir_buffer + DIR_BUF_SIZE - 100;
	k = 0;
	total = 0;
	j = findfirst(dn, &fb, FA_DIREC);	
	if (j == 0) do
	{  if (fb.ff_attrib == FA_DIREC)
		j = sprintf(p, "%-12s          [Dir]\n", fb.ff_name);
	else
	{  j = sprintf(p, "%-12s   %8ld   %2d/%02d/%2d\n",	    
	fb.ff_name, fb.ff_fsize, (fb.ff_fdate >> 5) & 0xf,
	fb.ff_fdate & 0x1f, (fb.ff_fdate >> 9) + 80);
	total += fb.ff_fsize;
	}
	p += j;
	k++;
	if (p > limit)					
	{  p = stpcpy(p, "Directory too big\n");
	break;
	}
	} while (0 == findnext(&fb));			
	if(dn[1] == ':') j = toupper(dn[0]) - 'A' + 1;
	else j = 0;
	getdfree(j, &df);
	sprintf(p, "... %d files - %ld bytes total, %ld bytes free ...\n\n",
		k, total, (long)df.df_avail * df.df_bsec * df.df_sclus);
	pp_send(dir_buffer);				
	}
	
	
	
	else if(!strnicmp(buffer, "MD", 2))   
	{  dn = buffer + 3;                 
	k = mkdir(dn);			
	if (k != 0)
	{  pp_send("CAN\'T CREATE DIRECTORY");
	continue;
	}
	pp_send("OK");
	}
	
	else if(!strnicmp(buffer, "PUT", 3))
	{  size = strtol(buffer+3, &endptr, 10);	
	buffer[count] = 0;
	strcpy(name, endptr + 1);
	h = open(name, O_RDONLY);
	if (h >= 0)            
	{  close(h);
	sprintf(b, "%s EXISTS", name);
	pp_send(b);
	}
	else
	{  h = open(name, O_WRONLY | O_BINARY | O_CREAT, S_IREAD | S_IWRITE);
	if (h < 0) pp_send("CAN\'T OPEN");
	else
	{  pp_send("SEND");
	receive_file(h, size);			
	close(h);
	printf("Received: %s\n", name);
	}
	}
	}
	
	else       
	{  buffer[8] = 0;
	printf("Unrecognized command >%s<\n", buffer);
	}
   }
}





int receive_file(int handle, long size)	
{  int i, j, n;

while(size > 0)
{  for (i=0;  i<MAX_WAIT;  i++)
{  j = pp_read_data_block(&n, buffer);
if (j != CHAR1_TIMEOUT) break;
}
if (j == CSUM_FAIL || j == COUNT_FAIL)	
{  printf("Repeat\n");
pp_send("REPEAT");
continue;
}
else if (j == OK)
{  write(handle, buffer, n);
size -= n;
pp_send("OK");				
}
else barfr(j);
}
}






pp_send(char *s)          
{
	int j;
	
	while(1)
	{  j = pp_send_data_block(strlen(s), s);
	if (j == OK) return;
	if (j != CHAR1_TIMEOUT) barft(j);
	}
}


barfr(int j)               
{  int i;

printf("Data receive failed with return code %d\n", j);
for(i=0;  i<8;  i++) printf("  %02x", buffer[i]);
printf("\n");
pp_initialize(0);
exit(99);
}


barft(int j)			
{
	printf("Transmission failed with error code %d\n", j);
	pp_initialize(0);
	exit(99);
}

