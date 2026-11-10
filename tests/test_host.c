#include "engine.h"
#include "ui.h"
#include "display.h"
#include "handbook.h"
#include "math_layout.h"
#include "c21_logo.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
static unsigned checks;
static void logo_pixel(int x,int y,int black){rect(x,y,1,1,black);}
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static void expect(CalcId id,double a,double b,double c,double expected){double x[]={a,b,c},r=-1;CHECK(calculate(id,x,&r)==OK);CHECK(fabs(r-expected)<=1e-10*fmax(fabs(expected),1e-20));}
static void save_frame(const char *path){FILE *f=fopen(path,"wb");int i;CHECK(f!=0);fprintf(f,"P5\n320 240\n255\n");for(i=0;i<320*240;i++)fputc(framebuffer[i]?0:255,f);fclose(f);}
static void check_menu_shortcuts(App a,unsigned char *visited){
 unsigned count=0,i;App cursor=a;
 if(visited[a.menu])return;
 visited[a.menu]=1;
 do{count++;app_key(&cursor,K_DOWN);}while(cursor.selected!=a.selected&&count<=10);
 CHECK(count>0&&count<=10);
 for(i=0;i<count;i++){
  App enter=a,digit=a;
  enter.selected=(int)i;app_key(&enter,K_ENTER);
  app_key(&digit,K_DIGIT0+(i==9?0:(int)i+1));
  CHECK(memcmp(&enter,&digit,sizeof(App))==0);
  if(enter.screen==S_MENU&&enter.depth>a.depth)check_menu_shortcuts(enter,visited);
 }
 if(count<9){App invalid=a;app_key(&invalid,K_DIGIT0+9);CHECK(memcmp(&a,&invalid,sizeof(App))==0);}
}
static void check_info_restore(App a,int exit_key){
 App before=a;app_key(&a,K_INFO);CHECK(a.screen==S_INFO&&!a.handbook_origin);
 app_key(&a,K_RIGHT);app_key(&a,exit_key);
 CHECK(a.screen==before.screen&&a.calc==before.calc&&a.row==before.row);
 CHECK(a.menu==before.menu&&a.selected==before.selected&&a.depth==before.depth);
 CHECK(memcmp(a.values,before.values,sizeof(a.values))==0);
 CHECK(memcmp(a.edit,before.edit,sizeof(a.edit))==0);
 CHECK(memcmp(a.units,before.units,sizeof(a.units))==0);
 CHECK(a.output_unit==before.output_unit&&a.unit_cursor==before.unit_cursor);
 CHECK(a.error==before.error&&a.result==before.result);
}
int main(void){double x[3]={1,1,1},r;unsigned id,i,j;App a;
 expect(VOLTAGE,.02,1000,0,20);expect(CURRENT,12,1000,0,.012);expect(RESISTANCE,12,.02,0,600);expect(POWER,12,.5,0,6);
 expect(MW_DBM,.001,0,0,0);expect(MW_DBM,1,0,0,30);expect(DBM_MW,0,0,0,.001);expect(DBM_MW,-30,0,0,1e-6);
 expect(WAVELENGTH,100e6,0,0,2.99792458);expect(QUARTER_WAVE,100e6,0,0,.749481145);
 expect(LC_FREQ,1e-3,1e-6,0,5032.921210448704);expect(LC_CAP,5032.921210448704,1e-3,0,1e-6);
 expect(XL,1e3,1e-3,0,6.283185307179586);expect(XC,1e3,1e-6,0,159.15494309189535);
 expect(FSPL,1000,1e9,0,92.44778322188338);expect(FRESNEL,1e9,500,500,8.657257908300624);
 expect(LC_FREQ,1e200,1e-200,0,1/6.283185307179586);
 expect(FRESNEL,299792458.,1e300,1e300,sqrt(5e299));
 for(id=0;id<LEGACY_CALC_COUNT;id++)for(i=0;i<calculations[id].count;i++){
  double bad[]={0,-1,NAN,INFINITY,-INFINITY};
  for(j=0;j<5;j++){x[0]=x[1]=x[2]=1;x[i]=bad[j];if(j<2&&(id==DBM_MW||id==POWER||(id==VOLTAGE&&(j==0||i==0))||(id==CURRENT&&i==0)||(id==RESISTANCE&&i==0&&j==0)))CHECK(calculate((CalcId)id,x,&r)==OK);else CHECK(calculate((CalcId)id,x,&r)==INVALID);}
 }
 x[0]=DBL_MAX;x[1]=DBL_MAX;CHECK(calculate(POWER,x,&r)==RANGE);x[0]=-DBL_MAX;CHECK(calculate(DBM_MW,x,&r)==RANGE);x[0]=DBL_MAX;CHECK(calculate(DBM_MW,x,&r)==RANGE);
 x[0]=DBL_MIN;x[1]=DBL_MIN;CHECK(calculate(POWER,x,&r)==RANGE);
 CHECK(calculate(CALC_COUNT,x,&r)==INVALID);CHECK(calculate(POWER,0,&r)==INVALID);CHECK(calculate(POWER,x,0)==INVALID);
 CHECK(calculate((CalcId)-1,x,&r)==INVALID);
 CHECK(parse_number("1.25e3",1e-3,&r)==OK&&r==1.25);CHECK(parse_number("-30",1,&r)==OK&&r==-30);
 {const char *bad[]={""," ","1x","1e","--1","nan","inf","1e9999","1e-9999"};for(i=0;i<sizeof(bad)/sizeof(bad[0]);i++)CHECK(parse_number(bad[i],1,&r)==INVALID);}
 CHECK(parse_number("1e300",1e100,&r)==RANGE);CHECK(parse_number("1e-300",1e-100,&r)==RANGE);CHECK(parse_number("1",0,&r)==INVALID);
 CHECK(convert_output(.001,.001,&r)==OK&&r==1);CHECK(convert_output(DBL_MAX,1e-3,&r)==RANGE);
 /* Round trips across a useful RF range. */
 for(i=0;i<100;i++){double power=pow(10,-12+i*.15),db;x[0]=power;CHECK(calculate(MW_DBM,x,&db)==OK);x[0]=db;CHECK(calculate(DBM_MW,x,&r)==OK);CHECK(fabs(r/power-1)<1e-12);}
 /* Every category enters a submenu, another nested level, and restores focus. */
 for(i=0;i<9;i++){app_init(&a);app_key(&a,K_ENTER);for(j=0;j<i;j++)app_key(&a,K_DOWN);CHECK(a.selected==(int)i);app_key(&a,K_ENTER);CHECK(a.screen==S_MENU&&a.depth==1);if(i!=8){app_key(&a,K_ENTER);CHECK(a.screen==S_MENU&&a.depth==2);app_key(&a,K_ESC);}app_key(&a,K_ESC);CHECK(a.menu==0&&a.selected==(int)i);}
 app_init(&a);app_key(&a,K_ENTER);app_key(&a,K_DOWN);app_key(&a,K_ENTER);app_key(&a,K_ENTER);app_key(&a,K_ENTER);CHECK(a.screen==S_FORM&&a.calc==VOLTAGE);
 app_key(&a,K_ENTER);app_key(&a,K_DELETE);app_key(&a,K_DIGIT0+2);app_key(&a,K_ENTER);CHECK(strcmp(a.values[0],"2")==0);
 app_key(&a,K_RIGHT);app_key(&a,K_DOWN);app_key(&a,K_ESC);CHECK(a.units[0]==0);
 app_key(&a,K_RIGHT);app_key(&a,K_DOWN);app_key(&a,K_ENTER);CHECK(a.units[0]==1);
 app_key(&a,K_DOWN);app_key(&a,K_ENTER);app_key(&a,K_DIGIT0+1);app_key(&a,K_ENTER);app_key(&a,K_DOWN);app_key(&a,K_DOWN);app_key(&a,K_ENTER);CHECK(a.screen==S_RESULT);CHECK(fabs(a.result-.002)<1e-15);
 app_key(&a,K_ENTER);CHECK(a.screen==S_INFO);for(i=0;i<4;i++){app_draw(&a);app_key(&a,K_RIGHT);}app_key(&a,K_ESC);CHECK(a.screen==S_RESULT);
 {unsigned required[]={0x3a9,0x3bc,0x3c0,0x3bb,0x394,0x3a3,0x3b5,0x3c6,0x3b8,0x3b1,0x3b2,0x3b7,0x3c4,0x3c9,0x3c1,0x3b3,0x221a,0xb1,0xd7,0xf7,0xb0,0xb2,0xb3,0x207b,0x2080,0x2081,0x2190,0x2191,0x2192,0x2193};for(i=0;i<sizeof(required)/sizeof(required[0]);i++)CHECK(glyph_supported(required[i]));}
 app_init(&a);app_draw(&a);save_frame("build/splash-unbranded.pgm");app_key(&a,K_ENTER);app_draw(&a);save_frame("build/main-menu.pgm");a.screen=S_SYMBOLS;app_draw(&a);save_frame("build/symbols.pgm");
 /* Draw all calculators and all nine categories to catch bounds issues in host tools. */
 for(id=0;id<LEGACY_CALC_COUNT;id++){a.calc=(CalcId)id;a.screen=S_FORM;memset(a.units,0,sizeof(a.units));a.output_unit=0;for(i=0;i<3;i++)strcpy(a.values[i],"1");app_draw(&a);}
 /* Preserve the original checks above; extend them for the approved handbook. */
 CHECK(glyph_supported('&'));CHECK(glyph_supported('>'));CHECK(glyph_supported('<'));
 clear_screen();c21_draw_mono(0,0,C21_CORNER_W,C21_CORNER_H,c21_corner_bits,logo_pixel);
 {int equal=1;for(i=0;i<24;i++)for(j=0;j<24;j++)if(framebuffer[i*320+j]!=!!(c21_corner_bits[i*3+j/8]&(128>>(j%8))))equal=0;CHECK(equal);}
 for(i=0;i<10;i++)CHECK(glyph_supported(0x2080+i));
 app_init(&a);app_draw(&a);
 {int equal=1;for(i=0;i<320*240;i++)if(framebuffer[i]!=!!(c21_splash_bits[i/8]&(128>>(i%8))))equal=0;CHECK(equal);}
 save_frame("build/splash.pgm");
 app_key(&a,K_ENTER);for(i=0;i<9;i++)app_key(&a,K_DOWN);
 CHECK(a.selected==9);app_draw(&a);save_frame("build/ten-category-menu.pgm");app_key(&a,K_ENTER);
 CHECK(a.screen==S_MENU&&a.depth==1);
 for(id=0;id<LEGACY_CALC_COUNT;id++){
  const Calculation *c=&calculations[id];const HandbookEntry *h=&handbook[id];
  unsigned group,item;
  if(id==LC_INDUCTANCE){group=4;item=5;}else if(id<4){group=1;item=id+1;}else if(id<6){group=2;item=id-3;}else if(id<8){group=3;item=id-5;}else if(id>=12){group=3;item=id-9;}else{group=4;item=id-7;}
  app_init(&a);app_key(&a,K_ENTER);app_key(&a,K_DIGIT0);CHECK(a.menu==17&&a.screen==S_MENU);
  app_key(&a,K_DIGIT0+(int)group);CHECK(a.menu==17+(int)group);
  app_key(&a,K_DIGIT0+(int)item);CHECK(a.screen==S_INFO&&a.handbook_origin&&a.calc==(CalcId)id);
  for(i=0;i<HANDBOOK_PAGES;i++){char path[80];CHECK(a.info_page==i);app_draw(&a);snprintf(path,sizeof(path),"build/handbook-%02u-%02u.pgm",id,i);save_frame(path);app_key(&a,K_RIGHT);}
  CHECK(a.info_page==0);app_key(&a,K_ESC);CHECK(a.screen==S_MENU&&a.selected==(int)item-1);
  app_key(&a,K_ENTER);app_key(&a,K_ENTER);CHECK(a.screen==S_FORM&&a.calc==(CalcId)id);
  CHECK(calculate((CalcId)id,h->example_inputs,&r)==OK);CHECK(fabs(r-h->example_result)<=1e-10*fabs(h->example_result));
  CHECK(h->purpose&&*h->purpose&&h->measurements&&*h->measurements&&h->laboratory&&*h->laboratory&&h->limits&&*h->limits&&h->example&&*h->example&&h->mistakes&&*h->mistakes);
  /* Each selectable input unit must reproduce the handbook reference result. */
  for(i=0;i<c->count;i++)for(j=0;j<c->inputs[i].count;j++){
   unsigned k;
   for(k=0;k<c->count;k++){a.units[k]=k==i?j:0;snprintf(a.values[k],sizeof(a.values[k]),"%.17g",h->example_inputs[k]/c->inputs[k].units[a.units[k]].scale);}
   a.screen=S_FORM;a.row=c->count+1;app_key(&a,K_ENTER);CHECK(a.screen==S_RESULT);CHECK(fabs(a.result-h->example_result)<=1e-10*fabs(h->example_result));
  }
  for(i=0;i<c->output_count;i++){CHECK(convert_output(h->example_result,c->outputs[i].scale,&r)==OK);CHECK(fabs(r*c->outputs[i].scale-h->example_result)<=1e-12*fabs(h->example_result));}
  a.screen=S_FORM;a.row=c->count+2;app_key(&a,K_ENTER);CHECK(a.screen==S_INFO&&!a.handbook_origin);app_key(&a,K_RIGHT);app_key(&a,K_ENTER);CHECK(a.screen==S_FORM);
  app_key(&a,K_ESC);CHECK(a.screen==S_MENU&&a.selected==(int)item-1);app_key(&a,K_ESC);CHECK(a.menu==17);app_key(&a,K_ESC);CHECK(a.menu==0&&a.selected==9);
 }
 CHECK(a.menu==0&&a.selected==9);
 /* All 22 numbered menus use digits with the same result as arrow/Enter. */
 {unsigned char visited[APP_MENU_COUNT]={0};unsigned menu;app_init(&a);app_key(&a,K_ENTER);check_menu_shortcuts(a,visited);for(menu=0;menu<APP_MENU_COUNT;menu++)CHECK(visited[menu]);}
 app_init(&a);app_key(&a,K_ENTER);app_key(&a,K_DIGIT0+2);app_key(&a,K_DIGIT0+1);app_key(&a,K_DIGIT0+1);
 CHECK(a.calc==VOLTAGE&&a.screen==S_FORM);
 strcpy(a.values[0],"2.5");strcpy(a.values[1],"100");a.units[0]=1;a.output_unit=1;a.result=.25;a.error=INVALID;
 check_info_restore(a,K_ESC);check_info_restore(a,K_ENTER);check_info_restore(a,K_INFO);
 a.screen=S_EDIT;strcpy(a.edit,"1e-");check_info_restore(a,K_ESC);check_info_restore(a,K_ENTER);check_info_restore(a,K_INFO);
 a.screen=S_UNITS;a.unit_cursor=2;check_info_restore(a,K_ESC);check_info_restore(a,K_ENTER);check_info_restore(a,K_INFO);
 a.screen=S_RESULT;check_info_restore(a,K_ESC);check_info_restore(a,K_ENTER);check_info_restore(a,K_INFO);
 /* Units are numbered; a digit commits exactly that pending selection. */
 a.screen=S_FORM;a.row=0;app_key(&a,K_RIGHT);app_key(&a,K_DIGIT0+3);CHECK(a.screen==S_FORM&&a.units[0]==2);
 app_key(&a,K_INFO);app_key(&a,K_DIGIT0+8);CHECK(a.info_page==7);app_key(&a,K_ESC);CHECK(a.screen==S_FORM);
 /* Measured formula rectangles must contain every drawn pixel. */
 for(id=0;id<FORMULA_COUNT;id++){if(id>=LEGACY_CALC_COUNT&&id<CALC_COUNT)continue;
  MathArena arena;const MathNode *node;MathBox box;int fits=1,ink=0;char path[80];
  math_arena_init(&arena);node=formula_build(&arena,(int)id);CHECK(node&&!arena.overflow&&arena.used<MATH_CAPACITY);
  box=math_measure(node);CHECK(box.width>0&&box.width<=310&&box.above+box.below<=90);
  clear_screen();math_draw(node,5,70+box.above,1);
  for(i=0;i<240;i++)for(j=0;j<320;j++)if(framebuffer[i*320+j]){ink++;if(j<5||j>=(unsigned)(5+box.width)||i<70||i>=(unsigned)(70+box.above+box.below))fits=0;}
  CHECK(fits&&ink>8);snprintf(path,sizeof(path),"build/math-%02u.pgm",id);save_frame(path);
  clear_screen();CHECK(formula_draw((int)id,5,55,310,90));
  if(id<LEGACY_CALC_COUNT){
   char literals[3][32];const char *bindings[3];
   for(i=0;i<3;i++){snprintf(literals[i],sizeof(literals[i]),"%.8g",handbook[id].example_inputs[i]);bindings[i]=literals[i];}
   math_arena_init(&arena);node=formula_build_values(&arena,(int)id,bindings);box=math_measure(node);
   CHECK(node&&!arena.overflow&&box.width<=310&&box.above+box.below<=70);
   clear_screen();math_draw(node,5,70+box.above,1);fits=1;ink=0;
   for(i=0;i<240;i++)for(j=0;j<320;j++)if(framebuffer[i*320+j]){ink++;if(j<5||j>=(unsigned)(5+box.width)||i<70||i>=(unsigned)(70+box.above+box.below))fits=0;}
   CHECK(fits&&ink>8);CHECK(formula_draw_values((int)id,handbook[id].example_inputs,5,55,310,70));
  }
 }
 {MathArena arena;const MathNode *f,*n,*d;MathBox box;
  math_arena_init(&arena);n=math_text(&arena,"1");d=math_text(&arena,"2");f=math_fraction(&arena,n,d);box=math_measure(f);
  CHECK(box.width==11&&box.above==15&&box.below==10);
  clear_screen();math_draw(f,10,40,1);
  for(i=10;i<21;i++){CHECK(framebuffer[37*320+i]==1);CHECK(framebuffer[38*320+i]==0);}
  CHECK(framebuffer[25*320+15]);CHECK(framebuffer[41*320+14]);
  CHECK(text_width("λ")==5&&text_width("2π")==11);
  clear_screen();CHECK(!formula_draw(LC_FREQ,0,0,1,1));CHECK(!formula_draw(-1,0,0,310,90));
  math_arena_init(&arena);for(i=0;i<MATH_CAPACITY;i++)CHECK(math_text(&arena,"x")!=0);
  CHECK(math_text(&arena,"x")==0&&arena.overflow);math_arena_init(&arena);CHECK(!arena.overflow&&arena.used==0);
 }
 printf("PASS: %u checks (math, invalid/boundary inputs, units, UI, glyph coverage, branding, handbook)\n",checks);return 0;
}
