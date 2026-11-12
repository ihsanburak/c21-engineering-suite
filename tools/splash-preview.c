/* Actual C animation framebuffer transport; not hardware emulation. */
#include <stdio.h>
#include <limits.h>
#include "ui.h"
#include "display.h"
#include "splash_anim.h"
int main(void){App a;unsigned f;app_init(&a);while(scanf("%u",&f)==1){if(f==UINT_MAX){a.screen=S_MENU;app_draw(&a);}else{a.screen=S_SPLASH;splash_anim_draw(&a,f);}if(fwrite(framebuffer,1,sizeof(framebuffer),stdout)!=sizeof(framebuffer)||fflush(stdout))return 1;}return 0;}
