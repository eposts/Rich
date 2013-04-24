
#pragma inline

#include <dos.h>
#include "pport.h"

#define XT_CODE                  

#if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
#define LARGE_DATA
#else
#undef LARGE_DATA
#endif


#define INPUT_SYNC_BIT  0x80  
#define OUTPUT_SYNC_BIT 0x10  


static unsigned Current_port;     



int pp_initialize(int port)        

{                                
	unsigned p;
	static unsigned int far *bios_ptr = MK_FP(0x37, 8);
    
	if (port > 2 || port < 0) return(NO_PORT);  
	p = *(port + bios_ptr);
	if (p == 0) return(NO_PORT);
	else
	{  outportb(p, 0);            
	Current_port = p;
	return(OK);
	}
}



void pp_wait_quiet(void)               
{                                      
	unsigned i;                         
	unsigned char x, y, z;              
	
#define WAIT_COUNT   20000u            
	
	
	x = inportb(Current_port + 1);
	for (i=0;  i<WAIT_COUNT;  i++)      
	{  y = inportb(Current_port +1);
	if (y != x)
	{  x = y;
	i = 0;
	}
	}
	
	
	z = inportb(Current_port);         
	z &= OUTPUT_SYNC_BIT;               
	
	
	while(1)                            
	{  z ^= OUTPUT_SYNC_BIT;
	outportb(Current_port, z);       
	for (i=0;  i<WAIT_COUNT;  i++)   
	{  y = inportb(Current_port + 1);
	if(y != x) break;
	}
	if (i >= WAIT_COUNT) break;      
	x = y;
	}
	pp_initialize(Current_port);        
}                                      




int pp_send_byte(unsigned char data)     
{                                        
	asm mov  bl, data                     
		asm mov  dx, Current_port             
		asm inc  dx                           
		asm xor  cx, cx
L0:
	asm in   al, dx                       
		asm test al, INPUT_SYNC_BIT
		asm jnz  L1
		asm loop L0
		
		return(START_TIMEOUT);
	
L1:
	asm dec  dx                           
		asm mov  al, bl
		asm and  al, 0xf
		asm or   al, OUTPUT_SYNC_BIT
		asm out  dx, al
		
		asm inc  dx
		asm xor  cx, cx
L2:                                      
	asm in   al, dx
		asm test al, INPUT_SYNC_BIT
		asm jz   L3                   	 
		asm loop L2
		
		return(SYNC_TIMEOUT);
	
L3:
	asm dec  dx                           
		
#ifdef XT_CODE
		asm mov  cl, 4
		asm shr  bl, cl
#else
		asm shr  bl, 4
#endif   
		
		asm mov  al, bl
		asm out  dx, al 
		
		return(OK);
}



int pp_send_n_bytes(int count, unsigned char *input)  
{                                      
	asm push si                         
		asm push di                         
		
#ifdef LARGE_DATA
		asm les  si, input
#else
		asm mov  si, input
#endif
		
		asm mov  di, count
		asm mov  bh, INPUT_SYNC_BIT
		asm mov  dx, Current_port
		
MAIN_LOOP:
	
#ifdef LARGE_DATA
	asm mov  bl, es:[si]
#else
		asm mov  bl, [si]
#endif
		
		asm inc  dx
		asm xor  cx, cx
L0:
	asm in   al, dx   	            
		asm test al, bh
		asm jnz  L1
		asm loop L0
		
		asm jmp  BREAK_LOOP
		
L1:
	asm dec  dx
		asm mov  al, bl
		asm and  al, 0xf
		asm or   al, OUTPUT_SYNC_BIT
		asm out  dx, al
		
		asm inc  dx
		asm xor  cx, cx
L2:
	asm in   al, dx
		asm test al, bh
		asm jz   L3                   
		asm loop L2
		
		asm jmp  BREAK_LOOP
		
L3:
	asm dec  dx
		
#ifdef XT_CODE
		asm mov  cl, 4
		asm shr  bl, cl
#else
		asm shr  bl, 4
#endif   
		
		asm mov  al, bl
		asm out  dx, al 
		
		asm inc  si                   
		asm dec  di
		asm jnz  MAIN_LOOP
		
		asm inc  dx
L6:                        
	
	asm in   al, dx         
        asm test al, bh
        asm jz  L6              
		
		
BREAK_LOOP:
	asm mov  ax, count
		asm sub  ax, di               
		asm pop  di
		asm pop  si   
		return(_AX);
	
}



int pp_read_byte(unsigned char *output)   
{
	asm mov  dx, Current_port
		asm xor  cx, cx
		asm inc  dx
		
L0:                                       
	asm in   al, dx
		asm test al, INPUT_SYNC_BIT
		asm jz   L1
		asm loop L0
		
		return(START_TIMEOUT);
	
L1:asm in   al, dx                  
	   
	   asm mov  ah, al                  
	   asm dec  dx
	   asm mov  al, OUTPUT_SYNC_BIT
	   asm out  dx, al                  
	   
#ifdef XT_CODE
	   asm shr  ah, 1
	   asm shr  ah, 1
	   asm shr  ah, 1
#else
	   asm shr  ah, 3
#endif
	   
	   asm xor  cx, cx
	   asm inc  dx                      
L2:
   asm in   al, dx
	   asm test al, INPUT_SYNC_BIT
	   asm jnz  L3
	   asm loop L2
	   
	   return(SYNC_TIMEOUT);
   
L3:asm in   al, dx               
	   asm shl  al, 1
	   asm and  al, 0xf0
	   asm or   ah, al               
	   
	   asm dec  dx
	   asm xor  al, al
	   asm out  dx, al               
	   
#ifdef LARGE_DATA                
	   asm les  bx, output
	   asm mov  es:[bx], ah
#else
	   asm mov  bx, output
	   asm mov  [bx], ah
#endif
	   
	   return(OK);
}


int pp_read_n_bytes(int count, unsigned char *output)  
{                                   
	asm push si
		asm push di
		
#ifdef LARGE_DATA
		asm les  di, output
#else
		asm mov  di, output
#endif
		
		asm mov  si, count
		asm mov  bh, INPUT_SYNC_BIT
		asm mov  dx, Current_port
		
MAIN_LOOP:
	asm xor  cx, cx
		asm inc  dx
		
L0:
	asm in   al, dx
		asm test al, bh
		asm jz   L1
		asm loop L0
		
		asm jmp  BREAK_LOOP
		
L1:asm in   al, dx               
   asm mov  ah, al
   asm dec  dx
   asm mov  al, OUTPUT_SYNC_BIT
   asm out  dx, al
   
#ifdef XT_CODE
   asm shr  ah, 1
   asm shr  ah, 1
   asm shr  ah, 1
#else
   asm shr  ah, 3
#endif
   
   asm xor  cx, cx
   asm inc  dx
L2:
	asm in   al, dx
		asm test al, bh
		asm jnz  L3
		asm loop L2
		
		asm jmp  BREAK_LOOP
		
L3:asm in   al, dx               
   asm shl  al, 1
   asm and  al, 0xf0
   asm or   ah, al
   
   asm dec  dx
   asm xor  al, al
   asm out  dx, al
   
#ifdef LARGE_DATA
   asm mov  es:[di], ah
#else
   asm mov  [di], ah
#endif
   
   asm inc  di
   asm dec  si
   asm jnz  MAIN_LOOP
   
BREAK_LOOP:
	pp_delay();                
	
	asm mov  ax, count
		asm sub  ax, si            
		
		asm pop  di
		asm pop  si
		return(_AX);
}



#define NDEL   16

void pp_delay(void)
{
	int i;
	
	i = 0;
	while(i < NDEL) i++;
}



int pp_send_data_block(int count, unsigned char *input) 
{
	unsigned csum;
	unsigned char *p, *pend;
	int c2[2];
	int j;
	
	asm mov  dx, 0             
		asm mov  cx, count
		asm shr  cx, 1             
		asm mov  ah, 0 
		asm push ds
#ifdef LARGE_DATA
		asm lds  si, input
#else
		asm mov  si, input
#endif
		asm cld
L1:
	asm lodsw
		asm add  dx, ax
		asm loop L1
		asm pop  ds
		
		asm mov  csum, dx
		
		c2[0] = c2[1] = count;
	j = pp_send_n_bytes(4, (char *)c2);
	if (j == 0) return(CHAR1_TIMEOUT);
	if (j != 4) return(GEN_TIMEOUT);
	
	j = pp_send_n_bytes(count, input);
	if (j != count) return(GEN_TIMEOUT);
	
	j = pp_send_n_bytes(2, (char *)&csum);
	if (j != 2) return(GEN_TIMEOUT);
	
	return(OK);
}



int pp_read_data_block(int *count, unsigned char *output)


{
	int  k, i;
	unsigned csum_in, csum_cal;
	int c[2];
	
	k = pp_read_n_bytes(4, (unsigned char *)c);
#ifdef DEBUG
	printf("%04x", k);
	printf(" %04x %04x\n", c[0], c[1]);
#endif
	if (k == 0) return(CHAR1_TIMEOUT);
	if (k != 4) return(GEN_TIMEOUT);
	
	if (c[1] != c[0]) return(COUNT_FAIL);
	
	k = pp_read_n_bytes(c[1], output);
#ifdef DEBUG
	printf("%04x", k);
	for (i=0;  i<k;  i++) printf(" %02x", output[i]);
	printf("\n");
#endif
	if (k != c[0]) return(GEN_TIMEOUT);
	
	k = pp_read_n_bytes(2, (unsigned char *)&csum_in);
#ifdef DEBUG
	printf("%04x", k);
	printf(" %04x\n", csum_in);
#endif
	if (k != 2) return(GEN_TIMEOUT);
	
	asm mov  dx, 0                   
		asm mov  cx, c[0]
		asm shr  cx, 1 
		asm mov  ah, 0 
		asm push ds
#ifdef LARGE_DATA
		asm lds  si, output
#else
		asm mov  si, output
#endif
		asm cld
L1:
	asm lodsw
		asm add  dx, ax
		asm loop L1
		asm pop  ds
		
		asm mov  csum_cal, dx
		
		if (csum_in != csum_cal) return(CSUM_FAIL);
		
		*count = c[0];
		return(OK);
}
