/****************************************************/
/* 文件： analyze.c                                  */
/* 语义分析代码实现					                 */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/* 变量内存位置计数器counter for variable memory locations */
static int location = 0;

/* 函数traverse是一个通用的递归访问语法树函数
 * 他通过调用preProc函数来实现前序遍历；
 * 调用函数postProc实现后序遍历 
 */
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{ if (t != NULL)
  { preProc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}

/* 函数nullProc是一个什么都不做的函数，
 * 目的是产生单前叙或者单后序遍历的遍历过程
 */
static void nullProc(TreeNode * t)
{ if (t==NULL) return;
  else return;
}

/* 函数insertNode把在t（语法树）中存储的节点插入符号表
 */
static void insertNode( TreeNode * t)
{ switch (t->nodekind)
  { case StmtK:
      switch (t->kind.stmt)
      { case AssignK:
        case ReadK:
          if (st_lookup(t->attr.name) == -1)
          /* 没有在符号表中找到，把这个表标识符按照的新的定义处理 */
            st_insert(t->attr.name,t->lineno,location++);
          else
          /* 已经在符号表中找到，所以忽略位置信息，插入使用这个变量的行号 */ 
            st_insert(t->attr.name,t->lineno,0);
          break;
        default:
          break;
      }
      break;
    case ExpK:
      switch (t->kind.exp)
      { case IdK:
          if (st_lookup(t->attr.name) == -1)
           /* 没有在符号表中找到，把这个表标识符按照的新的定义处理 */
            st_insert(t->attr.name,t->lineno,location++);
          else
          /* 已经在符号表中找到，所以忽略位置信息，插入使用这个变量的行号 */ 
			st_insert(t->attr.name,t->lineno,0);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

/* 函数buildSymtab通过前叙遍历语法树构建符号表
 */
void buildSymtab(TreeNode * syntaxTree)
{ traverse(syntaxTree,insertNode,nullProc);
  if (TraceAnalyze)
  { fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}
/* 打印错误信息 */
static void typeError(TreeNode * t, char * message)
{ fprintf(listing,"Type error at line %d: %s\n",t->lineno,message);
  Error = TRUE;
}

/* 函数checkNode在语法树的一个节点执行类型检查
 */
static void checkNode(TreeNode * t)
{ switch (t->nodekind)
  { 
  /* 表达式 */
	case ExpK:
      switch (t->kind.exp)
      { 
	  /* 数值运算，要求操作数都是整数 */
		case OpK:
          if ((t->child[0]->type != Integer) ||
              (t->child[1]->type != Integer))
            typeError(t,"Op applied to non-integer");
		  /* 数值比较运算，结果是布尔型 */
          if ((t->attr.op == EQ) || (t->attr.op == LT))
            t->type = Boolean;
		  /* 其他数值运算，结果是整数 */
          else
            t->type = Integer;
          break;
		  /* 常数和变量，类型为整数 */
        case ConstK:
        case IdK:
          t->type = Integer;
          break;
        default:
          break;
      }
      break;
	/* 语句 */
    case StmtK:
      switch (t->kind.stmt)
      { 
		/* IF语句，类型是布尔型 */
	    case IfK:
          if (t->child[0]->type == Integer)
            typeError(t->child[0],"if test is not Boolean");
          break;
		/* 赋值语句语句，类型是整型 */
        case AssignK:
          if (t->child[0]->type != Integer)
            typeError(t->child[0],"assignment of non-integer value");
          break;
		/* 输出语句，类型是整型 */
        case WriteK:
          if (t->child[0]->type != Integer)
            typeError(t->child[0],"write of non-integer value");
          break;
		/* 循环语句，测试类型是布尔型 */
        case RepeatK:
          if (t->child[1]->type == Integer)
            typeError(t->child[1],"repeat test is not Boolean");
          break;
        default:
          break;
      }
      break;
    default:
      break;

  }
}

/* 函数typeCheck通过后序遍历语法树来执行类型检查
 */
void typeCheck(TreeNode * syntaxTree)
{ traverse(syntaxTree,nullProc,checkNode);
}
