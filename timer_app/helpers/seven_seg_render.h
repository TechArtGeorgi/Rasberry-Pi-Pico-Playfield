#ifndef SEVEN_SEG_RENDER_H
#define SEVEN_SEG_RENDER_H

#include <stdbool.h>
#include <stdint.h>
#include "DEV_Config.h"
#include "pico/util/datetime.h"

/* Low-level digit / colon renderers */
void sevenseg_draw_digit(int x, int y, int seg, int thick, int d, UWORD fg, UWORD bg);
void sevenseg_draw_colon(int x, int y, int dot, int gap, UWORD fg);

/* High-level: draw full time (HH:MM) in 7-seg style */
void sevenseg_draw_time(datetime_t t, UWORD *fb);

#endif /* SEVEN_SEG_RENDER_H */
