#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "display.h"
#include "handbook.h"
#include "math_layout.h"
#include "c21_logo.h"
static unsigned checks,frames,scroll_pages;
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL mono UI line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static void render(App *a){App old=*a;unsigned i;app_draw(a);CHECK(!memcmp(a,&old,sizeof(old)));if(display_clipped_pixels)fprintf(stderr,"clip screen=%d calc=%d page=%u row=%u pixels=%u\n",a->screen,a->calc,a->info_page,a->row,display_clipped_pixels);CHECK(display_clipped_pixels==0);for(i=0;i<76800;i++)if(framebuffer[i]>1)break;CHECK(i==76800);frames++;}
static void shot(App *a,const char *name){char path[180];FILE *f;unsigned i;render(a);snprintf(path,sizeof(path),"build/phase3c-%s.pgm",name);f=fopen(path,"wb");CHECK(f!=0);fprintf(f,"P5\n320 240\n255\n");for(i=0;i<76800;i++)fputc(framebuffer[i]?0:255,f);CHECK(!fclose(f));}
static void key(App *a,int k){app_key(a,k);render(a);}
static void wrapping(void){
 clear_screen();CHECK(wrapped_text(12,77,30,2,0,"alpha beta gamma",1)==3);CHECK(!display_clipped_pixels);
 {unsigned char one[76800];memcpy(one,framebuffer,sizeof(one));clear_screen();CHECK(wrapped_text(12,77,30,2,1,"alpha beta gamma",1)==3);CHECK(memcmp(one,framebuffer,sizeof(one))!=0);}
 clear_screen();CHECK(wrapped_text(12,77,30,11,0,"A\nB\nC",1)==3);CHECK(wrapped_text(0,0,12,0,0,"ΩΩ ΩΩ",1)==2);
 CHECK(wrapped_text(0,0,12,0,0,"1234567",1)==4);CHECK(wrapped_text(0,0,0,0,0,"text",1)==0);
 clear_screen();text_fit(12,77,30,"long UTF-8 μΩ string",1);CHECK(!display_clipped_pixels);
}
static void menus(void){
 unsigned category;App a;
 for(category=1;category<=10;category++){
  app_init(&a);key(&a,K_ENTER);CHECK(a.screen==S_MENU&&a.menu==0);
  key(&a,K_DIGIT0+(category==10?0:(int)category));CHECK(a.menu!=0&&a.screen==S_MENU);
  {unsigned n;for(n=0;n<6;n++)key(&a,K_DOWN);for(n=0;n<6;n++)key(&a,K_UP);}
  key(&a,K_ESC);CHECK(a.menu==0);
 }
 app_init(&a);key(&a,K_ENTER);shot(&a,"main-menu");
 {unsigned i,j;for(i=0;i<24;i++)for(j=0;j<24;j++)CHECK(framebuffer[(i+2)*320+j+294]==!!(c21_corner_bits[i*3+j/8]&(128>>(j%8))));}
}
static void screens(void){
 unsigned id,i,page;App a;char name[80];
 app_init(&a);a.screen=S_SYMBOLS;shot(&a,"symbols");
 for(id=0;id<LEGACY_CALC_COUNT;id++){
  app_init(&a);a.calc=(CalcId)id;a.screen=S_FORM;
  clear_screen();CHECK(formula_draw((CalcId)id,12,77,296,128));CHECK(!display_clipped_pixels);
  {MathInteraction paint;unsigned j;clear_screen();CHECK(formula_interactive((CalcId)id,calc_input_id((CalcId)id,0),5,68,310,58,&paint));
   for(j=0;j<paint.count;j++){MathFieldRect *f=&paint.fields[j];CHECK(f->x>=8&&f->x+f->width<=312&&f->y>=68&&f->y+f->height<=126);}
  }
  clear_screen();CHECK(formula_draw_values((CalcId)id,handbook[id].example_inputs,12,114,296,66));CHECK(!display_clipped_pixels);
  if(calculations[id].count<=2){int y=96+35*(int)calculations[id].count;clear_screen();CHECK(formula_draw((CalcId)id,12,y,296,215-y));CHECK(!display_clipped_pixels);}
  for(i=0;i<calculations[id].count;i++)strcpy(a.values[i],"-1.23456789012345678901234E-123");
  for(i=0;i<calculations[id].count+3;i++){a.row=i;render(&a);}
  a.row=0;shot(&a,id==FRESNEL?"fresnel-list-long":"list-long");
  for(i=0;i<calculations[id].count;i++){
   a.row=i;key(&a,K_ENTER);CHECK(a.screen==S_EDIT);a.edit_cursor=31;render(&a);key(&a,K_ESC);
   key(&a,K_RIGHT);CHECK(a.screen==S_UNITS);key(&a,K_DOWN);key(&a,K_UP);key(&a,K_ESC);
  }
  key(&a,K_MODE);CHECK(a.input_mode==INPUT_FORMULA);
  for(i=0;i<calculations[id].count;i++){key(&a,K_TAB);key(&a,K_UNITS);key(&a,K_ESC);}
  a.result_valid=1;a.result=1.2345678901e100;a.screen=S_RESULT;render(&a);key(&a,K_OUTPUT_UNITS);key(&a,K_ESC);
  key(&a,K_TARGET);if(a.screen==S_TARGET)key(&a,K_ESC);
  a.result_valid=0;a.input_error=NUM_EMPTY;a.error=INVALID;a.error_field=0;render(&a);
  a.input_error=NUM_OK;a.error=OK;key(&a,K_INFO);CHECK(a.screen==S_INFO);
  for(page=0;page<HANDBOOK_PAGES;page++){
   unsigned total,limit,j;key(&a,K_DIGIT0+1+(int)page);CHECK(a.info_page==page&&a.info_scroll==0);total=app_info_lines(&a);limit=total>11?total-11:0;
   for(j=0;j<limit+2;j++){key(&a,K_DOWN);}CHECK(a.info_scroll==limit);if(limit)scroll_pages++;
   if(limit){snprintf(name,sizeof(name),"handbook-scroll-%u-%u",id,page);shot(&a,name);}
   for(j=0;j<limit+2;j++){key(&a,K_UP);}CHECK(a.info_scroll==0);
   if(id==LC_CAP){snprintf(name,sizeof(name),"handbook-page-%u",page+1);shot(&a,name);}
  }
  key(&a,K_ESC);CHECK(a.screen==S_RESULT);
  key(&a,K_MODE);CHECK(a.screen==S_FORM);a.row=0;a.error=INVALID;a.input_error=NUM_FORMAT;render(&a);
 }
}
static void acceptance_screens(void){
 App a;unsigned i;app_init(&a);key(&a,K_ENTER);key(&a,K_DIGIT0+3);key(&a,K_DIGIT0+1);key(&a,K_DIGIT0+2);
 CHECK(a.calc==LC_CAP);key(&a,K_RIGHT);key(&a,K_DIGIT0+3);key(&a,K_ENTER);key(&a,K_DIGIT0+8);key(&a,K_DIGIT0+6);key(&a,K_DIGIT0+8);key(&a,K_ENTER);
 key(&a,K_DOWN);key(&a,K_RIGHT);key(&a,K_DIGIT0+4);key(&a,K_ENTER);key(&a,K_DIGIT0+1);key(&a,K_DIGIT0);key(&a,K_ENTER);
 key(&a,K_OUTPUT_UNITS);key(&a,K_DIGIT0+4);shot(&a,"lc-list-inputs");key(&a,K_CALCULATE);CHECK(a.result_valid);shot(&a,"lc-list-result");
 key(&a,K_MODE);shot(&a,"lc-formula-result");if(a.row!=1)key(&a,K_RIGHT);key(&a,K_ENTER);key(&a,K_CLEAR);key(&a,K_DIGIT0+2);key(&a,K_DIGIT0);key(&a,K_ENTER);key(&a,K_CALCULATE);shot(&a,"lc-formula-edited");
 key(&a,K_INFO);key(&a,K_DIGIT0+6);shot(&a,"handbook-limits");key(&a,K_ESC);CHECK(!strcmp(a.values[1],"20"));
 key(&a,K_TARGET);shot(&a,"output-selector");key(&a,K_ESC);key(&a,K_UNITS);shot(&a,"units");key(&a,K_ESC);key(&a,K_ENTER);shot(&a,"numeric-editor");
 key(&a,K_CLEAR);key(&a,K_DIGIT0);key(&a,K_ENTER);CHECK(a.screen==S_EDIT&&a.input_error!=NUM_OK);shot(&a,"invalid-input");
 /* All menu focus positions fit and remain distinct in B/W. */
 app_init(&a);key(&a,K_ENTER);for(i=0;i<10;i++){key(&a,K_DOWN);CHECK(a.selected==(int)((i+1)%10));}
}
int main(void){(void)c21_draw_mono;wrapping();menus();screens();acceptance_screens();printf("PASS: %u Phase 3C monochrome UI checks (%u frames, %u scrollable handbook pages)\n",checks,frames,scroll_pages);return 0;}
