#include "display_fb.h"
#include <stdlib.h>
#include "LCD_1in14.h"
#include "GUI_Paint.h"

UWORD *setup_display(bool portrait, UWORD *fb_current) {
    LCD_1IN14_Init(portrait ? VERTICAL : HORIZONTAL);
    LCD_1IN14_Clear(BLACK);

    UDOUBLE size_bytes = (UDOUBLE)LCD_1IN14.WIDTH * (UDOUBLE)LCD_1IN14.HEIGHT * 2;
    fb_current = fb_current ? (UWORD*)realloc(fb_current, size_bytes)
                            : (UWORD*)malloc(size_bytes);
    if (!fb_current) return NULL;

    Paint_NewImage((UBYTE*)fb_current, LCD_1IN14.WIDTH, LCD_1IN14.HEIGHT, 0, BLACK);
    Paint_SetScale(65);
    Paint_Clear(BLACK);
    return fb_current;
}
