#include "lab_engine.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <float.h>
#define LP 3.14159265358979323846
static int integer(double v){return isfinite(v)&&v==floor(v);}
int lab_parameter_valid(CalcId id,unsigned i,double v){
 if(!isfinite(v))return 0;
 switch(id){
 case PCB_IPC_EXT:case PCB_IPC_INT:return i==0?(v>=.0001&&v<=.01):i==1?(v>=17e-6&&v<=105e-6):(v>=10&&v<=100);
 case PCB_MS_EPS:case PCB_MS_Z:return i==2?(v>=1&&v<=12):v>0;
 case PCB_DELAY:return i==0?v>=0:v>=1;
 case VNA_REFLECTED:case VNA_DELIVERED:case VNA_MISMATCH:return v>=0&&v<=1;
 case VNA_RL_GAMMA:return v>=0;
 case VNA_S21_DB:return 1;
 case VNA_Z_MAG:case VNA_Z_PHASE:return i==0?v>=0:1;
 case LAB_E12:case LAB_E24:return v>=1e-12&&v<=1e12;
 case LAB_SMD3:return integer(v)&&(v==0||(v>=100&&v<=999));
 case LAB_SMD4:return integer(v)&&(v==0||(v>=1000&&v<=9999));
 case LAB_TOL_R:case LAB_TOL_C:case LAB_TOL_L:return i==0?v>=0:(v>=0&&v<=1);
 case LAB_DIV_MIN:case LAB_DIV_MAX:return i==0?v>=0:i<3?v>0:(v>=0&&v<1);
 case LAB_RC_MIN:case LAB_RC_MAX:return i<2?v>0:(v>=0&&v<=1);
 case LAB_REL_ERROR:return i==0||v!=0;
 case LAB_ABS_ERROR:case LAB_MEAN:case LAB_POP_SD:case LAB_SAMPLE_SD:case LAB_MEAN_U:return 1;
 case LAB_RECT_U:case LAB_COMBINED_U:return v>=0;
 case LAB_EXPANDED_U:return i==0?v>=0:v>0;
 case LAB_FORMAT:return i==0?1:(integer(v)&&v>=1&&v<=12);
 default:return v>0;
 }
}
Status lab_microstrip(const double *x,double *effective,double *impedance){
 double u,a,b,e,f,z,t,u2,u4;
 if(!x||!effective||!impedance||!isfinite(x[0])||!isfinite(x[1])||!isfinite(x[2])||x[0]<=0||x[1]<=0||x[2]<1||x[2]>12)return INVALID;
 u=x[0]/x[1];if(u<.1||u>10)return INVALID;u2=u*u;u4=u2*u2;
 /* Stage floating calls: retained Windows TinyCC miscompiles nested double expressions. */
 a=u/52;a*=a;a+=u4;a/=u4+.432;a=log(a);a/=49;
 b=u/18.1;b=b*b*b;b=log(1+b);b/=18.7;a=1+a+b;
 b=(x[2]-.9)/(x[2]+3);b=log(b);b=exp(.053*b);b*=.564;
 t=log(1+10/u);t=exp((-a*b)*t);e=(x[2]-1)*t;e+=x[2]+1;e/=2;
 f=log(30.666/u);f=exp(.7528*f);f=exp(-f);f=6+(2*LP-6)*f;
 t=sqrt(1+4/u2);t+=f/u;t=log(t);z=sqrt(e);z=(376.730313668/(2*LP))*t/z;
 if(!isfinite(e)||!isfinite(z)||z<=0)return RANGE;
 *effective=e;*impedance=z;return OK;
}
static double preferred(double x,int e24){
 static const unsigned char e12[]={10,12,15,18,22,27,33,39,47,56,68,82};
 static const unsigned char e24v[]={10,11,12,13,15,16,18,20,22,24,27,30,33,36,39,43,47,51,56,62,68,75,82,91};
 const unsigned char *values=e24?e24v:e12;unsigned n=e24?24:12,i;int decade=(int)floor(log10(x)),j;double best=1e300,chosen=0,c,d;
 for(j=-1;j<=1;j++)for(i=0;i<n;i++){c=values[i]*pow(10.0,decade+j-1);d=fabs(log(c)-log(x));if(d<best-1e-14||(fabs(d-best)<=1e-14&&c>chosen)){best=d;chosen=c;}}
 return chosen;
}
static double stats(const double *x,int which,int *zero){
 double m=0,s=0,c=0,t,u,mean,ss=0;unsigned i;
 for(i=0;i<4;i++)if(fabs(x[i])>m)m=fabs(x[i]);
 if(m==0){*zero=1;return 0;}
 for(i=0;i<4;i++){u=x[i]/m;t=s+u;c+=fabs(s)>=fabs(u)?(s-t)+u:(u-t)+s;s=t;}
 mean=(s+c)/4;if(which==0){*zero=mean==0;return mean*m;}
 for(i=0;i<4;i++){u=x[i]/m-mean;ss+=u*u;}
 *zero=ss==0;u=sqrt(ss/(which==1?4:3));if(which==3)u/=2;return m*u;
}
int lab_details(CalcId id){return id==PCB_MS_EPS||id==PCB_MS_Z||id==LAB_TOL_R||id==LAB_TOL_C||id==LAB_TOL_L||id==LAB_FORMAT;}
int lab_list_only(CalcId id){return id==PCB_MS_EPS||id==PCB_MS_Z||id==LAB_E12||id==LAB_E24||id==LAB_SMD3||id==LAB_SMD4||id==LAB_POP_SD||id==LAB_SAMPLE_SD||id==LAB_MEAN_U||id==LAB_FORMAT;}
Status lab_calculate(CalcId id,const double *x,double *out){
 double y=0,a,b,m,z;int zero=0,sign;Status st;char buf[40];
 switch(id){
 case PCB_TRACE_R:y=exp(log(x[0])+log(x[1])-log(x[2])-log(x[3]));break;
 case PCB_VIA_R:m=fmax(x[2],x[3]);a=log(m)+log(1+fmin(x[2],x[3])/m);y=exp(log(x[0])+log(x[1])-log(LP)-log(x[3])-a);break;
 case PCB_IPC_EXT:case PCB_IPC_INT:a=log(x[0])+log(x[1])-2*log(.0000254);y=exp(log(id==PCB_IPC_EXT?.048:.024)+.44*log(x[2])+.725*a);break;
 case PCB_FOIL_T:y=x[0]/8960;break;case PCB_FOIL_M:y=x[0]*8960;break;
 case PCB_MS_EPS:case PCB_MS_Z:st=lab_microstrip(x,&a,&b);if(st!=OK)return st;y=id==PCB_MS_EPS?a:b;break;
 case PCB_DELAY:y=x[0]==0?0:exp(log(x[0])+.5*log(x[1])-log(299792458.0));zero=x[0]==0;break;
 case VNA_REFLECTED:y=x[0]*x[0];zero=x[0]==0;break;
 case VNA_DELIVERED:y=(1-x[0])*(1+x[0]);zero=x[0]==1;break;
 case VNA_MISMATCH:if(x[0]==1)return RANGE;a=x[0]*x[0];if(a<.01){double term=a,sum=a;unsigned i;for(i=2;i<=12;i++){term*=a;sum+=term/i;}y=(10/log(10.0))*sum;}else y=(-10/log(10.0))*(log(1-x[0])+log(1+x[0]));zero=x[0]==0;break;
 case VNA_RL_GAMMA:y=exp(-x[0]*(log(10.0)/20));break;
 case VNA_S21_DB:y=-x[0];zero=1;break;
 case VNA_S21_LIN:y=(-20/log(10.0))*log(x[0]);zero=x[0]==1;break;
 case VNA_Z_MAG:y=hypot(x[0],x[1]);zero=x[0]==0&&x[1]==0;break;
 case VNA_Z_PHASE:if(x[0]==0&&x[1]==0)return INVALID;y=atan2(x[1],x[0]);zero=x[1]==0;break;
 case LAB_E12:case LAB_E24:y=preferred(x[0],id==LAB_E24);break;
 case LAB_SMD3:case LAB_SMD4:a=floor(x[0]/10);b=fmod(x[0],10);y=a*pow(10.0,b);zero=x[0]==0;break;
 case LAB_TOL_R:case LAB_TOL_C:case LAB_TOL_L:if(x[0]>DBL_MAX/(1+x[1]))return RANGE;y=x[0]*(1-x[1]);zero=x[0]==0||x[1]==1;break;
 case LAB_DIV_MIN:case LAB_DIV_MAX:
  sign=id==LAB_DIV_MIN?-1:1;a=log(x[1])+log(1-sign*x[3]);b=log(x[2])+log(1+sign*x[4]);m=fmax(a,b);z=m+log(exp(a-m)+exp(b-m));y=x[0]==0?0:exp(log(x[0])+b-z);zero=x[0]==0;break;
 case LAB_RC_MIN:case LAB_RC_MAX:
  sign=id==LAB_RC_MIN?-1:1;zero=sign==-1&&(x[2]==1||x[3]==1);y=zero?0:exp(log(x[0])+log(x[1])+log(1+sign*x[2])+log(1+sign*x[3]));break;
 case LAB_ABS_ERROR:y=fabs(x[0]-x[1]);zero=x[0]==x[1];break;
 case LAB_REL_ERROR:m=fmax(fabs(x[0]),fabs(x[1]));a=fabs(x[1]/m);y=fabs(x[0]/m-x[1]/m)/a;zero=x[0]==x[1];break;
 case LAB_MEAN:case LAB_POP_SD:case LAB_SAMPLE_SD:case LAB_MEAN_U:
  y=stats(x,id==LAB_MEAN?0:id==LAB_POP_SD?1:id==LAB_SAMPLE_SD?2:3,&zero);break;
 case LAB_RECT_U:y=x[0]/sqrt(3.0);zero=x[0]==0;break;
 case LAB_COMBINED_U:a=hypot(x[0],x[1]);y=hypot(a,x[2]);zero=x[0]==0&&x[1]==0&&x[2]==0;break;
 case LAB_EXPANDED_U:y=x[0]*x[1];zero=x[0]==0;break;
 case LAB_FORMAT:snprintf(buf,sizeof(buf),"%.*g",(int)x[1],x[0]);errno=0;y=strtod(buf,0);if(errno==ERANGE||(y!=0&&fabs(y)<DBL_MIN))return RANGE;zero=x[0]==0;break;
 default:return INVALID;
 }
 if(!isfinite(y)||(!zero&&y==0))return RANGE;
 *out=y;return OK;
}
