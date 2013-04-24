


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <conio.h>
#include <assert.h>
#if defined( __MSC__ )
   #include <malloc.h>          
   #if defined( toupper )
       #undef toupper
   #endif
#else
   #include <alloc.h>           
#endif



#if defined( __MSC__ )
   #define  ASSEMBLE   _asm
#else
   #define  ASSEMBLE   asm
#endif


#define MAX_COLS            80  
#define MAX_LINES           24  
#define BUFF_SIZE         1042  
#define MAX_LINE_LENGTH   1040  
#define FNAME_LENGTH        45  
#define NO_MARKERS           3  
#define UNDO_STACK_LEN     200  


#define READ_LENGTH             1024
#define DEFAULT_BIN_LENGTH      64


#define REGX_SIZE               200     



#define ERROR             (-1)  
#define OK                   0  
#define TRUE                 1  
#define FALSE                0  

#define MAX_KEYS           256  
#define MAX_TWO_KEYS       128  
#define STROKE_LIMIT      1024  


#define STACK_UNDERFLOW      1  
#define STACK_OVERFLOW       2  
#define SAS_P               20  
#define NUM_FUNCS          139

#define NUM_COLORS          14  


#define RTURN           262           
#define ESC             258           
#define CONTROL_BREAK   269           



#define WARNING         1    
#define FATAL           2    
#define INFO            3    



#define NOTMARKED       0    
#define BOX             1    
#define LINE            2    
#define STREAM          3    

#define MOVE            1
#define DELETE          2
#define COPY            3
#define KOPY            4
#define FILL            5
#define OVERLAY         6
#define NUMBER          7
#define SWAP            8

#define LEFT            1
#define RIGHT           2

#define ASCENDING       1
#define DESCENDING      2



#define LOCAL           1
#define NOT_LOCAL       2
#define GLOBAL          3

#define CURLINE         1
#define NOTCURLINE      2


#define BOYER_MOORE     0
#define REG_EXPRESSION  1

#define CLR_SEARCH      0
#define WRAPPED         1
#define SEARCHING       2
#define REPLACING       3
#define NFA_GAVE_UP     4

#define IGNORE          1
#define MATCH           2

#define PROMPT          1
#define NOPROMPT        2

#define FORWARD         1
#define BACKWARD        2

#define BEGIN           1
#define END             2


#define BEGINNING       1
#define CURRENT         2


#define NO_WRAP         0
#define FIXED_WRAP      1
#define DYNAMIC_WRAP    2


#define MM_DD_YY        0
#define DD_MM_YY        1
#define YY_MM_DD        2
#define MM_DD_YYYY      3
#define DD_MM_YYYY      4
#define YYYY_MM_DD      5

#define _12_HOUR        0
#define _24_HOUR        1



#define EXIST           0
#define WRITE           2
#define READ            4
#define READ_WRITE      6

#define NORMAL          0x00
#define READ_ONLY       0x01
#define HIDDEN          0x02
#define SYSTEM          0x04
#define VOLUME_LABEL    0x08
#define SUBDIRECTORY    0x10
#define ARCHIVE         0x20


#define RETRY           1
#define ABORT           2
#define FAIL            3



#define NATIVE          1
#define CRLF            2
#define LF              3
#define BINARY          4
#define TEXT            5

#define OVERWRITE       1
#define APPEND          2


#define EOL_CHAR        0x11
#define RULER_PTR       0x19
#define RULER_FILL      0x2e
#define RULER_TICK      0x04
#define LM_CHAR         0xb4
#define RM_CHAR_RAG     0x3c
#define RM_CHAR_JUS     0xc3
#define PGR_CHAR        0x14



#define VERTICAL_CHAR   0xba


#define SMALL_INS       0
#define BIG_INS         1



#define A_YES           1
#define A_NO            2
#define A_ALWAYS        3
#define A_QUIT          4
#define A_ABORT         5
#define A_OVERWRITE     6
#define A_APPEND        7


#define VIDEO_INT       0x10

#define HERC_REVERSE    0x70
#define HERC_UNDER      0x01
#define HERC_NORMAL     0x07
#define HERC_HIGH       0x0f

#define COLOR_HEAD      0x4b
#define COLOR_TEXT      0x07
#define COLOR_DIRTY     0x02
#define COLOR_MODE      0x17
#define COLOR_BLOCK     0x71
#define COLOR_MESSAGE   0x0f
#define COLOR_HELP      0x1a
#define COLOR_DIAG      0x0e
#define COLOR_EOF       0x09
#define COLOR_CURL      0x0f
#define COLOR_RULER     0x02
#define COLOR_POINTER   0x0a
#define COLOR_OVRS      0x00

#define COLOR_80        3
#define MONO_80         7

#define VGA             3
#define EGA             2
#define CGA             1
#define MDA             0


#define SAS_DELIMITERS  " \n"


typedef unsigned char far * text_ptr;


typedef struct s_line_list {
   text_ptr line;                       
   int      len;                        
   int      dirty;                      
   struct s_line_list far *next;        
   struct s_line_list far *prev;        
} line_list_struc;

typedef line_list_struc far *line_list_ptr;


struct vcfg {
   int color;
   int rescan;
   int mode;
   int far *videomem;
};


typedef struct {
   char sig[8];                 
   int  clr[2][NUM_COLORS];     
} COLORS;



typedef struct {
   int parent_key;
   int child_key;
   int func;
} TWO_KEY_TYPE;


typedef struct {
   char sig[8];
   TWO_KEY_TYPE key[MAX_TWO_KEYS];
} TWO_KEY;


typedef struct {
   char sig[8];
   unsigned char key[MAX_KEYS];
} KEY_FUNC;



typedef struct {
   char fname[14];              
   long fsize;                  
} FTYPE;



typedef struct {
   int  row;                    
   int  col;                    
   int  wid;                    
   int  hgt;                    
   int  max_cols;               
   int  max_lines;              
   int  cnt;                    
   int  cols;                   
   int  lines;                  
   int  prow;                   
   int  vcols;                  
   int  nfiles;                 
   int  avail;                  
   int  select;                 
   int  flist_col[5];           
} DIRECTORY;


typedef struct {
   int  pattern_length;                 
   int  search_defined;                 
   unsigned char pattern[MAX_COLS];     
   int  forward_md2;                    
   int  backward_md2;                   
   char skip_forward[256];              
   char skip_backward[256];             
} boyer_moore_type;



typedef struct {
   char sig[8];                 
   int  color_scheme;           
   int  sync;                   
   int  sync_sem;               
   int  record;                 
   int  insert;                 
   int  indent;                 
   int  ptab_size;              
   int  ltab_size;              
   int  smart_tab;              
   int  inflate_tabs;           
   int  search_case;            
   int  enh_kbd;                
   int  cursor_size;            
   char *eof;                   
   int  control_z;              
   int  crlf;                   
   int  trailing;               
   int  show_eol;               
   int  word_wrap;              
   int  left_margin;            
   int  parg_margin;            
   int  right_margin;           
   int  right_justify;          
   int  format_sem;             
   int  undo_max;               
   int  do_backups;             
   int  ruler;                  
   int  date_style;             
   int  time_style;             
} mode_infos;



typedef struct {
   int nlines;                  
   int ncols;                   
   int line_length;             
   int mode_line;               
   int head_color;              
   int text_color;              
   int dirty_color;             
   int mode_color;              
   int block_color;             
   int message_color;           
   int help_color;              
   int diag_color;              
   int eof_color;               
   int curl_color;              
   int ruler_color;             
   int ruler_pointer;           
   int hilited_file;            
   int overscan;                
   int old_overscan;            
   int adapter;                 
   unsigned int overw_cursor;   
   unsigned int insert_cursor;  
   char far *display_address;   
} displays;



typedef struct s_window {
   struct s_file_infos *file_info;       
   line_list_ptr ll;            
   int  ccol;                   
   int  rcol;                   
   int  bcol;                   
   int  cline;                  
   long rline;                  
   long bin_offset;             
   int  top_line;               
   int  bottom_line;            
   int  vertical;               
   int  start_col;              
   int  end_col;                
   int  page;                   
   int  visible;                
   int  letter;                 
   int  ruler;                  
   char ruler_line[MAX_COLS+2]; 
   struct s_window *next;       
   struct s_window *prev;       
} WINDOW;



typedef struct {
   char          reserved[21];
   unsigned char attrib;
   unsigned      time;
   unsigned      date;
   long          size;
   char          name[13];
} DTA;



typedef struct {
   WINDOW *current_window;      
   struct s_file_infos *current_file; 
   struct s_file_infos *file_list; 
   WINDOW *window_list;         
   int  window_count;           
   int  file_count;             
   int  arg;                    
   int  found_first;            
   int  argc;                   
   char **argv;                 
   char path[MAX_COLS];         
   DTA  dta;                    
   int  file_mode;              
   int  file_chunk;             
   int  sas_defined;            
   int  sas_search_type;        
   int  sas_arg;                
   int  sas_argc;               
   char **sas_argv;             
   char sas_tokens[MAX_COLS];   
   char sas_path[MAX_COLS];     
   char *sas_arg_pointers[SAS_P]; 
   int  sas_found_first;        
   DTA  sas_dta;                
   long sas_rline;              
   int  sas_rcol;               
   int  sas_mode;               
   int  sas_chunk;              
   line_list_ptr sas_ll;        
   int  marked;                 
   int  prompt_line;            
   int  prompt_col;             
   struct s_file_infos *marked_file;   marked block */
   char rw_name[MAX_COLS];      
   char pattern[MAX_COLS];      
   char subst[MAX_COLS];        
   int  replace_flag;           
   int  replace_defined;        
   int  overlap;                
   line_list_ptr buff_node;     
   int  copied;                 
   char line_buff[BUFF_SIZE*2+8]; 
   char tabout_buff[BUFF_SIZE+8]; 
   int  line_buff_len;          
   int  tabout_buff_len;        
   int  command;                
   int  key_pressed;            
   int  key_pending;            
   int  first_key;              
   int  wrapped;                
   int  stop;                   
   int  control_break;          
   int  recording_key;          
   int  stroke_count;           
   int  macro_next;             
   int  macro_executing;        
   int  mstack_pointer;         
   int  current_macro;          
   int  screen_display;         
} status_infos;



typedef struct {
   long rline;                  
   int  rcol;                   
   int  ccol;                   
   int  bcol;                   
   int  marked;                 
} MARKER;


typedef struct s_file_infos {
   line_list_ptr line_list;    
   line_list_ptr line_list_end; 
   line_list_ptr undo_top;     
   line_list_ptr undo_bot;     
   int  undo_count;            
   MARKER marker[NO_MARKERS];  
   long length;                
   int  modified;              
   int  dirty;                 
   int  new_file;              
   int  backed_up;             
   int  crlf;                  
   int  open_mode;             
   char file_name[MAX_COLS];   
   char backup_fname[MAX_COLS];
   int  block_type;            
   line_list_ptr block_start;  
   line_list_ptr block_end;    
   int  block_bc;              
   long block_br;              
   int  block_ec;              
   long block_er;              
   int  file_no;               
   int  ref_count;             
   int  next_letter;           
   unsigned int  file_attrib;  
   struct s_file_infos *next;  
   struct s_file_infos *prev;  
} file_infos;



typedef struct {
  int key;      
  int next;     
} STROKES;


typedef struct {
   char sig[8];                         
   int  first_stroke[MAX_KEYS];         
   STROKES strokes[STROKE_LIMIT];       
} MACRO;



typedef struct {
   int key;                             
   int macro;                           
} MACRO_STACK;



typedef struct {
   int  flag;                   
   int  code;                   
   int  rw;                     
   int  drive;                  
   int  extended;               
   int  class;                  
   int  action;                 
   int  locus;                  
   int  dattr;                  
   char dname[10];              
} CEH;



typedef struct {
   text_ptr pivot_ptr;
   int  direction;
   unsigned char *order_array;
   int  block_len;
   int  pivot_len;
   int  bc;
   int  ec;
} SORT;


typedef struct {
   unsigned char ignore[256];
   unsigned char match[256];
} SORT_ORDER;



typedef struct {
   int       defined;           
   int       leading;           f */
   int       all_space;         f */
   int       blank_lines;       f */
   int       ignore_eol;        f */
   WINDOW    *w1;               
   WINDOW    *w2;               
   line_list_ptr  d1;           
   line_list_ptr  d2;           
   long      rline1;            
   long      rline2;            
   long      bin_offset1;       
   long      bin_offset2;       
} DIFF;


typedef struct {
   int  pattern_length;                 
   int  search_defined;                 
   int  node_count;                     
   unsigned char pattern[MAX_COLS];     
} REGX_INFO;


typedef struct {
   int  node_type[REGX_SIZE];
   int  term_type[REGX_SIZE];
   int  c[REGX_SIZE];
   char *class[REGX_SIZE];
   int  next1[REGX_SIZE];
   int  next2[REGX_SIZE];
} NFA_TYPE;


typedef struct parse_tree {
   int node_type;
   int leaf_char;
   char *leaf_string;
   struct parse_tree *r_pt;
   struct parse_tree *l_pt;
} PARSE_TREE;
