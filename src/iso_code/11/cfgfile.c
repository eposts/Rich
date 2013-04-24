


#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tdecfg.h"
#include "cfgfile.h"


extern struct vcfg cfg;
extern FILE *tde_exe;                  
extern KEY_FUNC key_func;
extern MACRO macros;
extern COLORS temp_colours;
extern MODE_INFO in_modes;
extern long keys_offset;
extern long two_key_offset;
extern long macro_offset;
extern long color_offset;
extern long mode_offset;
extern long sort_offset;



char line_in[2000];             
int  stroke_count;              
unsigned int line_no;           
int modes[NUM_MODES];


TWO_KEY two_key_list;

SORT_ORDER sort_order;



void tdecfgfile( void )
{
FILE *config;
char fname[80];
int  rc;

   
   cls( );
   xygoto( 0, 3 );
   puts( "Enter configuration file name, e.g. tde.cfg  :" );
   gets( fname );

   if (strlen( fname ) != 0) {
      rc = OK;
      if ((rc = access( fname, EXIST )) != 0) {
         puts( "\n\n Error: File not found." );
         getkey( );
         rc = ERROR;
      } else if ((config = fopen( fname, "r" )) == NULL ) {
         puts( "\n\nError: Cannot open configuration file." );
         getkey( );
         rc = ERROR;
      }

    
      if (rc == OK) {
         fseek( tde_exe, keys_offset, SEEK_SET );
         fread( (void *)&key_func, sizeof(KEY_FUNC), 1, tde_exe );
         fseek( tde_exe, two_key_offset, SEEK_SET );
         fread( (void *)&two_key_list, sizeof(TWO_KEY), 1, tde_exe );
         fseek( tde_exe, macro_offset, SEEK_SET );
         fread( (void *)&macros, sizeof(MACRO), 1, tde_exe );
         fseek( tde_exe, color_offset, SEEK_SET );
         fread( (void *)&temp_colours, sizeof(COLORS), 1, tde_exe );
         fseek( tde_exe, mode_offset, SEEK_SET );
         fread( (void *)&in_modes, sizeof( MODE_INFO ), 1, tde_exe );
         fseek( tde_exe, sort_offset, SEEK_SET );
         fread( (void *)&sort_order, sizeof( SORT_ORDER ), 1, tde_exe );

         stroke_count = get_stroke_count( );

        
         modes[Ins]         = in_modes.insert;
         modes[Ind]         = in_modes.indent;
         modes[PTAB]        = in_modes.ptab_size;
         modes[LTAB]        = in_modes.ltab_size;
         modes[Smart]       = in_modes.smart_tab;
         modes[Write_Z]     = in_modes.control_z;
         modes[Crlf]        = in_modes.crlf;
         modes[Trim]        = in_modes.trailing;
         modes[Eol]         = in_modes.show_eol;
         modes[WW]          = in_modes.word_wrap;
         modes[Left]        = in_modes.left_margin;
         modes[Para]        = in_modes.parg_margin;
         modes[Right]       = in_modes.right_margin;
         modes[Size]        = in_modes.cursor_size;
         modes[Backup]      = in_modes.do_backups;
         modes[Ruler]       = in_modes.ruler;
         modes[Date]        = in_modes.date_style;
         modes[Time]        = in_modes.time_style;
         modes[InflateTabs] = in_modes.inflate_tabs;
         modes[Initcase]    = in_modes.search_case;
         modes[JustRM]      = in_modes.right_justify;

         line_no = 1;
         while (!feof( config )) {
            if (fgets( line_in, 1500, config ) == NULL)
               break;
            parse_line( line_in );
            ++line_no;
         }

        
         in_modes.insert        = modes[Ins];
         in_modes.indent        = modes[Ind];
         in_modes.ptab_size     = modes[PTAB];
         in_modes.ltab_size     = modes[LTAB];
         in_modes.smart_tab     = modes[Smart];
         in_modes.control_z     = modes[Write_Z];
         in_modes.crlf          = modes[Crlf];
         in_modes.trailing      = modes[Trim];
         in_modes.show_eol      = modes[Eol];
         in_modes.word_wrap     = modes[WW];
         in_modes.left_margin   = modes[Left];
         in_modes.parg_margin   = modes[Para];
         in_modes.right_margin  = modes[Right];
         in_modes.cursor_size   = modes[Size];
         in_modes.do_backups    = modes[Backup];
         in_modes.ruler         = modes[Ruler];
         in_modes.date_style    = modes[Date];
         in_modes.time_style    = modes[Time];
         in_modes.inflate_tabs  = modes[InflateTabs];
         in_modes.search_case   = modes[Initcase];
         in_modes.right_justify = modes[JustRM];

         fseek( tde_exe, keys_offset, SEEK_SET );
         fwrite( (void *)&key_func, sizeof(KEY_FUNC), 1, tde_exe );
         fseek( tde_exe, two_key_offset, SEEK_SET );
         fwrite( (void *)&two_key_list, sizeof(TWO_KEY), 1, tde_exe );
         fseek( tde_exe, macro_offset, SEEK_SET );
         fwrite( (void *)&macros, sizeof(MACRO), 1, tde_exe );
         fseek( tde_exe, color_offset, SEEK_SET );
         fwrite( (void *)&temp_colours, sizeof(COLORS), 1, tde_exe );
         fseek( tde_exe, mode_offset, SEEK_SET );
         fwrite( (void *)&in_modes, sizeof( MODE_INFO ), 1, tde_exe );
         fseek( tde_exe, sort_offset, SEEK_SET );
         fwrite( (void *)&sort_order, sizeof( SORT_ORDER ), 1, tde_exe );
         fclose( config );
         printf( "\n\n    Configuration file read.  Press a key to continue :" );
         getkey( );
      }
   }
   cls( );
}



void parse_line( char *line )
{
char key[1042];         
char *residue;          
int key_no;             
int parent_key;         
int color;              
int mode_index;         
int func_no;            
int color_no;           
int mode_no;            
int found;              
int i;


   residue = parse_token( line, key );
   if (*key != '\0' && *key != ';') {
      if (strlen( key ) > 1) {
      
         found = FALSE;
         key_no = search( key, valid_keys, AVAIL_KEYS-1 );
         if (key_no != ERROR) {
           
            found = TRUE;
            if (residue != NULL) {
               residue = parse_token( residue, key );

               
               if (*key != '\0' && *key != ';') {
                  func_no = search( key, valid_func, NUM_FUNC );
                  if (func_no != ERROR) {
                     clear_previous_twokey( key_no );
                     clear_previous_macro( key_no );
                     key_func.key[key_no] = func_no;
                     if (func_no == PlayBack)
                        parse_macro( key_no, residue );
                  } else {
                     parent_key = key_no;

                    
                     if (strlen( key ) == 1) {
                        key_no = *key;
                        residue = parse_token( residue, key );
                        if (*key != '\0' && *key != ';') {
                           func_no = search( key, valid_func, NUM_FUNC );
                           if (func_no != ERROR && func_no != PlayBack) {
                              if (insert_twokey( parent_key+256, key_no,
                                                          func_no ) == ERROR) {
                                 printf( "==> %s", line_in );
                                 printf( "Out of room for two-key: line %u  : function %s\n",
                                      line_no, key );
                              }
                           } else {
                              printf( "==> %s", line_in );
                              if ( func_no == ERROR)
                                 printf( "Unrecognized function: line %u  : function %s\n",
                                         line_no, key );
                              else
                                 printf( "Cannot assign a macro to two-keys: line %u  : function %s\n",
                                         line_no, key );
                           }
                        }
                     } else {
                        residue = parse_token( residue, key );
                        key_no = search( key, valid_keys, AVAIL_KEYS-1 );
                        if (key_no != ERROR && *key != '\0' && *key != ';') {
                           func_no = search( key, valid_func, NUM_FUNC );
                           if (func_no != ERROR && func_no != PlayBack) {
                              if (insert_twokey( parent_key+256, key_no+256,
                                                        func_no )  == ERROR) {
                                 printf( "==> %s", line_in );
                                 printf( "Out of room for two-key: line %u  : function %s\n",
                                      line_no, key );
                              }
                           } else {
                              printf( "==> %s", line_in );
                              if ( func_no == ERROR)
                                 printf( "Unrecognized function: line %u  : function %s\n",
                                         line_no, key );
                              else
                                 printf( "Cannot assign a macro to two-keys: line %u  : function %s\n",
                                         line_no, key );
                           }
                        } else {
                           printf( "==> %s", line_in );
                           printf( "Unrecognized function: line %u  : function %s\n",
                                   line_no, key );
                        }
                     }
                  }
               }
            }
         }

        
         if (!found) {
            color = search( key, valid_colors, (NUM_COLORS * 2) - 1 );
            if (color != ERROR) {
               if (*key == 'm')
                  i = 0;
               else
                  i = 1;
               found = TRUE;
               if (residue != NULL) {
                  residue = parse_token( residue, key );

                 
                  if (*key != '\0' && *key != ';') {
                     color_no = atoi( key );
                     if (color_no >= 0 && color_no <= 127)
                        temp_colours.clr[i][color] = color_no;
                     else {
                        printf( "==> %s", line_in );
                        printf( "Color number out of range: line %u  : number %s\n",
                                 line_no, key );
                     }
                  }
               }
            }
         }

         
         if (!found) {
            mode_index = search( key, valid_modes, NUM_MODES-1 );
            if (mode_index != ERROR) {
               found = TRUE;

               
               if (residue != NULL) {
                  residue = parse_token( residue, key );
                  if (*key != '\0' && *key != ';') {
                     switch ( mode_index ) {
                        case Ins         :
                        case Ind         :
                        case Smart       :
                        case Trim        :
                        case Eol         :
                        case Backup      :
                        case Ruler       :
                        case InflateTabs :
                        case JustRM      :
                           mode_no = search( key, off_on, 1 );
                           if (mode_no == ERROR) {
                              printf( "==> %s", line_in );
                              printf( "Off/On error: " );
                           }
                           break;
                        case LTAB     :
                        case PTAB     :
                           mode_no = atoi( key );
                           if (mode_no > 520 || mode_no < 1) {
                              mode_no = ERROR;
                              printf( "==> %s", line_in );
                              printf( "Tab error: " );
                           }
                           break;
                        case Left    :
                           mode_no = atoi( key );
                           if (mode_no < 1 || mode_no > modes[Right]) {
                              mode_no = ERROR;
                              printf( "==> %s", line_in );
                              printf( "Left margin error: " );
                           } else
                              --mode_no;
                           break;
                        case Para    :
                           mode_no = atoi( key );
                           if (mode_no < 1 || mode_no > modes[Right]) {
                              mode_no = ERROR;
                              printf( "==> %s", line_in );
                              printf( "Paragraph margin error: " );
                           } else
                              --mode_no;
                           break;
                        case Right   :
                           mode_no = atoi( key );
                           if (mode_no < modes[Left] || mode_no > 1040) {
                              mode_no = ERROR;
                              printf( "==> %s", line_in );
                              printf( "Right margin error: " );
                           } else
                              --mode_no;
                           break;
                        case Crlf    :
                           mode_no = search( key, valid_crlf, 1 );
                           if (mode_no == ERROR) {
                              printf( "==> %s", line_in );
                              printf( "CRLF or LF error: " );
                           }
                           break;
                        case WW      :
                           mode_no = search( key, valid_wraps, 2 );
                           if (mode_no == ERROR) {
                              printf( "==> %s", line_in );
                              printf( "Word wrap error: " );
                           }
                           break;
                        case Size    :
                           mode_no = search( key, valid_cursor, 1 );
                           if (mode_no == ERROR) {
                              printf( "==> %s", line_in );
                              printf( "Cursor size error: " );
                           }
                           break;
                        case Write_Z :
                           mode_no = search( key, valid_z, 1 );
                           if (mode_no == ERROR) {
                              printf( "==> %s", line_in );
                              printf( "Control Z error: " );
                           }
                           break;
                        case Date    :
                           mode_no = search( key, valid_dates, 5 );
                           if (mode_no == ERROR) {
                              printf( "==> %s", line_in );
                              printf( "Date format error: " );
                           }
                           break;
                        case Time    :
                           mode_no = search( key, valid_times, 1 );
                           if (mode_no == ERROR) {
                              printf( "==> %s", line_in );
                              printf( "Time format error: " );
                           }
                           break;
                        case Initcase    :
                           mode_no = search( key, init_case_modes, 1 );
                           if (mode_no == ERROR) {
                              printf( "==> %s", line_in );
                              printf( "Initial Case Mode error: " );
                           }
                           break;
                        case Match   :
                           for (i=0; i<256; i++)
                              sort_order.match[i] = (char)i;
                           new_sort_order( key, sort_order.match );
                           break;
                        case Ignore  :
                           for (i=0; i<256; i++)
                              sort_order.ignore[i] = (char)i;
                           for (i=65; i<91; i++)
                              sort_order.ignore[i] = (char)(i + 32);
                           new_sort_order( key, sort_order.ignore );
                           break;
                     }
                     if (mode_no != ERROR)
                        modes[mode_index] = mode_no;
                     else
                        printf( " line = %u  :  unknown mode = %s\n",
                              line_no, key );
                  }
               }
            }
         }
         if (!found) {
            printf( "==> %s", line_in );
            printf( "Unrecognized editor setting: line %u  :  %s\n", line_no, key );
         }
      }
   }
}



char *parse_token( char *line, char *token )
{
   
   while (*line == ' ')
      ++line;

   
   while (*line != ' ' && *line != '\0' && *line != '\n')
      *token++ = *line++;
   *token = '\0';

   
   if (*line != '\0' && *line != '\n')
      return( line );
   else
      return( NULL );
}



int  search( char *token, CONFIG_DEFS list[], int num )
{
int bot;
int mid;
int top;
int rc;

   bot = 0;
   top = num;
   while (bot <= top) {
      mid = (bot + top) / 2;
      rc = stricmp( token, list[mid].key );
      if (rc == 0)
         return( list[mid].key_index );
      else if (rc < 0)
         top = mid - 1;
      else
         bot = mid + 1;
   }
   return( ERROR );
}



void parse_macro( int macro_key, char *residue )
{
int  rc;
char literal[1042];
char *l;
int  key_no;

   
   initialize_macro( macro_key );
   while (residue != NULL) {
      
      while (*residue == ' ')
         ++residue;

     
      if (*residue == ';')
         residue = NULL;

     
      else if (*residue == '\"') {
         rc = parse_literal( macro_key, residue, literal, &residue );
         if (rc == OK) {
            l = literal;
            while (*l != '\0'  &&  rc == OK) {
               rc = record_keys( macro_key, *l );
               ++l;
            }
         } else {
            printf( "==> %s", line_in );
            printf( "Literal not recognized: line %u  : literal  %s\n", line_no, literal );
         }

      
      } else {
         residue = parse_token( residue, literal );
         key_no = search( literal, valid_keys, AVAIL_KEYS );
         if (key_no != ERROR)
            record_keys( macro_key, key_no+256 );
         else {
            printf( "==> %s", line_in );
            printf( "Unrecognized key: line %u  : key %s\n", line_no, literal );
         }
      }
   }
   check_macro( macro_key );
}



int  parse_literal( int macro_key, char *line, char *literal, char **residue )
{
int quote_state = 1;    

   line++;
   
   while (*line != '\0' && *line != '\n') {
      if (*line != '\"')
         *literal++ = *line++;
      else {
         if (*(line+1) == '\"') {
            *literal++ = '\"';
            line++;
            line++;
         } else {
            line++;
            --quote_state;
            break;
         }
      }
   }
   *literal = '\0';

   
   if (*line != '\0' && *line != '\n')
      *residue = line;
   else
      *residue = NULL;
   if (quote_state != 0) {
      *residue = NULL;
      return( ERROR );
   } else
      return( OK );
}



void initialize_macro( int macro_key )
{
register int next;
int  prev;

   next = macros.first_stroke[macro_key];

  
   if (next != STROKE_LIMIT+1) {
      do {
         prev = next;
         next = macros.strokes[next].next;
         macros.strokes[prev].key  = MAX_KEYS+1;
         macros.strokes[prev].next = STROKE_LIMIT+1;
         ++stroke_count;
      } while (next != -1);
   }

  
   for (next=0; macros.strokes[next].next != STROKE_LIMIT+1;)
      next++;
   macros.first_stroke[macro_key] = next;
   macros.strokes[next].key  = -1;
   macros.strokes[next].next = -1;
}



void clear_previous_macro( int macro_key )
{
register int next;
int prev;

   next = macros.first_stroke[macro_key];

   
   if (next != STROKE_LIMIT+1) {
      do {
         prev = next;
         next = macros.strokes[next].next;
         macros.strokes[prev].key  = MAX_KEYS+1;
         macros.strokes[prev].next = STROKE_LIMIT+1;
      } while (next != -1);
   }

   macros.first_stroke[macro_key] = STROKE_LIMIT+1;
}



void check_macro( int macro_key )
{
register int next;
register int key;

  
   key = macro_key;
   if (key != 0) {
      next = macros.first_stroke[key];
      if (macros.strokes[next].key == -1) {
         macros.strokes[next].key  = MAX_KEYS+1;
         macros.strokes[next].next = STROKE_LIMIT+1;
         macros.first_stroke[key-256] = STROKE_LIMIT+1;
         if (key_func.key[key] == PlayBack)
            key_func.key[key] = 0;
      }
   }
}



int  record_keys( int macro_key, int key )
{
register int next;
register int prev;
int func;
int rc;

   rc = OK;
   if (stroke_count == 0) {
      printf( "==> %s", line_in );
      printf( "No more room in macro buffer:  line %u\n",
               line_no );
      rc = ERROR;
   } else {
      func = getfunc( key );
      if (func != RecordMacro && func != SaveMacro && func != LoadMacro &&
          func != ClearAllMacros) {

         
         next = macros.first_stroke[macro_key];
         if (macros.strokes[next].next != STROKE_LIMIT+1) {
            while (macros.strokes[next].next != -1)
               next = macros.strokes[next].next;
         }
         prev = next;

       
         if (macros.strokes[next].key != -1) {
            for (; next < STROKE_LIMIT &&
                         macros.strokes[next].next != STROKE_LIMIT+1;)
               next++;
            if (next == STROKE_LIMIT) {
               for (next=0; next < prev &&
                            macros.strokes[next].next != STROKE_LIMIT+1;)
                  next++;
            }
         }
         if (next == prev && macros.strokes[prev].key != -1) {
            rc = ERROR;
         } else {
        
            macros.strokes[prev].next = next;
            macros.strokes[next].next = -1;
            macros.strokes[next].key  = key;
            stroke_count--;
         }
      }
   }
   return( rc );
}



void new_sort_order( unsigned char *residue, unsigned char *sort )
{
int i;

   sort += 33;
   for (i=33; *residue != '\0'  &&  *residue != '\n' && i <= 255; i++)
      *sort++ = *residue++;
}



int  get_stroke_count( void )
{
int count = 0;
int i;

   for (i=0; i<STROKE_LIMIT; i++)
      if (macros.strokes[i].next == STROKE_LIMIT+1)
         ++count;
   return( count );
}



etfunc( int c )
{
register int i = c;

   if (i <= 256)
      i = 0;
   else
      i = key_func.key[i-256];
   return( i );
}



void clear_previous_twokey( int two_key )
{
int i;

   for (i=0; i < MAX_TWO_KEYS; i++) {
      if (two_key == two_key_list.key[i].parent_key) {
         two_key_list.key[i].parent_key = 0;
         two_key_list.key[i].child_key  = 0;
         two_key_list.key[i].func       = 0;
      }
   }
}


int  insert_twokey( int parent_key, int child_key, int func_no )
{
register int i;
int  rc;

   for (i=0; i < MAX_TWO_KEYS; i++) {
      if (parent_key == two_key_list.key[i].parent_key) {
         if (child_key == two_key_list.key[i].child_key) {
            two_key_list.key[i].parent_key = 0;
            two_key_list.key[i].child_key  = 0;
            two_key_list.key[i].func       = 0;
         }
      }
   }
   for (i=0; i < MAX_TWO_KEYS; i++) {
      if (two_key_list.key[i].parent_key == 0) {
         two_key_list.key[i].parent_key = parent_key;
         two_key_list.key[i].child_key  = child_key;
         two_key_list.key[i].func       = func_no;
         break;
      }
   }
   rc = OK;
   if (i == MAX_TWO_KEYS)
      rc = ERROR;
   return( rc );
}
