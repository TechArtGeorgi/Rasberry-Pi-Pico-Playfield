#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "hardware/rtc.h"
#include "hardware/gpio.h"

#include "DEV_Config.h"
#include "LCD_1in14.h"
#include "GUI_Paint.h"

#include "helpers/date_time_helper.h"
#include "helpers/seven_seg_render.h"
#include "helpers/lcd_bg_gradient.h"
#include "helpers/lcd_bg_presets.h"
#include "helpers/button_logic.h"
#include "helpers/display_fb.h"
#include "helpers/date_draw_overlay.h"

int main(void) {

    stdio_init_all();
    rtc_init();

    DEV_Delay_ms(100);
    if (DEV_Module_Init() != 0) return -1;
    DEV_SET_PWM(50);

    button_init();

    bool portrait = false;
    UWORD *fb = setup_display(portrait, NULL);
    if (!fb) return -1;
    
    LcdBgGradient bg; 
    lcd_bg_init_gradient(&bg, GRADIENT_SUNSET, 3);

    datetime_t now;
    rtc_get_datetime(&now);

    lcd_bg_draw(fb, portrait, &bg);
    date_overlay_draw(&now, 0, portrait ? 6 : 10, &Font20, WHITE, WHITE);
    sevenseg_draw_time(now, fb, portrait, WHITE, BLACK);
    LCD_1IN14_Display(fb);

    int last_min = now.min;

    for (;;) {
        if (button_falling_edge()) {
            portrait = !portrait;
            fb = setup_display(portrait, fb);
            if (!fb) return -1;

            rtc_get_datetime(&now);

            lcd_bg_draw(fb, portrait, &bg);
            date_overlay_draw(&now, 0, portrait ? 6 : 10, &Font20, WHITE, WHITE);
            sevenseg_draw_time(now, fb, portrait, portrait ? YELLOW : WHITE, BLACK);
            LCD_1IN14_Display(fb);
            last_min = now.min;
        }

        datetime_t just_set;
        if (poll_and_set_rtc(&just_set)) {
            lcd_bg_draw(fb, portrait, &bg);
            date_overlay_draw(&just_set, 0, portrait ? 6 : 10, &Font20, WHITE, WHITE);
            sevenseg_draw_time(just_set, fb, portrait, portrait ? YELLOW : WHITE, BLACK);
            LCD_1IN14_Display(fb);
            now = just_set;
            last_min = just_set.min;
        }

        rtc_get_datetime(&now);
        if (now.min != last_min) {
            lcd_bg_draw(fb, portrait, &bg);
            date_overlay_draw(&now, 0, portrait ? 6 : 10, &Font20, WHITE, WHITE);
            sevenseg_draw_time(now, fb, portrait, portrait ? YELLOW : WHITE, BLACK);
            LCD_1IN14_Display(fb);
            last_min = now.min;
        }
    }
}
