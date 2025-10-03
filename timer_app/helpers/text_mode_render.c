#include "text_mode_render.h"    // Module’s own header first

// Standard C
#include <stdio.h>               // snprintf

// Pico SDK
// (none needed here; datetime_t is already pulled via our header)

// Project / vendor
#include "GUI_Paint.h"           // Paint_*, colors
#include "Fonts.h"               // Font20, Font48 decls
#include "LCD_1in14.h"           // LCD_1IN14_Display
#include "date_time_helper.h"    // ensure_dotw, weekday_name


void textmode_draw_time(datetime_t t, UWORD *fb) {
    ensure_dotw(&t);
    Paint_Clear(BLACK);

    char date_line[48];
    char time_line[16];

    snprintf(date_line, sizeof date_line, "%s %02d/%02d/%02d",
             weekday_name(t.dotw), t.day, t.month, t.year % 100);
    snprintf(time_line, sizeof time_line, "%02d:%02d",
             (int)t.hour, (int)t.min);

    Paint_DrawString_EN(0, 12, date_line, &Font20, BLACK, MAGENTA);
    Paint_DrawString_EN(55, 50, time_line, &Font48, BLACK, WHITE);

    LCD_1IN14_Display(fb);
}
