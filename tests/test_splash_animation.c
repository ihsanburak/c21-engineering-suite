#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "ui.h"
#include "display.h"
#include "splash_anim.h"
static unsigned checks;
static unsigned char base[76800],saved[76800];
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"SPLASH FAIL %d: %s\n",__LINE__,#x);exit(1);}}while(0)
int main(void){App a,old;unsigned l,f,i,ink,outside,suffix;const unsigned char *bits=app_splash_bitmap();
 for(l=0;l<3;l++){
  app_init(&a);loc_set((Language)l);old=a;app_draw(&a);memcpy(base,framebuffer,sizeof(base));
  for(f=0;f<SPLASH_ANIM_FRAMES;f++){
   splash_anim_draw(&a,f);CHECK(!memcmp(&a,&old,sizeof(a)));CHECK(!display_clipped_pixels&&!display_substitutions);
   memcpy(saved,framebuffer,sizeof(saved));splash_anim_draw(&a,f);CHECK(!memcmp(saved,framebuffer,sizeof(saved)));
   ink=0;for(i=0;i<76800;i++){if(framebuffer[i])ink++;if(framebuffer[i]>1){CHECK(0);}}
   if(f==0)CHECK(ink==0);
   if(f==15)CHECK(ink>0);
   if(f==SPLASH_ANIM_FRAMES-1){
    outside=suffix=0;for(i=0;i<76800;i++){unsigned x=i%320,y=i/320;if(x>=214&&x<=255&&y>=176&&y<=186){CHECK(base[i]==0);suffix+=framebuffer[i];}else if(framebuffer[i]!=base[i])outside++;}
    CHECK(outside==0&&suffix>35&&suffix<150);
   }
  }
  splash_anim_draw(&a,UINT_MAX);CHECK(!memcmp(saved,framebuffer,sizeof(saved)));
  app_key(&a,K_ENTER);CHECK(a.screen==S_MENU);app_draw(&a);CHECK(!display_clipped_pixels);
 }
 /* Original bitmap remains identical to its original decoder after every frame. */
 app_init(&a);app_draw(&a);outside=0;for(i=0;i<76800;i++)if(framebuffer[i]!=!!(bits[i/8]&(128u>>(i&7))))outside++;CHECK(!outside);
 printf("PASS: %u splash animation checks (56 frames, 3 languages, original bitmap retained, bounded deterministic rendering)\n",checks);return 0;
}
