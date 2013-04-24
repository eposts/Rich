/****************************************************/
/* 文件：scan.c                                     */
/* TINY扫描程序的执行代码							*/
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"

/* 扫描程序有限自动机的状态 */
typedef enum
   { START,INASSIGN,INCOMMENT,INNUM,INID,DONE }
   StateType;

/* 用来保存当前保留字或者标识符 */
char tokenString[MAXTOKENLEN+1];

/* BUFLEN定义源文件行中的最大字符数 */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* 保留当前行 */
static int linepos = 0; /* 保存LineBuf中的当前位置 */
static int bufsize = 0; /* 当前缓存中保存的字符数 */
static int EOF_flag = FALSE; /* 当遇到EOF（文件结束符）调整ungetNextChar的操作 */

/* getNextCharlineBuf取得下一个非空格的字符。如果读完了缓存中的全部字符，
   就把新的一行读入缓存 */
static int getNextChar(void)
{ if (!(linepos < bufsize))
  { lineno++;
    if (fgets(lineBuf,BUFLEN-1,source))
    { if (EchoSource) fprintf(listing,"%4d: %s",lineno,lineBuf);
      bufsize = strlen(lineBuf);
      linepos = 0;
      return lineBuf[linepos++];
    }
    else
    { EOF_flag = TRUE;
      return EOF;
    }
  }
  else return lineBuf[linepos++];
}

/* ungetNextChar把一个字符退回到lineBuf缓存中 */
static void ungetNextChar(void)
{ if (!EOF_flag) linepos-- ;}

/* 保留字表*/
static struct
    { char* str;
      TokenType tok;
    } reservedWords[MAXRESERVED]
   = {{"if",IF},{"then",THEN},{"else",ELSE},{"end",END},
      {"repeat",REPEAT},{"until",UNTIL},{"read",READ},
      {"write",WRITE}};

/* T I N Y对保留字的识别是通过首先将它们看作是标识符，
 * 之后再在保留字表中查找它们来完成的。
 * 我们的扫描程序使用了一种非常简便的方法—线性搜索，
 * 即按顺序从开头到结尾搜索表格。这对于小型表格不成问题。
 */

static TokenType reservedLookup (char * s)
{ int i;
  for (i=0;i<MAXRESERVED;i++)
    if (!strcmp(s,reservedWords[i].str))
      return reservedWords[i].tok;
  return ID;
}

/****************************************/
/* 分析程序的主函数  */
/****************************************/
/* 耗输入字符并根据图8.2.1
 * 中的DFA返回下一个被识别的记号
 */
TokenType getToken(void)
{  /* tokenString的引索 */
   int tokenStringIndex = 0;
   /* 保存当前要返回的记号 */
   TokenType currentToken;
   /* 当前的状态－总是从START开始 */
   StateType state = START;
   /* 是否保存到tokenString的标记 */
   int save;
   while (state != DONE)
   { int c = getNextChar();
     save = TRUE;
     switch (state)
     { case START:
         if (isdigit(c))
           state = INNUM;
         else if (isalpha(c))
           state = INID;
         else if (c == ':')
           state = INASSIGN;
         else if ((c == ' ') || (c == '\t') || (c == '\n'))
           save = FALSE;
         else if (c == '{')
         { save = FALSE;
           state = INCOMMENT;
         }
         else
         { state = DONE;
           switch (c)
           { case EOF:
               save = FALSE;
               currentToken = ENDFILE;
               break;
             case '=':
               currentToken = EQ;
               break;
             case '<':
               currentToken = LT;
               break;
             case '+':
               currentToken = PLUS;
               break;
             case '-':
               currentToken = MINUS;
               break;
             case '*':
               currentToken = TIMES;
               break;
             case '/':
               currentToken = OVER;
               break;
             case '(':
               currentToken = LPAREN;
               break;
             case ')':
               currentToken = RPAREN;
               break;
             case ';':
               currentToken = SEMI;
               break;
             default:
               currentToken = ERROR;
               break;
           }
         }
         break;
       case INCOMMENT:
         save = FALSE;
         if (c == EOF)
         { state = DONE;
           currentToken = ENDFILE;
         }
         else if (c == '}') state = START;
         break;
       case INASSIGN:
         state = DONE;
         if (c == '=')
           currentToken = ASSIGN;
         else
		   /*退回到缓存中 */
           ungetNextChar();
           save = FALSE;
           currentToken = ERROR;
         }
         break;
       case INNUM:
         if (!isdigit(c))
         { /* 退回到缓存中 */
           ungetNextChar();
           save = FALSE;
           state = DONE;
           currentToken = NUM;
         }
         break;
       case INID:
         if (!isalpha(c))
         { /* 退回到缓存中 */
           ungetNextChar();
           save = FALSE;
           state = DONE;
           currentToken = ID;
         }
         break;
       case DONE:
       default: 
         fprintf(listing,"Scanner Bug: state= %d\n",state);
         state = DONE;
         currentToken = ERROR;
         break;
     }
     if ((save) && (tokenStringIndex <= MAXTOKENLEN))
       tokenString[tokenStringIndex++] = (char) c;
     if (state == DONE)
     { tokenString[tokenStringIndex] = '\0';
       if (currentToken == ID)
         currentToken = reservedLookup(tokenString);
     }
   }
   if (TraceScan) {
     fprintf(listing,"\t%d: ",lineno);
     printToken(currentToken,tokenString);
   }
   return currentToken;
} 

