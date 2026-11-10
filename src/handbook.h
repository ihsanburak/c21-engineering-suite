#ifndef C21_HANDBOOK_H
#define C21_HANDBOOK_H
#include "engine.h"
typedef struct {
 const char *purpose, *measurements, *laboratory, *limits, *example, *mistakes;
 double example_inputs[CALC_MAX_INPUTS], example_result;
} HandbookEntry;
extern const HandbookEntry handbook[CALC_COUNT];
#define HANDBOOK_PAGES 8
#endif
