

#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <dos.h>
#include <dir.h>
#include "pport.h"

#define TRUE   1
#define FALSE  0

#define MAX_WAIT  1200

void view(char *s);
int get_reply(char *b);
int transmit_file(int handle, long size, char *name);

#define T_BLOCK   2048           
#define DIR_BUF_SIZE 30000       

unsigned char buffer[T_BLOCK];
char dir_buffer[DIR_BUF_SIZE];

main()
{
   int i, j, n, h, k;
   long size, total;
   char b[81], *p1, *p2, *p3, *pz, *p, *limit, n2[121], root[81], n1[101];
   struct ffblk fb;
   struct dfree df;
   
   printf("\n\n\n                            File transfer client\n\n");
   
   pp_initialize(0);			

   pp_send("HI", TRUE);			
   n = get_reply(buffer);		

   if (strnicmp(buffer, "HI", 2))
   {  printf("Bad response from server\n");	
      pp_initialize(0);	
      exit(1);
   }
   printf("Server is alive and well\n");
   
   while(1)
   {  printf(">>");
      fgets(b, 81, stdin);		
      p1 = strtok(b, " \n");		
      if (p1 == NULL) continue;
      p2 = strtok(NULL, " \n");		
      
      if (!strnicmp(p1, "quit", 4))	
      {  pp_send("quit", FALSE);	
         pp_initialize(0);
         exit(0);
      }

      else if(!strnicmp(p1, "pwd", 3)) 
      {  pp_send("PWD", FALSE);
         n = get_reply(buffer);
         buffer[81] = 0;
         printf("%s\n", buffer);
      }
      
      else if(!strnicmp(p1, "LPWD", 4))   
      {  getcwd(b, 81);			  
         printf("%s\n", b);
      }

      
      else if (!strnicmp(p1, "cd", 2))       
      {  if (p2 == NULL)
         {  printf("ERROR: no directory given\n");
            continue;
         }
         sprintf(buffer, "CD %s", p2);	    
         pp_send(buffer);		    
         n = get_reply(buffer);
         if (!strnicmp(buffer, "OK", 2))
         {  pp_send("PWD", FALSE);	    
            n = get_reply(buffer);
            buffer[121] = 0;
            strupr(buffer);
            printf("Directory on server is now %s\n", buffer);
         }
         else printf("%s\n", buffer);
      }

      else if(!strnicmp(p1, "LCD", 3))    
      {  if (p2 == NULL)
         {  printf("ERROR: no directory given\n");
            continue;
         }
         if (p2[1] == ':') 		
         {  if (strlen(p2) == 2) strcat(p2, "\\");
            j = toupper(p2[0]) - 'A';
            k = setdisk(j);
            if (j >= k)
            {  printf("BAD DISK\n");
               continue;
            }
         }
         k = chdir(p2);			
         if (k != 0)
         {  printf("BAD DIRECTORY\n");
            continue;
         }
         else				
         {  getcwd(buffer, 500);
            strupr(buffer);
            printf("Local directory is now %s\n", buffer);
         }
      }
      
      else if (!strnicmp(p1, "md", 2))       
      {  if (p2 == NULL)
         {  printf("ERROR: no directory given\n");
            continue;
         }
         sprintf(buffer, "MD %s", p2);	     
         pp_send(buffer);		     
         n = get_reply(buffer);
         printf("%s\n", buffer);
      }
      

      else if (!strnicmp(p1, "dir", 3))       
      {  if (p2 == NULL) p2 = "";
         sprintf(buffer, "DIR %s", p2);	      
         pp_send(buffer);		      
         n = get_reply(dir_buffer);
         view(dir_buffer);		      
      }

      else if(!strnicmp(p1, "LDIR", 4))   
      {  if (p2 == NULL) 
         {  b[0] = 0;
            p2 = b;
         }
         j = strlen(p2);
         
         if (j == 0 || p2[j-1] == ':' || p2[j-1] == '\\') strcat(p2, "*.*");

         p = dir_buffer;
         limit = dir_buffer + DIR_BUF_SIZE - 100;
         k = 0;
         total = 0;
         j = findfirst(p2, &fb, FA_DIREC);
         if (j == 0) do
         {  if (fb.ff_attrib == FA_DIREC)/**/
               j = sprintf(p, "%-12s          [Dir]\n", fb.ff_name);
            else			/**/
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
         if(p2[1] == ':') j = toupper(p2[0]) - 'A' + 1;
         else j = 0;
         getdfree(j, &df);
         sprintf(p, "... %d files - %ld bytes total, %ld bytes free ...\n\n",
                   k, total, (long)df.df_avail * df.df_bsec * df.df_sclus);
         view(dir_buffer);	
      }


      else if (!strnicmp(p1, "put", 3))      
      {  if (p2 == NULL)                     
         {  printf("ERROR: no file name\n");
            continue;
         }
         pz = strtok(NULL, " \n");     


         if (NULL != strchr(p2, '?') || NULL != strchr(p2, '*'))
         {                    

            if (pz != NULL)
            {  n = strlen(pz);
               if(pz[n-1] != '\\') strcat(pz, "\\");
            }     
         }


         strcpy(root, p2);
         n = strlen(p2);
         for (i=n;  i > 0;  i--)
         {  if (root[i-1] == ':' || root[i-1] == '\\') break;
         }
         root[i] = 0;      


         j = findfirst(p2, &fb, 0);	
         if (j != 0)
         {  printf("No such file\n");
            continue;
         }

         else do
         {  p2 = fb.ff_name;            
            p3 = pz;
            if (p3 == NULL) p3 = p2;
            j = strlen(p3);
            if (p3[j-1] == ':' || p3[j-1] == '\\') 
            {  strcpy(n2, p3);
               strcat(n2, p2);            
               p3 = n2;
            }
            strcpy(n1, root);
            strcat(n1, p2);               
            p2 = n1;

            h = open(p2, O_RDONLY | O_BINARY);
            if (h < 0)
            {  printf("Can\'t open\n");
               continue;
            }
            size = filelength(h);		
            sprintf(buffer, "PUT%ld %s", size, p3); 
                                             
            pp_send(buffer, FALSE);		
            n = get_reply(buffer);

            if(!strnicmp(buffer, "SEND", 4)) j = transmit_file(h, size, p2);
            else
            {  buffer[n] = 0;
               printf("%s\n", buffer);
            }
            close(h);
         } while (0 == findnext(&fb));     
      }
      
      else if (!strnicmp(p1, "HELP", 4))   
      {  printf("\n Client commands:\n\n"
            "QUIT\n"
            "PUT     source   [dest]   source may contain *?, dest may be directory\n"
            "DIR     [template]        display directory on server\n"
            "LDIR    [template]        display local directory (on client)\n"
            "PWD                       print current directory on server\n"
            "LPWD                      print current directory on client\n"
            "CD      dir               change directory on server\n"
            "LCD     dir               local change directory\n"
            "MD      dir               make directory on server\n"
            "\n"
         );
      }            

      else     
      {  printf("UNKNOWN COMMAND\n");
      }
   }
}


int get_reply(char *b)        
{  int i, j, n;

   for (i=0;  i<MAX_WAIT;  i++)        
   {  j = pp_read_data_block(&n, b);
      if (j != CHAR1_TIMEOUT) break;
   }
   if (j != OK) barfr(j);

   b[n] = 0;                           
   return(n);
}





int transmit_file(int handle, long size, char *name)	
{  int j, n, k, i;
   char b[81];

   while(size > 0)
   {  n = (size > T_BLOCK)? T_BLOCK : size;
      read(handle, buffer, n);		
      do
      {  j = pp_send_data_block(n, buffer);
         if (j != OK) barft(j);

         k = get_reply(b);
      } while (!strnicmp(b, "REPEAT", 6));

      if (strnicmp(b, "OK", 2))
      {  b[k] = 0;
         printf("Screw-up: %s\n", b);
         return;
      }
      size -= n;
   }
   printf("%s transmitted ok\n", name);
   return;
}

pp_send(char *s, int msg)        
{
   int flag, j, k;
   
   
   flag = TRUE;
   while(1)
   {  j = pp_send_data_block(strlen(s), s);  

      if (j == OK) return;
      if (j != CHAR1_TIMEOUT) barft(j);

      if (msg && flag) printf("Waiting for server to respond\n");
      flag = FALSE;

      if (kbhit())
      {  k = getch();
         if (k == 'q' || k == 'Q') exit(0);
         continue;
      }
   }
}


barft(int j)                
{
   printf("Data transmit failed with return code %d\n", j);
   pp_initialize(0);
   exit(99);
}


barfr(int j)                
{
   printf("Data receive failed with return code %d\n", j);
   pp_initialize(0);
   exit(99);
}


#define PAGE_LENGTH  25

void view(char *s)      
{  char *p;
   int j, k;
   
   if (!isatty(fileno(stdout))) printf("%s", s);
   else
   {  j = 0;
      p = s;
      while(0 != (k = *p++))
      {  putchar(k);
         if (k == '\n')
         {  j++;
            if (j >= PAGE_LENGTH - 2)
            {  printf("..... more .....\n");
               getch();
               j = 0;
            }
         }
      }
   }
}