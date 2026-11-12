/* Compile the real native adapter and loop; only OS/SDL boundaries are stubbed. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ui.h"
#include "settings.h"
static unsigned checks,dispatches,flips,waits,hold_samples,delays,hold_polls;
static void observed_app_key(App *a,int k);
#define main c21_native_entry
#define app_key observed_app_key
#include "../src/main.c"
#undef main
#undef app_key
#define CHECK(x) do {checks++;if(!(x)){fprintf(stderr,"NATIVE INPUT FAIL %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static const t_key *held1,*held2;
static int loop_mode,both_keys;
int has_colors=0;
static int same_key(const t_key *a,const t_key *b){return a&&b&&a->row==b->row&&a->col==b->col&&a->tpad_row==b->tpad_row&&a->tpad_col==b->tpad_col&&a->tpad_arrow==b->tpad_arrow;}
int (isKeyPressed)(const t_key *k){return same_key(k,held1)||same_key(k,held2);}
static int poll(const t_key *one,const t_key *two){int k;held1=one;held2=two;k=read_key();held1=held2=0;return k;}
static void observed_app_key(App *a,int k){dispatches++;app_key(a,k);if(loop_mode){CHECK(a->screen==S_MENU);if(loop_mode==1){CHECK(a->depth==(dispatches==2?1:0));CHECK(a->running==(dispatches<4));}else{CHECK(a->depth==0&&a->running==(dispatches<2));}}}
void wait_no_key_pressed(void){
 unsigned j,before=dispatches;waits++;
 /* Model a held press until release; SDK wait/Touchpad I2C are not emulated. */
 if(loop_mode&&held1)for(j=0;j<5;j++){CHECK(read_key()==((same_key(held1,&KEY_NSPIRE_CLICK)||same_key(held1,&KEY_NSPIRE_ENTER))?K_ENTER:K_ESC));CHECK(dispatches==before);hold_samples++;}
 held1=held2=0;
}
int SDL_Init(unsigned f){(void)f;return 0;}
SDL_Surface *SDL_SetVideoMode(int w,int h,int bpp,unsigned f){static SDL_PixelFormat fmt;static SDL_Surface s={&fmt};CHECK(w==320&&h==240&&bpp==8);(void)f;return &s;}
Uint32 SDL_MapRGB(SDL_PixelFormat *f,unsigned char r,unsigned char g,unsigned char b){(void)f;(void)g;(void)b;return r;}
int SDL_FillRect(SDL_Surface *s,SDL_Rect *r,Uint32 c){(void)s;(void)r;(void)c;return 0;}
int SDL_Flip(SDL_Surface *s){(void)s;CHECK(loop_mode);flips++;if(loop_mode==2){CHECK(flips<=SPLASH_ANIM_FRAMES+1);held1=flips>SPLASH_ANIM_FRAMES?&KEY_NSPIRE_ESC:0;held2=0;}else{CHECK(flips<=4);held1=flips<=2?&KEY_NSPIRE_CLICK:&KEY_NSPIRE_ESC;held2=both_keys&&flips<=2?&KEY_NSPIRE_ENTER:0;}return 0;}
void SDL_Delay(unsigned ms){CHECK(loop_mode==2);CHECK(dispatches==0);if(ms==SPLASH_ANIM_FRAME_MS)delays++;else{CHECK(ms==20&&flips==SPLASH_ANIM_FRAMES);hold_polls++;if(hold_polls==5)held1=&KEY_NSPIRE_CLICK;}}
void SDL_Quit(void){}
typedef struct {const t_key *key;int action;} Mapping;
static const Mapping mappings[]={
 {&KEY_NSPIRE_UP,K_UP},{&KEY_NSPIRE_DOWN,K_DOWN},{&KEY_NSPIRE_LEFT,K_LEFT},{&KEY_NSPIRE_RIGHT,K_RIGHT},
 {&KEY_NSPIRE_ENTER,K_ENTER},{&KEY_NSPIRE_CLICK,K_ENTER},{&KEY_NSPIRE_ESC,K_ESC},
 {&KEY_NSPIRE_S,K_SETTINGS},{&KEY_NSPIRE_F,K_MODE},{&KEY_NSPIRE_C,K_CALCULATE},
 {&KEY_NSPIRE_U,K_UNITS},{&KEY_NSPIRE_O,K_TARGET},{&KEY_NSPIRE_TAB,K_TAB},{&KEY_NSPIRE_H,K_INFO},
 {&KEY_NSPIRE_DEL,K_DELETE},{&KEY_NSPIRE_PERIOD,K_DOT},{&KEY_NSPIRE_PLUS,K_PLUS},
 {&KEY_NSPIRE_NEGATIVE,K_SIGN},{&KEY_NSPIRE_MINUS,K_SIGN},{&KEY_NSPIRE_EE,K_EXP},{&KEY_NSPIRE_eEXP,K_EXP},
 {&KEY_NSPIRE_0,K_DIGIT0},{&KEY_NSPIRE_1,K_DIGIT0+1},{&KEY_NSPIRE_2,K_DIGIT0+2},
 {&KEY_NSPIRE_3,K_DIGIT0+3},{&KEY_NSPIRE_4,K_DIGIT0+4},{&KEY_NSPIRE_5,K_DIGIT0+5},
 {&KEY_NSPIRE_6,K_DIGIT0+6},{&KEY_NSPIRE_7,K_DIGIT0+7},{&KEY_NSPIRE_8,K_DIGIT0+8},{&KEY_NSPIRE_9,K_DIGIT0+9}};
static const t_key *physical(int k,int click){unsigned i;if(k==K_ENTER)return click?&KEY_NSPIRE_CLICK:&KEY_NSPIRE_ENTER;for(i=0;i<sizeof(mappings)/sizeof(*mappings);i++)if(mappings[i].action==k)return mappings[i].key;CHECK(0);return 0;}
static int event(int k,int click){if(k==K_CLEAR)return poll(&KEY_NSPIRE_CTRL,&KEY_NSPIRE_DEL);return poll(physical(k,click),0);}
static void paired(App *a,App *b,int k){app_key(a,event(k,0));app_key(b,event(k,1));CHECK(memcmp(a,b,sizeof(*a))==0);}
static void type_pair(App *a,App *b,const char *s){for(;*s;s++)paired(a,b,K_DIGIT0+*s-'0');}
static void flow(void){
 App a,b,before;app_init(&a);app_init(&b);settings_bind(0);
 paired(&a,&b,K_ENTER);CHECK(a.screen==S_MENU&&a.depth==0);
 paired(&a,&b,K_DOWN);paired(&a,&b,K_ENTER);CHECK(a.screen==S_MENU&&a.depth==1);
 paired(&a,&b,K_ENTER);CHECK(a.screen==S_MENU&&a.depth==2);
 paired(&a,&b,K_DOWN);paired(&a,&b,K_ENTER);CHECK(a.calc==CURRENT&&a.screen==S_FORM);
 paired(&a,&b,K_ENTER);CHECK(a.screen==S_EDIT);type_pair(&a,&b,"12");paired(&a,&b,K_ENTER);CHECK(!strcmp(a.values[0],"12"));
 paired(&a,&b,K_DOWN);paired(&a,&b,K_ENTER);type_pair(&a,&b,"100");paired(&a,&b,K_ENTER);
 paired(&a,&b,K_LEFT);CHECK(a.screen==S_UNITS);paired(&a,&b,K_DOWN);paired(&a,&b,K_UP);paired(&a,&b,K_ENTER);CHECK(a.screen==S_FORM&&a.units[1]==0);
 /* List action rows: output units, Calculate, Info. */
 paired(&a,&b,K_DOWN);paired(&a,&b,K_DOWN);paired(&a,&b,K_ENTER);CHECK(a.result_valid&&fabs(a.result-.12)<1e-12);
 paired(&a,&b,K_ESC);paired(&a,&b,K_MODE);CHECK(a.screen==S_FORMULA);
 paired(&a,&b,K_RIGHT);paired(&a,&b,K_ENTER);CHECK(a.screen==S_EDIT);
 paired(&a,&b,K_CLEAR);type_pair(&a,&b,"200");paired(&a,&b,K_ENTER);paired(&a,&b,K_CALCULATE);CHECK(a.result_valid&&fabs(a.result-.06)<1e-12);
 before=a;paired(&a,&b,K_INFO);CHECK(a.screen==S_INFO);paired(&a,&b,K_RIGHT);paired(&a,&b,K_ENTER);CHECK(a.screen==before.screen&&!strcmp(a.values[1],"200"));
 paired(&a,&b,K_TARGET);CHECK(a.screen==S_TARGET);paired(&a,&b,K_DOWN);paired(&a,&b,K_ENTER);CHECK(a.screen==S_FORMULA);
 before=a;paired(&a,&b,K_SETTINGS);CHECK(a.screen==S_SETTINGS);paired(&a,&b,K_ENTER);CHECK(a.screen==S_LANGUAGE);paired(&a,&b,K_ENTER);paired(&a,&b,K_ESC);paired(&a,&b,K_ESC);CHECK(a.screen==before.screen&&!strcmp(a.values[1],before.values[1]));
}
int main(void){
 unsigned i;CHECK(poll(0,0)==K_NONE);CHECK(poll(&KEY_NSPIRE_ENTER,&KEY_NSPIRE_CLICK)==K_ENTER);
 for(i=0;i<sizeof(mappings)/sizeof(*mappings);i++)CHECK(poll(mappings[i].key,0)==mappings[i].action);
 CHECK(poll(&KEY_NSPIRE_CTRL,&KEY_NSPIRE_DEL)==K_CLEAR);CHECK(poll(&KEY_NSPIRE_CTRL,&KEY_NSPIRE_LEFT)==K_HOME);CHECK(poll(&KEY_NSPIRE_CTRL,&KEY_NSPIRE_RIGHT)==K_END);
 CHECK(poll(&KEY_NSPIRE_SHIFT,&KEY_NSPIRE_DEL)==K_FORWARD_DELETE);CHECK(poll(&KEY_NSPIRE_SHIFT,&KEY_NSPIRE_U)==K_OUTPUT_UNITS);CHECK(poll(&KEY_NSPIRE_ESC,&KEY_NSPIRE_CLICK)==K_ESC);
 CHECK(poll(&KEY_NSPIRE_CLICK,&KEY_NSPIRE_DOWN)==K_ENTER);
 flow();
 for(i=0;i<2;i++){loop_mode=1;both_keys=(int)i;flips=waits=dispatches=hold_samples=0;CHECK(c21_native_entry(0,0)==0);CHECK(flips==4&&dispatches==4&&waits==5&&hold_samples==20);loop_mode=0;}
 loop_mode=2;flips=waits=dispatches=hold_samples=delays=hold_polls=0;CHECK(c21_native_entry(0,0)==0);CHECK(flips==SPLASH_ANIM_FRAMES+1&&dispatches==2&&waits==3&&delays==SPLASH_ANIM_FRAMES-1&&hold_polls==5&&hold_samples==10);loop_mode=0;
 printf("PASS: %u native input checks (real adapter/loop, Enter-click equivalence, modeled hold/release; not hardware emulation)\n",checks);return 0;
}
