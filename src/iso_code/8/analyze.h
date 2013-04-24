/****************************************************/
/* 文件：analyze.h                                  */
/* TINY compiler编译器的语法分析接口文件		    */
/****************************************************/

#ifndef _ANALYZE_H_
#define _ANALYZE_H_

/*
 * 函数buildSymtab通过前叙遍历语法树来建立符号表
 */
void buildSymtab(TreeNode *);

/* 
 * 函数typeCheck通过后续遍历语法树进行类型检查
 */
void typeCheck(TreeNode *);

#endif
