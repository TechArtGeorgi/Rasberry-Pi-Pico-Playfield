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

static void lcd_draw_time(const datetime_t* t, UWORD *fb) {
    char d[32], h[32];
    snprintf(d, sizeof d, "%02d/%02d/%04d", t->day, t->month, t->year);
    snprintf(h, sizeof h, "%02d:%02d", t->hour, t->min);

    Paint_Clear(BLACK);
    Paint_DrawString_EN(45, 20, d, &Font20, BLACK, MAGENTA);
    Paint_DrawString_EN(45, 40, h, &Font48, BLACK, WHITE);
    LCD_1IN14_Display(fb);
}

int main(void){
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
    // Paint_SetRotate(ROTATE_0);
    Paint_Clear(WHITE);

    set_rtc_from_stdin();

    uint32_t last = to_ms_since_boot(get_absolute_time());
    for(;;){
        uint32_t nowms = to_ms_since_boot(get_absolute_time());
        if (nowms - last >= 1000) {
            datetime_t now; rtc_get_datetime(&now);

            // USB print (For Debugging, will remove)
            // printf("%04d-%02d-%02d %02d:%02d:%02d\r\n",
                //    now.year, now.month, now.day, now.hour, now.min, now.sec);

    
                // LCD draw
            lcd_draw_time(&now, BlackImage);
            last = nowms;
        }
    }
}
