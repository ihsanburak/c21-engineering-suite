#include "rf_engine.h"
#include <math.h>
#include <float.h>
#define RF_PI 3.14159265358979323846
#define RF_C 299792458.0
#define RF_KB 1.380649e-23
#define RF_RE 6371000.0
static double sum_db(const double *x,unsigned n){double m=0,s=0,c=0,t,u;unsigned i;for(i=0;i<n;i++)if(fabs(x[i])>m)m=fabs(x[i]);if(m==0)return 0;for(i=0;i<n;i++){u=x[i]/m;t=s+u;c+=fabs(s)>=fabs(u)?(s-t)+u:(u-t)+s;s=t;}return (s+c)*m;}
static double exp_minus_one(double x){double t=x,s=x;unsigned i;if(x>=.01)return exp(x)-1;for(i=2;i<=12;i++){t*=x/i;s+=t;}return s;}
static double noise_log(const double *x){double a=log(x[0]),f=x[2]*(log(10.0)/10),b,m,t;
 if(f>0){b=f>50?log(290.0)+f:log(290.0)+log(exp_minus_one(f));m=fmax(a,b);t=exp(fmin(a,b)-m);a=m+log(1+t);}
 return log(RF_KB)+log(x[1])+a;
}
static double log_one_plus(double x){double t=x,s=x;unsigned i;if(x>=.01)return log(1+x);for(i=2;i<=12;i++){t*=-x;s+=t/i;}return s;}
static double friis_log(const double *x){
 double a=log(x[0]),b=0,c=0,m,s=0;int top=0;
 if(x[1]>1)b=log(x[1]-1)-log(x[3]);
 if(x[2]>1)c=log(x[2]-1)-log(x[3])-log(x[4]);
 m=a;if(x[1]>1&&b>m){m=b;top=1;}if(x[2]>1&&c>m){m=c;top=2;}
 if(top!=0)s+=exp(a-m);
 if(top!=1&&x[1]>1)s+=exp(b-m);
 if(top!=2&&x[2]>1)s+=exp(c-m);
 a=log_one_plus(s);return m+a;
}
int rf_parameter_valid(CalcId id,unsigned i,double v){
 if(!isfinite(v))return 0;
 switch(id){
 case RF_DBW_DBM:case RF_DBM_DBW:case RF_DBW_W:case RF_GAIN_RATIO:case RF_AMP_RATIO:case RF_GAIN_SUM:case RF_ERP:case RF_FROM_ERP:case RF_DBI:case RF_DBD:case RF_SENS:case RF_MARGIN:return 1;
 case RF_VRMS:return i==0?v>=0:v>0;
 case RF_LOSS_SUM:return v>=0;
 case RF_EIRP:return i!=1||v>=0;
 case RF_NOISE_W:case RF_NOISE_DBM:return i==2?v>=0:v>0;
 case RF_NOISE_SNR:return i==0?v>0:1;
 case RF_NF_FACTOR:return v>=0;
 case RF_FACTOR_NF:return v>=1;
 case RF_FRIIS_F:case RF_FRIIS_NF:return i<3?v>=1:v>0;
 case RF_SPEED:return v>=1;
 case RF_DIEL_WAVE:return i==1?v>=1:v>0;
 case RF_LINK:return i==1||i==4||i==5?v>=0:1;
 case RF_CLEAR:return i==0?v>=0:v>0;
 case RF_HORIZON:return i==2?v>0:v>=0;
 case RF_POL:return v>=0&&v<=RF_PI/2;
 case RF_PHASE:case RF_DELAY:case RF_LP:case RF_HP:return i==0?v>=0:v>0;
 case RF_REFLECT:return i==0?v>=0:i==1?1:v>0;
 case RF_VSWR:case RF_RETURN:return v>=0&&v<=1;
 case RF_IMPEDANCE:return i<2?fabs(v)<1:v>0;
 default:return v>0;
 }
}
Status rf_reflection(const double *x,double *re,double *im){double m,a,b,c,d;
 if(!x||!re||!im||!isfinite(x[0])||!isfinite(x[1])||!isfinite(x[2])||x[0]<0||x[2]<=0)return INVALID;
 m=fabs(x[0]);if(fabs(x[1])>m)m=fabs(x[1]);if(x[2]>m)m=x[2];a=x[0]/m;b=x[1]/m;c=x[2]/m;d=(a+c)*(a+c)+b*b;
 if(d==0)return RANGE;
 *re=((a-c)*(a+c)+b*b)/d;*im=2*c*b/d;return OK;
}
Status rf_impedance(const double *x,double *re,double *im){double n,d;
 if(!x||!re||!im||!isfinite(x[0])||!isfinite(x[1])||!isfinite(x[2])||x[2]<=0)return INVALID;
 n=x[0]*x[0]+x[1]*x[1];if(n>=1)return INVALID;d=(1-x[0])*(1-x[0])+x[1]*x[1];if(d==0)return INVALID;
 *re=x[2]*((1-n)/d);*im=x[2]*((2*x[1])/d);if(!isfinite(*re)||!isfinite(*im)||*re==0||(*im==0&&x[1]!=0))return RANGE;return OK;
}
Status rf_lmatch(const double *x,int hp,double *series,double *shunt){double hi,lo,q,w,xs,xp;
 if(!x||!series||!shunt||!isfinite(x[0])||!isfinite(x[1])||!isfinite(x[2])||x[0]<=0||x[1]<=0||x[2]<=0||x[0]==x[1])return INVALID;
 hi=fmax(x[0],x[1]);lo=fmin(x[0],x[1]);q=.5*(log(hi-lo)-log(lo));w=log(2*RF_PI)+log(x[2]);xs=q+log(lo);xp=log(hi)-q;
 *series=exp(hp?-w-xs:xs-w);*shunt=exp(hp?xp-w:-w-xp);
 return !isfinite(*series)||!isfinite(*shunt)||*series==0||*shunt==0?RANGE:OK;
}
int rf_has_details(CalcId id){return id==RF_REFLECT||id==RF_IMPEDANCE||id==RF_LMATCH_LP||id==RF_LMATCH_HP;}
int rf_regulatory(CalcId id){return id==RF_EIRP||id==RF_ERP||id==RF_FROM_ERP||id==RF_LINK;}
Status rf_calculate(CalcId id,const double *x,double *out){double y=0,a=0,b=0,d=0,t[6];int zero=0;Status st;
 switch(id){
 case RF_W_DBW:st=calculate(MW_DBM,x,&a);if(st!=OK)return st;y=a-30;zero=1;break;
 case RF_DBW_W:t[0]=x[0]+30;return calculate(DBM_MW,t,out);
 case RF_DBW_DBM:y=x[0]+30;zero=1;break;case RF_DBM_DBW:y=x[0]-30;zero=1;break;
 case RF_GAIN_DB:y=(10/log(10.0))*log(x[0]);zero=1;break;
 case RF_GAIN_RATIO:y=exp(x[0]*(log(10.0)/10));break;
 case RF_AMP_DB:y=(20/log(10.0))*log(x[0]);zero=1;break;
 case RF_AMP_RATIO:y=exp(x[0]*(log(10.0)/20));break;
 case RF_POWER_RATIO:y=exp(log(x[0])-log(x[1]));break;
 case RF_VRMS:y=x[0]==0?0:exp(.5*(log(x[0])+log(x[1])));zero=x[0]==0;break;
 case RF_GAIN_SUM:case RF_LOSS_SUM:y=sum_db(x,3);zero=1;break;
 case RF_EIRP:t[0]=x[0];t[1]=-x[1];t[2]=x[2];y=sum_db(t,3);zero=1;break;
 case RF_ERP:case RF_DBD:y=x[0]-2.15;zero=1;break;
 case RF_FROM_ERP:case RF_DBI:y=x[0]+2.15;zero=1;break;
 case RF_NOISE_W:a=noise_log(x);y=exp(a);break;
 case RF_NOISE_DBM:a=noise_log(x);y=(10/log(10.0))*a+30;zero=1;break;
 case RF_SNR:y=(10/log(10.0))*(log(x[0])-log(x[1]));zero=1;break;
 case RF_NOISE_SNR:y=exp(log(x[0])-x[1]*(log(10.0)/10));break;
 case RF_NF_FACTOR:y=exp(x[0]*(log(10.0)/10));break;
 case RF_FACTOR_NF:y=(10/log(10.0))*log(x[0]);zero=1;break;
 case RF_FRIIS_F:case RF_FRIIS_NF:a=friis_log(x);y=id==RF_FRIIS_F?exp(a):(10/log(10.0))*a;zero=id==RF_FRIIS_NF&&x[0]==1&&x[1]==1&&x[2]==1;break;
 case RF_SENS:y=x[0]+x[1];zero=1;break;
 case RF_HALF:y=exp(log(RF_C)-log(2.0)-log(x[0]));break;
 case RF_SPEED:y=exp(log(RF_C)-.5*log(x[0]));break;
 case RF_DIEL_WAVE:y=exp(log(RF_C)-log(x[0])-.5*log(x[1]));break;
 case RF_LINK:t[0]=x[0];t[1]=-x[1];t[2]=x[2];t[3]=x[3];t[4]=-x[4];t[5]=-x[5];y=sum_db(t,6);zero=1;break;
 case RF_MARGIN:y=x[0]-x[1];zero=1;break;
 case RF_CLEAR:y=x[0]/x[1];zero=x[0]==0;break;
 case RF_HORIZON:a=log(RF_RE)+log(x[2]);if((x[0]>0&&log(x[0])>log(.01)+a)||(x[1]>0&&log(x[1])>log(.01)+a))return INVALID;b=x[0]==0?0:exp(.5*(log(2.0)+a+log(x[0])));d=x[1]==0?0:exp(.5*(log(2.0)+a+log(x[1])));y=b+d;zero=x[0]==0&&x[1]==0;break;
 case RF_POL:if(x[0]==RF_PI/2)return RANGE;y=(-20/log(10.0))*log(cos(x[0]));zero=1;break;
 case RF_PHASE:case RF_DELAY:y=x[0]==0?0:exp(log(2*RF_PI)+log(x[0])-log(x[1]));if(id==RF_DELAY)y=-y;zero=x[0]==0;break;
 case RF_RC_CUTOFF:y=exp(-log(2*RF_PI)-log(x[0])-log(x[1]));break;
 case RF_LP:case RF_HP:a=fmax(x[0],x[1]);b=x[0]/a;d=x[1]/a;a=sqrt(b*b+d*d);y=id==RF_LP?d/a:b/a;zero=id==RF_HP&&x[0]==0;break;
 case RF_SERIES_Q:y=exp(.5*(log(x[1])-log(x[2]))-log(x[0]));break;
 case RF_SERIES_BW:y=exp(log(x[0])-log(2*RF_PI)-log(x[1]));break;
 case RF_REFLECT:st=rf_reflection(x,&a,&b);if(st!=OK)return st;y=hypot(a,b);if(y>1&&y<=1+8*DBL_EPSILON)y=1;zero=1;break;
 case RF_VSWR:if(x[0]==1)return RANGE;y=(1+x[0])/(1-x[0]);break;
 case RF_RETURN:if(x[0]==0)return RANGE;y=(-20/log(10.0))*log(x[0]);zero=x[0]==1;break;
 case RF_IMPEDANCE:st=rf_impedance(x,&a,&b);if(st!=OK)return st;y=a;break;
 case RF_LMATCH_LP:case RF_LMATCH_HP:st=rf_lmatch(x,id==RF_LMATCH_HP,&a,&b);if(st!=OK)return st;y=a;break;
 default:return INVALID;
 }
 if(!isfinite(y)||(!zero&&y==0))return RANGE;
 *out=y;return OK;
}
