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
#include "helpers/star_overlay.h"

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

    /* Background: solid black so white stars look clean */
    LcdBgGradient bg;
    lcd_bg_init_gradient(&bg, MIDNIGHT_CITY, 4);

    /* Stars: seed once (tiny, grayscale twinkle) */
    static Star g_stars[200];
    star_overlay_init(g_stars, 200, 0x12345678u, 1, 2);   // size range 1..1

    /* Twinkle controls */
    uint32_t last_star_ms     = 0;
    const uint32_t STAR_TICK_MS   = 80;      // redraw stars ~12.5 FPS (gentle)
    const uint32_t STAR_PERIOD_MS = 12000;   // 12s up+down cycle (slow)
    const uint8_t  STAR_MIN_B     = 5;       // dim floor
    const uint8_t  STAR_MAX_B     = 255;     // soft ceiling (not harsh white)

    datetime_t now;
    rtc_get_datetime(&now);

    /* Initial full paint */
    lcd_bg_draw(fb, portrait, &bg);
    date_overlay_draw(&now, 0, portrait ? 6 : 10, &Font16, OFFWHITE, WHITE);
    sevenseg_draw_time(now, fb, portrait, OFFWHITE, BLACK);
    {
        uint32_t t_ms = to_ms_since_boot(get_absolute_time());
        star_overlay_draw(fb, t_ms, g_stars, 200, STAR_PERIOD_MS, STAR_MIN_B, STAR_MAX_B);
    }
    LCD_1IN14_Display(fb);

    int last_min = now.min;

    for (;;) {
        /* Orientation toggle */
        if (button_falling_edge()) {
            portrait = !portrait;
            fb = setup_display(portrait, fb);
            if (!fb) return -1;

            rtc_get_datetime(&now);

            lcd_bg_draw(fb, portrait, &bg);
            date_overlay_draw(&now, 0, portrait ? 6 : 10, &Font16, OFFWHITE, WHITE);
            sevenseg_draw_time(now, fb, portrait, portrait ? OFFWHITE : OFFWHITE, BLACK);

            uint32_t t_ms = to_ms_since_boot(get_absolute_time());
            star_overlay_draw(fb, t_ms, g_stars, 200, STAR_PERIOD_MS, STAR_MIN_B, STAR_MAX_B);

            LCD_1IN14_Display(fb);
            last_min = now.min;
        }

        /* RTC set from UART */
        datetime_t just_set;
        if (poll_and_set_rtc(&just_set)) {
            lcd_bg_draw(fb, portrait, &bg);
            date_overlay_draw(&just_set, 0, portrait ? 6 : 10, &Font16, OFFWHITE, WHITE);
            sevenseg_draw_time(just_set, fb, portrait, portrait ? OFFWHITE : OFFWHITE, BLACK);

            uint32_t t_ms = to_ms_since_boot(get_absolute_time());
            star_overlay_draw(fb, t_ms, g_stars, 200, STAR_PERIOD_MS, STAR_MIN_B, STAR_MAX_B);

            LCD_1IN14_Display(fb);
            now = just_set;
            last_min = just_set.min;
        }

        /* Minute tick */
        rtc_get_datetime(&now);
        if (now.min != last_min) {
            lcd_bg_draw(fb, portrait, &bg);
            date_overlay_draw(&now, 0, portrait ? 6 : 10, &Font16, OFFWHITE, WHITE);
            sevenseg_draw_time(now, fb, portrait, portrait ? OFFWHITE : OFFWHITE, BLACK);

            uint32_t t_ms = to_ms_since_boot(get_absolute_time());
            star_overlay_draw(fb, t_ms, g_stars, 200, STAR_PERIOD_MS, STAR_MIN_B, STAR_MAX_B);

            LCD_1IN14_Display(fb);
            last_min = now.min;
        }

        /* Twinkle-only refresh (no re-seed, just brightness update) */
        uint32_t t_ms = to_ms_since_boot(get_absolute_time());
        if (t_ms - last_star_ms >= STAR_TICK_MS) {
            last_star_ms = t_ms;
            star_overlay_draw(fb, t_ms, g_stars, 200, STAR_PERIOD_MS, STAR_MIN_B, STAR_MAX_B);
            LCD_1IN14_Display(fb);
        }

        sleep_ms(5);
    }
}
