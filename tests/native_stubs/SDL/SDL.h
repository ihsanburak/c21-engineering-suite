#ifndef C21_TEST_SDL_H
#define C21_TEST_SDL_H
typedef unsigned int Uint32;
typedef short Sint16;
typedef unsigned short Uint16;
typedef struct { int unused; } SDL_PixelFormat;
typedef struct { SDL_PixelFormat *format; } SDL_Surface;
typedef struct { Sint16 x,y; Uint16 w,h; } SDL_Rect;
#define SDL_INIT_VIDEO 1
#define SDL_SWSURFACE 0
int SDL_Init(unsigned flags);
SDL_Surface *SDL_SetVideoMode(int w,int h,int bpp,unsigned flags);
Uint32 SDL_MapRGB(SDL_PixelFormat *format,unsigned char r,unsigned char g,unsigned char b);
int SDL_FillRect(SDL_Surface *surface,SDL_Rect *rect,Uint32 color);
int SDL_Flip(SDL_Surface *surface);
void SDL_Delay(unsigned ms);
void SDL_Quit(void);
#endif
