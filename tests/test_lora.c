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
#include "phase3f_vectors.h"
#include "rf_engine.h"
#include "lab_engine.h"
#include "lora_engine.h"
#include "version.h"
#include "phase3e3_vectors.h"
#include "semtech_airtime_vectors.h"
#include "semtech_legacy_vectors.h"
static unsigned checks,frames,formula_count,list_fallbacks;static unsigned char visited[APP_MENU_COUNT];static unsigned char calc_seen[CALC_COUNT],book_seen[CALC_COUNT];
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"EXPANSION FAIL line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static void near(double a,double b){CHECK(isfinite(a));if(!(fabs(a-b)<=2e-10*fmax(fabs(b),1e-300)))fprintf(stderr,"reference mismatch got %.17g expected %.17g\n",a,b);CHECK(fabs(a-b)<=2e-10*fmax(fabs(b),1e-300));}
static void draw(App *a){App b=*a;app_draw(a);CHECK(!memcmp(a,&b,sizeof(b)));if(display_clipped_pixels||display_substitutions)fprintf(stderr,"drawing id=%d screen=%d page=%u clipped=%u substituted=%u\n",a->calc,a->screen,a->info_page,display_clipped_pixels,display_substitutions);CHECK(!display_clipped_pixels&&!display_substitutions);frames++;}
static void key(App *a,int k){app_key(a,k);draw(a);}
static void digit(App *a,unsigned n){key(a,K_DIGIT0+(n==10?0:(int)n));}
static void type(App *a,const char *s){for(;*s;s++){int k=K_NONE;if(*s>='0'&&*s<='9')k=K_DIGIT0+*s-'0';else if(*s=='.')k=K_DOT;else if(*s=='E'||*s=='e')k=K_EXP;else if(*s=='-')k=K_SIGN;else if(*s=='+')k=K_PLUS;CHECK(k!=K_NONE);key(a,k);}}
static App open_case(const LoRaCase *v,int mode,Language l){App a;unsigned i;app_init(&a);loc_set(l);key(&a,K_ENTER);for(i=0;i<3;i++)digit(&a,v->path[i]);CHECK(a.calc==v->id&&a.screen==S_FORM);if(mode){key(&a,K_MODE);if(a.input_mode==INPUT_FORMULA)formula_count++;else{CHECK(a.mode_fallback);list_fallbacks++;}}return a;}
static void focus(App *a,unsigned row){unsigned n=0;if(a->screen==S_RESULT)key(a,K_ESC);CHECK(a->screen==S_FORM||a->screen==S_FORMULA);if(a->input_mode==INPUT_FORMULA){CHECK(row<calculations[a->calc].count);while(a->row!=row&&n++<16)key(a,K_TAB);}else while(a->row!=row&&n++<16)key(a,K_DOWN);CHECK(a->row==row);if(a->input_mode==INPUT_LIST&&row<calculations[a->calc].count){unsigned y,marked=0;draw(a);for(y=53;y<159;y++)marked+=framebuffer[y*320+5]!=0;CHECK(marked==10);}}
static void edit(App *a,unsigned row,const char *s){char normalized[32];unsigned j;snprintf(normalized,sizeof(normalized),"%s",s);for(j=0;normalized[j];j++)if(normalized[j]=='e')normalized[j]='E';{char *exponent=strchr(normalized,'E');if(exponent){exponent++;if(*exponent=='+'||*exponent=='-')exponent++;while(exponent[0]=='0'&&exponent[1]>='0'&&exponent[1]<='9')memmove(exponent,exponent+1,strlen(exponent));}}s=normalized;focus(a,row);key(a,K_ENTER);CHECK(a->screen==S_EDIT);key(a,K_CLEAR);type(a,s);key(a,K_ENTER);if(a->screen==S_EDIT)fprintf(stderr,"edit rejected calc=%d field=%u text=%s err=%d\n",a->calc,row,s,a->input_error);CHECK(a->screen==(a->input_mode==INPUT_FORMULA?S_FORMULA:S_FORM));CHECK(!strcmp(a->values[row],s));}
static void unit(App *a,unsigned row,unsigned u){unsigned count=calculations[a->calc].count;if(a->screen==S_RESULT)key(a,K_ESC);if(row==count){key(a,K_OUTPUT_UNITS);}else{focus(a,row);key(a,a->input_mode==INPUT_FORMULA?K_UNITS:K_RIGHT);}CHECK(a->screen==S_UNITS);digit(a,u+1);CHECK(a->screen==(a->input_mode==INPUT_FORMULA?S_FORMULA:S_FORM));}
static void result(App *a,double ref){key(a,K_CALCULATE);if(!(a->screen==S_RESULT&&a->result_valid))fprintf(stderr,"result failed id=%d status=%d input=%d\n",a->calc,a->error,a->input_error);if(!a->result_valid)fprintf(stderr,"values=%s %s %s %s units=%u %u %u %u mode=%d\n",a->values[0],a->values[1],a->values[2],a->values[3],a->units[0],a->units[1],a->units[2],a->units[3],a->input_mode);CHECK(a->screen==S_RESULT&&a->result_valid&&a->error==OK);near(a->result,ref);}
static void info_return(App *a){App b=*a;key(a,K_INFO);CHECK(a->screen==S_INFO&&a->calc==b.calc);digit(a,8);key(a,K_ESC);CHECK(a->screen==b.screen&&a->row==b.row&&a->input_mode==b.input_mode);CHECK(!memcmp(a->values,b.values,sizeof(b.values))&&!memcmp(a->units,b.units,sizeof(b.units)));CHECK(a->result==b.result&&a->result_valid==b.result_valid);}
static void language(App *a,Language l){App b=*a;if(l==LANG_TR)l=LANG_TR_ASCII;key(a,K_SETTINGS);key(a,K_ENTER);digit(a,l==LANG_EN?1:2);key(a,K_ESC);key(a,K_ESC);CHECK(loc_get()==l&&a->screen==b.screen);CHECK(!memcmp(a->values,b.values,sizeof(b.values))&&!memcmp(a->units,b.units,sizeof(b.units)));CHECK(a->result==b.result&&a->result_valid==b.result_valid&&a->input_mode==b.input_mode);}
static void shot(App *a,const char *tag){FILE *f;char path[180];unsigned i;draw(a);snprintf(path,sizeof(path),"build/phase3f-%02d-%s.pgm",a->calc,tag);f=fopen(path,"wb");CHECK(f!=0);fprintf(f,"P5\n320 240\n255\n");for(i=0;i<76800;i++)fputc(framebuffer[i]?0:255,f);CHECK(!fclose(f));}

static void menus(App a){unsigned n=0,i;App b=a;unsigned char local[CALC_COUNT]={0};if(visited[a.menu])return;visited[a.menu]=1;do{n++;app_key(&b,K_DOWN);}while(b.selected!=a.selected&&n<=10);CHECK(n>0&&n<=10);draw(&a);for(i=1;i<=n;i++){App d=a;digit(&d,i);if(d.screen==S_FORM||d.screen==S_INFO){CHECK((unsigned)d.calc<CALC_COUNT&&!local[d.calc]);local[d.calc]=1;if(d.screen==S_FORM)calc_seen[d.calc]=1;else book_seen[d.calc]=1;}else if(d.screen==S_MENU)menus(d);}}
static void flow(void){unsigned n,l,mode,i,u;char text[32];
 for(n=0;n<12;n++)for(l=0;l<3;l++)for(mode=0;mode<2;mode++){
  const LoRaCase *v=&lora_cases[n];const Calculation *c=&calculations[v->id];App a=open_case(v,mode,(Language)l);App old;
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
static void book_math(void){unsigned n,l,p,i;for(n=0;n<12;n++){
 const LoRaCase *v=&lora_cases[n];const Calculation *c=&calculations[v->id];const HandbookEntry *h=&handbook[v->id];MathArena arena;MathBox box;const MathNode *tree;
 math_arena_init(&arena);tree=formula_build(&arena,v->id);box=math_measure(tree);if(!(tree&&!arena.overflow&&box.width<=296&&box.above+box.below<=87))fprintf(stderr,"math id=%d nodes=%u overflow=%d size=%dx%d\n",v->id,arena.used,arena.overflow,box.width,box.above+box.below);CHECK(tree&&!arena.overflow&&box.width<=296&&box.above+box.below<=87);near(h->example_result,v->ref);
 for(i=0;i<c->count;i++)near(h->example_inputs[i],v->si[i]);
 for(l=0;l<3;l++){App a=open_case(v,0,(Language)l);key(&a,K_INFO);
  CHECK(loc_has(c->name)&&loc_has(c->definitions)&&loc_has(c->usage));CHECK(loc_has(h->purpose)&&loc_has(h->measurements)&&loc_has(h->laboratory)&&loc_has(h->limits)&&loc_has(h->example)&&loc_has(h->mistakes));
  for(p=0;p<8;p++){unsigned lines;digit(&a,p+1);lines=app_info_lines(&a);for(i=0;i<lines+2;i++)key(&a,K_DOWN);CHECK(a.info_scroll==(lines>11?lines-11:0));if(p==6&&l==1)shot(&a,"handbook-example-tr");}
  key(&a,K_ESC);CHECK(a.screen==S_FORM&&a.calc==v->id);
 }
}}
static void invalid(void){unsigned n,i;double x[8],r;for(n=0;n<12;n++){
 const LoRaCase *v=&lora_cases[n];App a=open_case(v,0,LANG_EN);
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


static void oracle(void){unsigned n;double r;for(n=0;n<sizeof(semtech_vectors)/sizeof(semtech_vectors[0]);n++){CHECK(calculate(LORA_AIRTIME,semtech_vectors[n].si,&r)==OK);near(r,semtech_vectors[n].seconds);}
 for(n=0;n<sizeof(semtech_legacy_vectors)/sizeof(semtech_legacy_vectors[0]);n++){CHECK(calculate(LORA_AIRTIME,semtech_legacy_vectors[n].si,&r)==OK);CHECK(floor(r*1000+.999)==semtech_legacy_vectors[n].milliseconds);}}
static void boundaries(void){double x[8]={7,125000,1,20,0,1,8,0},r,t;unsigned n;
 CHECK(lora_calculate(LORA_AIRTIME,0,&r)==INVALID);CHECK(lora_calculate(LORA_AIRTIME,x,0)==INVALID);CHECK(lora_calculate((CalcId)-1,x,&r)==INVALID);CHECK(calculate(LORA_AIRTIME,x,&r)==OK);near(r,.056576);x[0]=12;CHECK(calculate(LORA_AIRTIME,x,&r)==INVALID);x[7]=1;CHECK(calculate(LORA_AIRTIME,x,&r)==OK);near(r,1.318912);
 x[0]=11;CHECK(calculate(LORA_AIRTIME,x,&r)==OK);near(r,.741376);x[7]=0;CHECK(calculate(LORA_AIRTIME,x,&r)==INVALID);
 x[0]=6;x[7]=1;CHECK(calculate(LORA_AIRTIME,x,&r)==INVALID);x[0]=7;x[1]=100000;CHECK(calculate(LORA_AIRTIME,x,&r)==INVALID);
 x[1]=125000;x[2]=0;CHECK(calculate(LORA_AIRTIME,x,&r)==INVALID);x[2]=1;x[3]=256;CHECK(calculate(LORA_AIRTIME,x,&r)==INVALID);x[3]=20;x[4]=2;CHECK(calculate(LORA_AIRTIME,x,&r)==INVALID);
 x[4]=0;x[5]=2;CHECK(calculate(LORA_AIRTIME,x,&r)==INVALID);x[5]=1;x[6]=5;CHECK(calculate(LORA_AIRTIME,x,&r)==INVALID);x[6]=65536;CHECK(calculate(LORA_AIRTIME,x,&r)==INVALID);x[6]=8;x[7]=.5;CHECK(calculate(LORA_AIRTIME,x,&r)==INVALID);
 x[0]=.75;CHECK(calculate(LORA_CODE_INDEX,x,&r)==INVALID);x[0]=2.0/3;CHECK(calculate(LORA_CODE_INDEX,x,&r)==OK);near(r,2);
 x[0]=2;x[1]=1;CHECK(calculate(LORA_DUTY,x,&r)==INVALID);x[0]=1;x[1]=0;CHECK(calculate(LORA_DUTY,x,&r)==INVALID);x[0]=1e-300;x[1]=1e300;CHECK(calculate(LORA_DUTY,x,&r)==RANGE);
 x[0]=.056576;x[1]=3600;x[2]=.01;CHECK(calculate(LORA_PACKET_COUNT,x,&r)==OK);near(r,636);x[2]=0;CHECK(calculate(LORA_PACKET_COUNT,x,&r)==OK);CHECK(r==0);x[2]=1.01;CHECK(calculate(LORA_PACKET_COUNT,x,&r)==INVALID);
 x[0]=3;x[1]=60;x[2]=.1;CHECK(calculate(LORA_PACKET_COUNT,x,&r)==OK);near(r,2);x[0]=1e-300;x[1]=1e300;x[2]=1;CHECK(calculate(LORA_PACKET_COUNT,x,&r)==RANGE);x[0]=1;x[1]=9007199254740991.0;x[2]=1;CHECK(calculate(LORA_PACKET_COUNT,x,&r)==OK);CHECK(r==9007199254740991.0);
 x[0]=0;x[1]=1e300;CHECK(calculate(LORA_TX_ENERGY,x,&r)==OK);CHECK(r==0);x[0]=1e300;CHECK(calculate(LORA_TX_ENERGY,x,&r)==RANGE);x[0]=1e-300;x[1]=1e-300;CHECK(calculate(LORA_TX_ENERGY,x,&r)==RANGE);
 /* Full eight-field input focus remains on-screen, including rows 3/4/5 of a long form. */
 {const LoRaCase*v=&lora_cases[5];App a=open_case(v,1,LANG_EN);for(n=0;n<v->count;n++){unit(&a,n,v->units[n]);edit(&a,n,v->entered[n]);}result(&a,v->ref);edit(&a,0,"12");key(&a,K_CALCULATE);CHECK(a.error==INVALID&&!a.result_valid);info_return(&a);edit(&a,7,"1");result(&a,1.318912);shot(&a,"sf12-ldro-en");}
 /* A zero-LDRO short-symbol packet and optional manual LDRO have separate exact references. */
 x[0]=7;x[1]=125000;x[2]=1;x[3]=20;x[4]=0;x[5]=1;x[6]=8;x[7]=1;CHECK(calculate(LORA_AIRTIME,x,&r)==OK);near(r,.066816);
 /* M_STACK preserves vertical bounds and does not reset state; compact worked-example equation fits. */
 clear_screen();CHECK(formula_draw(LORA_AIRTIME,12,77,296,128));CHECK(!display_clipped_pixels&&!display_substitutions);clear_screen();CHECK(formula_draw_primary(LORA_AIRTIME,12,130,296,50));CHECK(!display_clipped_pixels&&!display_substitutions);
 x[0]=7;x[1]=125000;CHECK(calculate(LORA_SYMBOL_T,x,&t)==OK);CHECK(calculate(LORA_SYMBOL_RATE,x,&r)==OK);near(t*r,1);
}
static void inverse_flows(void){unsigned i;const LoRaCase*v=&lora_cases[5];App a=open_case(v,0,LANG_TR);for(i=0;i<v->count;i++){unit(&a,i,v->units[i]);edit(&a,i,v->entered[i]);}result(&a,v->ref);key(&a,K_TARGET);digit(&a,2);CHECK(a.calc==LORA_PAYLOAD_T);result(&a,.044032);key(&a,K_TARGET);digit(&a,3);CHECK(a.calc==LORA_PREAMBLE_T);result(&a,.012544);key(&a,K_TARGET);digit(&a,1);CHECK(a.calc==LORA_AIRTIME);result(&a,v->ref);info_return(&a);
 v=&lora_cases[0];a=open_case(v,1,LANG_EN);for(i=0;i<v->count;i++){unit(&a,i,v->units[i]);edit(&a,i,v->entered[i]);}result(&a,v->ref);key(&a,K_TARGET);digit(&a,2);result(&a,976.5625);key(&a,K_TARGET);digit(&a,1);result(&a,v->ref);
 v=&lora_cases[3];a=open_case(v,1,LANG_TR_ASCII);edit(&a,0,"1");result(&a,.8);key(&a,K_TARGET);digit(&a,2);result(&a,1);key(&a,K_TARGET);digit(&a,1);result(&a,.8);
}
static void visible_text(const App *a,int x,int y,int width,const char *expected){unsigned char got[320*9];unsigned i;int same=1;draw((App*)a);for(i=0;i<9;i++)memcpy(got+i*width,framebuffer+(y+i)*320+x,width);clear_screen();text_fit(x,y,width,expected,1);for(i=0;i<9;i++)if(memcmp(got+i*width,framebuffer+(y+i)*320+x,width))same=0;if(!same)fprintf(stderr,"text mismatch id=%d screen=%d expected=%s\n",a->calc,a->screen,expected);CHECK(same);}
static void limit_notices(void){static const unsigned indices[]={7,32,19,1};static const char*messages[]={"Zero-thickness microstrip; H limits","4 normalized readings; H assumptions","Numeric codes only; R/EIA-96 excluded","IPC-2221 estimate; not a safety rating"};unsigned n,i,l;char textbuf[80];
 for(n=0;n<4;n++)for(l=0;l<3;l++){const LabCase*b=&lab_cases[indices[n]];LoRaCase v={0};App a;v.id=b->id;v.count=b->count;v.output=b->output;v.ref=b->ref;for(i=0;i<3;i++)v.path[i]=b->path[i];for(i=0;i<v.count;i++){v.units[i]=b->units[i];v.entered[i]=b->entered[i];}a=open_case(&v,0,(Language)l);for(i=0;i<v.count;i++){unit(&a,i,v.units[i]);edit(&a,i,v.entered[i]);}result(&a,v.ref);visible_text(&a,6,228,308,tr(messages[n]));info_return(&a);key(&a,K_SETTINGS);snprintf(textbuf,sizeof(textbuf),tr("Version: %s"),C21_VERSION);visible_text(&a,13,185,294,textbuf);snprintf(textbuf,sizeof(textbuf),tr("Build date: %s"),C21_BUILD_DATE);visible_text(&a,13,201,294,textbuf);key(&a,K_ESC);CHECK(a.screen==S_RESULT&&a.result_valid);}
 {const LoRaCase*v=&lora_cases[10];App a=open_case(v,0,LANG_EN);unit(&a,0,0);edit(&a,0,"1");unit(&a,1,0);edit(&a,1,"9007199254740991");unit(&a,2,0);edit(&a,2,"1");result(&a,9007199254740991.0);CHECK(a.result==9007199254740991.0);visible_text(&a,13,72,294,"N = 9007199254740991 pkt");shot(&a,"exact-large-count-en");}
}
int main(void){App a;unsigned i;settings_bind(0);CHECK(CALC_COUNT==138&&CALC_MAX_INPUTS==8&&sizeof(lora_cases)/sizeof(lora_cases[0])==12);app_init(&a);key(&a,K_ENTER);menus(a);for(i=0;i<APP_MENU_COUNT;i++)CHECK(visited[i]);for(i=0;i<CALC_COUNT;i++)CHECK(calc_seen[i]&&book_seen[i]);for(i=0;i<VAR_COUNT;i++){CHECK(variable_symbol((VariableId)i)&&*variable_symbol((VariableId)i));CHECK(variable_meaning((VariableId)i)&&*variable_meaning((VariableId)i));}oracle();boundaries();book_math();flow();inverse_flows();limit_notices();invalid();printf("PASS: %u Phase 3F LoRa/RC checks (%u frames, 12 new targets, 138 reachable targets and Handbook entries, 5376 vendor cases, %u Formula starts, %u List fallbacks)\n",checks,frames,formula_count,list_fallbacks);return 0;}
