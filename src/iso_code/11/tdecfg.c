


#include <bios.h>
#include <dos.h>
#include <io.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tdecfg.h"


struct vcfg cfg;                
FILE *tde_exe;                  

long sort_offset;
long mode_offset;
long color_offset;
long macro_offset;
long keys_offset;
long two_key_offset;
long help_offset;

struct screen cfg_choice[] = {
   {5,25,"1.  Change colors" },
   {7,25,"2.  Redefine keys" },
   {9,25,"3.  Install new help screen" },
  {11,25,"4.  Set default modes" },
  {13,25,"5.  Install permanent macro file" },
  {15,25,"6.  Read in a configuration file" },
  {17,25,"7.  Exit" },
 {20,20,"Please enter choice: " },
  {0,0,NULL}
};


char *greatest_composer_ever = "W. A. Mozart, 1756-1791";



void main( int argc, char *argv[] )
{
int  rc;
int  c;
char fname[82];
char *buff;

 
   if ((buff = malloc( 8200 )) == NULL) {
      puts( "\nNot enough memory." );
      exit( 1 );
   }

   puts( "\nEnter tde executable file name (<Enter> = \"tde.exe\")  :" );
   gets( fname );

   if (strlen(fname) == 0)
      strcpy( fname, "tde.exe" );

   if ((rc = access( fname, EXIST )) != 0) {
      puts( "\nFile not found." );
      exit( 1 );
   } else if ((tde_exe = fopen( fname, "r+b" )) == NULL ) {
      puts( "\nCannot open executable file." );
      exit( 2 );
   }
   if ((rc = find_offsets( buff )) == ERROR)
      puts( "\nFatal error finding offsets.\n" );
   free( buff );
   if (rc == ERROR)
      exit( 3 );

   video_config( );
   cls( );
   show_box( 0, 0, cfg_choice, NORMAL );
   for (rc=0; rc != 1;) {
      xygoto( 42, 20 );
      c = getkey( );
      while (c != '1' && c != '2' && c != '3' && c != '4' && c != '5' &&
             c != '6' && c != '7')
         c = getkey( );
      switch (c) {
         case '1' :
            tdecolor( );
            show_box( 0, 0, cfg_choice, NORMAL );
            break;
         case '2' :
            tdekeys( );
            show_box( 0, 0, cfg_choice, NORMAL );
            break;
         case '3' :
            tdehelp( );
            show_box( 0, 0, cfg_choice, NORMAL );
            break;
         case '4' :
            tdemodes( );
            show_box( 0, 0, cfg_choice, NORMAL );
            break;
         case '5' :
            tdemacro( );
            show_box( 0, 0, cfg_choice, NORMAL );
            break;
         case '6' :
            tdecfgfile( );
            show_box( 0, 0, cfg_choice, NORMAL );
            break;
         case '7' :
            rc = 1;
            break;
      }
   }
   fcloseall( );
   puts( " " );
   puts( " " );
}




void build_next_table( char *pattern, char *next )
{
int  j;
int  t;
int  len;

   len = strlen( pattern );
   t = next[0] = -1;
   j = 0;
   while (j < len) {
      while (t >= 0  &&  pattern[j] != pattern[t])
         t = (int)next[t];
      j++;
      t++;
      next[j] =  pattern[j] == pattern[t]  ?  next[t]  :  (char)t;
   }
}



int  find_offsets( char *buff )
{
long off;
int  m1, m2, m3, m4, m5, m6, m7;
char sig1[8] = { '\x00','\x01','\x02','\x03','\x04','\x05','\x06','\x07' };
char sig2[8] = "$ modes";
char sig3[8] = "$colors";
char sig4[8] = "$macbuf";
char sig5[8] = "$  keys";
char sig6[8] = "$twokey";
char sig7[8] = "$  help";
char sig1_next[8];
char sig2_next[8];
char sig3_next[8];
char sig4_next[8];
char sig5_next[8];
char sig6_next[8];
char sig7_next[8];
unsigned int cnt;
register char *b;

   
   build_next_table( sig1, sig1_next );
   build_next_table( sig2, sig2_next );
   build_next_table( sig3, sig3_next );
   build_next_table( sig4, sig4_next );
   build_next_table( sig5, sig5_next );
   build_next_table( sig6, sig6_next );
   build_next_table( sig7, sig7_next );

  
   m1 = m2 = m3 = m4 = m5 = m6 = m7 = 0;
   off = 100000L;
   sort_offset = mode_offset    = color_offset = macro_offset = 0L;
   keys_offset = two_key_offset = help_offset  = 0L;
   fseek( tde_exe, off, SEEK_SET );
   while (!feof( tde_exe )) {
      cnt = fread( buff, sizeof(char), 8192, tde_exe );
      b = (char *)buff;
      for (; cnt > 0; off++, cnt--, b++) {

        
         if (m1 < 8) {
            if (sig1[m1] == *b) {
               m1++;
               if (m1 == 8)
                  sort_offset = off - 7L;
            } else {
               while (m1 > 0  &&  *b != sig1[m1])
                  m1 = (int)sig1_next[m1];
               m1++;
            }
         }
         if (m2 < 8) {
            if (sig2[m2] == *b) {
               m2++;
               if (m2 == 8)
                  mode_offset = off - 7L;
            } else {
               while (m2 > 0  &&  *b != sig2[m2])
                  m2 = (int)sig2_next[m2];
               m2++;
            }
         }
         if (m3 < 8) {
            if (sig3[m3] == *b) {
               m3++;
               if (m3 == 8)
                  color_offset = off - 7L;
            } else {
               while (m3 > 0  &&  *b != sig3[m3])
                  m3 = (int)sig3_next[m3];
               m3++;
            }
         }
         if (m4 < 8) {
            if (sig4[m4] == *b) {
               m4++;
               if (m4 == 8)
                  macro_offset = off - 7L;
            } else {
               while (m4 > 0  &&  *b != sig4[m4])
                  m4 = (int)sig4_next[m4];
               m4++;
            }
         }
         if (m5 < 8) {
            if (sig5[m5] == *b) {
               m5++;
               if (m5 == 8)
                  keys_offset = off - 7L;
            } else {
               while (m5 > 0  &&  *b != sig5[m5])
                  m5 = (int)sig5_next[m5];
               m5++;
            }
         }
         if (m6 < 8) {
            if (sig6[m6] == *b) {
               m6++;
               if (m6 == 8)
                  two_key_offset = off - 7L;
            } else {
               while (m6 > 0  &&  *b != sig6[m6])
                  m6 = (int)sig6_next[m6];
               m6++;
            }
         }
         if (m7 < 8) {
            if (sig7[m7] == *b) {
               m7++;
               if (m7 == 8)
                  help_offset = off - 7L;
            } else {
               while (m7 > 0  &&  *b != sig7[m7])
                  m7 = (int)sig7_next[m7];
               m7++;
            }
         }
      }
   }
   m1 = OK;
   if (sort_offset  == 0L || mode_offset == 0L || color_offset   == 0L ||
       macro_offset == 0L || keys_offset == 0L || two_key_offset == 0L ||
       help_offset  == 0L)
      m1 = ERROR;
   return( m1 );
}



void xygoto( int col, int row )
{
union REGS inregs, outregs;

   inregs.h.ah = 2;
   inregs.h.bh = 0;
   inregs.h.dh = row;
   inregs.h.dl = col;
   int86( VIDEO_INT, &inregs, &outregs );
}



void video_config( void )
{
#pragma pack( 1 )    

struct LOWMEMVID
{
   char     vidmode;           
   unsigned scrwid;            
   unsigned scrlen;            
   unsigned scroff;            
   struct   LOCATE
   {
      unsigned char col;
      unsigned char row;
   } csrpos[8];                
   struct   CURSIZE
   {
      unsigned char end;
      unsigned char start;
   } csrsize;                  
   char      page;             
   unsigned  addr_6845;        
   char      crt_mode_set;     
   char      crt_palette;      
   char      system_stuff[29]; 
   char      rows;             
   unsigned  points;           
   char      ega_info;         
   char      info_3;           
} vid;
struct LOWMEMVID _far *pvid = &vid;

#pragma pack( )    

union REGS in, out;
unsigned char temp, active_display;

   
   movedata( 0, 0x449, FP_SEG( pvid ), FP_OFF( pvid ), sizeof( vid ) );

   cfg.rescan = FALSE;
   in.x.ax =  0x1a00;
   int86( VIDEO_INT, &in, &out );
   temp = out.h.al;
   active_display = out.h.bl;
   if (temp == 0x1a && (active_display == 7 || active_display == 8))
      cfg.adapter = VGA;
   else {
      in.h.ah =  0x12;
      in.h.bl =  0x10;
      int86( VIDEO_INT, &in, &out );
      if (out.h.bl != 0x10) {         
         if (vid.ega_info & 0x08) {
            if (vid.addr_6845 == 0x3d4)
               cfg.adapter = CGA;
            else
               cfg.adapter = MDA;
         } else
            cfg.adapter = EGA;
      } else if (vid.addr_6845 == 0x3d4)
         cfg.adapter = CGA;
      else
         cfg.adapter = MDA;
   }

   if (cfg.adapter == CGA)
      cfg.rescan = TRUE;

   cfg.mode = vid.vidmode;
   if (vid.addr_6845 == 0x3D4) {
      cfg.color = TRUE;
      FP_SEG( cfg.videomem ) = 0xb800;
   } else {
      cfg.color = FALSE;
      FP_SEG( cfg.videomem ) = 0xb000;
   }
   FP_OFF( cfg.videomem ) = 0x0000;
   if (cfg.color == TRUE)
      cfg.attr = COLOR_ATTR;
   else
      cfg.attr = MONO_ATTR;

   cfg.overscan = vid.crt_palette;
}


int getkey( void )
{
unsigned key;
unsigned lo;

  
   key = _bios_keybrd( 0 );
   lo = key & 0X00FF;
   lo = (int)((lo == 0) ? (((key & 0XFF00) >> 8) + 256) : lo);
   return( lo );
}



void s_output( char far *s, int line, int col, int attr )
{
int far *screen_ptr;
int max_col;
int off;

   max_col = 80;
   screen_ptr = cfg.videomem;
   off = line * 160 + col * 2;

   ASSEMBLE {
        push    ds              
        push    di              
        push    si              

        mov     bx, WORD PTR attr               
        mov     cx, WORD PTR col                
        mov     dx, WORD PTR max_col            
        mov     di, WORD PTR screen_ptr         
        add     di, WORD PTR off
        mov     ax, WORD PTR screen_ptr+2       
        mov     es, ax
        mov     si, WORD PTR s  
        or      si, si          
        je      getout          
        mov     ax, WORD PTR s+2        
        or      ax, ax          
        je      getout          
        mov     ds, ax          
        mov     ah, bl          
   }
top:

   ASSEMBLE {
        cmp     cx, dx          
        jge     getout          
        lodsb                   
        or      al, al          
        je      getout          
        stosw                   
        inc     cx              
        jmp     SHORT top       
   }
getout:

   ASSEMBLE {
        pop     si              
        pop     di              
        pop     ds              
   }
}



void hlight_line( int x, int y, int lgth, int attr )
{
int off, far *pointer;

   pointer = cfg.videomem;
   off = y * 160 + 2 * x + 1;  
   ASSEMBLE {
        push    di              

        mov     cx, lgth        

        mov     di, WORD PTR pointer    
        add     di, off                 
        mov     ax, WORD PTR pointer+2
        mov     es, ax
        mov     ax, attr        
   }
lite_len:

   ASSEMBLE {
        stosb                   
        inc     di              
        loop    lite_len        
        pop     di              
   }
}



void scroll_window( int lines, int r1, int c1, int r2, int c2, int attr )
{
char rah, ral;

   ASSEMBLE {
        mov     ax, lines
        cmp     ax, 0           
        jge     a1

        neg     ax                      
        mov     BYTE PTR ral, al        
        mov     BYTE PTR rah, 7         
        dec     r2                      
        jmp     SHORT a2
   }
a1:

   ASSEMBLE {
        mov     BYTE PTR ral, al        
        mov     BYTE PTR rah, 6         
   }
a2:

   ASSEMBLE {
        mov     ax, WORD PTR r1         
        mov     ch, al                  
        mov     ax, WORD PTR c1         
        mov     cl, al                  
        mov     ax, WORD PTR r2         
        mov     dh, al                  
        mov     ax, WORD PTR c2         
        mov     dl, al                  
        mov     ax, WORD PTR attr       
        mov     bh, al                  
        mov     ah, BYTE PTR rah        
        mov     al, BYTE PTR ral        
        push    bp                      
        int     VIDEO_INT               
        pop     bp
   }
}



void cls( void )
{
   scroll_window( 0, 0, 0, 24, 79, NORMAL );
}



void show_box( int x, int y, struct screen *p, int  attr )
{

   while (p->text) {
      s_output( p->text, p->row+y, p->col+x, attr );
      p++;
   }
}



void make_window( int col, int row, int width, int height, int attr )
{
   buf_box( col++, row++, width, height, attr );
   clear_window( col, row, width-2, height-2 );
}



void buf_box( int col, int row, int width, int height, int attr )
{
int  i;
int  row_count;
char string[82];

   if (height > 0 && width > 0 && height < 25 && width < 81) {
      row_count = 1;
      string[0]= U_LEFT;
      for (i=1; i<width-1; i++)
         string[i] = HOR_LINE;
      string[i++] = U_RIGHT; string[i] = '\0';
      s_output( string, row, col, attr );
      ++row_count;
      ++row;

      if (row_count < height) {
         string[0] = VER_LINE;
         string[1] = '\0';
         for (i=1; i<height-1; i++) {
            s_output( string, row, col, attr );
            s_output( string, row, col+width-1, attr );
            ++row;
            ++row_count;
         }
      }

      if (row_count <= height) {
         string[0] = L_LEFT;
         for (i=1; i<width-1; i++)
            string[i] = HOR_LINE;
         string[i++] = L_RIGHT; string[i] = '\0';
         s_output( string, row, col, attr );
      }
   }
}



void clear_window( int col, int row, int width, int height )
{

   scroll_window( 0, row, col, row+height-1, col+width-1, NORMAL );
}



void window_control( WINDOW **window_ptr, int action, int col, int row,
                     int width, int height )
{
WINDOW  *p;
size_t  store_me;

   if (action == SAVE) {
      p = (WINDOW *)malloc( sizeof(WINDOW) );
      if (p != NULL) {
         p->n = NULL;

         
         if (*window_ptr != NULL)
            p->n = *window_ptr;
         *window_ptr = p;
         store_me = (width * height) * sizeof( int );
         p->buf = (int *)malloc( store_me );
         save_window( p->buf, col, row, width, height );
      }
   } else if (action == RESTORE) {
      if (*window_ptr != NULL) {
         p = *window_ptr;
         restore_window( p->buf, col, row, width, height );

        
         *window_ptr = p->n;
         free( p->buf );
         free( p );
}  }  }



void save_window( int *destination, int col, int row, int width, int height )
{
int i, j, offset;
int far *pointer;

   pointer = cfg.videomem;
   offset = row * 80 + col;
   pointer += offset;
   for (i=0; i < height; i++) {
      for (j=0; j < width; j++)
         *destination++ = *(pointer + j);
      pointer += 80;
   }
}



void restore_window( int *source, int col, int row, int width, int height )
{
int i, j, offset;
int far *pointer;

   pointer = cfg.videomem;
   offset = row * 80 + col;
   pointer += offset;
   for (i=0; i < height; i++) {
      for (j=0; j < width; j++)
         *(pointer + j) = *source++;
      pointer += 80;
   }
}
