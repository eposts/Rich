
#if defined( __MSC__ )
   #define  ASSEMBLE   _asm
#else
   #define  ASSEMBLE   asm
#endif


#define TRUE            1
#define FALSE           0

#define OK              0
#define ERROR           (-1)


#define MAX_KEYS        256
#define AVAIL_KEYS      233


#define MAX_TWO_KEYS    128   
#define STROKE_LIMIT    1024


#define NUM_FUNC        139

#define NUM_COLORS      14

#define NUM_MODES       23

/*
 * mode indexes
 */
#define Ins              0
#define Ind              1
#define PTAB             2
#define LTAB             3
#define Smart            4
#define Write_Z          5
#define Crlf             6
#define Trim             7
#define Eol              8
#define WW               9
#define Left            10
#define Para            11
#define Right           12
#define Size            13
#define Backup          14
#define Ruler           15
#define Date            16
#define Time            17
#define Initcase        18
#define InflateTabs     19
#define Match           20
#define Ignore          21
#define JustRM          22



#define EXIST           0

#define VIDEO_INT       0x10

#define VGA             3
#define EGA             2
#define CGA             1
#define MDA             0

#define NORMAL          7
#define COLOR_ATTR      31
#define MONO_ATTR       112


#define ESC             27
#define LEFT            (75 | 0x100)
#define RIGHT           (77 | 0x100)
#define UP              (72 | 0x100)
#define DOWN            (80 | 0x100)
#define RTURN           13
#define PGUP            (73 | 0x100)
#define PGDN            (81 | 0x100)
#define F1              (59 | 0x100)
#define F2              (60 | 0x100)
#define F3              (61 | 0x100)
#define F5              (63 | 0x100)
#define F6              (64 | 0x100)
#define F7              (65 | 0x100)
#define F8              (66 | 0x100)
#define F10             (68 | 0x100)


#define U_LEFT          218
#define U_RIGHT         191
#define VER_LINE        179
#define HOR_LINE        196
#define L_LEFT          192
#define L_RIGHT         217

#define SAVE            0
#define RESTORE         1



typedef struct win {
   int x;
   int y;
   int *buf;
   struct win *n;
} WINDOW;



struct screen {
   unsigned int   row;
   unsigned int   col;
   unsigned char *text;
};



struct vcfg {
   int color;
   int rescan;
   int mode;
   int overscan;
   int adapter;
   int attr;
   int far *videomem;
};


typedef struct {
   char sig[8];                 
   int  clr[2][NUM_COLORS];     
} COLORS;



typedef struct {
   int dply_col;        
   int dply_row;        
   int line_length;     
   int avail_lines;     
   int v_row;           
   int select;          
   int num_entries;     
   int ulft_col;        
   int ulft_row;        
   int total_col;       
   int total_row;       
} HELP_WINDOW;



typedef struct {
   char *key;           
   int  key_index;      
   unsigned char func_index;     
} KEY_DEFS;



typedef struct {
   char *key;           
   int  key_index;      
} CONFIG_DEFS;



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
} MODE_INFO;


typedef struct {
   unsigned char ignore[256];
   unsigned char match[256];
} SORT_ORDER;



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



void main( int, char *[] );
void build_next_table( char *, char * );
int  find_offsets( char * );
void xygoto( int, int );
void video_config( void );
int  getkey( void );
void s_output( char far *, int, int, int );
void cls( void );
void hlight_line( int, int, int, int );
void c_off( void );
void c_on( void );
void scroll_window( int, int, int, int, int, int );
void cls( void );
void show_box( int, int, struct screen *, int );
void make_window( int, int, int, int, int );
void buf_box( int, int, int, int, int );
void clear_window( int, int, int, int );
void window_control( WINDOW **, int, int, int, int, int );
void save_window( int *, int, int, int, int );
void restore_window( int *, int, int, int, int );




void tdecolor( void );
void initialize_color( void );
void restore_overscan( int );
void show_init_sample( void );
void color_number( char *, int );
void current_color_number( char *, int );
void show_help_color( void );
void show_fileheader_color( void );
void show_text_color( void );
void show_curl_color( void );
void show_dirty_color( void );
void show_warning_color( void );
void show_mode_color( void );
void show_wrapped_color( void );
void show_overscan_color( void );
void show_eof_color( void );
void show_ruler_color( void );
void show_rulerptr_color( void );
void show_block_color( void );
void show_hilitedfile_color( void );
void change_colors( void );




void tdekeys( void );
void initialize_keys( void );
void show_key_def_list( HELP_WINDOW *, KEY_DEFS * );
void show_func_list( HELP_WINDOW *, char *[] );
void position_cursor( HELP_WINDOW *, int, int *, int *, int * );
void master_help( HELP_WINDOW *, KEY_DEFS *, struct screen *, char *, int * );
void new_assignment_help( HELP_WINDOW *, char *[], struct screen *, int * );
void save_and_draw( HELP_WINDOW *, struct screen *, WINDOW ** );




void tdehelp( void );




void tdemacro( void );




void tdemodes( void );
void initialize_modes( void );
void show_init_mode( void );
void show_insert_mode( void );
void show_indent_mode( void );
void show_smart_mode( void );
void show_ptabsize( void );
void show_ltabsize( void );
void show_controlz( void );
void show_eol_out( void );
void show_trail( void );
void show_eol_display( void );
void show_ww( void );
void show_left( void );
void show_para( void );
void show_right( void );
void show_cursor_size( void );
void show_backup_mode( void );
void show_ruler_mode( void );
void show_date_style( void );
void show_time_style( void );
void show_initcase( void );
void change_modes( void );




void tdecfgfile( void );
void parse_line( char * );
char *parse_token( char *, char * );
int  search( char *, CONFIG_DEFS [], int );
void parse_macro( int, char * );
int  parse_literal( int, char *, char *, char ** );
void initialize_macro( int );
void clear_previous_macro( int );
void check_macro( int );
int  record_keys( int, int );
void new_sort_order( unsigned char *, unsigned char * );
int  get_stroke_count( void );
int  getfunc( int );
void clear_previous_twokey( int );
int  insert_twokey( int, int, int );



#define   Help                        1
#define   Rturn                       2
#define   NextLine                    3
#define   BegNextLine                 4
#define   LineDown                    5
#define   LineUp                      6
#define   CharRight                   7
#define   CharLeft                    8
#define   PanRight                    9
#define   PanLeft                    10
#define   WordRight                  11
#define   WordLeft                   12
#define   ScreenDown                 13
#define   ScreenUp                   14
#define   EndOfFile                  15
#define   TopOfFile                  16
#define   BotOfScreen                17
#define   TopOfScreen                18
#define   EndOfLine                  19
#define   BegOfLine                  20
#define   JumpToLine                 21
#define   CenterWindow               22
#define   CenterLine                 23
#define   HorizontalScreenRight      24
#define   HorizontalScreenLeft       25
#define   ScrollDnLine               26
#define   ScrollUpLine               27
#define   PanUp                      28
#define   PanDn                      29
#define   ToggleOverWrite            30
#define   ToggleSmartTabs            31
#define   ToggleIndent               32
#define   ToggleWordWrap             33
#define   ToggleCRLF                 34
#define   ToggleTrailing             35
#define   ToggleZ                    36
#define   ToggleEol                  37
#define   ToggleSync                 38
#define   ToggleRuler                39
#define   ToggleTabInflate           40
#define   SetTabs                    41
#define   SetLeftMargin              42
#define   SetRightMargin             43
#define   SetParagraphMargin         44
#define   FormatParagraph            45
#define   FormatText                 46
#define   LeftJustify                47
#define   RightJustify               48
#define   CenterJustify              49
#define   Tab                        50
#define   BackTab                    51
#define   ParenBalance               52
#define   BackSpace                  53
#define   DeleteChar                 54
#define   StreamDeleteChar           55
#define   DeleteLine                 56
#define   DelEndOfLine               57
#define   WordDelete                 58
#define   AddLine                    59
#define   SplitLine                  60
#define   JoinLine                   61
#define   DuplicateLine              62
#define   AbortCommand               63
#define   UndoLine                   64
#define   UndoDelete                 65
#define   ToggleSearchCase           66
#define   FindForward                67
#define   FindBackward               68
#define   RepeatFindForward1         69
#define   RepeatFindForward2         70
#define   RepeatFindBackward1        71
#define   RepeatFindBackward2        72
#define   ReplaceString              73
#define   DefineDiff                 74
#define   RepeatDiff                 75
#define   MarkBox                    76
#define   MarkLine                   77
#define   MarkStream                 78
#define   UnMarkBlock                79
#define   FillBlock                  80
#define   NumberBlock                81
#define   CopyBlock                  82
#define   KopyBlock                  83
#define   MoveBlock                  84
#define   OverlayBlock               85
#define   DeleteBlock                86
#define   SwapBlock                  87
#define   BlockToFile                88
#define   PrintBlock                 89
#define   BlockExpandTabs            90
#define   BlockCompressTabs          91
#define   BlockIndentTabs            92
#define   BlockTrimTrailing          93
#define   BlockUpperCase             94
#define   BlockLowerCase             95
#define   BlockRot13                 96
#define   BlockFixUUE                97
#define   BlockEmailReply            98
#define   BlockStripHiBit            99
#define   SortBoxBlock              100
#define   DateTimeStamp             101
#define   EditFile                  102
#define   DirList                   103
#define   File                      104
#define   Save                      105
#define   SaveAs                    106
#define   SetFileAttributes         107
#define   EditNextFile              108
#define   DefineGrep                109
#define   RepeatGrep                110
#define   RedrawScreen              111
#define   SizeWindow                112
#define   SplitHorizontal           113
#define   SplitVertical             114
#define   NextWindow                115
#define   PreviousWindow            116
#define   ZoomWindow                117
#define   NextHiddenWindow          118
#define   SetMark1                  119
#define   SetMark2                  120
#define   SetMark3                  121
#define   GotoMark1                 122
#define   GotoMark2                 123
#define   GotoMark3                 124
#define   RecordMacro               125
#define   PlayBack                  126
#define   SaveMacro                 127
#define   LoadMacro                 128
#define   ClearAllMacros            129
#define   Pause                     130
#define   Quit                      131
#define   NextDirtyLine             132
#define   PrevDirtyLine             133
#define   FindRegX                  134
#define   RepeatFindRegX            135
#define   RepeatFindRegXBackward    136
#define   DefineRegXGrep            137
