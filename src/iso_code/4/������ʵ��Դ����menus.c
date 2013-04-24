
#include <conio.h>
#include<dos.h>
#include <graphics.h>
#include<stdio.h>
#include<stdlib.h>

/*定义一些常数*/
#define NO 0
#define YES 1
#define NONE -1
#define MAX_MENU 7/*根据情况改变可以在菜单显示的最大项*/

/*全局参数*/
int X,Y;
int Selection;
int button,x,y;
void *p;
size_t area;

/*菜单结构*/
struct MENU
{
      /*左上角*/
      int x;	
      int y;
      /*右下角*/
      int x1;
      int y1;
      unsigned char Active[MAX_MENU];/* 菜单项是否激活的标志*/
      char *head;/* 菜单的名字（可选项）*/
};

int MouseOn(struct MENU *t);
void Process();
void Xorbar(int sx,int sy,int ex,int ey,int c);
void Show();
void Hide();
void Status();

/* 通过下面的参数用户可以改变按钮的位置。*/

struct MENU File={200,110,250,130};
struct MENU Edit={295,110,345,130};
struct MENU Help={390,110,440,130};
struct MENU Close={600,10,620,30};
struct MENU Neeraj={380,300,460,315};

void main()
{
 int gd = DETECT,gm;
 
 /*初始化图形界面*/
 initgraph(&gd, &gm,"");
 Selection=NONE;
 /*是否激活菜单项，YES表示激活；NO表示取消激活*/
 for(gd=0;gd<MAX_MENU;gd++)
 {
	File.Active[gd]=YES;
	Edit.Active[gd]=YES;
	Help.Active[gd]=YES;
	Close.Active[gd]=YES;
 }
 /*菜单项的名字，用户可以修改以显示其他的名称*/
 File.head="[ File Menu ]";
 Edit.head="[ Edit Menu ]";
 Help.head="[ Help Menu ]";
 Close.head="[ Exit ]";
 /*为界面分配内存*/
 area=imagesize(0,0,150,150);
 p=malloc(area);
 
 /*没有足够的内存*/
 if(p==NULL)
 {
	closegraph();
	restorecrtmode();
	printf("Not Enough Memory !\n try to reduce the area used.\n");
	exit(1);
 }
 setfillstyle(1,7);
 bar(0,0,640,480);
 X=2;Y=2;
 /*绘制这个界面*/
 Process();
 End();
 /*关闭图形显示方式*/
 closegraph();
 printf("GUI-MENUS By Neeraj Sharma  -   n21@indiatimes.com\n");
 getch();
}

void Process()
{
	/*文件菜单项的子菜单项*/
	char *fm[]={"New","Open","Save","Save as","Exit"};
	/*编辑菜单项的子菜单项*/
	char *em[]={"Undo","Cut","Copy","Paste","Delete","Select All"};
	/*帮助菜单项的子菜单项*/
	char *hm[]={"Help Topics","About Menus"};
	
	/* 如果用户在这里改变的了菜单项的值，那么也同样需要改变在函数
	   中的PullMenuDown(char *,int)值*/
	char KEY;
	/* 绘制各个菜单 */
	Win(140,100,500,140);
	/* 绘制文件菜单项 */
	DrawBtn(&File,"File");
	/* 绘制编辑菜单项 */
	DrawBtn(&Edit,"Edit");
	/* 绘制帮助菜单项*/
	DrawBtn(&Help,"Help");
	settextstyle(0,0,0);
	outtextxy(Close.x+7,Close.y+7,"x");
	Comments();
	Show();
	/* 正如下面所示用户可以激活和取消激活一个菜单项
	   下面的'save as'被取消激活*/
	File.Active[3]=NO;/* Save as*/
	Edit.Active[0]=NO;/* 取消激活*/
	while(1)
	{
		if(kbhit())
		{
			KEY=getch();
			/*如果用户按下"ESC"或者"-"推出程序*/
			if(KEY==27|| KEY==45) break;
		}
		Status();
		/*如果按下鼠标左键*/
		if(button==1)
		{
		  /*用户点击Close*/
		  if(MouseOn(&Close)) break;
		  /*用户点击File*/
		  if(MouseOn(&File))
		  {
			  PullMenuDown(fm,5,&File);/* 菜单项为5*/
			  if(Selection==NONE) Msg("No Selection !");
			  else Msg(fm[Selection]);
			  if(Selection==4) break;
		  }
		  /*用户点击Edit*/
		  if(MouseOn(&Edit))
		  {
			  PullMenuDown(em,6,&Edit);/* 菜单项为6*/
			  if(Selection==NONE) Msg("No Selection !");
			  else Msg(em[Selection]);
		  }
		  /*用户点击Help*/
		  if(MouseOn(&Help))
		  {
			  PullMenuDown(hm,2,&Help);/* 菜单项为2*/
			  if(Selection==NONE) Msg("No Selection !");
			  else Msg(hm[Selection]);
			  if(Selection==1) Msg("* Programming By NEERAJ SHARMA *");
		  }
		}
		else
		{
			/*鼠标在那个菜单项上，就高亮显示之*/
			
			/*高亮显示文件菜单项*/
			if(MouseOn(&File)) Hlight(&File);
			/*高亮显示编辑菜单项*/
			if(MouseOn(&Edit)) Hlight(&Edit);
			/*高亮显示帮助菜单项*/
			if(MouseOn(&Help)) Hlight(&Help);
			/*高亮显示关闭菜单项*/
			if(MouseOn(&Close)) Hlight(&Close);
			if(MouseOn(&Neeraj))
			{
				Msg("Hello, everyone");
				while(MouseOn(&Neeraj))
				{
					Status();
					if(button!=0) break;
					if(kbhit()) break;
				 }
				 Msg("");
			}
		}
	}
	Msg(":: Bye Bye ::");
}

/* 绘制菜单项 */
DrawBtn(struct MENU *tm,char *txt)
{
	/*设置绘制的颜色*/
	setcolor(0);
	/*设置文本格式*/
	settextstyle(2,0,4);
	/*输出txt文本*/
	outtextxy(tm->x+15,tm->y+4,txt);
}
/*高亮显示函数*/
Hlight(struct MENU *t)
{
/* 高亮 */
 winp(t,0);
/* 显示菜单项名称  */
 Msg(t->head);
 /*在没有输入的情况下，一直高亮*/
 while(MouseOn(t))
 {
	Status();
	if(button!=0) break;
	if(kbhit()) break;
 }
 /*消除高亮*/
 setcolor(7);
 Hide();
 rectangle(t->x,t->y,t->x1,t->y1);
 Msg("");
 /*重绘*/
 Show();
}

int MouseOn(struct MENU *t)
{
 int flag;
 flag=0;
 if(x>=t->x && x<=t->x1 && y>=t->y && y<=t->y1) flag=1;
 return flag;
}

void Xorbar(int sx,int sy,int ex,int ey,int c)
{
 int i;
 setwritemode(1);
 setcolor(c);
 Hide();
 for(i=sy;i<=ey;i++)
 line(sx,i,ex,i);
 Show();
 setwritemode(0);
}

Push(struct MENU *tm)
{
	winp(tm,1);
	while(button==1)
	{
		Status();
		if(kbhit()) break;
	}
}

Up(struct MENU *tm)
{
	setcolor(7);
	Hide();
	rectangle(tm->x,tm->y,tm->x1,tm->y1);
	Show();
	while(button==1)
	{
		Status();
		if(kbhit()) break;
	}
}

/*弹出下拉菜单*/
PullMenuDown(char *H[],int n,struct MENU *tm)
{
	int i;
	i=n*20;
	/* 保存背景*/
	Push(tm);
	/* 隐藏鼠标光标*/
	Hide();
	/* 把菜单项位图读入内存，以供收回弹出项时重新绘制*/
	getimage(tm->x+1,tm->y1+2,tm->x+100,tm->y1+i+5,p);
	/* 重新设置这块区域*/
	Win(tm->x+1,tm->y1+2,tm->x+100,tm->y1+i+5);
	setcolor(0);
	settextstyle(2,0,4);
	/*弹出下拉菜单项*/
	for(i=0;i<n;i++)
	{
		if(!tm->Active[i])
		{
			setcolor(15);
			outtextxy(tm->x+15,tm->y1+9+i*20,H[i]);
			setcolor(0);
			outtextxy(tm->x+14,tm->y1+8+i*20,H[i]);
		}
		else    outtextxy(tm->x+14,tm->y1+8+i*20,H[i]);
	}
	/*显示鼠标光标*/
	Show();
	while(button==1)  Status();
	HandleMenu(n,tm);
}

PullMenuUp(int n,struct MENU *tm)
{
	int i;
	/*隐藏鼠标光标*/
	Hide();
	/* 把读入内容的原菜单项位图重新绘制出来*/
	putimage(tm->x+1,tm->y1+2,p,COPY_PUT);
	/*显示鼠标光标*/
	Show();
	Up(tm);
}
/* 处理菜单项命令函数 */
HandleMenu(int n,struct MENU *tm)
{
	int i;
	char Key;
	setcolor(0);
	Selection=NONE;
	i=0;
	while(1)
	{
		Status();
		if(button==1) break;
		for(i=0;i<n;i++)
		{
			Status();
			if(MouseOn1(tm->x+4,tm->y1+6+i*20,tm->x+97,tm->y1+22+i*20))
			{
			    Xorbar(tm->x+3,tm->y1+6+i*20,tm->x+97,tm->y1+22+i*20,14);
			    while(MouseOn1(tm->x+4,tm->y1+6+i*20,tm->x+97,tm->y1+22+i*20))
			    {
				Status();
				if(button==1)
				{
					if(tm->Active[i]==YES)
					{
						Selection=i;
						break;
					}
					else
					Selection=NONE;
				}
				if(kbhit()) break;
			    }
			    Xorbar(tm->x+3,tm->y1+6+i*20,tm->x+97,tm->y1+22+i*20,14);
			}
		}
	}
	PullMenuUp(n,tm);
}

MouseOn1(int cx,int cy,int cx1,int cy1)
{
 int flag;
 flag=0;
 if(x>=cx && x<=cx1 && y>=cy && y<=cy1) flag=1;
 return flag;
}

/* Mouse & win routines */

union REGS i,o;
struct SREGS ss;

/* 显示鼠标光标函数 */
void Show()
{
	 i.x.ax=1;
	 int86(0x33,&i,&o);
}

/* 隐藏鼠标光标函数*/
void Hide()
{
	 i.x.ax=2;
	 int86(0x33,&i,&o);
}

/* 获得鼠标状态函数 */
void Status()
{
	 i.x.ax=3;
	 int86(0x33,&i,&o);
	 button=o.x.bx;
	 x=o.x.cx;
	 y=o.x.dx;
}
/*绘制菜单项的背景和边框*/
winp(struct MENU *t,int state)
{
   Hide();
   /*非高亮模式*/
   if(state==1)
   {
   	/*两条暗边框线*/
	setcolor(0);
	line(t->x,t->y,t->x,t->y1);
	line(t->x,t->y,t->x1,t->y);
	/*两条亮边框线*/
	setcolor(15);
	line(t->x1,t->y,t->x1,t->y1);
	line(t->x,t->y1,t->x1,t->y1);
  }
  /*高亮模式*/
  else
  {
	/*两条亮边框线*/
	setcolor(15);
	line(t->x,t->y,t->x,t->y1);
	line(t->x,t->y,t->x1,t->y);
	/*两条暗边框线*/
	setcolor(0);
	line(t->x1,t->y,t->x1,t->y1);
	line(t->x,t->y1,t->x1,t->y1);
  }
  Show();
}
/*绘制一个矩形区域*/
Win(int sx,int sy,int ex,int ey)
{
   Hide();
   /*设置填充的颜色*/
   setfillstyle(1,7);
   /*绘制一个区域*/
   bar(sx,sy,ex,ey);
   /*两条亮边框线*/
   setcolor(15);
   line(sx,sy,sx,ey);
   line(sx,sy,ex,sy);
   /*两条暗边框线*/
   setcolor(0);
   line(ex,sy,ex,ey);
   line(sx,ey,ex,ey);
   Show();
}

Msg(char *m)
{
 int l;
 setcolor(14);
 rectangle(18,438,622,477);
 setfillstyle(1,0);
 bar(20,440,620,475);
 if(MouseOn(&Neeraj))
 {
	 settextstyle(2,0,7);
	 setcolor(11);
	 l=textwidth(m)/2;
	 outtextxy(320-l,440,m);
 }
 else
 {
	 settextstyle(2,0,8);
	 setcolor(15);
	 l=textwidth(m)/2;
	 outtextxy(320-l,440,m);
	 outtextxy(321-l,441,m);
 }
}

Comments()
{
   setfillstyle(1,0);
   bar(140,5,500,40);
   setcolor(14);
   rectangle(140,5,500,40);
   settextstyle(2,0,8);
   setcolor(15);
   outtextxy(260,10,"GUI-MENUS");
   setfillstyle(1,8);
   bar(140,150,502,402);
   setcolor(14);
   rectangle(138,148,502,402);
   Msg("Welcome !");
   settextstyle(2,0,5);
   setcolor(10);
   outtextxy(158,180,"Hello Friends !");
   settextstyle(2,0,4);
   outtextxy(260,200,"Here i am with my menu program.");
   outtextxy(158,220,"As you can see it's like windows menu,you can");
   outtextxy(158,240,"add more menus to it. It's just a concept for");
   outtextxy(158,260,"building more menus. I hope you will like my effort,");
   outtextxy(158,280,"email me at :      n21@indiatimes.com");
   setcolor(14);
   outtextxy(370,300,"- Neeraj Sharma");
   line(380,304+textheight("A"),380+textwidth("Neeraj Sharma"),304+textheight("A"));
}

End()
{
  int i,j;
  setcolor(0);
  Hide();
  for(j=0;j<40;j++)
  {
	for(i=0;i<481;i+=20)
	   line(0,0+i+j,640,j+0+i);
	delay(100);
	if(kbhit()) if(getch()==27) break;
  }
  settextstyle(2,0,5);
  setcolor(15);
  outtextxy(220,180,"Thanks for being with us !");
  Show();
  delay(3000);
}

