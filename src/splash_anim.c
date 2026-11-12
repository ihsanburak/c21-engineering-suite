#include "splash_anim.h"
#include "display.h"
#include <stdint.h>

/* Original Eagle pixels arrive from below; no logo tracing or replacement. */
static uint32_t scatter(unsigned x,unsigned y){uint32_t v=x*374761393u+y*668265263u+0xC21u;v=(v^(v>>13))*1274126177u;return v^(v>>16);}
static void dot(int x,int y){if(x>=0&&x<320&&y>=0&&y<240)framebuffer[y*320+x]=1;}
typedef struct {unsigned char x,y,move;} Pen;
/* Small original handwriting-style suffix, independent of the Eagle bitmap. */
static const Pen suffix[]={
 {215,184,1},{215,185,0},
 {227,178,1},{225,177,0},{222,178,0},{220,181,0},{221,184,0},{224,185,0},{227,183,0},
 {228,181,0},{230,178,0},{233,177,0},{236,179,0},{235,183,0},{232,185,0},{229,184,0},{228,181,0},{234,181,0},{238,178,0},
 {239,184,0},{240,180,0},{242,177,0},{244,178,0},{244,183,0},{246,179,0},{248,177,0},{250,178,0},{250,183,0},{254,181,0}
};
static int magnitude(int v){return v<0?-v:v;}
static void handwriting(unsigned frame){
 static const unsigned char bayer[]={0,8,2,10,12,4,14,6,3,11,1,9,15,7,13,5};
 unsigned i,total=0,visited=0,budget,density,t=frame-35+1;
 for(i=1;i<sizeof(suffix)/sizeof(*suffix);i++)if(!suffix[i].move){int dx=magnitude((int)suffix[i].x-suffix[i-1].x),dy=magnitude((int)suffix[i].y-suffix[i-1].y);total+=(unsigned)(dx>dy?dx:dy)+1;}
 budget=t*total/17;if(budget>total)budget=total;
 density=t*2;if(density>16)density=16;
 for(i=1;i<sizeof(suffix)/sizeof(*suffix);i++)if(!suffix[i].move){
  int x=suffix[i-1].x,y=suffix[i-1].y,x1=suffix[i].x,y1=suffix[i].y;
  int dx=magnitude(x1-x),dy=-magnitude(y1-y),sx=x<x1?1:-1,sy=y<y1?1:-1,err=dx+dy;
  for(;;){int twice;visited++;if(visited<=budget&&bayer[(y&3)*4+(x&3)]<density)dot(x,y);if(x==x1&&y==y1)break;twice=2*err;if(twice>=dy){err+=dy;x+=sx;}if(twice<=dx){err+=dx;y+=sy;}}
 }
}
void splash_anim_draw(const App *app,unsigned frame){
 const unsigned char *bits=app_splash_bitmap();unsigned x,y;
 if(frame>=SPLASH_ANIM_FRAMES)frame=SPLASH_ANIM_FRAMES-1;
 if(frame>=35){app_draw(app);handwriting(frame);return;}
 clear_screen();
 for(y=0;y<240;y++)for(x=0;x<320;x++)if(bits[(y*320+x)/8]&(128u>>(x&7))){
  uint32_t h=scatter(x,y);unsigned arrival=20+h%15,delay=(h>>5)%5;
  if(frame>=arrival)dot((int)x,(int)y);
  else if((h&7)==0&&frame>=delay){
   int left=(int)(arrival-frame),duration=(int)(arrival-delay),start=240+(int)((h>>9)%64);
   int pos=(int)y+(start-(int)y)*left*left/(duration*duration);
   dot((int)x,pos);if((h&31)==0)dot((int)x,pos+1);
  }
 }
}
