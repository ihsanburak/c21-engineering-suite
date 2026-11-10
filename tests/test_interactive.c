#include "ui.h"
#include "display.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "interactive_vectors.h"
static unsigned checks;
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"INTERACTIVE FAIL line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static void near(double got,double expected){CHECK(isfinite(got));CHECK(fabs(got-expected)<=1e-9*fmax(fabs(expected),1e-300));}
static void digit(App *a,int n){app_key(a,K_DIGIT0+n);}
static App open_calculator(CalcId id){
 App a;app_init(&a);app_key(&a,K_ENTER);
 if(id<=POWER){digit(&a,2);digit(&a,1);digit(&a,(int)id+1);}
 else if(id<=DBM_MW){digit(&a,1);digit(&a,1);digit(&a,(int)id-3);}
 else if(id<=QUARTER_WAVE){digit(&a,7);digit(&a,1);digit(&a,(int)id-5);}
 else if(id<=LC_CAP){digit(&a,3);digit(&a,1);digit(&a,(int)id-7);}
 else if(id<=XC){digit(&a,3);digit(&a,2);digit(&a,(int)id-9);}
 else {digit(&a,7);digit(&a,2);digit(&a,(int)id-11);}
 CHECK(a.screen==S_FORM&&a.calc==id&&!a.result_valid);return a;
}
static void focus(App *a,unsigned row){unsigned guard=0;if(a->screen==S_RESULT)app_key(a,K_ESC);CHECK(a->screen==S_FORM);while(a->row!=row&&guard++<8)app_key(a,K_DOWN);CHECK(a->row==row);}
static void type(App *a,const char *s){
 for(;*s;s++){int k=K_NONE;if(*s>='0'&&*s<='9')k=K_DIGIT0+*s-'0';else if(*s=='.')k=K_DOT;else if(*s=='-')k=K_SIGN;else if(*s=='+')k=K_PLUS;else if(*s=='e'||*s=='E')k=K_EXP;CHECK(k!=K_NONE);app_key(a,k);}
}
static void begin_edit(App *a,unsigned row){focus(a,row);app_key(a,K_ENTER);CHECK(a->screen==S_EDIT);app_key(a,K_CLEAR);CHECK(a->edit[0]==0&&a->edit_cursor==0);}
static void edit(App *a,unsigned row,const char *value){begin_edit(a,row);type(a,value);app_key(a,K_ENTER);CHECK(a->screen==S_FORM&&a->error==OK&&!a->result_valid);}
static void unit(App *a,unsigned row,unsigned u){focus(a,row);app_key(a,K_RIGHT);CHECK(a->screen==S_UNITS);digit(a,(int)u+1);CHECK(a->screen==S_FORM);CHECK(row<calculations[a->calc].count?a->units[row]==u:a->output_unit==u);}
static App fill(const InteractiveVector *v){App a=open_calculator(v->id);unsigned i;for(i=0;i<calculations[v->id].count;i++){unit(&a,i,v->units[i]);edit(&a,i,v->values[i]);}unit(&a,calculations[v->id].count,v->output);return a;}
static void calculate_key(App *a){focus(a,calculations[a->calc].count+1);app_key(a,K_ENTER);}
static void result(App *a,double expected){calculate_key(a);CHECK(a->screen==S_RESULT&&a->result_valid&&a->error==OK);near(a->result,expected);}
static void screenshot(App *a,const char *name){char path[120];FILE *f;unsigned i;app_draw(a);snprintf(path,sizeof(path),"build/phase3a-%s.pgm",name);f=fopen(path,"wb");CHECK(f!=0);fprintf(f,"P5\n320 240\n255\n");for(i=0;i<76800;i++)fputc(framebuffer[i]?0:255,f);CHECK(fclose(f)==0);}
static int zero_valid(CalcId id,unsigned i){return id==POWER||id==DBM_MW||id==VOLTAGE||(id==CURRENT&&i==0)||(id==RESISTANCE&&i==0);}
static void all_calculators(void){
 unsigned id,j;for(id=0;id<sizeof(vectors)/sizeof(vectors[0]);id++){
  const InteractiveVector *v=&vectors[id];unsigned n=calculations[id].count;App a=open_calculator(v->id);char name[64];double displayed;
  /* Missing-field detection is reached through CALCULATE, one field at a time. */
  for(j=0;j<n;j++){CHECK(a.values[j][0]==0);calculate_key(&a);if(!(a.screen==S_FORM&&!a.result_valid&&a.input_error==NUM_EMPTY&&a.error_field==j))fprintf(stderr,"missing id=%u field=%u screen=%d err=%d input=%d at=%u values=%s/%s/%s\n",id,j,a.screen,a.error,a.input_error,a.error_field,a.values[0],a.values[1],a.values[2]);CHECK(a.screen==S_FORM&&!a.result_valid&&a.input_error==NUM_EMPTY&&a.error_field==j);unit(&a,j,v->units[j]);edit(&a,j,v->values[j]);}
  unit(&a,n,v->output);snprintf(name,sizeof(name),"%02u-user-inputs",id);screenshot(&a,name);
  result(&a,v->expected_si);CHECK(convert_output(a.result,calculations[id].outputs[v->output].scale,&displayed)==OK);near(displayed,v->expected_display);
  snprintf(name,sizeof(name),"%02u-live-result",id);screenshot(&a,name);
  /* A static handbook example must never replace the live values/result. */
  {App before=a;app_key(&a,K_INFO);digit(&a,7);CHECK(a.screen==S_INFO&&a.info_page==6);app_key(&a,K_ESC);CHECK(a.screen==S_RESULT&&a.result_valid);near(a.result,before.result);CHECK(memcmp(a.values,before.values,sizeof(a.values))==0);CHECK(memcmp(a.units,before.units,sizeof(a.units))==0);}
  for(j=0;j<n;j++){
   edit(&a,j,v->changed[j]);result(&a,v->changed_si[j]);CHECK(a.result!=v->expected_si);
   if((id==LC_CAP&&j==1)||id==MW_DBM){snprintf(name,sizeof(name),"%02u-recalculated",id);screenshot(&a,name);}
   edit(&a,j,v->values[j]);result(&a,v->expected_si);
   begin_edit(&a,j);type(&a,"1E");app_key(&a,K_ENTER);CHECK(a.screen==S_EDIT&&a.input_error==NUM_FORMAT);app_key(&a,K_ESC);CHECK(strcmp(a.values[j],v->values[j])==0);
   begin_edit(&a,j);app_key(&a,K_ENTER);CHECK(a.screen==S_EDIT&&a.input_error==NUM_EMPTY);app_key(&a,K_ESC);
   begin_edit(&a,j);type(&a,"1E9999");app_key(&a,K_ENTER);CHECK(a.screen==S_EDIT&&a.input_error==NUM_RANGE);app_key(&a,K_ESC);
   begin_edit(&a,j);type(&a,"1E-9999");app_key(&a,K_ENTER);CHECK(a.screen==S_EDIT&&a.input_error==NUM_RANGE);app_key(&a,K_ESC);
   begin_edit(&a,j);type(&a,"0");app_key(&a,K_ENTER);
   if(zero_valid((CalcId)id,j)){CHECK(a.screen==S_FORM);result(&a,id==DBM_MW?.001:0);edit(&a,j,v->values[j]);}
   else{CHECK(a.screen==S_EDIT&&(a.input_error==NUM_ZERO||a.input_error==NUM_DOMAIN));app_key(&a,K_ESC);}
  }
 }
}
static void acceptance(void){
 App a=fill(&vectors[LC_CAP]);double displayed;result(&a,3.3620240277167235e-12);CHECK(convert_output(a.result,1e-12,&displayed)==OK);near(displayed,3.3620240277167235);
 screenshot(&a,"accept-cap-10nH-result");edit(&a,1,"20");screenshot(&a,"accept-cap-20nH-inputs");result(&a,1.6810120138583618e-12);screenshot(&a,"accept-cap-20nH-result");
 a=fill(&vectors[CURRENT]);screenshot(&a,"accept-ohm-inputs");result(&a,.12);screenshot(&a,"accept-ohm-result");
 a=fill(&vectors[MW_DBM]);screenshot(&a,"accept-rf-100-inputs");result(&a,20);screenshot(&a,"accept-rf-100-result");edit(&a,0,"200");screenshot(&a,"accept-rf-200-inputs");result(&a,23.010299956639812);screenshot(&a,"accept-rf-200-result");
}
static void editor_and_signed(void){
 App a=open_calculator(CURRENT);unsigned i;
 begin_edit(&a,0);type(&a,"102");app_key(&a,K_HOME);app_key(&a,K_RIGHT);app_key(&a,K_FORWARD_DELETE);CHECK(strcmp(a.edit,"12")==0);
 app_key(&a,K_END);app_key(&a,K_DELETE);type(&a,"2");app_key(&a,K_LEFT);type(&a,".");app_key(&a,K_HOME);type(&a,"-");app_key(&a,K_END);type(&a,"E-1");CHECK(strcmp(a.edit,"-1.2E-1")==0);
 {App before=a;app_key(&a,K_INFO);app_key(&a,K_ESC);CHECK(a.edit_cursor==before.edit_cursor&&strcmp(a.edit,before.edit)==0);}
 app_key(&a,K_ENTER);CHECK(a.screen==S_FORM);edit(&a,1,"100");result(&a,-.0012);
 edit(&a,0,"-1.5E+1");result(&a,-.15);
 edit(&a,0,"0");result(&a,0);
 a=open_calculator(RESISTANCE);edit(&a,0,"-12");edit(&a,1,"-.12");result(&a,100);
 a=open_calculator(POWER);edit(&a,0,"-5");edit(&a,1,".25");result(&a,-1.25);
 a=open_calculator(LC_FREQ);edit(&a,0,"1.5E-9");edit(&a,1,"1E-12");result(&a,4109362960.409999);
 begin_edit(&a,0);for(i=0;i<31;i++)digit(&a,1);digit(&a,2);CHECK(a.input_error==NUM_FULL&&strlen(a.edit)==31&&a.edit_cursor==31);app_key(&a,K_CLEAR);CHECK(a.edit[0]==0&&a.edit_cursor==0);
}
static void numeric_extremes(void){
 static const struct {CalcId id;const char *x,*y,*z;} over[]={
 {VOLTAGE,"1E200","1E200",""},{CURRENT,"1E300","1E-300",""},{RESISTANCE,"1E300","1E-300",""},{POWER,"1E200","1E200",""},{DBM_MW,"10000","",""},{WAVELENGTH,"1E-300","",""},{QUARTER_WAVE,"1E-301","",""},{LC_CAP,"1E-150","1E-100",""},{XL,"1E200","1E200",""},{XC,"1E-200","1E-200",""},{FRESNEL,"1E-307","1E308","1E308"}};
 static const struct {CalcId id;const char *x,*y;} under[]={
 {VOLTAGE,"1E-200","1E-200"},{CURRENT,"1E-300","1E300"},{RESISTANCE,"1E-300","1E300"},{POWER,"1E-200","1E-200"},{DBM_MW,"-10000",""},{LC_CAP,"1E150","1E100"},{XL,"1E-200","1E-200"},{XC,"1E200","1E200"}};
 unsigned i,j;for(i=0;i<sizeof(over)/sizeof(over[0]);i++){App a=open_calculator(over[i].id);const char *s[]={over[i].x,over[i].y,over[i].z};for(j=0;j<calculations[a.calc].count;j++){unit(&a,j,0);edit(&a,j,s[j]);}calculate_key(&a);CHECK(a.screen==S_FORM&&a.error==RANGE&&!a.result_valid);}
 for(i=0;i<sizeof(under)/sizeof(under[0]);i++){App a=open_calculator(under[i].id);const char *s[]={under[i].x,under[i].y};for(j=0;j<calculations[a.calc].count;j++){unit(&a,j,0);edit(&a,j,s[j]);}calculate_key(&a);CHECK(a.screen==S_FORM&&a.error==RANGE&&!a.result_valid);}
 /* Output boundary conversion must also reject an unrepresentable display. */
 {App a=open_calculator(CURRENT);edit(&a,0,"1E308");edit(&a,1,"1");unit(&a,2,2);calculate_key(&a);CHECK(a.screen==S_FORM&&a.error==RANGE&&!a.result_valid);}
 {App a=open_calculator(RESISTANCE);edit(&a,0,"1E-300");edit(&a,1,"1E20");unit(&a,2,2);calculate_key(&a);CHECK(a.screen==S_FORM&&a.error==RANGE&&!a.result_valid);}
 /* Log outputs and ideal LC frequency remain finite at these extremes. */
 {App a=open_calculator(MW_DBM);unit(&a,0,0);edit(&a,0,"1E-300");result(&a,-2970);}
 {App a=open_calculator(FSPL);edit(&a,0,"1E300");edit(&a,1,"1E300");result(&a,11852.447783221883);}
 {App a=open_calculator(LC_FREQ);edit(&a,0,"1E308");edit(&a,1,"1E308");result(&a,1.5915494309189535e-309);}
}
int main(void){all_calculators();acceptance();editor_and_signed();numeric_extremes();printf("PASS: %u interactive key-event checks across all 14 calculators\n",checks);return 0;}
