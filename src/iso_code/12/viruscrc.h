/*
VIRUSCRC.H
这个是防病毒CRC校验VALIDCRC.C和VIRSUDAT.C的头文件
*/


typedef
  unsigned long crc32_t;

union filecrc
  {
  char searchstr[8];	/* 要搜索的字符串. */

  struct
    {
    crc32_t polynomial;	/* 这个文件的多项式. */
    crc32_t crc;	/* 为这个文件计算出的CRC. */
    } x;
  };


extern const union filecrc _viruscrc;


/* CRC校验的返回值. */
enum
  {
  CRC_VALID,	/* CRC有效. */
  CRC_INVALID,	/* CRC无效 */
  CRC_ISZERO,	/* CRC多项式被重置为0 */
  CRC_NOMEM,	/* 内存不足 */
  CRC_FILEERR	/* 未找到程序文件 */
  };


int validatecrc(const char *progname);

#define isvalidcrc(progname)  (validatecrc(progname) == CRC_VALID)
