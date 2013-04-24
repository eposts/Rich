

#include "tdestr.h"
#include "common.h"
#include "define.h"
#include "tdefunc.h"

#include <bios.h>               






void video_config( struct vcfg *cfg )
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
   char      skip[13];         
   char      kb_flag_3;        
} vid;
struct LOWMEMVID _far *pvid = &vid;
#pragma pack( )    

union REGS in, out;
unsigned char temp, active_display;

   
   _fmemmove( pvid, (void far *)0x00000449l, sizeof( vid ) );

   cfg->rescan = FALSE;
   in.x.ax =  0x1a00;
   int86( VIDEO_INT, &in, &out );
   temp = out.h.al;
   active_display = out.h.bl;
   if (temp == 0x1a && (active_display == 7 || active_display == 8))
      g_display.adapter = VGA;
   else {
      in.h.ah =  0x12;
      in.h.bl =  0x10;
      int86( VIDEO_INT, &in, &out );
      if (out.h.bl != 0x10) {         
         if (vid.ega_info & 0x08)
            g_display.adapter = vid.addr_6845 == 0x3d4 ? CGA : MDA;
         else
            g_display.adapter = EGA;
      } else if (vid.addr_6845 == 0x3d4)
         g_display.adapter = CGA;
      else
         g_display.adapter = MDA;
   }

   if (g_display.adapter == CGA || g_display.adapter == EGA) {
      if (g_display.adapter == CGA)
         cfg->rescan = TRUE;
      g_display.insert_cursor = mode.cursor_size == SMALL_INS ? 0x0607 : 0x0407;
      g_display.overw_cursor = mode.cursor_size == SMALL_INS ? 0x0407 : 0x0607;
   } else {
      g_display.insert_cursor = mode.cursor_size == SMALL_INS ? 0x0b0c : 0x070b;
      g_display.overw_cursor = mode.cursor_size == SMALL_INS ? 0x070b : 0x0b0c;
   }

   cfg->mode = vid.vidmode;
   if (vid.addr_6845 == 0x3D4) {
      cfg->color = TRUE;
      cfg->videomem = (void far *)0xb8000000l;
   } else {
      cfg->color = FALSE;
      cfg->videomem = (void far *)0xb0000000l;
   }

 
   g_display.old_overscan = vid.crt_palette;


   
   if ((vid.kb_flag_3 & 0x10) != 0)
      mode.enh_kbd = TRUE;
   else
      mode.enh_kbd = FALSE;
   if (mode.enh_kbd == FALSE)
      simulate_enh_kbd( 1 );
}




o routines, PC specific    */




int getkey( void )
{
unsigned key, num_lock, control, shift;
register scan;
register unsigned lo;


   while (waitkey( mode.enh_kbd ));

  
   if (mode.enh_kbd) {
      key = _bios_keybrd( 0x10 );
      lo  = _bios_keybrd( 0x12 );

      
      if ((key & 0x00ff) == 0x00e0 && (key & 0xff00) != 0)
         key = key & 0xff00;
   } else {
      key = _bios_keybrd( 0 );
      lo  = _bios_keybrd( 2 );
   }
   num_lock = lo & 0x20;
   control  = lo & 0x04;
   shift    = lo & 0x03;
   scan = (key & 0xff00) >> 8;
   lo = key & 0X00FF;

   if (lo == 0) {
     
      if (scan == 3)
         lo = 430;

      
      else
         lo = scan | 0x100;

  
   } else if (key == 0xffff)
      lo = CONTROL_BREAK;


   
   if (scan == 14 && lo == 0x7f)
      lo = 8;

  
   else if (scan == 0xe0) {
     
      if (lo == 13 && !shift)
         lo = 285;
      
      else if (lo == 13)
         lo = 298;
      
      else if (lo == 10)
         lo = 299;
   }

  
   if (lo < 32) {

  
      if (scan > 0x80)
         scan = 0;

     
      else if (scan == 1) {
         if (shift)
            lo = 259;
         else if (control)
            lo = 260;
         else
            lo = 258;
      }

      
      else if (scan == 28) {
         if (shift)
            lo = 263;
         else if (control)
            lo = 264;
         else
            lo = 262;
      }

   
      else if (scan == 14) {
         if (shift)
            lo = 266;
         else if (control)
            lo = 267;
         else
            lo = 265;
      }

     
      else if (scan == 15) {
         lo = 268;
      }

      
      else if (scan > 0)
         lo += 430;

   } else if (!num_lock) {
      switch (scan) {
        
         case 74 :
            lo = 423;
            break;

         
         case 78 :
            lo = 424;
            break;
      }
   }


   
   if (lo > 256 && (shift || control)) {

      
      if (shift) {
         if (lo >= 371 && lo <= 374)
            lo += 55;

        
         else if (lo >= 376 && lo <= 387)
            lo += 86;

        
         else if (lo >= 272 && lo <= 309)
            lo += 202;
      }
   }

   
   if (lo == 10  &&  scan != 0)
      lo = 425;
   return( lo );
}



int  waitkey( int enh_keyboard )
{
   return( _bios_keybrd( enh_keyboard ? 0x11 : 0x01 ) == 0 ? 1 : 0 );
}



int getfunc( int c )
{
register int i = c;
int  key_found;

   if (!g_status.key_pending) {
      i = c;
      if (i <= 256)
         i = 0;
      else
         i = key_func.key[i-256];
   } else {

      
      key_found = FALSE;
      for (i=0; i < MAX_TWO_KEYS; i++) {
         if (g_status.first_key == two_key_list.key[i].parent_key &&
                               c == two_key_list.key[i].child_key) {
            i = two_key_list.key[i].func;
            key_found = TRUE;
            break;
         }
      }
      if (key_found == FALSE)
         i = ERROR;
   }
   return( i );
}



int  two_key( WINDOW *arg_filler )
{
   s_output( "Next Key..", g_display.mode_line, 67, g_display.diag_color );
   g_status.key_pending = TRUE;
   g_status.first_key = g_status.key_pressed;
   return( OK );
}



void flush_keyboard( void )
{
   if (mode.enh_kbd) {
      while (!waitkey( mode.enh_kbd ))
         _bios_keybrd( 0x10 );
   } else {
      while (!waitkey( mode.enh_kbd ))
         _bios_keybrd( 0 );
   }
}










void xygoto( int col, int line )
{
union REGS inregs, outregs;

   inregs.h.ah = 2;
   inregs.h.bh = 0;
   inregs.h.dh = (unsigned char)line;
   inregs.h.dl = (unsigned char)col;
   int86( 0x10, &inregs, &outregs );
}



void update_line( WINDOW *window )
{
text_ptr text;      
char far *screen_ptr;
int  off;
int  attr;
int  line;
int  col;
int  bcol;
int  bc;
int  ec;
int  normal;
int  block;
int  max_col;
int  block_line;
int  len;
int  show_eol;
int  c;
long rline;
file_infos *file;

   if (window->rline > window->file_info->length || window->ll->len == EOF
             || !g_status.screen_display)
      return;
   file = window->file_info;
   max_col = window->end_col + 1 - window->start_col;
   line = window->cline;
   normal = window->ll->dirty == FALSE ? g_display.text_color : g_display.dirty_color;
   block = g_display.block_color;
   show_eol = mode.show_eol;

   screen_ptr = g_display.display_address;
   off = line * 160 + window->start_col * 2;

 
   text = window->ll->line;
   len  = window->ll->len;
   if (g_status.copied && ptoul( window->ll ) == ptoul( g_status.buff_node )) {
      text = (text_ptr)g_status.line_buff;
      len  = g_status.line_buff_len;
   }
   if (mode.inflate_tabs)
      text = tabout( text, &len );

 
   bc = window->bcol;
   if (bc > 0) {
      if (text == NULL) {
         show_eol = FALSE;
         len = 0;
      } else {
         if ((col = len) < bc) {
            bc = col;
            show_eol = FALSE;
         }
         text += bc;
         len  -= bc;
      }
   }

  
   if (len > max_col)
      len = max_col;

   bcol = window->bcol;
   rline = window->rline;
   if (file->block_type && rline >= file->block_br && rline <= file->block_er)
      block_line = TRUE;
   else
      block_line = FALSE;


   if (block_line == TRUE && (file->block_type == BOX ||
         (file->block_type == STREAM &&
         rline == file->block_br && rline == file->block_er))) {

     
      bc = file->block_bc;
      ec = file->block_ec;
      if (ec < bcol || bc >= bcol + max_col)
       
         ec = bc = max_col + 1;
      else if (ec < bcol + max_col) {
      
         ec = ec - bcol;
         if (bc < bcol)
            bc = 0;
         else
            bc = bc - bcol;
      } else if (bc < bcol + max_col) {
      
         bc = bc - bcol;
         if (ec > bcol + max_col)
            ec = max_col;
         else
            ec = ec - bcol;
      } else if (bc < bcol  &&  ec >= bcol + max_col) {
       
         bc = 0;
         ec = max_col;
      }


   ASSEMBLE {
      


        push    ds                      
        push    si                      
        push    di                      


        mov     ax, WORD PTR bc         
        mov     bl, al                  
        mov     ax, WORD PTR ec         
        mov     bh, al                  
        mov     ax, WORD PTR normal     
        mov     dl, al                  
        mov     ax, WORD PTR block      
        mov     dh, al                  
        mov     ax, WORD PTR max_col    
        mov     ch, al                  
        xor     cl, cl                  

        mov     di, WORD PTR screen_ptr         
        add     di, WORD PTR off                
        mov     ax, WORD PTR screen_ptr+2       
        mov     es, ax
        mov     si, WORD PTR text       
        mov     ax, WORD PTR text+2     
        mov     ds, ax                  
        cmp     si, 0                   
        jne     not_null                
        cmp     ax, 0                   
        je      dspl_eol                
   }
not_null:

   ASSEMBLE {
        cmp     cl, ch                  
        jge     getout                  
        cmp     cl, BYTE PTR len        
        je      dspl_eol                
   }
top:

   ASSEMBLE {
        lodsb                   
        mov     ah, dl          
        cmp     cl, bl          
        jl      ch_out1         
        cmp     cl, bh          
        jg      ch_out1         
        mov     ah, dh          
   }
ch_out1:

   ASSEMBLE {
        stosw                    attribute */
        inc     cl              
        cmp     cl, ch          
        jge     getout          
        cmp     cl, BYTE PTR len        
        jl      top             
   }
dspl_eol:

   ASSEMBLE {
        cmp     WORD PTR show_eol, FALSE        
        je      block_eol       
        mov     al, EOL_CHAR    
        mov     ah, dl          
        cmp     cl, bl          
        jl      ch_out2         
        cmp     cl, bh          
        jg      ch_out2         
        mov     ah, dh          
   }
ch_out2:

   ASSEMBLE {
        stosw                   
        inc     cl              
        cmp     cl, ch          
        je      getout          
   }
block_eol:

   ASSEMBLE {
        mov     al, ' '          spaces */
   }
b1:

   ASSEMBLE {
        mov     ah, dl          
        cmp     cl, bl          
        jl      ch_out3         
        cmp     cl, bh          
        jg      ch_out3         
        mov     ah, dh          
   }
ch_out3:

   ASSEMBLE {
        stosw                   
        inc     cl              
        cmp     cl, ch          
        jl      b1              
   }
getout:

   ASSEMBLE {
        pop     di
        pop     si
        pop     ds
      }


   } else if (block_line == TRUE && file->block_type == STREAM &&
              (rline == file->block_br || rline == file->block_er)) {
      if (rline == file->block_br)
         bc = file->block_bc;
      else {
         bc = file->block_ec + 1;
         ec = normal;
         normal = block;
         block = ec;
      }
      if (bc < bcol)
         bc = 0;
      else if (bc < bcol + max_col)
         bc = bc - bcol;
      else
         bc = max_col + 1;


   ASSEMBLE {
      
        push    ds                      
        push    si                      
        push    di                      


        mov     ax, WORD PTR bc         
        mov     bl, al                  
        mov     ax, WORD PTR len        
        mov     bh, al                  
        mov     ax, WORD PTR normal     
        mov     dl, al                  
        mov     ax, WORD PTR block      
        mov     dh, al                  
        mov     ax, WORD PTR max_col    
        mov     ch, al                  
        xor     cl, cl                  

        mov     di, WORD PTR screen_ptr         
        add     di, WORD PTR off                
        mov     ax, WORD PTR screen_ptr+2       
        mov     es, ax
        mov     si, WORD PTR text       
        mov     ax, WORD PTR text+2     
        mov     ds, ax                  
        cmp     si, 0                   
        jne     nott_null               
        cmp     ax, 0                   
        je      ddspl_eol               
   }
nott_null:

   ASSEMBLE {
        cmp     cl, ch          
        je      ggetout         
        cmp     cl, bh          
        je      ddspl_eol       
   }
ttop:

   ASSEMBLE {
        lodsb                   
        mov     ah, dl          
        cmp     cl, bl          
        jl      str_out1        
        mov     ah, dh          
   }
str_out1:

   ASSEMBLE {
        stosw                   
        inc     cl              
        cmp     cl, ch          
        je      ggetout         
        cmp     cl, bh          
        jl      ttop            
   }
ddspl_eol:

   ASSEMBLE {
        cmp     WORD PTR show_eol, FALSE        
        je      stream_eol      
        mov     al, EOL_CHAR    
        mov     ah, dl          
        cmp     cl, bl          
        jl      str_out2        
        mov     ah, dh          
   }
str_out2:

   ASSEMBLE {
        stosw                   
        inc     cl              
        cmp     cl, ch          
        jge     ggetout         
   }
stream_eol:

   ASSEMBLE {
        mov     al, ' '          spaces */
   }
c1:

   ASSEMBLE {
        mov     ah, dl          
        cmp     cl, bl          
        jl      str_out3        
        mov     ah, dh          
   }
str_out3:

   ASSEMBLE {
        stosw                   
        inc     cl              
        cmp     cl, ch          
        jl      c1              
   }
ggetout:

   ASSEMBLE {
        pop     di
        pop     si
        pop     ds
      }


   } else {
      if (block_line)
         attr = block;
      else
         attr = normal;

      assert( len >= 0 );
      assert( len <= MAX_COLS );

   ASSEMBLE {
  
        mov     dx, ds          
        push    di              
        push    si              


        mov     bx, WORD PTR attr               
        mov     ax, WORD PTR len                
        mov     bh, al                          
        mov     cx, WORD PTR max_col            
        mov     ch, cl                          
        xor     cl, cl                          
        mov     di, WORD PTR screen_ptr         
        add     di, WORD PTR off                
        mov     ax, WORD PTR screen_ptr+2       
        mov     es, ax
        mov     si, WORD PTR text       
        mov     ax, WORD PTR text+2     
        mov     ds, ax                  
        cmp     si, 0                   
        jne     nnot_null               
        cmp     ax, 0                   
        je      normeol                 
   }
nnot_null:

   ASSEMBLE {
        mov     ah, bl                  
        cmp     cl, ch          
        jge     getoutt         
        cmp     cl, bh          
        je      normeol         
   }
topp:

   ASSEMBLE {
        lodsb                   
        stosw                   
        inc     cl              
        cmp     cl, ch          
        jge     getoutt         
        cmp     cl, bh          
        jl      topp            
   }
normeol:

   ASSEMBLE {
        cmp     WORD PTR show_eol, FALSE        
        je      clreol          
        mov     al, EOL_CHAR    
        mov     ah, bl          
        stosw                   
        inc     cl              
        cmp     cl, ch          
        jge     getoutt         
   }
clreol:

   ASSEMBLE {
        mov     ah, bl          
        mov     al, ' '         
        sub     ch, cl          
        mov     cl, ch          
        xor     ch, ch          
        rep     stosw           
   }
getoutt:

   ASSEMBLE {
        pop     si
        pop     di
        mov     ds, dx
      }


   }
}



void c_output( int c, int col, int line, int attr )
{
void far *screen_ptr;
int  off;

   screen_ptr = (void far *)g_display.display_address;
   off = line * 160 + col * 2;

   ASSEMBLE {
        mov     bx, WORD PTR screen_ptr         
        add     bx, WORD PTR off                
        mov     ax, WORD PTR screen_ptr+2       
        mov     es, ax
        mov     cx, WORD PTR attr       
        mov     ah, cl                  
        mov     cx, WORD PTR c          
        mov     al, cl                  
        mov     WORD PTR es:[bx], ax    
   }


}



void s_output( char far *s, int line, int col, int attr )
{
void far *screen_ptr;
int  off;
int  max_col;

   max_col = g_display.ncols;
   screen_ptr = (void far *)g_display.display_address;
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
        cmp     al, 0x0a        
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

/
}



void eol_clear( int col, int line, int attr )
{
int  max_col;
void far *screen_ptr;
int  off;

   max_col = g_display.ncols;
   screen_ptr = (void far *)g_display.display_address;
   off = line * 160 + col * 2;

   ASSEMBLE {
        push    di                              

        mov     bx, WORD PTR attr               
        mov     dx, WORD PTR col                
        mov     cx, WORD PTR max_col            
        cmp     dx, cx                          
        jge     getout                          
        sub     cx, dx                          
        mov     di, WORD PTR screen_ptr         
        add     di, WORD PTR off                
        mov     ax, WORD PTR screen_ptr+2       
        mov     es, ax
        mov     ah, bl                          
        mov     al, ' '                         
        rep     stosw                           
   }
getout:

   ASSEMBLE {
        pop     di                              
   }


void window_eol_clear( WINDOW *window, int attr )
{
int  max_col;
void far *screen_ptr;
int  off;

   if (!g_status.screen_display)
      return;
   screen_ptr = (void far *)g_display.display_address;
   off = window->cline * 160 + window->start_col * 2;
   max_col = window->end_col + 1 - window->start_col;

   ASSEMBLE {
        push    di                              

        mov     bx, WORD PTR attr               
        mov     cx, WORD PTR max_col            
        mov     di, WORD PTR screen_ptr         
        add     di, WORD PTR off                
        mov     ax, WORD PTR screen_ptr+2       
        mov     es, ax
        mov     ah, bl                          
        mov     al, ' '                         
        rep     stosw                           

        pop     di                              
   }


}



void hlight_line( int x, int y, int lgth, int attr )
{
int  off;
void far *screen_ptr;

   screen_ptr = (void far *)g_display.display_address;
   off = y * 160 + 2 * x + 1;  

   ASSEMBLE {
        push    di              

        mov     cx, lgth        

        mov     di, WORD PTR screen_ptr 
        add     di, off                 
        mov     ax, WORD PTR screen_ptr+2
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



void cls( void )
{
int  line;

     line = g_display.nlines + 1;

   ASSEMBLE {
        xor     ch, ch                  
        xor     cl, cl                  
        mov     ax, WORD PTR line       
        mov     dh, al                  
        mov     dl, 79                  
        mov     bh, 7                   
        mov     al, 0                   
        mov     ah, 6                   
        push    bp                      
        int     0x10
        pop     bp
   }
}



void set_cursor_size( int csize )
{
   ASSEMBLE {
        mov     ah, 1                   
        mov     cx, WORD PTR csize      
        push    bp
        int     VIDEO_INT               
        pop     bp
   }
}



void set_overscan_color( int color )
{
   ASSEMBLE {
        mov     ah, 0x0b                
        mov     bl, BYTE PTR color      
        xor     bh, bh
        push    bp
        int     VIDEO_INT               
        pop     bp
   }
}



void save_screen_line( int col, int line, char *screen_buffer )
{
char far *p;

   p = g_display.display_address + line * 160 + col * 2;
   _fmemcpy( screen_buffer, p, 160 );
}



void restore_screen_line( int col, int line, char *screen_buffer )
{
char far *p;

   p = g_display.display_address + line * 160 + col * 2;
   _fmemcpy( p, screen_buffer, 160 );
}
