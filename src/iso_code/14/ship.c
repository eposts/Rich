
# include <dos.h>
# include "13h.h"
# include <time.h>
# include <stdio.h>
# include "timer.h"
# include <alloc.h>
# include <stdlib.h>

struct stBOMB
{
  int  x;			
  int  y;
  BOOL Used;		
};
struct stEXPL
{
  int  x;			
  int  y;
  BYTE Num;
  BOOL Used;		
};
struct stSHIP
{
  int  x;			
  int  y;
  BOOL Used;		
  BYTE Dir;
  BYTE Type;
};
struct stTORP
{
  int  x;			
  int  y;
  BOOL Used;		
};

struct stBOMB bbBomb[21];
struct stSHIP spShip[21];
struct stTORP tpTorp[21];
struct stEXPL elExpl[21];

int BombUsed=0;
int ShipUsed=0;
int TorpUsed=0;
int ExplUsed=0;

int MAXBOMB=5;		
int MAXSHIP=3;		
int MAXTORP=5;		
int MAXEXPL=5;		

int ID_Timer[MAXTIMER+1];	
int ID_Bomb[21];
int ID_Ship[21];
int ID_Torp[21];
int ID_Expl[21];

BOOL DISP=TRUE;
BOOL CLS=FALSE;
BOOL bPaint=FALSE;
BOOL ALIVE=TRUE;
BOOL SOUND=TRUE;
BOOL Flash=FALSE;
BOOL DONE=FALSE;
BYTE SHIP[2][32][64];
BYTE BOMB[32][32];
BYTE TORP[16][16];
BYTE PAL[256][3];
BYTE STR[80];
BYTE far * PIC_EXPL;
BYTE far * PIC_SHIP;
void far * Back1;
void far * Back2;

int  MyX=0;
int  MyY=0;
int  Life=3;
int  Score=25;
int  LevelScore=25;
int  Level=0;
int  Step=0;
/*  */
char *Info="\
\\Welcome to play this game!\
\n\
\n\
\n\
	  In this game,you will drive a warship to fight with\n\
	the submarines.\n\n\
	     KEYS:\n\
	     'Left' and 'Right' keys to move your warship.\n\
	     'Space bar' or 'Ctrl' key to send bombs.\n\
	     'S' key is a sound switch.\n\
	     'D' key is a state switch.\n\n\
							      \n\
					 \n\n\
			       GOOD LUCK!\0";

/*  */
void StepIn()
{
  int i;
  for (i=0;i<256;i++) 
	  /*  */
	  setpal(i,PAL[i][0]*Step/64,PAL[i][1]*Step/64,PAL[i][2]*Step/64);
  Step++;
  if (Step>64) 
	  /*  */
	  KillAllTimer();
}

/*  */
void StepOut()
{
  int i;
  for (i=0;i<256;i++) 
	  /*  */
	  setpal(i,PAL[i][0]*Step/64,PAL[i][1]*Step/64,PAL[i][2]*Step/64);
  Step--;
  /*  */
  if (Step<1) 
	  KillAllTimer();
}
void OutInfo()
{
 if (*Info++)
 {
    sound(1000);
    printf("%c",*(Info-1));
    nosound();
 }
 else
   KillAllTimer();
}

void Logo()
{
  FILE *bmp;
  int  i,r,g,b,x,y;

  setmode();
  bmp=fopen("logo.bmp","rb");
  fseek(bmp,54,SEEK_SET);
  for (i=0;i<256;i++)
  {
      PAL[i][2]=fgetc(bmp)>>2;
      PAL[i][1]=fgetc(bmp)>>2;
      PAL[i][0]=fgetc(bmp)>>2;
      fgetc(bmp);
      setpal(i,0,0,0);
  }

  for (y=0;y<200;y++)
      for (x=0;x<320;x++) 
		  pokeb(0xa000,y*320+x,fgetc(bmp));

  fclose(bmp);
  CreateTimer(1,StepIn);		
  while (ScanKey()!=57&&TimerUsed) 
	  TimerEvent();
  Step=64;
  StepIn();
  wait();
  CreateTimer(1,StepOut);
  while (ScanKey()!=57&&TimerUsed) TimerEvent();
  Step=0;
  StepOut();
  closemode();
  CreateTimer(2,OutInfo);
  locate(6,26);
  while (ScanKey()!=57&&TimerUsed) 
	  TimerEvent();
  while (*Info++) 
	  printf("%c",*(Info-1));
  wait();
}

void KillSelf()
{
  if (SOUND)
  {
     sound(1000);
     delay(1000);
     nosound();
  }
  Life--;
  if (Life<0) ALIVE=FALSE;
}

void SetAllPal()
{
  FILE *act;
  int  i,r,g,b;

  act=fopen("comm.act","rb");
  for (i=0;i<256;i++)
  {
      r=fgetc(act)>>2;
      g=fgetc(act)>>2;
      b=fgetc(act)>>2;
      setpal(i,r,g,b);
  }
  fclose(act);
}


void readboat()
{
  FILE *bmp;
  int  x,y;

  bmp=fopen("boat.bmp","rb");
  fseek(bmp,1078,SEEK_SET);
  for (y=0;y<32;y++)
  {
      for (x=0;x<64;x++) SHIP[0][31-y][x]=fgetc(bmp);
      for (x=0;x<64;x++) SHIP[1][31-y][x]=fgetc(bmp);
  }
  fclose(bmp);
}
void readbomb()
{
  FILE *bmp;
  int  x,y;

  bmp=fopen("bomb.bmp","rb");
  fseek(bmp,516,SEEK_SET);
  for (y=0;y<32;y++)
      for (x=0;x<32;x++) BOMB[31-y][x]=fgetc(bmp);
  fclose(bmp);
}

void readtorp()
{
  FILE *bmp;
  int  x,y;

  bmp=fopen("torp.bmp","rb");
  fseek(bmp,516,SEEK_SET);
  for (y=0;y<16;y++)
      for (x=0;x<16;x++) TORP[15-y][x]=fgetc(bmp);
  fclose(bmp);
}
void readship()
{
  FILE *bmp;
  long i;

  PIC_SHIP=Back2;
  bmp=fopen("ship.bmp","rb");
  fseek(bmp,1078,SEEK_SET);
  for (i=0;i<10080;i++)
          *PIC_SHIP++=fgetc(bmp);
  fclose(bmp);
}

void readexpl()
{
  FILE *bmp;
  long i;

  PIC_EXPL=Back1;
  bmp=fopen("expl.bmp","rb");
  fseek(bmp,1078,SEEK_SET);
  for (i=0;i<59520;i++)
	  *PIC_EXPL++=fgetc(bmp);
  fclose(bmp);
}
void bomb(int xx,int yy)
{
  int y,x;
  for (y=0;y<32;y++)
      for (x=0;x<32;x++)
          if (BOMB[y][x]) 
			  Pset(x+xx,y+yy,BOMB[y][x]);
}
void boat(int xx,int yy,int num)
{
  int y,x;
  for (y=0;y<32;y++)
      for (x=0;x<64;x++)
	  if (SHIP[num][y][x]) 
		  Pset(x+xx,y+yy,SHIP[num][y][x]);
}
void torp(int xx,int yy)
{
  int y,x;
  for (y=0;y<16;y++)
      for (x=0;x<16;x++)
          if (TORP[y][x]) 
			  Pset(x+xx,y+yy,TORP[y][x]);
}
void ship(int xx,int yy,BYTE Type,BYTE Dir)
{
  int x,y;

  bPaint=TRUE;
  PIC_SHIP=Back2;
  PIC_SHIP+=(long)(Type*2+Dir)*63l;
  for (y=0;y<20;y++)
  {
      for (x=0;x<63;x++)
	  if (*PIC_SHIP++) 
		  Pset(x+xx,19-y+yy,*(PIC_SHIP-1));
      PIC_SHIP+=441l;
  }
}

int CreateTorp(int x,int y)
{
  int i=0;

  if (TorpUsed==MAXTORP) return NULL;

  while (tpTorp[++i].Used);

  tpTorp[i].x=x;
  tpTorp[i].y=y;
  tpTorp[i].Used=TRUE;
  TorpUsed++;
  return i;
}
void KillTorp(int *TorpID)
{
  if (tpTorp[*TorpID].Used)
  {
     TorpUsed--;
     tpTorp[*TorpID].Used=FALSE;
  }
  *TorpID=NULL;
}
void DrawTorp()
{
  int i=0;
  int ID;

  while (!tpTorp[++i].Used);

  if (i==MAXTORP+1) return;
  bPaint=TRUE;
  if (!CLS)
  {
     ClearScreen(2);
     CLS=TRUE;
  }
  for (i=1;i<=MAXTORP;i++)
  {
      if (tpTorp[i].Used)
      {
	 if (tpTorp[i].y<34)
	 {
	    KillTorp(&ID_Torp[i]);
            if (tpTorp[i].x-MyX>10&&tpTorp[i].x-MyX<50)
	    {
	       ID=CreateExpl(tpTorp[i].x-23,tpTorp[i].y-26);
	       if (ID) ID_Expl[ID]=ID;
	       KillSelf();
	    }
	 }
	 torp(tpTorp[i].x,tpTorp[i].y);
	 tpTorp[i].y--;
      }
  }

}
int CreateShip(int y,BYTE Type)
{
  int i=0;

  if (ShipUsed==MAXSHIP) return NULL;

  while (spShip[++i].Used);

  spShip[i].y=y;
  spShip[i].Used=TRUE;
  spShip[i].Dir=random(2);
  spShip[i].Type=Type;
  spShip[i].x=-64;
  if (!spShip[i].Dir) spShip[i].x=320;
  ShipUsed++;
  return i;
}
void KillShip(int *ShipID)
{
  if (spShip[*ShipID].Used)
  {
     ShipUsed--;
     spShip[*ShipID].Used=FALSE;
  }
  *ShipID=NULL;
}
void DrawShip()
{
  int i=0;
  int ID;

  while (!spShip[++i].Used);

  if (i==MAXSHIP+1) return;
  bPaint=TRUE;
  if (!CLS)
  {
     ClearScreen(2);
     CLS=TRUE;
  }
  for (i=1;i<=MAXSHIP;i++)
  {
      if (spShip[i].Used)
      {
	 if (random(10)==3)
	 {
	    ID=CreateTorp(spShip[i].x+32,spShip[i].y);
	    if (ID) ID_Torp[ID]=ID;
	 }
         ship(spShip[i].x,spShip[i].y,spShip[i].Type,spShip[i].Dir);
	 if (spShip[i].Dir)
         {
            spShip[i].x+=2;
            if (spShip[i].x>320) 
				KillShip(&ID_Ship[i]);
         }
         else
         {
            spShip[i].x-=2;
            if (spShip[i].x<-64) 
				KillShip(&ID_Ship[i]);
         }
      }
  }

}
int CreateExpl(int x,int y)
{
  int i=0;

  if (ExplUsed==MAXEXPL) return NULL;

  while (elExpl[++i].Used);

  elExpl[i].x=x;
  elExpl[i].y=y;
  elExpl[i].Used=TRUE;
  elExpl[i].Num=0;
  ExplUsed++;
  return i;
}
void KillExpl(int *ExplID)
{
  if (elExpl[*ExplID].Used)
  {
     ExplUsed--;
     elExpl[*ExplID].Used=FALSE;
     elExpl[*ExplID].Num=0;
  }
  *ExplID=NULL;
}

void DrawExpl()
{
  int  x,y,i=0;

  while (!elExpl[++i].Used);

  if (i==MAXEXPL+1) return;
  bPaint=TRUE;
  if (!CLS)
  {
     ClearScreen(2);
     CLS=TRUE;
  }
  for (i=1;i<=MAXEXPL;i++)
  {
      if (elExpl[i].Num==15) 
		  KillExpl(&ID_Expl[i]);
      if (elExpl[i].Used)
      {
         bPaint=TRUE;
         PIC_EXPL=Back1;
         PIC_EXPL+=(long)elExpl[i].Num*62l;
         elExpl[i].Num++;
         for (y=0;y<64;y++)
         {
	     if (SOUND) sound(random(2000)+500);
	     for (x=0;x<62;x++)
		 if (*PIC_EXPL++) 
			 Pset(x+elExpl[i].x,(63-y)+elExpl[i].y,*(PIC_EXPL-1));
	     PIC_EXPL+=870l;
	 }
	 nosound();
      }
  }
}

int CreateBomb(int x,int y)
{
  int i=0;

  if (BombUsed==MAXBOMB) return NULL;

  while (bbBomb[++i].Used);

  bbBomb[i].x=x;
  bbBomb[i].y=y;
  bbBomb[i].Used=TRUE;
  BombUsed++;
  return i;
}
void KillBomb(int *BombID)
{
  if (bbBomb[*BombID].Used)
  {
     BombUsed--;
     bbBomb[*BombID].Used=FALSE;
  }
  *BombID=NULL;
}
void DrawBomb()
{
  int i=0,j;
  int ID;

  while (!bbBomb[++i].Used);

  if (i==MAXBOMB+1) return;
  bPaint=TRUE;
  if (!CLS)
  {
     ClearScreen(2);
     CLS=TRUE;
  }
  for (i=1;i<=MAXBOMB;i++)
  {
      if (bbBomb[i].Used)
      {
         for (j=1;j<=MAXSHIP;j++)
         {
	     if (spShip[j].Used&&abs(spShip[j].y-bbBomb[i].y)<16&&abs(spShip[j].x-bbBomb[i].x+16)<32)
             {
		switch (spShip[j].Type)
                {
                  case 0:
                       Score+=10;
                       LevelScore+=10;
                       break;
                  case 1:
                       Score+=20;
                       LevelScore+=20;
                       break;
                  case 2:
                       Score+=40;
                       LevelScore+=40;
                       break;
                  case 3:
                       Life++;
                       break;
                }
		KillShip(&ID_Ship[j]);
		KillBomb(&ID_Bomb[i]);
		ID=CreateExpl(bbBomb[i].x-20,bbBomb[i].y);
		if (ID) ID_Expl[ID]=ID;
	     }
         }
         bomb(bbBomb[i].x,bbBomb[i].y);
	 bbBomb[i].y+=2;
	 if (bbBomb[i].y>170) KillBomb(&ID_Bomb[i]);
      }
  }
}

void DrawObject()
{
  CLS=FALSE;
  DrawBomb();
  DrawShip();
  DrawTorp();
  if (!bPaint) return;
  if (!CLS) ClearScreen(2);
  boat(MyX,2,Flash);
  DrawExpl();
  if (DISP)
  {
     OutString(0,0,"LIFE:",96);
     itoa(Life+1,STR,10);
     OutString(5,0,STR,96);
     if (SOUND) OutString(7,0,"Sound",2);
     itoa(MAXBOMB-BombUsed,STR,10);
     OutString(13,0,"BOMB:",89);
     OutString(18,0,STR,89);
     itoa(Level+1,STR,10);
     OutString(21,0,"LEVEL:",13);
     OutString(27,0,STR,13);
     OutString(30,0,"SCORE:",52);
     itoa(Score,STR,10);
     OutString(36,0,STR,52);
  }
  ReFresh();
}

void InitGame()
{
  int i;

  BombUsed=0;
  ShipUsed=0;
  TorpUsed=0;
  ExplUsed=0;

  for (i=1;i<=MAXSHIP;i++) KillShip(&ID_Ship[i]);
  for (i=1;i<=MAXBOMB;i++) KillBomb(&ID_Bomb[i]);
  for (i=1;i<=MAXTORP;i++) KillTorp(&ID_Torp[i]);
  for (i=1;i<=MAXEXPL;i++) KillExpl(&ID_Expl[i]);

  Flash=FALSE;
  ALIVE=TRUE;
  DONE=FALSE;
  MyX=0;
  MyY=0;
  Life=3;
  Score=25;
  LevelScore=25;
  Level=0;
}

void GameMain()
{
  BYTE   bKey;
  int    ShipType;
  int    i,ID;

  bKey=ScanKey();
  while (bKey!=1&&ALIVE)
  {
    TimerEvent();
    if (Life==0)
       Flash=!Flash;
    else
       Flash=FALSE;
    if (random(10)==4)
    {
       i=random(100);
       if (i<50)
          ShipType=0;
       else if (i>=50&&i<75)
          ShipType=1;
       else if (i>=75&&i<95)
          ShipType=2;
       else if (i>95)
          ShipType=3;
       ID=CreateShip(random(6)*20+70,ShipType);
       if (ID) ID_Ship[ID]=ID;
    }
    switch (bKey)
    {
      case 115:
      case 75:
	   bPaint=TRUE;
           MyX-=4;
           if (MyX<0) MyX=0;
	   break;
      case 2:
	   Life++;
	   break;
      case 3:
	    MAXBOMB++;
	    if (MAXBOMB>20) MAXBOMB=20;
      case 77:
      case 116:
	   bPaint=TRUE;
           MyX+=4;
	   if (MyX>256) MyX=256;
           break;
      case 31:
	   SOUND=!SOUND;
	   break;
      case 32:
	   DISP=!DISP;
	   break;
      case 57:
	   if (LevelScore==0) break;
           ID=CreateBomb(MyX+20,32);
	   if (ID)
	   {
	      Score-=5;
	      LevelScore-=5;
	      ID_Bomb[ID]=ID;
	   }
    }
    bKey=ScanKey();
    if (LevelScore>=500)
    {
       Level++;
       LevelScore=0;
       if (Level<15)
	  OutString(15,10,"Next Level!",13);
       else
       {
	  OutString(14,10,"Very Good!!",13);
	  DONE=TRUE;
	  bKey=1;
       }
       ReFresh();
       wait();
    }
    MAXSHIP=Level+3;
    MAXTORP=Level+5;
    MAXEXPL=Level+5;
  }

}

void main()
{
  BOOL QUIT=FALSE;
  int  nKey;

  Video=farmalloc(64000);
  if (Video==NULL)
  {
     printf("Not enough Memory\n");
     exit(0);
  }
  Back1=farmalloc(59520);
  if (Back1==NULL)
  {
     printf("Not enough Memory\n");
     farfree(Video);
     exit(0);
  }
  Back2=farmalloc(10080);
  if (Back2==NULL)
  {
     printf("Not enough Memory\n");
     farfree(Back2);
     farfree(Video);
     exit(0);
  }

  randomize();
  Logo();
  setmode();
  SetAllPal();
  readboat();
  readship();
  readexpl();
  readbomb();
  readtorp();
  ClearScreen(2);
  boat(0,2,0);
  KillAllTimer();
  CreateTimer(1,DrawObject);
  ReFresh();

  while (!QUIT)
  {
     GameMain();
     bPaint=TRUE;
     if (!ALIVE)
	OutString(15,10,"GAME OVER",96);
     OutString(14,12,"QUIT? (Y/N)",89);
     ReFresh();
     nKey=getch();
     while (!(nKey=='Y'||nKey=='y'||nKey=='N'||nKey=='n'))
     {
	nKey=getch();
     }
     if (nKey=='Y'||nKey=='y') QUIT=TRUE;
     if (!ALIVE||DONE) 
		 InitGame();
  }
  closemode();
  KillAllTimer();
  farfree(Video);
  farfree(Back1);
  farfree(Back2);
  printf("***NOBLE STUDIO***Written By FanobleMeng--2001.12");
}
