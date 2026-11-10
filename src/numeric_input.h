#ifndef C21_NUMERIC_INPUT_H
#define C21_NUMERIC_INPUT_H
#include <stddef.h>
typedef enum { NUM_OK,NUM_EMPTY,NUM_FORMAT,NUM_RANGE,NUM_FULL,NUM_DOMAIN,NUM_ZERO } NumericError;
NumericError numeric_parse(const char *text,double scale,double *si);
int numeric_insert(char *text,size_t capacity,unsigned *cursor,char ch);
void numeric_backspace(char *text,unsigned *cursor);
void numeric_delete(char *text,unsigned cursor);
const char *numeric_error_text(NumericError error);
#endif
