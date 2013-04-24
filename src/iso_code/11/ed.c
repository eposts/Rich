

#include "tdestr.h"     
#include "define.h"     
#include "tdefunc.h"    
#include "global.h"     
#include "prompts.h"    
#include "default.h"    



int  insert_newline( WINDOW *window )
{
char *source;           
char *dest;             
int  len;               
int  split_len;
int  add;               
int  rcol;
int  rc;
long length;
int  carriage_return;
int  split_line;
int  wordwrap;
int  dirty;
int  old_bcol;
register WINDOW *win;   
file_infos *file;       
line_list_ptr new_node;
text_ptr new_line;      

   rc = OK;
   win = window;
   file = win->file_info;
   length = file->length;
   wordwrap = mode.word_wrap;
   switch (g_status.command) {
      case WordWrap :
         carriage_return = TRUE;
         split_line = FALSE;
         break;
      case AddLine  :
         split_line = carriage_return = FALSE;
         break;
      case SplitLine :
         split_line = carriage_return = TRUE;
         break;
      case Rturn :
      default    :

        
         if (file->crlf == BINARY)
            return( next_line( win ) );

         show_ruler_char( win );
         carriage_return = TRUE;
         split_line = FALSE;
         break;
   }

   
   new_node = (line_list_ptr)my_malloc( sizeof(line_list_struc), &rc );
   new_line = NULL;
   win->visible = FALSE;
   old_bcol = win->bcol;
   if (rc == OK) {
      new_node->line  = new_line;
      new_node->len   = 0;
      new_node->dirty = FALSE;

      if (win->ll->len != EOF) {
         win->file_info->modified = TRUE;
         if (mode.do_backups == TRUE)
            rc = backup_file( win );
         copy_line( win->ll );
         detab_linebuff( );
         len = g_status.line_buff_len;
         split_len = 0;
         if (win->rcol < len)
            win->ll->dirty = TRUE;

         source = g_status.line_buff + len;
         if (carriage_return || split_line) {
            if (win->rcol < len) {
               source = g_status.line_buff + win->rcol;
               split_len = len - win->rcol;
               len = win->rcol;
            }
         }
         g_status.line_buff_len = len;
         entab_linebuff( );
         if (un_copy_line( win->ll, win, TRUE ) == OK) {

            assert( split_len >= 0 );
            assert( split_len < MAX_LINE_LENGTH );

            memmove( g_status.line_buff, source, split_len );
            g_status.line_buff_len = len = split_len;
            g_status.copied = TRUE;
            entab_linebuff( );
         } else
            rc = ERROR;
      } else {
         g_status.line_buff_len = len = 0;
         g_status.copied = TRUE;
      }

      if (rc == OK) {
         new_node->line  = new_line;
         new_node->len   = 0;
         new_node->dirty = TRUE;

         
         if (win->ll->next != NULL) {
            win->ll->next->prev = new_node;
            new_node->next = win->ll->next;
            win->ll->next = new_node;
            new_node->prev = win->ll;
         } else {
            new_node->next = win->ll;
            if (win->ll->prev != NULL)
               win->ll->prev->next = new_node;
            new_node->prev = win->ll->prev;
            win->ll->prev = new_node;
            if (new_node->prev == NULL)
               win->file_info->line_list = new_node;
            win->ll = new_node;
         }

         ++file->length;
         detab_linebuff( );
         entab_linebuff( );
         rc = un_copy_line( new_node, win, FALSE );
         adjust_windows_cursor( win, 1 );

         file->dirty = NOT_LOCAL;
         if (length == 0l || wordwrap || win->cline == win->bottom_line)
            file->dirty = GLOBAL;
         else if (!split_line)
            update_line( win );

        
         if (rc == OK  &&  (carriage_return || split_line)) {
            dirty = file->dirty;
            if (win->cline < win->bottom_line)
               win->cline++;
            win->rline++;
            if (win->ll->next != NULL) {
               win->bin_offset += win->ll->len;
               win->ll = win->ll->next;
            }
            rcol = win->rcol;
            old_bcol = win->bcol;

            if (win->ll->next != NULL) {
               if (mode.indent || wordwrap) {
               
                  add = find_left_margin( wordwrap == FIXED_WRAP ?
                                          win->ll : win->ll->prev, wordwrap );

                  assert( add >= 0 );
                  assert( add < MAX_LINE_LENGTH );

                  copy_line( win->ll );
                  detab_linebuff( );
                  len = g_status.line_buff_len;
                  source = g_status.line_buff;
                  if (len + add > MAX_LINE_LENGTH)
                     add = MAX_LINE_LENGTH - len;
                  dest = source + add;

                  assert( len >= 0);
                  assert( len < MAX_LINE_LENGTH );

                  memmove( dest, source, len );


                  assert( add >= 0 );
                  assert( add < MAX_LINE_LENGTH );

                  memset( source, ' ', add );
                  win->rcol = add;
                  g_status.line_buff_len += add;
                  entab_linebuff( );
                  rc = un_copy_line( win->ll, win, TRUE );
               } else
                  win->rcol = 0;
            }
            if (rc == OK  &&  split_line) {
               win->rline--;
               win->ll = win->ll->prev;
               if (win->cline > win->top_line + window->ruler)
                  win->cline--;
               win->rcol = rcol;
            }
            check_virtual_col( win, win->rcol, win->ccol );
            if (dirty == GLOBAL || file->dirty == LOCAL || wordwrap)
               file->dirty = GLOBAL;
            else
               file->dirty = dirty;
         }
      } else {
         if (new_node != NULL)
            my_free( new_node );
      }
   } else {
      if (new_node != NULL)
         my_free( new_node );
      error( WARNING, window->bottom_line, main4 );
   }

  
   win->visible = TRUE;
   if (rc == OK) {
      if (file->dirty != GLOBAL)
         my_scroll_down( win );
      restore_marked_block( win, 1 );
      show_size( win );
      show_avail_mem( );
      if (old_bcol != win->bcol) {
         make_ruler( win );
         show_ruler( win );
      }
   }
   return( rc );
}



int  insert_overwrite( WINDOW *window )
{
char *source;           
char *dest;             
int  len;               
int  pad;               
int  add;               
register int rcol;
register WINDOW *win;  
int  rc;

   win = window;
   if (win->ll->len == EOF || g_status.key_pressed >= 256)
      rc = OK;
   else {
      rcol = win->rcol;
     
      if (rcol >= g_display.line_length) {
        
         error( WARNING, win->bottom_line, ed2 );
         rc = ERROR;
      } else {
         copy_line( win->ll );
         detab_linebuff( );

         len = g_status.line_buff_len;
         pad = rcol > len ? rcol - len : 0;

         if (mode.insert || rcol >= len)
          
            add = 1;
         else
           
            add = 0;

        
         if (len + pad + add >= g_display.line_length) {
           
            error( WARNING, win->bottom_line, ed3 );
            rc = ERROR;
         } else {

           
            if (pad > 0  || add > 0) {
               source = g_status.line_buff + rcol - pad;
               dest = source + pad + add;

               assert( len + pad - rcol >= 0 );
               assert( len + pad - rcol < MAX_LINE_LENGTH );

               memmove( dest, source, len + pad - rcol );

              

               assert( pad >= 0 );
               assert( pad < MAX_LINE_LENGTH );

               memset( source, ' ', pad );
            }
            g_status.line_buff[rcol] = (char)g_status.key_pressed;
            g_status.line_buff_len += pad + add;
            entab_linebuff( );

            
            win->file_info->dirty = NOT_LOCAL;
            win->ll->dirty = TRUE;
            show_changed_line( win );
            if (win->ccol < win->end_col) {
               show_curl_line( win );
               show_ruler_char( win );
               win->ccol++;
            } else {
               win->bcol++;
               win->file_info->dirty = LOCAL;
               make_ruler( win );
               show_ruler( win );
            }
            rcol++;
         }

        
         check_virtual_col( win, rcol, win->ccol );
         win->file_info->modified = TRUE;
         if (mode.word_wrap) {
            add = mode.right_justify;
            mode.right_justify = FALSE;
            g_status.command = FormatText;
            word_wrap( win );
            mode.right_justify = add;
         }
         rc = OK;
      }
   }
   return( rc );
}



int  join_line( WINDOW *window )
{
int  len;               
int  new_len;           
int  next_len;          
text_ptr q;             
text_ptr tab_free;      
int  pad;               
register WINDOW *win;   
WINDOW *wp;
line_list_ptr next_node;
int  rc;

   win = window;
   if (win->ll->len == EOF  ||  win->ll->next->len == EOF)
      return( ERROR );

   rc = OK;

   assert( win->ll->next != NULL );

   next_node = win->ll->next;
   load_undo_buffer( win->file_info, win->ll->line, win->ll->len );
   copy_line( win->ll );
   detab_linebuff( );

  
   if (win->rcol < (len = g_status.line_buff_len))
      len = win->rcol;

  
   pad = win->rcol > len ? win->rcol - len : 0;

   assert( pad >= 0 );
   assert( pad < MAX_LINE_LENGTH );

  
   next_len = next_node->len;
   tab_free = detab_a_line( next_node->line, &next_len );

   assert( next_len >= 0 );
   assert( next_len < MAX_LINE_LENGTH );
   assert( len >= 0 );
   assert( len < MAX_LINE_LENGTH );

 
   new_len = len + pad + next_len;
   if (new_len >= g_display.line_length) {
     
      error( WARNING, win->bottom_line, ed4 );
      rc = ERROR;
   } else {
      if (mode.do_backups == TRUE) {
         win->file_info->modified = TRUE;
         rc = backup_file( win );
      }
      q = (text_ptr)(g_status.line_buff + len);
   
      if (pad > 0) {
         while (pad--)
            *q++ = ' ';
      }
      _fmemcpy( q, tab_free, next_len );
      g_status.line_buff_len = new_len;
      entab_linebuff( );

      if ((rc = un_copy_line( win->ll, win, FALSE )) == OK) {

         if (next_node->next != NULL)
            next_node->next->prev = win->ll;
         win->ll->next = next_node->next;
         win->ll->dirty = TRUE;

         --win->file_info->length;
         ++win->rline;
         adjust_windows_cursor( win, -1 );
         restore_marked_block( win, -1 );
         --win->rline;

         wp = g_status.window_list;
         while (wp != NULL) {
            if (wp->file_info == win->file_info) {
            
               if (wp != win) {
                  if (wp->ll == next_node)
                     wp->ll = win->ll->next;
               }
            }
            wp = wp->next;
         }

       
         if (next_node->line != NULL)
            my_free( next_node->line );
         my_free( next_node );

         show_size( win );
         show_avail_mem( );
         win->file_info->dirty = GLOBAL;
      }
   }
   return( rc );
}



int  word_delete( WINDOW *window )
{
int  len;               
int  count;             
register int start;     
char *source;           
char *dest;             
text_ptr p;
register WINDOW *win;   
int  rc;

   win = window;
   if (win->rline > win->file_info->length  || win->ll->len == EOF)
      return( ERROR );

   rc = OK;
   copy_line( win->ll );
   detab_linebuff( );
   if (win->rcol >= (len = g_status.line_buff_len)) {
      rc = join_line( win );
      if (rc == OK) {
         p = win->ll->line;
         if (p != NULL) {
            p += win->rcol;
            if (win->rcol < win->ll->len) {
               len = win->ll->len - win->rcol;
               load_undo_buffer( win->file_info, p, len );
            }
         }
      }
   } else {

      assert( len >= 0);
      assert( len < MAX_LINE_LENGTH );

    
      start = win->rcol;
      if (isspace( g_status.line_buff[start] )) {
      
         while (start < len  &&  isspace( g_status.line_buff[start] ))
            ++start;
      } else {
      
         while (start < len  &&  !isspace( g_status.line_buff[start] ))
            ++start;
         while (start < len  &&  isspace( g_status.line_buff[start] ))
            ++start;
      }

      
      count = start - win->rcol;
      source = g_status.line_buff + start;
      dest = g_status.line_buff + win->rcol;

      assert( len - start >= 0 );

      memmove( dest, source, len - start );
      g_status.line_buff_len = len - count;
      entab_linebuff( );
      win->file_info->modified = TRUE;
      win->file_info->dirty = GLOBAL;
      win->ll->dirty = TRUE;

    
      if (g_status.command == WordDelete)
         show_changed_line( win );
   }
   return( rc );
}



int  dup_line( WINDOW *window )
{
register int len;       
text_ptr p;
register WINDOW *win;   
line_list_ptr next_node;
int  rc;

   win = window;

   if (win->rline > win->file_info->length  ||  win->ll->len == EOF)
      return( ERROR );

   entab_linebuff( );
   rc = un_copy_line( win->ll, win, TRUE );
   len = win->ll->len;

   assert( len >= 0);
   assert( len < MAX_LINE_LENGTH );

   p = NULL;
   next_node = NULL;
   if (rc == OK) {
      p = (text_ptr)my_malloc( len, &rc );
      next_node = (line_list_ptr)my_malloc( sizeof(line_list_struc), &rc );
   }

   if (rc == OK) {
      win->file_info->modified = TRUE;
      if (mode.do_backups == TRUE)
         rc = backup_file( win );
      ++win->file_info->length;

      if (len > 0)
         _fmemcpy( p, win->ll->line, len );

      next_node->line  = p;
      next_node->dirty = TRUE;
      next_node->len   = len;

      if (win->ll->next != NULL)
         win->ll->next->prev = next_node;

      next_node->next = win->ll->next;
      next_node->prev = win->ll;
      win->ll->next = next_node;

      adjust_windows_cursor( win, 1 );

      
      if  (win->cline != win->bottom_line)
         my_scroll_down( win );
      win->file_info->dirty = NOT_LOCAL;

      
      restore_marked_block( win, 1 );
      show_size( win );
      show_avail_mem( );
   } else {
      
      if (p != NULL)
         my_free( p );
      if (next_node != NULL)
         my_free( next_node );
      error( WARNING, win->bottom_line, ed5 );
   }
   return( rc );
}



int  back_space( WINDOW *window )
{
int  rc;                
int  len;               
char *source;           
char *dest;             
text_ptr p;             
int  plen;              
int  del_count;         
int  pos;               
register int rcol;
int  ccol;
int  old_bcol;
register WINDOW *win;  
WINDOW *wp;
line_list_ptr temp_ll;

   win = window;
   if (win->rline > win->file_info->length || win->ll->len == EOF)
      return( ERROR );
   rc = OK;
   copy_line( win->ll );
   detab_linebuff( );
   len = g_status.line_buff_len;
   rcol = win->rcol;
   ccol = win->ccol;
   old_bcol = win->bcol;
   if (rcol == 0) {
      if (win->rline > 1) {
        

         assert( win->ll->prev != NULL );

         p = win->ll->prev->line;
         plen = win->ll->prev->len;
         if (len + 2 + plen >= g_display.line_length) {
          
            error( WARNING, win->bottom_line, ed4 );
            return( ERROR );
         }

         win->file_info->modified = TRUE;
         if ((rc = un_copy_line( win->ll, win, TRUE )) == OK) {
            --win->rline;
            win->ll = win->ll->prev;
            win->bin_offset -= win->ll->len;
            win->ll->dirty = TRUE;
            copy_line( win->ll );
            detab_linebuff( );
            len = g_status.line_buff_len;
            rcol = len;

            p = win->ll->next->line;
            plen = win->ll->next->len;

            
            assert( plen >= 0 );
            assert( len  >= 0 );

            _fmemcpy( g_status.line_buff+len, p, plen );
            g_status.line_buff_len = len + plen;

            load_undo_buffer( win->file_info, p, plen );
            if (p != NULL)
               my_free( p );

            temp_ll = win->ll->next;

            if (temp_ll->prev != NULL)
               temp_ll->prev->next = temp_ll->next;
            temp_ll->next->prev = temp_ll->prev;

            --win->file_info->length;
            ++win->rline;
            restore_marked_block( win, -1 );
            adjust_windows_cursor( win, -1 );
            --win->rline;

            wp = g_status.window_list;
            while (wp != NULL) {
               if (wp->file_info == win->file_info) {
                  if (wp != win) {
                     if (wp->ll == temp_ll)
                        wp->ll = win->ll->next;
                  }
               }
               wp = wp->next;
            }

            my_free( temp_ll );

            if (win->cline > win->top_line + win->ruler)
               --win->cline;

            
            ccol = rcol - win->bcol;
            show_size( win );
            show_avail_mem( );
            check_virtual_col( win, rcol, ccol );
            win->file_info->dirty = GLOBAL;
            make_ruler( win );
            show_ruler( win );
         }
      } else
         return( ERROR );
   } else {
    
      del_count = 1;   
      if (mode.indent) {
        
         pos = first_non_blank( (text_ptr)g_status.line_buff, len );
         if (pos == rcol  ||
                         is_line_blank( (text_ptr)g_status.line_buff, len )) {
           
            temp_ll = win->ll->prev;
            for (; temp_ll != NULL; temp_ll=temp_ll->prev) {
               p = temp_ll->line;
               plen = first_non_blank( p, temp_ll->len );
               if (plen < rcol  &&  plen != temp_ll->len) {
                
                  del_count = rcol - plen;
                  break;
               }
            }
         }
      }

      
      if (rcol - del_count < len) {
         dest = g_status.line_buff + rcol - del_count;
         if (rcol > len) {
            source = g_status.line_buff + len;
            pos = 0;
            len = (rcol + 1) - del_count;
         } else {
            source = g_status.line_buff + rcol;
            pos = len - rcol;
            len = len - del_count;
         }

         assert( pos >= 0 );
         assert( len >= 0 );
         assert( len <= MAX_LINE_LENGTH );

         memmove( dest, source, pos );
         g_status.line_buff_len = len;
         entab_linebuff( );
      }
      rcol -= del_count;
      ccol -= del_count;
      win->file_info->dirty = NOT_LOCAL;
      win->ll->dirty = TRUE;
      show_ruler_char( win );
      show_changed_line( win );
      check_virtual_col( win, rcol, ccol );
      if (!win->file_info->dirty)
         show_curl_line( win );
      if (old_bcol != win->bcol) {
         make_ruler( win );
         show_ruler( win );
      }
   }
   win->file_info->modified = TRUE;
   return( rc );
}



int  line_kill( WINDOW *window )
{
register WINDOW *win;   
register WINDOW *wp;
line_list_ptr killed_node;
int  rc;

   win = window;
   killed_node = win->ll;
   rc = OK;
   if (killed_node->len != EOF) {
      win->file_info->modified = TRUE;
      if (mode.do_backups == TRUE)
         rc = backup_file( win );

      if (rc == OK) {
         load_undo_buffer( win->file_info,
            g_status.copied ? (text_ptr)g_status.line_buff : killed_node->line,
            g_status.copied ? g_status.line_buff_len       : killed_node->len );

         --win->file_info->length;

         win->ll = win->ll->next;

         if (killed_node->prev != NULL)
            killed_node->prev->next = killed_node->next;
         else
            win->file_info->line_list = win->ll;

         killed_node->next->prev = killed_node->prev;

         wp = g_status.window_list;
         while (wp != NULL) {
            if (wp->file_info == win->file_info) {
               if (wp != win) {
                  if (wp->ll == killed_node)
                     wp->ll = win->ll;
               }
            }
            wp = wp->next;
         }

        
         if (killed_node->line != NULL)
            my_free( killed_node->line );
         my_free( killed_node );

         win->file_info->dirty = NOT_LOCAL;

         g_status.copied = FALSE;
         
         adjust_windows_cursor( win, -1 );
         restore_marked_block( win, -1 );

         
         if (win->file_info->dirty == NOT_LOCAL)
            my_scroll_down( win );
         show_size( win );
         show_avail_mem( );
      }
   } else
      rc = ERROR;
   return( rc );
}



int  char_del_under( WINDOW *window )
{
char *source;    
int  len;
register WINDOW *win;   

   win = window;
   if (win->rline > win->file_info->length || win->ll->len == EOF)
      return( OK );
   copy_line( win->ll );
   detab_linebuff( );
   if (win->rcol < (len = g_status.line_buff_len)) {
      
      source = g_status.line_buff + win->rcol + 1;

      assert( len - win->rcol >= 0 );

      memmove( source-1, source, len - win->rcol );
      --g_status.line_buff_len;
      entab_linebuff( );
      win->file_info->dirty    = GLOBAL;
      win->file_info->modified = TRUE;
      win->ll->dirty = TRUE;
      show_changed_line( win );
   } else if (g_status.command == StreamDeleteChar)
      join_line( win );
   return( OK );
}



int  eol_kill( WINDOW *window )
{
register WINDOW *win;   

   win = window;
   if (win->rline > win->file_info->length  ||  win->ll->len == EOF)
      return( OK );
   copy_line( win->ll );
   detab_linebuff( );
   load_undo_buffer( win->file_info, (text_ptr)g_status.line_buff,
                     g_status.line_buff_len );
   if (win->rcol < g_status.line_buff_len) {
     
      g_status.line_buff_len = win->rcol;
      entab_linebuff( );
      win->file_info->dirty = GLOBAL;
      win->ll->dirty = TRUE;
      show_changed_line( win );
   }
   return( OK );
}



int  undo_line( WINDOW *window )
{
register WINDOW *win;   

   win = window;
   if (win->rline <= win->file_info->length  &&  win->ll->len != EOF &&
                            g_status.copied) {
      g_status.copied = FALSE;
      copy_line( win->ll );
      detab_linebuff( );
      win->file_info->dirty = GLOBAL;
      show_changed_line( win );
   }
   return( OK );
}



int  undo( WINDOW *window )
{
register WINDOW *win;   
line_list_ptr node;

   win = window;
   if (win->file_info->undo_count > 0) {
      entab_linebuff( );
      if (un_copy_line( win->ll, win, TRUE ) == ERROR)
         return( ERROR );

      node = win->file_info->undo_top;
      win->file_info->undo_top = node->next;
      win->file_info->undo_top->prev = NULL;
      --win->file_info->undo_count;

      node->next = node->prev = NULL;

      ++win->file_info->length;

      if (win->ll->prev != NULL)
         win->ll->prev->next = node;
      node->prev = win->ll->prev;

      win->ll->prev = node;
      node->next = win->ll;
      win->ll = node;
      win->ll->dirty = TRUE;

      if (win->ll->prev == NULL)
         win->file_info->line_list = win->ll;

      adjust_windows_cursor( win, 1 );

     
      win->file_info->dirty = GLOBAL;
      show_size( win );
      show_avail_mem( );
   }
   return( OK );
}



int  beg_next_line( WINDOW *window )
{
int  rc;

   window->rcol = 0;
   rc = prepare_move_down( window );
   check_virtual_col( window, window->rcol, window->ccol );
   sync( window );
   make_ruler( window );
   show_ruler( window );
   return( rc );
}



int  next_line( WINDOW *window )
{
register int rcol;
register WINDOW *win;   
int  rc;

   win = window;
   rc = prepare_move_down( win );
   rcol = first_non_blank( win->ll->line, win->ll->len );
   check_virtual_col( win, rcol, win->ccol );
   sync( win );
   make_ruler( win );
   show_ruler( win );
   return( rc );
}



int  home( WINDOW *window )
{
register int rcol;
register WINDOW *win;   
text_ptr p;

   win = window;
   if (g_status.copied && win->file_info == g_status.current_window->file_info){
      rcol = first_non_blank( (text_ptr)g_status.line_buff,
                                        g_status.line_buff_len );
      if (is_line_blank( (text_ptr)g_status.line_buff, g_status.line_buff_len))
         rcol = 0;
   } else {
      p = win->ll->line;
      if (p == NULL)
         rcol = 0;
      else {
         rcol = first_non_blank( p, win->ll->len );
         if (is_line_blank( p, win->ll->len ))
            rcol = 0;
      }
   }
   if (win->rcol == rcol)
      rcol = 0;
   check_virtual_col( win, rcol, win->ccol );
   sync( win );
   make_ruler( win );
   show_ruler( win );
   return( OK );
}



int  goto_eol( WINDOW *window )
{
register int rcol;
register WINDOW *win;   

   win = window;
   rcol = find_end( win->ll->line, win->ll->len );
   if (g_status.copied) {
      if (win->file_info == g_status.current_window->file_info)
         rcol = find_end( (text_ptr)g_status.line_buff, g_status.line_buff_len);
   }
   win->ccol = win->start_col + rcol - win->bcol;
   check_virtual_col( win, rcol, win->ccol );
   sync( win );
   make_ruler( win );
   show_ruler( win );
   return( OK );
}



int  goto_top( WINDOW *window )
{
register WINDOW *win;   

   win = window;
   entab_linebuff( );
   if (un_copy_line( win->ll, win, TRUE ) == ERROR)
      return( ERROR );
   update_line( win );
   for (; win->cline > win->top_line+win->ruler; win->cline--,win->rline--) {
      if (win->rline <= 1L)
         break;
      else {
         win->ll = win->ll->prev;
         win->bin_offset -= win->ll->len;
      }
   }
   show_curl_line( win );
   sync( win );
   return( OK );
}



int  goto_bottom( WINDOW *window )
{
register WINDOW *win;   
int  at_top;

   win = window;
   entab_linebuff( );
   if (un_copy_line( win->ll, win, TRUE ) == ERROR)
      return( ERROR );
   if (win->ll->len == EOF) {
      if (win->rline > 1) {
         at_top = FALSE;
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
      }
   } else {
      update_line( win );
      for (; win->cline < win->bottom_line; win->cline++,win->rline++) {
         if (win->ll == NULL || win->ll->next == NULL || win->ll->next->len == EOF)
            break;
         else {
            win->bin_offset += win->ll->len;
            win->ll = win->ll->next;
         }
      }
      show_curl_line( win );
   }
   sync( win );
   return( OK );
}



int  set_tabstop( WINDOW *window )
{
char num_str[MAX_COLS]; 
int  tab;               
register int rc;
register file_infos *file;

   itoa( mode.ltab_size, num_str, 10 );
   
   rc = get_name( ed7a, window->bottom_line, num_str, g_display.message_color );
   if (rc == OK   &&  *num_str != '\0') {
      tab = atoi( num_str );
      if (tab < MAX_COLS/2) {
         mode.ltab_size = tab;
         if (mode.inflate_tabs) {
            for (file=g_status.file_list; file != NULL; file=file->next)
               file->dirty = GLOBAL;
         }
      } else {
       
         error( WARNING, window->bottom_line, ed8 );
         rc = ERROR;
      }
   }

   itoa( mode.ptab_size, num_str, 10 );
  
   rc = get_name( ed7b, window->bottom_line, num_str, g_display.message_color );
   if (rc == OK  &&  *num_str != '\0') {
      tab = atoi( num_str );
      if (tab < MAX_COLS/2) {
         mode.ptab_size = tab;
         show_tab_modes( );
         if (mode.inflate_tabs) {
            for (file=g_status.file_list; file != NULL; file=file->next)
               file->dirty = GLOBAL;
         }
      } else {
       
         error( WARNING, window->bottom_line, ed8 );
         rc = ERROR;
      }
   }
   return( rc );
}


void show_line_col( WINDOW *window )
{
int  i;
register int k;
char line_col[20], num[10];
char *hex_digit = "0123456789abcdef";

  
   memset( line_col, ' ', 13 );
   line_col[13] = '\0';

  
   itoa( window->rcol+1, num, 10 );
   i = strlen( num ) - 1;
   for (k=12; i>=0; i--, k--)
      line_col[k] = num[i];

  
   line_col[k--] = ':';

   ltoa( window->rline, num, 10 );
   i = strlen( num ) - 1;
   for (; i>=0; i--, k--)
      line_col[k] = num[i];

  
   s_output( line_col, window->top_line-1, window->end_col-12,
             g_display.head_color );

   strcpy( line_col, " =   " );
   i = window->rcol;
   if (g_status.copied) {
      if (mode.inflate_tabs)
         i = entab_adjust_rcol( (text_ptr)g_status.line_buff,
                                 g_status.line_buff_len, i );
      if (i < g_status.line_buff_len) {
         k = (int)g_status.line_buff[i];
         line_col[2] = *(hex_digit + (k >> 4));
         line_col[3] = *(hex_digit + (k & 0x000f));
         line_col[4] = 'x';
         i = TRUE;
      } else
         i = FALSE;
   } else {
      if (mode.inflate_tabs  &&  window->ll->len != EOF)
         i = entab_adjust_rcol( window->ll->line, window->ll->len, i );
      if (i < window->ll->len) {
         k = (int)window->ll->line[i];
         line_col[2] = *(hex_digit + (k >> 4));
         line_col[3] = *(hex_digit + (k & 0x000f));
         line_col[4] = 'x';
         i = TRUE;
      } else
         i = FALSE;
   }
   s_output( line_col, g_display.mode_line, 58, g_display.mode_color );
   if (i == TRUE)
      c_output( k, 58, g_display.mode_line, g_display.mode_color );


   
   if (window->file_info->crlf == BINARY && !window->vertical) {
      k =  window->ll->line == NULL  ?  0  :  window->rcol;
      memset( line_col, ' ', 7 );
      line_col[7] = '\0';
      s_output( line_col, window->top_line-1, 61, g_display.head_color );
      ltoa( window->bin_offset + k, line_col, 10 );
      s_output( line_col, window->top_line-1, 61, g_display.head_color );
   }
   show_asterisk( window );
}



void show_asterisk( WINDOW *window )
{
   c_output( window->file_info->modified ? '*' : ' ', window->start_col+4,
             window->top_line-1, g_display.head_color );
}



int  toggle_overwrite( WINDOW *arg_filler )
{
   mode.insert = !mode.insert;
   show_insert_mode( );
   set_cursor_size( mode.insert ? g_display.insert_cursor :
                    g_display.overw_cursor );
   return( OK );
}



int  toggle_smart_tabs( WINDOW *arg_filler )
{
   mode.smart_tab = !mode.smart_tab;
   show_tab_modes( );
   return( OK );
}



int  toggle_indent( WINDOW *arg_filler )
{
   mode.indent = !mode.indent;
   show_indent_mode( );
   return( OK );
}



int  set_left_margin( WINDOW *window )
{
register int rc;
char temp[MAX_COLS];

   itoa( mode.left_margin + 1, temp, 10 );
   
   rc = get_name( ed9, window->bottom_line, temp, g_display.message_color );
   if (rc == OK  &&  *temp != '\0') {
      rc = atoi( temp ) - 1;
      if (rc < 0 || rc >= mode.right_margin) {
        
         error( WARNING, window->bottom_line, ed10 );
         rc = ERROR;
      } else {
         mode.left_margin = rc;
         show_all_rulers( );
      }
   }
   return( rc );
}



int  set_right_margin( WINDOW *window )
{
char line_buff[(MAX_COLS+1)*2]; 
register int rc;
int  prompt_line;
char temp[MAX_COLS];

   prompt_line = window->bottom_line;
   save_screen_line( 0, prompt_line, line_buff );
   set_prompt( ed11a, prompt_line );
   rc = get_yn( );
   restore_screen_line( 0, prompt_line, line_buff );
   if (rc != ERROR) {
      mode.right_justify =  rc == A_YES ? TRUE : FALSE;

      itoa( mode.right_margin + 1, temp, 10 );
      
      rc = get_name( ed11, prompt_line, temp, g_display.message_color );
      if (rc == OK  &&  *temp != '\0') {
         rc = atoi( temp ) - 1;
         if (rc <= mode.left_margin || rc > MAX_LINE_LENGTH) {
           
            error( WARNING, prompt_line, ed12 );
            rc = ERROR;
         } else {
            mode.right_margin = rc;
            show_all_rulers( );
         }
      }
   }
   return( rc );
}



int  set_paragraph_margin( WINDOW *window )
{
register int rc;
char temp[80];

   itoa( mode.parg_margin + 1, temp, 10 );
   
   rc = get_name( ed13, window->bottom_line, temp, g_display.message_color );
   if (rc == OK  &&  *temp != '\0') {
      rc = atoi( temp ) - 1;
      if (rc < 0 || rc >= mode.right_margin) {
         /*
          * paragraph margin out of range
          */
         error( WARNING, window->bottom_line, ed14 );
         rc = ERROR;
      } else {
         mode.parg_margin = rc;
         show_all_rulers( );
      }
   }
   return( rc );
}



int  toggle_crlf( WINDOW *window )
{
register WINDOW *w;

   ++window->file_info->crlf;
   if (window->file_info->crlf > BINARY)
      window->file_info->crlf = CRLF;
   w = g_status.window_list;
   while (w != NULL) {
      if (w->file_info == window->file_info  &&  w->visible)
         show_crlf_mode( w );
      w = w->next;
   }
   return( OK );
}



int  toggle_ww( WINDOW *arg_filler )
{
   ++mode.word_wrap;
   if (mode.word_wrap > DYNAMIC_WRAP)
      mode.word_wrap = NO_WRAP;
   show_wordwrap_mode( );
   return( OK );
}


int  toggle_trailing( WINDOW *arg_filler )
{
   mode.trailing = !mode.trailing;
   show_trailing( );
   return( OK );
}



int  toggle_z( WINDOW *arg_filler )
{
   mode.control_z = !mode.control_z;
   show_control_z( );
   return( OK );
}



int  toggle_eol( WINDOW *arg_filler )
{
register file_infos *file;

   mode.show_eol = !mode.show_eol;
   for (file=g_status.file_list; file != NULL; file=file->next)
      file->dirty = GLOBAL;
   return( OK );
}



int  toggle_search_case( WINDOW *arg_filler )
{
   mode.search_case = (mode.search_case == IGNORE) ? MATCH : IGNORE;
   show_search_case( );
   build_boyer_array( );
   return( OK );
}



int  toggle_sync( WINDOW *arg_filler )
{
   mode.sync = !mode.sync;
   show_sync_mode( );
   return( OK );
}



int  toggle_ruler( WINDOW *arg_filler )
{
register WINDOW *wp;

   mode.ruler = !mode.ruler;
   wp = g_status.window_list;
   while (wp != NULL) {
      if (mode.ruler) {
        
         if (wp->bottom_line - wp->top_line >0) {
            if (wp->cline == wp->top_line)
               ++wp->cline;
            if (wp->cline > wp->bottom_line)
               wp->cline = wp->bottom_line;
            wp->ruler = TRUE;
         } else
            wp->ruler = FALSE;
      } else {

      
         if (wp->rline == ((wp->cline - wp->ruler) - (wp->top_line - 1)))
            --wp->cline;
         if (wp->cline < wp->top_line)
            wp->cline = wp->top_line;
         wp->ruler = FALSE;
      }
      make_ruler( wp );
      setup_window( wp );
      if (wp->visible)
         redraw_current_window( wp );
      wp = wp->next;
   }
   return( OK );
}



int  toggle_tabinflate( WINDOW *arg_filler )
{
register file_infos *file;

   mode.inflate_tabs = !mode.inflate_tabs;
   for (file=g_status.file_list; file != NULL; file=file->next)
      file->dirty = GLOBAL;
   show_tab_modes( );
   return( OK );
}



void sync( WINDOW *window )
{
register WINDOW *wp;
register file_infos *fp;

   if (mode.sync && mode.sync_sem) {

   
#if defined( __MSC__ )
      switch (g_status.command) {
         case  NextLine        :
         case  BegNextLine     :
         case  LineDown        :
         case  LineUp          :
         case  WordRight       :
         case  WordLeft        :
         case  ScreenDown      :
         case  ScreenUp        :
         case  EndOfFile       :
         case  TopOfFile       :
         case  BotOfScreen     :
         case  TopOfScreen     :
         case  JumpToLine      :
         case  CenterWindow    :
         case  CenterLine      :
         case  ScrollDnLine    :
         case  ScrollUpLine    :
         case  PanUp           :
         case  PanDn           :
         case  NextDirtyLine   :
         case  PrevDirtyLine   :
         case  ParenBalance    :
            assert( g_status.copied == FALSE );
            break;
         default  :
            break;
      }
#endif

      mode.sync_sem = FALSE;
      for (wp = g_status.window_list;  wp != NULL;  wp = wp->next) {
         if (wp->visible  &&  wp != window) {

         
#if defined( __MSC__ )
            assert( wp != NULL );
            assert( wp->file_info != NULL );
            assert( wp->file_info->line_list != NULL );
            assert( wp->file_info->line_list_end != NULL );
            assert( wp->file_info->line_list_end->len == EOF );
            assert( wp->visible == TRUE );
            assert( wp->rline >= 0 );
            assert( wp->rline <= wp->file_info->length + 1 );
            assert( wp->rcol >= 0 );
            assert( wp->rcol < MAX_LINE_LENGTH );
            assert( wp->ccol >= wp->start_col );
            assert( wp->ccol <= wp->end_col );
            assert( wp->bcol >= 0 );
            assert( wp->bcol < MAX_LINE_LENGTH );
            assert( wp->bcol == wp->rcol-(wp->ccol - wp->start_col) );
            assert( wp->start_col >= 0 );
            assert( wp->start_col < wp->end_col );
            assert( wp->end_col < g_display.ncols );
            assert( wp->cline >= wp->top_line );
            assert( wp->cline <= wp->bottom_line );
            assert( wp->top_line > 0 );
            assert( wp->top_line <= wp->bottom_line );
            assert( wp->bottom_line < MAX_LINES );
            assert( wp->bin_offset >= 0 );
            if (wp->ll->next == NULL)
               assert( wp->ll->len == EOF );
            else
               assert( wp->ll->len >= 0 );
            assert( wp->ll->len <  MAX_LINE_LENGTH );
#endif

            (*do_it[g_status.command])( wp );
            show_line_col( wp );
            show_ruler_pointer( wp );
         }
      }
      mode.sync_sem = TRUE;
      for (fp = g_status.file_list; fp != NULL; fp = fp->next)
         if (fp->dirty != FALSE)
            fp->dirty = GLOBAL;
   }
}


/*
 * 作用: 建立起编辑器的结构，并且在需要的时候显示变化
 */
void editor( )
{
char *name;  
register WINDOW *window;        
int  c;

   /*
    * 初始化搜索结构
    */
   g_status.sas_defined = FALSE;
   for (c=0; c<SAS_P; c++)
      g_status.sas_arg_pointers[c] = NULL;

   g_status.file_mode = TEXT;
   /*
    * 检查用户是否指定了具体的文件来编辑，如果没有提供帮助
    */
   if (g_status.argc > 1) {
      c = *g_status.argv[1];
      if (c == '/'  ||  c == '-') {
         c = *(g_status.argv[1] + 1);
         if (c == 'f'  ||  c == 'g') {
            /*
             * 如果是搜索文件，那么用户在命令参数中至少要提供4个参数
             * 比如   editor -f findme *.c
             */
            if (g_status.argc >= 4) {

               assert( strlen( g_status.argv[2] ) < MAX_COLS );

               if (c == 'f') {
                  g_status.command = DefineGrep;
                  strcpy( (char *)sas_bm.pattern, g_status.argv[2] );
               } else {
                  g_status.command = DefineRegXGrep;
                  strcpy( (char *)regx.pattern, g_status.argv[2] );
               }

               for (c=3; c <= g_status.argc; c++)
                  g_status.sas_arg_pointers[c-3] = g_status.argv[c];
               g_status.sas_argc = g_status.argc - 3;
               g_status.sas_arg = 0;
               g_status.sas_argv = g_status.sas_arg_pointers;
               g_status.sas_found_first = FALSE;
               if (g_status.command == DefineGrep) {
                  g_status.sas_defined = TRUE;
                  g_status.sas_search_type = BOYER_MOORE;
                  bm.search_defined = sas_bm.search_defined = OK;
                  build_boyer_array( );
                  c = OK;
               } else {
                  c = build_nfa( );
                  if (c == OK) {
                     g_status.sas_defined = TRUE;
                     g_status.sas_search_type = REG_EXPRESSION;
                     regx.search_defined = sas_regx.search_defined = OK;
                  } else
                     g_status.sas_defined = FALSE;
               }
               if (c != ERROR)
                  c = search_and_seize( g_status.current_window );
            } else
               c = ERROR;
         } else if (c == 'b' || c == 'B') {
            c = atoi( g_status.argv[1] + 2 );
            if (c <= 0 || c >= MAX_LINE_LENGTH)
               c = DEFAULT_BIN_LENGTH;
            ++g_status.arg;
            g_status.file_mode = BINARY;
            g_status.file_chunk = c;
            c = edit_next_file( g_status.current_window );
         } else
            c = ERROR;
      } else
         c = edit_next_file( g_status.current_window );
   } else {
      name = g_status.rw_name;
      *name = '\0';
      /*
       * 要编辑的文件名
       */
      c = get_name( ed15, g_display.nlines, name, g_display.text_color );

      assert( strlen( name ) < MAX_COLS );

      if (c == OK) {
         if (*name != '\0')
            c = attempt_edit_display( name, GLOBAL, TEXT, 0 );
         else
            c = dir_help( (WINDOW *)NULL );
      }
   }

   g_status.stop =   c == OK  ?  FALSE  :  TRUE;
   if (c == OK)
      set_cursor_size( mode.insert ? g_display.insert_cursor :
                       g_display.overw_cursor );

   /*
    * 主循环：在用户结束文档编辑前，处理用户的编辑命令
	* 并且负责在视图中反映变化
    */
   for (; g_status.stop != TRUE;) {
      window = g_status.current_window;


      /*
       * 在处理任何编辑命令前，检测一些参数的设置
       */
      assert( window != NULL );
      assert( window->file_info != NULL );
      assert( window->file_info->line_list != NULL );
      assert( window->file_info->line_list_end != NULL );
      assert( window->file_info->line_list_end->len == EOF );
      assert( window->visible == TRUE );
      assert( window->rline >= 0 );
      assert( window->rline <= window->file_info->length + 1 );
      assert( window->rcol >= 0 );
      assert( window->rcol < MAX_LINE_LENGTH );
      assert( window->ccol >= window->start_col );
      assert( window->ccol <= window->end_col );
      assert( window->bcol >= 0 );
      assert( window->bcol < MAX_LINE_LENGTH );
      assert( window->bcol == window->rcol-(window->ccol - window->start_col) );
      assert( window->start_col >= 0 );
      assert( window->start_col < window->end_col );
      assert( window->end_col < g_display.ncols );
      assert( window->cline >= window->top_line );
      assert( window->cline <= window->bottom_line );
      assert( window->top_line > 0 );
      assert( window->top_line <= window->bottom_line );
      assert( window->bottom_line < MAX_LINES );
      assert( window->bin_offset >= 0 );
      if (window->ll->next == NULL)
         assert( window->ll->len == EOF );
      else
         assert( window->ll->len >= 0 );
      assert( window->ll->len <  MAX_LINE_LENGTH );

      display_dirty_windows( window );

      /*
       * 在处理编辑命令前，把错误处理器的状态设置成一个已知的状态。
       */
      ceh.flag = OK;



      /*
       * 得到用户的输入。查找赋予用户输入的这个键的命令功能。
       * 所有普通文本的输入都赋予了功能0，这些文本包括ASCII码和扩展ASCII码
       */
      g_status.key_pressed = getkey( );
      g_status.command = getfunc( g_status.key_pressed );
      if (g_status.wrapped  ||  g_status.key_pending) {
         g_status.key_pending = FALSE;
         g_status.wrapped = FALSE;
         show_search_message( CLR_SEARCH, g_display.mode_color );
      }
      g_status.control_break = FALSE;
      if (g_status.command >= 0 && g_status.command < NUM_FUNCS) {
         record_keys( window->bottom_line );
         (*do_it[g_status.command])( window );
      }
   }
   cls( );
   xygoto( 0, 0 );
}



void display_dirty_windows( WINDOW *window )
{
register WINDOW *below;         
register WINDOW *above;         
file_infos *file;               

   
   above = below = window;
   while (above->prev || below->next) {
      if (above->prev) {
         above = above->prev;
         show_dirty_window( above );
      }
      if (below->next) {
         below = below->next;
         show_dirty_window( below );
      }
   }
   file = window->file_info;
   if (file->dirty == LOCAL || file->dirty == GLOBAL)
      display_current_window( window );
   for (file=g_status.file_list; file != NULL; file=file->next)
      file->dirty = FALSE;

  
   xygoto( window->ccol, window->cline );
   show_line_col( window );
   show_ruler_pointer( window );
}




void show_dirty_window( WINDOW *window )
{
register WINDOW *win;   
int  dirty;

  win = window;
  if (win->visible) {
     dirty = win->file_info->dirty;
     if (dirty == GLOBAL || dirty == NOT_LOCAL) {
        display_current_window( win );
        show_size( win );
     }
     show_asterisk( win );
  }
}
