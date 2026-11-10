#ifndef C21_LOCALE_H
#define C21_LOCALE_H
#include <stddef.h>
typedef enum { LANG_EN, LANG_TR, LANG_TR_ASCII } Language;
void loc_set(Language language);
Language loc_get(void);
const char *tr(const char *english);
int loc_has(const char *english);
unsigned loc_count(void);
const char *loc_value(unsigned index);
#endif
