/* PC UI preview transport. This executable does not emulate the ARM/OS runtime. */
#include <stdio.h>
#include "ui.h"
#include "display.h"
#include "settings.h"
static int trace_state;
static int send_frame(const App *app){
 /* Bit 0: running; bit 1: Turkish, bit 2: ASCII, for preview chrome only. */
 unsigned char running=(unsigned char)(app->running|(loc_get()!=LANG_EN?2:0)|(loc_get()==LANG_TR_ASCII?4:0));
 app_draw(app);
 if(fwrite(&running,1,1,stdout)!=1)return 0;
 if(fwrite(framebuffer,1,sizeof(framebuffer),stdout)!=sizeof(framebuffer))return 0;
 if(fflush(stdout)!=0)return 0;
 if(trace_state){fprintf(stderr,"{\"screen\":%d,\"menu\":%d,\"calc\":%d,\"valid\":%d,\"result\":%.17g}\n",app->screen,app->menu,app->calc,app->result_valid,app->result);fflush(stderr);}
 return 1;
}
int main(int argc,char **argv){App app;int key;(void)argv;trace_state=argc>2;app_init(&app);settings_bind(argc>1?0:"build/c21-host-preview.language.cfg");loc_set(settings_load());if(!send_frame(&app))return 1;
 while(app.running&&scanf("%d",&key)==1){app_key(&app,key);if(!send_frame(&app))return 1;}
 return 0;
}
