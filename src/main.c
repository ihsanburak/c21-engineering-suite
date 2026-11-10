#include <os.h>
#include <SDL/SDL.h>
#include "ui.h"
#include "display.h"
#include "settings.h"
static int read_key(void){
 static const t_key *digits[]={&KEY_NSPIRE_0,&KEY_NSPIRE_1,&KEY_NSPIRE_2,&KEY_NSPIRE_3,&KEY_NSPIRE_4,&KEY_NSPIRE_5,&KEY_NSPIRE_6,&KEY_NSPIRE_7,&KEY_NSPIRE_8,&KEY_NSPIRE_9};
 unsigned i;
 if(isKeyPressed(KEY_NSPIRE_CTRL)&&isKeyPressed(KEY_NSPIRE_DEL))return K_CLEAR;
 if(isKeyPressed(KEY_NSPIRE_CTRL)&&isKeyPressed(KEY_NSPIRE_LEFT))return K_HOME;
 if(isKeyPressed(KEY_NSPIRE_CTRL)&&isKeyPressed(KEY_NSPIRE_RIGHT))return K_END;
 if(isKeyPressed(KEY_NSPIRE_SHIFT)&&isKeyPressed(KEY_NSPIRE_DEL))return K_FORWARD_DELETE;
 if(isKeyPressed(KEY_NSPIRE_SHIFT)&&isKeyPressed(KEY_NSPIRE_U))return K_OUTPUT_UNITS;
 if(isKeyPressed(KEY_NSPIRE_S))return K_SETTINGS;
 if(isKeyPressed(KEY_NSPIRE_F))return K_MODE;
 if(isKeyPressed(KEY_NSPIRE_C))return K_CALCULATE;
 if(isKeyPressed(KEY_NSPIRE_U))return K_UNITS;
 if(isKeyPressed(KEY_NSPIRE_O))return K_TARGET;
 if(isKeyPressed(KEY_NSPIRE_TAB))return K_TAB;
 if(isKeyPressed(KEY_NSPIRE_ESC))return K_ESC;
 /* Ndless distinguishes the keyboard Enter key from a central Touchpad press. */
 if(isKeyPressed(KEY_NSPIRE_ENTER)||isKeyPressed(KEY_NSPIRE_CLICK))return K_ENTER;
 if(isKeyPressed(KEY_NSPIRE_UP))return K_UP;
 if(isKeyPressed(KEY_NSPIRE_DOWN))return K_DOWN;
 if(isKeyPressed(KEY_NSPIRE_LEFT))return K_LEFT;
 if(isKeyPressed(KEY_NSPIRE_RIGHT))return K_RIGHT;
 if(isKeyPressed(KEY_NSPIRE_H))return K_INFO;
 if(isKeyPressed(KEY_NSPIRE_DEL))return K_DELETE;
 if(isKeyPressed(KEY_NSPIRE_PERIOD))return K_DOT;
 if(isKeyPressed(KEY_NSPIRE_PLUS))return K_PLUS;
 if(isKeyPressed(KEY_NSPIRE_NEGATIVE)||isKeyPressed(KEY_NSPIRE_MINUS))return K_SIGN;
 if(isKeyPressed(KEY_NSPIRE_EE)||isKeyPressed(KEY_NSPIRE_eEXP))return K_EXP;
 for(i=0;i<10;i++)if((isKeyPressed)(digits[i]))return K_DIGIT0+(int)i;
 return K_NONE;
}
int main(int argc,char **argv){
 SDL_Surface *screen;App app;int x,y,key;Uint32 white,black;
 if(SDL_Init(SDL_INIT_VIDEO)<0)return 1;
 screen=SDL_SetVideoMode(320,240,has_colors?16:8,SDL_SWSURFACE);
 if(!screen){SDL_Quit();return 1;}
 white=SDL_MapRGB(screen->format,255,255,255);black=SDL_MapRGB(screen->format,0,0,0);
 app_init(&app);if(argc>0&&settings_bind_executable(argv[0]))loc_set(settings_load());wait_no_key_pressed();
 while(app.running){
  app_draw(&app);SDL_FillRect(screen,0,white);
  /* Run-length rectangles work with both monochrome and color SDL surfaces. */
  for(y=0;y<240;y++)for(x=0;x<320;){int start=x;SDL_Rect r;while(x<320&&!framebuffer[y*320+x])x++;start=x;while(x<320&&framebuffer[y*320+x])x++;if(x>start){r.x=(Sint16)start;r.y=(Sint16)y;r.w=(Uint16)(x-start);r.h=1;SDL_FillRect(screen,&r,black);}}
  SDL_Flip(screen);
  do{key=read_key();if(key==K_NONE)SDL_Delay(20);}while(key==K_NONE);
  app_key(&app,key);wait_no_key_pressed();
 }
 SDL_Quit();return 0;
}
