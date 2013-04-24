
#include "tdestr.h"
#include "common.h"
#include "define.h"
#include "tdefunc.h"



int  initialize_window( void )
{
int  top;
int  bottom;
int  start_col;
int  end_col;
WINDOW *wp;        
WINDOW *window;
register file_infos *fp;     
register int rc;
line_list_ptr ll;
line_list_ptr temp_ll;

   rc = OK;
   window = g_status.current_window;
   fp = g_status.current_file;
   if (window == NULL) {
      
      top = start_col = 0;
      bottom  = g_display.nlines;
      end_col = g_display.ncols - 1;
   } else {
     
      top       = window->top_line - 1;
      bottom    = window->bottom_line;
      start_col = window->start_col;
      end_col   = window->end_col;
   }

   assert( top < bottom );
   assert( start_col < end_col );
   assert( fp != NULL );

   if (create_window( &wp, top, bottom, start_col, end_col, fp ) == ERROR) {
      
      error( WARNING, bottom, main4 );

     
      if (fp->ref_count == 0) {

         
         if (fp->prev != NULL)
            fp->prev->next = fp->next;
         else
            g_status.file_list = fp->next;

         if (fp->next != NULL)
            fp->next->prev = fp->prev;

        

         ll = fp->undo_top;
         while (ll != NULL) {
            temp_ll = ll->next;
            if (ll->line != NULL)
               my_free( ll->line );
            my_free( ll );
            ll = temp_ll;
         }

         ll = fp->line_list;
         while (ll != NULL) {
            temp_ll = ll->next;
            if (ll->line != NULL)
               my_free( ll->line );
            my_free( ll );
            ll = temp_ll;
         }

#if defined( __MSC__ )
         _fheapmin( );
#endif

         free( fp );
         wp = g_status.current_window;
         if (wp != NULL && wp->visible)
            g_status.current_file = wp->file_info;
         else
            g_status.stop = TRUE;
      }
      rc = ERROR;
   }

   if (rc != ERROR) {
     
      wp->ccol = wp->start_col;
      wp->rcol = wp->bcol = 0;
      wp->rline = 1L;
      wp->ll    = fp->line_list;
      wp->visible = TRUE;
      wp->letter = fp->next_letter++;
      if (window != NULL)
         window->visible = FALSE;

      
      g_status.current_window = wp;
   }
   return( rc );
}



int  next_window( WINDOW *window )
{
register WINDOW *wp;
int  change;

   if (window != NULL) {
      change = FALSE;
     
      wp = window->next;
      while (wp != NULL) {
         if (wp->visible) {
            change = TRUE;
            break;
         }
         wp = wp->next;
      }

      
      if (!change) {
         wp = g_status.window_list;
         while (wp != window) {
            if (wp->visible) {
               change = TRUE;
               break;
            }
            wp = wp->next;
         }
      }
      if (change == TRUE) {
         entab_linebuff( );
         un_copy_line( window->ll, window, TRUE );
         g_status.current_window = wp;
         g_status.current_file = wp->file_info;
      }
   }
   return( OK );
}



int  prev_window( WINDOW *window )
{
register WINDOW *wp;
int  change;

   if (window != NULL) {
      change = FALSE;

     
      wp = window->prev;
      while (wp != NULL) {
         if (wp->visible) {
            change = TRUE;
            break;
         }
         wp = wp->prev;
      }

     
      if (!change) {
         wp = window->next;
         if (wp != NULL) {
            while (wp->next != NULL)
               wp = wp->next;
            while (wp != window) {
               if (wp->visible) {
                  change = TRUE;
                  break;
               }
               wp = wp->prev;
            }
         }
      }
      if (change == TRUE) {
         entab_linebuff( );
         un_copy_line( window->ll, window, TRUE );
         g_status.current_window = wp;
         g_status.current_file = wp->file_info;
      }
   }
   return( OK );
}


/*
 * 作用: 在光标处水平切分窗口
 * 参数:  window:  当前窗口的指针
 */
int  split_horizontal( WINDOW *window )
{
register WINDOW *wp;
register WINDOW *win;   
WINDOW *temp;
file_infos *file;       
int  rc;

   rc = OK;
   win = window;
   if ( win != NULL) {

      /*
       * 检查是否有空放置新窗口
       */
      if (win->bottom_line - win->cline < 2) {
         /*
          * 向上引动光标
          */
         error( WARNING, win->bottom_line, win1 );
         rc = ERROR;
      } else {
         file = win->file_info;

         assert( file != NULL );

         if (create_window( &temp, win->cline+1, win->bottom_line,
                            win->start_col, win->end_col, file ) == ERROR) {
            /*
             * 内存不足
             */
            error( WARNING, win->bottom_line, main4 );
            rc = ERROR;
         }
         if (rc == OK  &&  temp != NULL) {
            entab_linebuff( );
            un_copy_line( win->ll, win, TRUE );
            wp = temp;
            /*
             * 记录当前窗口因为创立新的窗口而丢生的行。
			 * 并且调整它的页大小              
             */
            win->bottom_line = win->cline;
            setup_window( win );
            display_current_window( win );

            /*
             * 设置新的光标位置
             */
            wp->rcol = win->rcol;
            wp->ccol = win->ccol;
            wp->bcol = win->bcol;
            wp->rline = win->rline;
            wp->bin_offset = win->bin_offset;
            wp->ll    = win->ll;
            wp->cline = wp->cline + win->cline - (win->top_line + win->ruler);
            if (wp->cline > wp->bottom_line)
               wp->cline = wp->bottom_line;
            wp->visible = TRUE;
            wp->vertical = win->vertical;
            wp->letter = file->next_letter++;
            wp->ruler  = mode.ruler;

            /*
             * 新的窗口变成了当前窗口
             */
            g_status.current_window = wp;

            show_window_count( g_status.window_count );
            show_window_header( wp );
            display_current_window( wp );
            if (wp->vertical)
               show_vertical_separator( wp );
            make_ruler( wp );
            show_ruler( wp );
            rc = OK;
         }
      }
   } else
      rc = ERROR;
   return( rc );
}



int  split_vertical( WINDOW *window )
{
register WINDOW *wp;
register WINDOW *win;   
WINDOW *temp;
file_infos *file;       
int  rc;

   rc = OK;
   win = window;
   if (win != NULL) {

      
      if (win->start_col + 15 > win->ccol) {
        
         error( WARNING, win->bottom_line, win2 );
         rc = ERROR;
      } else if (win->end_col - 15 < win->ccol) {
        
         error( WARNING, win->bottom_line, win3 );
         rc = ERROR;
      } else {
         file = win->file_info;

         assert( file != NULL );

         if (create_window( &temp, win->top_line-1, win->bottom_line,
                            win->ccol+1, win->end_col, file ) == ERROR) {
           
            error( WARNING, win->bottom_line, main4 );
            rc = ERROR;
         }

         if (rc == OK  &&  temp != NULL) {
            entab_linebuff( );
            un_copy_line( win->ll, win, TRUE );
            wp = temp;

            
            win->ccol = win->end_col = win->ccol - 1;
            win->rcol--;
            win->vertical = TRUE;
            show_window_header( win );
            show_vertical_separator( win );
            display_current_window( win );
            make_ruler( win );
            show_ruler( win );
            show_ruler_pointer( win );

            
            wp->rcol = win->rcol;
            wp->ccol = wp->start_col + win->ccol - win->start_col;
            if (wp->ccol > wp->end_col)
               wp->ccol = wp->end_col;
            wp->bcol  = win->bcol;
            wp->rline = win->rline;
            wp->bin_offset = win->bin_offset;
            wp->ll       = win->ll;
            wp->cline    = win->cline;
            wp->visible  = TRUE;
            wp->vertical = TRUE;
            wp->letter   = file->next_letter++;
            wp->ruler    = mode.ruler;

            
            g_status.current_window = wp;

            check_virtual_col( wp, wp->rcol, wp->ccol );
            wp->file_info->dirty = FALSE;
            show_window_count( g_status.window_count );
            show_window_header( wp );
            display_current_window( wp );
            make_ruler( wp );
            show_ruler( wp );
         }
      }
   } else
      rc = ERROR;
   return( rc );
}



void show_vertical_separator( WINDOW *window )
{
int  i;
int  line;
int  col;

   line = window->top_line - 1;
   col  = window->end_col + 1;
   if (col < g_display.ncols - 1) {
      i = window->bottom_line - line;

      assert( i <= g_display.nlines );

      while (i-- >= 0)
         c_output( VERTICAL_CHAR, col, line++, g_display.head_color );
   }
}



int  size_window( WINDOW *window )
{
char line_buff[(MAX_COLS+1)*2]; 
int  func;
int  c;
int  resize;
int  show_above_ruler;
int  old_bottom_line;
int  old_top_line;
int  new_bottom_line;
int  new_top_line;
register WINDOW *above;
register WINDOW *win;

   win = window;
   if (win->top_line != 1 && !win->vertical) {
      entab_linebuff( );
      un_copy_line( win->ll, win, TRUE );
      save_screen_line( 0, win->bottom_line, line_buff );

     
      set_prompt( win4, win->bottom_line );

      
      above = g_status.window_list;
      while (above->bottom_line + 2 != win->top_line || !above->visible)
         above = above->next;
      if (above->vertical)
         
         error( WARNING, win->bottom_line, win5 );
      else {
         old_top_line = win->top_line;
         old_bottom_line = above->bottom_line;
         show_above_ruler = FALSE;
         for (func=0; func != AbortCommand && func != Rturn; ) {

            
            c = getkey( );
            func = getfunc( c );
            if (c == RTURN || func == NextLine || func == BegNextLine)
               func = Rturn;
            else if (c == ESC)
               func = AbortCommand;
            resize = FALSE;

           
            if (func == LineUp) {
               if (above->bottom_line > above->top_line + above->ruler) {
                  if (win->rline == (win->cline - (win->top_line+win->ruler-1)))
                     --win->cline;
                  --win->top_line;
                  if (above->cline == above->bottom_line)
                     --above->cline;
                  --above->bottom_line;
                  resize = TRUE;
                  if (mode.ruler) {
                     if (win->ruler == FALSE) {
                        if (win->cline == win->top_line)
                           ++win->cline;
                        if (win->cline > win->bottom_line)
                           win->cline = win->bottom_line;
                        win->ruler = TRUE;
                     }
                  }
               }

            
            } else if (func == LineDown) {
               if (win->bottom_line > win->top_line + win->ruler) {
                  if (win->cline == win->top_line + win->ruler)
                     ++win->cline;
                  ++win->top_line;
                  ++above->bottom_line;
                  resize = TRUE;
                  if (mode.ruler) {
                     if (above->ruler == FALSE) {
                        if (above->cline == above->top_line)
                           ++above->cline;
                        if (above->cline > above->bottom_line)
                           above->cline = above->bottom_line;
                        above->ruler = TRUE;
                        make_ruler( above );
                        show_above_ruler = TRUE;
                     }
                  }
               }
            }

           
            if (resize == TRUE) {
               setup_window( above );
               display_current_window( above );
               if (show_above_ruler) {
                  show_ruler( above );
                  show_ruler_pointer( above );
                  show_above_ruler = FALSE;
               }
               setup_window( win );
               show_window_header( win );
               win->ruler = mode.ruler;
               make_ruler( win );
               show_ruler( win );
               show_ruler_pointer( win );
               display_current_window( win );
               save_screen_line( 0, win->bottom_line, line_buff );

               
               set_prompt( win4, win->bottom_line );
            }
         }
         new_top_line = win->top_line;
         new_bottom_line = above->bottom_line;
         for (above=g_status.window_list; above != NULL; above=above->next) {
            if (!above->visible) {
               if (above->bottom_line == old_bottom_line) {
                  above->bottom_line = new_bottom_line;
                  if (above->cline < new_bottom_line)
                     above->cline = new_bottom_line;
                  setup_window( above );
               } else if (above->top_line == old_top_line) {
                  above->top_line = new_top_line;
                  if (above->cline < new_top_line)
                     above->cline = new_top_line;
                  if ((long)(above->cline+1L - (above->top_line+above->ruler)) >
                                                                above->rline)
                     above->cline = (int)above->rline + above->top_line +
                                     above->ruler - 1;
                  setup_window( above );
               }
            }
         }
      }
      restore_screen_line( 0, win->bottom_line, line_buff );
   } else {
     if (win->vertical)
       
        error( WARNING, win->bottom_line, win5 );
     else
        
        error( WARNING, win->bottom_line, win6 );
   }
   return( OK );
}



int  zoom_window( WINDOW *window )
{
register WINDOW *wp;

   if (window != NULL) {
      entab_linebuff( );
      un_copy_line( window->ll, window, TRUE );
      for (wp=g_status.window_list; wp != NULL; wp=wp->next) {
         if (wp != window && wp->visible)
            wp->visible = FALSE;

        
         diff.defined = FALSE;
         if (wp->top_line != 1)
            wp->cline = wp->cline - (wp->top_line+wp->ruler) + 1;
         wp->top_line = 1;
         wp->bottom_line = g_display.nlines;
         wp->end_col   = g_display.ncols - 1;
         wp->start_col = 0;
         wp->vertical  = FALSE;
         check_virtual_col( wp, wp->rcol, wp->ccol );
         make_ruler( wp );
      }
      redraw_screen( window );
      show_ruler( window );
   }
   return( OK );
}



int  next_hidden_window( WINDOW *window )
{
int  poof = FALSE;
register WINDOW *wp;

   if (window != NULL) {

     
      wp = window;
      for (wp=window->next; wp != NULL && !poof; ) {
         if (!wp->visible)
            poof = TRUE;
         else
            wp = wp->next;
      }

     
      if (!poof) {
         for (wp=g_status.window_list; wp != NULL && !poof; ) {
            if (!wp->visible)
               poof = TRUE;
            else
               wp = wp->next;
         }
      }

      if (poof) {
         entab_linebuff( );
         un_copy_line( window->ll, window, TRUE );
         wp->cline = window->top_line + window->ruler +
                       (wp->cline - (wp->top_line + wp->ruler));
         wp->top_line = window->top_line;
         wp->bottom_line = window->bottom_line;
         wp->start_col = window->start_col;
         wp->end_col   = window->end_col;
         wp->vertical  = window->vertical;
         if (wp->cline < wp->top_line + wp->ruler)
            wp->cline = wp->top_line + wp->ruler;
         if (wp->cline > wp->bottom_line)
            wp->cline = wp->bottom_line;
         if ((wp->cline+1L - (wp->top_line+wp->ruler)) > wp->rline)
            wp->cline = (int)wp->rline + wp->top_line + wp->ruler - 1;
         check_virtual_col( wp, wp->rcol, wp->ccol );
         wp->visible = TRUE;
         window->visible = FALSE;
         if (diff.defined  &&  (diff.w1 == window  ||  diff.w2 == window))
            diff.defined = FALSE;
         g_status.current_window = wp;
         redraw_current_window( wp );
         make_ruler( wp );
         show_ruler( wp );
      }
   }
   return( OK );
}



void setup_window( WINDOW *window )
{
   window->page = window->bottom_line - (window->top_line + window->ruler) -
                  g_status.overlap + 1;
   if (window->page < 1)
      window->page = 1;
}



void finish( WINDOW *window )
{
register WINDOW *wp;   
register WINDOW *win;  
file_infos *file, *fp;  
int  poof;
int  cline;
int  top;
int  bottom;
int  start_col;
int  end_col;
int  max_letter;
int  file_change = FALSE;
line_list_ptr ll;
line_list_ptr temp_ll;

   win = window;
   entab_linebuff( );
   if (un_copy_line( win->ll, win, TRUE ) == ERROR)
      return;

   file = win->file_info;
   
   file = win->file_info;
   for (wp=g_status.window_list; wp != NULL; wp=wp->next) {
      if (wp->file_info == file) {
         if (!wp->visible) {
            if (wp->prev == NULL) {
               if (wp->next == NULL)
                  g_status.stop = TRUE;
               else
                  g_status.window_list = wp->next;
            } else
               wp->prev->next = wp->next;
            if (wp->next)
               wp->next->prev = wp->prev;
            --wp->file_info->ref_count;
            free( wp );
            --g_status.window_count;
         }
      }
   }

   if (win->prev == NULL && win->next == NULL)
      g_status.stop = TRUE;

   poof = FALSE;

   if (g_status.stop != TRUE) {
     
      top       = win->top_line;
      bottom    = win->bottom_line;
      start_col = win->start_col;
      end_col   = win->end_col;
      wp = g_status.window_list;
      if (wp != NULL) {
         while (wp->next != NULL)
            wp = wp->next;
      }
      while (wp != NULL && poof == FALSE) {
         if (wp->top_line == top         &&  wp->bottom_line == bottom  &&
             wp->start_col == start_col  &&  wp->end_col == end_col     &&
                                                              !wp->visible)
            poof = TRUE;
         else
            wp = wp->prev;
      }

      if (poof == FALSE) {
         
         wp = g_status.window_list;
         while (wp != NULL && poof == FALSE) {
            if (wp->bottom_line+2 == win->top_line &&
                wp->start_col     == win->start_col &&
                     wp->end_col       == win->end_col   && wp->visible) {
               poof = TRUE;
               top  = wp->top_line;
            } else
               wp = wp->next;
         }
         if (poof == FALSE) {
           
            wp = g_status.window_list;
            while (wp != NULL && poof == FALSE) {
               if (wp->top_line-2 == win->bottom_line  &&
                   wp->start_col     == win->start_col &&
                         wp->end_col    == win->end_col   && wp->visible) {
                  poof = TRUE;
                  bottom = wp->bottom_line;
               } else
                  wp = wp->next;
            }
         }
         if (poof == FALSE) {
           
            wp = g_status.window_list;
            while (wp != NULL && poof == FALSE) {
               if (wp->top_line    == win->top_line  &&
                         wp->bottom_line == win->bottom_line &&
                         wp->start_col-2 == win->end_col     && wp->visible) {
                  poof = TRUE;
                  end_col = wp->end_col;
               } else
                  wp = wp->next;
            }
         }
         if (poof == FALSE) {
           
            wp = g_status.window_list;
            while (wp != NULL && poof == FALSE) {
               if (wp->top_line    == win->top_line  &&
                   wp->bottom_line == win->bottom_line &&
                   wp->end_col+2   == win->start_col   && wp->visible) {
                  poof = TRUE;
                  start_col = wp->start_col;
               } else
                  wp = wp->next;
            }
         }
         if (poof == FALSE) {
           
            wp = g_status.window_list;
            if (wp != NULL) {
               while (wp->next != NULL)
                  wp = wp->next;
            }
            while (wp != NULL && poof == FALSE) {
               if (!wp->visible)
                  poof = TRUE;
               else
                  wp = wp->prev;
            }
         }
      }
      if (poof) {
         wp->visible = TRUE;
         cline = wp->cline - (wp->top_line+wp->ruler);
         wp->top_line = top;
         wp->bottom_line = bottom;
         wp->cline = (wp->top_line+wp->ruler) + cline;
         if (wp->cline > wp->bottom_line)
            wp->cline = wp->top_line+wp->ruler;
         wp->start_col = start_col;
         wp->end_col   = end_col;
         if (start_col == 0 && end_col == g_display.ncols - 1)
            wp->vertical = FALSE;
         else
            wp->vertical = TRUE;
         check_virtual_col( wp, wp->rcol, wp->ccol );
         setup_window( wp );
         show_window_header( wp );
         if (wp->vertical)
            show_vertical_separator( wp );

         
         g_status.current_window = wp;
      }
   }

   if (!poof && g_status.stop != TRUE)
     
      error( WARNING, win->bottom_line, win7 );
   else {

      
      if (--file->ref_count == 0) {

        
         if (file == g_status.marked_file) {
            g_status.marked      = FALSE;
            g_status.marked_file = NULL;
         }

         for (fp=g_status.file_list; fp != NULL; fp=fp->next) {
            if (fp->file_no > file->file_no)
               fp->file_no--;
         }
         file_change = file->file_no;

        
         if (file->prev == NULL)
            g_status.file_list = file->next;
         else
            file->prev->next = file->next;
         if (file->next)
            file->next->prev = file->prev;

        
         ll = file->undo_top;
         while (ll != NULL) {
            temp_ll = ll->next;
            if (ll->line != NULL)
               my_free( ll->line );
            my_free( ll );
            ll = temp_ll;
         }

         ll = file->line_list;
         while (ll != NULL) {
            temp_ll = ll->next;
            if (ll->line != NULL)
               my_free( ll->line );
            my_free( ll );
            ll = temp_ll;
         }

#if defined( __MSC__ )
         _fheapmin( );
#endif

         free( file );
         if (--g_status.file_count) {
            show_file_count( g_status.file_count );
            show_avail_mem( );
         }
      }

      
      if (win->prev == NULL)
         g_status.window_list = win->next;
      else
         win->prev->next = win->next;

      if (win->next)
         win->next->prev = win->prev;

      if (diff.defined  &&  (diff.w1 == win  ||  diff.w2 == win))
         diff.defined = FALSE;

     
      free( win );
      --g_status.window_count;

      if (g_status.stop == FALSE) {
         g_status.current_file = wp->file_info;
         wp->file_info->dirty = LOCAL;
         make_ruler( wp );
         show_ruler( wp );
         show_window_count( g_status.window_count );
         if (file_change) {
            for (wp=g_status.window_list; wp!=NULL; wp=wp->next)
               if (wp->visible)
                  show_window_number_letter( wp );
         } else {
            max_letter = 'a';
            for (wp=g_status.window_list; wp!=NULL; wp=wp->next) {
               if (wp->file_info == file && wp->letter > max_letter)
                  max_letter = wp->letter;
            }
            if (max_letter < file->next_letter - 1)
               file->next_letter = max_letter + 1;
         }
      }
   }

   if (g_status.stop == TRUE) {
      if (g_status.sas_defined && g_status.sas_arg < g_status.sas_argc) {
         show_avail_mem( );
         for (bottom=0; bottom <= g_display.nlines; bottom++)
            eol_clear( 0, bottom, g_display.text_color );
         bottom  = g_display.nlines;
         set_prompt( win18, bottom );
         top = getkey( );
         top = getfunc( top );
         eol_clear( 0, bottom, g_display.text_color );
         if (top == RepeatGrep) {
            g_status.command = RepeatGrep;
            g_status.window_list = g_status.current_window = NULL;
            if (search_and_seize( g_status.window_list ) != ERROR)
               g_status.stop = FALSE;
         }
      }
   }
}



int  create_window( WINDOW **window, int top, int bottom, int start_col,
                    int end_col, file_infos *file )
{
WINDOW *wp;             
register WINDOW *prev;
int  rc;                

   rc = OK;
   
   if ((*window = (WINDOW *)calloc( 1, sizeof(WINDOW) )) == NULL) {
     
      error( WARNING, g_display.nlines, main4 );
      rc = ERROR;
   } else {

    
      wp              = *window;
      wp->file_info   = file;
      wp->top_line    = top+1;
      wp->bottom_line = bottom;
      wp->start_col   = start_col;
      wp->end_col     = end_col;
      wp->bin_offset  = 0;
      wp->ruler       = mode.ruler;
      make_ruler( wp );
      wp->cline       = wp->top_line + wp->ruler;
      if (start_col == 0 && end_col == g_display.ncols-1)
         wp->vertical = FALSE;
      else
         wp->vertical = TRUE;
      wp->prev        = NULL;
      wp->next        = NULL;

      setup_window( wp );

     
      prev = g_status.current_window;
      if (prev) {
         (*window)->prev = prev;
         if (prev->next)
            prev->next->prev = *window;
         (*window)->next = prev->next;
         prev->next = *window;
      }
      if (g_status.window_list == NULL)
         g_status.window_list = *window;

      
      ++file->ref_count;
      file->dirty = LOCAL;
      ++g_status.window_count;
   }
   return( rc );
}
