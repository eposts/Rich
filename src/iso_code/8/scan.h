/****************************************************/
/* 文件： scan.h                                    */
/* TINY编译器的扫描器接口						    */
/****************************************************/

#ifndef _SCAN_H_
#define _SCAN_H_

/* 一个记号的最大的长度 */
#define MAXTOKENLEN 40

/* tokenString数组用来存储每个记号 */
extern char tokenString[MAXTOKENLEN+1];

/* getToken函数返回源文件中的下个记号
 */
TokenType getToken(void);

#endif
