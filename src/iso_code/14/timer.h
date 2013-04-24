
# define  MAXTIMER     10

struct TM
{
  DWORD Interval;              
  DWORD LastTimer;             
  BOOL  Enable;                
  BOOL  Used;                 
  void  (*Pointer)();          
};

struct TM tmTM[MAXTIMER+1];
int    TimerUsed=0;


DWORD BiosTimer(void)
{
  DWORD BIOSTIMER=0;
  BIOSTIMER=peek(0x0,0x46e);
  BIOSTIMER<<=8;
  BIOSTIMER+=peek(0x0,0x46c);
  return (BIOSTIMER);
}


void TimerEvent()
{
  int   i;
  DWORD TimerDiff;

  for (i=1;i<=MAXTIMER;i++)
  {
      if (tmTM[i].Used&&tmTM[i].Enable)
      {
         TimerDiff=BiosTimer()-tmTM[i].LastTimer;
	 if (tmTM[i].Interval<=TimerDiff)
         {
            tmTM[i].Pointer();
            tmTM[i].LastTimer=BiosTimer();
         }
      }
  }
}


int CreateTimer(DWORD Interval,void (*Pointer)())
{
  int i=0;
  if (TimerUsed==MAXTIMER) return NULL;

  while (tmTM[++i].Used);

  tmTM[i].Pointer=Pointer;
  tmTM[i].Interval=Interval;
  tmTM[i].Enable=TRUE;
  tmTM[i].Used=TRUE;
  tmTM[i].LastTimer=BiosTimer();

  TimerUsed++;
  return i;
}


void KillTimer(int *TimerID)
{
  if (tmTM[*TimerID].Used)
  {
     TimerUsed--;
     tmTM[*TimerID].Used=FALSE;
  }
  *TimerID=0;
}


void KillAllTimer()
{
  int i;
  for (i=0;i<=MAXTIMER;i++) tmTM[i].Used=FALSE;
  TimerUsed=0;
}