#ifndef C21_SETTINGS_H
#define C21_SETTINGS_H
#include "locale.h"
/* Explicit application-owned path. Empty path disables I/O (host default). */
void settings_bind(const char *path);
int settings_bind_executable(const char *executable);
Language settings_load(void);
int settings_save(Language language);
#endif
