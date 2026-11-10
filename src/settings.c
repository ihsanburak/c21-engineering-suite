#include "settings.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
static char settings_path[512];
static const unsigned char magic[8]={'C','2','1','L','A','N','G','1'};
static uint32_t sequence(const unsigned char *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static int valid(const unsigned char *p){unsigned i;unsigned char sum=0xA7;for(i=0;i<5;i++)sum^=p[i];return p[4]<=LANG_TR_ASCII&&p[5]==sum&&p[6]==0xC2&&p[7]==0x1D;}
static int latest(const unsigned char data[24]){int a=valid(data+8),b=valid(data+16);uint32_t delta;if(!a)return b?1:-1;if(!b)return 0;delta=sequence(data+16)-sequence(data+8);return delta&&delta<0x80000000u?1:0;}
static int read_record(FILE *f,unsigned char data[24]){return fread(data,1,24,f)==24&&fgetc(f)==EOF&&!ferror(f)&&!memcmp(data,magic,8);}
void settings_bind(const char *p){settings_path[0]=0;if(p&&strlen(p)<sizeof(settings_path))strcpy(settings_path,p);}
int settings_bind_executable(const char *exe){const char *a,*b,*last;static const char name[]="c21-engineering-suite.language.cfg.tns";size_t n;settings_bind(0);if(!exe)return 0;a=strrchr(exe,'/');b=strrchr(exe,'\\');last=!a?b:!b?a:(a>b?a:b);if(!last)return 0;n=(size_t)(last-exe)+1;if(n+sizeof(name)>sizeof(settings_path))return 0;memcpy(settings_path,exe,n);memcpy(settings_path+n,name,sizeof(name));return 1;}
Language settings_load(void){unsigned char d[24];FILE *f;int slot;if(!settings_path[0])return LANG_EN;f=fopen(settings_path,"rb");if(!f)return LANG_EN;if(!read_record(f,d)){fclose(f);return LANG_EN;}fclose(f);slot=latest(d);/* Normalize legacy Turkish without rewriting on load. */return slot<0?LANG_EN:d[8+slot*8+4]==LANG_TR?LANG_TR_ASCII:(Language)d[8+slot*8+4];}
int settings_save(Language lang){unsigned char d[24]={0},p[8]={0};uint32_t seq=1;int slot=0,old,i,ok;FILE *f;
 if(!settings_path[0]||(lang!=LANG_EN&&lang!=LANG_TR&&lang!=LANG_TR_ASCII))return 0;
 /* Preserve enum/file values; future writes persist the normalized choice. */
 if(lang==LANG_TR)lang=LANG_TR_ASCII;
 errno=0;f=fopen(settings_path,"r+b");
 if(f){if(!read_record(f,d)){fclose(f);return 0;}old=latest(d);if(old>=0){seq=sequence(d+8+old*8)+1;slot=1-old;}if(fseek(f,8+slot*8,SEEK_SET)){fclose(f);return 0;}}
 else{if(errno!=ENOENT)return 0;f=fopen(settings_path,"wb");if(!f)return 0;memcpy(d,magic,8);if(fwrite(d,1,24,f)!=24||fseek(f,8,SEEK_SET)){fclose(f);return 0;}}
 p[0]=(unsigned char)seq;p[1]=(unsigned char)(seq>>8);p[2]=(unsigned char)(seq>>16);p[3]=(unsigned char)(seq>>24);p[4]=(unsigned char)lang;p[5]=0xA7;for(i=0;i<5;i++)p[5]^=p[i];p[6]=0xC2;p[7]=0x1D;
 ok=fwrite(p,1,8,f)==8;if(fflush(f))ok=0;if(fclose(f))ok=0;return ok;
}
