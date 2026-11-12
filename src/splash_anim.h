#ifndef C21_SPLASH_ANIM_H
#define C21_SPLASH_ANIM_H
#include "ui.h"
#define SPLASH_ANIM_FRAMES 56u
#define SPLASH_ANIM_FRAME_MS 50u
/* Pure 320x240 rendering: no allocation, delay, input, state reset or asset edit. */
void splash_anim_draw(const App *app,unsigned frame);
#endif
