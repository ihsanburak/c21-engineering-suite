#ifndef C21_RF_ENGINE_H
#define C21_RF_ENGINE_H
#include "engine.h"
int rf_parameter_valid(CalcId id,unsigned index,double value);
Status rf_calculate(CalcId id,const double *si,double *result);
Status rf_reflection(const double *si,double *real,double *imaginary);
Status rf_impedance(const double *si,double *real,double *imaginary);
Status rf_lmatch(const double *si,int highpass,double *series,double *shunt);
int rf_has_details(CalcId id);
int rf_regulatory(CalcId id);
#endif
