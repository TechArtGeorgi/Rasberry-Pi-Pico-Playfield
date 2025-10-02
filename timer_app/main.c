#include "pico/stdlib.h"
#include "hardware/rtc.h"

#include "DEV_Config.h"
#include "LCD_1in14.h"
#include "GUI_Paint.h"
#include "Fonts.h"
#include "lib/Fonts/font48.c"
#include "helpers/date_time_helper.h"

#include <stdio.h>
#include <stdlib.h>

static void lcd_draw_time(datetime_t t, UWORD *fb) {
    ensure_dotw(&t);
    char date_line[48];
    char time_line[32];

    snprintf(date_line, sizeof date_line, "%s %02d/%02d/%01d",
             weekday_name(t.dotw), t.day, t.month, t.year % 100);

    snprintf(time_line, sizeof time_line, "%02d:%02d", t.hour, t.min);

    Paint_Clear(BLACK);
    Paint_DrawString_EN(0, 18, date_line, &Font20, BLACK, MAGENTA);
    Paint_DrawString_EN(45, 50, time_line, &Font48, BLACK, WHITE);
    LCD_1IN14_Display(fb);
}

int main(void) {
    stdio_init_all();
    rtc_init();

    DEV_Delay_ms(100);
    if (DEV_Module_Init() != 0) return -1;
    DEV_SET_PWM(50);

    LCD_1IN14_Init(HORIZONTAL);
    LCD_1IN14_Clear(BLACK);

    UDOUBLE Imagesize = LCD_1IN14_HEIGHT * LCD_1IN14_WIDTH * 2; // same as demo
    UWORD *BlackImage = (UWORD*)malloc(Imagesize);
    if (!BlackImage) return -1;

    Paint_NewImage((UBYTE*)BlackImage, LCD_1IN14.WIDTH, LCD_1IN14.HEIGHT, 0, WHITE);
    Paint_SetScale(65);
    Paint_Clear(WHITE);

    set_rtc_from_stdin();

    uint32_t last = to_ms_since_boot(get_absolute_time());
    for (;;) {
        uint32_t nowms = to_ms_since_boot(get_absolute_time());
        if (nowms - last >= 1000) {
            datetime_t now; 
            rtc_get_datetime(&now);

            // print_time();

            lcd_draw_time(now, BlackImage);
            last = nowms;
        }
    }
}
