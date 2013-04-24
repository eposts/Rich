

#include "tdestr.h"
#include "common.h"
#include "tdefunc.h"
#include "criterr.h"


#define CEH_ROW         5
#define CEH_COL         6
#define CEH_WIDTH       69
#define CEH_HEIGHT      15

#define CEH_OFFSET      ((CEH_ROW * 160) + (CEH_COL * 2))

#define NEXT_LINE       160



int ceh_buffer[CEH_HEIGHT][CEH_WIDTH];



int  far crit_err_handler( void )
{
int  rc;
int  c;

   save_area( (char far *)ceh_buffer );
   show_error_screen( CEH_ROW, CEH_COL );
   xygoto( 60, 17 );
   do
      c = getkey( );
   while (c != 'Q' && c != 'q' && c != 'R' && c != 'r' && c != 'A' && c != 'a');
   switch ( c ) {
      case 'A':
      case 'a':
         rc = ABORT;
         break;
      case 'Q':
      case 'q':
         rc = FAIL;
         break;
      case 'R':
      case 'r':
      default :
         rc = RETRY;
         break;
   }
   restore_area( (char far *)ceh_buffer );
   return( rc );
}



void show_error_screen( int row, int col )
{
char **p;

   for (p=criterr_screen; *p != NULL; p++, row++)
      s_output( *p, row, col, g_display.help_color );
   s_output( error_code[ceh.code],    8, 23, g_display.help_color );
   s_output( operation[ceh.rw],       9, 23, g_display.help_color );
   if (ceh.dattr == 0)
      c_output( ceh.drive + 'a',     23, 10, g_display.help_color );
   else
      s_output( critt1,              10, 23, g_display.help_color );
   s_output( ext_err[ceh.extended],  11, 23, g_display.help_color );
   s_output( error_class[ceh.class], 12, 23, g_display.help_color );
   s_output( locus[ceh.locus],       13, 23, g_display.help_color );
   s_output( device_type[ceh.dattr], 14, 23, g_display.help_color );
   s_output( ceh.dattr == 0 ? critt1 : ceh.dname,
                                     15, 23, g_display.help_color );
}



void save_area( char far *dest )
{
char far *source;
register int hgt;

   source = (char far *)g_display.display_address + CEH_OFFSET;
   for (hgt=CEH_HEIGHT; hgt; hgt--) {
      _fmemcpy( dest, source, CEH_WIDTH*2 );
      source += NEXT_LINE;
      dest += (CEH_WIDTH*2);
   }
}



void restore_area( char far *source )
{
char far *dest;
register int hgt;

   dest = (char far *)g_display.display_address + CEH_OFFSET;
   for (hgt=CEH_HEIGHT; hgt; hgt--) {
      _fmemcpy( dest, source, CEH_WIDTH*2 );
      dest += NEXT_LINE;
      source += (CEH_WIDTH*2);
   }
}
