#ifndef C21_LORA_ENGINE_H
#define C21_LORA_ENGINE_H
#include "engine.h"
int lora_parameter_valid(CalcId id,unsigned i,double value);
Status lora_calculate(CalcId id,const double *si,double *result);
Status lora_payload_symbols(const double *six,unsigned *symbols);
int lora_list_only(CalcId id);
#endif
