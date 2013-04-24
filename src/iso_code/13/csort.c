/************************************************************************

 名称： csort.c
                                                                      
  一个动画程序来显示6个排序算法的动作。

  运行这个程序要求包含graphics.lib头文件，并且CGA和Hercules驱动应该包含在
  这个头文件中。

**************************************************************************/

#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <graphics.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAXNUM   200    /* 要排序的元素的最大数目 */
#define XAXIS    260    /* 图形显示屏幕的x坐标值 */
#define YAXIS    15     /* 图形显示屏幕的y坐标值 */
#define MAXPICKS 8      /* 用户可以选择最大选项数 */
#define TIMES    3      /* 执行的次数 */

int xaxis = XAXIS;
int yaxis = YAXIS;

/* 排序算法 */
enum sort {bubble, delayed, shell, shell_metzner,
           quick, insertion, all, stop};

/* 用户选项的名称 */
char *sorts[MAXPICKS] =
                 {"Bubble Sort", "Delayed Exchange Sort", "Shell Sort",
                  "Shell-Metzner Sort",QuickSort", "Insertion Sort",
                   "All", "Exit to Dos"};

/*****  函数原型  *************************/

void main( void );
void driver( enum sort atype, int *m, int elements,
             int random, int delay_factor );
enum sort pick_sort( int *elements, int *random, int *delay_factor );
void Initialize( void );
void Setscreen( int *m, int elements, int random );
int  Swap_Pixels( int *m, int i, int j, int delay_factor );
int  gprintf( int *xloc, int *yloc, char *fmt, ... );
void print_menu( char *mysorts[] );
void get_number( int *elements, int *times, char *tstring, int *x, int *y );
void Showdata ( int *m );

void Bubble( int *m, int elements, int delay_factor );
void Delayed( int *m, int elements, int delay_factor );
void Shell_Metzner( int *m, int elements, int delay_factor );
void Quicksort( int *m, int left, int right, int delay_factor );
void Insertion( int *m, int elements, int delay_factor );
void Shell( int *m, int elements, int delay_factor );


/*****  main函数  ***************************************/
/*                                                  */
/****************************************************/

void main( void )
{
 int array[MAXNUM];    /*  要排序的数组  */
 int elements;         /*  元素的个数       */
 int random;           /*  随机还是最差情况 */
 int delay_factor;     /*  延迟因子0-1000     */

 enum sort stype = all;
 /* 初始化图形界面 */
 Initialize();
 while( stype != stop )
   {
    random = 0;
    elements = 0;
    delay_factor = 0;

	/* 提示用户选择排序算法、排序情况和延迟因子 */
    stype = pick_sort( &elements, &random, &delay_factor );
    if ( stype != stop )
      {
       driver( stype, array, elements, random, delay_factor );
       /* Showdata( array ); */
       delay( 1350 );
      }
   }
 closegraph();
}


/*****  pick_sort函数  *******************************************************

  显示一个简单的菜单选项，提示用户输入一些运行参数

  被调用者:  main

  调用: print_menu
         gprintf
         get_number


  返回值  :  需要的排序算法(可以是六种算法中的一种、所有的排序算法或者退出)

  参数:

    *elements					元素个数，小于200
    *random						排序方式，随机或者最差情况
    *delay_factor				延迟因子
*************************************************************************/

enum sort pick_sort( int *elements, int *random, int *delay_factor )
{
/* 对用户的提示 */
 static char query1[] = "Which Sort (1-8)?";
 static char query2[] = "How Many Elements < 200?";
 static char query3[] = "(R)andom or (W)orst Case?";
 static char query4[] = "Delay Factor (0-1000)?";

 static char achar[2] = "x";
 char bchar = 0;
 char nstring[TIMES + 1];  /* 和数字等价的字符串  */
 int tens = TIMES; /* 十的指数 */
 int *tpower;
 int x = 50;
 int y = 30;
 char pick = 0;
 int x2;
 int i;    /* 循环变量 */
 tpower = &tens;

 cleardevice();
 print_menu( sorts );

 /************** 取得排序参数 *************************/
 gprintf( &x, &y, query1 );
 while ( pick <= 48 || pick >= 57 )  /* 允许数字1-8 */
  {
   pick = getch();
  }
 achar[0] = pick;
 x2 = x + 4 + textwidth( query1 );
 outtextxy( x2, y, achar );

 if ( pick != 56 )
   {
    y = 100;

    /******** 取得用来排序的元素个数 *****/
    gprintf( &x, &y, query2 );
    x2 = x + 4 + textwidth( query2  );
    for ( i = 0; i < TIMES + 1; i++ )
      nstring[i] = 0;        /* 把字符串初始化为空 */

    get_number( elements, tpower, nstring, &x2, &y );
    if ( *elements == 0 || *elements > MAXNUM ) *elements = MAXNUM;

    y += textheight("H" ) + 1;

    /****** 是随机模式还是最差模式 ***********/
    gprintf( &x, &y, query3 );
    bchar = 0;
    while( bchar != 82 && bchar != 87 )
      {
       bchar = toupper( getch( ) );
       if ( bchar == 13 ) bchar = 82;
      }
    *random = ( bchar ^ 87 ); /* XOR检查是否是最差模式 */
    achar[0] = bchar;
    x2 = x + 4 + textwidth( query3 );
    outtextxy( x2, y, achar );

    y += textheight( "H" ) + 1;

    /****** 取得延迟因子数目  ******************/
    gprintf( &x, &y, query4 );
    x2 = x + 4 + textwidth( query4 );
    *tpower = TIMES;
    for ( i = 0; i < TIMES + 1; i++ )
      nstring[i] = 0;        /* 把字符串初始化为空 */

    get_number( delay_factor, tpower, nstring, &x2, &y );

   }
  switch( pick - 48 )
    {
     case 1:
        return( bubble );
     case 2:
        return( delayed );
     case 3:
        return( shell );
     case 4:
        return( shell_metzner );
     case 5:
        return( quick );
     case 6:
        return( insertion );
     case 7:
        return( all );
     default:
        return( stop );
    }
}

/*****  print_menu函数  *****************************************

   在图形界面上显示选择菜单
   屏幕会产生类似的：

                   1. Bubble Sort
                   2. Delayed Exchange Sort
                   3. Shell Sort
                   4. Shell Metzner Sort
                   5. Quicksort
                   6. Insertion Sort
                   7. All
                   8. Exit to Dos

    调用者: pick_sort

*************************************************************/

void print_menu( char *mysorts[] )
{
 int x, y;   /* 屏幕的坐标 */
 int i;
 x = 240;
 y = 10;

 for ( i = 0; i < MAXPICKS; i++ )
   {
    gprintf( &x, &y, "%d. %s", i+1, mysorts[i] );
    y += textheight ( "H" ) + 1;
   }
}

/*****  get_number函数  ******************************************************

一个递归的程序，把从getch()接收的数字显示在屏幕上。
这个程序只接收从'0'到'9'的字符和回车，其他的字符都被忽略。

    调用者: pick_sort, get_number

    参数:

    int *a_number   一个指向整数的指针，返回给调用函数使用

    int *times      get_number最大被调用的次数。作为一个结束递归的标志。
					如果用户输入了允许的最大数字或者按下回车符，表示输入结束。

    char *tstring   返回*a_number数字的等价字符串形式。
                    比如，如果*a_number = 123,那么*tstring = "123"。
					在初始调用的时候，这个字符串被初始化为空串。
                    字符串的长度用来决定键入的数字的十的指数

    int *x, *y      输入数字的屏幕显示坐标值。使用textwidth函数计算出的文本宽度
					来增加*x的值
*************************************************************************/

void get_number( int *a_number, int *times, char *tstring, int *x, int *y )
{
int power;         /* 十的指数，用10的power来乘以数字 */
char achar[2];
char bchar = 0;
achar[1] = 0;

while ( bchar <= 47 || bchar >= 59 )  /* 允许的数字0-9 */
  {
   bchar = getch();
   if ( bchar == 13 )   /* 13 = CR; 用户按了ENTER建  */
     {
      bchar = 48;
      *times = 0;
      break;
     }
  }

 if ( *times )
   {
    achar[0] = bchar;

    outtextxy( *x, *y, achar );
    *x = *x + textwidth( achar );
    tstring[TIMES - ( (*times)--)] = achar[0];
    if ( *times )
    get_number( a_number, times, tstring, x, y );
   }

    power = (int)( pow10(( strlen( tstring ) - ((*times) + 1))));
    bchar = tstring[*times];
    *a_number += ( power  * ( bchar - 48 ));
    (*times )++;
}


/*****  driver  **********************************************************

   根据参数的，选择合适的排序算法运行

   他得到排序的类型、要排序的数组首地址、元素的数目、随机/最坏情况、延迟因子

	调用者：main

	调用: Setscreen, gprintf, all the sort functions

    参数:

    enum sort atype    指定排序算法

    int *array         用来排序的数组首地址

    int elements       元素的个数

    int random         random = 1  worst case = 0

    int delay_factor   0 = 没有延迟;  1000 = 两个元素交换有1秒延迟。
					   通过这个参数可以让用户对排序的过程有个感性的认识。
					   不过1000是非常慢的。

*************************************************************************/

void driver( enum sort atype, int *array, int elements,
            int random, int delay_factor )
{

switch( atype )
  {
	/* 所有排序算法 */
   case all    :
	/* 冒泡排序 */
   case bubble :
            Setscreen( array, elements, random );
            gprintf( &xaxis, &yaxis, *(sorts + bubble) );
            Bubble( array, elements, delay_factor );
            if ( atype != all ) break; else delay( 1350 );
	/* 延迟交换排序 */
   case delayed:
            Setscreen( array, elements, random );
            gprintf( &xaxis, &yaxis, *(sorts + delayed) );
            Delayed( array, elements, delay_factor );
            if ( atype != all ) break; else delay( 1350 );
	/* 希尔排序 */
   case shell  :
            Setscreen( array, elements, random );
            gprintf( &xaxis, &yaxis, *(sorts + shell ));
            Shell( array, elements, delay_factor );
            if ( atype != all ) break; else delay( 1350 );
	/* Metzner希尔排序 */
   case shell_metzner:
            Setscreen( array, elements, random );
            gprintf( &xaxis, &yaxis, *(sorts + shell_metzner) );
            Shell_Metzner( array, elements, delay_factor );
            if ( atype != all ) break; else delay( 1350 );
	/* 快速排序 */
   case quick  :
            Setscreen( array, elements, random );
            gprintf( &xaxis, &yaxis, *(sorts + quick) );
            Quicksort( array, 0, elements - 1, delay_factor );
            if ( atype != all ) break; else delay( 1350 );
	/* 插入排序 */
   case insertion:
            Setscreen( array, elements, random );
            gprintf( &xaxis, &yaxis, *(sorts + insertion) );
            Insertion( array, elements, delay_factor );
            if ( atype != all ) break; else delay( 1350 );
	/* 停止 */
   case stop:

   default:;
  }
}


/*****  initialize函数  *********************************/
/*							                            */
/*  初始化图形驱动									    */
/*														*/
/********************************************************/

void Initialize( void )
{
 int    GraphDriver; /* 图形设备驱动   */
 int    GraphMode;   /* 图形模式值      */
 int    ErrorCode;   /* 出错代码  */

 GraphDriver = DETECT;              /* 自动检测显示设备类型 */
 initgraph( &GraphDriver, &GraphMode, "" );
 ErrorCode = graphresult();   /* 读取初始化的结果*/
 if( ErrorCode != grOk )      /* 如果在初始化的过程中发生错误  */
   {
    printf(" Graphics System Error: %s\n", grapherrormsg( ErrorCode ) );
    exit( 1 );
   }

}

/*****  gprintf函数  ************************************/
/*						                                */
/*  和C标志库中的PRINTF函数很类似。但是gprintf可以      */
/*  图形模式中在指定的坐标位置显示文本			        */
/*														*/
/*                                                      */
/*  gprintf函数的返回值并没有被使用						*/
/*                                                      */
/********************************************************/

int gprintf( int *xloc, int *yloc, char *fmt, ... )
{
 va_list  argptr;  /* 参数列表指针          */
 char str[80];     /* 用来保存参赛    */
 int count;        /* vspringf的放回值  */

 va_start( argptr, fmt );               /* 初始化va_ 函数         */
 count = vsprintf( str, fmt, argptr );  /* 把字符串放到存储区中	  */
 outtextxy( *xloc, *yloc, str );        /* 在图形模式下显示字符串 */
 va_end( argptr );                      /* 关闭va_ 函数			  */
 return( count );                       /* 返回显示的字符数		  */

}


/*****  Setscreen  *******************************************************

   为排序初始化屏幕

   调用者: driver

   参数:

    int *array         要排序的数组

    int elements       元素的个数

    int random         random = 1或者worst case = 0

*************************************************************************/

void Setscreen( int *array, int elements, int random )
{
int j;

cleardevice();

/* 随机产生元素系列 */
if ( random )
  {
   randomize();
   for ( j = 0; j < elements; j++ )
     {
      *( array + j) = random( elements );
      putpixel( 3*j, *(array+j), 10);
     }
  }
else /* 初始化最坏情况，即元素全部倒置 */
  {
   for ( j = 0; j < elements; j++ )
     {
      *(array + j) = elements - j;
      putpixel( 3*j, *(array+j), 10);
     }

   }
}



/*****  Swap_Pixels函数  ****************************/
/*                                                  */
/*  交换两个数组元素，并且改变相应的象素位置        */
/*  通过打开和关闭显示来制作动画效果				*/
/*                                                  */
/****************************************************/

int Swap_Pixels( int *array, int i, int j, int delay_factor )
{
int h;
h = *(array + i);
putpixel( 3 * i, *(array + i), 0);
putpixel( 3 * j, *(array + i), 10 );
*(array + i) = *(array + j);
putpixel( 3 * j, *( array + j), 0 );
putpixel( 3 * i, *(array + j), 10 );
*(array + j) = h;

delay( delay_factor );
return( h );
}

/*****  冒泡排序 ************************************/
/*                                                  */
/****************************************************/

void Bubble( int *array, int elements, int delay_factor )
{
int i,j;

for ( i = 0; i < elements - 1 ; i++ )
 for ( j = i + 1; j < elements; j++ )
   {
    if ((*(array+i)) > (*(array+j)))
      {
       Swap_Pixels( array, i, j, delay_factor );
      }
   }
}

/*****  延迟交换排序  *******************************/
/*                                                  */
/****************************************************/

void Delayed( int *array, int elements, int delay_factor )
{
int p, h, k, i, j;

for ( p = 0; p < elements-1; p++ )
 {
  h = p;
  for ( k = p + 1; k < elements ; k++ )
    if (*(array+k) < *(array+h))
      h = k;
  if ( p != h )
   {
    i = h;
    j = p;
    Swap_Pixels( array, i, j, delay_factor );
   }
 }
}

/*****  希尔排序  ***********************************/
/*                                                  */
/****************************************************/

void Shell( int *array, int elements, int delay_factor )
{
 int p, f, i, j, m;


p = elements;
while ( p > 1)
  {
   p /= 2;
   
   m = elements - p;
   do{
     f = 0;
     for ( j = 0; j < m; j++ )
       {
        i = j + p;
        if (*(array + j) > *(array + i))
          {
           Swap_Pixels( array, i, j, delay_factor );
           f = 1;
          }
       }
     } while( f );
  }
}

/*****  Metzner希尔排序  ****************************/
/*                                                  */
/****************************************************/

void Shell_Metzner( int *array, int elements, int delay_factor )
{
int p, k, t, i, j;

p = elements;
p /= 2;
while ( p != 0 )
  {
  k = elements - p;
  for ( t = 0; t < k; t++ )
    {
    i = t;
    while ( i >= 0 )
      {
      j = i + p;
      if (*(array+j) < *(array + i))
        {
        Swap_Pixels( array, i, j, delay_factor );
        i = i - p;
        }
      else
        break;
      }
    }
  p /= 2;
  }
}

/*****  快速排序  ***********************************/
/*                                                  */
/****************************************************/

void Quicksort( int *array, int left, int right, int delay_factor )
{
 int i, j, t;

 if ( right > left )
 {
  i = left - 1; j = right;
  do {
      do i++;
        while ( array[i] < array[right] );
      do j--;
        while ( array[j] > array[right] && j > 0 );
      t = Swap_Pixels( array, i, j, delay_factor );
     } while ( j > i );

      putpixel( 3*j, *(array + j), 0);
      array[j] =array[i];
      putpixel( 3*j, *(array + j), 10 );
      putpixel( 3*i, *(array + i), 0 );
      array[i] =array[right];
      putpixel( 3*i, *(array + i), 10 );
      putpixel( 3*right, *(array + right), 0 );
      array[right] = t;
      putpixel( 3*right, *(array + right), 10 );

      Quicksort( array, left, i - 1, delay_factor );
      Quicksort( array, i + 1, right, delay_factor );

  }
}


/*****  插入排序  ***********************************/
/*                                                  */
/****************************************************/

void Insertion( int *array, int elements, int delay_factor )
{
 int p, j, t;


 for ( p = 0; p < elements - 1; p++ )
   {
    t = *(array + p + 1);
    for ( j = p; j >= 0; j-- )
      {
       if ( t <= *(array + j) )
         {
          *(array + j + 1) = *(array + j);
          putpixel( 3*(j + 1), *(array + j + 1), 10 );
          putpixel( 3*j, *(array + j + 1), 0 );
          delay( delay_factor );
         }
       else
         break;
      }
    *(array + j + 1) = t;
    putpixel( 3*(p + 1), t, 0 );
    putpixel( 3*(j + 1), t, 10 );
   }
}

