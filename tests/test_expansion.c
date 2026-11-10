#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "ui.h"
#include "display.h"
#include "handbook.h"
#include "math_layout.h"
#include "settings.h"
#include "phase3e1_vectors.h"
static unsigned checks,frames,formula_count,list_fallbacks;static unsigned char visited[APP_MENU_COUNT];static uint64_t calc_seen,book_seen;
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"EXPANSION FAIL line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static void near(double a,double b){CHECK(isfinite(a));if(!(fabs(a-b)<=2e-10*fmax(fabs(b),1e-300)))fprintf(stderr,"reference mismatch got %.17g expected %.17g\n",a,b);CHECK(fabs(a-b)<=2e-10*fmax(fabs(b),1e-300));}
static void draw(App *a){App b=*a;app_draw(a);CHECK(!memcmp(a,&b,sizeof(b)));if(display_clipped_pixels||display_substitutions)fprintf(stderr,"drawing id=%d screen=%d page=%u clipped=%u substituted=%u\n",a->calc,a->screen,a->info_page,display_clipped_pixels,display_substitutions);CHECK(!display_clipped_pixels&&!display_substitutions);frames++;}
static void key(App *a,int k){app_key(a,k);draw(a);}
static void digit(App *a,unsigned n){key(a,K_DIGIT0+(n==10?0:(int)n));}
static void type(App *a,const char *s){for(;*s;s++){int k=K_NONE;if(*s>='0'&&*s<='9')k=K_DIGIT0+*s-'0';else if(*s=='.')k=K_DOT;else if(*s=='E'||*s=='e')k=K_EXP;else if(*s=='-')k=K_SIGN;else if(*s=='+')k=K_PLUS;CHECK(k!=K_NONE);key(a,k);}}
static App open_case(const ExpansionCase *v,int mode,Language l){App a;unsigned i;app_init(&a);loc_set(l);key(&a,K_ENTER);for(i=0;i<3;i++)digit(&a,v->path[i]);CHECK(a.calc==v->id&&a.screen==S_FORM);if(mode){key(&a,K_MODE);if(a.input_mode==INPUT_FORMULA)formula_count++;else{CHECK(a.mode_fallback);list_fallbacks++;}}return a;}
static void focus(App *a,unsigned row){unsigned n=0;if(a->screen==S_RESULT)key(a,K_ESC);CHECK(a->screen==S_FORM||a->screen==S_FORMULA);if(a->input_mode==INPUT_FORMULA){CHECK(row<calculations[a->calc].count);while(a->row!=row&&n++<8)key(a,K_TAB);}else while(a->row!=row&&n++<8)key(a,K_DOWN);CHECK(a->row==row);}
static void edit(App *a,unsigned row,const char *s){char normalized[32];unsigned j;snprintf(normalized,sizeof(normalized),"%s",s);for(j=0;normalized[j];j++)if(normalized[j]=='e')normalized[j]='E';{char *exponent=strchr(normalized,'E');if(exponent){exponent++;if(*exponent=='+'||*exponent=='-')exponent++;while(exponent[0]=='0'&&exponent[1]>='0'&&exponent[1]<='9')memmove(exponent,exponent+1,strlen(exponent));}}s=normalized;focus(a,row);key(a,K_ENTER);CHECK(a->screen==S_EDIT);key(a,K_CLEAR);type(a,s);key(a,K_ENTER);if(a->screen==S_EDIT)fprintf(stderr,"edit rejected calc=%d field=%u text=%s err=%d\n",a->calc,row,s,a->input_error);CHECK(a->screen==(a->input_mode==INPUT_FORMULA?S_FORMULA:S_FORM));CHECK(!strcmp(a->values[row],s));}
static void unit(App *a,unsigned row,unsigned u){unsigned count=calculations[a->calc].count;if(a->screen==S_RESULT)key(a,K_ESC);if(row==count){key(a,K_OUTPUT_UNITS);}else{focus(a,row);key(a,a->input_mode==INPUT_FORMULA?K_UNITS:K_RIGHT);}CHECK(a->screen==S_UNITS);digit(a,u+1);CHECK(a->screen==(a->input_mode==INPUT_FORMULA?S_FORMULA:S_FORM));}
static void result(App *a,double ref){key(a,K_CALCULATE);if(!(a->screen==S_RESULT&&a->result_valid))fprintf(stderr,"result failed id=%d status=%d input=%d\n",a->calc,a->error,a->input_error);if(!a->result_valid)fprintf(stderr,"values=%s %s %s %s units=%u %u %u %u mode=%d\n",a->values[0],a->values[1],a->values[2],a->values[3],a->units[0],a->units[1],a->units[2],a->units[3],a->input_mode);CHECK(a->screen==S_RESULT&&a->result_valid&&a->error==OK);near(a->result,ref);}
static void info_return(App *a){App b=*a;key(a,K_INFO);CHECK(a->screen==S_INFO&&a->calc==b.calc);digit(a,8);key(a,K_ESC);CHECK(a->screen==b.screen&&a->row==b.row&&a->input_mode==b.input_mode);CHECK(!memcmp(a->values,b.values,sizeof(b.values))&&!memcmp(a->units,b.units,sizeof(b.units)));CHECK(a->result==b.result&&a->result_valid==b.result_valid);}
static void language(App *a,Language l){App b=*a;if(l==LANG_TR)l=LANG_TR_ASCII;key(a,K_SETTINGS);key(a,K_ENTER);digit(a,l==LANG_EN?1:2);key(a,K_ESC);key(a,K_ESC);CHECK(loc_get()==l&&a->screen==b.screen);CHECK(!memcmp(a->values,b.values,sizeof(b.values))&&!memcmp(a->units,b.units,sizeof(b.units)));CHECK(a->result==b.result&&a->result_valid==b.result_valid&&a->input_mode==b.input_mode);}
static void shot(App *a,const char *tag){FILE *f;char path[180];unsigned i;draw(a);snprintf(path,sizeof(path),"build/phase3e1-%02d-%s.pgm",a->calc,tag);f=fopen(path,"wb");CHECK(f!=0);fprintf(f,"P5\n320 240\n255\n");for(i=0;i<76800;i++)fputc(framebuffer[i]?0:255,f);CHECK(!fclose(f));}
static void menus(App a){unsigned n=0,i;App b=a;uint64_t local=0;if(visited[a.menu])return;visited[a.menu]=1;do{n++;app_key(&b,K_DOWN);}while(b.selected!=a.selected&&n<=10);CHECK(n>0&&n<=10);for(i=1;i<=n;i++){App d=a;digit(&d,i);if(d.screen==S_FORM&&d.calc<41){CHECK(!(local&(UINT64_C(1)<<d.calc)));local|=UINT64_C(1)<<d.calc;calc_seen|=UINT64_C(1)<<d.calc;}else if(d.screen==S_INFO&&d.calc<41){CHECK(!(local&(UINT64_C(1)<<d.calc)));local|=UINT64_C(1)<<d.calc;book_seen|=UINT64_C(1)<<d.calc;}else if(d.screen==S_MENU)menus(d);}}
static void all_flows(void){unsigned n,l,mode,i;CHECK(CALC_COUNT>=41&&CALC_MAX_INPUTS>=4);CHECK(sizeof(expansion_cases)/sizeof(expansion_cases[0])==26);
 for(n=0;n<26;n++)for(l=0;l<3;l++)for(mode=0;mode<2;mode++){
  const ExpansionCase *v=&expansion_cases[n];const Calculation *c=&calculations[v->id];App a=open_case(v,mode,(Language)l);double shown;
  CHECK(c->count==v->count);key(&a,K_CALCULATE);CHECK(!a.result_valid&&a.input_error==NUM_EMPTY&&a.error_field==0);
  for(i=0;i<v->count;i++){if(i){key(&a,K_CALCULATE);CHECK(a.input_error==NUM_EMPTY&&a.error_field==i&&!a.result_valid);}unit(&a,i,v->units[i]);edit(&a,i,v->entered[i]);}
  unit(&a,v->count,v->output);if(l==LANG_EN&&!mode){shot(&a,"list-inputs-en");if(c->count==4){focus(&a,0);shot(&a,"list-first-fields-en");focus(&a,3);shot(&a,"list-last-fields-en");}}result(&a,v->ref);CHECK(convert_output(a.result,c->outputs[a.output_unit].scale,&shown)==OK);near(shown,v->shown);info_return(&a);
  if(l==LANG_EN){if(mode)shot(&a,"formula-result-en");else shot(&a,"list-result-en");}
  if(l==LANG_TR&&mode)shot(&a,"formula-result-tr");
  if(l==LANG_TR_ASCII&&mode)shot(&a,"formula-result-ascii");
  {App b=a;key(&a,K_MODE);CHECK(!memcmp(a.values,b.values,sizeof(a.values))&&!memcmp(a.units,b.units,sizeof(a.units)));CHECK(a.result_valid&&a.result==b.result);key(&a,K_MODE);}
  for(i=0;i<v->count;i++){
   edit(&a,i,v->changed[i]);result(&a,v->changed_ref[i]);CHECK(a.result!=v->ref);edit(&a,i,v->entered[i]);result(&a,v->ref);
   focus(&a,i);key(&a,K_ENTER);key(&a,K_CLEAR);key(&a,K_ENTER);CHECK(a.screen==S_EDIT&&a.input_error==NUM_EMPTY);info_return(&a);key(&a,K_ESC);
   focus(&a,i);key(&a,K_ENTER);key(&a,K_CLEAR);type(&a,"1E");key(&a,K_ENTER);CHECK(a.screen==S_EDIT&&a.input_error==NUM_FORMAT);key(&a,K_ESC);
   focus(&a,i);key(&a,K_ENTER);key(&a,K_CLEAR);type(&a,"1E9999");key(&a,K_ENTER);CHECK(a.screen==S_EDIT&&a.input_error==NUM_RANGE);key(&a,K_ESC);
  }
  language(&a,LANG_EN);language(&a,LANG_TR);language(&a,LANG_TR_ASCII);
  /* Verify each unit boundary using key-entered values, not direct field assignment. */
  if(l==LANG_EN&&mode==0){for(i=0;i<c->count;i++){unsigned u;for(u=0;u<c->inputs[i].count;u++){char txt[32];snprintf(txt,sizeof(txt),"%.17g",v->si[i]/c->inputs[i].units[u].scale);unit(&a,i,u);edit(&a,i,txt);result(&a,v->ref);}unit(&a,i,v->units[i]);edit(&a,i,v->entered[i]);}
   for(i=0;i<c->output_count;i++){unit(&a,c->count,i);result(&a,v->ref);CHECK(convert_output(a.result,c->outputs[i].scale,&shown)==OK);near(shown*c->outputs[i].scale,v->ref);}
  }
 }
}
static void handbook_and_math(void){unsigned n,l,p,i;for(n=0;n<26;n++){
 const ExpansionCase *v=&expansion_cases[n];const Calculation *c=&calculations[v->id];const HandbookEntry *h=&handbook[v->id];double got;MathArena arena;const MathNode *node;MathBox box;
 CHECK(loc_has(c->name)&&loc_has(c->definitions)&&loc_has(c->usage));CHECK(loc_has(h->purpose)&&loc_has(h->measurements)&&loc_has(h->laboratory)&&loc_has(h->limits)&&loc_has(h->example)&&loc_has(h->mistakes));
 CHECK(calculate(v->id,h->example_inputs,&got)==OK);near(got,v->ref);near(h->example_result,v->ref);
 math_arena_init(&arena);node=formula_build(&arena,v->id);box=math_measure(node);CHECK(node&&!arena.overflow);if(!(box.width<=296&&box.above+box.below<=128))fprintf(stderr,"formula too large id=%d width=%d height=%d\n",v->id,box.width,box.above+box.below);CHECK(box.width<=296&&box.above+box.below<=128);
 if(formula_interactive_fits(v->id,310,58)){MathInteraction f;unsigned seen=0,outputs=0;clear_screen();CHECK(formula_interactive(v->id,calc_input_id(v->id,0),5,68,310,58,&f));for(i=0;i<f.count;i++){MathFieldRect *r=&f.fields[i];unsigned j;CHECK(r->x>=5&&r->x+r->width<=315&&r->y>=68&&r->y+r->height<=126);if(r->role==M_OUTPUT)outputs++;else for(j=0;j<c->count;j++)if(r->variable==calc_input_id(v->id,j))seen|=1u<<j;}CHECK(outputs==1&&seen==((1u<<c->count)-1));}
 for(l=0;l<3;l++){App a;app_init(&a);loc_set((Language)l);key(&a,K_ENTER);digit(&a,10);digit(&a,v->book[0]);digit(&a,v->book[1]);CHECK(a.screen==S_INFO&&a.calc==v->id&&a.handbook_origin);
  for(p=0;p<8;p++){unsigned lines;digit(&a,p+1);lines=app_info_lines(&a);for(i=0;i<lines+2;i++)key(&a,K_DOWN);CHECK(a.info_scroll==(lines>11?lines-11:0));if(p==6&&l==1)shot(&a,"handbook-example-tr");}
  key(&a,K_ENTER);CHECK(a.screen==S_FORM&&a.calc==v->id);info_return(&a);
 }
 }}
static void boundaries(void){unsigned n,i;double x[4],r,rx,z,phi;for(n=0;n<26;n++){const ExpansionCase *v=&expansion_cases[n];for(i=0;i<v->count;i++){memcpy(x,v->si,sizeof(x));x[i]=NAN;CHECK(calculate(v->id,x,&r)==INVALID);x[i]=INFINITY;CHECK(calculate(v->id,x,&r)==INVALID);}}
#define EXPECT(id,a,b,c,d,s) do{x[0]=a;x[1]=b;x[2]=c;x[3]=d;CHECK(calculate(id,x,&r)==s);}while(0)
 EXPECT(LED_RESISTOR,2,2,.02,0,INVALID);EXPECT(LED_RESISTOR,1,2,.02,0,INVALID);EXPECT(LED_RESISTOR,5,2,0,0,INVALID);
 EXPECT(LDO_LOSS,3,5,.1,0,INVALID);EXPECT(DCDC_EFF,11,10,0,0,INVALID);EXPECT(DCDC_IN,1,0,0,0,INVALID);EXPECT(DCDC_OUT,1,1.01,0,0,INVALID);EXPECT(BAT_RUNTIME,1,1,1.01,0,INVALID);EXPECT(DUTY_POWER,1,1,-.01,0,INVALID);EXPECT(RES_RATING,1,0,0,0,INVALID);
 EXPECT(POWER_IR,-.02,1000,0,0,OK);near(r,.4);EXPECT(POWER_VR,-12,100,0,0,OK);near(r,1.44);EXPECT(R_SERIES,0,0,0,0,OK);CHECK(r==0);EXPECT(R_SERIES,-1,2,0,0,INVALID);EXPECT(R_PARALLEL,0,1,0,0,INVALID);
 EXPECT(KVL_REMAINDER,1,2,3,0,OK);near(r,-4);EXPECT(KCL_REMAINDER,1,2,3,0,OK);near(r,-4);EXPECT(KVL_REMAINDER,DBL_MAX,DBL_MAX,DBL_MAX,0,OK);near(r,-DBL_MAX);
 EXPECT(RC_CHARGE,5,1e3,1e-3,0,OK);CHECK(r==0);EXPECT(RC_DISCHARGE,-5,1e3,1e-3,0,OK);near(r,-5);EXPECT(RC_CHARGE,5,1,1,1e-12,OK);near(r,4.9999999999975e-12);EXPECT(RC_DISCHARGE,5,1,1,1e6,RANGE);EXPECT(RC_CHARGE,5,1,1,1e6,OK);near(r,5);
 EXPECT(RC_CHARGE,1e300,1e100,1e100,1e-100,OK);near(r,1);EXPECT(R_PARALLEL,DBL_MAX,DBL_MAX,0,0,OK);near(r,DBL_MAX/2);EXPECT(V_DIVIDER,12,1e308,1e308,0,OK);near(r,6);EXPECT(CAP_ENERGY,1e-300,1e150,0,0,OK);near(r,.5);
 EXPECT(RC_CHARGE,0,1,1,1,OK);CHECK(r==0);EXPECT(RC_DISCHARGE,0,1,1,1,OK);CHECK(r==0);EXPECT(RC_TAU,1e300,1e300,0,0,RANGE);EXPECT(RC_TAU,1e-300,1e-300,0,0,RANGE);EXPECT(RL_TAU,1e-300,1e300,0,0,RANGE);
 EXPECT(CAP_ENERGY,1e300,1e300,0,0,RANGE);EXPECT(POWER_IR,1e300,1,0,0,RANGE);EXPECT(POWER_VR,1e-300,1,0,0,RANGE);EXPECT(AVG_CURRENT,1,0,0,0,INVALID);
 EXPECT(DUTY_POWER,0,0,0,0,OK);CHECK(r==0);EXPECT(DUTY_POWER,DBL_MAX,DBL_MAX,.5,0,OK);near(r,DBL_MAX);EXPECT(DCDC_EFF,0,1,0,0,OK);CHECK(r==0);EXPECT(LDO_LOSS,5,5,.1,0,OK);CHECK(r==0);
 EXPECT(AH_WH,7200,3.7,0,0,OK);x[0]=r;x[1]=3.7;CHECK(calculate(WH_AH,x,&r)==OK);near(r,7200);EXPECT(DCDC_IN,9,.9,0,0,OK);x[0]=r;CHECK(calculate(DCDC_OUT,x,&r)==OK);near(r,9);
 x[0]=100;x[1]=.01;x[2]=1e-6;x[3]=1000;CHECK(series_rlc(x,&rx,&z,&phi)==OK);near(rx,-96.323090020099465);near(z,138.845733355477);CHECK(phi<0);x[0]=0;x[1]=0;CHECK(series_rlc(x,&rx,&z,&phi)==INVALID);
}
static void inverse_ui(void){unsigned n,i;for(n=0;n<3;n++){
 unsigned index=n==0?12:n==1?16:22;const ExpansionCase *v=&expansion_cases[index];App a=open_case(v,1,LANG_TR);double original;
 for(i=0;i<v->count;i++){unit(&a,i,v->units[i]);edit(&a,i,v->entered[i]);}unit(&a,v->count,v->output);result(&a,v->ref);original=a.result;
 key(&a,K_TARGET);CHECK(a.screen==S_TARGET);digit(&a,2);CHECK(!a.result_valid);CHECK(a.calc==(n==0?RLC_PHASE:n==1?WH_AH:DCDC_OUT));result(&a,expansion_cases[n==0?13:n==1?17:23].ref);
 if(n==2){key(&a,K_TARGET);digit(&a,3);CHECK(a.calc==DCDC_EFF);result(&a,.9);}
 key(&a,K_TARGET);digit(&a,1);CHECK(a.calc==v->id);result(&a,original);info_return(&a);
 }}
static void invalid_ui(void){unsigned n,i;const unsigned indexes[]={8,21,24};
 for(n=0;n<3;n++){const ExpansionCase *v=&expansion_cases[indexes[n]];App a=open_case(v,1,LANG_TR);
  for(i=0;i<v->count;i++){unit(&a,i,v->units[i]);edit(&a,i,v->entered[i]);}result(&a,v->ref);
  edit(&a,n==2?0:1,n==2?"11":"6");key(&a,K_CALCULATE);CHECK(!a.result_valid&&a.error==INVALID&&a.input_error==NUM_DOMAIN);CHECK(a.error_field==v->count);info_return(&a);shot(&a,"invalid-related-inputs-tr");
 }
 {const ExpansionCase *v=&expansion_cases[19];App a=open_case(v,1,LANG_EN);for(i=0;i<v->count;i++){unit(&a,i,v->units[i]);edit(&a,i,v->entered[i]);}result(&a,v->ref);focus(&a,2);key(&a,K_ENTER);key(&a,K_CLEAR);type(&a,"101");key(&a,K_ENTER);CHECK(a.screen==S_EDIT&&a.input_error==NUM_DOMAIN);info_return(&a);key(&a,K_ESC);CHECK(!strcmp(a.values[2],v->entered[2]));}
}
int main(void){App a;settings_bind(0);app_init(&a);key(&a,K_ENTER);menus(a);{unsigned menu;for(menu=0;menu<APP_MENU_COUNT;menu++)CHECK(visited[menu]);}CHECK(calc_seen==((UINT64_C(1)<<41)-1)&&book_seen==calc_seen);boundaries();all_flows();handbook_and_math();inverse_ui();invalid_ui();printf("PASS: %u Phase 3E-1 expansion checks (%u frames, 26 new calculators, %u Formula Mode flows, %u List fallbacks)\n",checks,frames,formula_count,list_fallbacks);return 0;}
