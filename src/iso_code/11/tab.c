
#include "tdestr.h"
#include "common.h"
#include "tdefunc.h"
#include "define.h"



int  tab_key( WINDOW *window )
{
int  spaces;    
char *source;   
char *dest;     
int  pad;
int  len;
register int rcol;
int  old_bcol;
register WINDOW *win;   
int  rc;

   win  = window;
   if (win->ll->len  ==  EOF)
      return( OK );
   rcol = win->rcol;
   old_bcol = win->bcol;
   show_ruler_char( win );
  
   if (mode.smart_tab)
      spaces = next_smart_tab( win );
   else
      spaces = mode.ltab_size - (rcol % mode.ltab_size);

   assert( spaces >= 0 );
   assert( spaces < MAX_LINE_LENGTH );

   rc = OK;
   if (mode.insert && rcol + spaces < g_display.line_length) {
      copy_line( win->ll );
      detab_linebuff( );

     
      len = g_status.line_buff_len;
      pad = rcol > len ? rcol - len : 0;
      if (len + pad + spaces >= g_display.line_length) {
         
         error( WARNING, win->bottom_line, ed1 );
         rc = ERROR;
         g_status.copied = FALSE;
      } else {
         if (pad > 0  || spaces > 0) {
            source = g_status.line_buff + rcol - pad;
            dest = source + pad + spaces;

            assert( len + pad - rcol >= 0 );
            assert( len + pad - rcol < MAX_LINE_LENGTH );

            memmove( dest, source, len + pad - rcol );

           

            assert( pad + spaces >= 0 );
            assert( pad + spaces < MAX_LINE_LENGTH );

            memset( source, ' ', pad + spaces );
            g_status.line_buff_len += pad + spaces;
            entab_linebuff( );
         }

         win->ll->dirty = TRUE;
         win->file_info->dirty = GLOBAL;
         show_changed_line( win );
         rcol += spaces;
         win->ccol += spaces;
      }
   } else if (rcol + spaces <= g_display.line_length) {
     
      rcol += spaces;
      win->ccol += spaces;
   }
   check_virtual_col( win, rcol, win->ccol );
   if (old_bcol != win->bcol) {
      make_ruler( win );
      show_ruler( win );
   }
   return( rc );
}

  the required distance.
 */
int  backtab( WINDOW *window )
{
int  spaces;    
char *source;   
char *dest;     
int  pad;
int  len;
register int rcol;
int  old_bcol;
register WINDOW *win;   

   win  = window;
   rcol = win->rcol;
   if (win->ll->len == EOF || win->rcol == 0)
      return( OK );
   old_bcol = win->bcol;
   show_ruler_char( win );

 
   if (mode.smart_tab)
      spaces = prev_smart_tab( win );
   else
      spaces = win->rcol % mode.ltab_size;

   if (spaces == 0)
      spaces = mode.ltab_size;
   copy_line( win->ll );
   detab_linebuff( );
   len = g_status.line_buff_len;
   if (mode.insert && rcol - spaces < len) {
      pad = rcol > len ? rcol - len : 0;
      if (pad > 0  || spaces > 0) {
       
         if (pad > 0) {

            assert( rcol - pad >= 0 );
            assert( pad < MAX_LINE_LENGTH );

            source = g_status.line_buff + rcol - pad;
            dest = source + pad;

            assert( pad >= 0 );
            assert( pad < MAX_LINE_LENGTH );

            memmove( dest, source, pad );
            memset( source, ' ', pad );
            g_status.line_buff_len += pad;
         }
         source = g_status.line_buff + rcol;
         dest = source - spaces;

         assert( len + pad - rcol >= 0 );
         assert( len + pad - rcol < MAX_LINE_LENGTH );

         memmove( dest, source, len + pad - rcol );
         g_status.line_buff_len -= spaces;
         entab_linebuff( );
      }

      win->ll->dirty = TRUE;
      win->file_info->dirty = GLOBAL;
      show_changed_line( win );
      rcol -= spaces;
      win->ccol -= spaces;
   } else {
     
      rcol -= spaces;
      if (rcol < 0)
         rcol = 0;
      win->ccol -= spaces;
   }
   check_virtual_col( win, rcol, win->ccol );
   if (old_bcol != win->bcol) {
      make_ruler( win );
      show_ruler( win );
   }
   return( OK );
}



int  next_smart_tab( WINDOW *window )
{
register int spaces;    
text_ptr s;             
line_list_ptr ll;
register WINDOW *win;   
int  len;

   
   win = window;
   ll = win->ll->prev;
   while (ll != NULL  && is_line_blank( ll->line, ll->len ))
      ll = ll->prev;

   if (ll != NULL) {
      s = ll->line;
      
      if (window->rcol >= find_end( s, ll->len ))
         spaces = mode.ltab_size - (window->rcol % mode.ltab_size);
      else {

         len = ll->len;
         s = detab_a_line( s, &len );

         spaces = 0;
         s = s + window->rcol;
         len -= window->rcol;

        
         while (*s != ' '  &&  len > 0) {
            ++s;
            ++spaces;
            --len;
         }

         
         if (len > 0)
            while (*s == ' ' && len > 0) {
               ++s;
               ++spaces;
               --len;
            }
      }
   } else
      spaces = mode.ltab_size - (window->rcol % mode.ltab_size);
   return( spaces );
}



int  prev_smart_tab( WINDOW *window )
{
register int spaces;    
text_ptr s;             
int  len;
line_list_ptr ll;
WINDOW *win;            

  
   win = window;
   ll = win->ll->prev;
   while (ll != NULL  && is_line_blank( ll->line, ll->len ))
      ll = ll->prev;

   if (ll != NULL) {
      s = ll->line;

      
      if (window->rcol < first_non_blank( s, ll->len ))
         spaces = window->rcol % mode.ltab_size;
      else {

         len = ll->len;
         if (mode.inflate_tabs)
            s = detab_a_line( s, &len );

       
         if (len < window->rcol) {
            s += len;
            spaces = window->rcol - len;
         } else {
            len = window->rcol;
            s += window->rcol;
            spaces = 0;
         }

         while (*(s-1) == ' ' && len > 0) {
            --s;
            ++spaces;
            --len;
         }

        
         while (*(s-1) != ' '  &&  len > 0) {
            --s;
            ++spaces;
            --len;
         }
         if (len == 0 && *s == ' ')
            spaces = window->rcol % mode.ltab_size;
         if (spaces > window->rcol)
            spaces = window->rcol;
      }
   } else
      spaces = window->rcol % mode.ltab_size;

  
   if (spaces < 0)
      spaces = 0;
   return( spaces );
}



text_ptr entab( text_ptr s, int len )
{
int  tab_size;
int  last_col;
int  space;
register int col;
text_ptr to;

   assert( s != NULL );
   assert( len >= 0 );
   assert( len < MAX_LINE_LENGTH );

   tab_size = mode.ptab_size;
   to = (text_ptr)g_status.tabout_buff;
   if (s == NULL)
      g_status.tabout_buff_len = 0;
   else {
      g_status.tabout_buff_len = len;
      for (last_col=col=0; ; s++, len--) {
         if (len == 0) {

           
            if (col != last_col) {
               while (last_col < col) {
                  space = tab_size - last_col % tab_size;
                  if (space <= 1) {
                     *to++ = ' ';
                     last_col++;
                  } else if (last_col + space <= col) {
                     *to++ = '\t';
                     last_col += space;
                     g_status.tabout_buff_len -= (space - 1);
                  } else {
                     *to++ = ' ';
                     last_col++;
                  }
               }
            }

            
            break;
         } else if (*s == ' ')
            col++;
         else {
            if (col != last_col) {
               while (last_col < col) {
                  space = tab_size - last_col % tab_size;

                 
                  if (space <= 1) {
                     *to++ = ' ';
                     last_col++;
                  } else if (last_col + space <= col) {
                     *to++ = '\t';
                     last_col += space;
                     g_status.tabout_buff_len -= (space - 1);
                  } else {
                     *to++ = ' ';
                     last_col++;
                  }
               }
            }

           
            if (*s == '\t')
               col = col + tab_size - (col % tab_size);
            else
               ++col;
            last_col = col;
            *to++ = *s;

            if (*s == '\"' || *s == '\'') {
               while (len > 0) {
                 *to++ = *++s;
                 --len;
               }
               break;
            }
         }
      }
   }
   return( (text_ptr)g_status.tabout_buff );
}



void detab_linebuff( void )
{
int  show_eol;
int  len;

   if (mode.inflate_tabs  &&  g_status.copied) {
      len = g_status.line_buff_len;
      show_eol = mode.show_eol;
      mode.show_eol = FALSE;
      tabout( (text_ptr)g_status.line_buff, &len );

      assert( len >= 0 );
      assert( len < MAX_LINE_LENGTH );

      memmove( g_status.line_buff, g_status.tabout_buff, len );
      g_status.line_buff_len = len;
      mode.show_eol = show_eol;
   }
}



void entab_linebuff( void )
{
   if (mode.inflate_tabs  &&  g_status.copied) {
      entab( (text_ptr)g_status.line_buff, g_status.line_buff_len );

      assert( g_status.tabout_buff_len >= 0 );
      assert( g_status.tabout_buff_len < MAX_LINE_LENGTH );

      memmove( g_status.line_buff, g_status.tabout_buff,
                       g_status.tabout_buff_len );
      g_status.line_buff_len = g_status.tabout_buff_len;
   }
}



text_ptr detab_a_line( text_ptr s, int *len )
{
int  show_eol;

   if (mode.inflate_tabs) {

      assert( *len >= 0 );
      assert( *len < MAX_LINE_LENGTH );
      assert( s != NULL );

      show_eol = mode.show_eol;
      mode.show_eol = FALSE;
      s = tabout( s, len );
      mode.show_eol = show_eol;
   }
   return( s );
}



int  detab_adjust_rcol( text_ptr s, int rcol )
{
register int col;

   assert( rcol >= 0 );
   assert( rcol < MAX_LINE_LENGTH );
   assert( s != NULL );
   assert( mode.ptab_size != 0 );

   for (col=0; rcol > 0; rcol--,s++) {
      if (*s == '\t')
         col += (mode.ptab_size - (col % mode.ptab_size));
      else if (rcol > 0)
         col++;
   }

   assert( col >= 0 );
   assert( col < MAX_LINE_LENGTH );

   return( col );
}



int  entab_adjust_rcol( text_ptr s, int len, int rcol )
{
register int col;
register int last_col;

   assert( len >= 0 );
   assert( len < MAX_LINE_LENGTH );
   assert( rcol >= 0 );
   assert( rcol < MAX_LINE_LENGTH );
   assert( mode.ptab_size != 0 );

   if (s != NULL) {
      for (last_col=col=0; col < rcol  &&  s != NULL  &&  len > 0; s++, len--) {
         if (*s != '\t')
            ++col;
         else
            col += (mode.ptab_size - (col % mode.ptab_size));
         if (col > rcol)
            break;
         ++last_col;
      }
   } else
      last_col = rcol;

   assert( last_col >= 0 );
   assert( last_col < MAX_LINE_LENGTH );

   return( last_col );
}



int  block_expand_tabs( WINDOW *window )
{
int  prompt_line;
int  len;
int  tab;
int  tab_size;
int  dirty;
register int spaces;
line_list_ptr p;                
file_infos *file;
WINDOW *sw, s_w;
long er;
int  i;
int  rc;
char *b;

   
   prompt_line = window->bottom_line;

   if (un_copy_line( window->ll, window, TRUE ) == ERROR)
      return( ERROR );
   check_block( );
   rc = OK;
   if (g_status.marked == TRUE) {

      file  = g_status.marked_file;
      if (file->block_type != LINE) {
        
         error( WARNING, prompt_line, block20 );
         return( ERROR );
      }

     
      dirty = FALSE;
      tab_size = mode.ptab_size;
      sw = g_status.window_list;
      for (; ptoul( sw->file_info ) != ptoul( file );)
         sw = sw->next;
      dup_window_info( &s_w, sw );
      p  = file->block_start;
      er = file->block_er;
      s_w.rline = file->block_br;
      s_w.visible = FALSE;
      for (; s_w.rline <= er  &&  !g_status.control_break; s_w.rline++) {

        
         tab = FALSE;
         g_status.copied = FALSE;

         copy_line( p );
         len = g_status.line_buff_len;
         for (b=g_status.line_buff, i=1; len > 0  &&  rc == OK; b++, len--) {

            
            if (*b == '\t') {
               tab = TRUE;
               spaces = i % tab_size;
               if (spaces)
                  spaces = tab_size - spaces;
               if (spaces) {

                  assert( len >= 0 );
                  assert( len < MAX_LINE_LENGTH );

                  memmove( b + spaces, b, len );
               }

               assert( spaces + 1 >= 0 );
               assert( spaces + 1 < MAX_LINE_LENGTH );

               memset( b, ' ', spaces+1 );
               i += spaces + 1;
               b += spaces;
               g_status.line_buff_len += spaces;
            } else
               i++;
         }

         
         if (tab) {
            rc = un_copy_line( p, &s_w, TRUE );
            dirty = TRUE;
         }
         p = p->next;
      }

      
      g_status.copied = FALSE;
      if (dirty)
         file->dirty = GLOBAL;
   }
   return( rc );
}



int  block_compress_tabs( WINDOW *window )
{
register int col;
register int spaces;
int  len;
int  rc;
int  prompt_line;
int  last_col;
int  tab;
int  tab_size;
int  dirty;
line_list_ptr p;                
text_ptr from;                  
file_infos *file;
WINDOW *sw, s_w;
long er;
char *to;
int  indent_only;

   
   prompt_line = window->bottom_line;
   entab_linebuff( );
   if (un_copy_line( window->ll, window, TRUE ) == ERROR)
      return( ERROR );
   check_block( );
   rc = OK;
   if (g_status.marked == TRUE) {

      file  = g_status.marked_file;
      if (file->block_type != LINE) {
       
         error( WARNING, prompt_line, block26 );
         return( ERROR );
      }

      indent_only = g_status.command == BlockIndentTabs ? TRUE : FALSE;

      
      g_status.command = WordWrap;

      
      dirty = FALSE;
      tab_size = mode.ptab_size;
      sw = g_status.window_list;
      for (; ptoul( sw->file_info ) != ptoul( file );)
         sw = sw->next;
      dup_window_info( &s_w, sw );
      s_w.visible = FALSE;
      s_w.ll  =  p  = file->block_start;
      er = file->block_er;
      s_w.rline = file->block_br;
      for (; rc == OK  &&  s_w.rline <= er  &&  !g_status.control_break; s_w.rline++) {

         
         tab = FALSE;

         from = p->line;
         to   = g_status.line_buff;
         g_status.line_buff_len = len  = p->len;

         for (last_col=col=0; ; from++, len--) {
            if (len == 0) {

            
               if (col != last_col) {
                  while (last_col < col) {
                     spaces = tab_size - last_col % tab_size;
                     if (spaces <= 1) {
                        *to++ = ' ';
                        last_col++;
                     } else if (last_col + spaces <= col) {
                        *to++ = '\t';
                        last_col += spaces;
                        g_status.line_buff_len -= (spaces - 1);
                        tab = TRUE;
                     } else {
                        *to++ = ' ';
                        last_col++;
                     }
                  }
               }

             
               break;
            } else if (*from == ' ')
               col++;
            else {
               if (col != last_col) {
                  while (last_col < col) {
                     spaces = tab_size - last_col % tab_size;

                  
                     if (spaces <= 1) {
                        *to++ = ' ';
                        last_col++;
                     } else if (last_col + spaces <= col) {
                        *to++ = '\t';
                        last_col += spaces;
                        g_status.line_buff_len -= (spaces - 1);
                        tab = TRUE;
                     } else {
                        *to++ = ' ';
                        last_col++;
                     }
                  }
               }

               
               if (*from == '\t')
                  col = col + tab_size - (col % tab_size);
               else
                  ++col;
               last_col = col;
               *to++ = *from;

              
               if (*from == '\"' || *from == '\''  || indent_only) {
                  while (len > 0) {
                     *to++ = *++from;
                     --len;
                  }
                  break;
               }
            }
         }

         
         if (tab) {
            g_status.copied = TRUE;
            rc = un_copy_line( p, &s_w, TRUE );
            dirty = TRUE;
         }
         p = p->next;
      }

      
      g_status.copied = FALSE;
      if (dirty)
         file->dirty = GLOBAL;
   }
   return( rc );
}
