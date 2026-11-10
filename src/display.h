#ifndef C21_DISPLAY_H
#define C21_DISPLAY_H
#include <stdint.h>
extern unsigned char framebuffer[320*240];
extern unsigned display_clipped_pixels;
extern unsigned display_substitutions;
unsigned display_codepoint(unsigned codepoint);
void clear_screen(void);
void frame(int x,int y,int w,int h,int ink);
void text_fit(int x,int y,int width,const char *s,int ink);
unsigned wrapped_text(int x,int y,int width,unsigned lines,unsigned skip,const char *s,int ink);
void rect(int x,int y,int w,int h,int ink);
void text(int x,int y,const char *utf8,int ink);
int text_width(const char *utf8);
void paragraph(int x,int y,const char *utf8,int max_lines);
void bitmap(int x,int y,int w,int h,const unsigned char *bits);
int glyph_supported(unsigned codepoint);
#endif
