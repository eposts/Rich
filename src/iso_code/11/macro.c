

#include "tdestr.h"             
#include "common.h"
#include "define.h"
#include "tdefunc.h"



int  record_on_off( WINDOW *window )
{
register int next;
int  prev;
int  line;
int  key;
int  func;
char line_buff[(MAX_COLS+2)*2]; 

   mode.record = !mode.record;
   if (mode.record == TRUE) {
      line = window->bottom_line;
      show_avail_strokes( );
      save_screen_line( 0, line, line_buff );
      
      set_prompt( main11, line );

      
      key = getkey( );
      func = getfunc( key );

      
      if (key <= 256 || (func != 0 && func != PlayBack)) {
         
         error( WARNING, line, main12 );
         mode.record = FALSE;
      } else if (g_status.stroke_count == 0) {
        
         error( WARNING, line, main13 );
         mode.record = FALSE;
      } else {

        
         prev = OK;
         if (func == PlayBack) {
            
            set_prompt( main14, line );
            if (get_yn( ) == A_NO) {
               prev = ERROR;
               mode.record = FALSE;
            }
         }
         if (prev == OK) {
            g_status.recording_key = key;
            next = macro.first_stroke[key-256];

            
            if (next != STROKE_LIMIT+1) {
               do {
                  prev = next;
                  next = macro.strokes[next].next;
                  macro.strokes[prev].key  = MAX_KEYS+1;
                  macro.strokes[prev].next = STROKE_LIMIT+1;
                  ++g_status.stroke_count;
               } while (next != -1);
               show_avail_strokes( );
            }

            
            for (next=0; macro.strokes[next].next != STROKE_LIMIT+1;)
               next++;
            macro.first_stroke[key-256] = next;
            macro.strokes[next].key  = -1;
            macro.strokes[next].next = -1;
            key_func.key[key-256] = PlayBack;
           
            s_output( main15, g_display.mode_line, 22,
                      g_display.mode_color | 0x80 );
         }
      }
      restore_screen_line( 0, line, line_buff );
   }

   if (mode.record == FALSE) {
      memset( line_buff, ' ', 36 );
      line_buff[36] = '\0';
      s_output( line_buff, g_display.mode_line, 22, g_display.mode_color );
      show_tab_modes( );
      show_indent_mode( );
      show_sync_mode( );
      show_search_case( );
      show_wordwrap_mode( );

     
      key = g_status.recording_key;
      if (key != 0) {
         next = macro.first_stroke[key-256];
         if (macro.strokes[next].key == -1) {
            macro.strokes[next].key  = MAX_KEYS+1;
            macro.strokes[next].next = STROKE_LIMIT+1;
            macro.first_stroke[key-256] = STROKE_LIMIT+1;
            if (getfunc( key ) == PlayBack)
               key_func.key[key-256] = 0;
         }
      }
      g_status.recording_key = 0;
   }
   return( OK );
}


/*
 * 作用: 把键盘命令放到缓存中
 * 参数: line: 要提示显示的行
 * 注意: 如果next值是-1，表明到了记录的末尾；
 *       如果next值是STROKE_LIMIT+1表示空间已经用完
 */
void record_keys( int line )
{
register int next;
register int prev;
int  key;
int  func;

   if (mode.record == TRUE) {
      if (g_status.stroke_count == 0)
         /*
          * 宏记录已经没有空间记录更多的操作
          */
         error( WARNING, line, main13 );
      else {
         key = g_status.key_pressed;
         func = getfunc( key );
         if (func != RecordMacro && func != SaveMacro && func != LoadMacro &&
             func != ClearAllMacros) {

            /*
             * 如果next值是-1，表明到了记录的末尾
             */
            next = macro.first_stroke[g_status.recording_key - 256];
            if (macro.strokes[next].next != STROKE_LIMIT+1) {
               while (macro.strokes[next].next != -1)
                  next = macro.strokes[next].next;
            }
            prev = next;

            /*
             * 找到一个空间来记录当前的操作
             */
            if (macro.strokes[next].key != -1) {
               for (; next < STROKE_LIMIT &&
                            macro.strokes[next].next != STROKE_LIMIT+1;)
                  next++;
               if (next == STROKE_LIMIT) {
                  for (next=0; next < prev &&
                               macro.strokes[next].next != STROKE_LIMIT+1;)
                     next++;
               }
            }
            if (next == prev && macro.strokes[prev].key != -1)
               /*
                * 记录缓存没有记录
                */
               error( WARNING, line, main13 );
            else {
            /*
             * 如果我们正在记录第一个宏节点，那么next == prev
             */
               macro.strokes[prev].next = next;
               macro.strokes[next].next = -1;
               macro.strokes[next].key  = key;
               g_status.stroke_count--;
               show_avail_strokes( );
            }
         }
      }
   }
}



void show_avail_strokes( void )
{
char strokes[MAX_COLS];

   s_output( main18, g_display.mode_line, 33, g_display.mode_color );
   itoa( g_status.stroke_count, strokes, 10 );
   s_output( "      ", g_display.mode_line, 51, g_display.mode_color );
   s_output( strokes, g_display.mode_line, 51, g_display.mode_color );
}



int  save_strokes( WINDOW *window )
{
FILE *fp;                       
char name[MAX_COLS+2];          
char line_buff[(MAX_COLS+1)*2]; 
register int rc;
int  prompt_line;
int  fattr;

   name[0] = '\0';
   prompt_line = window->bottom_line;
   save_screen_line( 0, prompt_line, line_buff );
   
   if ((rc = get_name( main19, prompt_line, name,
                 g_display.message_color )) == OK  &&  *name != '\0') {

      
      rc = get_fattr( name, &fattr );
      if (rc == OK) {
         
         set_prompt( main20, prompt_line );
         if (get_yn( ) != A_YES  ||  change_mode( name, prompt_line ) == ERROR)
            rc = ERROR;
      }
      if (rc != ERROR) {
         if ((fp = fopen( name, "wb" )) != NULL) {
            fwrite( &macro.first_stroke[0], sizeof(int), MAX_KEYS, fp );
            fwrite( &macro.strokes[0], sizeof(STROKES), STROKE_LIMIT, fp );
            fclose( fp );
         }
      }
   }
   restore_screen_line( 0, prompt_line, line_buff );
   return( OK );
}



int  load_strokes( WINDOW *window )
{
register FILE *fp;      
char dname[MAX_COLS];   
char stem[MAX_COLS];    
register int rc;

   dname[0] = '\0';
   
   if (get_name( main21, window->bottom_line, dname,
                 g_display.message_color ) == OK  &&  *dname != '\0') {
      if (validate_path( dname, stem ) == OK) {
         rc = list_and_pick( dname, stem, window );

         if (rc == OK) {
            if ((fp = fopen( dname, "rb" )) != NULL && ceh.flag != ERROR) {
               fwrite( &macro.first_stroke[0], sizeof(int), MAX_KEYS, fp );
               fwrite( &macro.strokes[0], sizeof(STROKES), STROKE_LIMIT, fp );
               fclose( fp );
            }
            if (ceh.flag == OK)
               connect_macros( );
         }
      } else
        
         error( WARNING, window->bottom_line, main22 );
   }
   return( OK );
}



int  clear_macros( WINDOW *arg_filler )
{
register int i;

   g_status.stroke_count = STROKE_LIMIT;
   for (i=0; i<STROKE_LIMIT; i++) {
      macro.strokes[i].next = STROKE_LIMIT+1;
      macro.strokes[i].key  = MAX_KEYS+1;
   }
   for (i=0; i<MAX_KEYS; i++) {
      macro.first_stroke[i] = STROKE_LIMIT+1;
      if (key_func.key[i] == PlayBack)
         key_func.key[i] = 0;
   }
   return( OK );
}



void connect_macros( void )
{
register int i;

   
   for (i=0; i<MAX_KEYS; i++)
      if (key_func.key[i] == PlayBack)
         key_func.key[i] = 0;

  
   g_status.stroke_count = 0;
   for (i=0; i<STROKE_LIMIT; i++)
      if (macro.strokes[i].next == STROKE_LIMIT+1)
         ++g_status.stroke_count;

   
   for (i=0; i<MAX_KEYS; i++) {
      if (macro.first_stroke[i] != STROKE_LIMIT+1)
         if (key_func.key[i] == 0)
            key_func.key[i] = PlayBack;
   }
}



int  play_back( WINDOW *window )
{
int  key;
int  rc = OK;
int  popped;            

  
   if (mode.record == TRUE && g_status.key_pressed == g_status.recording_key)
      rc = ERROR;
   else {

      
      g_status.macro_executing = TRUE;
      g_status.mstack_pointer  = -1;
      popped = FALSE;
      rc = OK;
      while (rc == OK) {

         
         if (popped == FALSE) {

            
            g_status.macro_next = macro.first_stroke[g_status.key_pressed-256];
            g_status.current_macro = g_status.key_pressed;
            key = macro.strokes[g_status.macro_next].key;
         }
         popped = FALSE;
         if (key != MAX_KEYS+1  &&  key != -1) {
            do {

               window = g_status.current_window;
               display_dirty_windows( window );
               ceh.flag = OK;
               g_status.key_pressed = macro.strokes[g_status.macro_next].key;
               g_status.command = getfunc( g_status.key_pressed );
               if (g_status.wrapped  ||  g_status.key_pending) {
                  g_status.key_pending = FALSE;
                  g_status.wrapped = FALSE;
                  show_search_message( CLR_SEARCH, g_display.mode_color );
               }

               
               if (g_status.control_break == TRUE) {
                  rc = ERROR;
                  break;
               }

              
               if (g_status.command == PlayBack) {

                 
                  if (g_status.current_macro != g_status.key_pressed) {
                     if (push_macro_stack(
                                   macro.strokes[g_status.macro_next].next )
                                   != OK) {
                        error( WARNING, window->bottom_line, ed16 );
                        rc = ERROR;
                     }
                     g_status.macro_next =
                                macro.first_stroke[g_status.key_pressed-256];
                     g_status.current_macro = g_status.key_pressed;
                     key = macro.strokes[g_status.macro_next].key;

                   
                     continue;
                  } else

                    
                     break;
               }


              
#if defined(  __MSC__ )
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
#endif


               if (g_status.command >= 0 && g_status.command < NUM_FUNCS)
                   rc = (*do_it[g_status.command])( window );
               g_status.macro_next =
                          macro.strokes[g_status.macro_next].next;
            } while (rc == OK  &&  g_status.macro_next != -1);

            
            if (g_status.macro_next == -1 && g_status.mstack_pointer < 0)
               rc = ERROR;
            else if (rc != ERROR  &&  g_status.mstack_pointer >= 0) {

               
               if (g_status.current_macro != g_status.key_pressed) {
                  if (pop_macro_stack( &g_status.macro_next ) != OK) {
                     error( WARNING, window->bottom_line, ed17 );
                     rc = ERROR;
                  } else {
                     popped = TRUE;
                     key = macro.strokes[g_status.macro_next].key;
                  }
               }
            }
         }
      }
      g_status.macro_executing = FALSE;
   }
   return( OK );
}



int  push_macro_stack( int key )
{
   
   if (g_status.mstack_pointer+1 < MAX_KEYS) {

      
      ++g_status.mstack_pointer;
      macro_stack[g_status.mstack_pointer].key = key;
      macro_stack[g_status.mstack_pointer].macro = g_status.current_macro;
      return( OK );
   } else
      return( STACK_OVERFLOW );
}



int  pop_macro_stack( int *key )
{

  
   if (g_status.mstack_pointer >= 0) {

      
      *key = macro_stack[g_status.mstack_pointer].key;
      g_status.current_macro = macro_stack[g_status.mstack_pointer].macro;
      --g_status.mstack_pointer;
      return( OK );
   } else
      return( STACK_UNDERFLOW );
}



int  pause( WINDOW *arg_filler )
{
int  c;

  
   s_output( paused1, g_display.mode_line, 23, g_display.mode_color | 0x80 );
   s_output( paused2, g_display.mode_line, 23+strlen( paused1 ),
             g_display.mode_color );

  
   c = getkey( );
   show_modes( );
   if (mode.record == TRUE) {
    
      s_output( main15, g_display.mode_line, 23, g_display.mode_color | 0x80 );
      show_avail_strokes( );
   }
   return( c == ESC ? ERROR : OK );
}
