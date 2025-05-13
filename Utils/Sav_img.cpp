#include "..\..\include\mltk.h"
#include <malloc.h>
/*****************  ��������� PCX  **************/
struct pcxheader {
                 char       manuf;          /* �ᥣ�� =10 ��� Paintbrush     */
                 char       hard;           /* ���ଠ�� � ���ᨨ           */
                 char       encod;          /* ��㯯���� ����஢����(=1)     */
                 char       bitpx;          /* ��� �� ���                  */
                 int        x1;             /* ������� ���⨭��(������.)    */
                 int        y1;
                 int        x2;
                 int        y2;
                 int        hres;           /* ��ਧ.ࠧ�襭�� ��ᯫ��      */
                 int        vres;           /* ���⨪.ࠧ�襭�� ��ᯫ��     */
                 char       clrma[48];      /* ������                       */
                 char       vmode;          /* (����������)                */
                 char       nplanes;        /* ���-�� ���᪮�⥩ (���.2.5=0) */
                 int        bplin;          /* ���� �� ��ப�                */
                 int        palinfo;        /* ����.� ������ (1=�.,2=��)*/
                 int        shres;          /* ����襭�� ᪠����           */
                 int        svres;          /*                               */
                 char       xtra[54];       /* ���.���⮥ ���� (䨫���)     */
                 }      hh;

/*========================================================================*/
write_pcx()
{
int             i,j,k,n;
unsigned char   *img,*pc;
int width, height;
char   rgb[256*4];       /* ������                        */

width=xr-xl+1;
height=yd-yu+1;

hh.manuf=10;
hh.hard=5;
hh.encod=1;
hh.bitpx=8;
hh.x1=0;
hh.y1=0;
hh.x2=width-1;
hh.y2=height-1;
hh.hres=0;
hh.vres=0;
hh.vmode=0;
hh.nplanes=1;
hh.bplin=width;
hh.palinfo=2;
hh.shres=0;
hh.svres=0;
fwrite(&hh,128,1,out);

pc=(char *) halloc((long)width*2,1);
img=bufer+(long)yu*512+xl;
for(i=0;i<height;i++)
  {
  for(k=j=0;k<width;k++)
      {
      if(img[k]==img[k+1] && k+1<width)
        {
        for(n=1;n<63 && k+1<width && img[k]==img[k+1];n++,k++);
        pc[j++]=n+0xC0;
        pc[j++]=img[k];
        }
      else
        {
        if(img[k]>=(unsigned char)0xC0) pc[j++]=0xC1;
        pc[j++]=img[k];
        }
      }
  fwrite(pc,j,1,out);
  img+=512;
  }
for(i=0;i<256;i++)
  {
  rgb[(i+i+i)]=i;
  rgb[(i+i+i)+1]=i;
  rgb[(i+i+i)+2]=i;
  }
fwrite("\14",1,1,out);
fwrite(rgb,3,256,out);
hfree(pc);
return 1;
}
/*========================================================================*/
