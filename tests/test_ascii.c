#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ui.h"
#include "display.h"
#include "math_layout.h"
#include "settings.h"
#include "handbook.h"
static unsigned checks,frames;
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL ASCII line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static void render(App *a){App b=*a;app_draw(a);CHECK(!memcmp(&b,a,sizeof(b)));if(display_clipped_pixels)fprintf(stderr,"clip screen=%d calc=%d page=%u row=%u count=%u\n",a->screen,a->calc,a->info_page,a->row,display_clipped_pixels);CHECK(!display_clipped_pixels);CHECK(!display_substitutions);frames++;}
static void key(App *a,int k){app_key(a,k);render(a);}
static void shot(App *a,const char *name){char path[160];FILE *f;unsigned i;render(a);snprintf(path,sizeof(path),"build/ascii-%s.pgm",name);f=fopen(path,"wb");CHECK(f!=0);fprintf(f,"P5\n320 240\n255\n");for(i=0;i<76800;i++)fputc(framebuffer[i]?0:255,f);CHECK(!fclose(f));}
static void choose(App *a,Language l){App b=*a;if(l==LANG_TR)l=LANG_TR_ASCII;key(a,K_SETTINGS);key(a,K_ENTER);key(a,K_DIGIT0+(l==LANG_EN?1:2));CHECK(loc_get()==l);key(a,K_ESC);key(a,K_ESC);CHECK(a->screen==b.screen);b.settings_return=a->settings_return;b.language_cursor=a->language_cursor;b.language_saved=a->language_saved;CHECK(!memcmp(a,&b,sizeof(b)));}
static void mapping(void){
 static const unsigned codes[]={0xC7,0x11E,0x130,0xD6,0x15E,0xDC,0xE7,0x11F,0x131,0xF6,0x15F,0xFC};
 static const char expected[]="CGIOSUcgiosu";static unsigned char native[76800];unsigned i,id;const char *p;
 loc_set(LANG_TR);for(i=0;i<12;i++)CHECK(display_codepoint(codes[i])==codes[i]);p=tr("Inductance");CHECK(!strcmp(p,"Endüktans"));
 loc_set(LANG_TR_ASCII);CHECK(tr("Inductance")==p&&!strcmp(p,"Endüktans"));for(i=0;i<12;i++)CHECK(display_codepoint(codes[i])==(unsigned)expected[i]);
 for(i=0;i<128;i++){CHECK(display_codepoint(i)==i);}for(i=0x370;i<=0x3ff;i++){CHECK(display_codepoint(i)==i);}for(i=0x2070;i<=0x219f;i++){CHECK(display_codepoint(i)==i);}
 clear_screen();text(12,20,"ÇĞİÖŞÜçğıöşü Ω μ π √ ± × ÷ ° ² ³ ⁻ ₀ ₁ ← →",1);memcpy(native,framebuffer,sizeof(native));CHECK(!display_substitutions);
 loc_set(LANG_EN);clear_screen();text(12,20,"CGIOSUcgiosu Ω μ π √ ± × ÷ ° ² ³ ⁻ ₀ ₁ ← →",1);CHECK(!memcmp(native,framebuffer,sizeof(native)));
 for(id=0;id<LEGACY_CALC_COUNT;id++){
  MathInteraction f;loc_set(LANG_TR);clear_screen();CHECK(formula_interactive((CalcId)id,calc_input_id((CalcId)id,0),5,68,310,58,&f));memcpy(native,framebuffer,sizeof(native));
  loc_set(LANG_TR_ASCII);clear_screen();CHECK(formula_interactive((CalcId)id,calc_input_id((CalcId)id,0),5,68,310,58,&f));CHECK(!memcmp(native,framebuffer,sizeof(native)));CHECK(!display_substitutions);
 }
}
static void layouts(void){App a;unsigned l,id,i,p;for(l=0;l<2;l++){
 for(i=1;i<=10;i++){app_init(&a);key(&a,K_ENTER);choose(&a,l?LANG_TR_ASCII:LANG_TR);key(&a,K_DIGIT0+(i==10?0:(int)i));CHECK(a.menu!=0);key(&a,K_ENTER);render(&a);}
 for(id=0;id<LEGACY_CALC_COUNT;id++){
  app_init(&a);loc_set(l?LANG_TR_ASCII:LANG_TR);a.calc=(CalcId)id;a.screen=S_FORM;for(i=0;i<calculations[id].count;i++)strcpy(a.values[i],"-1.23456789012345678901234E-123");
  for(i=0;i<calculations[id].count+3;i++){a.row=i;render(&a);}a.row=0;key(&a,K_ENTER);CHECK(a.screen==S_EDIT);choose(&a,l?LANG_TR:LANG_TR_ASCII);choose(&a,l?LANG_TR_ASCII:LANG_TR);key(&a,K_ESC);key(&a,K_RIGHT);render(&a);key(&a,K_ESC);key(&a,K_MODE);render(&a);
  a.result=1.234e100;a.result_valid=1;a.screen=S_RESULT;render(&a);key(&a,K_INFO);
  for(p=0;p<8;p++){unsigned n;key(&a,K_DIGIT0+1+(int)p);n=app_info_lines(&a);for(i=0;i<n+2;i++)key(&a,K_DOWN);CHECK(a.info_scroll==(n>11?n-11:0));}
  key(&a,K_ESC);CHECK(a.screen==S_RESULT);a.screen=S_SYMBOLS;render(&a);
 }
 }}
static void flow(void){App a;double v;unsigned i;app_init(&a);settings_bind(0);key(&a,K_ENTER);choose(&a,LANG_TR);shot(&a,"menu-native");choose(&a,LANG_TR_ASCII);shot(&a,"menu-fallback");
 key(&a,K_DIGIT0+3);key(&a,K_DIGIT0+1);key(&a,K_DIGIT0+2);CHECK(a.calc==LC_CAP);key(&a,K_RIGHT);key(&a,K_DIGIT0+3);key(&a,K_ENTER);key(&a,K_DIGIT0+8);key(&a,K_DIGIT0+6);key(&a,K_DIGIT0+8);key(&a,K_ENTER);key(&a,K_DOWN);key(&a,K_RIGHT);key(&a,K_DIGIT0+4);key(&a,K_ENTER);key(&a,K_DIGIT0+1);key(&a,K_DIGIT0);key(&a,K_ENTER);key(&a,K_OUTPUT_UNITS);key(&a,K_DIGIT0+4);shot(&a,"list-fallback");key(&a,K_CALCULATE);CHECK(a.result_valid&&a.result>3.362024e-12&&a.result<3.362025e-12);v=a.result;key(&a,K_MODE);shot(&a,"calculator-fallback");choose(&a,LANG_TR);CHECK(a.result==v);shot(&a,"calculator-native");
 key(&a,K_INFO);key(&a,K_DIGIT0+3);shot(&a,"handbook-native");choose(&a,LANG_TR_ASCII);shot(&a,"handbook-fallback");key(&a,K_ESC);CHECK(a.result_valid&&a.result==v);key(&a,K_ENTER);CHECK(a.screen==S_EDIT);key(&a,K_CLEAR);key(&a,K_DIGIT0+2);key(&a,K_DIGIT0);choose(&a,LANG_EN);choose(&a,LANG_TR_ASCII);CHECK(!strcmp(a.edit,"20"));key(&a,K_ENTER);key(&a,K_CALCULATE);CHECK(a.result_valid&&a.result>1.681012e-12&&a.result<1.681013e-12);shot(&a,"recalculated-fallback");
 for(i=0;i<60;i++){choose(&a,(Language)(i%3));}key(&a,K_SETTINGS);key(&a,K_ENTER);CHECK(a.language_cursor==1);key(&a,K_UP);CHECK(a.language_cursor==0);key(&a,K_DOWN);CHECK(a.language_cursor==1);shot(&a,"settings-fallback");key(&a,K_DOWN);CHECK(a.language_cursor==0);key(&a,K_UP);CHECK(a.language_cursor==1);key(&a,K_ESC);key(&a,K_ESC);
}
static void persistence(void){char path[160];FILE *f;App a;snprintf(path,sizeof(path),"build/ascii-settings-%lu-%lu.bin",(unsigned long)time(0),(unsigned long)clock());settings_bind(path);CHECK(settings_load()==LANG_EN);CHECK(settings_save(LANG_TR));CHECK(settings_load()==LANG_TR_ASCII);CHECK(settings_save(LANG_TR_ASCII));CHECK(settings_load()==LANG_TR_ASCII);app_init(&a);loc_set(settings_load());CHECK(loc_get()==LANG_TR_ASCII&&display_codepoint(0x15e)=='S');CHECK(settings_save(LANG_EN));CHECK(settings_load()==LANG_EN);f=fopen(path,"rb");CHECK(f!=0);CHECK(!fseek(f,0,SEEK_END));CHECK(ftell(f)==24);CHECK(!fclose(f));settings_bind(0);}
int main(void){mapping();layouts();flow();persistence();printf("PASS: %u ASCII fallback checks (%u native-resolution frames)\n",checks,frames);return 0;}
