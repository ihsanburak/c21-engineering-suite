#include "display.h"
#include "locale.h"
#include <string.h>
typedef struct { unsigned code; unsigned char rows[9]; } Glyph;
#include "font_data.h"
unsigned char framebuffer[320*240];
unsigned display_clipped_pixels;
unsigned display_substitutions;
static unsigned decode(const char **s) {
 const unsigned char *p=(const unsigned char *)*s; unsigned c=*p++, n=0;
 if(c>=0xc2&&c<0xe0){c&=31;n=1;}
 else if(c>=0xe0&&c<0xf0){c&=15;n=2;}
 else if(c>=0xf0&&c<0xf5){c&=7;n=3;}
 else if(c>=128){*s=(const char *)p;return '?';}
 while(n--){if((*p&0xc0)!=0x80){*s=(const char *)p;return '?';}c=(c<<6)|(*p++&63);}
 *s=(const char *)p;return c;
}
static const Glyph *find(unsigned c) {
 unsigned i;for(i=0;i<sizeof(glyphs)/sizeof(glyphs[0]);i++)if(glyphs[i].code==c)return &glyphs[i];
 return 0;
}
int glyph_supported(unsigned c){return find(c)!=0;}
void clear_screen(void){memset(framebuffer,0,sizeof(framebuffer));display_clipped_pixels=0;display_substitutions=0;}
void rect(int x,int y,int w,int h,int ink){
 int i,j;for(j=y;j<y+h;j++)for(i=x;i<x+w;i++)if(i>=0&&i<320&&j>=0&&j<240)framebuffer[j*320+i]=(unsigned char)!!ink;else display_clipped_pixels++;
}
/* Rendering only: the source text, parsed values and glyph metrics stay intact.
   Use native Turkish by default; map a missing Turkish glyph automatically. */
unsigned display_codepoint(unsigned c){
 unsigned ascii;
 switch(c){
 case 0xC7:ascii='C';break;case 0x11E:ascii='G';break;case 0x130:ascii='I';break;
 case 0xD6:ascii='O';break;case 0x15E:ascii='S';break;case 0xDC:ascii='U';break;
 case 0xE7:ascii='c';break;case 0x11F:ascii='g';break;case 0x131:ascii='i';break;
 case 0xF6:ascii='o';break;case 0x15F:ascii='s';break;case 0xFC:ascii='u';break;
 default:return c;
 }
 return loc_get()==LANG_TR_ASCII||!glyph_supported(c)?ascii:c;
}
static void draw(int x,int y,unsigned c,int ink){
 const Glyph *g=find(display_codepoint(c));int i,j;if(!g){g=find('?');display_substitutions++;}
 for(j=0;j<9;j++)for(i=0;i<5;i++)if(g->rows[j]&(16>>i))rect(x+i,y+j,1,1,ink);
}
void text(int x,int y,const char *s,int ink){int start=x;while(*s){unsigned c=decode(&s);if(c=='\n'){x=start;y+=12;}else{draw(x,y,c,ink);x+=6;}}}
int text_width(const char *s){int width=0;while(s&&*s){decode(&s);width+=6;}return width?width-1:0;}
void paragraph(int x,int y,const char *s,int lines){int start=x,row=0;while(*s&&row<lines){unsigned c=decode(&s);if(c=='\n'||x>308){x=start;y+=12;row++;if(c=='\n')continue;}if(row<lines){draw(x,y,c,1);x+=6;}}}
void bitmap(int x,int y,int w,int h,const unsigned char *bits){int i,j;for(j=0;j<h;j++)for(i=0;i<w;i++)if(bits[(j*w+i)/8]&(128>>((j*w+i)%8)))rect(x+i,y+j,1,1,1);}

void frame(int x,int y,int w,int h,int ink){rect(x,y,w,1,ink);rect(x,y+h-1,w,1,ink);rect(x,y,1,h,ink);rect(x+w-1,y,1,h,ink);}
void text_fit(int x,int y,int width,const char *s,int ink){
 int n=width/6,i=0;const char *p=s;
 if(text_width(s)<=width){text(x,y,s,ink);return;}
 if(n<3)return;
 while(*p&&i<n-3){draw(x+i*6,y,decode(&p),ink);i++;}text(x+i*6,y,"...",ink);
}
/* UTF-8 word wrapping, also used without painting to measure scroll extent. */
unsigned wrapped_text(int x,int y,int width,unsigned lines,unsigned skip,const char *s,int ink){
 unsigned row=0,col=0,cols=(unsigned)(width/6);const char *p=s;
 if(!cols||!s)return 0;
 while(*p){
  const char *q;unsigned len=0;
  if(*p=='\n'){p++;row++;col=0;continue;}
  if(*p==' '){p++;if(col&&col<cols)col++;continue;}
  q=p;while(*q&&*q!=' '&&*q!='\n'){decode(&q);len++;}
  if(col&&col+len>cols){row++;col=0;}
  while(p<q){unsigned c=decode(&p);if(col==cols){row++;col=0;}
   if(row>=skip&&row-skip<lines)draw(x+(int)col*6,y+(int)(row-skip)*12,c,ink);
   col++;
  }
 }
 return row+1;
}
