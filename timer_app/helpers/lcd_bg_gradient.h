#ifndef LCD_BG_GRADIENT_H
#define LCD_BG_GRADIENT_H

#include <stdbool.h>
#include "DEV_Config.h"  

typedef struct {
    UWORD stops[4];
    int   nstops;
} LcdBgGradient;

void lcd_bg_init_gradient(LcdBgGradient *dst, const UWORD *colors, int ncolors);
void lcd_bg_draw(UWORD *fb, bool vertical, const LcdBgGradient *g);

#endif
