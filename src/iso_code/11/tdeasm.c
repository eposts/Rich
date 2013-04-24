
#include "tdestr.h"
#include "common.h"
#include "tdefunc.h"


unsigned long ptoul( void far *s )
{
   ASSEMBLE {
        mov     ax, WORD PTR s          
        mov     dx, WORD PTR s+2        
        mov     bx, dx          
        mov     cl, 12          
        shr     dx, cl          
        mov     cl, 4           
        shl     bx, cl          
        add     ax, bx          
        adc     dx, 0           
   }
}



text_ptr tabout( text_ptr s, int *len )
{
text_ptr to;
int space;
int col;
int i;
int tab_size;
int show_tab;
int tab_len;


   tab_size = mode.ptab_size;
   show_tab = mode.show_eol;
   to  = (text_ptr)g_status.tabout_buff;
   i = tab_len  = *len;

   ASSEMBLE {
        push    si
        push    di
        push    ds
        push    es

        mov     bx, WORD PTR tab_size   
        xor     cx, cx                  

        mov     di, WORD PTR to
        mov     ax, WORD PTR to+2
        mov     es, ax                  
        mov     si, WORD PTR s
        mov     ax, WORD PTR s+2
        mov     ds, ax                  
   }
top:

   ASSEMBLE {
        cmp     cx, MAX_LINE_LENGTH     
        jge     get_out

        cmp     WORD PTR i, 0           
        jle     get_out

        lodsb                           
        cmp     al, 0x09                
        je      expand_tab

        stosb                           
        inc     cx                      
        dec     WORD PTR i              
        jmp     SHORT top
   }
expand_tab:

   ASSEMBLE {
        mov     ax, cx
        xor     dx, dx                  
        IDIV    bx                      
        mov     ax, bx                  
        sub     ax, dx                  
        mov     dx, ax                  
        add     cx, ax                  
        cmp     cx, MAX_LINE_LENGTH     
        jge     get_out                 
        mov     ax, ' '                 
        cmp     WORD PTR show_tab, 0    
        je      do_the_tab
        mov     ax, 0x09                
   }
do_the_tab:

   ASSEMBLE {
        stosb                           
        dec     dx
        cmp     dx, 0                   
        jle     end_of_space            
        add     WORD PTR tab_len, dx    
        mov     ax, ' '                 
   }
space_fill:

   ASSEMBLE {
        cmp     dx, 0                   
        jle     end_of_space            
        stosb                           
        dec     dx                      
        jmp     SHORT space_fill        
  }
end_of_space:

   ASSEMBLE {
        dec     WORD PTR i
        jmp     SHORT top
   }
get_out:

   ASSEMBLE {
        pop     es
        pop     ds
        pop     di
        pop     si
   }
   if (tab_len > MAX_LINE_LENGTH)
      tab_len = MAX_LINE_LENGTH;
   *len = g_status.tabout_buff_len = tab_len;
   return( (text_ptr)g_status.tabout_buff );


}
