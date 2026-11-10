#include "math_layout.h"
#include "display.h"
#include "lab_engine.h"
#include "lora_engine.h"
#include <string.h>
#include <stdio.h>
static int max(int a,int b){return a>b?a:b;}
void math_arena_init(MathArena *a){memset(a,0,sizeof(*a));}
static MathNode *node(MathArena *a,MathKind kind,const MathNode *x,const MathNode *y,const MathNode *z){
 MathNode *n;if(a->used>=MATH_CAPACITY){a->overflow=1;return 0;}
 n=&a->nodes[a->used++];n->kind=kind;n->a=x;n->b=y;n->c=z;n->variable=VAR_NONE;n->role=M_CONSTANT;return n;
}
const MathNode *math_text(MathArena *a,const char *s){MathNode *n=node(a,M_TEXT,0,0,0);if(n)n->text=s;return n;}
const MathNode *math_row(MathArena *a,const MathNode *x,const MathNode *y,int gap){MathNode *n=node(a,M_ROW,x,y,0);if(n)n->gap=gap<0?0:gap;return n;}
const MathNode *math_stack(MathArena *a,const MathNode *x,const MathNode *y,int gap){MathNode *n=node(a,M_STACK,x,y,0);if(n)n->gap=gap<0?0:gap;return n;}
const MathNode *math_fraction(MathArena *a,const MathNode *x,const MathNode *y){return node(a,M_FRACTION,x,y,0);}
const MathNode *math_root(MathArena *a,const MathNode *x){return node(a,M_ROOT,x,0,0);}
const MathNode *math_script(MathArena *a,const MathNode *x,const MathNode *sup,const MathNode *sub){return node(a,M_SCRIPT,x,sup,sub);}
const MathNode *math_parens(MathArena *a,const MathNode *x){return node(a,M_PARENS,x,0,0);}
static const MathNode *variable(MathArena *a,const MathNode *child,VariableId id,MathRole role){MathNode *n=node(a,M_VARIABLE,child,0,0);if(n){n->variable=id;n->role=role;}return n;}
static MathBox measure(const MathNode *n,int depth){
 MathBox z={0,0,0},a,b,c;if(!n||depth>24)return z;
 a=measure(n->a,depth+1);b=measure(n->b,depth+1);c=measure(n->c,depth+1);
 switch(n->kind){
 case M_STACK:z.width=max(a.width,b.width);z.above=a.above;z.below=a.below+n->gap+b.above+b.below;break;
 case M_VARIABLE:z.width=a.width+4;z.above=a.above+2;z.below=a.below+2;break;
 case M_TEXT:z.width=text_width(n->text);z.above=7;z.below=2;break;
 case M_ROW:z.width=a.width+b.width+n->gap;z.above=max(a.above,b.above);z.below=max(a.below,b.below);break;
 case M_FRACTION:z.width=max(a.width,b.width)+6;z.above=a.above+a.below+6;z.below=b.above+b.below+1;break;
 case M_ROOT:z.width=a.width+11;z.above=a.above+3;z.below=max(a.below,2);break;
 case M_SCRIPT:{int up=max(a.above,b.below+2),down=max(5,c.above-2);z.width=a.width+1+max(b.width,c.width);z.above=max(a.above,n->b?b.above+up:0);z.below=max(a.below,n->c?c.below+down:0);break;}
 case M_PARENS:z.width=a.width+10;z.above=a.above+1;z.below=a.below+1;break;
 }
 return z;
}
MathBox math_measure(const MathNode *n){return measure(n,0);}
static void stroke(int x0,int y0,int x1,int y1,int ink){
 int dx=x1>x0?x1-x0:x0-x1,sx=x0<x1?1:-1;
 int dy=y1>y0?y0-y1:y1-y0,sy=y0<y1?1:-1,err=dx+dy;
 for(;;){int e2;rect(x0,y0,1,1,ink);if(x0==x1&&y0==y1)break;e2=2*err;if(e2>=dy){err+=dy;x0+=sx;}if(e2<=dx){err+=dx;y0+=sy;}}
}
static void draw(const MathNode *n,int x,int baseline,int ink,int depth,MathInteraction *paint){
 MathBox a,b,z;if(!n||depth>24)return;a=math_measure(n->a);b=math_measure(n->b);z=math_measure(n);
 switch(n->kind){
 case M_VARIABLE:{
  int top=baseline-z.above,h=z.above+z.below;
  if(paint&&paint->count<MATH_FIELD_RECTS){MathFieldRect *r=&paint->fields[paint->count++];r->x=x;r->y=top;r->width=z.width;r->height=h;r->variable=n->variable;r->role=n->role;}
  if(paint&&n->role==M_INPUT&&n->variable==paint->active){rect(x,top,z.width,h,1);draw(n->a,x+2,baseline,0,depth+1,paint);}
  else{
   draw(n->a,x+2,baseline,ink,depth+1,paint);
   if(paint&&n->role==M_OUTPUT){rect(x,top,z.width,2,ink);rect(x,top+h-2,z.width,2,ink);rect(x,top,2,h,ink);rect(x+z.width-2,top,2,h,ink);}
   else if(paint)rect(x,top+h-1,z.width,1,ink);
  }
  break;
 }
 case M_STACK:draw(n->a,x+(z.width-a.width)/2,baseline,ink,depth+1,paint);draw(n->b,x+(z.width-b.width)/2,baseline+a.below+n->gap+b.above,ink,depth+1,paint);break;
 case M_TEXT:if(n->text)text(x,baseline-7,n->text,ink);break;
 case M_ROW:draw(n->a,x,baseline,ink,depth+1,paint);draw(n->b,x+a.width+n->gap,baseline,ink,depth+1,paint);break;
 case M_FRACTION:
  draw(n->a,x+(z.width-a.width)/2,baseline-6-a.below,ink,depth+1,paint);
  rect(x,baseline-3,z.width,1,ink);
  draw(n->b,x+(z.width-b.width)/2,baseline+1+b.above,ink,depth+1,paint);break;
 case M_ROOT:
  stroke(x,baseline-1,x+3,baseline+z.below-1,ink);
  stroke(x+3,baseline+z.below-1,x+7,baseline-a.above-2,ink);
  rect(x+7,baseline-a.above-2,z.width-7,1,ink);
  draw(n->a,x+10,baseline,ink,depth+1,paint);break;
 case M_SCRIPT:
  draw(n->a,x,baseline,ink,depth+1,paint);
  draw(n->b,x+a.width+1,baseline-max(a.above,b.below+2),ink,depth+1,paint);
  {MathBox sub=math_measure(n->c);draw(n->c,x+a.width+1,baseline+max(5,sub.above-2),ink,depth+1,paint);}break;
 case M_PARENS:{
  int top=baseline-z.above,bottom=baseline+z.below-1,mid=(top+bottom)/2,r=x+z.width-1;
  stroke(x+3,top,x+1,top+2,ink);stroke(x+1,top+2,x,mid,ink);stroke(x,mid,x+1,bottom-2,ink);stroke(x+1,bottom-2,x+3,bottom,ink);
  stroke(r-3,top,r-1,top+2,ink);stroke(r-1,top+2,r,mid,ink);stroke(r,mid,r-1,bottom-2,ink);stroke(r-1,bottom-2,r-3,bottom,ink);
  draw(n->a,x+5,baseline,ink,depth+1,paint);break;
 }
 }
}
void math_draw(const MathNode *n,int x,int baseline,int ink){draw(n,x,baseline,ink,0,0);}
/* Small construction helpers keep formula trees explicit and inspectable. */
#define T(s) math_text(a,s)
#define R(x,y) math_row(a,x,y,3)
#define F(x,y) math_fraction(a,x,y)
#define S(x,sup,sub) math_script(a,x,sup,sub)
#define P(x) math_parens(a,x)
static const MathNode *eq(MathArena *a,const MathNode *left,const MathNode *right){return R(R(left,T("=")),right);}
static const MathNode *times(MathArena *a,const MathNode *x,const MathNode *y,int numeric){return numeric?R(R(x,T("×")),y):R(x,y);}
static const MathNode *value(MathArena *a,const char *const *values,int index,const char *symbol){const MathNode *n=math_text(a,values?values[index]:symbol);return a->interactive?variable(a,n,calc_input_id((CalcId)a->formula_id,(unsigned)index),M_INPUT):n;}
static const MathNode *sub_value(MathArena *a,const char *const *values,int i,const char *base,const char *sub){
 const MathNode *n;if(values)return value(a,values,i,base);n=math_script(a,math_text(a,base),0,math_text(a,sub));return a->interactive?variable(a,n,calc_input_id((CalcId)a->formula_id,(unsigned)i),M_INPUT):n;
}
#define W(i,s,sub) sub_value(a,values,i,s,sub)
#define ADD(x,y) R(R(x,T("+")),y)
#define SUBTRACT(x,y) R(R(x,T("-")),y)
#define SQUARED(x) S(x,T("2"),0)
#define V(i,s) value(a,values,i,s)
#define MUL(x,y) times(a,x,y,values!=0)
const MathNode *formula_build_values(MathArena *a,int id,const char *const *values){
 const MathNode *left=0,*right=0;
 const char *c=values?"299792458":"c";
 switch(id){
 case VOLTAGE:left=T("V");right=MUL(V(0,"I"),V(1,"R"));break;
 case CURRENT:left=T("I");right=F(V(0,"V"),V(1,"R"));break;
 case RESISTANCE:left=T("R");right=F(V(0,"V"),V(1,"I"));break;
 case POWER:left=T("P");right=MUL(V(0,"V"),V(1,"I"));break;
 case MW_DBM:left=T("dBm");right=R(R(T("10"),S(T("log"),0,T("10"))),P(F(V(0,"P"),T(values?"0.001":"1mW"))));break;
 case DBM_MW:left=T("P");right=R(R(T(values?"0.001":"1mW"),T("×")),S(T("10"),F(V(0,"dBm"),T("10")),0));break;
 case WAVELENGTH:left=T("λ");right=F(T(c),V(0,"f"));break;
 case QUARTER_WAVE:left=T("l");right=F(T(c),MUL(T("4"),V(0,"f")));if(!values)right=R(R(F(T("λ"),T("4")),T("=")),right);break;
 case LC_FREQ:left=S(T("f"),0,T("0"));right=F(T("1"),MUL(T("2π"),math_root(a,MUL(V(0,"L"),V(1,"C")))));break;
 case LC_CAP:left=T("C");right=F(T("1"),MUL(S(P(MUL(T("2π"),V(0,"f"))),T("2"),0),V(1,"L")));break;
 case LC_INDUCTANCE:left=T("L");right=F(T("1"),MUL(S(P(MUL(T("2π"),V(0,"f"))),T("2"),0),V(1,"C")));break;
 case XL:left=S(T("X"),0,T("L"));right=MUL(MUL(T("2π"),V(0,"f")),V(1,"L"));break;
 case XC:left=S(T("X"),0,T("C"));right=F(T("1"),MUL(MUL(T("2π"),V(0,"f")),V(1,"C")));break;
 case FSPL:left=S(T("L"),0,T("fs"));right=R(R(T("20"),S(T("log"),0,T("10"))),P(F(MUL(MUL(T("4π"),V(0,"d")),V(1,"f")),T(c))));break;
 case FRESNEL:{
  const MathNode *d1=values?V(1,"d₁"):S(T("d"),0,T("1"));
  const MathNode *d2=values?V(2,"d₂"):S(T("d"),0,T("2"));
  if(a->interactive){d1=variable(a,d1,VAR_D1,M_INPUT);d2=variable(a,d2,VAR_D2,M_INPUT);}
  const MathNode *lambda=values?F(T(c),V(0,"f")):a->interactive?F(T("c"),V(0,"f")):T("λ");
  left=S(T("r"),0,T("1"));right=math_root(a,F(MUL(MUL(lambda,d1),d2),R(R(d1,T("+")),d2)));break;
 }
 case POWER_IR:left=T("P");right=MUL(SQUARED(V(0,"I")),V(1,"R"));break;
 case POWER_VR:left=T("P");right=F(SQUARED(V(0,"V")),V(1,"R"));break;
 case R_SERIES:left=T("R");right=ADD(W(0,"R","1"),W(1,"R","2"));break;
 case R_PARALLEL:left=T("R");right=F(MUL(W(0,"R","1"),W(1,"R","2")),ADD(W(0,"R","1"),W(1,"R","2")));break;
 case V_DIVIDER:left=S(T("V"),0,T("o"));right=F(MUL(W(0,"V","i"),W(2,"R","2")),ADD(W(1,"R","1"),W(2,"R","2")));break;
 case I_DIVIDER:left=S(T("I"),0,T("1"));right=F(MUL(W(0,"I","s"),W(2,"R","2")),ADD(W(1,"R","1"),W(2,"R","2")));break;
 case KVL_REMAINDER:left=S(T("V"),0,T("3"));right=SUBTRACT(SUBTRACT(W(0,"V","s"),W(1,"V","1")),W(2,"V","2"));break;
 case KCL_REMAINDER:left=S(T("I"),0,T("3"));right=SUBTRACT(SUBTRACT(W(0,"I","s"),W(1,"I","1")),W(2,"I","2"));break;
 case LED_RESISTOR:left=T("R");right=F(SUBTRACT(W(0,"V","s"),W(1,"V","f")),V(2,"I"));break;
 case CAP_ENERGY:left=T("E");right=F(MUL(V(0,"C"),SQUARED(V(1,"V"))),T("2"));break;
 case RC_CHARGE:left=S(T("V"),0,T("c"));right=MUL(W(0,"V","s"),P(SUBTRACT(T("1"),S(T("e"),F(R(T("-"),V(3,"t")),MUL(V(1,"R"),V(2,"C"))),0))));break;
 case RC_DISCHARGE:left=S(T("V"),0,T("c"));right=MUL(W(0,"V","0"),S(T("e"),F(R(T("-"),V(3,"t")),MUL(V(1,"R"),V(2,"C"))),0));break;
 case RLC_MAG:case RLC_PHASE:{
  const MathNode *x=SUBTRACT(MUL(MUL(T("2π"),V(3,"f")),V(1,"L")),F(T("1"),MUL(MUL(T("2π"),V(3,"f")),V(2,"C"))));
  if(id==RLC_MAG){left=T("|Z|");right=math_root(a,ADD(SQUARED(V(0,"R")),SQUARED(P(x))));}
  else{left=T("φ");right=R(T("atan2"),P(R(R(x,T(",")),V(0,"R"))));}
  break;
 }
 case RC_TAU:left=T("τ");right=MUL(V(0,"R"),V(1,"C"));break;
 case RL_TAU:left=T("τ");right=F(V(0,"L"),V(1,"R"));break;
 case AH_WH:left=T("E");right=MUL(V(0,"Q"),V(1,"V"));break;
 case WH_AH:left=T("Q");right=F(V(0,"E"),V(1,"V"));break;
 case AVG_CURRENT:left=T("I");right=F(V(0,"Q"),V(1,"t"));break;
 case BAT_RUNTIME:left=T("t");right=F(MUL(V(2,"k"),V(0,"Q")),V(1,"I"));break;
 case DUTY_POWER:left=T("P");right=ADD(MUL(V(2,"D"),W(0,"P","on")),MUL(P(SUBTRACT(T("1"),V(2,"D"))),W(1,"P","off")));break;
 case LDO_LOSS:left=T("P");right=MUL(P(SUBTRACT(W(0,"V","i"),W(1,"V","o"))),V(2,"I"));break;
 case DCDC_IN:left=S(T("P"),0,T("i"));right=F(W(0,"P","o"),V(1,"η"));break;
 case DCDC_OUT:left=S(T("P"),0,T("o"));right=MUL(V(1,"η"),W(0,"P","i"));break;
 case DCDC_EFF:left=T("η");right=F(W(0,"P","o"),W(1,"P","i"));break;
 case RES_RATING:left=S(T("P"),0,T("r"));right=F(V(0,"P"),V(1,"k"));break;
 case RF_DBW_DBM:left=T("P");right=ADD(V(0,"P"),T("30"));break;
 case RF_DBM_DBW:left=T("P");right=SUBTRACT(V(0,"P"),T("30"));break;
 case RF_GAIN_DB:left=T("G");right=MUL(T("10"),R(S(T("log"),0,T("10")),P(V(0,"G"))));break;
 case RF_GAIN_RATIO:left=T("G");right=S(T("10"),F(V(0,"G"),T("10")),0);break;
 case RF_AMP_DB:left=T("G");right=MUL(T("20"),R(S(T("log"),0,T("10")),P(V(0,"A"))));break;
 case RF_AMP_RATIO:left=T("A");right=S(T("10"),F(V(0,"G"),T("20")),0);break;
 case RF_POWER_RATIO:left=T("G");right=F(W(0,"P","o"),W(1,"P","i"));break;
 case RF_VRMS:left=T("V");right=math_root(a,MUL(V(0,"P"),V(1,"R")));break;
 case RF_GAIN_SUM:left=T("G");right=ADD(ADD(V(0,"G₁"),V(1,"G₂")),V(2,"G₃"));break;
 case RF_LOSS_SUM:left=T("L");right=ADD(ADD(V(0,"L₁"),V(1,"L₂")),V(2,"L₃"));break;
 case RF_EIRP:left=T("EIRP");right=ADD(SUBTRACT(W(0,"P","t"),W(1,"L","t")),W(2,"G","t"));break;
 case RF_ERP:left=T("ERP");right=SUBTRACT(V(0,"EIRP"),T("2.15"));break;
 case RF_FROM_ERP:left=T("EIRP");right=ADD(V(0,"ERP"),T("2.15"));break;
 case RF_DBI:left=S(T("G"),0,T("i"));right=ADD(W(0,"G","d"),T("2.15"));break;
 case RF_DBD:left=S(T("G"),0,T("d"));right=SUBTRACT(W(0,"G","i"),T("2.15"));break;
 case RF_NOISE_W:left=T("N");right=MUL(MUL(S(T("k"),0,T("B")),V(1,"B")),P(ADD(V(0,"T"),MUL(T("T₀"),P(SUBTRACT(S(T("10"),F(V(2,"NF"),T("10")),0),T("1")))))));break;
 case RF_NOISE_DBM:left=S(T("N"),0,T("m"));right=MUL(T("10"),R(S(T("log"),0,T("10")),P(F(MUL(MUL(S(T("k"),0,T("B")),V(1,"B")),P(ADD(V(0,"T"),MUL(T("T₀"),P(SUBTRACT(S(T("10"),F(V(2,"NF"),T("10")),0),T("1"))))))),T("1mW")))));break;
 case RF_SNR:left=T("SNR");right=MUL(T("10"),R(S(T("log"),0,T("10")),P(F(V(0,"S"),V(1,"N")))));break;
 case RF_NOISE_SNR:left=T("N");right=F(V(0,"S"),S(T("10"),F(V(1,"SNR"),T("10")),0));break;
 case RF_NF_FACTOR:left=T("F");right=S(T("10"),F(V(0,"NF"),T("10")),0);break;
 case RF_FACTOR_NF:left=T("NF");right=MUL(T("10"),R(S(T("log"),0,T("10")),P(V(0,"F"))));break;
 case RF_FRIIS_F:left=T("F");right=ADD(ADD(V(0,"F₁"),F(SUBTRACT(V(1,"F₂"),T("1")),V(3,"G₁"))),F(SUBTRACT(V(2,"F₃"),T("1")),MUL(V(3,"G₁"),V(4,"G₂"))));break;
 case RF_FRIIS_NF:left=T("NF");right=MUL(T("10"),R(S(T("log"),0,T("10")),P(ADD(ADD(V(0,"F₁"),F(SUBTRACT(V(1,"F₂"),T("1")),V(3,"G₁"))),F(SUBTRACT(V(2,"F₃"),T("1")),MUL(V(3,"G₁"),V(4,"G₂")))))));break;
 case RF_SENS:left=S(T("S"),0,T("min"));right=ADD(W(0,"N","m"),V(1,"SNR"));break;
 case RF_HALF:left=T("l");right=F(T("c"),MUL(T("2"),V(0,"f")));break;
 case RF_SPEED:left=T("v");right=F(T("c"),math_root(a,W(0,"ε","r")));break;
 case RF_DIEL_WAVE:left=T("λ");right=F(T("c"),MUL(V(0,"f"),math_root(a,W(1,"ε","r"))));break;
 case RF_LINK:left=S(T("P"),0,T("r"));right=SUBTRACT(SUBTRACT(ADD(ADD(SUBTRACT(W(0,"P","t"),W(1,"L","t")),W(2,"G","t")),W(3,"G","r")),W(4,"L","p")),W(5,"L","r"));break;
 case RF_MARGIN:left=T("M");right=SUBTRACT(W(0,"P","r"),W(1,"S","min"));break;
 case RF_CLEAR:left=T("q");right=F(V(0,"h"),V(1,"r₁"));break;
 case RF_HORIZON:left=T("d");right=ADD(math_root(a,MUL(MUL(R(T("2"),S(T("R"),0,T("E"))),V(2,"k")),V(0,"h₁"))),math_root(a,MUL(MUL(R(T("2"),S(T("R"),0,T("E"))),V(2,"k")),V(1,"h₂"))));break;
 case RF_POL:left=T("L");right=MUL(T("-20"),R(S(T("log"),0,T("10")),P(R(T("cos"),P(V(0,"θ"))))));break;
 case RF_PHASE:left=T("φ");right=F(MUL(T("2π"),V(0,"l")),V(1,"λ"));break;
 case RF_DELAY:left=T("φ");right=F(MUL(T("-2π"),V(0,"l")),V(1,"λ"));break;
 case RF_RC_CUTOFF:left=S(T("f"),0,T("c"));right=F(T("1"),MUL(MUL(T("2π"),V(0,"R")),V(1,"C")));break;
 case RF_LP:left=T("A");right=F(W(1,"f","c"),math_root(a,ADD(SQUARED(V(0,"f")),SQUARED(W(1,"f","c")))));break;
 case RF_HP:left=T("A");right=F(V(0,"f"),math_root(a,ADD(SQUARED(V(0,"f")),SQUARED(W(1,"f","c")))));break;
 case RF_SERIES_Q:left=T("Q");right=F(math_root(a,F(V(1,"L"),V(2,"C"))),V(0,"R"));break;
 case RF_SERIES_BW:left=T("B");right=F(V(0,"R"),MUL(T("2π"),V(1,"L")));break;
 case RF_REFLECT:left=T("ρ");right=math_root(a,F(ADD(SQUARED(P(SUBTRACT(W(0,"R","L"),V(2,"Z₀")))),SQUARED(W(1,"X","L"))),ADD(SQUARED(P(ADD(W(0,"R","L"),V(2,"Z₀")))),SQUARED(W(1,"X","L")))));break;
 case RF_VSWR:left=T("VSWR");right=F(ADD(T("1"),V(0,"ρ")),SUBTRACT(T("1"),V(0,"ρ")));break;
 case RF_RETURN:left=T("RL");right=MUL(T("-20"),R(S(T("log"),0,T("10")),P(V(0,"ρ"))));break;
 case RF_IMPEDANCE:left=S(T("R"),0,T("L"));right=MUL(V(2,"Z₀"),F(SUBTRACT(SUBTRACT(T("1"),SQUARED(W(0,"Γ","r"))),SQUARED(W(1,"Γ","i"))),ADD(SQUARED(P(SUBTRACT(T("1"),W(0,"Γ","r")))),SQUARED(W(1,"Γ","i")))));break;
 case RF_LMATCH_LP:left=S(T("L"),0,T("s"));right=F(MUL(math_root(a,SUBTRACT(F(R(T("max"),P(R(R(W(0,"R","s"),T(",")),W(1,"R","L")))),R(T("min"),P(R(R(W(0,"R","s"),T(",")),W(1,"R","L"))))),T("1"))),R(T("min"),P(R(R(W(0,"R","s"),T(",")),W(1,"R","L"))))),MUL(T("2π"),V(2,"f")));break;
 case RF_LMATCH_HP:left=S(T("C"),0,T("s"));right=F(T("1"),MUL(MUL(MUL(T("2π"),V(2,"f")),math_root(a,SUBTRACT(F(R(T("max"),P(R(R(W(0,"R","s"),T(",")),W(1,"R","L")))),R(T("min"),P(R(R(W(0,"R","s"),T(",")),W(1,"R","L"))))),T("1")))),R(T("min"),P(R(R(W(0,"R","s"),T(",")),W(1,"R","L"))))));break;
 case RF_W_DBW:left=T("dBW");right=MUL(T("10"),R(S(T("log"),0,T("10")),P(F(V(0,"P"),T("1W")))));break;
 case RF_DBW_W:left=T("P");right=MUL(T("1W"),S(T("10"),F(V(0,"P"),T("10")),0));break;
#include "lab_formulas.inc"
#include "lora_formulas.inc"
 case FORMULA_Z:left=T("Z");right=R(R(T("R"),T("+")),R(T("j"),T("X")));break;
 case FORMULA_DELTA_V:left=T("ΔV");right=R(T("I"),T("R"));break;
 default:return 0;
 }
 if(a->interactive&&id<CALC_COUNT)left=variable(a,left,calc_output_id((CalcId)id),M_OUTPUT);
 return eq(a,left,right);
}
const MathNode *formula_build(MathArena *a,int id){return formula_build_values(a,id,0);}
int formula_draw(int id,int x,int y,int w,int h){
 MathArena arena;const MathNode *n;MathBox b;math_arena_init(&arena);n=formula_build(&arena,id);b=math_measure(n);
 if(!n||arena.overflow||b.width>w||b.above+b.below>h)return 0;
 math_draw(n,x+(w-b.width)/2,y+(h-b.above-b.below)/2+b.above,1);return 1;
}
int formula_draw_primary(int id,int x,int y,int w,int h){MathArena a;const MathNode *n;MathBox b;math_arena_init(&a);n=formula_build(&a,id);if(n&&n->kind==M_STACK)n=n->a;b=math_measure(n);if(!n||a.overflow||b.width>w||b.above+b.below>h)return 0;math_draw(n,x+(w-b.width)/2,y+(h-b.above-b.below)/2+b.above,1);return 1;}
int formula_draw_values(int id,const double *inputs,int x,int y,int w,int h){
 MathArena arena;const MathNode *n;MathBox b;char values[CALC_MAX_INPUTS][32];const char *bindings[CALC_MAX_INPUTS];unsigned i;
 if(!inputs||id<0||id>=CALC_COUNT)return 0;
 for(i=0;i<calculations[id].count;i++){snprintf(values[i],sizeof(values[i]),"%.8g",inputs[i]);bindings[i]=values[i];}
 math_arena_init(&arena);n=formula_build_values(&arena,id,bindings);b=math_measure(n);
 if(!n||arena.overflow||b.width>w||b.above+b.below>h)return 0;
 math_draw(n,x+(w-b.width)/2,y+(h-b.above-b.below)/2+b.above,1);return 1;
}

const MathNode *formula_build_interactive(MathArena *a,CalcId id){a->interactive=1;a->formula_id=id;return formula_build(a,id);}
int formula_interactive_fits(CalcId id,int w,int h){if(lab_list_only(id)||lora_list_only(id))return 0;MathArena a;const MathNode *n;MathBox b;math_arena_init(&a);n=formula_build_interactive(&a,id);b=math_measure(n);return n&&!a.overflow&&b.width<=w&&b.above+b.below<=h;}
int formula_interactive(CalcId id,VariableId active,int x,int y,int w,int h,MathInteraction *paint){
 MathArena a;const MathNode *n;MathBox b;if(!paint)return 0;
 memset(paint,0,sizeof(*paint));paint->active=active;math_arena_init(&a);n=formula_build_interactive(&a,id);b=math_measure(n);
 if(!n||a.overflow||b.width>w||b.above+b.below>h)return 0;
 draw(n,x+(w-b.width)/2,y+(h-b.above-b.below)/2+b.above,1,0,paint);return 1;
}
