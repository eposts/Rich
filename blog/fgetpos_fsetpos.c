/*
 *ftell() 和 fseek() 用长整型表示文件内的偏移 (位置), 因此, 偏移量被限制在 20 亿 (231-1) 以内。而新的 fgetpos() 和 fsetpos()
 *函数使用了一个特殊的类型定义 fpos_t 来表示偏移量。这个类型会适当选择, 因此, fgetpos() 和 fsetpos 可以表示任意大小的文件
 *偏移。fgetpos() 和  gsetpos() 也可以用来记录多字节流式文件的状态。
 */

/*-----------------------------------------------------------------------------
 *  函数原型： int fsetpos(FILE *fp, const fpos_t *pos);
 *-----------------------------------------------------------------------------*/
/*头文件：#include<stdio.h>
 *
 *是否是标准函数：是
 *
 *函数功能：将文件指针定位在pos指定的位置上。该函数的功能与前面提到的fgetpos相反，是将文件指针fp按照pos指定的位置在文件中定位。pos值以内部格式存储,仅由fgetpos和fsetpos使用。
 *
 *返回值：成功返回0，否则返回非0。
 *
 *例程如下 应用fsetpos函数定位文件指针。
 */

#include <stdio.h>

void main( void )

{

   FILE   *fp;

   fpos_t pos;

   char   buffer[50];

   /*以只读方式打开名为test.txt的文件*/

   if( (fp = fopen( "test.txt", "rb" )) == NULL )

      printf( "Trouble opening file/n" );

   else

   {

      /*设置pos值*/

      pos = 10;

      /*应用fsetpos函数将文件指针fp按照

      pos指定的位置在文件中定位*/

      if( fsetpos( fp, &pos ) != 0 )

        perror( "fsetpos error" );

            else

            {

                /*从新定位的文件指针开始读取16个字符到buffer缓冲区*/

                fread( buffer, sizeof( char ), 16, fp );

                 /*显示结果*/

                printf( "16 bytes at byte %ld: %.16s/n", pos, buffer );

                }

      }

   fclose( fp );

}

/*
 *例程说明：
 *
 *（1）首先，程序以只读方式打开名为test.txt的文件。在这里，test.txt文件中已存入字符串This is a test for testing the function of fsetpos.
 *
 *（2）将pos设置为10。应用fsetpos函数将文件指针fp按照pos指定的位置在文件中定位。这样文件指针fp指向字符串中test的字母t。
 *
 *（3）再从新定位的文件指针开始读取16个字符到buffer缓冲区，也就是说读取字符串"test for testing"到缓冲区buffer。
 *
 *（4）最后显示结果：16 bytes at byte 10: test for testing 。
 */





/*-----------------------------------------------------------------------------
 *  函数原型：int fgetpos( FILE *stream, fpos_t *pos );
 *-----------------------------------------------------------------------------*/

/*
 *头文件：#include<stdio.h>
 *
 *是否是标准函数：是
 *
 *函数功能：取得当前文件的指针所指的位置，并把该指针所指的位置数存放到pos所指的对象中。pos值以内部格式存储,仅由fgetpos和fsetpos使用。其中fsetpos的功能与fgetpos相反，为了详细介绍，将在后节给与说明。
 *
 *返回值：成功返回0，失败返回非0，并设置errno。
 *
 *例程如下：应用fgetpos函数取得当前文件的指针所指的位置。
 */

#include <string.h>

#include <stdio.h>

int main(void)

{

   FILE *fp;

   char string[] = "This is a test";

   fpos_t pos;

   /* 以读写方式打开一个名为test.txt的文件 */

   fp = fopen("test.txt", "w+");

   /* 将字符串写入文件 */

   fwrite(string, strlen(string), 1, fp);

   /* 取得指针位置并存入&pos所指向的对象 */

   fgetpos(fp, &pos);

   printf("The file pointer is at byte %ld/n", pos);

    /*重设文件指针的位置*/

   fseek(fp,3,0);

    /* 再次取得指针位置并存入&pos所指向的对象 */

   fgetpos(fp, &pos);

   printf("The file pointer is at byte %ld/n", pos);

   fclose(fp);

   return 0;

}

/*
 *例程说明：
 *
 *（1）首先，程序以读写方式打开一个名为test.txt的文件，并把字符串"This is a test"写入文件。注意：字符串共14个字节，地址为0~13。用fwrite函数写入后，文件指针自动指向文件最后一个字节的下一个位置。即这时的fp的值应该是14。
 *
 *（2）再用fgetpos函数取得指针位置并存入&pos所指向的对象，此时， pos中的内容为14。然后在屏幕上显示出The file pointer is at byte 14。
 *
 *（3）再用fseek函数重设文件指针的位置，让fp的值为3，即指向文件中第4个字节。
 *
 *再次取得指针位置并存入&pos所指向的对象。然后在屏幕上显示出The file pointer is at byte 3。
 *
 */
