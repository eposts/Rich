

#include "tdestr.h"
#include "common.h"
#include "define.h"
#include "tdefunc.h"



int  define_diff( WINDOW *window )
{
int  rc;
char temp[MAX_COLS];
int  num1;
int  let1;
int  num2;
int  let2;
int  start;
char line_buff[(MAX_COLS+1)*2]; 
char buff[MAX_COLS*2];           

  
   *temp = '\0';
   rc = get_name( diff_prompt1, window->bottom_line, temp,
                  g_display.message_color );
   if (rc == OK) {
      rc = verify_number( temp, &num1 );
      if (rc == OK)
         rc = verify_letter( temp, &let1, &diff.w1 );
   } else
      return( ERROR );
   if (rc == ERROR) {
      combine_strings( buff, diff_prompt6a, temp, diff_prompt6b );
      error( WARNING, window->bottom_line, buff );
      return( ERROR );
   }

   *temp = '\0';
   rc = get_name( diff_prompt2, window->bottom_line, temp,
                  g_display.message_color );
   if (rc == OK) {
      rc = verify_number( temp, &num2 );
      if (rc == OK)
         rc = verify_letter( temp, &let2, &diff.w2 );
   } else
      return( ERROR );
   if (rc == ERROR) {
      combine_strings( buff, diff_prompt6a, temp, diff_prompt6b );
      error( WARNING, window->bottom_line, buff );
      return( ERROR );
   }

   
   save_screen_line( 0, window->bottom_line, line_buff );
   set_prompt( diff_prompt7a, window->bottom_line );
   start = get_yn( );
   restore_screen_line( 0, window->bottom_line, line_buff );
   if (start != ERROR)
      diff.leading =  start == A_YES ?  TRUE  :  FALSE;
   else
      return( ERROR );

   
   save_screen_line( 0, window->bottom_line, line_buff );
   set_prompt( diff_prompt7b, window->bottom_line );
   start = get_yn( );
   restore_screen_line( 0, window->bottom_line, line_buff );
   if (start != ERROR) {
      if (start == A_YES)
         diff.leading = diff.all_space = TRUE;
      else
         diff.all_space = FALSE;
   } else
      return( ERROR );

   
   save_screen_line( 0, window->bottom_line, line_buff );
   set_prompt( diff_prompt7c, window->bottom_line );
   start = get_yn( );
   restore_screen_line( 0, window->bottom_line, line_buff );
   if (start != ERROR)
      diff.blank_lines =  start == A_YES  ?  TRUE : FALSE;
   else
      return( ERROR );

   
   save_screen_line( 0, window->bottom_line, line_buff );
   set_prompt( diff_prompt7d, window->bottom_line );
   start = get_yn( );
   restore_screen_line( 0, window->bottom_line, line_buff );
   if (start != ERROR)
      diff.ignore_eol =  start == A_YES  ?  TRUE : FALSE;
   else
      return( ERROR );

   
   save_screen_line( 0, window->bottom_line, line_buff );
   set_prompt( diff_prompt3, window->bottom_line );
   start = get_bc( );
   restore_screen_line( 0, window->bottom_line, line_buff );

   if (start != ERROR) {
      entab_linebuff( );
      if (un_copy_line( window->ll, window, TRUE ) == ERROR)
         return( ERROR );

     
      diff.defined = TRUE;
      if (start == BEGINNING) {
         diff.d1 = diff.w1->file_info->line_list;
         diff.d2 = diff.w2->file_info->line_list;
         diff.rline1 = 1L;
         diff.rline2 = 1L;
         diff.bin_offset1 = 0;
         diff.bin_offset2 = 0;
         rc = differ( 0, 0, window->bottom_line );
      } else {
         diff.d1 = diff.w1->ll;
         diff.d2 = diff.w2->ll;
         diff.rline1 = diff.w1->rline;
         diff.rline2 = diff.w2->rline;
         diff.bin_offset1 = diff.w1->bin_offset;
         diff.bin_offset2 = diff.w2->bin_offset;
         rc = differ( diff.w1->rcol, diff.w2->rcol, window->bottom_line );
      }
   }
   return( rc );
}



int  repeat_diff( WINDOW *window )
{
register int rc = ERROR;

   if (diff.defined) {
      entab_linebuff( );
      if (un_copy_line( window->ll, window, TRUE ) == ERROR)
         return( ERROR );

     
      diff.d1 = diff.w1->ll;
      diff.d2 = diff.w2->ll;
      diff.rline1 = diff.w1->rline;
      diff.rline2 = diff.w2->rline;
      diff.bin_offset1 = diff.w1->bin_offset;
      diff.bin_offset2 = diff.w2->bin_offset;
      rc = differ( diff.w1->rcol, diff.w2->rcol, window->bottom_line );
   } else
      error( WARNING, window->bottom_line, diff_prompt5 );
   return( rc );
}



int  differ( int initial_rcol1, int initial_rcol2, int bottom )
{
int  rcol1;             
int  rcol2;             
int  r1;                
int  r2;                
char c1;                
char c2;               
int  leading2;          
int  len1;              
int  len2;              
line_list_ptr node1;    
line_list_ptr node2;    
text_ptr diff1;         
text_ptr diff2;         
long rline1;            
long rline2;            
long bin_offset1;       
long bin_offset2;       
int  len;               
register int tabs;      
char line_buff[(MAX_COLS+1)*2];  

   
   rline1 = diff.rline1;
   rline2 = diff.rline2;
   node1 = diff.d1;
   node2 = diff.d2;
   bin_offset1 = diff.bin_offset1;
   bin_offset2 = diff.bin_offset2;
   tabs  = mode.inflate_tabs;
   if (diff.blank_lines) {
      while (node1->len != EOF  && is_line_blank( node1->line, node1->len )) {
         bin_offset1 += node1->len;
         node1 = node1->next;
         ++rline1;
         initial_rcol1 = 0;
      }
      while (node2->len != EOF  && is_line_blank( node2->line , node2->len)) {
         bin_offset2 += node2->len;
         node2 = node2->next;
         ++rline2;
         initial_rcol2 = 0;
      }
   }

   
   if (node1->len != EOF  &&  node2->len != EOF) {
      diff1 = node1->line;
      diff2 = node2->line;
      rcol1 = initial_rcol1;
      rcol2 = initial_rcol2;
      len1  = node1->len;
      len2  = node2->len;

      assert( rcol1 >= 0 );
      assert( rcol1 < MAX_LINE_LENGTH );
      assert( rcol2 >= 0 );
      assert( rcol2 < MAX_LINE_LENGTH );
      assert( len1 >= 0 );
      assert( len1 < MAX_LINE_LENGTH );
      assert( len2 >= 0 );
      assert( len2 < MAX_LINE_LENGTH );

      len = find_end( diff1, len1 );
      if (rcol1 > len)
         rcol1 = len;
      len = find_end( diff2, len2 );
      if (rcol2 > len)
         rcol2 = len;

      if (diff.leading) {
         leading1 = skip_leading_space( diff1, len1 );
         leading2 = skip_leading_space( diff2, len2 );
         if (tabs) {
            leading1 = detab_adjust_rcol( diff1, leading1 );
            leading2 = detab_adjust_rcol( diff2, leading2 );
         }
         if (rcol1 < leading1)
            rcol1 = leading1;
         if (rcol2 < leading2)
            rcol2 = leading2;
      }

      
      assert( rcol1 >= 0 );
      assert( rcol1 < MAX_LINE_LENGTH );
      assert( rcol2 >= 0 );
      assert( rcol2 < MAX_LINE_LENGTH );

      r1 =  tabs ? entab_adjust_rcol( diff1, len1, rcol1 ) : rcol1;
      r2 =  tabs ? entab_adjust_rcol( diff2, len2, rcol2 ) : rcol2;

      assert( r1 >= 0 );
      assert( r1 <= len1 );
      assert( r2 >= 0 );
      assert( r2 <= len2 );
      assert( r1 <= rcol1 );
      assert( r2 <= rcol2 );

      s_output( diff_message, g_display.mode_line, 67, g_display.diag_color );
      while (node1->len != EOF  &&  node2->len != EOF  &&
                         !g_status.control_break) {

         
         c1 = (char)(r1 < len1 ? *(diff1 + r1)  : 0);
         c2 = (char)(r2 < len2 ? *(diff2 + r2)  : 0);

         
         if (tabs) {
            if (c1 == '\t')
               c1 = ' ';
            if (c2 == '\t')
               c2 = ' ';
         }

         
         if (diff.all_space) {
            while (c1 == ' '  &&  r1 < len1) {
               ++rcol1;
               r1 = tabs ? entab_adjust_rcol( diff1, len1, rcol1 ) : rcol1;
               c1 =  (char)(r1 < len1  ?  *(diff1 + r1) :  0);
               if (c1 == '\t'  &&  tabs)
                  c1 = ' ';
            }
            while (c2 == ' '  &&  r2 < len2) {
               ++rcol2;
               r2 = tabs ? entab_adjust_rcol( diff2, len2, rcol2 ) : rcol2;
               c2 =  (char)(r2 < len2  ? *(diff2 + r2) : 0);
               if (c2 == '\t'  &&  tabs)
                  c2 = ' ';
            }
         }

         
         if (diff.ignore_eol) {
            if (r1 >= len1) {
               node1 = skip_eol( node1, &r1, &rcol1, &rline1, &bin_offset1 );
               len1  = node1->len;
               if (len1 != EOF) {
                  diff1 = node1->line;
                  c1 =  (char)(r1 < len1  ?  *(diff1 + r1) : 0);
                  if (c1 == '\t'  &&  tabs)
                     c1 = ' ';
               }
            }
            if (r2 >= len2) {
               node2 = skip_eol( node2, &r2, &rcol2, &rline2, &bin_offset2 );
               len2  = node2->len;
               if (len2 != EOF) {
                  diff2 = node2->line;
                  c2 =  (char)(r2 < len2  ? *(diff2 + r2)  :  0);
                  if (c2 == '\t'  &&  tabs)
                     c2 = ' ';
               }
            }
         }

        
         if (mode.search_case == IGNORE) {
            c1 = (char)tolower( c1 );
            c2 = (char)tolower( c2 );
         }

         
         while (r1 < len1  && r2 < len2) {
            if (c1 == c2) {
               if (diff.all_space) {
                  do {
                     ++rcol1;
                     r1 = tabs ? entab_adjust_rcol( diff1,len1,rcol1 ) : rcol1;
                     c1 =  (char)(r1 < len1  ?  *(diff1 + r1)  :  0);
                     if (c1 == '\t'  &&  tabs)
                        c1 = ' ';
                  } while (c1 == ' '  &&  r1 < len1);
                  do {
                     ++rcol2;
                     r2 = tabs ? entab_adjust_rcol( diff2,len2,rcol2 ) : rcol2;
                     c2 =  (char)(r2 < len2  ?  *(diff2 + r2)  :  0);
                     if (c2 == '\t'  &&  tabs)
                        c2 = ' ';
                  } while (c2 == ' '  &&  r2 < len2);
               } else {
                  ++rcol1;
                  ++rcol2;
                  r1 = tabs ? entab_adjust_rcol( diff1, len1, rcol1 ) : rcol1;
                  r2 = tabs ? entab_adjust_rcol( diff2, len2, rcol2 ) : rcol2;
                  c1 =  (char)(r1 < len1  ?  *(diff1 + r1)  :  0);
                  c2 =  (char)(r2 < len2  ?  *(diff2 + r2)  :  0);
                  if (tabs) {
                     if (c1 == '\t')
                        c1 = ' ';
                     if (c2 == '\t')
                        c2 = ' ';
                  }
               }
               if (diff.ignore_eol) {
                  if (r1 >= len1) {
                     node1 = skip_eol(node1, &r1, &rcol1, &rline1,&bin_offset1);
                     len1  = node1->len;
                     if (len1 != EOF) {
                        diff1 = node1->line;
                        c1 =  (char)(r1 < len1  ?  *(diff1 + r1)  : 0);
                        if (c1 == '\t'  &&  tabs)
                           c1 = ' ';
                     }
                  }
                  if (r2 >= len2) {
                     node2 = skip_eol(node2, &r2, &rcol2, &rline2,&bin_offset2);
                     len2  = node2->len;
                     if (len2 != EOF) {
                        diff2 = node2->line;
                        c2 = (char)(r2 < len2  ? *(diff2 + r2)  :  0);
                        if (c2 == '\t'  &&  tabs)
                           c2 = ' ';
                     }
                  }
               }
               if (mode.search_case == IGNORE) {
                  c1 = (char)tolower( c1 );
                  c2 = (char)tolower( c2 );
               }
            } else {

               
               update_line( diff.w1 );
               diff.w1->bin_offset = bin_offset1;
               find_adjust( diff.w1, node1, rline1, rcol1 );
               check_virtual_col( diff.w1, rcol1, rcol1 );
               show_diff_window( diff.w1 );
               update_line( diff.w2 );
               diff.w2->bin_offset = bin_offset2;
               bin_offset_adjust( diff.w2, rline2 );
               find_adjust( diff.w2, node2, rline2, rcol2 );
               check_virtual_col( diff.w2, rcol2, rcol2 );
               show_diff_window( diff.w2 );
               s_output( diff_blank, g_display.mode_line, 67,
                         g_display.mode_color );
               return( OK );
            }
         }

        
         if (node1->len != EOF && node2->len != EOF) {
            if (rcol1 != len1  &&  rcol2 != len2) {
               update_line( diff.w1 );
               diff.w1->bin_offset = bin_offset1;
               find_adjust( diff.w1, node1, rline1, rcol1 );
               show_diff_window( diff.w1 );
               update_line( diff.w2 );
               diff.w2->bin_offset = bin_offset2;
               find_adjust( diff.w2, node2, rline2, rcol2 );
               show_diff_window( diff.w2 );
               s_output( diff_blank, g_display.mode_line, 67,
                         g_display.mode_color );
               return( OK );
            } else {
               node1 = skip_eol( node1, &r1, &rcol1, &rline1, &bin_offset1 );
               len1  = node1->len;
               diff1 = node1->line;
               node2 = skip_eol( node2, &r2, &rcol2, &rline2, &bin_offset2 );
               len2  = node2->len;
               diff2 = node2->line;
            }
         }

         assert( rcol1 >= 0 );
         assert( rcol1 < MAX_LINE_LENGTH );
         assert( rcol2 >= 0 );
         assert( rcol2 < MAX_LINE_LENGTH );
         assert( r1 >= 0 );
         assert( r1 < MAX_LINE_LENGTH );
         assert( r2 >= 0 );
         assert( r2 < MAX_LINE_LENGTH );
         assert( r1 <= rcol1 );
         assert( r2 <= rcol2 );
         if (node1->len == EOF)
            assert( len1 == EOF );
         else {
            assert( len1 >= 0 );
            assert( len1 < MAX_LINE_LENGTH );
         }
         if (node2->len == EOF)
            assert( len2 == EOF );
         else {
            assert( len2 >= 0 );
            assert( len2 < MAX_LINE_LENGTH );
         }
      }
      save_screen_line( 0, bottom, line_buff );
      set_prompt( diff_prompt4, bottom );
      getkey( );
      restore_screen_line( 0, bottom, line_buff );
      s_output( diff_blank, g_display.mode_line, 67, g_display.mode_color );
   }
   return( ERROR );
}



int  skip_leading_space( text_ptr s, int len )
{
register int count = 0;

   assert( len >= 0 );
   assert( len < MAX_LINE_LENGTH );

   if (s != NULL) {
      if (mode.inflate_tabs) {
         while (len > 0  &&  (*s == ' ' || *s == '\t')) {
            ++count;
            ++s;
            --len;
         }
      } else {
         while (len > 0  &&  *s == ' ') {
           ++count;
           ++s;
           --len;
         }
      }
   }
   if (len == 0)
      count = 0;
   return( count );
}



line_list_ptr skip_eol( line_list_ptr d, int *r, int *rcol, long *rline,
                        long *bin_offset )
{
int  leading;
long rl;
long bo;

   *r = *rcol = 0;
   rl = *rline;
   bo = *bin_offset;
   if (d->len != EOF) {
      bo += d->len;
      d = d->next;
      ++rl;
      if (diff.blank_lines) {
         while (d->len != EOF  &&  is_line_blank( d->line, d->len )) {
            bo += d->len;
            d = d->next;
            ++rl;
         }
      }
      if (d->len != EOF) {
         if (diff.leading) {
            leading = skip_leading_space( d->line, d->len );
            if (mode.inflate_tabs)
               leading = detab_adjust_rcol( d->line, leading );
            *rcol = leading;
         } else
            *rcol = 0;
         *r = *rcol;
         if (mode.inflate_tabs)
            *r = entab_adjust_rcol( d->line, d->len, *rcol );
      }
   }
   *rline = rl;
   *bin_offset = bo;
   return( d );
}



void show_diff_window( WINDOW *win )
{
   if (win->file_info->dirty == LOCAL)
      display_current_window( win );
   else
      show_curl_line( win );
   show_line_col( win );
   make_ruler( win );
   show_ruler( win );
   show_ruler_pointer( win );
   win->file_info->dirty = FALSE;
}



int  verify_number( char *temp, int *num )
{
register file_infos *fp;

   /*
    * see if string has a number.  if string does have a number, convert it.
    */
   if (*temp == '\0' || !isdigit( *temp ))
      return( ERROR );
   *num = 0;
   while (isdigit( *temp ))
      *num = *num * 10 + *temp++ - '0';

   
   for (fp=g_status.file_list; fp != NULL; fp=fp->next) {
      if (fp->file_no == *num)
         return( OK );
   }
   return( ERROR );
}




int  verify_letter( char *temp, int *let, WINDOW **win )
{
register file_infos *fp;
register WINDOW *wp;
int  num;

   if (verify_number( temp, &num ) == OK) {
      while (isdigit( *temp ))
         temp++;
      if (*temp == '\0' || !isalpha( *temp ))
         return( ERROR );
      *let = (int)tolower( *temp );
      for (fp=g_status.file_list; fp != NULL; fp=fp->next) {
         if (fp->file_no == num)
            break;
      }
      for (wp=g_status.window_list; wp != NULL; wp=wp->next) {
         if (wp->file_info == fp  &&  wp->letter == *let  &&  wp->visible) {
            *win = wp;
            return( OK );
         }
      }
   }
   return( ERROR );
}
