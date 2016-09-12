#include "vesa8bit.h"
#include<stdio.h>
#include<dos.h>

#define NOTE_1    440 /* 音调频率 */
#define NOTE_2    495
#define NOTE_3    550
#define NOTE_4    587
#define NOTE_5    660
#define NOTE_6    733
#define NOTE_7    825
#define ONE_BEEP  600 /* 一拍延时600ms */
#define HALF_BEEP 300 /* 半拍延时300ms */

int m;
char a[1000];
int x,y;
int deep,level;
int func[100];
int xb,yb,xcon[20],ycon[20],bbcon;
const int n=15;
int getkeyscode();
int  draw_main(int loc);
int  draw_sent(int begin,int end,int *x1,int *y1);
int nots(int loca);
void sort(char loca[],int tail);
char *displaymenu1();
char *displaymenu2();
int  draw_switch(int locat);
int cmps(char *key,char *a,int head,int tail);
int find(char *key,char *a,int head,int tail);
void  text_main(char *a,int length);
void draw_arrow(int x,int y,int x1,int y1);
void draw_diam(int x,int y);
void draw_rect(int x,int y);
void music();
int draw_if(int locat);
int draw_while(int locat);
typedef void interrupt (*InterruptFunctionPointer)(void);
InterruptFunctionPointer old_8h;
int ticks = 0;

typedef struct
{
   unsigned short int freq;
   unsigned short int delay;
} SONG;

SONG song[] =
{
   {NOTE_5, ONE_BEEP},
   {NOTE_3, HALF_BEEP},
   {NOTE_5, HALF_BEEP},
   {NOTE_1*2, ONE_BEEP*2},
   {NOTE_6, ONE_BEEP},
   {NOTE_1*2, ONE_BEEP},
   {NOTE_5, ONE_BEEP*2},
   {NOTE_5, ONE_BEEP},
   {NOTE_1, HALF_BEEP},
   {NOTE_2, HALF_BEEP},
   {NOTE_3, ONE_BEEP},
   {NOTE_2, HALF_BEEP},
   {NOTE_1, HALF_BEEP},
   {NOTE_2, ONE_BEEP*4},
   {NOTE_5, ONE_BEEP},
   {NOTE_3, HALF_BEEP},
   {NOTE_5, HALF_BEEP},
   {NOTE_1*2, HALF_BEEP*3},
   {NOTE_7, HALF_BEEP},
   {NOTE_6, ONE_BEEP},
   {NOTE_1*2, ONE_BEEP},
   {NOTE_5, ONE_BEEP*2},
   {NOTE_5, ONE_BEEP},
   {NOTE_2, HALF_BEEP},
   {NOTE_3, HALF_BEEP},
   {NOTE_4, HALF_BEEP*3},
   {NOTE_7/2, HALF_BEEP},
   {NOTE_1, ONE_BEEP*4},
   {NOTE_6, ONE_BEEP},
   {NOTE_1*2, ONE_BEEP},
   {NOTE_1*2, ONE_BEEP*2},
   {NOTE_7, ONE_BEEP},
   {NOTE_6, HALF_BEEP},
   {NOTE_7, HALF_BEEP},
   {NOTE_1*2, ONE_BEEP*2},
   {NOTE_6, HALF_BEEP},
   {NOTE_7, HALF_BEEP},
   {NOTE_1*2, HALF_BEEP},
   {NOTE_6, HALF_BEEP},
   {NOTE_6, HALF_BEEP},
   {NOTE_5, HALF_BEEP},
   {NOTE_3, HALF_BEEP},
   {NOTE_1, HALF_BEEP},
   {NOTE_2, ONE_BEEP*4},
   {NOTE_5, ONE_BEEP},
   {NOTE_3, HALF_BEEP},
   {NOTE_5, HALF_BEEP},
   {NOTE_1*2, HALF_BEEP*3},
   {NOTE_7, HALF_BEEP},
   {NOTE_6, ONE_BEEP},
   {NOTE_1*2, ONE_BEEP},
   {NOTE_5, ONE_BEEP*2},
   {NOTE_5, ONE_BEEP},
   {NOTE_2, HALF_BEEP},
   {NOTE_3, HALF_BEEP},
   {NOTE_4, HALF_BEEP*3},
   {NOTE_7/2, HALF_BEEP},
   {NOTE_1, ONE_BEEP*3},
};

void main()
{
  char *address;
  char key[20];
  char s[100];
  int ky,k,p,q,t;
  int b[100],loc,length;
  FILE * fp;
  int choice,i,j;
  set_vesa_mode_1024x768x8bit();    /* 切换显卡到1024x768x256色模式 */
  display_8bit_bmp("hello.bmp"); /* 显示一张图片 */
  bioskey(0);

  
  address=displaymenu1();
  fp=fopen(address,"r");
  i=0;
  bbcon=0;
  deep=0;
  level=0;
  k=1;
  while (!feof(fp))
  {
    a[i]=fgetc(fp);
    i++;
  }
  length=i-1;
  text_main(a,length);
  ky=0;
  while (ky!=1)
  {
  loc=func[k];
  set_vesa_mode_1024x768x8bit();
  p=loc;
  while (a[p]!=10)
    p--;
  q=--p;
  while ((a[q]!=10)&&(q!=0))
    q--;
  while ((a[q]==10)||(a[q]==32))
    q++;
  for (j=q;j<=p;j++)
    s[j-q]=a[j];
    s[j-q]='\0';
  vesa_puts(0,0,4,"now : ");
  vesa_puts(50,0,4,s);
  for (t=1;t<=func[0];t++)
  {
    p=func[t];
    while (a[p]!=10)
      p--;
    q=--p;
    while ((a[q]!=10)&&(q!=0))
      q--;
    while ((a[q]==10)||(a[q]==32))
      q++;
	s[0]=t+'0';
	s[1]='\0';
	vesa_puts(0,20*t,4,s);
	vesa_puts(16,20*t,4," for : ");
    for (j=q;j<=p;j++)
      s[j-q]=a[j];
      s[j-q]='\0';
	vesa_puts(72,20*t,4,s);
  }

  x=512;
  draw_rect(512,0);
  vesa_puts(512-20,n-8,4,"begin");
  y=4*n;
  draw_main(loc);
  vesa_line(x-m,y,x+m,y,4);
  vesa_line(x-m,y,x-m,y+2*n,4);
  vesa_line(x-m,y+2*n,x+m,y+2*n,4);
  vesa_line(x+m,y,x+m,y+2*n,4);
  vesa_puts(x-12,y+n-8,4,"end");
  ky=getkeyscode();
  switch (ky)
	{
	  case 1: break;
	  case 2:k=1;break;
	  case 3:k=2;break;
	  case 4:k=3;break;
	  case 75:k--;if (k==0) k=func[0];break;
	  case 5:k=4;break;
	  case 6:k=5;break;
	  case 7:k=6;break;
	  case 8:k=7;break;
	  case 9:k=8;break;
	  case 77:k++;if (k>func[0]) k=1;break;
	  default:break;
	}
  }
    set_vesa_mode_1024x768x8bit();    
    display_8bit_bmp("farewell.bmp");
    bioskey(0);
}
int getkeyscode()
{
  union REGS rg;
  rg.h.ah=0;
  int86(0x16,&rg,&rg);
  return rg.h.ah;
}
int nots(int loca)
{
  if (((a[loca]>65)&&(a[loca]<90))||((a[loca]>97)&&(a[loca]<122))||((a[loca]>48)&&(a[loca]<57)))
    return 0;
  return 1;
}
void sort(char loca[],int tail)
{
  int i,j,temp;
  for (i=0;i<=tail;i++)
    for (j=i;j<=tail;j++)
	  if (loca[j]<loca[i])
	  {
	    temp=loca[i];
		loca[i]=loca[j];
		loca[j]=temp;
	  }
}
char *displaymenu1()
{
	char address[100];
	system("cls");
	printf("\n\n\n\n\n\n\n\n\n\n\n\n");
	printf("     please enter the file address: ");
	scanf("%s",address);
	return address;
}
char *displaymenu2()
{
	char address[100];
	system("cls");
	printf("\n\n\n\n\n\n\n\n\n\n\n\n");
	printf("     please enter the output address: ");
	scanf("%s",address);
	return address;
}
int cmps(char *key,char *a,int head,int tail)
{
  int i;
  for (i=head;i<=tail;i++)
    if (key[i-head]!=a[i])
	  return 0;
  return 1;
}
int find(char *key,char *a,int head,int tail)
{
  int i,len,j;
  int bb;
  len=strlen(key);
  for (i=head;i+len<tail;i++)
    {
      bb=1;
      for (j=0;j<len;j++)
 	if (key[j]!=a[i+j])
 	  bb=0;
      if (bb)
	return i;
    }
  return 0;
}
void  text_main(char *a,int length)
{
  int bb,i,j,k,lev,head,tail,line,begin,end;
  char *address;
  FILE * fp;
  int fp1[100],fpnow;
  int li[100];
  address=displaymenu2();
  fp=fopen(address,"w");
  i=0;
  m=0;
  func[0]=0;
  lev=0;
  line=0;
  while (i<=length)
   {
	  if ((a[i]==10)||(i==0))
	    {
		  line++;
		  bb=0;
		  head=i;
		  tail=i+1;
		  while ((a[tail]!=10)&&(tail<=length))
		    tail++;
		  tail--;
		  if (head!=0)
		  fprintf(fp,"%c",a[head]);
		  fprintf(fp,"%3.d",line);
		  for (j=head;j<=tail;j++)
		    if (a[j]=='{')
			  {
			    bb=1;
			    lev++;
				if (lev==1)
				  {
				    func[0]++;
					func[func[0]]=j-1;
				  }
				for (k=1;k<=lev;k++)
				  fprintf(fp,"-");
			    li[lev]=line;
			  }
		  for (j=head;j<=tail;j++)
			if (a[j]=='}')
			  {
			    bb=2;
				for (k=1;k<=lev;k++)
				  fprintf(fp,"_");
			    fpnow=ftell(fp);
			    fseek(fp,fp1[lev],0);
			    fprintf(fp,"       /*begin line: %d end line: %d*/",li[lev],line);
			    fseek(fp,fpnow,0);
			    lev--;
			  }
		  if ((!bb)&&(lev)) {fprintf(fp,"|       ");}
		  if ((!bb)&&(!lev)) fprintf(fp,"        ");
		  if (bb)
            for (k=lev+1;k<=8;k++)
	      fprintf(fp," ");
          for (j=head+1;j<=tail;j++)
		  {
	        fprintf(fp,"%c",a[j]);
	      }
		  if (bb==1)
		    fp1[lev]=ftell(fp)+1;
	  if (bb)
	  for (j=0;j<=50;j++)
	    fprintf(fp," ");
		begin=head+1;
		end=tail;
		while ((a[begin]==10)||(a[begin]==32))
		  begin++;
		if (((end-begin)*4.2)>m)
		  m=((end-begin)*4.2);

		i=tail+1;
		}
		else i++;
    }
}
void draw_arrow(int x,int y,int x1,int y1)
{
  vesa_line(x,y,x1,y1,4);
  if (x1>x) x=x1-2*n;
  if (y1>y) y=y1-2*n;
  if (x1<x) x=x1+2*n;
  if (y1<y) y=y1+2*n;
  vesa_line((x+7*x1+y1-y)/8,(y+7*y1+x-x1)/8,x1,y1,4);
  vesa_line((x+7*x1+y-y1)/8,(y+7*y1+x1-x)/8,x1,y1,4);
}
void draw_diam(int x,int y)
{
  vesa_line(x,y,x-m,y+n,4);
  vesa_line(x,y,x+m,y+n,4);
  vesa_line(x-m,y+n,x,y+2*n,4);
  vesa_line(x+m,y+n,x,y+2*n,4);
  draw_arrow(x,y+2*n,x,y+4*n);
}

void draw_rect(int x,int y)
{
  vesa_line(x-m,y,x+m,y,4);
  vesa_line(x-m,y,x-m,y+2*n,4);
  vesa_line(x-m,y+2*n,x+m,y+2*n,4);
  vesa_line(x+m,y,x+m,y+2*n,4);
  draw_arrow(x,y+2*n,x,y+4*n);
}
int draw_if(int locat)
{
  int i,j,k,kuo,fen,bb,head,tail,xe,ye,kuoe,lx,ly,bbe;
  char s[50];
  kuo=0;
  fen=0;
  bbe=0;
  level++;
  deep++;
  i=locat;
  i=i+3;
  while ((a[i]!=10)&&(a[i]!=32))
    i++;
  i--;
  xe=x;
  ye=y;
  for (j=locat+3;j<=i;j++)
    s[j-locat-3]=a[j];
    s[j-locat-3]='\0';
  vesa_puts(x-(j-locat-3)*4,y+n-8,4,s);
  draw_diam(x,y);
  y=y+4*n;
  i=i+2;
  locat=i;
  while (a[i]!=10)
    i++;
  i--;
  for (j=locat;j<=i;j++)
    {
	if (a[j]=='{')
	  kuo++;
	if (a[j]==';')
	  fen++;
	}
  if (!kuo)
  {
   while(!((a[i]==';')&&(a[i+1]==10)))
	  i++;
    i=draw_sent(locat,i,&x,&y);
    i=i+2;
    locat=i;
    while (a[i]!=10)
      i++;
    i--;
	tail=i;
	head=locat;
	for (k=head;k<=tail;k++)
	  {
	    if ((cmps("else",a,k,k+3))&&(nots(k+4)))
		  {
		    bbe=1;
		    vesa_line(xe+m,ye+n,xe+((deep-level+1)*2.1*m),ye+n,4);
			vesa_line(xe+((deep-level+1)*2.1*m),ye+n,xe+((deep-level+1)*2.1*m),ye+3*n,4);
			xe=xe+((deep-level+1)*2.1*m);
			ye=ye+3*n;
			locat=k+6;
			i=locat;
			while (a[i]!=10)
			  i++;
			 i--;
			 kuoe=0;
			 for (j=locat;j<=i;j++)
			   if (a[j]=='{')
			     kuoe++;
			 if (kuoe)
			   {
			     lx=x;
				 ly=y;
				 x=xe;
				 y=ye;
			     i=draw_main(i);
  				 xe=x;
				 ye=y;
				 x=lx;
				 y=ly;
				 if (ye<=(y-n))
			  {
			    vesa_line(xe,ye,xe,y-n,4);
				draw_arrow(xe,y-n,x,y-n);
			  }
			  if (ye>(y-n))
			  {
				vesa_line(xe,ye,xe-1.05*m,ye,4);
				vesa_line(xe-1.05*m,ye,xe-1.05*m,y-n,4);
				draw_arrow(xe-1.05*m,y-n,x,y-n);
			  }
			   }
			 if (!kuoe)
			   {    
			while(!((a[i]==';')&&(a[i+1]==10)))
	              i++;

			     i=draw_sent(locat,i,&xe,&ye);
			   }
			  if (ye<=(y-n))
			  {
			    vesa_line(xe,ye,xe,y-n,4);
				draw_arrow(xe,y-n,x,y-n);
			  }
			  if (ye>(y-n))
			  {
				vesa_line(xe,ye,xe-1.05*m,ye,4);
				vesa_line(xe-1.05*m,ye,xe-1.05*m,y-n,4);
				draw_arrow(xe-1.05*m,y-n,x,y-n);
			  }
		  }
	  }

  }
  if (kuo)
    {
      i=draw_main(locat);
	  i=i+2;
    locat=i;
    while (a[i]!=10)
      i++;
    i--;
	tail=i;
	head=locat;
	for (k=head;k<=tail;k++)
	  {
	    if ((cmps("else",a,k,k+3))&&(nots(k+4)))
		  {
		    bbe=1;
		    vesa_line(xe+m,ye+n,xe+((deep-level+1)*2.1*m),ye+n,4);
			vesa_line(xe+((deep-level+1)*2.1*m),ye+n,xe+((deep-level+1)*2.1*m),ye+3*n,4);
			xe=xe+((deep-level+1)*2.1*m);
			ye=ye+3*n;
			locat=k+5;
			i=locat;
			while (a[i]!=10)
			  i++;
			 i--;
			 kuoe=0;
			 for (j=locat;j<=i;j++)
			   if (a[j]=='{')
			     kuoe++;
			 if (kuoe)
			   {
			     lx=x;
				 ly=y;
				 x=xe;
				 y=ye;
			     i=draw_main(i);
  				 xe=x;
				 ye=y;
				 x=lx;
				 y=ly;
				 if (ye<=(y-n))
			  {
			    vesa_line(xe,ye,xe,y-n,4);
				draw_arrow(xe,y-n,x,y-n);
			  }
			  if (ye>(y-n))
			  {
				vesa_line(xe,ye,xe-1.05*m,ye,4);
				vesa_line(xe-1.05*m,ye,xe-1.05*m,y-n,4);
				draw_arrow(xe-1.05*m,y-n,x,y-n);
			  }
			   }
			 if (!kuoe)
			   {
			    while(!((a[i]==';')&&(a[i+1]==10)))
	               i++;
                 i=draw_sent(locat,i,&xe,&ye);
			   }
			  if (ye<=(y-n))
			  {
			    vesa_line(xe,ye,xe,y-n,4);
				draw_arrow(xe,y-n,x,y-n);
			  }
			  if (ye>(y-n))
			  {
				vesa_line(xe,ye,xe-1.05*m,ye,4);
				vesa_line(xe-1.05*m,ye,xe-1.05*m,y-n,4);
				draw_arrow(xe-1.05*m,y-n,x,y-n);
			  }
		  }
	  }
	}
  if (!bbe)
    {
	  vesa_line(xe+m,ye+n,xe+((deep-level+1)*1.5*m),ye+n,4);
	  vesa_line(xe+((deep-level+1)*1.5*m),ye+n,x+((deep-level+1)*1.5*m),y-n,4);
	  draw_arrow(x+((deep-level+1)*1.5*m),y-n,x,y-n);
	  level--;
      if (!level)
        deep=0;
	  return locat;
	}
	if (bbcon)
	{
	  vesa_line(xcon[xcon[0]]-m,ycon[ycon[0]]+n,xcon[xcon[0]]-3*m,ycon[ycon[0]]+n,4);
	  vesa_line(xcon[xcon[0]]-3*m,ycon[ycon[0]]+n,x-3*m,y-n,4);
	  draw_arrow(x-3*m,y-n,x,y-n);
	  bbcon=0;
	  xcon[0]--;
	  ycon[0]--;
	}
  level--;
  if (!level)
    deep=0;
  return i;

}
int  draw_while(int locat)
{
  int i,j,k,kuo,fen,bb,head,tail,xe,ye,kuoe,lx,ly;
  char s[50];
  kuo=0;
  fen=0;
  level++;
  deep++;
  i=locat;
  i=i+6;
  while ((a[i]!=10)&&(a[i]!=32))
    i++;
  i--;
  xe=x;
  ye=y;
  for (j=locat+5;j<=i;j++)
    s[j-locat-5]=a[j];
    s[j-locat-5]='\0';
  vesa_puts(x-(j-locat-5)*4,y+n-8,4,s);
  draw_diam(x,y);
  y=y+4*n;
  i=i+2;
  locat=i;
  while (a[i]!=10)
    i++;
  i--;
  for (j=locat;j<=i;j++)
    {
	if (a[j]=='{')
	  kuo++;
	if (a[j]==';')
	  fen++;
	}
  if (!kuo)
  {
   while(!((a[i]==';')&&(a[i+1]==10)))
	  i++;
    i=draw_sent(locat,i,&x,&y);
  }
  if (kuo)
    {
      i=draw_main(locat);
	}
	vesa_line(xe+m,ye+n,xe+((deep-level+1)*1.5*m),ye+n,4);
	vesa_line(xe+((deep-level+1)*1.5*m),ye+n,x+((deep-level+1)*1.5*m),y-n,4);
	draw_arrow(x+((deep-level+1)*1.5*m),y-n,x,y-n);
	if (bbcon)
	{
	  vesa_line(xcon[xcon[0]]-m,ycon[ycon[0]]+n,xcon[xcon[0]]-3*m,ycon[ycon[0]]+n,4);
	  vesa_line(xcon[xcon[0]]-3*m,ycon[ycon[0]]+n,x-3*m,y-n,4);
	  draw_arrow(x-3*m,y-n,x,y-n);
	  bbcon=0;
	  xcon[0]--;
	  ycon[0]--;
	}
	level--;
	if (!level)
    deep=0;
    return i;
}
int  draw_do(int locat)
{
  int i,j,k,kuo,fen,bb,head,tail,xe,ye,kuoe,lx,ly;
  char s[50];
  kuo=0;
  fen=0;
  level++;
  deep++;
  i=locat;
  while (a[i]!=10)
    i++;
  locat=++i;
  while (a[i]!=10)
    i++;
  i--;
  xe=x;
  ye=y;
  for (j=locat;j<=i;j++)
    {
	if (a[j]=='{')
	  kuo++;
	if (a[j]==';')
	  fen++;
	}
  if (!kuo)
  {
   while(!((a[i]==';')&&(a[i+1]==10)))
	  i++;
    i=draw_sent(locat,i,&x,&y);
  }
  if (kuo)
    {
      i=draw_main(locat);
	}
	while(a[i]!=10)
	  i++;
	locat=++i;
	while(a[i]!=10)
	  i++;
	i--;
	head=locat;
	tail=i;
	for (j=head;((j<=tail)&&(!cmps("while",a,j,j+4)));j++);
	i=j+6;
	locat=i;
    while ((a[i]!=10)&&(a[i]!=32))
      i++;
    i--;
	for (j=locat;j<=i;j++)
      s[j-locat]=a[j];
    s[j-locat]='\0';
    vesa_puts(x-(j-locat)*4,y+n-8,4,s);
    draw_diam(x,y);
	vesa_line(x+m,y+n,x+((deep-level+1)*1.5*m),y+n,4);
	vesa_line(x+((deep-level+1)*1.5*m),y+n,xe+((deep-level+1)*1.5*m),ye-n,4);
	draw_arrow(xe+((deep-level+1)*1.5*m),ye-n,xe,ye-n);
    y=y+4*n;
	if (bbcon)
	{
	  vesa_line(xcon[xcon[0]]-m,ycon[ycon[0]]+n,xcon[xcon[0]]-3*m,ycon[ycon[0]]+n,4);
	  vesa_line(xcon[xcon[0]]-3*m,ycon[ycon[0]]+n,x-3*m,y-n,4);
	  draw_arrow(x-3*m,y-n,x,y-n);
	  bbcon=0;
	  xcon[0]--;
	  ycon[0]--;
	}
	level--;
	if (!level)
    deep=0;
    return i;
}
int  draw_for(int locat)
{
  int i,j,k,kuo,fen,bb,head,tail,xe,ye,kuoe,lx,ly;
  char s[50];
  kuo=0;
  fen=0;
  level++;
  deep++;
  i=locat;
  i=i+4;
  while (a[i]!=10)
    i++;
  xe=x;
  ye=y;
  for (j=locat+3;j<=i;j++)
    s[j-locat-3]=a[j];
    s[j-locat-3]='\0';
  vesa_puts(x-(j-locat-3)*4,y+n-8,4,s);
  draw_diam(x,y);
  y=y+4*n;
  i=i+2;
  locat=i;
  while (a[i]!=10)
    i++;
  i--;
  for (j=locat;j<=i;j++)
    {
	if (a[j]=='{')
	  kuo++;
	if (a[j]==';')
	  fen++;
	}
  if (!kuo)
  {
   while(!((a[i]==';')&&(a[i+1]==10)))
	  i++;
    i=draw_sent(locat,i,&x,&y);
  }
  if (kuo)
    {
      i=draw_main(locat);
	}
	vesa_line(xe+m,ye+n,xe+((deep-level+1)*1.5*m),ye+n,4);
	vesa_line(xe+((deep-level+1)*1.5*m),ye+n,x+((deep-level+1)*1.5*m),y-n,4);
	draw_arrow(x+((deep-level+1)*1.5*m),y-n,x,y-n);
	if (bbcon)
	{
	  vesa_line(xcon[xcon[0]]-m,ycon[ycon[0]]+n,xcon[xcon[0]]-3*m,ycon[ycon[0]]+n,4);
	  vesa_line(xcon[xcon[0]]-3*m,ycon[ycon[0]]+n,x-3*m,y-n,4);
	  draw_arrow(x-3*m,y-n,x,y-n);
	  bbcon=0;
	  xcon[0]--;
	  ycon[0]--;
	}
	level--;
	if (!level)
    deep=0;
    return i;
}

int  draw_switch(int locat)
{
  int i,j,k,kuo,fen,bb,head,tail,xe,ye,kuoe,lx,ly;
  char s[50];
  i=draw_main(locat);
  j=xcon[0];
  while(xcon[0])
  {
	  vesa_line(xcon[xcon[0]]-m,ycon[ycon[0]]+n,xcon[xcon[0]]-(j-xcon[0]+4)*m,ycon[ycon[0]]+n,4);
	  vesa_line(xcon[xcon[0]]-(j-xcon[0]+4)*m,ycon[ycon[0]]+n,x-(j-xcon[0]+4)*m,y-n,4);
	  draw_arrow(x-(j-xcon[0]+4)*m,y-n,x,y-n);
	  bbcon=0;
	  xcon[0]--;
	  ycon[0]--;
  }
  return i;
}
int  draw_sent(int begin,int end,int *x1,int *y1)
{
  int i,j,k,head,tail,lev,deep,bb,beginx,beginy;
  char s[50];
  int loca[100],cate[100];
  if (!((&x==x1)&&(&y==y1)))
  {beginx=x;
  beginy=y;
  x=*x1;
  y=*y1;}
  lev=0;
  i=begin;
  while (i<=end)
  {
    if (!nots(i))
	 {
		  bb=0;
		  head=i;
		  tail=i;
		  while ((a[tail]!=10)&&(tail<=end))
		    tail++;
		  tail--;
		  for (j=head;j<=tail;j++)
		    if (a[j]=='{')
			  {
			    bb=1;
			    lev++;
			  }
		  for (j=head;j<=tail;j++)
			if (a[j]=='}')
			  {
			    bb=2;
			    lev--;
			    i=j;
			  }
		  for (j=head;j<=tail;j++)
		    {
			if ((cmps("if",a,j,j+1))&&(nots(j+2)))
			  {
			    i=draw_if(j);
				bb=3;
			  }
			}
		  for (j=head;j<=tail;j++)
		    {
			if ((cmps("while",a,j,j+4))&&(nots(j+5)))
			  {
			    xb=x;
				yb=y;
			    i=draw_while(j);
				bb=3;
			  }
			}
		  for (j=head;j<=tail;j++)
		    {
			if ((cmps("for",a,j,j+2))&&(nots(j+3)))
			  {
			    xb=x;
				yb=y;
			    i=draw_for(j);
				bb=3;
			  }
			}
		  for (j=head;j<=tail;j++)
		    {
			if ((cmps("do",a,j,j+1))&&(nots(j+2)))
			  {
			    i=draw_do(j);
				bb=3;
				xb=x;
				yb=y-4*n;
			  }
			}
		  for (j=head;j<=tail;j++)
		    {
			if ((cmps("switch",a,j,j+5))&&(nots(j+6)))
			  {
			    xb=x;
				yb=y;
			    i=draw_switch(j);
				bb=3;
			  }
			}
		  for (j=head;j<=tail;j++)
		    {
			if ((cmps("break",a,j,j+4))&&(nots(j+5)))
			  {
			    xcon[0]++;
				ycon[0]++;
			    xcon[xcon[0]]=x;
				ycon[ycon[0]]=y;
				bbcon=1;
			  }
			}
		  for (j=head;j<=tail;j++)
		    {
			if ((cmps("continue",a,j,j+7))&&(nots(j+8)))
			  {
                bb=3;
				draw_rect(x,y);
		        while (nots(head))
		          head++;
		        for (j=head;j<=tail;j++) s[j-head]=a[j];
		        s[j-head]='\0';
		        vesa_puts(x-(j-head)*4,y+n-8,4,s);
				vesa_line(x-m,y+n,x-3*m,y+n,4);
				vesa_line(x-3*m,y+n,x-3*m,yb+n,4);
				draw_arrow(x-3*m,yb+n,xb-m,yb+n);
		        y=y+4*n;
		     	i=tail+1;
			  }
			}
		  if (!bb)
		    {
			      draw_rect(x,y);
		      while (nots(head))
			head++;
		      for (j=head;j<=tail;j++) s[j-head]=a[j];
		  s[j-head]='\0';
		  vesa_puts(x-(j-head)*4,y+n-8,4,s);
		     y=y+4*n;
			 i=tail+1;

			}
		}
		else i++;
    }
	if (!((&x==x1)&&(&y==y1)))
	{
	*x1=x;
	*y1=y;
	x=beginx;
	y=beginy;
	return i;
	}
	return i;  
  }

int  draw_main(int loc)
{
  int i,j,k,head,tail,lev,deep,begin,end,bb;
  char s[50];
  int loca[100],cate[100];
  lev=0;
  i=loc;
  while (lev!=1)
    {
	  if (a[i]!='{')
	    i++;
		else lev++;
	}
  begin=i;
  i++;
  while (lev!=0)
  {
    if (a[i]=='{') lev++;
	if (a[i]=='}') lev--;
	i++;
  }
  end=i;
  i=begin;
  while (i<=end)
  {
    if (!nots(i))
	 {
		  bb=0;
		  head=i;
		  tail=i;
		  while ((a[tail]!=10)&&(tail<=end))
		    tail++;
		  tail--;
		  for (j=head;j<=tail;j++)
		    if (a[j]=='{')
			  {
			    bb=1;
			    lev++;
			  }
		  for (j=head;j<=tail;j++)
			if (a[j]=='}')
			  {
			    bb=2;
			    lev--;
			    i=j;
			  }
		  for (j=head;j<=tail;j++)
		    {
			if ((cmps("if",a,j,j+1))&&(nots(j+2)))
			  {
			    i=draw_if(j);
				bb=3;
			  }
			}
		  for (j=head;j<=tail;j++)
		    {
			if ((cmps("while",a,j,j+4))&&(nots(j+5)))
			  {
			    xb=x;
				yb=y;
			    i=draw_while(j);
				bb=3;
			  }
			}
		  for (j=head;j<=tail;j++)
		    {
			if ((cmps("for",a,j,j+2))&&(nots(j+3)))
			  {
			    xb=x;
				yb=y;
			    i=draw_for(j);
				bb=3;
			  }
			}
		  for (j=head;j<=tail;j++)
		    {
			if ((cmps("do",a,j,j+1))&&(nots(j+2)))
			  {
			    i=draw_do(j);
				bb=3;
				xb=x;
				yb=y-4*n;
			  }
			}
		  for (j=head;j<=tail;j++)
		    {
			if ((cmps("switch",a,j,j+5))&&(nots(j+6)))
			  {
			    xb=x;
				yb=y;
			    i=draw_switch(j);
				bb=3;
			  }
			}
		  for (j=head;j<=tail;j++)
		    {
			if ((cmps("break",a,j,j+4))&&(nots(j+5)))
			  {
			    xcon[0]++;
				ycon[0]++;
			    xcon[xcon[0]]=x;
				ycon[ycon[0]]=y;
				bbcon=1;
			  }
			}
		  for (j=head;j<=tail;j++)
		    {
			if ((cmps("continue",a,j,j+7))&&(nots(j+8)))
			  {
                bb=3;
				draw_rect(x,y);
		        while (nots(head))
		          head++;
		        for (j=head;j<=tail;j++) s[j-head]=a[j];
		        s[j-head]='\0';
		        vesa_puts(x-(j-head)*4,y+n-8,4,s);
				vesa_line(x-m,y+n,x-3*m,y+n,4);
				vesa_line(x-3*m,y+n,x-3*m,yb+n,4);
				draw_arrow(x-3*m,yb+n,xb-m,yb+n);
		        y=y+4*n;
		     	i=tail+1;
			  }
			}
		  if (!bb)
		    {
			      draw_rect(x,y);
		      while (nots(head))
			head++;
		      for (j=head;j<=tail;j++) s[j-head]=a[j];
		  s[j-head]='\0';
		  vesa_puts(x-(j-head)*4,y+n-8,4,s);
		     y=y+4*n;
			 i=tail+1;

			}
		}
		else i++;
    }
	return i;
  }
void interrupt int_8h(void)
{
   if(ticks != 0)
      --ticks;
   outportb(0x20, 0x20); /* send EOI signal */
}

void set_clock_frequency(int n)
{
   outportb(0x43, 0x36);
   outportb(0x40, (1193180/n) & 0xFF);
   outportb(0x40, (1193180/n) >> 8);
}

void reset_clock_frequency(void)
{
   outportb(0x43, 0x36);
   outportb(0x40, 0);
   outportb(0x40, 0);
}

void set_sound_frequency(int n)
{
   outportb(0x43, 0xB6);
   outportb(0x42, (1193180/n) & 0xFF);
   outportb(0x42, (1193180/n) >> 8);
}

void sound_delay(int n)
{
   outportb(0x61, inportb(0x61) | 3);  /* 开喇叭 */
   ticks = n;
   while(ticks != 0)
      ;
   outportb(0x61, inportb(0x61) & ~3); /* 关喇叭 */
}

void music()
{
   int i;
   old_8h = getvect(8);
   setvect(8, int_8h);
   set_clock_frequency(1000); /* 设置时钟振荡频率为1000次/秒 */
   for(i=0; i<sizeof(song)/sizeof(song[0]); i++)
   {
      set_sound_frequency(song[i].freq);
      sound_delay(song[i].delay);
   }
   reset_clock_frequency();
   setvect(8, old_8h);
}

