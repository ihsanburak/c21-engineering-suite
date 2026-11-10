#include "lora_engine.h"
#include <math.h>
#include <float.h>
static int whole(double v,double lo,double hi){return isfinite(v)&&v>=lo&&v<=hi&&v==floor(v);}
static int bw_valid(double v){return v==125000||v==250000||v==500000;}
static int cr_index(double v){unsigned i;for(i=1;i<=4;i++)if(fabs(v-4.0/(4+i))<=1e-12)return (int)i;return 0;}
static int flag(double v){return v==0||v==1;}
int lora_parameter_valid(CalcId id,unsigned i,double v){
 if(!isfinite(v))return 0;
 switch(id){
 case LORA_SYMBOL_T:case LORA_SYMBOL_RATE:return i==0?whole(v,7,12):bw_valid(v);
 case LORA_BITRATE:return i==0?whole(v,7,12):i==1?bw_valid(v):whole(v,1,4);
 case LORA_CODE_RATE:return whole(v,1,4);
 case LORA_CODE_INDEX:return cr_index(v)!=0;
 case LORA_AIRTIME:case LORA_PAYLOAD_T:
  return i==0?whole(v,7,12):i==1?bw_valid(v):i==2?whole(v,1,4):i==3?whole(v,0,255):(id==LORA_AIRTIME&&i==6)?whole(v,6,65535):flag(v);
 case LORA_PREAMBLE_T:return i==0?whole(v,7,12):i==1?bw_valid(v):whole(v,6,65535);
 case LORA_PAYLOAD_SYMBOLS:return i==0?whole(v,7,12):i==1?whole(v,1,4):i==2?whole(v,0,255):flag(v);
 case LORA_PACKET_COUNT:return i<2?v>0:(v>=0&&v<=1);
 case LORA_TX_ENERGY:return v>=0;
 default:return v>0;
 }
}
/* Independent integer form of the documented ceiling/max equation. All casts follow bounds checks. */
Status lora_payload_symbols(const double *x,unsigned *out){
 unsigned i;int numerator,denominator,blocks;
 if(!x||!out)return INVALID;
 for(i=0;i<6;i++)if(!lora_parameter_valid(LORA_PAYLOAD_SYMBOLS,i,x[i]))return INVALID;
 numerator=8*(int)x[2]-4*(int)x[0]+28+16*(int)x[4]-20*(int)x[3];denominator=4*((int)x[0]-2*(int)x[5]);
 blocks=numerator<=0?0:(numerator+denominator-1)/denominator;
 *out=8+(unsigned)(blocks*((int)x[1]+4));return OK;
}
int lora_list_only(CalcId id){return id==LORA_AIRTIME||id==LORA_PAYLOAD_T||id==LORA_PAYLOAD_SYMBOLS;}
Status lora_calculate(CalcId id,const double *x,double *out){
 double y=0,ts,a,six[6];unsigned n;int zero=0;Status st;unsigned i;
 if(!x||!out||id<LORA_FIRST_ID||id>=CALC_COUNT)return INVALID;
 for(i=0;i<calculations[id].count;i++)if(!lora_parameter_valid(id,i,x[i]))return INVALID;
 switch(id){
 case LORA_SYMBOL_T:y=ldexp(1.0,(int)x[0])/x[1];break;
 case LORA_SYMBOL_RATE:y=x[1]/ldexp(1.0,(int)x[0]);break;
 case LORA_BITRATE:ts=x[1]/ldexp(1.0,(int)x[0]);y=x[0]*ts*(4/(4+x[2]));break;
 case LORA_CODE_RATE:y=4/(4+x[0]);break;
 case LORA_CODE_INDEX:y=cr_index(x[0]);break;
 case LORA_PREAMBLE_T:ts=ldexp(1.0,(int)x[0])/x[1];y=(x[2]+4.25)*ts;break;
 case LORA_PAYLOAD_SYMBOLS:st=lora_payload_symbols(x,&n);if(st!=OK)return st;y=n;break;
 case LORA_AIRTIME:case LORA_PAYLOAD_T:
  ts=ldexp(1.0,(int)x[0])/x[1];six[0]=x[0];six[1]=x[2];six[2]=x[3];six[3]=x[4];six[4]=x[5];six[5]=x[id==LORA_AIRTIME?7:6];
  if(ts>.016&&six[5]!=1)return INVALID;
  st=lora_payload_symbols(six,&n);if(st!=OK)return st;y=(id==LORA_AIRTIME?x[6]+4.25+n:n)*ts;break;
 case LORA_DUTY:if(x[0]>x[1])return INVALID;y=x[0]/x[1];break;
 case LORA_PACKET_COUNT:
  if(x[2]==0){y=0;zero=1;break;}
  a=x[1]*x[2];a=a>0?a/x[0]:exp(log(x[1])+log(x[2])-log(x[0]));
  if(!isfinite(a)||a>9007199254740991.0)return RANGE;
  y=floor(a);zero=y==0;break;
 case LORA_TX_ENERGY:zero=x[0]==0||x[1]==0;y=zero?0:x[0]*x[1];break;
 default:return INVALID;
 }
 if(!isfinite(y)||(!zero&&y==0))return RANGE;
 *out=y;return OK;
}
