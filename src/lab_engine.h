#ifndef C21_LAB_ENGINE_H
#define C21_LAB_ENGINE_H
#include "engine.h"
int lab_parameter_valid(CalcId id,unsigned i,double value);
Status lab_calculate(CalcId id,const double *si,double *result);
Status lab_microstrip(const double *si,double *effective,double *impedance);
int lab_details(CalcId id);
int lab_list_only(CalcId id);
#endif
