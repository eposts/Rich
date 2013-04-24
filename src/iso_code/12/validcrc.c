/*
VALIDCRC.C

	这个模块的作用是验证连接的程序的CRC。
	代码是一个防病毒算法。CRC是一个检测病毒入侵的非常有效的办法。
	任何病毒如果把它自身嵌入程序必然会改变源代码的CRC。
*/


#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "viruscrc.h"


/* 从一个字取得低字节和高字节的宏. */
#define lowb(x)  (*(unsigned char *)&(x))
#define hib(x)   (*((unsigned char *)&(x) + 1))

/* 从一个双字取得低字和高字的宏. */
#define loww(x)  (*(unsigned short *)&(x))
#define hiw(x)   (*((unsigned short *)&(x) + 1))


void *bufalloc(size_t *size, size_t minsize);


#if defined(__TURBOC__)

#include <dir.h>
#define findprog(progname, pn)  strcpy(pn, searchpath(progname))
#define argv0  _argv[0]

#elif defined(_MSC_VER) || defined(_QC)

#define findprog(progname, pn)  _searchenv(progname, getenv("PATH"), pn)
extern char **__argv;
#define argv0  __argv[0]

#else
#error Unknown compiler.
#endif


/***/
/* 计算当前激活的程序的CRC，并且把它和在_viruscrc保存的CRC比较. */
int validatecrc(const char *progname)
{
int retcode;			/* 函数返回码 */

if (_viruscrc.x.polynomial != 0)
  {
  unsigned char *buffer;	/* 程序位图的缓存 */
  size_t bufsize;		/* 缓存的大小. */

  /* 如果可能，分配8k的内存；至少保证分配512字节. */
  bufsize = 8192;
  buffer = bufalloc(&bufsize, 512);

  if (buffer)
    {
    char pn[80];		/* 程序的全名 */
    FILE *progfile;		/* 文件指针 */

    if (_osmajor < 3)
      /* 根据在progname指定的路径搜索文件. */
      findprog(progname, pn);
    else
      strcpy(pn, argv0);

    if ((progfile = fopen(pn, "rb")) != NULL)
      {
      crc32_t table[256];	/* CRC表 */
      register crc32_t *halfi;	/* i/2的CRC指针 */
      crc32_t crc;		/* 当前的CRC */

      register size_t i;	/* 字节计数器. */
      unsigned char *bufptr;	/* 缓存区遍历指针 */

      /* 为了快速计算机生成一长CRC查找表 */
      for (i = 0, halfi = table, table[0] = 0; i < 256; i += 2, halfi++)
	if (hib(hiw(*halfi)) & 0x80)
	  table[i] = (table[i + 1] = *halfi << 1) ^ _viruscrc.x.polynomial;
	else
	  table[i + 1] = (table[i] = *halfi << 1) ^ _viruscrc.x.polynomial;

      crc = 0;
      while ((i = fread(buffer, 1, bufsize, progfile)) != 0)
	for (bufptr = buffer; i--; bufptr++)
	  crc = (crc << 8) ^ table[hib(hiw(crc)) ^ *bufptr];

      fclose(progfile);

      retcode = crc == _viruscrc.x.crc ? CRC_VALID : CRC_INVALID;
      }
    else
      retcode = CRC_FILEERR;

    free(buffer);
    }
  else
    retcode = CRC_NOMEM;
  }
else
  /* CRC多项式必须不为0 */
  retcode = CRC_ISZERO;

return (retcode);
}
