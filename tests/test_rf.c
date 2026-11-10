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
#include "phase3e2_vectors.h"
#include "rf_engine.h"
static unsigned checks,frames,formula_count,list_fallbacks;static unsigned char visited[APP_MENU_COUNT];static unsigned char calc_seen[CALC_COUNT],book_seen[CALC_COUNT];
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"EXPANSION FAIL line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static void near(double a,double b){CHECK(isfinite(a));if(!(fabs(a-b)<=2e-10*fmax(fabs(b),1e-300)))fprintf(stderr,"reference mismatch got %.17g expected %.17g\n",a,b);CHECK(fabs(a-b)<=2e-10*fmax(fabs(b),1e-300));}
static void draw(App *a){App b=*a;app_draw(a);CHECK(!memcmp(a,&b,sizeof(b)));if(display_clipped_pixels||display_substitutions)fprintf(stderr,"drawing id=%d screen=%d page=%u clipped=%u substituted=%u\n",a->calc,a->screen,a->info_page,display_clipped_pixels,display_substitutions);CHECK(!display_clipped_pixels&&!display_substitutions);frames++;}
static void key(App *a,int k){app_key(a,k);draw(a);}
static void digit(App *a,unsigned n){key(a,K_DIGIT0+(n==10?0:(int)n));}
static void type(App *a,const char *s){for(;*s;s++){int k=K_NONE;if(*s>='0'&&*s<='9')k=K_DIGIT0+*s-'0';else if(*s=='.')k=K_DOT;else if(*s=='E'||*s=='e')k=K_EXP;else if(*s=='-')k=K_SIGN;else if(*s=='+')k=K_PLUS;CHECK(k!=K_NONE);key(a,k);}}
static App open_case(const RFCase *v,int mode,Language l){App a;unsigned i;app_init(&a);loc_set(l);key(&a,K_ENTER);for(i=0;i<3;i++)digit(&a,v->path[i]);CHECK(a.calc==v->id&&a.screen==S_FORM);if(mode){key(&a,K_MODE);if(a.input_mode==INPUT_FORMULA)formula_count++;else{CHECK(a.mode_fallback);list_fallbacks++;}}return a;}
static void focus(App *a,unsigned row){unsigned n=0;if(a->screen==S_RESULT)key(a,K_ESC);CHECK(a->screen==S_FORM||a->screen==S_FORMULA);if(a->input_mode==INPUT_FORMULA){CHECK(row<calculations[a->calc].count);while(a->row!=row&&n++<8)key(a,K_TAB);}else while(a->row!=row&&n++<8)key(a,K_DOWN);CHECK(a->row==row);}
static void edit(App *a,unsigned row,const char *s){char normalized[32];unsigned j;snprintf(normalized,sizeof(normalized),"%s",s);for(j=0;normalized[j];j++)if(normalized[j]=='e')normalized[j]='E';{char *exponent=strchr(normalized,'E');if(exponent){exponent++;if(*exponent=='+'||*exponent=='-')exponent++;while(exponent[0]=='0'&&exponent[1]>='0'&&exponent[1]<='9')memmove(exponent,exponent+1,strlen(exponent));}}s=normalized;focus(a,row);key(a,K_ENTER);CHECK(a->screen==S_EDIT);key(a,K_CLEAR);type(a,s);key(a,K_ENTER);if(a->screen==S_EDIT)fprintf(stderr,"edit rejected calc=%d field=%u text=%s err=%d\n",a->calc,row,s,a->input_error);CHECK(a->screen==(a->input_mode==INPUT_FORMULA?S_FORMULA:S_FORM));CHECK(!strcmp(a->values[row],s));}
static void unit(App *a,unsigned row,unsigned u){unsigned count=calculations[a->calc].count;if(a->screen==S_RESULT)key(a,K_ESC);if(row==count){key(a,K_OUTPUT_UNITS);}else{focus(a,row);key(a,a->input_mode==INPUT_FORMULA?K_UNITS:K_RIGHT);}CHECK(a->screen==S_UNITS);digit(a,u+1);CHECK(a->screen==(a->input_mode==INPUT_FORMULA?S_FORMULA:S_FORM));}
static void result(App *a,double ref){key(a,K_CALCULATE);if(!(a->screen==S_RESULT&&a->result_valid))fprintf(stderr,"result failed id=%d status=%d input=%d\n",a->calc,a->error,a->input_error);if(!a->result_valid)fprintf(stderr,"values=%s %s %s %s units=%u %u %u %u mode=%d\n",a->values[0],a->values[1],a->values[2],a->values[3],a->units[0],a->units[1],a->units[2],a->units[3],a->input_mode);CHECK(a->screen==S_RESULT&&a->result_valid&&a->error==OK);near(a->result,ref);}
static void info_return(App *a){App b=*a;key(a,K_INFO);CHECK(a->screen==S_INFO&&a->calc==b.calc);digit(a,8);key(a,K_ESC);CHECK(a->screen==b.screen&&a->row==b.row&&a->input_mode==b.input_mode);CHECK(!memcmp(a->values,b.values,sizeof(b.values))&&!memcmp(a->units,b.units,sizeof(b.units)));CHECK(a->result==b.result&&a->result_valid==b.result_valid);}
static void language(App *a,Language l){App b=*a;if(l==LANG_TR)l=LANG_TR_ASCII;key(a,K_SETTINGS);key(a,K_ENTER);digit(a,l==LANG_EN?1:2);key(a,K_ESC);key(a,K_ESC);CHECK(loc_get()==l&&a->screen==b.screen);CHECK(!memcmp(a->values,b.values,sizeof(b.values))&&!memcmp(a->units,b.units,sizeof(b.units)));CHECK(a->result==b.result&&a->result_valid==b.result_valid&&a->input_mode==b.input_mode);}
static void shot(App *a,const char *tag){FILE *f;char path[180];unsigned i;draw(a);snprintf(path,sizeof(path),"build/phase3e2-%02d-%s.pgm",a->calc,tag);f=fopen(path,"wb");CHECK(f!=0);fprintf(f,"P5\n320 240\n255\n");for(i=0;i<76800;i++)fputc(framebuffer[i]?0:255,f);CHECK(!fclose(f));}

static void menus(App a){unsigned n=0,i;App b=a;unsigned char local[CALC_COUNT]={0};if(visited[a.menu])return;visited[a.menu]=1;do{n++;app_key(&b,K_DOWN);}while(b.selected!=a.selected&&n<=10);CHECK(n>0&&n<=10);draw(&a);for(i=1;i<=n;i++){App d=a;digit(&d,i);if(d.screen==S_FORM||d.screen==S_INFO){CHECK((unsigned)d.calc<CALC_COUNT&&!local[d.calc]);local[d.calc]=1;if(d.screen==S_FORM)calc_seen[d.calc]=1;else book_seen[d.calc]=1;}else if(d.screen==S_MENU)menus(d);}}
static void flow(void){unsigned n,l,mode,i,u;char text[32];
 for(n=0;n<47;n++)for(l=0;l<3;l++)for(mode=0;mode<2;mode++){
  const RFCase *v=&rf_cases[n];const Calculation *c=&calculations[v->id];App a=open_case(v,mode,(Language)l);App old;
  CHECK(c->count==v->count);key(&a,K_CALCULATE);CHECK(!a.result_valid&&a.input_error==NUM_EMPTY&&a.error_field==0);
  for(i=0;i<c->count;i++){if(i){key(&a,K_CALCULATE);CHECK(!a.result_valid&&a.input_error==NUM_EMPTY&&a.error_field==i);}unit(&a,i,v->units[i]);edit(&a,i,v->entered[i]);}
  unit(&a,c->count,v->output);result(&a,v->ref);info_return(&a);
  if(l==LANG_EN&&!mode){shot(&a,"result-en");key(&a,K_ESC);shot(&a,"inputs-en");result(&a,v->ref);}
  if(l==LANG_TR&&!mode)shot(&a,"result-tr");
  for(i=0;i<c->count;i++){edit(&a,i,v->changed[i]);result(&a,v->changed_ref[i]);CHECK(fabs(a.result-v->ref)>1e-14*fmax(fabs(v->ref),1e-300));info_return(&a);edit(&a,i,v->entered[i]);result(&a,v->ref);}
  old=a;key(&a,K_MODE);CHECK(!memcmp(a.values,old.values,sizeof(a.values))&&!memcmp(a.units,old.units,sizeof(a.units)));CHECK(a.result_valid&&a.result==old.result);key(&a,K_MODE);CHECK(!memcmp(a.values,old.values,sizeof(a.values)));result(&a,v->ref);
  language(&a,LANG_TR_ASCII);language(&a,LANG_TR);language(&a,LANG_EN);language(&a,(Language)l);
  /* Cursor correction, backspace, clear and cancel all travel through real keys. */
  focus(&a,0);key(&a,K_ENTER);key(&a,K_LEFT);key(&a,K_DELETE);key(&a,K_CLEAR);type(&a,v->entered[0]);key(&a,K_ENTER);result(&a,v->ref);
  if(l==LANG_EN&&!mode){
   for(i=0;i<c->count;i++)for(u=0;u<c->inputs[i].count;u++){
    double shown;CHECK(convert_output(v->si[i],c->inputs[i].units[u].scale,&shown)==OK);snprintf(text,sizeof(text),"%.15g",shown);unit(&a,i,u);edit(&a,i,text);result(&a,v->ref);
   }
   for(u=0;u<c->output_count;u++){unit(&a,c->count,u);result(&a,v->ref);}
  }
 }
}
static void book_math(void){unsigned n,l,p,i;for(n=0;n<47;n++){
 const RFCase *v=&rf_cases[n];const Calculation *c=&calculations[v->id];const HandbookEntry *h=&handbook[v->id];MathArena arena;MathBox box;const MathNode *tree;
 math_arena_init(&arena);tree=formula_build(&arena,v->id);box=math_measure(tree);if(!(tree&&!arena.overflow&&box.width<=296&&box.above+box.below<=87))fprintf(stderr,"math id=%d nodes=%u overflow=%d size=%dx%d\n",v->id,arena.used,arena.overflow,box.width,box.above+box.below);CHECK(tree&&!arena.overflow&&box.width<=296&&box.above+box.below<=87);near(h->example_result,v->ref);
 for(i=0;i<c->count;i++)near(h->example_inputs[i],v->si[i]);
 for(l=0;l<3;l++){App a=open_case(v,0,(Language)l);key(&a,K_INFO);
  CHECK(loc_has(c->name)&&loc_has(c->definitions)&&loc_has(c->usage));CHECK(loc_has(h->purpose)&&loc_has(h->measurements)&&loc_has(h->laboratory)&&loc_has(h->limits)&&loc_has(h->example)&&loc_has(h->mistakes));
  for(p=0;p<8;p++){unsigned lines;digit(&a,p+1);lines=app_info_lines(&a);for(i=0;i<lines+2;i++)key(&a,K_DOWN);CHECK(a.info_scroll==(lines>11?lines-11:0));if(p==6&&l==1)shot(&a,"handbook-example-tr");}
  key(&a,K_ESC);CHECK(a.screen==S_FORM&&a.calc==v->id);
 }
}}
static void invalid(void){unsigned n,i;double x[6],r;for(n=0;n<47;n++){
 const RFCase *v=&rf_cases[n];App a=open_case(v,0,LANG_EN);
 for(i=0;i<v->count;i++){unit(&a,i,v->units[i]);edit(&a,i,v->entered[i]);}result(&a,v->ref);
 for(i=0;i<v->count;i++){
  memcpy(x,v->si,sizeof(x));x[i]=NAN;CHECK(calculate(v->id,x,&r)==INVALID);x[i]=INFINITY;CHECK(calculate(v->id,x,&r)==INVALID);
  if(!parameter_valid(v->id,i,0)){focus(&a,i);key(&a,K_ENTER);key(&a,K_CLEAR);type(&a,"0");key(&a,K_ENTER);CHECK(a.screen==S_EDIT&&a.input_error==NUM_DOMAIN);info_return(&a);key(&a,K_ESC);result(&a,v->ref);}
  memcpy(x,v->si,sizeof(x));x[i]=-1;if(!parameter_valid(v->id,i,-1))CHECK(calculate(v->id,x,&r)==INVALID);
  x[i]=1e308;{Status st=calculate(v->id,x,&r);CHECK(st==OK||st==INVALID||st==RANGE);if(st==OK)CHECK(isfinite(r));}
  x[i]=1e-308;{Status st=calculate(v->id,x,&r);CHECK(st==OK||st==INVALID||st==RANGE);if(st==OK)CHECK(isfinite(r));}
 }
 focus(&a,0);key(&a,K_ENTER);key(&a,K_CLEAR);type(&a,"1E999");key(&a,K_ENTER);CHECK(a.screen==S_EDIT&&a.input_error==NUM_RANGE);key(&a,K_ESC);result(&a,v->ref);
 key(&a,K_ENTER); /* result opens contextual handbook in List Mode */
 if(a.screen==S_INFO)key(&a,K_ESC);
 }}
static void boundaries(void){double r,x[6]={0},re,im,L,C,w,Q;unsigned hp,reverse;
#define E(id,a,b,c,st) do{x[0]=a;x[1]=b;x[2]=c;CHECK(calculate(id,x,&r)==st);}while(0)
 E(RF_DBW_DBM,0,0,0,OK);near(r,30);E(RF_DBM_DBW,20,0,0,OK);near(r,-10);
 E(RF_GAIN_DB,.1,0,0,OK);near(r,-10);E(RF_GAIN_RATIO,-10,0,0,OK);near(r,.1);E(RF_AMP_DB,.5,0,0,OK);near(r,-6.020599913279624);
 E(RF_GAIN_RATIO,1e308,0,0,RANGE);E(RF_GAIN_RATIO,-1e308,0,0,RANGE);E(RF_NOISE_SNR,1e-300,1e300,0,RANGE);E(RF_POWER_RATIO,1e300,1e-300,0,RANGE);E(RF_CLEAR,1e-300,1e300,0,RANGE);
 E(RF_NOISE_DBM,290,125000,6,OK);near(r,-117.00608706414749);E(RF_NOISE_W,300,1,0,OK);near(r,4.141947e-21);E(RF_NOISE_W,0,1,0,INVALID);E(RF_NOISE_W,290,125000,-1,INVALID);
 E(RF_NOISE_DBM,290,1e308,1e308,OK);CHECK(isfinite(r));E(RF_NOISE_W,290,1e308,1e308,RANGE);
 E(RF_REFLECT,100,50,50,OK);near(r,.4472135954999579);CHECK(rf_reflection(x,&re,&im)==OK);near(re,.4);near(im,.2);
 E(RF_IMPEDANCE,.4,.2,50,OK);near(r,100);CHECK(rf_impedance(x,&re,&im)==OK);near(re,100);near(im,50);
 E(RF_IMPEDANCE,.8,.8,50,INVALID);E(RF_VSWR,1,0,0,RANGE);E(RF_RETURN,0,0,0,RANGE);E(RF_RETURN,1,0,0,OK);CHECK(r==0);E(RF_VSWR,0,0,0,OK);near(r,1);
 E(RF_REFLECT,50,0,50,OK);CHECK(r==0);E(RF_REFLECT,0,0,50,OK);near(r,1);E(RF_POL,0,0,0,OK);CHECK(r==0);E(RF_POL,1.5707963267948966,0,0,RANGE);E(RF_POL,2,0,0,INVALID);
 E(RF_HORIZON,100000,1,1,INVALID);E(RF_HORIZON,0,0,1,OK);CHECK(r==0);E(RF_HORIZON,1,1,0,INVALID);E(RF_SPEED,.9,0,0,INVALID);
 E(RF_LP,0,1,0,OK);near(r,1);E(RF_HP,0,1,0,OK);CHECK(r==0);E(RF_LP,1,0,0,INVALID);E(RF_LMATCH_LP,50,50,868e6,INVALID);
 x[0]=2;x[1]=4;x[2]=6;x[3]=10;x[4]=10;CHECK(calculate(RF_FRIIS_F,x,&r)==OK);near(r,2.35);CHECK(calculate(RF_FRIIS_NF,x,&r)==OK);near(r,3.7106786227173623);
 x[3]=1e-300;x[4]=1e-300;CHECK(calculate(RF_FRIIS_F,x,&r)==RANGE);CHECK(calculate(RF_FRIIS_NF,x,&r)==OK);near(r,6006.98970004336);
 x[0]=1;x[1]=2;x[2]=1;x[3]=1e300;x[4]=1;CHECK(calculate(RF_FRIIS_NF,x,&r)==OK);near(r,4.342944819032518e-300);
 /* Independent network reconstruction, both source/load orientations and topologies. */
 for(hp=0;hp<2;hp++)for(reverse=0;reverse<2;reverse++){
  double xs,xp,parallel_re,parallel_im,den,Zr,Zi;
  x[0]=reverse?200:50;x[1]=reverse?50:200;x[2]=868e6;CHECK(rf_lmatch(x,hp,&L,&C)==OK);w=2*3.14159265358979323846*x[2];Q=sqrt(3.0);
  if(!hp){near(L,Q*50/w);near(C,1/(w*(200/Q)));xs=w*L;xp=-1/(w*C);}else{near(L,1/(w*Q*50));near(C,200/(Q*w));xs=-1/(w*L);xp=w*C;}
  /* The high-R port has the shunt; reduce the network from the low-R side. */
  den=200*200+xp*xp;parallel_re=200*xp*xp/den;parallel_im=200*200*xp/den;Zr=parallel_re;Zi=parallel_im+xs;near(Zr,50);CHECK(fabs(Zi)<1e-11);
 }
}
static void acceptance(void){static const RFCase retained[]={
 {MW_DBM,1,{1,1,1},{1},0,{"100"},{0},{0},20,{0}},
 {MW_DBM,1,{1,1,1},{1},0,{"1000"},{0},{0},30,{0}},
 {WAVELENGTH,1,{7,1,1},{2},0,{"868"},{0},{0},0.34538301612903227,{0}},
 {QUARTER_WAVE,1,{7,1,2},{2},0,{"868"},{0},{0},0.086345754032258068,{0}},
 {FSPL,2,{7,2,1},{2,2},0,{"1","868"},{0},{0},91.218177725413213,{0}},
 {FRESNEL,3,{7,2,2},{2,2,2},0,{"868","1.5","1.5"},{0},{0},16.094634574813252,{0}},
 };unsigned n,i;for(n=0;n<6;n++){const RFCase *v=&retained[n];App a=open_case(v,0,LANG_EN);for(i=0;i<v->count;i++){unit(&a,i,v->units[i]);edit(&a,i,v->entered[i]);}result(&a,v->ref);shot(&a,"acceptance-en");}}
static void inverse_flows(void){
 const unsigned indices[]={2,4,19,15,21,45,17};const double expected[]={100,2,6,-117.00608706414754,3.7106786227173623,1,1e-15};unsigned n,i;
 for(n=0;n<7;n++){const RFCase *v=&rf_cases[indices[n]];App a=open_case(v,1,LANG_TR);for(i=0;i<v->count;i++){unit(&a,i,v->units[i]);edit(&a,i,v->entered[i]);}unit(&a,v->count,v->output);result(&a,v->ref);key(&a,K_TARGET);CHECK(a.screen==S_TARGET);digit(&a,2);CHECK(!a.result_valid);result(&a,expected[n]);info_return(&a);key(&a,K_TARGET);digit(&a,1);CHECK(a.calc==v->id);result(&a,v->ref);}
}
int main(void){App a;unsigned i;settings_bind(0);CHECK(CALC_COUNT>=88&&sizeof(rf_cases)/sizeof(rf_cases[0])==47);app_init(&a);key(&a,K_ENTER);menus(a);{unsigned menu;for(menu=0;menu<APP_MENU_COUNT;menu++)CHECK(visited[menu]);}for(i=0;i<CALC_COUNT;i++)CHECK(calc_seen[i]&&book_seen[i]);for(i=0;i<VAR_COUNT;i++){CHECK(variable_symbol((VariableId)i)&&*variable_symbol((VariableId)i));CHECK(variable_meaning((VariableId)i)&&*variable_meaning((VariableId)i));}boundaries();acceptance();book_math();flow();inverse_flows();invalid();printf("PASS: %u Phase 3E-2 RF checks (%u frames, 47 new targets, 88 reachable targets and Handbook entries, %u Formula Mode flows, %u List fallbacks)\n",checks,frames,formula_count,list_fallbacks);return 0;}
