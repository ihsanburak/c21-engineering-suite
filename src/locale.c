#include "locale.h"
#include <stdint.h>
#include <string.h>
typedef struct { uint32_t hash; const char *value; } Translation;
#include "locale_data.h"
static Language language;
static uint32_t hash_text(const char *s){uint32_t h=2166136261u;while(*s){h^=(unsigned char)*s++;h*=16777619u;}return h;}
static const Translation *lookup(const char *s){unsigned lo=0,hi=sizeof(translations)/sizeof(translations[0]);uint32_t h;if(!s)return 0;h=hash_text(s);while(lo<hi){unsigned mid=lo+(hi-lo)/2;if(translations[mid].hash<h)lo=mid+1;else hi=mid;}return lo<sizeof(translations)/sizeof(translations[0])&&translations[lo].hash==h?&translations[lo]:0;}
void loc_set(Language l){language=(l==LANG_TR||l==LANG_TR_ASCII)?l:LANG_EN;}
Language loc_get(void){return language;}
const char *tr(const char *s){const Translation *p;if(language==LANG_EN||!s)return s;p=lookup(s);return p?p->value:s;}
int loc_has(const char *s){return lookup(s)!=0;}
unsigned loc_count(void){return sizeof(translations)/sizeof(translations[0]);}
const char *loc_value(unsigned i){return i<loc_count()?translations[i].value:"";}
