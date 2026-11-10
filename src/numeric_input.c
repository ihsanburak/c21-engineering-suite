#include "numeric_input.h"
#include "engine.h"
#include <string.h>
#include <ctype.h>
NumericError numeric_parse(const char *s,double scale,double *out){
 const char *p=s;unsigned digits=0;
 if(!s||!*s)return NUM_EMPTY;
 if(*p=='+'||*p=='-')p++;
 while(isdigit((unsigned char)*p)){p++;digits++;}
 if(*p=='.'){p++;while(isdigit((unsigned char)*p)){p++;digits++;}}
 if(!digits)return NUM_FORMAT;
 if(*p=='e'||*p=='E'){p++;if(*p=='+'||*p=='-')p++;if(!isdigit((unsigned char)*p))return NUM_FORMAT;while(isdigit((unsigned char)*p))p++;}
 if(*p)return NUM_FORMAT;
 return parse_number(s,scale,out)==OK?NUM_OK:NUM_RANGE;
}
int numeric_insert(char *s,size_t cap,unsigned *cursor,char ch){
 size_t n=strlen(s);if(n+1>=cap)return 0;if(*cursor>n)*cursor=(unsigned)n;
 memmove(s+*cursor+1,s+*cursor,n-*cursor+1);s[(*cursor)++]=ch;return 1;
}
void numeric_backspace(char *s,unsigned *cursor){if(*cursor){size_t n=strlen(s);if(*cursor>n)*cursor=(unsigned)n;if(*cursor){memmove(s+*cursor-1,s+*cursor,n-*cursor+1);(*cursor)--;}}}
void numeric_delete(char *s,unsigned cursor){size_t n=strlen(s);if(cursor<n)memmove(s+cursor,s+cursor+1,n-cursor);}
const char *numeric_error_text(NumericError e){
 switch(e){case NUM_EMPTY:return "Enter a value";case NUM_FORMAT:return "Use decimal or scientific notation";case NUM_RANGE:return "Input or unit conversion out of range";case NUM_FULL:return "Input limit: 31 characters";case NUM_DOMAIN:return "Outside valid input domain";case NUM_ZERO:return "Denominator must not be zero";default:return "";}
}
