#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ui.h"
#include "settings.h"
#include "display.h"
#include "handbook.h"
static unsigned checks;
static unsigned char saved[76800];
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"LANGUAGE MENU FAIL %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static void draw(App *a){App old=*a;app_draw(a);CHECK(!memcmp(&old,a,sizeof(old)));CHECK(!display_clipped_pixels&&!display_substitutions);}
static void key(App *a,int k){app_key(a,k);draw(a);}
static void record(unsigned char *p,unsigned seq,Language l){unsigned i;p[0]=(unsigned char)seq;p[1]=p[2]=p[3]=0;p[4]=(unsigned char)l;p[5]=0xA7;for(i=0;i<5;i++)p[5]^=p[i];p[6]=0xC2;p[7]=0x1D;}
static void write_old(const char *path,const unsigned char *d){FILE *f=fopen(path,"wb");CHECK(f!=0);CHECK(fwrite(d,1,24,f)==24);CHECK(!fclose(f));}
static void unchanged(const char *path,const unsigned char *d){unsigned char got[24];FILE *f=fopen(path,"rb");CHECK(f!=0);CHECK(fread(got,1,24,f)==24&&fgetc(f)==EOF);CHECK(!fclose(f));CHECK(!memcmp(d,got,24));}
static void migration(void){
 char path[180];unsigned char data[24];Language l;App a;
 snprintf(path,sizeof(path),"build/v102-settings-%lu-%lu.bin",(unsigned long)time(0),(unsigned long)clock());settings_bind(path);CHECK(settings_load()==LANG_EN);
 for(l=LANG_EN;l<=LANG_TR_ASCII;l++){
  memset(data,0,24);memcpy(data,"C21LANG1",8);record(data+8,1,l);write_old(path,data);
  CHECK(settings_load()==(l==LANG_TR?LANG_TR_ASCII:l));unchanged(path,data);
  app_init(&a);loc_set(settings_load());CHECK(loc_get()==(l==LANG_TR?LANG_TR_ASCII:l));
  CHECK(settings_save(loc_get()));CHECK(settings_load()==(l==LANG_TR?LANG_TR_ASCII:l));
 }
 /* A torn newest slot falls back to the valid legacy Turkish slot, then normalizes. */
 memset(data,0,24);memcpy(data,"C21LANG1",8);record(data+8,1,LANG_TR);record(data+16,2,LANG_EN);data[23]=0;write_old(path,data);CHECK(settings_load()==LANG_TR_ASCII);unchanged(path,data);
 data[15]=0;write_old(path,data);CHECK(settings_load()==LANG_EN);unchanged(path,data);
 memset(data,'U',24);write_old(path,data);CHECK(settings_load()==LANG_EN&&!settings_save(LANG_TR_ASCII));unchanged(path,data);
 settings_bind(0);CHECK(settings_load()==LANG_EN);app_init(&a);CHECK(loc_get()==LANG_EN);
}
static void labels(App *a){unsigned row,x,y;int equal=1,empty=1;draw(a);memcpy(saved,framebuffer,sizeof(saved));
 clear_screen();
 for(row=0;row<2;row++){int yy=row?87:65;int ink=a->language_cursor!=row;if(!ink)rect(7,yy-3,306,15,1);text_fit(13,yy,294,row?"2. Turkce ASCII":"1. English",ink);}
 for(y=62;y<99;y++)for(x=8;x<312;x++)if(saved[y*320+x]!=framebuffer[y*320+x])equal=0;
 for(y=100;y<127;y++)for(x=8;x<312;x++)if(saved[y*320+x])empty=0;
 CHECK(equal&&empty);draw(a);
}
static void shot(App *a,const char *name){char path[160];FILE *f;unsigned i;draw(a);snprintf(path,sizeof(path),"build/v102-%s.pgm",name);f=fopen(path,"wb");CHECK(f!=0);fprintf(f,"P5\n320 240\n255\n");for(i=0;i<76800;i++)fputc(framebuffer[i]?0:255,f);CHECK(!fclose(f));}
static void menu(void){App a,b;unsigned i;app_init(&a);key(&a,K_ENTER);key(&a,K_SETTINGS);key(&a,K_ENTER);CHECK(a.screen==S_LANGUAGE&&a.language_cursor==0);labels(&a);shot(&a,"language-en");
 b=a;key(&a,K_DIGIT0+3);CHECK(!memcmp(&a,&b,sizeof(a))&&loc_get()==LANG_EN);
 for(i=0;i<5;i++){static const int keys[]={K_UP,K_DOWN,K_LEFT,K_RIGHT,K_TAB};key(&a,keys[i]);CHECK(a.language_cursor==(i%2?0:1));}
 key(&a,K_ENTER);CHECK(loc_get()==LANG_TR_ASCII);labels(&a);shot(&a,"language-ascii");
 key(&a,K_DIGIT0+1);CHECK(loc_get()==LANG_EN&&a.language_cursor==0);key(&a,K_DIGIT0+2);CHECK(loc_get()==LANG_TR_ASCII&&a.language_cursor==1);
 key(&a,K_ESC);key(&a,K_ESC);CHECK(a.screen==S_MENU);shot(&a,"menu-ascii");
}
static void coverage(void){unsigned i;loc_set(LANG_TR_ASCII);
 for(i=0;i<loc_count();i++){const char *p=loc_value(i);CHECK(p&&*p);}
 for(i=0;i<CALC_COUNT;i++){
  const HandbookEntry *h=&handbook[i];CHECK(loc_has(calculations[i].name));CHECK(loc_has(h->purpose));CHECK(loc_has(h->measurements));CHECK(loc_has(h->laboratory));CHECK(loc_has(h->limits));CHECK(loc_has(h->example));CHECK(loc_has(h->mistakes));
 }
 /* The Unicode resources remain accessible internally, with ASCII applied only on display. */
 loc_set(LANG_TR);CHECK(display_codepoint(0x15E)==0x15E);CHECK(strstr(tr("Inductance"),"End")!=0);
 loc_set(LANG_TR_ASCII);CHECK(display_codepoint(0x15E)=='S');CHECK(display_codepoint(0x3A9)==0x3A9);
}
int main(void){migration();menu();coverage();printf("PASS: %u V1.0.2 language menu/migration checks (2 visible choices; 138 Handbook entries)\n",checks);return 0;}
