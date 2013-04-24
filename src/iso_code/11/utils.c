

#include "tdestr.h"
#include "common.h"
#include "define.h"
#include "tdefunc.h"



int  myiswhitespc( int c )
{
   return( c == ' ' || (ispunct( c ) && c != '_') || iscntrl( c ) );
}



void check_virtual_col( WINDOW *window, int rcol, int ccol )
{
register int bcol;
int  start_col;
int  end_col;
file_infos *file;

   file      = window->file_info;
   bcol      = window->bcol;
   start_col = window->start_col;
   end_col   = window->end_col;

 
   if (ccol > end_col) {
 2;  */
      ccol = end_col;
      bcol = rcol - (ccol - start_col);
      file->dirty = LOCAL;
   }

  
   if (ccol < start_col) {
      if (bcol >= (start_col - ccol))
         bcol -= (start_col - ccol);
      ccol = start_col;
      file->dirty = LOCAL;
   }

  
   if (rcol < bcol) {
      ccol = rcol + start_col;
      bcol = 0;
      if (ccol > end_col) {
         bcol = rcol;
         ccol = start_col;
      }
      file->dirty = LOCAL;
   }

  
   if ((ccol - start_col) + bcol != rcol) {
      if (bcol < 0 || bcol > rcol) {
         bcol = rcol;
         file->dirty = LOCAL;
      }
      ccol = rcol - bcol + start_col;
      if (ccol > end_col) {
         bcol = rcol;
         ccol = start_col;
         file->dirty = LOCAL;
      }
   }

  
   if (rcol < 0) {
      rcol = bcol = 0;
      ccol = start_col;
      file->dirty = LOCAL;
   }

   if (rcol >= MAX_LINE_LENGTH) {
      rcol = MAX_LINE_LENGTH - 1;
      bcol = rcol - (ccol - start_col);
   }

   assert( rcol >= 0 );
   assert( rcol < MAX_LINE_LENGTH );
   assert( bcol >= 0 );
   assert( bcol < MAX_LINE_LENGTH );
   assert( ccol >= start_col );
   assert( ccol <= end_col );

   window->bcol = bcol;
   window->ccol = ccol;
   window->rcol = rcol;
}



void copy_line( line_list_ptr ll )
{
register unsigned int len;
text_ptr text_line;

   if (g_status.copied == FALSE  &&  ll->len != EOF) {

      assert( ll != NULL );

      len = ll->len;
      text_line = ll->line;
      g_status.buff_node = ll;

      assert( len < MAX_LINE_LENGTH );

      if (text_line != NULL)
         _fmemcpy( g_status.line_buff, text_line, len );

      g_status.line_buff_len = len;
      g_status.copied = TRUE;
   }
}



int  un_copy_line( line_list_ptr ll, WINDOW *window, int del_trailing )
{
text_ptr p;
size_t len;     
size_t ll_len;  
int  net_change;
int  rc;
char c;
file_infos *file;
WINDOW *wp;

   rc = OK;
   if (mode.do_backups == TRUE)
      rc = backup_file( window );

   if (g_status.copied == TRUE  &&  ll->len != EOF) {

      file = window->file_info;

      if (g_status.command == DeleteLine)
         del_trailing = FALSE;

      if (del_trailing  &&  mode.trailing  &&  file->crlf != BINARY) {
         len = g_status.line_buff_len;
         for (p=(text_ptr)(g_status.line_buff+len); len > 0; len--, p--) {
            c = *(p - 1);
            if (c != ' '  &&  c != '\t')
               break;
            if (!mode.inflate_tabs && c == '\t')
               break;
         }
         g_status.line_buff_len = len;
         file->dirty = GLOBAL;
         if (window->visible == TRUE)
            show_changed_line( window );
      }
      len = g_status.line_buff_len;
      ll_len =  (ll->line == NULL) ? 0 : ll->len;


      assert( len < MAX_LINE_LENGTH );
      assert( ll_len < MAX_LINE_LENGTH );

      net_change = len - ll_len;

      if (ll_len != len  ||  ll->line == NULL) {
         
         p = my_malloc( len, &rc );
         if (rc == ERROR)
            error( WARNING, window->bottom_line, main4 );

        
         if (rc != ERROR  &&  ll->line != NULL)
            my_free( ll->line );
      } else
         p = ll->line;

      if (rc != ERROR) {
         if (len > 0)
            _fmemcpy( p, g_status.line_buff, len );
         ll->line = p;
         ll->len = len;

         if (net_change != 0) {
            for (wp=g_status.window_list; wp != NULL; wp=wp->next) {
               if (wp->file_info == file && wp != window)
                  if (wp->rline > window->rline)
                     wp->bin_offset += net_change;
            }
         }

         file->modified = TRUE;
         show_avail_mem( );
      }
   }
   g_status.copied = FALSE;
   return( rc );
}



int  un_copy_tab_buffer( line_list_ptr ll, WINDOW *window )
{
text_ptr p;
int  len;               
int  net_change;
int  rc;
file_infos *file;
WINDOW *wp;

   rc = OK;
   file = window->file_info;
  
   if (mode.do_backups == TRUE) {
      window->file_info->modified = TRUE;
      rc = backup_file( window );
   }

   len = g_status.tabout_buff_len;

   assert( len >= 0 );
   assert( len < MAX_LINE_LENGTH );
   assert( ll->len >= 0 );
   assert( ll->len < MAX_LINE_LENGTH );

  
   p = my_malloc( len, &rc );
   if (rc == ERROR)
      error( WARNING, window->bottom_line, main4 );

   if (rc == OK) {
      net_change = len - ll->len;

      if (ll->line != NULL)
         my_free( ll->line );
      if (len > 0)
         _fmemcpy( p, g_status.line_buff, len );
      ll->line = p;
      ll->len  = len;

      if (net_change != 0) {
         for (wp=g_status.window_list; wp != NULL; wp=wp->next) {
            if (wp->file_info == file && wp != window)
               if (wp->rline > window->rline)
                  wp->bin_offset += net_change;
         }
      }

      file->modified = TRUE;
   }
   return( rc );
}



void load_undo_buffer( file_infos *file, text_ptr line_to_undo, int len )
{
int  rc;
text_ptr l;
line_list_ptr temp_ll;

   rc = OK;
   if (file->undo_count >= mode.undo_max) {
      --file->undo_count;
      temp_ll = file->undo_bot->prev;
      temp_ll->prev->next = file->undo_bot;
      file->undo_bot->prev = temp_ll->prev;
      if (temp_ll->line != NULL)
         my_free( temp_ll->line );
   } else
      temp_ll = (line_list_ptr)my_malloc( sizeof(line_list_struc), &rc );

   assert( len >= 0 );
   assert( len < MAX_LINE_LENGTH );

   l = my_malloc( len, &rc );

   if (rc == ERROR) {
      if (l != NULL)
         my_free( l );
      if (temp_ll != NULL)
         my_free( temp_ll );
   } else {
      if (len > 0)
         _fmemcpy( l, line_to_undo, len );
      temp_ll->line  = l;
      temp_ll->len   = len;
      temp_ll->dirty = TRUE;

      temp_ll->prev = NULL;
      temp_ll->next = file->undo_top;
      file->undo_top->prev = temp_ll;
      file->undo_top = temp_ll;

      ++file->undo_count;
   }
}



void set_prompt( char *prompt, int line )
{
register int prompt_col;

  
   prompt_col = strlen( prompt );

   assert( prompt_col <= MAX_COLS );

  
   s_output( prompt, line, 0, g_display.message_color );
   eol_clear( prompt_col, line, g_display.message_color );

  
   xygoto( prompt_col, line );
}



int  get_name( char *prompt, int line, char *name, int color )
{
int  col;               
int  c;                 
char *cp;               
char *answer;           
int first = TRUE;       
register int len;       
int  plen;              
int  func;              
int  stop;              
char *p;                
char buffer[MAX_COLS+2];
char line_buff[(MAX_COLS+1)*2]; 
int  normal;
int  local_macro = FALSE;
int  next;
int  regx_help_on;
char **pp;
int  i;

  
   assert( strlen( prompt ) < MAX_COLS );
   assert( strlen( name )   < MAX_COLS );

   strcpy( buffer, prompt );
   plen = strlen( prompt );
   answer = buffer + plen;
   strcpy( answer, name );

   regx_help_on = FALSE;
   len = strlen( answer );
   col = strlen( buffer );
   g_status.prompt_line = line;
   g_status.prompt_col = col;
   cp = answer + len;
   normal = g_display.text_color;
   save_screen_line( 0, line, line_buff );
   s_output( buffer, line, 0, color );
   eol_clear( col, line, normal );
   for (stop = FALSE; stop == FALSE;) {
      if (regx_help_on == TRUE)
         xygoto( -1, -1 );
      else
         xygoto( col, line );
      if (g_status.macro_executing) {
         next = g_status.macro_next;
         g_status.macro_next = macro.strokes[g_status.macro_next].next;
         if (g_status.macro_next != -1) {
            c = macro.strokes[g_status.macro_next].key;
            func = getfunc( c );
            if (func == PlayBack) {
               stop = TRUE;
               g_status.macro_next = next;
            }
         } else {
            c = 0x100;
            func = AbortCommand;
            stop = TRUE;
         }
      } else {
         if (local_macro == FALSE) {
            c = getkey( );
            func = getfunc( c );

           
            if (c == RTURN)
               func = Rturn;
            else if (c == ESC)
               func = AbortCommand;

            if (func == PlayBack) {
               local_macro = TRUE;
               next = macro.first_stroke[ c-256 ];
               c = macro.strokes[next].key;
               func = getfunc( c );
               next = macro.strokes[next].next;
            } else {
               g_status.key_pressed = c;
               record_keys( line );
            }
         } else {
            if (next != -1) {
               c = macro.strokes[next].key;
               next = macro.strokes[next].next;
            } else {
               local_macro = FALSE;
               c = 0x100;
            }
            func = getfunc( c );
         }
      }
      if (c == _F1)
         func = Help;
      if (regx_help_on == TRUE  &&  g_status.current_window != NULL) {
         redraw_screen( g_status.current_window );
         s_output( buffer, line, 0, color );
         eol_clear( col, line, normal );
         s_output( cp, line, col, color );
         regx_help_on = FALSE;
      } else {
         switch (func) {
            case Help :
               if ((g_status.command == FindRegX  ||
                    g_status.command == RepeatFindRegX) &&
                    regx_help_on == FALSE) {
                  regx_help_on = TRUE;
                  for (i=3,pp=regx_help; *pp != NULL; pp++, i++)
                     s_output( *pp, i, 12, g_display.help_color );
               }
               break;
            case ToggleSearchCase :
               mode.search_case = mode.search_case == IGNORE ? MATCH : IGNORE;
               build_boyer_array( );
               show_search_case( );
               break;
            case Rturn       :
            case NextLine    :
            case BegNextLine :
               answer[len] = '\0';
               assert( strlen( answer ) < MAX_COLS );
               strcpy( name, answer );
              
               stop = TRUE;
               break;
            case BackSpace :
             
               if (cp > answer) {
                  for (p=cp-1; p < answer+len; p++) {
                     *p = *(p+1);
                  }
                  --len;
                  --col;
                  --cp;
                  c_output( ' ', plen+len, line, normal );
                  s_output( cp, line, col, color );
                  *(answer + len) = '\0';
               }
               break;
            case DeleteChar :
            
               if (*cp) {
                  for (p=cp; p < answer+len; p++) {
                     *p = *(p+1);
                  }
                  --len;
                  c_output( ' ', plen+len, line, normal );
                  s_output( cp, line, col, color );
                  *(answer + len) = '\0';
               }
               break;
            case DeleteLine :
              
               col = plen;
               cp = answer;
               *cp = '\0';
               len = 0;
               eol_clear( col, line, normal );
               break;
            case AbortCommand :
               stop = TRUE;
               break;
            case CharLeft :
            
               if (cp > answer) {
                  col--;
                  cp--;
               }
               break;
            case CharRight :
              
               if (*cp) {
                  col++;
                  cp++;
                }
                break;
            case BegOfLine :
             
               col = plen;
               cp = answer;
               break;
            case EndOfLine :
             
               col = plen + len;
               cp = answer + len;
               break;
            default :
               if (c < 0x100) {
                
                  if (first) {
                    
                     col = plen;
                     cp = answer;
                     *cp = '\0';
                     len = 0;
                     eol_clear( col, line, normal );
                  }

                 
                  if (col < g_display.ncols-1) {
                     if (*cp == '\0') {
                        ++len;
                        *(answer + len) = '\0';
                     }
                     *cp = (char)c;
                     c_output( c, col, line, color );
                     ++cp;
                     ++col;
                  }
               }
               break;
         }
      }
      first = FALSE;
   }
   restore_screen_line( 0, line, line_buff );
   return( func == AbortCommand ? ERROR : OK );
}



int  get_sort_order( WINDOW *window )
{
register int c;
int  col;
char line_buff[(MAX_COLS+1)*2];         

   save_screen_line( 0, window->bottom_line, line_buff );
  
   s_output( utils4, window->bottom_line, 0, g_display.message_color );
   c = strlen( utils4 );
   eol_clear( c, window->bottom_line, g_display.text_color );
   xygoto( c, window->bottom_line );
   do {
      c = getkey( );
      col = getfunc( c );
      if (c == ESC)
         col = AbortCommand;
   } while (col != AbortCommand  &&  c != 'A'  &&  c != 'a'  &&
            c != 'D'  &&  c != 'd');
   switch ( c ) {
      case 'A' :
      case 'a' :
         sort.direction = ASCENDING;
         break;
      case 'D' :
      case 'd' :
         sort.direction = DESCENDING;
         break;
      default  :
         col = AbortCommand;
         break;
   }
   restore_screen_line( 0, window->bottom_line, line_buff );
   return( col == AbortCommand ? ERROR : OK );
}



int  get_replace_direction( WINDOW *window )
{
register int c;
int  col;
char line_buff[(MAX_COLS+1)*2];         

   save_screen_line( 0, window->bottom_line, line_buff );
 
   s_output( utils5, window->bottom_line, 0, g_display.message_color );
   c = strlen( utils5 );
   eol_clear( c, window->bottom_line, g_display.text_color );
   xygoto( c, window->bottom_line );
   do {
      c = getkey( );
      col = getfunc( c );
      if (c == ESC)
         col = AbortCommand;
   } while (col != AbortCommand  &&  c != 'F'  &&  c != 'f'  &&
            c != 'B'  &&  c != 'b');
   switch ( c ) {
      case 'F' :
      case 'f' :
         c = FORWARD;
         break;
      case 'B' :
      case 'b' :
         c = BACKWARD;
         break;
      default  :
         c = ERROR;
   }
   restore_screen_line( 0, window->bottom_line, line_buff );
   return( col == AbortCommand ? ERROR : c );
}



int  get_yn( void )
{
int  c;                 
register int rc;        

   do {
      c = getkey( );
      rc = getfunc( c );
      if (c== ESC)
         rc = AbortCommand;
   } while (rc != AbortCommand  &&  c != 'Y'  &&  c != 'y'  &&
            c != 'N'  &&  c != 'n');
   if (rc == AbortCommand || c == ESC)
      rc = ERROR;
   else {
      switch ( c ) {
         case 'Y' :
         case 'y' :
            rc = A_YES;
            break;
         case 'N' :
         case 'n' :
            rc = A_NO;
            break;
      }
   }
   return( rc );
}



int  get_lr( void )
{
int  c;                 
register int rc;        

   for (rc=OK; rc == OK;) {
      c = getkey( );
      if (getfunc( c ) == AbortCommand || c == ESC)
         rc = ERROR;
      else {
         switch ( c ) {
            case 'L' :
            case 'l' :
               rc = LEFT;
               break;
            case 'R' :
            case 'r' :
               rc = RIGHT;
               break;
         }
      }
   }
   return( rc );
}



int  get_bc( void )
{
int  c;                 
register int rc;        

   for (rc=OK; rc == OK;) {
      c = getkey( );
      if (getfunc( c ) == AbortCommand || c == ESC)
         rc = ERROR;
      else {
         switch ( c ) {
            case 'B' :
            case 'b' :
               rc = BEGINNING;
               break;
            case 'C' :
            case 'c' :
               rc = CURRENT;
               break;
         }
      }
   }
   return( rc );
}



int  get_oa( void )
{
int  c;                 
register int rc;        
int  func;

   rc = 0;
   while (rc != AbortCommand && rc != A_OVERWRITE && rc != A_APPEND) {
      c = getkey( );
      func = getfunc( c );
      if (func == AbortCommand || c == ESC)
         rc = AbortCommand;
      switch ( c ) {
         case 'O' :
         case 'o' :
            rc = A_OVERWRITE;
            break;
         case 'A' :
         case 'a' :
            rc = A_APPEND;
            break;
      }
   }
   return( rc );
}


void show_eof( WINDOW *window )
{
register int color;
char temp[MAX_COLS+2];

   assert( strlen( mode.eof ) < MAX_COLS );

   strcpy( temp, mode.eof );
   color = window->end_col + 1 - window->start_col;
   if (strlen( temp ) > (unsigned)color)
      temp[color] = '\0';
   color = g_display.eof_color;
   window_eol_clear( window, color );
   s_output( temp, window->cline, window->start_col, color );
}



void display_current_window( WINDOW *window )
{
int  count;     
int  number;    
register int i; 
WINDOW w;       
int  curl;      
int  eof;

  
   number = window->bottom_line - ((window->top_line + window->ruler) - 1);
   count  = window->cline - (window->top_line + window->ruler);
   dup_window_info( &w, window );

   w.cline -= count;
   w.rline -= count;
   for (eof=count; eof > 0; eof--)
      w.ll = w.ll->prev;


   
   eof = FALSE;
   curl = window->cline;
   for (i=number; i>0; i--) {
      if (w.ll->len != EOF) {
       
         if (w.cline != curl)
            update_line( &w );
         w.ll = w.ll->next;
      } else if (eof == FALSE) {
         show_eof( &w );
         eof = TRUE;
      } else
         window_eol_clear( &w, COLOR_TEXT );
      ++w.cline;
      ++w.rline;
   }
   show_asterisk( window );
   show_curl_line( window );
}



int  redraw_screen( WINDOW *window )
{
register WINDOW *above;        
register WINDOW *below;        

   cls( );
  
   redraw_current_window( window );

  
   above = below = window;
   while (above->prev || below->next) {
      if (above->prev) {
         above = above->prev;
         redraw_current_window( above );
      }
      if (below->next) {
         below = below->next;
         redraw_current_window( below );
      }
   }
   window->file_info->dirty = FALSE;
   show_modes( );
   return( OK );
}



void redraw_current_window( WINDOW *window )
{

  
   if (window->visible) {
      display_current_window( window );
      show_window_header( window );
      show_ruler( window );
      show_ruler_pointer( window );
      if (window->vertical)
         show_vertical_separator( window );
   }
}



void show_changed_line( WINDOW *window )
{
WINDOW *above;                  
WINDOW *below;                  
WINDOW w;                       
long changed_line;              
long top_line, bottom_line;     
int  line_on_screen;            
file_infos *file;               

   file = window->file_info;
   if ((file->dirty == LOCAL || file->dirty == GLOBAL) && window->visible)
      show_curl_line( window );
   changed_line = window->rline;

  
   if (file->dirty != LOCAL) {
      above = below = window;
      while (above->prev || below->next) {
         if (above->prev) {
            above = above->prev;
            dup_window_info( &w, above );
         } else if (below->next) {
            below = below->next;
            dup_window_info( &w, below );
         }

        
         if (w.file_info == file && w.visible) {

           
            line_on_screen = FALSE;
            top_line = w.rline - (w.cline - (w.top_line + w.ruler));
            bottom_line = w.rline + (w.bottom_line - w.cline);
            if (changed_line == w.rline)
               line_on_screen = CURLINE;
            else if (changed_line < w.rline && changed_line >= top_line) {
               line_on_screen = NOTCURLINE;
               while (w.rline > changed_line) {
                  w.ll = w.ll->prev;
                  --w.rline;
                  --w.cline;
               }
            } else if (changed_line > w.rline && changed_line <= bottom_line) {
               line_on_screen = NOTCURLINE;
               while (w.rline < changed_line) {
                  w.ll = w.ll->next;
                  ++w.rline;
                  ++w.cline;
               }
            }

          
            if (line_on_screen == NOTCURLINE)
               update_line( &w );
            else if (line_on_screen == CURLINE)
               show_curl_line( &w );
         }
      }
   }
   file->dirty = FALSE;
}



void show_curl_line( WINDOW *window )
{
int  text_color;
int  dirty_color;

   if (window->visible  &&  g_status.screen_display) {
      text_color = g_display.text_color;
      dirty_color = g_display.dirty_color;
      g_display.dirty_color = g_display.text_color = g_display.curl_color;
      update_line( window );
      g_display.text_color = text_color;
      g_display.dirty_color = dirty_color;
   }
}



void dup_window_info( WINDOW *dw, WINDOW *sw )
{
   memcpy( dw, sw, sizeof( WINDOW ) );
}



void adjust_windows_cursor( WINDOW *window, long line_change )
{
register WINDOW *next;
long i;
file_infos *file;
MARKER *marker;
long length;

   file = window->file_info;
   length = file->length;
   next = g_status.window_list;
   while (next != NULL) {
      if (next != window) {
         if (next->file_info == file) {
            if (next->rline > length + 1) {
               next->rline = length;
               next->ll    = file->line_list_end;
               file->dirty = NOT_LOCAL;
            } else if (next->rline < 1) {
               next->rline = 1;
               next->cline = next->top_line + next->ruler;
               next->ll    = file->line_list;
               next->bin_offset = 0;
               file->dirty = NOT_LOCAL;
            }
            if (next->rline > window->rline  &&  line_change) {
               file->dirty = NOT_LOCAL;
               if (line_change < 0) {
                  for (i=line_change; i < 0 && next->ll->next != NULL; i++) {
                     next->bin_offset += next->ll->len;
                     next->ll = next->ll->next;
                  }
               } else if (line_change > 0) {
                  for (i=line_change; i > 0 && next->ll->prev != NULL; i--) {
                     next->ll = next->ll->prev;
                     next->bin_offset -= next->ll->len;
                  }
               }
            }
            if (next->rline < (next->cline -(next->top_line+next->ruler-1))) {
               next->cline = (int)next->rline+(next->top_line+next->ruler)-1;
               file->dirty = NOT_LOCAL;
            }
         }
      }
      next = next->next;
   }

   
   for (i=0; i<3; i++) {
      marker = &file->marker[ (int) i ];
      if (marker->rline > window->rline) {
         marker->rline += line_change;
         if (marker->rline < 1L)
            marker->rline = 1L;
         else if (marker->rline > length)
            marker->rline = length;
      }
   }
}



int  first_non_blank( text_ptr s, int len )
{
register int count = 0;

   if (s != NULL) {
      if (mode.inflate_tabs) {
         for (; len > 0 && (*s == ' ' || *s == '\t'); s++, len--) {
            if (*s != '\t')
               ++count;
            else
               count += mode.ptab_size - (count % mode.ptab_size);
         }
      } else {
         while (len-- > 0  &&  *s++ == ' ')
           ++count;
      }
   }
   return( count );
}


int  find_end( text_ptr s, int len )
{
register int count = 0;

   if (s != NULL) {
      if (mode.inflate_tabs) {
         for (;len > 0; s++, len--) {
            if (*s == '\t')
               count += mode.ptab_size - (count % mode.ptab_size);
            else
               ++count;
         }
      } else
         count = len;
   }
   return( count );
}



int is_line_blank( text_ptr s, int len )
{
   if (s != NULL) {
      if (mode.inflate_tabs) {
        while (len > 0  &&  (*s == ' ' || *s == '\t')) {
           ++s;
           --len;
        }
      } else {
         while (len > 0  &&  *s == ' ') {
            ++s;
            --len;
         }
      }
   } else
      len = 0;
   return( len == 0 );
}



int  page_up( WINDOW *window )
{
int  i;                 
int  rc = OK;           
register WINDOW *win;   
long number;
long len;

   win = window;
   entab_linebuff( );
   if (un_copy_line( win->ll, win, TRUE ) == ERROR)
      return( ERROR );
   if (win->rline != (win->cline - (win->top_line + win->ruler - 1))) {
      i = win->cline - (win->top_line + win->ruler - 1);
      number = win->rline;
      if (( win->rline - i) < win->page)
         win->rline = (win->cline-(win->top_line + win->ruler -1)) + win->page;
      win->rline -= win->page;
      for (len =0, i=(int)(number - win->rline); i>0; i--)
         if (win->ll->prev != NULL) {
            win->ll = win->ll->prev;
            len -= win->ll->len;
         }
      win->file_info->dirty = LOCAL;
      win->bin_offset += len;
   } else
      rc = ERROR;
   sync( win );
   return( rc );
}



int  page_down( WINDOW *window )
{
int  i;                 
int  k;
int  rc = OK;
long len;
register WINDOW *win;  
line_list_ptr p;

   win = window;
   entab_linebuff( );
   if (un_copy_line( win->ll, win, TRUE ) == ERROR)
      return( ERROR );
   p = win->ll;
   k = win->cline - (win->top_line + win->ruler);
   for (len=i=0; i < win->page && p->next != NULL; i++, k++, p=p->next)
      if (p->len != EOF)
         len += p->len;
   if (k >= win->page) {
      win->rline += i;
      win->cline = win->cline + i - win->page;
      win->bin_offset += len;
      win->ll = p;
      win->file_info->dirty = LOCAL;
   } else
      rc = ERROR;
   sync( win );
   return( rc );
}



int  scroll_down( WINDOW *window )
{
int  rc = OK;
register WINDOW *win;   

   win = window;
   entab_linebuff( );
   if (un_copy_line( win->ll, win, TRUE ) == ERROR)
      return( ERROR );
   if (win->cline == win->top_line + win->ruler) {
      if (win->ll->next != NULL) {
         ++win->rline;
         win->bin_offset += win->ll->len;
         win->ll = win->ll->next;
         win->file_info->dirty = LOCAL;
      } else
         rc = ERROR;
   } else {
      --win->cline;
      win->file_info->dirty = LOCAL;
   }
   sync( win );
   return( rc );
}



int  scroll_up( WINDOW *window )
{
int  rc = OK;
register WINDOW *win;   

   win = window;
   entab_linebuff( );
   if (un_copy_line( win->ll, win, TRUE ) == ERROR)
      return( ERROR );
   if (win->rline > 1) {
      if (win->rline == (win->cline - (win->top_line + win->ruler - 1))) {
         if (!mode.sync)
            update_line( win );
         win->ll = win->ll->prev;
         win->bin_offset -= win->ll->len;
         --win->rline;
         --win->cline;
         if (!mode.sync)
            show_curl_line( win );
      } else {
         if (win->cline == win->bottom_line) {
            --win->rline;
            win->ll = win->ll->prev;
            win->bin_offset -= win->ll->len;
            win->file_info->dirty = LOCAL;
         } else {
            ++win->cline;
            win->file_info->dirty = LOCAL;
         }
      }
   } else
     rc = ERROR;
   sync( win );
   return( rc );
}



int  pan_up( WINDOW *window )
{
int  rc = OK;
register WINDOW *win;   

   win = window;
   entab_linebuff( );
   if (un_copy_line( win->ll, win, TRUE ) == ERROR)
      return( ERROR );

   
   if (win->rline != (win->cline+1 - (win->top_line + win->ruler))) {
      if (win->rline > 1) {
         --win->rline;
         win->ll = win->ll->prev;
         win->bin_offset -= win->ll->len;
         win->file_info->dirty = LOCAL;
      }
   } else
      rc = ERROR;
   sync( win );
   return( rc );
}



int  pan_down( WINDOW *window )
{
int  rc = OK;
register WINDOW *win;   

   win = window;
   entab_linebuff( );
   if (un_copy_line( win->ll, win, TRUE ) == ERROR)
      return( ERROR );
   if (win->ll->len != EOF) {
      ++win->rline;
      win->bin_offset += win->ll->len;
      win->ll = win->ll->next;
      win->file_info->dirty = LOCAL;
   } else
      rc = ERROR;
   sync( win );
   return( rc );
}



void show_window_header( WINDOW *window )
{
char status_line[MAX_COLS+2];   
register WINDOW *win;           
int  len;

   win = window;
   len = win->vertical ? win->end_col + 1 - win->start_col : win->end_col;

   assert( len >= 0 );
   assert( len <= MAX_COLS );

   memset( status_line, ' ', len );
   status_line[len] = '\0';
   s_output( status_line, win->top_line-1, win->start_col,g_display.head_color);
   show_window_number_letter( win );
   show_window_fname( win );
   show_crlf_mode( win );
   show_size( win );
   show_line_col( win );
}


void show_window_number_letter( WINDOW *window )
{
int  col;
char temp[10];
register WINDOW *win;   

   win = window;
   col = win->start_col;
   s_output( "   ", win->top_line-1, col, g_display.head_color );
   itoa( win->file_info->file_no, temp, 10 );
   s_output( temp, win->top_line-1, strlen( temp ) > 1 ? col : col+1,
             g_display.head_color );
   c_output( win->letter, col+2, win->top_line-1, g_display.head_color );
}



void show_window_fname( WINDOW *window )
{
char status_line[MAX_COLS+2];   
register int  fattr;
char *p;
register WINDOW *win;          
int  col;
int  len;

   win = window;
   col = win->start_col;
   len = win->vertical ? 11 : FNAME_LENGTH;

   assert( len >= 0 );
   assert( len <= MAX_COLS );

   memset( status_line, ' ', len );
   status_line[len] = '\0';
   s_output( status_line, win->top_line-1, col+5, g_display.head_color );

   assert( strlen( win->file_info->file_name ) < MAX_COLS );

   strcpy( status_line, win->file_info->file_name );
   p = status_line;
   if (win->vertical) {
      len = strlen( status_line );
      for (p=status_line+len;*(p-1) != ':' && *(p-1) != '\\' && p>status_line;)
         --p;
   } else {
      status_line[FNAME_LENGTH] = '\0';
      p = status_line;
   }
   s_output( p, win->top_line-1, col+5, g_display.head_color );
   if (!win->vertical) {
      fattr = win->file_info->file_attrib;
      p = status_line;
      *p++ = (char)(fattr & ARCHIVE   ? 'A' : '-');
      *p++ = (char)(fattr & SYSTEM    ? 'S' : '-');
      *p++ = (char)(fattr & HIDDEN    ? 'H' : '-');
      *p++ = (char)(fattr & READ_ONLY ? 'R' : '-');
      *p   = '\0';
      s_output( status_line, win->top_line-1, col+51, g_display.head_color );
   }
}



void show_crlf_mode( WINDOW *window )
{
char status_line[MAX_COLS+2];   

   if (!window->vertical) {
      switch (window->file_info->crlf) {
         case LF :
            strcpy( status_line, "lf  " );
            break;
         case CRLF :
            strcpy( status_line, "crlf" );
            break;
         case BINARY :
            strcpy( status_line, "BIN " );
            break;
         default :
            assert( FALSE );
      }
      s_output( status_line, window->top_line-1, window->start_col+56,
                g_display.head_color );
   }
}



void show_size( WINDOW *window )
{
char csize[20];

   if (!window->vertical  &&  window->file_info->crlf != BINARY) {
      s_output( "       ", window->top_line-1, 61, g_display.head_color );
      ltoa( window->file_info->length, csize, 10 );
      s_output( csize, window->top_line-1, 61, g_display.head_color );
   }
}



int  quit( WINDOW *window )
{
int  prompt_line;
char line_buff[(MAX_COLS+2)*2]; 
register file_infos *file;
WINDOW *wp;
int  count = 0;
int  rc = OK;

   entab_linebuff( );
   if (un_copy_line( window->ll, window, TRUE ) == ERROR)
      return( ERROR );
   prompt_line = window->bottom_line;
   file = window->file_info;
   for (wp=g_status.window_list; wp != NULL; wp=wp->next) {
      if (wp->file_info == file && wp->visible)
         ++count;
   }
   if (file->modified && count == 1) {
      save_screen_line( 0, prompt_line, line_buff );
      /*
       * abandon changes (y/n)
       */
      set_prompt( utils12, prompt_line );
      if (get_yn( ) != A_YES)
         rc = ERROR;
      restore_screen_line( 0, prompt_line, line_buff );
   }

   
   if (rc == OK)
      finish( window );
   return( OK );
}


int  move_up( WINDOW *window )
{
int  rc = OK;
register WINDOW *win;   
int  at_top = FALSE;    

   win = window;
   entab_linebuff( );
   if (un_copy_line( win->ll, win, TRUE ) == ERROR)
      return( ERROR );

   
   if (win->rline > 1) {
      if (win->cline == win->top_line + win->ruler) {
         win->file_info->dirty = LOCAL;
         at_top = TRUE;
      }
      if (!at_top)
         update_line( win );
      --win->rline;             
      win->ll = win->ll->prev;
      win->bin_offset -= win->ll->len;
      if (!at_top) {
         --win->cline;          
         show_curl_line( win );
      }
   } else
      rc = ERROR;
   sync( win );
   return( rc );
}



int  move_down( WINDOW *window )
{
int  rc;

   rc = prepare_move_down( window );
   sync( window );
   return( rc );
}


int  prepare_move_down( WINDOW *window )
{
int  rc = OK;
register WINDOW *win;   
int  at_bottom = FALSE; 

   win = window;
   entab_linebuff( );
   if (un_copy_line( win->ll, win, TRUE ) == ERROR)
      return( ERROR );
   if (win->cline == win->bottom_line) {
      win->file_info->dirty = LOCAL;
      at_bottom = TRUE;
   }
   if (!at_bottom)
      update_line( win );
   if (win->ll->len != EOF) {
      win->bin_offset += win->ll->len;
      ++win->rline;             
      win->ll = win->ll->next;
      if (!at_bottom) {
         ++win->cline;          
         show_curl_line( win );
      }
   } else if (win->cline > win->top_line + win->ruler) {
      --win->cline;
      win->file_info->dirty = LOCAL;
      rc = ERROR;
   } else
      rc = ERROR;
   return( rc );
}



int  move_left( WINDOW *window )
{
int  new_ruler = FALSE;

   if (window->ccol > window->start_col) {
      show_ruler_char( window );
      --window->ccol;
      --window->rcol;
   } else if (window->ccol == window->start_col && window->rcol > 0) {
      --window->rcol;
      --window->bcol;
      window->file_info->dirty = LOCAL;
      new_ruler = TRUE;
   }
   sync( window );
   if (new_ruler) {
      make_ruler( window );
      show_ruler( window );
   }
   return( OK );
}



int  move_right( WINDOW *window )
{
int  new_ruler = FALSE;

   if (window->rcol < g_display.line_length - 1) {
      if (window->ccol < window->end_col) {
         show_ruler_char( window );
         ++window->ccol;
         ++window->rcol;
      } else if (window->ccol == window->end_col) {
         ++window->rcol;
         ++window->bcol;
         window->file_info->dirty = LOCAL;
         new_ruler = TRUE;
      }
   }
   sync( window );
   if (new_ruler) {
      make_ruler( window );
      show_ruler( window );
   }
   return( OK );
}



int  pan_left( WINDOW *window )
{

   if (window->bcol > 0 ) {
      --window->bcol;
      --window->rcol;
      window->file_info->dirty = LOCAL;
      make_ruler( window );
      show_ruler( window );
   }
   sync( window );
   return( OK );
}



int  pan_right( WINDOW *window )
{
   if (window->rcol < g_display.line_length - 1) {

      ++window->rcol;
      ++window->bcol;
      window->file_info->dirty = LOCAL;
      make_ruler( window );
      show_ruler( window );
   }
   sync( window );
   return( OK );
}



int  word_left( WINDOW *window )
{
text_ptr p;             
int  len;               
int  rc;
register int rcol;
long rline;
line_list_ptr ll;
WINDOW w;

   entab_linebuff( );
   if (un_copy_line( window->ll, window, TRUE ) == ERROR)
      return( ERROR );
   rc = OK;
   dup_window_info( &w, window );
   rline = window->rline;
   rcol  = window->rcol;
   ll = window->ll;
   if (ll->len != EOF) {
      p = ll->line;
      len = ll->len;

      if (p != NULL  &&  rcol > 0  &&  rcol >= len  &&
                                      !myiswhitespc( *(p + len - 1) )) {
         rcol = len - 1;
         p += rcol;
         for (; rcol >= 0 && !myiswhitespc( *p ); rcol--, p--);
         ++rcol;
         check_virtual_col( window, rcol, rcol );
         make_ruler( window );
         show_ruler( window );
      } else {
         rcol = rcol >= len ? len-1 : rcol;
         if (rcol >= 0)
            p += rcol;
         if (p != NULL  &&  rcol > 0  &&  !myiswhitespc( *p )  &&
                                          !myiswhitespc( *(p-1) )) {
            for (; rcol >= 0 && !myiswhitespc( *p ); rcol--, p--);
            ++rcol;
            check_virtual_col( window, rcol, rcol );
            make_ruler( window );
            show_ruler( window );
         } else {

          
            if (p != NULL)
               for (; rcol >= 0 && !myiswhitespc( *p ); rcol--, p--);

           
            if (rcol < 0) {
               if (ll->prev != NULL) {
                  --rline;
                  ll = ll->prev;
                  p = ll->line;
                  rcol = ll->len - 1;
                  if (rcol >= 0)
                     p += rcol;
               } else
                  rc = ERROR;
            }

            
            while (rc == OK  &&  (p == NULL  ||  (p != NULL  &&
                                                  myiswhitespc( *p )))) {
               for (; rcol >= 0 && myiswhitespc( *p ); rcol--, p--);
               if (rcol < 0) {
                  if (ll->prev != NULL) {
                     --rline;
                     ll = ll->prev;
                     p = ll->line;
                     rcol = ll->len - 1;
                     if (rcol >= 0)
                        p += rcol;
                  } else
                     rc = ERROR;
               } else
                  break;
            }

         
            if (rc == OK  &&  p != NULL) {
               for (; rcol >= 0 && !myiswhitespc( *p ); rcol--, p--);
               bin_offset_adjust( window, rline );
               find_adjust( window, ll, rline, rcol+1 );
               if (rline != w.rline && !window->file_info->dirty) {
                  update_line( &w );
                  show_curl_line( window );
               }
               make_ruler( window );
               show_ruler( window );
            } else
               rc = ERROR;
         }
      }
   } else
      rc = ERROR;

   sync( window );
   return( rc );
}



int  word_right( WINDOW *window )
{
int  len;               
text_ptr p;             
int  rc;
WINDOW w;
register int rcol;
line_list_ptr ll;
long rline;

   entab_linebuff( );
   if (un_copy_line( window->ll, window, TRUE ) == ERROR)
      return( ERROR );
   rc = OK;
   dup_window_info( &w, window );
   rline = window->rline;
   rcol  = window->rcol;
   ll = window->ll;
   if (ll->len != EOF) {
      p = ll->line;
      len = ll->len;

     
      rcol = rcol >= len ? len-1 : rcol;
      if (rcol >= 0)
         p += rcol;

     
      if (p != NULL)
         for (; rcol < len && !myiswhitespc( *p ); rcol++, p++);
      else
         rcol = len;

      
      if (rcol == len) {
         ++rline;
         ll = ll->next;
         if (ll->len != EOF) {
            p = ll->line;
            len = ll->len;
            rcol = 0;
         } else
            rc = ERROR;
      }

     
      while (rc == OK && (p == NULL  ||  (p != NULL && myiswhitespc( *p )))) {
         for (; rcol < len && myiswhitespc( *p ); rcol++, p++);
         if (rcol == len) {
            ++rline;
            ll = ll->next;
            if (ll->len != EOF) {
               p = ll->line;
               len = ll->len;
               rcol = 0;
            } else
               rc = ERROR;
         } else
            break;
      }
   } else
      rc = ERROR;

   if (rc == OK) {
      bin_offset_adjust( window, rline );
      find_adjust( window, ll, rline, rcol );
      make_ruler( window );
      show_ruler( window );
   }

   if (rline != w.rline && !window->file_info->dirty) {
      update_line( &w );
      show_curl_line( window );
   }
   sync( window );
   return( rc );
}



int  next_dirty_line( WINDOW *window )
{
int  rc;
line_list_ptr ll;
long rline;
long bin_offset;       
WINDOW w;

   entab_linebuff( );
   if (un_copy_line( window->ll, window, TRUE ) == ERROR)
      return( ERROR );
   rc = OK;
   dup_window_info( &w, window );
   rline = window->rline;
   ll = window->ll;
   bin_offset = window->bin_offset;
   if (ll->len != EOF) {
      while (rc == OK) {
         if (ll->len != EOF) {
            ++rline;
            bin_offset += ll->len;
            ll = ll->next;
            if (ll->dirty == TRUE)
               break;
         } else
            rc = ERROR;
      }
   } else
      rc = ERROR;

   if (rc == OK) {
      window->bin_offset = bin_offset;
      find_adjust( window, ll, rline, window->rcol );
      make_ruler( window );
      show_ruler( window );
   } else
      error( WARNING, window->bottom_line, utils16 );

   if (rline != w.rline && !window->file_info->dirty) {
      update_line( &w );
      show_curl_line( window );
   }
   sync( window );
   return( rc );
}



int  prev_dirty_line( WINDOW *window )
{
int  rc;
line_list_ptr ll;
long rline;
long bin_offset;        
WINDOW w;

   entab_linebuff( );
   if (un_copy_line( window->ll, window, TRUE ) == ERROR)
      return( ERROR );
   rc = OK;
   dup_window_info( &w, window );
   rline = window->rline;
   ll = window->ll;
   bin_offset = window->bin_offset;
   if (ll->prev != NULL) {
      while (rc == OK) {
         if (ll->prev != NULL) {
            --rline;
            ll = ll->prev;
            bin_offset -= ll->len;
            if (ll->dirty == TRUE)
               break;
         } else
            rc = ERROR;
      }
   } else
      rc = ERROR;

   if (rc == OK) {
      window->bin_offset = bin_offset;
      find_adjust( window, ll, rline, window->rcol );
      make_ruler( window );
      show_ruler( window );
   } else
      error( WARNING, window->bottom_line, utils16 );

   if (rline != w.rline && !window->file_info->dirty) {
      update_line( &w );
      show_curl_line( window );
   }
   sync( window );
   return( rc );
}



int  center_window( WINDOW *window )
{
int  center;
int  center_line;
int  diff;
register file_infos *file;
register WINDOW *win;           

   win = window;
   file = win->file_info;
   center = (win->bottom_line + 1 - win->top_line) / 2 - win->ruler;
   center_line = win->top_line + win->ruler + center;
   diff = center_line - win->cline;
   entab_linebuff( );
   if (un_copy_line( win->ll, win, TRUE ) == ERROR)
      return( ERROR );
   if (g_status.command == CenterWindow) {
      if (diff > 0) {
         if (win->rline + diff <= file->length) {
            update_line( win );
            win->cline += diff;
            win->rline += diff;
            for (; diff > 0; diff--) {
               win->bin_offset += win->ll->len;
               win->ll = win->ll->next;
            }
            show_curl_line( win );
         }
      } else if (diff < 0) {
         update_line( win );
         win->cline += diff;
         win->rline += diff;
         for (; diff < 0; diff++) {
            win->ll = win->ll->prev;
            win->bin_offset -= win->ll->len;
         }
         show_curl_line( win );
      }
   } else {
      if (diff > 0) {
         win->cline += diff;
         if ((long)(win->cline+1 - (win->top_line + win->ruler)) > win->rline)
            win->cline = (win->top_line + win->ruler) - 1 + (int)win->rline;
         file->dirty = LOCAL;
      } else if (diff < 0) {
         win->cline = win->cline + diff;
         file->dirty = LOCAL;
      }
   }
   if (g_status.command == CenterWindow  ||  g_status.command == CenterLine)
      sync( win );
   return( OK );
}



int  horizontal_screen_right( WINDOW *window )
{
int  col;

   col = window->rcol;
   col += (window->end_col + 1 - window->start_col);
   if (col < MAX_LINE_LENGTH) {
      window->rcol = col;
      window->bcol += (window->end_col + 1 - window->start_col);
      window->file_info->dirty = LOCAL;
      check_virtual_col( window, window->rcol, window->ccol );
      make_ruler( window );
      show_ruler( window );
   }
   sync( window );
   return( OK );
}



int  horizontal_screen_left( WINDOW *window )
{
int  screen_width;

   screen_width = window->end_col + 1 - window->start_col;
   if (window->rcol - screen_width < 0) {
      if (window->bcol != 0) {
         window->bcol = 0;
         window->file_info->dirty = LOCAL;
      }
   } else {
      window->rcol -= screen_width;
      window->bcol -= screen_width;
      if (window->bcol < 0)
         window->bcol = 0;
      window->file_info->dirty = LOCAL;
   }
   check_virtual_col( window, window->rcol, window->ccol );
   sync( window );
   make_ruler( window );
   show_ruler( window );
   return( OK );
}



int  goto_top_file( WINDOW *window )
{
register WINDOW *win;   
long num;

   win = window;
   entab_linebuff( );
   if (un_copy_line( win->ll, win, TRUE ) == ERROR)
      return( ERROR );
   if (win->rline != win->cline - (win->top_line+win->ruler-1)) {
      win->bin_offset = 0;
      win->rline = win->cline - (win->top_line+win->ruler-1);
      win->ll = win->file_info->line_list;
      for (num=1; num < win->rline; num++) {
         win->bin_offset += win->ll->len;
         win->ll = win->ll->next;
      }
      display_current_window( win );
   }
   sync( win );
   return( OK );
}



int  goto_end_file( WINDOW *window )
{
register WINDOW *win;  
line_list_ptr ll;
long length;

   win = window;
   entab_linebuff( );
   if (un_copy_line( win->ll, win, TRUE ) == ERROR)
      return( ERROR );
   length = win->file_info->length;
   if (length > win->rline + win->bottom_line - win->cline) {
      win->rline = length - (win->bottom_line - win->cline) + 1;
      win->ll = win->file_info->line_list_end;
      for (;length >= win->rline; length--)
         win->ll = win->ll->prev;

      win->bin_offset = 0;
      ll = win->file_info->line_list;
      for (length = 1; length < win->rline; length++) {
         win->bin_offset += ll->len;
         ll = ll->next;
      }

      display_current_window( win );
   }
   sync( win );
   return( OK );
}



int  goto_line( WINDOW *window )
{
long number;            
long n;
char num_str[MAX_COLS]; 
register WINDOW *win;   
line_list_ptr ll;
int  rc;

   win = window;
   entab_linebuff( );
   if (un_copy_line( win->ll, win, TRUE ) == ERROR)
      return( ERROR );
  
   num_str[0] = '\0';
   
   if (get_name( find11, win->bottom_line, num_str,
                 g_display.message_color ) != OK  ||  *num_str == '\0')
      return( ERROR );
   number = atol( num_str );

   if (number > 0  && number <= (long)win->file_info->length) {
      update_line( win );
      ll = win->ll;
      n = win->rline;
      if (number < win->rline) {
         if (n - number < number - 1) {
            for (; n > number; n--) {
               ll = ll->prev;
               win->bin_offset -= ll->len;
            }
         } else {
            ll = win->file_info->line_list;
            n = 1;
            for (; n < number; n++) {
               win->bin_offset += ll->len;
               ll = ll->next;
            }
         }
      } else if (number > win->rline) {
         for (; n < number; n++) {
            win->bin_offset += ll->len;
            ll = ll->next;
         }
      }
      find_adjust( win, ll, number, win->rcol );
      if (!win->file_info->dirty)
         show_curl_line( win );
      rc = OK;
   } else {
      
      strcat( num_str, find12 );
      ltoa( win->file_info->length, num_str+25, 10 );
      error( WARNING, win->bottom_line, num_str );
      rc = ERROR;
   }
   return( rc );
}



int  set_marker( WINDOW *window )
{
register MARKER  *marker;       

   marker = &window->file_info->marker[g_status.command - SetMark1];
   marker->rline  = window->rline;
   marker->rcol   = window->rcol;
   marker->ccol   = window->ccol;
   marker->bcol   = window->bcol;
   marker->marked = TRUE;
   return( OK );
}



int  goto_marker( WINDOW *window )
{
int  m;
file_infos *file;
long new_rline;
long n;
MARKER *marker;
register WINDOW *win;   
line_list_ptr ll;
int  rc;

   win = window;
   m = g_status.command - GotoMark1;
   file = win->file_info;
   marker = &file->marker[m];
   if (marker->marked) {
      entab_linebuff( );
      if (un_copy_line( win->ll, win, TRUE ) == ERROR)
         return( ERROR );
      file->dirty = LOCAL;
      if (marker->rline > file->length)
         marker->rline = file->length;
      if (marker->rline < 1l)
         marker->rline = 1l;
      new_rline = marker->rline;
      ll = win->ll;
      if (new_rline < win->rline) {
         if (win->rline - new_rline < new_rline - 1) {
            for (n=win->rline; n > new_rline; n--) {
               ll = ll->prev;
               win->bin_offset -= ll->len;
            }
         } else {
            ll = win->file_info->line_list;
            win->bin_offset = 0;
            n = 1;
            for (; n < new_rline; n++) {
               win->bin_offset += ll->len;
               ll = ll->next;
            }
         }
      } else if (new_rline > win->rline) {
         n = win->rline;
         for (; n < new_rline; n++) {
            win->bin_offset += ll->len;
            ll = ll->next;
         }
      }
      win->rline  = new_rline;
      win->ll     = ll;
      win->rcol   = marker->rcol;
      win->ccol   = marker->ccol;
      win->bcol   = marker->bcol;
      if (win->rline < (win->cline - ((win->top_line + win->ruler) - 1)))
         win->cline = (int)win->rline + (win->top_line + win->ruler) - 1;
      check_virtual_col( win, win->rcol, win->ccol );
      make_ruler( window );
      show_ruler( window );
      rc = OK;
   } else {
      if (m == 9)
         m = -1;
      *(utils13 + 7) = (char)('0' + m + 1);
    
      error( WARNING, win->bottom_line, utils13 );
      rc = ERROR;
   }
   return( rc );
}



int  date_time_stamp( WINDOW *window )
{
char date_time[MAX_COLS];
char stuff[20];
register char *dt;
int  year, month, day;
int  hours, minutes;
int  one, two, three;
int  i;
int  pm;


   get_date( &year, &month, &day, &i );
   get_time( &hours, &minutes, &i, &i );
   dt = date_time;

 
   switch (mode.date_style) {
      case MM_DD_YY  :
      case DD_MM_YY  :
      case YY_MM_DD  :
         year = year % 100;
   }

   switch (mode.date_style) {
      case DD_MM_YY   :
      case DD_MM_YYYY :
         one = day;
         two = month;
         three = year;
         break;
      case YY_MM_DD   :
      case YYYY_MM_DD :
         one = year;
         two = month;
         three = day;
         break;
      case MM_DD_YY   :
      case MM_DD_YYYY :
      default         :
         one = month;
         two = day;
         three = year;
         break;
   }
   strcpy( dt, itoa( one, stuff, 10 ) );
   strcat( dt, "-" );
   strcat( dt, itoa( two, stuff, 10 ) );
   strcat( dt, "-" );
   strcat( dt, itoa( three, stuff, 10 ) );

   strcat( dt, "  " );

   pm = FALSE;
   if (mode.time_style == _12_HOUR) {
      if (hours >= 12 && hours < 24)
         pm = TRUE;
      if (hours < 1)
         hours = 12;
      else if (hours >= 13)
         hours -= 12;
   }

   if (hours < 1)
      strcat( dt, "0" );
   strcat( dt, itoa( hours, stuff, 10 ) );
   strcat( dt, ":" );
   if (minutes < 10)
      strcat( dt, "0" );
   strcat( dt, itoa( minutes, stuff, 10 ) );
   if (mode.time_style == _12_HOUR)
      strcat( dt, pm == FALSE ? "am" : "pm" );
   strcat( dt, "  " );
   return( add_chars( dt, window ) );
}



int  add_chars( char *string, WINDOW *window )
{
int  rc = OK;

   while (*string) {
      g_status.key_pressed = *string;
      rc = insert_overwrite( window );
      ++string;
   }
   return( rc );
}
