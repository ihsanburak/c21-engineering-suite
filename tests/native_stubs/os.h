#ifndef C21_TEST_OS_H
#define C21_TEST_OS_H
#include "keys.h"
int isKeyPressed(const t_key *key);
#define isKeyPressed(key) (isKeyPressed)(&(key))
extern int has_colors;
void wait_no_key_pressed(void);
#endif
