#ifndef C21_UI_H
#define C21_UI_H
#define APP_MENU_COUNT 73
#include "engine.h"
#include "locale.h"
#include <stdint.h>
#include "numeric_input.h"
typedef enum { K_NONE, K_UP, K_DOWN, K_LEFT, K_RIGHT, K_ENTER, K_ESC,
 K_DELETE, K_DOT, K_SIGN, K_EXP, K_INFO,K_CLEAR,K_HOME,K_END,K_FORWARD_DELETE,K_PLUS,
 K_MODE,K_CALCULATE,K_UNITS,K_TARGET,K_TAB,K_OUTPUT_UNITS,K_SETTINGS, K_DIGIT0=32 } Key;
typedef enum { S_SPLASH,S_MENU,S_FORM,S_EDIT,S_UNITS,S_RESULT,S_INFO,S_SYMBOLS,S_FORMULA,S_TARGET,S_SETTINGS,S_LANGUAGE } Screen;
typedef enum { INPUT_LIST,INPUT_FORMULA } InputMode;
typedef struct {
 Screen screen; int running,menu,selected,depth,parents[8],selections[8];
 CalcId calc; unsigned row,units[CALC_MAX_INPUTS],output_unit,unit_cursor,info_page;
 unsigned info_scroll;
 int handbook_origin;
 Screen info_return;
 char values[CALC_MAX_INPUTS][32],edit[32]; double result; Status error;
 unsigned edit_cursor,error_field;
 NumericError input_error;
 int result_valid;
 InputMode input_mode;
 Screen edit_return,units_return,target_return;
 unsigned target_cursor;unsigned char stored_valid[VAR_COUNT];
 unsigned units_focus;
 char stored_values[VAR_COUNT][32];unsigned stored_units[VAR_COUNT];
 int mode_fallback;
 Screen settings_return;unsigned language_cursor;int language_saved;
} App;
void app_init(App *app);
void app_key(App *app,int key);
void app_draw(const App *app);
const unsigned char *app_splash_bitmap(void);
unsigned app_info_lines(const App *app);
int app_request_formula(App *app,int width,int height);
#endif
