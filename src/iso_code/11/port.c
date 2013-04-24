
#include "tdestr.h"
#include "common.h"
#include "tdefunc.h"
#include "define.h"



void far * my_malloc( size_t size, int *rc )
{
void far *mem;

   assert( size < MAX_LINE_LENGTH );

   if (size == 0)

     
      mem = NULL;
   else {

#if defined( __MSC__ )
      mem = _fmalloc( size );
#else
      mem = farmalloc( size );
#endif

     
      if (mem == NULL)
         *rc = ERROR;
   }
   return( mem );
}



void my_free( void far *mem )
{
   assert( mem != NULL );

#if defined( __MSC__ )
   _ffree( mem );
#else
   farfree( mem );
#endif
}



int  my_findfirst( DTA far *dta, char far *path, int f_attr )
{
void far *old_dta;
void far *new_dta;
int  rc;

   new_dta = (void far *)dta;

   ASSEMBLE {

        mov     ah, 0x2f                
        int     0x21                    
        mov     WORD PTR old_dta, bx    
        mov     ax, es
        mov     WORD PTR old_dta+2, ax  

        push    ds                      
        mov     dx, WORD PTR new_dta    
        mov     ax, WORD PTR new_dta+2  
        mov     ds, ax                  
        mov     ah, 0x1a                
        int     0x21                    
        pop     ds                      

     push    ds                      
        mov     cx, WORD PTR f_attr     
        mov     dx, WORD PTR path       
        mov     ax, WORD PTR path+2     
        mov     ds, ax                  
        mov     ah, 0x4e                
        int     0x21                    
        pop     ds                      


        jc      an_error                
        xor     ax, ax                  
   }
an_error:

   ASSEMBLE {
        mov     WORD PTR rc, ax         


        push    ds                      
        mov     dx, WORD PTR old_dta    
        mov     ax, WORD PTR old_dta+2  
        mov     ds, ax                  
        mov     ah, 0x1a                
        int     0x21                    
        pop     ds                      
   }
   if (ceh.flag == ERROR)
      rc = ERROR;
   return( rc );
}



int  my_findnext( DTA far *dta )
{
void far *old_dta;
void far *new_dta;
int  rc;

   new_dta = (void far *)dta;

   ASSEMBLE {


        mov     ah, 0x2f                
        int     0x21                    
        mov     WORD PTR old_dta, bx    
        mov     ax, es
        mov     WORD PTR old_dta+2, ax  


        push    ds                      
        mov     dx, WORD PTR new_dta    
        mov     ax, WORD PTR new_dta+2  
        mov     ds, ax                  
        mov     ah, 0x1a                
        int     0x21                    
        pop     ds                      


        mov     ah, 0x4f                
        int     0x21                    


        jc      an_error                
        xor     ax, ax                  
   }
an_error:

   ASSEMBLE {
        mov     WORD PTR rc, ax         


        push    ds                      
        mov     dx, WORD PTR old_dta    
        mov     ax, WORD PTR old_dta+2  
        mov     ds, ax                  
        mov     ah, 0x1a                
        int     0x21                    
        pop     ds                      
   }
   if (ceh.flag == ERROR)
      rc = ERROR;
   return( rc );
}
