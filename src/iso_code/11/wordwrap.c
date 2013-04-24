

#include "tdestr.h"     
#include "common.h"     
#include "define.h"
#include "tdefunc.h"



int  find_left_margin( line_list_ptr ll, int wrap_mode )
{
register int lm;
int  len;
text_ptr source;

   if (wrap_mode == FIXED_WRAP) {
    
      if (g_status.copied) {
         source = (text_ptr)g_status.line_buff;
         len    = g_status.line_buff_len;
      } else {
         if (ll->prev != NULL) {
            source = ll->prev->line;
            len    = ll->prev->len;
         } else {
            source = NULL;
            len    = 0;
         }
      }
      if (source == NULL)
         lm = mode.parg_margin;
      else if (find_end( source, len ) == 0)
         lm = mode.parg_margin;
      else
         lm = mode.left_margin;
   } else {
     
      if (g_status.copied == TRUE) {
         source = (text_ptr)g_status.line_buff;
         len    = g_status.line_buff_len;
      } else {
         source = ll->line;
         len    = ll->len;
      }
      lm = first_non_blank( source, len );
      if (is_line_blank( source, len ) && ll->prev != NULL) {
         for (ll=ll->prev; ll != NULL; ll=ll->prev) {
            lm = first_non_blank( ll->line, ll->len );
            if (!is_line_blank( ll->line, ll->len ))
               break;
         }
      }
   }
   return( lm );
}



void word_wrap( WINDOW *window )
{
int  c;                 
register int len;       
int  i;                 
line_list_ptr p;        
int  rcol;
int  lm;
int  rm;
int  side;
register WINDOW *win;          

   win = window;

  
   c = g_status.key_pressed;
   rcol = win->rcol;
   copy_line( win->ll );
   detab_linebuff( );

  
   side = 1;
   p = win->ll->prev;
   while (p != NULL  &&  !is_line_blank( p->line, p->len )) {
      ++side;
      p = p->prev;
   }
   side = (side & 1) ? RIGHT : LEFT;


  
   p = win->ll->prev;

   lm = find_left_margin( win->ll, mode.word_wrap );
   rm = mode.right_margin;

  
   len = g_status.line_buff_len;
   if (rcol > rm+1 && c != ' ') {

     
      if ((p == NULL || is_line_blank( p->line, p->len )) &&
           first_non_blank( (text_ptr)g_status.line_buff,
                 g_status.line_buff_len ) > rm && mode.word_wrap == FIXED_WRAP)
         lm = mode.parg_margin;

      
      for (i=rcol-1; i > lm  &&  g_status.line_buff[i] != ' '; )
         i--;
      if (i > lm) {
         i++;
         win->rcol = i;
         g_status.command = WordWrap;
         insert_newline( win );
         if (mode.right_justify == TRUE)
            justify_right_margin( win, win->ll->prev,
                 mode.word_wrap == FIXED_WRAP ? find_left_margin( win->ll->prev,
                 mode.word_wrap ) : lm, rm, side );

         
         win->rcol = lm + rcol - i;
         check_virtual_col( win, win->rcol, win->rcol );

        

         len = find_end( win->ll->line, win->ll->len );
         if (len < rm+1)
            combine_wrap_spill( win, len, lm, rm, side, FALSE );
      }
   } else if (len > rm+1) {

      
      if (c == ' ' && rcol > rm) {
         for (i=rcol; i<len && g_status.line_buff[i] == ' ';)
            i++;

       
         if (i < len)
            combine_wrap_spill( win, i, lm, rm, side, TRUE );

      } else if (g_status.line_buff[rm+1] != ' ') {

        
         for (i=rm+1; i > lm  &&  g_status.line_buff[i] != ' '; )
            i--;

        
         if (i > lm) {
            i++;

            
            if (i > rcol) {
               combine_wrap_spill( win, i, lm, rm, side, TRUE );

           
            } else if (i <= rcol) {
               win->rcol = i;
               g_status.command = WordWrap;
               insert_newline( win );
               if (mode.right_justify == TRUE)
                  justify_right_margin( win, win->ll->prev,
                   mode.word_wrap == FIXED_WRAP ?
                   find_left_margin( win->ll->prev, mode.word_wrap ) : lm,
                   rm, side );
               win->rcol = lm + rcol - i;
               check_virtual_col( win, win->rcol, win->rcol );
               len = find_end( win->ll->line, win->ll->len );
               if (len < rm+1)
                  combine_wrap_spill( win, len, lm, rm, side, FALSE );
            }
         }

         
      } else {

        
         for (i=rm+1; i<len && g_status.line_buff[i] == ' '; )
            i++;

        
         if (i != len)
            combine_wrap_spill( win, i, lm, rm, side, TRUE );
      }
   }
}



int  format_paragraph( WINDOW *window )
{
register int len;       
int  first_line;        
int  spaces;            
line_list_ptr p;        
line_list_ptr pp;
char *source;           
char *dest;
int  rcol;              
int  lm;
int  rm;
int  pm;
int  margin;
int  eop;               
int  old_ww;            
long rline;
WINDOW w;               

   if (window->ll->len == EOF)
      return( ERROR );
   entab_linebuff( );
   if (un_copy_line( window->ll, window, TRUE ) == ERROR)
      return( ERROR );
   if (!is_line_blank( window->ll->line, window->ll->len )) {
      old_ww = mode.word_wrap;
      if (old_ww == NO_WRAP)
         mode.word_wrap = FIXED_WRAP;
      dup_window_info( &w, window );
      g_status.screen_display = FALSE;

      
      p = w.ll->prev;
      if (g_status.command == FormatParagraph) {
         while (p != NULL &&  !is_line_blank( p->line, p->len )) {
            --w.rline;
            w.ll = w.ll->prev;
            p = p->prev;
         }
         pm = mode.parg_margin;

      
      } else if (g_status.command == FormatText) {
         if (p == NULL || is_line_blank( p->line, p->len ))
            pm = mode.parg_margin;
         else
            pm = mode.left_margin;
      } else
         pm = mode.left_margin;

      g_status.command = WordWrap;
      p = w.ll;
      if (mode.word_wrap == FIXED_WRAP)
         lm = mode.left_margin;
      else
         lm = pm = find_left_margin( p, mode.word_wrap );
      rm = mode.right_margin;
      eop = FALSE;

     
      for (first_line=TRUE; p != NULL  &&  !is_line_blank( p->line, p->len ) &&
                            eop == FALSE  &&  !g_status.control_break;) {

         
         if (first_line) {
            margin = pm;
            first_line = FALSE;
         } else
            margin = lm;

         
         w.ll->dirty = TRUE;
         copy_line( w.ll );
         detab_linebuff( );
         remove_spaces( 0 );
         rcol = find_word( (text_ptr)g_status.line_buff,
                                     g_status.line_buff_len, 0 );
         if (rcol != ERROR && rcol != margin) {

           
            if (rcol < margin) {
               source = g_status.line_buff;
               spaces = margin - rcol;
               dest = source + spaces;

               assert( g_status.line_buff_len >= 0 );
               assert( g_status.line_buff_len < MAX_LINE_LENGTH );

               memmove( dest, source, g_status.line_buff_len );
               g_status.line_buff_len += spaces;
               while (spaces--)
                  *source++ = ' ';
            } else {
               w.rcol = margin;
               word_delete( &w );
               entab_linebuff( );
               un_copy_line( p, &w, TRUE );
               copy_line( w.ll );
               detab_linebuff( );
               remove_spaces( margin );
            }
         }

         

         source = g_status.line_buff;
         len = g_status.line_buff_len;
         for (; len < rm+1 && eop == FALSE;) {
            pp = p->next;
            if (is_line_blank( pp->line, pp->len ))
               eop = TRUE;
            else {
               w.ll = p;
               w.rcol = len + 1;
               if (*(p->line+len-1) == '.')
                  ++w.rcol;
               word_delete( &w );
               entab_linebuff( );
               un_copy_line( p, &w, TRUE );
               copy_line( p );
               detab_linebuff( );
               remove_spaces( margin );
               len = g_status.line_buff_len;
            }
         }
         if (len <= rm+1) {
            entab_linebuff( );
            un_copy_line( p, &w, TRUE );
            p = p->next;
            if (is_line_blank( p->line, p->len ))
               eop = TRUE;
            else {
               w.ll = w.ll->next;
               w.rline++;
            }
         } else {
            w.rcol = rm;
            g_status.key_pressed = *(w.ll->line + rm);
            rline = w.rline;
            word_wrap( &w );
            if (rline == w.rline) {
               w.ll = w.ll->next;
               ++w.rline;
            }
         }
         g_status.copied = FALSE;
         p = w.ll;
      }
      mode.word_wrap = old_ww;
      g_status.copied = FALSE;
      w.file_info->dirty = GLOBAL;
      g_status.screen_display = TRUE;
   }
   return( OK );
}



void combine_wrap_spill( WINDOW *window, int wrap_col, int lm, int rm,
                         int side, int new_line )
{
line_list_ptr p;        
line_list_ptr pp;       
int  p_len;             
int  non_blank;         
int  control_t;         
int  next_line_len;     
WINDOW w;               

   dup_window_info( &w, window );
   g_status.command = WordWrap;
   w.rcol = wrap_col;
   if (new_line) {
      insert_newline( &w );
      if (mode.right_justify == TRUE)
         justify_right_margin( &w, w.ll->prev, mode.word_wrap == FIXED_WRAP ?
                find_left_margin( w.ll->prev, mode.word_wrap ) : lm, rm, side );
      p = window->ll->next;
   } else
      p = window->ll;
   if (p != NULL) {
      p_len = find_end( p->line, p->len );
      pp = p->next;
      if (pp != NULL) {
         non_blank = first_non_blank( pp->line, pp->len );
         next_line_len = find_end( pp->line, pp->len ) - non_blank;
         if (!is_line_blank( pp->line, pp->len ) && p_len + next_line_len <= rm) {
            control_t = 1;
            if (mode.inflate_tabs) {
               if (*pp->line == ' '  ||  *pp->line == '\t')
                  ++control_t;
            } else if (*pp->line == ' ')
               ++control_t;
            w.ll = p;
            w.rcol = p_len + 1;
            if (*(p->line+p_len-1) == '.')
               ++w.rcol;
            while (control_t--)
               word_delete( &w );
            remove_spaces( lm );
            un_copy_line( w.ll, &w, TRUE );
         }
         window->file_info->dirty = GLOBAL;
      }
   }
}



void justify_right_margin( WINDOW *window, line_list_ptr ll, int lm, int rm,
                           int side )
{
int  len;
int  i;
int  word_count;
int  holes;
int  nb;
int  spaces;
text_ptr s;

  
   len = find_end( ll->line, ll->len );
   if (len <= lm || len >= rm+1)
      return;

  
   i = entab_adjust_rcol( ll->line, ll->len, lm );
   s = ll->line + i;
   len -= i;
   word_count = 0;
   while (len > 0) {
      while (len-- > 0  &&  *s++ == ' ');
      if (len == 0)
         break;
      ++word_count;
      while (len-- > 0  &&  *s++ != ' ');
   }

   
   if (word_count <= 1)
      return;

   holes = word_count - 1;
   copy_line( ll );
   detab_linebuff( );
   remove_spaces( lm );

  
   i = g_status.line_buff_len - 1;
   spaces = rm - i;
   if (spaces <= 0)
      return;
   g_status.line_buff_len += spaces;

  
   while (i < rm) {
      g_status.line_buff[rm] = g_status.line_buff[i];
      if (g_status.line_buff[rm] == ' ') {

       
         while (g_status.line_buff[i-1] == ' ')
            g_status.line_buff[--rm] = g_status.line_buff[--i];

         nb = side == LEFT ? spaces  holes + 1;
         spaces -= nb;
         --holes;
         while (nb-- > 0)
            g_status.line_buff[--rm] = ' ';
      }
      --i;
      --rm;
   }
   entab_linebuff( );
   un_copy_line( ll, window, window->bottom_line );
}



void remove_spaces( int lm )
{
int  period;
int  len;
int  i;
int  c;
char *s;
char *d;

   if ((i = len = g_status.line_buff_len) <= lm)
      return;

   period = FALSE;
   s = d = g_status.line_buff + lm;
   i -= lm;
   c = (int)*s++;
   while (c == ' ' && i > 0) {
      c = *s++;
      --i;
      --len;
   }
   period = c == '.' ? TRUE : FALSE;
   while (i > 0) {
      *d++ = (char)c;
      c = (int)*s++;
      --i;
      if (c != ' ')
         period =  c == '.' ? TRUE : FALSE;
      else {
         *d++ = (char)c;
         c = (int)*s++;
         --i;
         if (period  &&  c == ' ') {
            *d++ = (char)c;
            period = FALSE;
            if (i > 0)
               ++len;
         }
         while (c == ' '  &&  i > 0) {
            c = (int)*s++;
            --len;
            --i;
         }
      }
   }
   *d = (char)c;
   g_status.line_buff_len = len;
}



int  find_word( text_ptr p, int len, int start_col )
{
register int rc;
register char c;

   if (len <= start_col  ||  len < 0  || start_col < 0)
      return( ERROR );
   p += start_col;
   rc = start_col;

   if (mode.inflate_tabs) {
      while (len-- > 0 && ((c = *p++) == ' ' || c == '\t'))
         if (c != '\t')
            ++rc;
         else
            rc += mode.ptab_size - (rc % mode.ptab_size);
   } else
      while (len-- > 0  &&  (c = *p++) == ' ')
         ++rc;
   if (len <= 0)
     rc = ERROR;
   return( rc );
}



int  flush_left( WINDOW *window )
{
int  len;       
register int spaces;
char *source;
char *dest;
int  rcol;
int  lm;
register WINDOW *win;          

   win = window;
   copy_line( win->ll );
   detab_linebuff( );
   lm = mode.left_margin;
   rcol = find_word( (text_ptr)g_status.line_buff, g_status.line_buff_len, 0 );
   if (rcol != ERROR && rcol != lm) {

     
      if (rcol < lm) {
         source = g_status.line_buff;
         spaces = lm - rcol;
         dest = source + spaces;
         len = g_status.line_buff_len;
         if (len + spaces > MAX_LINE_LENGTH) {
            
            error( WARNING, win->bottom_line, ww1 );
            return( ERROR );
         } else {
            load_undo_buffer( win->file_info, win->ll->line, win->ll->len );

            assert( len >= 0 );
            assert( len < MAX_LINE_LENGTH );

            memmove( dest, source, len );
            g_status.line_buff_len += spaces;
            while (spaces--)
               *source++ = ' ';
            win->file_info->dirty = GLOBAL;
         }

    
      } else {
         dest = g_status.line_buff + lm;
         source = g_status.line_buff + rcol;

         assert( g_status.line_buff_len - rcol >= 0 );
         assert( g_status.line_buff_len - rcol < MAX_LINE_LENGTH );

         memmove( dest, source, g_status.line_buff_len - rcol );
         g_status.line_buff_len -= (rcol - lm);
         win->file_info->dirty = GLOBAL;
      }
      win->ll->dirty = TRUE;
      show_changed_line( win );
   }
   return( OK );
}



int  flush_right( WINDOW *window )
{
int  len;               
int  i;
int  spaces;
char *source;
char *dest;
register int rcol;
int  rm;
register WINDOW *win;   

   win = window;
   copy_line( win->ll );
   detab_linebuff( );
   source = g_status.line_buff;
   len = g_status.line_buff_len;
   if (!is_line_blank( (text_ptr)source, len )) {
      rm = mode.right_margin;
      for (rcol=len-1; rcol>=0 && *(source+rcol) == ' ';)
         rcol--;
      if (rcol != rm) {

       
         if (rcol < rm) {
            spaces = rm - rcol;
            dest = source + spaces;
            if (len + spaces > MAX_LINE_LENGTH) {
              
               error( WARNING, win->bottom_line, ww1 );
               return( ERROR );
            } else {
               load_undo_buffer( win->file_info, win->ll->line, win->ll->len );

               assert( len >= 0 );
               assert( len < MAX_LINE_LENGTH );

               memmove( dest, source, len );
               g_status.line_buff_len += spaces;
               while (spaces--)
                  *source++ = ' ';
               win->file_info->dirty = GLOBAL;
            }

        
         } else {
            load_undo_buffer( win->file_info, win->ll->line, win->ll->len );
            rcol = rcol - rm;
            i = first_non_blank( (text_ptr)source, len );
            if (rcol > i)
               rcol = i;
            dest = source + rcol;

            assert( len - rcol >= 0 );
            assert( len - rcol < MAX_LINE_LENGTH );

            memmove( source, dest, len - rcol );
            g_status.line_buff_len -= (rcol - rm);
            win->file_info->dirty = GLOBAL;
         }
         win->ll->dirty = TRUE;
         show_changed_line( win );
      }
   }
   return( OK );
}



int  flush_center( WINDOW *window )
{
int  len;               
char *source;           
char *dest;
int  rm;
int  lm;
register int spaces;    
int  center;            
int  first;             
int  last;              
register WINDOW *win;   

   win = window;
   copy_line( win->ll );
   detab_linebuff( );
   source = g_status.line_buff;
   len = g_status.line_buff_len;
   if (!is_line_blank( (text_ptr)source, len )) {
      rm = mode.right_margin;
      lm = mode.left_margin;
      center = (rm + lm) / 2;
      first = first_non_blank( (text_ptr)source, len );
      for (last=len-1; last>=0 && *(source+last) == ' ';)
         last--;
      spaces = last + first - 1;
      spaces = (spaces / 2) + (spaces & 1);
      if (spaces != center) {

         
         if (spaces < center) {
            spaces = center - spaces;
            dest = source + spaces;
            if (len + spaces > MAX_LINE_LENGTH) {
              
               error( WARNING, win->bottom_line, ww1 );
               return( ERROR );
            } else {
               load_undo_buffer( win->file_info, win->ll->line, win->ll->len );

               assert( len >= 0 );
               assert( len < MAX_LINE_LENGTH );

               memmove( dest, source, len );
               g_status.line_buff_len += spaces;
               while (spaces--)
                  *source++ = ' ';
               win->file_info->dirty = GLOBAL;
            }

         
         } else {
            load_undo_buffer( win->file_info, win->ll->line, win->ll->len );
            spaces = spaces - center;
            if (spaces > first)
               spaces = first;
            dest = source + spaces;

            assert( len - spaces >= 0 );
            assert( len - spaces < MAX_LINE_LENGTH );

            memmove( source, dest, len - spaces );
            g_status.line_buff_len -= spaces;
            win->file_info->dirty = GLOBAL;
         }
         win->ll->dirty = TRUE;
         show_changed_line( win );
      }
   }
   return( OK );
}
