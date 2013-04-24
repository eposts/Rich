
#include "tdestr.h"
#include "common.h"
#include "tdefunc.h"
#include "define.h"



int  sort_box_block( WINDOW *window )
{
int  prompt_line;
int  block_type;
line_list_ptr ll;
register file_infos *file;
WINDOW *sw;
int  rc;
char line_buff[(MAX_COLS+1)*2]; 

   rc = OK;
   prompt_line = window->bottom_line;
   entab_linebuff( );
   if (un_copy_line( window->ll, window, TRUE ) == ERROR)
      return( ERROR );
   check_block( );
   if (g_status.marked == TRUE) {
      file  = g_status.marked_file;
      block_type = file->block_type;
      if (block_type == BOX) {
      
         rc = get_sort_order( window );
         if (rc != ERROR) {
            file->modified = TRUE;
            if (mode.do_backups == TRUE) {
               sw = g_status.window_list;
               for (; ptoul( sw->file_info ) != ptoul( file );)
                  sw = sw->next;
               backup_file( sw );
            }

           
            sort.block_len = file->block_ec + 1 - file->block_bc;

          
            save_screen_line( 0, prompt_line, line_buff );
            eol_clear( 0, prompt_line, g_display.text_color );
            set_prompt( block22a, prompt_line );

           
            sort.bc  = g_status.marked_file->block_bc;
            sort.ec  = g_status.marked_file->block_ec;
            sort.order_array = (mode.search_case == IGNORE) ?
                                    sort_order.ignore : sort_order.match;

          
            ll = file->block_start->prev;
            quick_sort_block( file->block_br, file->block_er,
                              file->block_start, file->block_end );

            
            if (ll == NULL)
               ll = file->line_list;
            else
               ll = ll->next;
            set_prompt( block22b, prompt_line );
            insertion_sort_block( file->block_br, file->block_er, ll );

           
            file->dirty = GLOBAL;
            restore_cursors( file );
            restore_screen_line( 0, prompt_line, line_buff );
         }
      } else {
        
         error( WARNING, prompt_line, block23 );
         rc = ERROR;
      }
   } else {
     
      error( WARNING, prompt_line, block24 );
      rc = ERROR;
   }
   return( rc );
}



void quick_sort_block( long low, long high, line_list_ptr low_node,
                       line_list_ptr high_node )
{
long low_rline_stack[24];
long high_rline_stack[24];
line_list_ptr low_node_stack[24];
line_list_ptr high_node_stack[24];
long low_count;
long high_count;
long count;
line_list_ptr low_start;
line_list_ptr low_head;
line_list_ptr low_tail;
line_list_ptr high_end;
line_list_ptr high_head;
line_list_ptr high_tail;
line_list_ptr equal_head;
line_list_ptr equal_tail;
line_list_ptr walk_node;
line_list_ptr median_node;
int  i;
int  stack_pointer;

   assert( low_node->len != EOF);
   assert( high_node->len != EOF);

   stack_pointer = 0;
   for (;;) {

      
      while (high - low > 25) {

         assert( high >= 1 );
         assert( low  >= 1 );
         assert( low  <= high );

        
         walk_node  = low_node;
         count = (high - low) / 2;
         for (; count > 0; count--)
            walk_node = walk_node->next;

         
         load_pivot( low_node );
         if (compare_pivot( walk_node ) < 0) {
            low_head   = walk_node;
            median_node = low_node;
         } else {
            low_head   = low_node;
            median_node = walk_node;
         }
         high_head = high_node;
         load_pivot( median_node );
         if (compare_pivot( high_node ) < 0) {
            high_head   = median_node;
            median_node = high_node;
         }
         load_pivot( median_node );
         if (compare_pivot( low_head ) > 0) {
            low_tail    = median_node;
            median_node = low_head;
            low_head    = low_tail;
         }

         load_pivot( median_node );

         assert( compare_pivot( low_head ) <= 0 );
         assert( compare_pivot( high_head ) >= 0 );

         walk_node = low_node;
         for (i = 0; ; walk_node = walk_node->next) {
            if (compare_pivot( walk_node ) == 0)
               break;
            i = 1;
         }

         
         low_start  = low_node->prev;
         high_end   = high_node->next;
         low_head   = low_tail  = NULL;
         high_head  = high_tail = NULL;
         low_count  = high_count = 0;

         
         if (i == 0)
            walk_node = equal_head = equal_tail = low_node;
         else {
            equal_head = equal_tail = walk_node;
            equal_head->next->prev = equal_head->prev;
            equal_head->prev->next = equal_head->next;
            equal_head->next = low_node;
            walk_node = equal_head;
         }
         load_pivot( equal_head );

        
         for (count=low+1; count <= high; count++) {
            walk_node = walk_node->next;
            i = compare_pivot( walk_node );
            if (i > 0) {
               if (high_head == NULL)
                  high_head = high_tail = walk_node;
               else {
                  high_tail->next = walk_node;
                  walk_node->prev = high_tail;
                  high_tail = walk_node;
               }

               ++high_count;
            } else if (i < 0) {
               if (low_head == NULL)
                  low_head = low_tail = walk_node;
               else {
                  low_tail->next = walk_node;
                  walk_node->prev = low_tail;
                  low_tail = walk_node;
               }

               
               ++low_count;
            } else {
               equal_tail->next = walk_node;
               walk_node->prev = equal_tail;
               equal_tail = walk_node;
            }
         }

         assert( low_count >= 0 );
         assert( low_count < high - low );
         assert( high_count >= 0 );
         assert( high_count < high - low );

         
         if (low_count > 0) {
            low_head->prev = low_start;
            if (low_start != NULL)
               low_start->next = low_head;
            else
               g_status.marked_file->line_list = low_head;
            low_tail->next = equal_head;
            equal_head->prev = low_tail;
         } else {
            equal_head->prev = low_start;
            if (low_start != NULL)
               low_start->next = equal_head;
            else
               g_status.marked_file->line_list = equal_head;
         }
         if (high_count > 0) {
            high_head->prev = equal_tail;
            equal_tail->next = high_head;
            high_tail->next = high_end;
            high_end->prev  = high_tail;
         } else {
            equal_tail->next = high_end;
            high_end->prev   = equal_tail;
         }

         
         if (low_count > high_count) {

           
            if (high_count > 25) {
               low_rline_stack[stack_pointer]  = low;
               high_rline_stack[stack_pointer] = low + low_count - 1;
               low_node_stack[stack_pointer]   = low_head;
               high_node_stack[stack_pointer]  = low_tail;
               ++stack_pointer;
               low       = high - high_count + 1;
               high      = high;
               low_node  = high_head;
               high_node = high_tail;
            } else {
               low       = low;
               high      = low + low_count - 1;
               low_node  = low_head;
               high_node = low_tail;
            }
         } else {

            if (low_count > 25) {
               low_rline_stack[stack_pointer]  = high - high_count + 1;
               high_rline_stack[stack_pointer] = high;
               low_node_stack[stack_pointer]   = high_head;
               high_node_stack[stack_pointer]  = high_tail;
               ++stack_pointer;
               low       = low;
               high      = low + low_count - 1;
               low_node  = low_head;
               high_node = low_tail;
            } else {
               low       = high - high_count + 1;
               high      = high;
               low_node  = high_head;
               high_node = high_tail;
            }
         }

         assert( stack_pointer < 24 );
      }

     
      --stack_pointer;
      if (stack_pointer < 0)
         break;
      low       = low_rline_stack[stack_pointer];
      high      = high_rline_stack[stack_pointer];
      low_node  = low_node_stack[stack_pointer];
      high_node = high_node_stack[stack_pointer];
   }
}



void insertion_sort_block( long low, long high, line_list_ptr first_node )
{
long down;                      
long pivot;                     
long count;
line_list_ptr pivot_node;       
line_list_ptr down_node;        
text_ptr key;
int  dirty_flag;
int  len;

  
   if (low < high) {

      count = (int)(high - low) + 1;
      pivot_node = first_node->next;
      for (pivot=1; pivot < count; pivot++) {
         load_pivot( pivot_node );
         key = pivot_node->line;
         len = pivot_node->len;
         dirty_flag = pivot_node->dirty;
         down_node = pivot_node;
         for (down=pivot-1; down >= 0; down--) {
            
            if (compare_pivot( down_node->prev ) > 0) {
               down_node->line = down_node->prev->line;
               down_node->len = down_node->prev->len;
               down_node->dirty = down_node->prev->dirty;
            } else
               break;
            down_node = down_node->prev;
         }
         down_node->line = key;
         down_node->len  = len;
         down_node->dirty = (char)dirty_flag;
         pivot_node = pivot_node->next;
      }
   }
}



void load_pivot( line_list_ptr node )
{
   sort.pivot_ptr = node->line;
   sort.pivot_len = node->len;
}



int  compare_pivot( line_list_ptr node )
{
register int len;
register int bc;
int  rc;
int  left_over;

   len = node->len;
   bc  = sort.bc;

   assert( bc >= 0 );
   assert( len >= 0 );

   
   if (len < bc+1) {
      if (sort.pivot_len < bc+1)
         return( 0 );
      else
         return( sort.direction == ASCENDING ?  -1 : 1 );

  
   } else if (sort.pivot_len < bc+1) {
      if (len < bc+1)
         return( 0 );
      else
         return( sort.direction == ASCENDING ?  1 : -1 );
   } else {

      if (len == sort.pivot_len)
         left_over = 0;
      else if (len > sort.ec  &&  sort.pivot_len > sort.ec)
         left_over = 0;
      else {

         
         if (sort.direction == ASCENDING)
            left_over =  len > sort.pivot_len ? 1 : -1;
         else
            left_over =  len > sort.pivot_len ? -1 : 1;
      }

     
      if (len > sort.pivot_len)
         len = sort.pivot_len;
      len = len - bc;
      if (len > sort.block_len)
         len = sort.block_len;

      assert( len > 0 );

      if (sort.direction == ASCENDING)
         rc = my_memcmp( node->line + bc, sort.pivot_ptr + bc, len );
      else
         rc = my_memcmp( sort.pivot_ptr + bc, node->line + bc, len );

      
      if (rc == 0)
         rc = left_over;
      return( rc );
   }
}



int  my_memcmp( text_ptr s1, text_ptr s2, int len )
{
unsigned char *p;
register int c;

   assert( len >= 0 );
   assert( len < MAX_LINE_LENGTH );
   assert( s1 != NULL );
   assert( s2 != NULL );

   if (len == 0)
      return( 0 );

   p = sort.order_array;


   if (len < 10) {
      for (;len > 0  &&  (c = (int)p[*s1] - (int)p[*s2]) == 0;
                                              s1++, s2++, len--);
      return( c );
   } else {

      ASSEMBLE {



        push    ds                      
        push    si
        push    di
        push    bp

        xor     ax, ax                  
        mov     cx, WORD PTR len        
        cmp     cx, 0                   
        jle     get_out                 

        mov     bx, WORD PTR s2         
        mov     ax, WORD PTR s2+2
        mov     es, ax                  
        mov     si, WORD PTR s1
        mov     ax, WORD PTR s1+2
        mov     ds, ax                  
        mov     bp, p                   
        xor     ax, ax                  
        xor     dx, dx                  
      }
top:

   ASSEMBLE {
        mov     al, BYTE PTR ds:[si]    
        mov     di, ax
        mov     al, BYTE PTR [bp+di]    
        mov     dl, BYTE PTR es:[bx]    
        mov     di, dx
        mov     dl, BYTE PTR [bp+di]    
        sub     ax, dx                  
        jne     get_out
        inc     bx
        inc     si
        dec     cx
        cmp     cx, 0
        jg      top                     
      }
get_out:

   ASSEMBLE {
        pop     bp                      
        pop     di
        pop     si
        pop     ds                      
      }
   }
}
