#ifndef SEVEN_SEG_RENDER_H
#define SEVEN_SEG_RENDER_H

#include <stdbool.h>
#include "DEV_Config.h"

void sevenseg_draw_digit(int x, int y, int seg, int thick, int d, UWORD fg, UWORD bg);
void sevenseg_draw_colon(int x, int y, int dot, int gap, UWORD fg);
void sevenseg_draw_time(datetime_t t, UWORD *fb, bool vertical, UWORD fg, UWORD bg);

#endif
