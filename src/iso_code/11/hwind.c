

#include <bios.h>       
#include <dos.h>        

#include "tdestr.h"
#include "common.h"
#include "tdefunc.h"
#include "define.h"



void get_date( int *year, int *month, int *day, int *day_of_week  )
{
union REGS inregs, outregs;

   inregs.h.ah = 0x2a;
   intdos( &inregs, &outregs );
   *year        = (int)outregs.x.cx;
   *month       = (int)outregs.h.dh;
   *day         = (int)outregs.h.dl;
   *day_of_week = (int)outregs.h.al;
}



void get_time( int *hour, int *minutes, int *seconds, int *hundredths  )
{
union REGS inregs, outregs;

   inregs.h.ah = 0x2c;
   intdos( &inregs, &outregs );
   *hour       = (int)outregs.h.ch;
   *minutes    = (int)outregs.h.cl;
   *seconds    = (int)outregs.h.dh;
   *hundredths = (int)outregs.h.dl;
}



void show_modes( void )
{
char status_line[MAX_COLS+2];

   memset( status_line, ' ', MAX_COLS );
   status_line[MAX_COLS] = '\0';
   s_output( status_line, g_display.mode_line, 0, g_display.mode_color );
   s_output( "F=   W=", g_display.mode_line, 1, g_display.mode_color );
   s_output( "m=", g_display.mode_line, 12, g_display.mode_color );
   show_window_count( g_status.window_count );
   show_file_count( g_status.file_count );
   show_avail_mem( );
   show_tab_modes( );
   show_indent_mode( );
   show_sync_mode( );
   show_control_z( );
   show_insert_mode( );
   show_search_case( );
   show_wordwrap_mode( );
   show_trailing( );
}



void show_file_count( int fc )
{
char status_line[MAX_COLS+2];

   s_output( "  ", g_display.mode_line, 3, g_display.mode_color );
   s_output( itoa( fc, status_line, 10 ), g_display.mode_line, 3,
             g_display.mode_color );
}



void show_window_count( int wc )
{
char status_line[MAX_COLS+2];

   s_output( "  ", g_display.mode_line, 8, g_display.mode_color );
   s_output( itoa( wc, status_line, 10 ), g_display.mode_line, 8,
             g_display.mode_color );
}



void show_avail_mem( void )
{
char line[MAX_COLS+2];
unsigned long avail_mem;

#if defined( __MSC__ )
unsigned paragraphs;

   _dos_allocmem( 0xffff, &paragraphs );
  
   avail_mem = (long)paragraphs << 4;
#else
   avail_mem = farcoreleft( );
#endif

   s_output( "        ", g_display.mode_line, 14, g_display.mode_color );
   ultoa( avail_mem, line, 10 );
   s_output( line, g_display.mode_line, 14,
             g_display.mode_color );
}



void show_tab_modes( void )
{
char *blank_tab = "   ";
char ascii_tab[10];

   s_output( tabs, g_display.mode_line, 22, g_display.mode_color );
   s_output( mode.smart_tab ? smart : fixed, g_display.mode_line, 27,
             g_display.mode_color );
   s_output( mode.inflate_tabs ? intab : outtab, g_display.mode_line, 28,
             g_display.mode_color );
   s_output( blank_tab, g_display.mode_line, 29, g_display.mode_color );
   s_output( itoa( mode.ptab_size, ascii_tab, 10), g_display.mode_line, 29,
             g_display.mode_color );
}



void show_indent_mode( void )
{
   s_output( mode.indent ? indent : blank, g_display.mode_line, 32,
             g_display.mode_color );
}



void show_search_case( void )
{
   s_output( mode.search_case == IGNORE ? ignore : match, g_display.mode_line,
             40, g_display.mode_color );
}



void show_sync_mode( void )
{
   s_output( mode.sync ? sync_on : sync_off, g_display.mode_line, 48,
             g_display.mode_color );
}



void show_wordwrap_mode( void )
{
   s_output( ww_mode[mode.word_wrap], g_display.mode_line, 54,
             g_display.mode_color );
}



void show_trailing( void )
{
   c_output( mode.trailing ? 'T' : ' ', 66, g_display.mode_line,
             g_display.mode_color );
}



void show_control_z( void )
{
   c_output( mode.control_z ? 'Z' : ' ', 77, g_display.mode_line,
             g_display.mode_color );
}



void show_insert_mode( void )
{
   c_output( mode.insert ? 'i' : 'o', 79, g_display.mode_line,
             g_display.mode_color );
}



void my_scroll_down( WINDOW *window )
{
int  i;
int  curl;
int  eof;
WINDOW w;              

   if (!window->visible  ||  !g_status.screen_display)
      return;
   dup_window_info( &w, window );
   curl = i = window->bottom_line + 1 - window->cline;
   eof = FALSE;
   for (; i>0; i--) {
      if (w.ll->len != EOF) {
     
         if (i != curl)
            update_line( &w );
      } else if (eof == FALSE) {
         show_eof( &w );
         eof = TRUE;
      } else
         window_eol_clear( &w, COLOR_TEXT );
      if (w.ll->next != NULL)
         w.ll = w.ll->next;
      ++w.cline;
      ++w.rline;
   }
   show_curl_line( window );
}



void combine_strings( char *buff, char *s1, char *s2, char *s3 )
{
   assert( strlen( s1 ) + strlen( s2 ) + strlen( s3 ) < MAX_COLS );
   strcpy( buff, s1 );
   strcat( buff, s2 );
   strcat( buff, s3 );
}



void make_ruler( WINDOW *window )
{
register WINDOW *win;
char num[20];
register unsigned char *p;
int  len;
int  col;
int  i;
int  mod;

   win = window;

   if (win->bottom_line - win->top_line < 1)
      win->ruler = FALSE;
   if (win->ruler) {

    
      len = win->end_col + 1 - win->start_col;

      assert( len >= 0 );
      assert( len <= MAX_COLS );

      memset( win->ruler_line, RULER_FILL, len );
      win->ruler_line[len] = '\0';
      col = win->bcol+1;

      assert( col >= 1 );
      assert( col <= MAX_LINE_LENGTH );

      for (p=(unsigned char *)win->ruler_line; *p; col++, p++) {

      
         mod = col % 10;
         if (mod == 0) {
            itoa( col/10, num, 10 );

            for (i=0; num[i] && *p; col++, i++) {
               if (col == mode.left_margin+1)
                  *p = LM_CHAR;
               else if (col == mode.right_margin+1) {
                  if (mode.right_justify == TRUE)
                     *p = RM_CHAR_JUS;
                  else
                     *p = RM_CHAR_RAG;
               } else if (col == mode.parg_margin+1)
                  *p = PGR_CHAR;
               else
                  *p = num[i];
               p++;
            }

        
            if (*p == '\0')
               break;
         } else if (mod == 5)
            *p = RULER_TICK;
         if (col == mode.parg_margin+1)
            *p = PGR_CHAR;
         if (col == mode.left_margin+1)
            *p = LM_CHAR;
         else if (col == mode.right_margin+1) {
            if (mode.right_justify == TRUE)
               *p = RM_CHAR_JUS;
            else
               *p = RM_CHAR_RAG;
         }
      }
   }
}



void show_ruler( WINDOW *window )
{
   if (window->ruler && window->visible)
      s_output( window->ruler_line, window->top_line, window->start_col,
                g_display.ruler_color );
}



void show_ruler_char( WINDOW *window )
{
register WINDOW *win;
char c;

   win = window;
   if (win->ruler && win->visible) {
      c = win->ruler_line[win->ccol - win->start_col];
      c_output( c, win->ccol, win->top_line, g_display.ruler_color );
   }
}



void show_ruler_pointer( WINDOW *window )
{
   if (window->ruler && window->visible)
      c_output( RULER_PTR, window->ccol, window->top_line,
                g_display.ruler_pointer );
}



void show_all_rulers( void )
{
register WINDOW *wp;

   wp = g_status.window_list;
   while (wp != NULL) {
      make_ruler( wp );
      if (wp->visible) {
         show_ruler( wp );
         show_ruler_pointer( wp );
      }
      wp = wp->next;
   }
}
