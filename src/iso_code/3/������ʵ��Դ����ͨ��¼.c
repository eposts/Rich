/*编程建立一通讯簿，存放有姓名、电话号码、住址，然后对通信簿进行查找、添加、修改及删除。*/
#include<stdio.h>
struct person
{
	char name[8];
    char tel[15];
    char addr[50];
};

char filename[20];
FILE *fp;

void creat();
void output();
void search();
void append();
void modify();
void delete();

main()
{
	int m;
  
	creat();   
	while(1)
	{
		printf("\n\n添加，请按1");
 		printf("\n查找，请按2");
		printf("\n修改，请按3");
		printf("\n删除，请按4");
		printf("\n输出，请按5");
		printf("\n退出，请按0\n");
		scanf("%d",&m);
		if(m>=0&&m<=5)
		{
			switch(m)
    		{
			case 1: append();
    				break;
			case 2: search();
					break;
			case 3: modify();
					break;
			case 4: delete();
					break;
			case 5: output();
					break;
			case 0: exit();
			}
		 printf("\n\n操作完毕，请再次选择！");
		 }
		else
			printf("\n\n选择错误，请再次选择！");
	}
}


void creat()
{
	struct person one;
    long s1;

    printf("\n请输入通讯簿名：");
    scanf("%s",filename);
    if((fp=fopen(filename,"w"))==NULL)
    {
		printf("\n不能建立通讯簿！");
		exit();
    }
    fprintf(fp,"%-10s%-20s%-50s\n","姓名","电话号码","住址");
    printf("\n请输入姓名、电话号码及住址（以0结束）\n");
    scanf("%s",one.name);
    while(strcmp(one.name,"0"))
    {
		scanf("%s%s",one.tel,one.addr);
		fprintf(fp,"%-10s%-20s%-50s\n",one.name,one.tel,one.addr);
		scanf("%s",one.name);
    }
    fclose(fp);
}

void output()
{
	struct person one;

    if((fp=fopen(filename,"r"))==NULL)
    {
		printf("\n不能打开通讯簿！");
		exit();
    }
    printf("\n\n%20s\n","通 讯 簿");
    while(!feof(fp))
    {
		fscanf(fp,"%s%s%s\n",one.name,one.tel,one.addr);
		printf("%-10s%-20s%-50s",one.name,one.tel,one.addr);
    }
    fclose(fp);
}

void append()
{
	struct person one;

    if((fp=fopen(filename,"a"))==NULL)
    {
		printf("\n不能打开通讯簿！");
		exit();
    }
    printf("\n请输入添加的姓名、电话号码及住址\n");
    scanf("%s%s%s",one.name,one.tel,one.addr);
    fprintf(fp,"%-10s%-20s%-50s\n",one.name,one.tel,one.addr);
    fclose(fp);
}

void search()
{  
	int k=0;
    char namekey[8];
    struct person one;

    printf("\n请输入姓名:");
    scanf("%s",namekey);

    if((fp=fopen(filename,"rb"))==NULL)
    {  
		printf("\n不能打开通讯簿！");
		exit();
    }
    while(!feof(fp))
    {  
		fscanf(fp,"%s%s%s\n",one.name,one.tel,one.addr);
		if(!strcmp(namekey,one.name))
		{
			printf("\n\n已查到，记录为：");
			printf("\n%-10s%-18s%-50s",one.name,one.tel,one.addr);
			k=1;
        }
    }
    if(!k)  
		printf("\n\n对不起，通讯簿中没有此人的记录。");
    fclose(fp);
}

void modify()
{  
	int m,k=0;
    long offset;
    char namekey[8];
    struct person one;

    printf("\n请输入姓名:");
    scanf("%s",namekey);

    if((fp=fopen(filename,"r+"))==NULL)
    {  
		printf("\n不能打开通讯簿！");
		exit();
    }
    while(!feof(fp))
    {  
		offset=ftell(fp);
		fscanf(fp,"%s%s%s\n",one.name,one.tel,one.addr);
		if(!strcmp(namekey,one.name))
		{
			k=1;
			break;  
		}
    }
    if(k)
	{  
		printf("\n已查到，记录为：");
        printf("\n%-10s%-18s%-50s",one.name,one.tel,one.addr);
        printf("\n请输入新姓名、电话号码及住址：");
		scanf("%s%s%s",one.name,one.tel,one.addr);
		fseek(fp,offset,SEEK_SET);
		printf("%ld",ftell(fp));
		fprintf(fp,"%-10s%-20s%-50s\n",one.name,one.tel,one.addr);
	}
    else
		printf("\n对不起，通讯簿中没有此人的记录。");
    fclose(fp);
}

void delete()
{  
	int m,k=0,flag;
    long offset1,offset2;
    char namekey[8], valid[4];
    struct person one;

    printf("\n请输入姓名:");
    scanf("%s",namekey);
    if((fp=fopen(filename,"r+"))==NULL)
    {  
		printf("\n不能打开通讯簿！");
		exit();
    }
    while(!feof(fp))
    {  
		offset1=ftell(fp);
		fscanf(fp,"%s%s%s\n",one.name,one.tel,one.addr);
		if(!strcmp(namekey,one.name))
		{   
			k=1;
			break; 
		}
    }
    if(k)
	{ 
		printf("\n已查到，记录为");
        printf("\n%-10s%-18s%-50s",one.name,one.tel,one.addr);
        printf("\n确实要删除,按1；不删除，按0：");
        scanf("%d",&m);
        if(m)
		{   
			fseek(fp,offset1,SEEK_SET);
			fprintf(fp,"%-10s%-20s%-50s\n","","","");
		}
	}
    else
		printf("\n对不起，通讯簿中没有此人的记录。");
    fclose(fp);
}

