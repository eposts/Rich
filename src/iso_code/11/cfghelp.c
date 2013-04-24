

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tdecfg.h"
#include "cfghelp.h"

extern struct vcfg cfg;
extern FILE *tde_exe;                  
extern long help_offset;

static WINDOW *w_ptr;








void tdehelp( void )
{
int c;
int i;
char line[200];
char out_line[82];
char fname[82];
char *rc;
FILE *help_file;                  
long offset;


   cls( );
   show_box( 0, 0, help_screen, NORMAL );
   xygoto( 42, 14 );
   c = getkey( );
   while (c != '1' && c != '2')
      c = getkey( );
   if (c == '1') {
      puts( "" );
      puts( "" );
      puts( "" );
      puts( "Enter file name that contains new help screen :" );
      gets( fname );
      if ((c = access( fname, EXIST )) != 0) {
         puts( "\nFile not found.  Press any key to continue." );
         c = getkey( );
         cls( );
         return;
      } else if ((help_file = fopen( fname, "r" )) == NULL ) {
         puts( "\nCannot open help file.  Press any key to contine." );
         c = getkey( );
         cls( );
         return;
      }
      offset = help_offset + 8;
      rc = fgets( line, 100, help_file );
      for (c=0; c<25 && rc != NULL; c++) {
         memset( out_line, '\0', 82 );
         for (i=0; i<80 && line[i] != '\n'; i++)
             out_line[i] = line[i];
         fseek( tde_exe, offset, SEEK_SET );
         fwrite( out_line, sizeof( char ), 81, tde_exe );
         offset += 81;
         rc = fgets( line, 100, help_file );
      }
      fclose( help_file );
      puts( "" );
      puts( "" );
      puts( "" );
      puts( "New help screen successfully installed.  Press any key to continue." );
      c = getkey( );
   }
   cls( );
}
