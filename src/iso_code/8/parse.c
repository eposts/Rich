/****************************************************/
/* 文件： parse.c                                   */
/* TINY编译器的语法分析执行程序						*/
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; /* 保留当前的记号 */

/* 递归调用时候的函数原型 */
static TreeNode * stmt_sequence(void);		/* 语句序列 */
static TreeNode * statement(void);			/* 语句 */
static TreeNode * if_stmt(void);			/* if语句 */
static TreeNode * repeat_stmt(void);		/* 循环语句 */
static TreeNode * assign_stmt(void);		/* 赋值语句 */
static TreeNode * read_stmt(void);			/* 输入语句 */
static TreeNode * write_stmt(void);			/* 输出语句 */
static TreeNode * exp(void);				/* 表达式 */
static TreeNode * simple_exp(void);			/* 简单表达式 */
static TreeNode * term(void);				/* 乘法项 */
static TreeNode * factor(void);				/* 因子 */

/* 显示出错信息*/
static void syntaxError(char * message)
{ fprintf(listing,"\n>>> ");
  fprintf(listing,"Syntax error at line %d: %s",lineno,message);
  Error = TRUE;
}

/* 保存向前看记号的静态变量token和检测输入的记号是否和预期相同 */
static void match(TokenType expected)
{ 
/*它找到匹配时就调用getToken，否则就声明出错*/
  if (token == expected) token = getToken();
  else {
    syntaxError("unexpected token -> ");
    printToken(token,tokenString);
    fprintf(listing,"      ");
  }
}
/* 用来匹配非终结符stmt_sequence */
TreeNode * stmt_sequence(void)
{ TreeNode * t = statement();
  TreeNode * p = t;
  /* 当输入的记号不标识语句序列结束的记号 */
  while ((token!=ENDFILE) && (token!=END) &&
         (token!=ELSE) && (token!=UNTIL))
  { TreeNode * q;
  /* 跳过分号 */
    match(SEMI);
    q = statement();
    if (q!=NULL) {
      if (t==NULL) t = p = q;
      else /* 如果t不是NULL，那么p也一定不是NULL */
      {
		  /* 把state链接起来 */
		p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}
/* 用来匹配statement非终结符 */
TreeNode * statement(void)
{ TreeNode * t = NULL;
/* 检测当前的token，根据输入决定如何识别 */
  switch (token) {
    case IF : t = if_stmt(); break;
    case REPEAT : t = repeat_stmt(); break;
    case ID : t = assign_stmt(); break;
    case READ : t = read_stmt(); break;
    case WRITE : t = write_stmt(); break;
/* 出错，没有预料到的输入 */
    default : syntaxError("unexpected token -> ");
              printToken(token,tokenString);
              token = getToken();
              break;
  } 
  return t;
}
/* 用来匹配if_stmt非终结符 */
TreeNode * if_stmt(void)
{ TreeNode * t = newStmtNode(IfK);
  match(IF);
  if (t!=NULL) t->child[0] = exp();
  match(THEN);
  if (t!=NULL) t->child[1] = stmt_sequence();
  if (token==ELSE) {
    match(ELSE);
    if (t!=NULL) t->child[2] = stmt_sequence();
  }
  match(END);
  return t;
}
/* 用来匹配repeat_stmt非终结符 */
TreeNode * repeat_stmt(void)
{ TreeNode * t = newStmtNode(RepeatK);
  match(REPEAT);
  if (t!=NULL) t->child[0] = stmt_sequence();
  match(UNTIL);
  if (t!=NULL) t->child[1] = exp();
  return t;
}
/* 用来匹配assign_stmt非终结符 */
TreeNode * assign_stmt(void)
{ TreeNode * t = newStmtNode(AssignK);
  if ((t!=NULL) && (token==ID))
    t->attr.name = copyString(tokenString);
  match(ID);
  match(ASSIGN);
  if (t!=NULL) t->child[0] = exp();
  return t;
}
/* 用来匹配read_stmt非终结符 */
TreeNode * read_stmt(void)
{ TreeNode * t = newStmtNode(ReadK);
  match(READ);
  if ((t!=NULL) && (token==ID))
    t->attr.name = copyString(tokenString);
  match(ID);
  return t;
}
/* 用来匹配write_stmt非终结符 */
TreeNode * write_stmt(void)
{ TreeNode * t = newStmtNode(WriteK);
  match(WRITE);
  if (t!=NULL) t->child[0] = exp();
  return t;
}
/* 用来匹配exp非终结符 */
TreeNode * exp(void)
{ TreeNode * t = simple_exp();
  if ((token==LT)||(token==EQ)) {
    TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
    }
    match(token);
    if (t!=NULL)
      t->child[1] = simple_exp();
  }
  return t;
}
/* 用来匹配simple_stmt非终结符 */
TreeNode * simple_exp(void)
{ TreeNode * t = term();
  while ((token==PLUS)||(token==MINUS))
  { TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = term();
    }
  }
  return t;
}
/* 用来匹配term非终结符 */
TreeNode * term(void)
{ TreeNode * t = factor();
  while ((token==TIMES)||(token==OVER))
  { TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      p->child[1] = factor();
    }
  }
  return t;
}
/* 用来匹配factor非终结符 */
TreeNode * factor(void)
{ TreeNode * t = NULL;
  switch (token) {
    case NUM :
      t = newExpNode(ConstK);
      if ((t!=NULL) && (token==NUM))
        t->attr.val = atoi(tokenString);
      match(NUM);
      break;
    case ID :
      t = newExpNode(IdK);
      if ((t!=NULL) && (token==ID))
        t->attr.name = copyString(tokenString);
      match(ID);
      break;
    case LPAREN :
      match(LPAREN);
      t = exp();
      match(RPAREN);
      break;
    default:
      syntaxError("unexpected token -> ");
      printToken(token,tokenString);
      token = getToken();
      break;
    }
  return t;
}

/****************************************/
/* 语法分析程序的主函数					*/
/****************************************/
/*
 * 函数parse返回最近被构建的语法树
 */
TreeNode * parse(void)
{ TreeNode * t;
  token = getToken();
  t = stmt_sequence();
  if (token!=ENDFILE)
    syntaxError("Code ends before file\n");
  return t;
}
