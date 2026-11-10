#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "ui.h"
#include "display.h"
#include "settings.h"
#include "handbook.h"
#include "locale_keys.h"
static unsigned checks,frames;
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL localization line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static void render(App *a){App old=*a;app_draw(a);CHECK(!memcmp(&old,a,sizeof(old)));if(display_clipped_pixels)fprintf(stderr,"clip screen=%d calc=%d page=%u pixels=%u\n",a->screen,a->calc,a->info_page,display_clipped_pixels);CHECK(display_clipped_pixels==0);frames++;}
static void key(App *a,int k){app_key(a,k);render(a);}
static void shot(App *a,const char *name){FILE *f;unsigned i;char p[160];render(a);snprintf(p,sizeof(p),"build/phase3d-%s.pgm",name);f=fopen(p,"wb");CHECK(f!=0);fprintf(f,"P5\n320 240\n255\n");for(i=0;i<76800;i++)fputc(framebuffer[i]?0:255,f);CHECK(!fclose(f));}
static void glyphs(const char *s){const unsigned char *p=(const unsigned char *)s;while(*p){unsigned c=*p++,n=0;if(c>=0xc2&&c<0xe0){c&=31;n=1;}else if(c>=0xe0){c&=15;n=2;}while(n--)c=(c<<6)|(*p++&63);if(c!='\n')CHECK(glyph_supported(c));}}
static void change(App *a,Language lang){App before=*a;if(lang==LANG_TR)lang=LANG_TR_ASCII;key(a,K_SETTINGS);CHECK(a->screen==S_SETTINGS);key(a,K_ENTER);CHECK(a->screen==S_LANGUAGE);key(a,K_DIGIT0+(lang==LANG_EN?1:2));CHECK(loc_get()==lang);key(a,K_ESC);key(a,K_ESC);CHECK(a->screen==before.screen);before.settings_return=a->settings_return;before.language_cursor=a->language_cursor;before.language_saved=a->language_saved;CHECK(!memcmp(a,&before,sizeof(before)));}
static void catalog(void){unsigned i;const char *s;CHECK(loc_count()==sizeof(locale_keys)/sizeof(locale_keys[0]));loc_set(LANG_TR);for(i=0;i<sizeof(locale_keys)/sizeof(locale_keys[0]);i++){CHECK(loc_has(locale_keys[i]));s=tr(locale_keys[i]);CHECK(s&&*s);CHECK(strcmp(s,locale_keys[i])!=0);glyphs(s);}
 CHECK(!strcmp(tr("Inductance"),"Endüktans"));CHECK(!strcmp(tr("Capacitance"),"Kapasitans"));CHECK(!strcmp(tr("Noise Figure"),"Gürültü faktörü"));CHECK(!strcmp(tr("Reflection Coefficient"),"Yansıma katsayısı"));CHECK(!strcmp(tr("Voltage Drop"),"Gerilim düşümü"));
 for(i=0;i<LEGACY_CALC_COUNT;i++){const HandbookEntry *h=&handbook[i];CHECK(loc_has(calculations[i].name));CHECK(loc_has(calculations[i].definitions));CHECK(loc_has(calculations[i].usage));CHECK(loc_has(h->purpose)&&loc_has(h->measurements)&&loc_has(h->laboratory)&&loc_has(h->limits)&&loc_has(h->example)&&loc_has(h->mistakes));}
 loc_set(LANG_EN);for(i=0;i<sizeof(locale_keys)/sizeof(locale_keys[0]);i++)CHECK(tr(locale_keys[i])==locale_keys[i]);CHECK(!strcmp(tr("π μ Ω"),"π μ Ω"));loc_set((Language)9);CHECK(loc_get()==LANG_EN);
}
static void persistence(void){char path[180],other[210],uipath[210],longpath[600];FILE *f;unsigned i;unsigned char data[24];
 snprintf(path,sizeof(path),"build/phase3d-settings-%lu-%lu.bin",(unsigned long)time(0),(unsigned long)clock());snprintf(other,sizeof(other),"%s-unrelated",path);
 settings_bind(0);CHECK(settings_load()==LANG_EN&&!settings_save(LANG_TR));CHECK(!settings_bind_executable("no-directory.tns"));
 memset(longpath,'a',sizeof(longpath)-1);longpath[sizeof(longpath)-1]=0;settings_bind(longpath);CHECK(!settings_save(LANG_TR));
 settings_bind(path);CHECK(settings_load()==LANG_EN);CHECK(settings_save(LANG_TR));CHECK(settings_load()==LANG_TR_ASCII);CHECK(settings_save(LANG_EN));CHECK(settings_load()==LANG_EN);
 f=fopen(path,"r+b");CHECK(f!=0);CHECK(!fseek(f,23,SEEK_SET));CHECK(fputc(0,f)!=EOF);CHECK(!fclose(f));CHECK(settings_load()==LANG_TR_ASCII); /* Torn newest slot recovers old. */
 f=fopen(path,"r+b");CHECK(f!=0);CHECK(!fseek(f,15,SEEK_SET));CHECK(fputc(0,f)!=EOF);CHECK(!fclose(f));CHECK(settings_load()==LANG_EN);CHECK(settings_save(LANG_TR));CHECK(settings_load()==LANG_TR_ASCII);
 for(i=0;i<300;i++){CHECK(settings_save(i%2?LANG_TR:LANG_EN));CHECK(settings_load()==(i%2?LANG_TR_ASCII:LANG_EN));}
 f=fopen(path,"rb");CHECK(f!=0);CHECK(fread(data,1,24,f)==24&&fgetc(f)==EOF);CHECK(!fclose(f));
 f=fopen(other,"wb");CHECK(f!=0);CHECK(fwrite("UNRELATED",1,9,f)==9);CHECK(!fclose(f));settings_bind(other);CHECK(settings_load()==LANG_EN);CHECK(!settings_save(LANG_TR));f=fopen(other,"rb");CHECK(f!=0);CHECK(fread(data,1,24,f)==9&&!memcmp(data,"UNRELATED",9));CHECK(!fclose(f));
 /* Exact-size unrelated data must also remain byte-identical. */
 settings_bind(other);f=fopen(other,"wb");CHECK(f!=0);memset(data,'U',24);CHECK(fwrite(data,1,24,f)==24);CHECK(!fclose(f));CHECK(!settings_save(LANG_TR));f=fopen(other,"rb");CHECK(f!=0);CHECK(fread(data,1,24,f)==24);CHECK(!fclose(f));for(i=0;i<24;i++)CHECK(data[i]=='U');
 CHECK(settings_bind_executable("build/program.tns")); /* Path construction only, no I/O. */
 snprintf(uipath,sizeof(uipath),"%s-ui",path);settings_bind(uipath);
 {App a;app_init(&a);key(&a,K_ENTER);change(&a,LANG_TR);CHECK(a.language_saved==1&&settings_load()==LANG_TR_ASCII);app_init(&a);CHECK(loc_get()==LANG_EN);loc_set(settings_load());CHECK(loc_get()==LANG_TR_ASCII);}
 settings_bind("build/nonexistent-phase3d-directory/settings.cfg");CHECK(settings_load()==LANG_EN&&!settings_save(LANG_TR));settings_bind(0);
}
static void all_screens(void){unsigned lang,id,p,i;App a;
 for(lang=0;lang<2;lang++)for(id=0;id<LEGACY_CALC_COUNT;id++){
  app_init(&a);loc_set((Language)lang);a.screen=S_FORM;a.calc=(CalcId)id;
  for(i=0;i<calculations[id].count;i++)strcpy(a.values[i],"-1.23456789012345678901234E-123");
  for(i=0;i<calculations[id].count+3;i++){a.row=i;render(&a);}a.row=0;change(&a,lang==LANG_EN?LANG_TR:LANG_EN);change(&a,(Language)lang);
  key(&a,K_ENTER);CHECK(a.screen==S_EDIT);key(&a,K_SETTINGS);key(&a,K_ENTER);key(&a,K_ESC);key(&a,K_ESC);CHECK(a.screen==S_EDIT);key(&a,K_ESC);
  key(&a,K_RIGHT);CHECK(a.screen==S_UNITS);render(&a);key(&a,K_ESC);key(&a,K_MODE);CHECK(a.screen==S_FORMULA);for(i=0;i<calculations[id].count;i++)key(&a,K_TAB);
  a.result=1.2345678901e100;a.result_valid=1;a.screen=S_RESULT;render(&a);key(&a,K_TARGET);if(a.screen==S_TARGET)key(&a,K_ESC);
  key(&a,K_INFO);for(p=0;p<HANDBOOK_PAGES;p++){unsigned j,n;key(&a,K_DIGIT0+1+(int)p);n=app_info_lines(&a);for(j=0;j<n+2;j++)key(&a,K_DOWN);CHECK(a.info_scroll==(n>11?n-11:0));for(j=0;j<n+2;j++)key(&a,K_UP);CHECK(!a.info_scroll);}
  key(&a,K_ESC);a.screen=S_FORMULA;a.error=INVALID;a.input_error=NUM_DOMAIN;a.error_field=0;render(&a);a.screen=S_SYMBOLS;render(&a);
 }
}
static void flow(void){App a;unsigned i;double result;const char *langname;
 app_init(&a);CHECK(loc_get()==LANG_EN);key(&a,K_ENTER);shot(&a,"menu-en");change(&a,LANG_TR);shot(&a,"menu-tr");
 for(i=1;i<=10;i++){key(&a,K_DIGIT0+(i==10?0:(int)i));CHECK(a.screen==S_MENU&&a.menu!=0);key(&a,K_ESC);CHECK(a.menu==0);}
 key(&a,K_DIGIT0+3);key(&a,K_DIGIT0+1);key(&a,K_DIGIT0+2);CHECK(a.calc==LC_CAP);
 key(&a,K_RIGHT);key(&a,K_DIGIT0+3);key(&a,K_ENTER);key(&a,K_DIGIT0+8);key(&a,K_DIGIT0+6);key(&a,K_DIGIT0+8);key(&a,K_ENTER);key(&a,K_DOWN);key(&a,K_RIGHT);key(&a,K_DIGIT0+4);key(&a,K_ENTER);key(&a,K_DIGIT0+1);key(&a,K_DIGIT0);key(&a,K_ENTER);key(&a,K_OUTPUT_UNITS);key(&a,K_DIGIT0+4);key(&a,K_CALCULATE);CHECK(a.result_valid);result=a.result;CHECK(result>3.362024e-12&&result<3.362025e-12);
 key(&a,K_MODE);CHECK(a.screen==S_FORMULA);shot(&a,"calculator-tr");change(&a,LANG_EN);CHECK(a.result==result&&a.result_valid);shot(&a,"calculator-en");
 for(i=0;i<200;i++){change(&a,i%2?LANG_EN:LANG_TR);}CHECK(a.result==result&&a.result_valid); /* No heap in switching path. */
 key(&a,K_INFO);key(&a,K_DIGIT0+4);for(i=0;i<40;i++)key(&a,K_DOWN);
 key(&a,K_SETTINGS);key(&a,K_ENTER);key(&a,K_DIGIT0+1);key(&a,K_ESC);key(&a,K_ESC);{unsigned n=app_info_lines(&a);CHECK(a.info_scroll<=(n>11?n-11:0));}key(&a,K_DIGIT0+2);change(&a,LANG_TR);shot(&a,"handbook-tr");change(&a,LANG_EN);shot(&a,"handbook-en");key(&a,K_ESC);CHECK(a.result==result);
 key(&a,K_ENTER);CHECK(a.screen==S_EDIT);key(&a,K_CLEAR);key(&a,K_DIGIT0+2);key(&a,K_DIGIT0);change(&a,LANG_TR);CHECK(!strcmp(a.edit,"20"));key(&a,K_ENTER);key(&a,K_CALCULATE);CHECK(a.result_valid&&a.result>1.681012e-12&&a.result<1.681013e-12);shot(&a,"recalculated-tr");
 key(&a,K_UNITS);CHECK(a.screen==S_UNITS);change(&a,LANG_EN);key(&a,K_ESC);key(&a,K_TARGET);CHECK(a.screen==S_TARGET);change(&a,LANG_TR);key(&a,K_ESC);
 key(&a,K_SETTINGS);key(&a,K_ENTER);shot(&a,"language-settings-tr");key(&a,K_DIGIT0+1);shot(&a,"language-settings-en");key(&a,K_ESC);key(&a,K_ESC);
 for(i=0;i<2;i++){loc_set((Language)i);a.screen=S_SPLASH;langname=i?"splash-tr":"splash-en";shot(&a,langname);}
 clear_screen();text(10,20,"Ç Ğ İ Ö Ş Ü",1);text(10,38,"ç ğ ı ö ş ü",1);glyphs("ÇĞİÖŞÜçğıöşü");{FILE *f=fopen("build/phase3d-turkish-glyphs.pgm","wb");CHECK(f!=0);fprintf(f,"P5\n320 240\n255\n");for(i=0;i<76800;i++)fputc(framebuffer[i]?0:255,f);CHECK(!fclose(f));}
}
int main(void){catalog();persistence();all_screens();flow();printf("PASS: %u Phase 3D localization checks (%u frames, %u translations, 15 bilingual handbook entries)\n",checks,frames,loc_count());return 0;}
