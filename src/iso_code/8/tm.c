






#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


#define   IADDR_SIZE  1024 
#define   DADDR_SIZE  1024 
#define   NO_REGS 8
#define   PC_REG  7

#define   LINESIZE  121
#define   WORDSIZE  20



typedef enum {
   opclRR,     
   opclRM,     
   opclRA      
   } OPCLASS;

typedef enum {
   
   opHALT,    
   opIN,      
   opOUT,     
   opADD,    
   opSUB,    
   opMUL,    
   opDIV,    reg(t) */
   opRRLim,   

   
   opLD,      
   opST,      
   opRMLim,   

   
   opLDA,     
   opLDC,     
   opJLT,     
   opJLE,     
   opJGT,     
   opJGE,     
   opJEQ,     
   opJNE,     
   opRALim    
   } OPCODE;

typedef enum {
   srOKAY,
   srHALT,
   srIMEM_ERR,
   srDMEM_ERR,
   srZERODIVIDE
   } STEPRESULT;

typedef struct {
      int iop  ;
      int iarg1  ;
      int iarg2  ;
      int iarg3  ;
   } INSTRUCTION;


int iloc = 0 ;
int dloc = 0 ;
int traceflag = FALSE;
int icountflag = FALSE;

INSTRUCTION iMem [IADDR_SIZE];
int dMem [DADDR_SIZE];
int reg [NO_REGS];

char * opCodeTab[]
        = {"HALT","IN","OUT","ADD","SUB","MUL","DIV","????",
            
           "LD","ST","????", 
           "LDA","LDC","JLT","JLE","JGT","JGE","JEQ","JNE","????"
           
          };

char * stepResultTab[]
        = {"OK","Halted","Instruction Memory Fault",
           "Data Memory Fault","Division by 0"
          };

char pgmName[20];
FILE *pgm  ;

char in_Line[LINESIZE] ;
int lineLen ;
int inCol  ;
int num  ;
char word[WORDSIZE] ;
char ch  ;
int done  ;


int opClass( int c )
{ if      ( c <= opRRLim) return ( opclRR );
  else if ( c <= opRMLim) return ( opclRM );
  else                    return ( opclRA );
} 


void writeInstruction ( int loc )
{ printf( "%5d: ", loc) ;
  if ( (loc >= 0) && (loc < IADDR_SIZE) )
  { printf("%6s%3d,", opCodeTab[iMem[loc].iop], iMem[loc].iarg1);
    switch ( opClass(iMem[loc].iop) )
    { case opclRR: printf("%1d,%1d", iMem[loc].iarg2, iMem[loc].iarg3);
                   break;
      case opclRM:
      case opclRA: printf("%3d(%1d)", iMem[loc].iarg2, iMem[loc].iarg3);
                   break;
    }
    printf ("\n") ;
  }
} 


void getCh (void)
{ if (++inCol < lineLen)
  ch = in_Line[inCol] ;
  else ch = ' ' ;
} 


int nonBlank (void)
{ while ((inCol < lineLen)
         && (in_Line[inCol] == ' ') )
    inCol++ ;
  if (inCol < lineLen)
  { ch = in_Line[inCol] ;
    return TRUE ; }
  else
  { ch = ' ' ;
    return FALSE ; }
} 


int getNum (void)
{ int sign;
  int term;
  int temp = FALSE;
  num = 0 ;
  do
  { sign = 1;
    while ( nonBlank() && ((ch == '+') || (ch == '-')) )
    { temp = FALSE ;
      if (ch == '-')  sign = - sign ;
      getCh();
    }
    term = 0 ;
    nonBlank();
    while (isdigit(ch))
    { temp = TRUE ;
      term = term * 10 + ( ch - '0' ) ;
      getCh();
    }
    num = num + (term * sign) ;
  } while ( (nonBlank()) && ((ch == '+') || (ch == '-')) ) ;
  return temp;
} 


int getWord (void)
{ int temp = FALSE;
  int length = 0;
  if (nonBlank ())
  { while (isalnum(ch))
    { if (length < WORDSIZE-1) word [length++] =  ch ;
      getCh() ;
    }
    word[length] = '\0';
    temp = (length != 0);
  }
  return temp;
} 


int skipCh ( char c  )
{ int temp = FALSE;
  if ( nonBlank() && (ch == c) )
  { getCh();
    temp = TRUE;
  }
  return temp;
} 


int atEOL(void)
{ return ( ! nonBlank ());
} 


int error( char * msg, int lineNo, int instNo)
{ printf("Line %d",lineNo);
  if (instNo >= 0) printf(" (Instruction %d)",instNo);
  printf("   %s\n",msg);
  return FALSE;
} 


int readInstructions (void)
{ OPCODE op;
  int arg1, arg2, arg3;
  int loc, regNo, lineNo;
  for (regNo = 0 ; regNo < NO_REGS ; regNo++)
      reg[regNo] = 0 ;
  dMem[0] = DADDR_SIZE - 1 ;
  for (loc = 1 ; loc < DADDR_SIZE ; loc++)
      dMem[loc] = 0 ;
  for (loc = 0 ; loc < IADDR_SIZE ; loc++)
  { iMem[loc].iop = opHALT ;
    iMem[loc].iarg1 = 0 ;
    iMem[loc].iarg2 = 0 ;
    iMem[loc].iarg3 = 0 ;
  }
  lineNo = 0 ;
  while (! feof(pgm))
  { fgets( in_Line, LINESIZE-2, pgm  ) ;
    inCol = 0 ; 
    lineNo++;
    lineLen = strlen(in_Line)-1 ;
    if (in_Line[lineLen]=='\n') in_Line[lineLen] = '\0' ;
    else in_Line[++lineLen] = '\0';
    if ( (nonBlank()) && (in_Line[inCol] != '*') )
    { if (! getNum())
        return error("Bad location", lineNo,-1);
      loc = num;
      if (loc > IADDR_SIZE)
        return error("Location too large",lineNo,loc);
      if (! skipCh(':'))
        return error("Missing colon", lineNo,loc);
      if (! getWord ())
        return error("Missing opcode", lineNo,loc);
      op = opHALT ;
      while ((op < opRALim)
             && (strncmp(opCodeTab[op], word, 4) != 0) )
          op++ ;
      if (strncmp(opCodeTab[op], word, 4) != 0)
          return error("Illegal opcode", lineNo,loc);
      switch ( opClass(op) )
      { case opclRR :
        
        if ( (! getNum ()) || (num < 0) || (num >= NO_REGS) )
            return error("Bad first register", lineNo,loc);
        arg1 = num;
        if ( ! skipCh(','))
            return error("Missing comma", lineNo, loc);
        if ( (! getNum ()) || (num < 0) || (num >= NO_REGS) )
            return error("Bad second register", lineNo, loc);
        arg2 = num;
        if ( ! skipCh(',')) 
            return error("Missing comma", lineNo,loc);
        if ( (! getNum ()) || (num < 0) || (num >= NO_REGS) )
            return error("Bad third register", lineNo,loc);
        arg3 = num;
        break;

        case opclRM :
        case opclRA :
        
        if ( (! getNum ()) || (num < 0) || (num >= NO_REGS) )
            return error("Bad first register", lineNo,loc);
        arg1 = num;
        if ( ! skipCh(','))
            return error("Missing comma", lineNo,loc);
        if (! getNum ())
            return error("Bad displacement", lineNo,loc);
        arg2 = num;
        if ( ! skipCh('(') && ! skipCh(',') )
            return error("Missing LParen", lineNo,loc);
        if ( (! getNum ()) || (num < 0) || (num >= NO_REGS))
            return error("Bad second register", lineNo,loc);
        arg3 = num;
        break;
        }
      iMem[loc].iop = op;
      iMem[loc].iarg1 = arg1;
      iMem[loc].iarg2 = arg2;
      iMem[loc].iarg3 = arg3;
    }
  }
  return TRUE;
} 



STEPRESULT stepTM (void)
{ INSTRUCTION currentinstruction  ;
  int pc  ;
  int r,s,t,m  ;
  int ok ;

  pc = reg[PC_REG] ;
  if ( (pc < 0) || (pc > IADDR_SIZE)  )
      return srIMEM_ERR ;
  reg[PC_REG] = pc + 1 ;
  currentinstruction = iMem[ pc ] ;
  switch (opClass(currentinstruction.iop) )
  { case opclRR :
    
      r = currentinstruction.iarg1 ;
      s = currentinstruction.iarg2 ;
      t = currentinstruction.iarg3 ;
      break;

    case opclRM :
    
      r = currentinstruction.iarg1 ;
      s = currentinstruction.iarg3 ;
      m = currentinstruction.iarg2 + reg[s] ;
      if ( (m < 0) || (m > DADDR_SIZE))
         return srDMEM_ERR ;
      break;

    case opclRA :
    
      r = currentinstruction.iarg1 ;
      s = currentinstruction.iarg3 ;
      m = currentinstruction.iarg2 + reg[s] ;
      break;
  } 

  switch ( currentinstruction.iop)
  { 
    case opHALT :
    
      printf("HALT: %1d,%1d,%1d\n",r,s,t);
      return srHALT ;
      

    case opIN :
    
      do
      { printf("Enter value for IN instruction: ") ;
        fflush (stdin);
        fflush (stdout);
        gets(in_Line);
        lineLen = strlen(in_Line) ;
        inCol = 0;
        ok = getNum();
        if ( ! ok ) printf ("Illegal value\n");
        else reg[r] = num;
      }
      while (! ok);
      break;

    case opOUT :  
      printf ("OUT instruction prints: %d\n", reg[r] ) ;
      break;
    case opADD :  reg[r] = reg[s] + reg[t] ;  break;
    case opSUB :  reg[r] = reg[s] - reg[t] ;  break;
    case opMUL :  reg[r] = reg[s] * reg[t] ;  break;

    case opDIV :
    
      if ( reg[t] != 0 ) reg[r] = reg[s] / reg[t];
      else return srZERODIVIDE ;
      break;

    
    case opLD :    reg[r] = dMem[m] ;  break;
    case opST :    dMem[m] = reg[r] ;  break;

    
    case opLDA :    reg[r] = m ; break;
    case opLDC :    reg[r] = currentinstruction.iarg2 ;   break;
    case opJLT :    if ( reg[r] <  0 ) reg[PC_REG] = m ; break;
    case opJLE :    if ( reg[r] <=  0 ) reg[PC_REG] = m ; break;
    case opJGT :    if ( reg[r] >  0 ) reg[PC_REG] = m ; break;
    case opJGE :    if ( reg[r] >=  0 ) reg[PC_REG] = m ; break;
    case opJEQ :    if ( reg[r] == 0 ) reg[PC_REG] = m ; break;
    case opJNE :    if ( reg[r] != 0 ) reg[PC_REG] = m ; break;

    
  } 
  return srOKAY ;
} 


int doCommand (void)
{ char cmd;
  int stepcnt=0, i;
  int printcnt;
  int stepResult;
  int regNo, loc;
  do
  { printf ("Enter command: ");
    fflush (stdin);
    fflush (stdout);
    gets(in_Line);
    lineLen = strlen(in_Line);
    inCol = 0;
  }
  while (! getWord ());

  cmd = word[0] ;
  switch ( cmd )
  { case 't' :
    
      traceflag = ! traceflag ;
      printf("Tracing now ");
      if ( traceflag ) printf("on.\n"); else printf("off.\n");
      break;

    case 'h' :
    
      printf("Commands are:\n");
      printf("   s(tep <n>      "\
             "Execute n (default 1) TM instructions\n");
      printf("   g(o            "\
             "Execute TM instructions until HALT\n");
      printf("   r(egs          "\
             "Print the contents of the registers\n");
      printf("   i(Mem <b <n>>  "\
             "Print n iMem locations starting at b\n");
      printf("   d(Mem <b <n>>  "\
             "Print n dMem locations starting at b\n");
      printf("   t(race         "\
             "Toggle instruction trace\n");
      printf("   p(rint         "\
             "Toggle print of total instructions executed"\
             " ('go' only)\n");
      printf("   c(lear         "\
             "Reset simulator for new execution of program\n");
      printf("   h(elp          "\
             "Cause this list of commands to be printed\n");
      printf("   q(uit          "\
             "Terminate the simulation\n");
      break;

    case 'p' :
    
      icountflag = ! icountflag ;
      printf("Printing instruction count now ");
      if ( icountflag ) printf("on.\n"); else printf("off.\n");
      break;

    case 's' :
    
      if ( atEOL ())  stepcnt = 1;
      else if ( getNum ())  stepcnt = abs(num);
      else   printf("Step count?\n");
      break;

    case 'g' :   stepcnt = 1 ;     break;

    case 'r' :
    
      for (i = 0; i < NO_REGS; i++)
      { printf("%1d: %4d    ", i,reg[i]);
        if ( (i % 4) == 3 ) printf ("\n");
      }
      break;

    case 'i' :
    
      printcnt = 1 ;
      if ( getNum ())
      { iloc = num ;
        if ( getNum ()) printcnt = num ;
      }
      if ( ! atEOL ())
        printf ("Instruction locations?\n");
      else
      { while ((iloc >= 0) && (iloc < IADDR_SIZE)
                && (printcnt > 0) )
        { writeInstruction(iloc);
          iloc++ ;
          printcnt-- ;
        }
      }
      break;

    case 'd' :
    
      printcnt = 1 ;
      if ( getNum  ())
      { dloc = num ;
        if ( getNum ()) printcnt = num ;
      }
      if ( ! atEOL ())
        printf("Data locations?\n");
      else
      { while ((dloc >= 0) && (dloc < DADDR_SIZE)
                  && (printcnt > 0))
        { printf("%5d: %5d\n",dloc,dMem[dloc]);
          dloc++;
          printcnt--;
        }
      }
      break;

    case 'c' :
    
      iloc = 0;
      dloc = 0;
      stepcnt = 0;
      for (regNo = 0;  regNo < NO_REGS ; regNo++)
            reg[regNo] = 0 ;
      dMem[0] = DADDR_SIZE - 1 ;
      for (loc = 1 ; loc < DADDR_SIZE ; loc++)
            dMem[loc] = 0 ;
      break;

    case 'q' : return FALSE;  

    default : printf("Command %c unknown.\n", cmd); break;
  }  
  stepResult = srOKAY;
  if ( stepcnt > 0 )
  { if ( cmd == 'g' )
    { stepcnt = 0;
      while (stepResult == srOKAY)
      { iloc = reg[PC_REG] ;
        if ( traceflag ) writeInstruction( iloc ) ;
        stepResult = stepTM ();
        stepcnt++;
      }
      if ( icountflag )
        printf("Number of instructions executed = %d\n",stepcnt);
    }
    else
    { while ((stepcnt > 0) && (stepResult == srOKAY))
      { iloc = reg[PC_REG] ;
        if ( traceflag ) writeInstruction( iloc ) ;
        stepResult = stepTM ();
        stepcnt-- ;
      }
    }
    printf( "%s\n",stepResultTab[stepResult] );
  }
  return TRUE;
} 






main( int argc, char * argv[] )
{ if (argc != 2)
  { printf("usage: %s <filename>\n",argv[0]);
    exit(1);
  }
  strcpy(pgmName,argv[1]) ;
  if (strchr (pgmName, '.') == NULL)
     strcat(pgmName,".tm");
  pgm = fopen(pgmName,"r");
  if (pgm == NULL)
  { printf("file '%s' not found\n",pgmName);
    exit(1);
  }

  
  if ( ! readInstructions ())
         exit(1) ;
  
  
  
  printf("TM  simulation (enter h for help)...\n");
  do
     done = ! doCommand ();
  while (! done );
  printf("Simulation done.\n");
  return 0;
}
