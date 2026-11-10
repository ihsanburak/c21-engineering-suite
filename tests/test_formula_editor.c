#include "ui.h"
#include "formula_editor_refs.h"
#include "math_layout.h"
#include "display.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"FORMULA EDITOR FAIL line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static void near(double x,double expected){CHECK(isfinite(x));CHECK(fabs(x-expected)<1e-9*fmax(fabs(expected),1e-300));}
static void digit(App *a,int n){app_key(a,K_DIGIT0+n);}
static App open_calc(CalcId id,int formula){
 App a;app_init(&a);app_key(&a,K_ENTER);
 if(id==CURRENT){digit(&a,2);digit(&a,1);digit(&a,2);}
 else if(id==MW_DBM){digit(&a,1);digit(&a,1);digit(&a,1);}
 else {digit(&a,3);digit(&a,1);digit(&a,id==LC_INDUCTANCE?3:2);}
 CHECK(a.screen==S_FORM&&a.calc==id);if(formula)app_key(&a,K_MODE);
 CHECK(a.input_mode==(formula?INPUT_FORMULA:INPUT_LIST));return a;
}
static void focus(App *a,unsigned field){unsigned guard=0;if(a->screen==S_RESULT)app_key(a,K_ESC);CHECK(a->screen==S_FORM||a->screen==S_FORMULA);while(a->row!=field&&guard++<8)app_key(a,a->input_mode==INPUT_FORMULA?K_RIGHT:K_DOWN);CHECK(a->row==field);}
static void type(App *a,const char *s){for(;*s;s++){int k=K_NONE;if(*s>='0'&&*s<='9')k=K_DIGIT0+*s-'0';else if(*s=='.')k=K_DOT;else if(*s=='-')k=K_SIGN;else if(*s=='+')k=K_PLUS;else if(*s=='E'||*s=='e')k=K_EXP;CHECK(k!=K_NONE);app_key(a,k);}}
static void edit(App *a,unsigned field,const char *value){focus(a,field);app_key(a,K_ENTER);CHECK(a->screen==S_EDIT);app_key(a,K_CLEAR);type(a,value);app_key(a,K_ENTER);CHECK(a->screen==(a->input_mode==INPUT_FORMULA?S_FORMULA:S_FORM)&&a->error==OK&&!a->result_valid);}
static void unit(App *a,unsigned field,unsigned index){
 unsigned before=a->row;if(a->screen==S_RESULT)app_key(a,K_ESC);
 if(a->input_mode==INPUT_FORMULA&&field==calculations[a->calc].count)app_key(a,K_OUTPUT_UNITS);
 else{focus(a,field);app_key(a,a->input_mode==INPUT_FORMULA?K_UNITS:K_RIGHT);}
 CHECK(a->screen==S_UNITS);digit(a,(int)index+1);CHECK(a->screen==(a->input_mode==INPUT_FORMULA?S_FORMULA:S_FORM));
 CHECK(field<calculations[a->calc].count?a->units[field]==index:a->output_unit==index);
 if(a->input_mode==INPUT_FORMULA&&field==calculations[a->calc].count)CHECK(a->row==before);
}
static void compute(App *a,double expected){app_key(a,K_CALCULATE);CHECK(a->screen==S_RESULT&&a->result_valid&&a->error==OK);near(a->result,expected);}
static void target(App *a,int item,CalcId expected){app_key(a,K_TARGET);CHECK(a->screen==S_TARGET);digit(a,item);CHECK(a->calc==expected&&a->screen==(a->input_mode==INPUT_FORMULA?S_FORMULA:S_FORM));}
static void shot(App *a,const char *name){char path[160];FILE *f;unsigned i;app_draw(a);snprintf(path,sizeof(path),"build/phase3b-%s.pgm",name);f=fopen(path,"wb");CHECK(f!=0);fprintf(f,"P5\n320 240\n255\n");for(i=0;i<76800;i++)fputc(framebuffer[i]?0:255,f);CHECK(fclose(f)==0);}
static void preserved(App *a){
 App before=*a;app_key(a,K_INFO);CHECK(a->screen==S_INFO);digit(a,7);app_key(a,K_ESC);CHECK(a->screen==before.screen&&a->row==before.row&&a->input_mode==before.input_mode);CHECK(!memcmp(a->values,before.values,sizeof(a->values)));CHECK(!memcmp(a->units,before.units,sizeof(a->units)));CHECK(a->result_valid==before.result_valid&&a->result==before.result);
}
static void lc(int mode){
 App a=open_calc(LC_CAP,mode);char name[80];App before;unsigned i;
 app_key(&a,K_CALCULATE);CHECK(a.input_error==NUM_EMPTY&&!a.result_valid&&a.screen==(mode?S_FORMULA:S_FORM));
 unit(&a,0,2);edit(&a,0,"868");unit(&a,1,3);edit(&a,1,"10");unit(&a,2,3);
 snprintf(name,sizeof(name),"lc-%s-10-inputs",mode?"formula":"list");shot(&a,name);
 if(mode){unsigned original=a.row;app_key(&a,K_LEFT);CHECK(a.row==(original+1)%2);app_key(&a,K_TAB);CHECK(a.row==original);for(i=0;i<8;i++){app_key(&a,K_TAB);CHECK(a.row<2);}CHECK(calc_input_id(a.calc,a.row)!=calc_output_id(a.calc));}
 compute(&a,B_CAP_10);snprintf(name,sizeof(name),"lc-%s-10-result",mode?"formula":"list");shot(&a,name);preserved(&a);
 edit(&a,1,"20");compute(&a,B_CAP_20);snprintf(name,sizeof(name),"lc-%s-20-result",mode?"formula":"list");shot(&a,name);
 before=a;app_key(&a,K_MODE);CHECK(!memcmp(a.values,before.values,sizeof(a.values))&&!memcmp(a.units,before.units,sizeof(a.units)));CHECK(a.result_valid&&a.result==before.result);app_key(&a,K_MODE);
 target(&a,1,LC_FREQ);CHECK(a.units[0]==3&&a.units[1]==3&&a.output_unit==2);compute(&a,868e6);
 target(&a,2,LC_INDUCTANCE);CHECK(a.units[0]==2&&a.units[1]==3&&a.output_unit==3);compute(&a,20e-9);
 edit(&a,1,"5");compute(&a,B_IND_C5);snprintf(name,sizeof(name),"lc-%s-inductance",mode?"formula":"list");shot(&a,name);
 target(&a,3,LC_CAP);compute(&a,5e-12);
 app_key(&a,K_ESC);before=a;app_key(&a,K_TARGET);app_key(&a,K_DOWN);app_key(&a,K_ESC);CHECK(a.calc==before.calc&&a.screen==before.screen&&!memcmp(a.values,before.values,sizeof(a.values))&&a.result_valid==before.result_valid);
 focus(&a,0);app_key(&a,K_ENTER);app_key(&a,K_CLEAR);type(&a,"0");app_key(&a,K_ENTER);CHECK(a.screen==S_EDIT&&a.input_error==NUM_DOMAIN);preserved(&a);app_key(&a,K_ESC);CHECK(!strcmp(a.values[0],before.values[0]));
 focus(&a,0);app_key(&a,K_ENTER);app_key(&a,K_CLEAR);type(&a,"1E");app_key(&a,K_ENTER);CHECK(a.screen==S_EDIT&&a.input_error==NUM_FORMAT);app_key(&a,K_ESC);
 focus(&a,0);app_key(&a,K_ENTER);app_key(&a,K_CLEAR);type(&a,"1E9999");app_key(&a,K_ENTER);CHECK(a.screen==S_EDIT&&a.input_error==NUM_RANGE);app_key(&a,K_ESC);
 CHECK(!app_request_formula(&a,1,1)&&a.screen==S_FORM&&a.input_mode==INPUT_LIST&&a.mode_fallback);CHECK(!strcmp(a.values[0],before.values[0]));CHECK(app_request_formula(&a,310,58)&&a.screen==S_FORMULA);
}
static void ohm(int mode){
 App a=open_calc(CURRENT,mode);char name[60];unit(&a,0,0);edit(&a,0,"12");unit(&a,1,0);edit(&a,1,"100");compute(&a,.12);
 snprintf(name,sizeof(name),"ohm-%s-current",mode?"formula":"list");shot(&a,name);
 target(&a,1,VOLTAGE);compute(&a,12);target(&a,3,RESISTANCE);compute(&a,100);preserved(&a);
 edit(&a,1,"-.12");app_key(&a,K_CALCULATE);CHECK(!a.result_valid&&a.error==INVALID);
 edit(&a,0,"-12");compute(&a,100);target(&a,2,CURRENT);compute(&a,-.12);
 app_key(&a,K_ESC);focus(&a,1);app_key(&a,K_ENTER);app_key(&a,K_CLEAR);type(&a,"0");app_key(&a,K_ENTER);CHECK(a.screen==S_EDIT&&a.input_error==NUM_ZERO);app_key(&a,K_ESC);
}
static void rf(int mode){
 App a=open_calc(MW_DBM,mode);char name[60];unit(&a,0,1);edit(&a,0,"100");compute(&a,20);edit(&a,0,"200");compute(&a,23.010299956639812);
 snprintf(name,sizeof(name),"rf-%s-dbm",mode?"formula":"list");shot(&a,name);preserved(&a);
 if(mode){App before=a;app_key(&a,K_OUTPUT_UNITS);CHECK(a.screen==S_UNITS);app_key(&a,K_ESC);CHECK(a.screen==before.screen&&a.result_valid&&a.row==before.row);app_key(&a,K_ENTER);CHECK(a.screen==S_EDIT&&!strcmp(a.edit,"200"));app_key(&a,K_DELETE);CHECK(!strcmp(a.edit,"20"));preserved(&a);app_key(&a,K_ESC);CHECK(!strcmp(a.values[0],"200")&&a.result_valid);}
 target(&a,2,DBM_MW);CHECK(a.output_unit==1);compute(&a,.2);edit(&a,0,"-30");compute(&a,1e-6);
 target(&a,1,MW_DBM);CHECK(a.units[0]==1);compute(&a,-30);
}
static void fields_and_bounds(void){
 unsigned id,i,j;static unsigned char normal[76800];
 for(id=0;id<LEGACY_CALC_COUNT;id++){
  MathInteraction paint,active;int inside=1,changed=0;unsigned outputs=0,seen=0;
  clear_screen();CHECK(formula_interactive((CalcId)id,VAR_NONE,5,68,310,58,&paint));memcpy(normal,framebuffer,sizeof(normal));
  CHECK(paint.count>=calculations[id].count+1&&paint.count<MATH_FIELD_RECTS);
  for(i=0;i<paint.count;i++){
   const MathFieldRect *r=&paint.fields[i];CHECK(r->x>=5&&r->y>=68&&r->x+r->width<=315&&r->y+r->height<=126);
   CHECK(r->role==M_INPUT||r->role==M_OUTPUT);
   if(r->role==M_OUTPUT){outputs++;CHECK(r->variable==calc_output_id((CalcId)id));}
   else{int found=0;for(j=0;j<calculations[id].count;j++)if(r->variable==calc_input_id((CalcId)id,j)){found=1;seen|=1u<<j;}CHECK(found);}
   for(j=i+1;j<paint.count;j++){const MathFieldRect *s=&paint.fields[j];CHECK(r->x+r->width<=s->x||s->x+s->width<=r->x||r->y+r->height<=s->y||s->y+s->height<=r->y);}
  }
  CHECK(outputs==1&&seen==((1u<<calculations[id].count)-1));
  clear_screen();CHECK(formula_interactive((CalcId)id,calc_input_id((CalcId)id,0),5,68,310,58,&active));
  for(i=0;i<76800;i++){
   if(framebuffer[i]&&(i%320<5||i%320>=315||i/320<68||i/320>=126))inside=0;
   if(framebuffer[i]!=normal[i]){int match=0;changed++;for(j=0;j<paint.count;j++){const MathFieldRect *r=&paint.fields[j];if(r->role==M_INPUT&&r->variable==calc_input_id((CalcId)id,0)&&(int)(i%320)>=r->x&&(int)(i%320)<r->x+r->width&&(int)(i/320)>=r->y&&(int)(i/320)<r->y+r->height)match=1;}if(!match)inside=0;}
  }
  CHECK(inside&&changed>0);
 }
 CHECK(calc_input_id(LC_CAP,0)==VAR_F&&calc_input_id(LC_CAP,1)==VAR_L&&calc_output_id(LC_CAP)==VAR_C);
 CHECK(calc_input_id(LC_INDUCTANCE,0)==VAR_F&&calc_input_id(LC_INDUCTANCE,1)==VAR_C&&calc_output_id(LC_INDUCTANCE)==VAR_L);
}
static void inverse_limits(void){
 App a=open_calc(LC_INDUCTANCE,1);edit(&a,0,"1E-150");edit(&a,1,"1E-100");app_key(&a,K_CALCULATE);CHECK(a.error==RANGE&&!a.result_valid&&a.screen==S_FORMULA);
 edit(&a,0,"1E150");edit(&a,1,"1E100");app_key(&a,K_CALCULATE);CHECK(a.error==RANGE&&!a.result_valid);
 focus(&a,1);app_key(&a,K_ENTER);app_key(&a,K_CLEAR);type(&a,"0");app_key(&a,K_ENTER);CHECK(a.screen==S_EDIT&&a.input_error==NUM_DOMAIN);app_key(&a,K_ESC);
 edit(&a,0,"868E6");edit(&a,1,"5E-12");unit(&a,2,3);compute(&a,B_IND_C5);
}
int main(void){int mode;fields_and_bounds();inverse_limits();for(mode=0;mode<2;mode++){
 App a=open_calc(LC_CAP,mode);unit(&a,0,2);edit(&a,0,"868");unit(&a,1,3);edit(&a,1,"10");unit(&a,2,3);
 target(&a,1,LC_FREQ);CHECK(a.values[1][0]==0&&a.units[1]==3);target(&a,3,LC_CAP);CHECK(!strcmp(a.values[0],"868")&&!strcmp(a.values[1],"10")&&a.output_unit==3);
 lc(mode);ohm(mode);rf(mode);
 }printf("PASS: %u Phase 3B formula-editor checks (both modes, targets, fields, bounds)\n",checks);return 0;}
