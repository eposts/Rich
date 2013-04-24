/*为了让本实例尽可能简洁的突出鼠标编程，而不在其他方面过多分散读者的注意力。我忽略了很多实现的细节，读者对于可以在十进制编辑框中输入字符"F"之类的问题不要太吃惊。有兴趣的读者可以很容易的修正这个"bug"*/
#include<dos.h>
#include<conio.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#define HEX 0
#define DEC 1
#define OCT 2
#define BIN 3

int index=0,choice=0;
/*初始化鼠标器*/
void initmouse()
{
	_AX=0;
	geninterrupt(0x33);

}
/*显示鼠标光标*/
void showmouse()
{
	_AX=1;
	geninterrupt(0x33);
}
/*隐藏鼠标*/
void hidemouse()
{
	_AX=2;
	geninterrupt(0x33);
}
/*取鼠标状态和鼠标位置*/
void getmouse(int *button,int *x,int *y)
{
	_AX=3;
	_BX=0;
	geninterrupt(0x33);
	*button=_BL;
	*x=_CX;
	*y=_DX;
}
/*设置程序中的数字字符表*/
int getnum(char c)
{
	int	j;
	char alpha_set[36]="0123456789abcdefghijklmnopqrstuvwzyz";
	for(j=0;j<36;j++)
	{
		if(alpha_set[j]==c)
			break;
	}
	return j;
}
/*把任意radix进制的数，转换为十进制数*/
unsigned long convert_to_decimal(char *_num,int radix)
{
	int i,len;
	unsigned long dec=0;
	len=strlen(_num);
	len--;
	for(i=0;_num[i]!=NULL;i++,len--)
	{
		dec+=(getnum(_num[i])*pow(radix,len));
	}
	return dec;
}
/*在给定的坐标(x,y)的位置显示一个字符串*/
void display(int x,int y,char string[80])
{	gotoxy(x,y);
	cprintf(string);
}
/*清屏，并且把背景色设置为青绿色*/
void make_screen_cyan()
{
	textbackground(CYAN);
	clrscr();
}
/*绘制程序的初始界面*/
void start_screen()
{
	index=0;
	hidemouse();
	make_screen_cyan();
	textbackground(RED);
	textcolor(BLACK);
	display(20,1,"Radix Converter by NILOY MONDAL.");
	display(10,2,"Email:-yiwei@zju.edu.cn");
	display(30,3,"Press Escape to Quit");
	textbackground(CYAN);
	display(10,5,"Hexadecimal:- ");
	display(10,7,"Decimal    :- ");
	display(10,9,"Octal      :- ");
	display(10,11,"Binary     :- ");
	textbackground(BLUE);
	display(23,5,"                                                      ");
	display(23,7,"                                                      ");
	display(23,9,"                                                      ");
	display(23,11,"                                                      ");
	if(choice==HEX)
		gotoxy(24,5);
	else if(choice==DEC)
		gotoxy(24,7);
	else if(choice==OCT)
		gotoxy(24,9);
	else if(choice==BIN)
		gotoxy(24,11);
	showmouse();
}

void main()
{
	char text[80]="\0",buffer[80];
	char ch,*charhex,*chardec,*charoct,*charbin;
	int button,mousex,mousey,x,y;
	unsigned long deci;

	initmouse();
	start_screen();
	showmouse();
	while(1)
	{
		if(kbhit())
		{
			ch=getch();
/*ESCAPE键的ASCII值是27，如果用户按下ESC，程序结束*/
			if(ch==27)		
				break;
/*如果按删除键并且光标没有越过输入框的边界*/
			if(ch=='\b'&&wherex()>=24)   
			{
/*把光标退后一格，并且放一个空格在那里*/
				cprintf("\b");
				cprintf("%c",255);
/*再把光标退后一格，弹出删除的字符*/
				cprintf("\b");
				if(index!=0)
					index--;			
				text[index]=NULL;
			}
			else if(wherex()>=24&&ch>='0'&&ch<='f')
			{
				cprintf("%c",ch);
/*把字符放入字符数组*/
				text[index]=ch;			
/*把数组最后一位置零*/
				text[index+1]=NULL;	
				index++;
			}
/*保存当前的坐标*/
			x=wherex(),y=wherey();
/*程序首先把字符串通过convert_to_decimal()转化为十进制数，之后调用stdlib.h定义的ltoa()把十进制数转换为其它进制数，比如二进制、八进制、十六进制等等.*/
			switch(choice)
			{
				case HEX:
					deci=convert_to_decimal(text,16);
					gotoxy(24,7);
					printf("%ld                             ",deci);
					gotoxy(24,11);
					printf("%s                              ",ltoa(deci,buffer,2));
					gotoxy(24,9);
					printf("%s                              ",ltoa(deci,buffer,8));
					break;
				case DEC:
					deci=atol(text);
					gotoxy(24,5);
					printf("%s                             ",ltoa(deci,buffer,16));
					gotoxy(24,9);
					printf("%s                             ",ltoa(deci,buffer,8));
					gotoxy(24,11);
					printf("%s                             ",ltoa(deci,buffer,2));
					break;
				case OCT:
					deci=convert_to_decimal(text,8);
					gotoxy(24,7);
					printf("%ld                             ",deci);
					gotoxy(24,5);
					printf("%s                             ",ltoa(deci,buffer,16));
					gotoxy(24,11);
					printf("%s                              ",ltoa(deci,buffer,2));
					break;
				case BIN:
					deci=convert_to_decimal(text,2);
					gotoxy(24,5);
					printf("%s                            ",ltoa(deci,buffer,16));
					gotoxy(24,7);
					printf("%ld                            ",deci);
					gotoxy(24,9);
					printf("%s                            ",ltoa(deci,buffer,8));
					break;
			}
			gotoxy(x,y);
		}
		getmouse(&button,&mousex,&mousey);
		mousex++,mousey++;
/*下面的代码检测用户在什么地方按了什么按钮*/
		if(mousex/8>23&&mousex/8<50&&mousey/16==2&&button&1==1)
		{
			choice=HEX;
			start_screen();
		}
		else if(mousex/8>23&&mousex/8<50&&mousey/16==3&&button&1==1)
		{
			choice=DEC;
			start_screen();
		}

		else if(mousex/8>23&&mousex/8<50&&mousey/16==4&&button&1==1)
		{
			choice=OCT;
			start_screen();
		}
		else if(mousex/8>23&&mousex/8<50&&mousey/16==5&&button&1==1)
		{
			choice=BIN;
			start_screen();
		}
	}

}
