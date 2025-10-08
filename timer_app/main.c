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
#include "helpers/star_overlay.h"   // stars (seed once, twinkle forever)

/* ===== Main ===== */
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

    /* Background gradient (pick any preset) */
    LcdBgGradient bg;
    lcd_bg_init_gradient(&bg, MIDNIGHT_CITY, 3);

    /* RTC read */
    datetime_t now;
    rtc_get_datetime(&now);

    /* --- Stars: seed once (e.g., 200 stars, sizes 1..2 px) --- */
    static Star g_stars[200];
    star_overlay_init(g_stars, 150, 0x12345678u, 0.5, 2);

    /* timing for star-only refresh & twinkle period */
    uint32_t last_star_tick_ms = 0;
    const uint32_t star_tick_interval_ms = 30;   // refresh stars every ~30ms
    const uint32_t star_period_ms        = 12000; // full fade-in+out cycle

    /* Initial full paint */
    lcd_bg_draw(fb, portrait, &bg);
    date_overlay_draw(&now, 0, portrait ? 6 : 10, &Font20, WHITE, WHITE);
    sevenseg_draw_time(now, fb, portrait, OFFWHITE, BLACK);
    {
        uint32_t t_ms = to_ms_since_boot(get_absolute_time());
        star_overlay_draw(fb, t_ms, g_stars, 200, star_period_ms);
    }
    LCD_1IN14_Display(fb);

    int last_min = now.min;

    for (;;) {
        /* --- Orientation toggle --- */
        if (button_falling_edge()) {
            portrait = !portrait;
            fb = setup_display(portrait, fb);
            if (!fb) return -1;

            rtc_get_datetime(&now);

            lcd_bg_draw(fb, portrait, &bg);
            date_overlay_draw(&now, 0, portrait ? 6 : 10, &Font20, WHITE, WHITE);
            sevenseg_draw_time(now, fb, portrait, portrait ? OFFWHITE : WHITE, BLACK);

            uint32_t t_ms = to_ms_since_boot(get_absolute_time());
            star_overlay_draw(fb, t_ms, g_stars, 200, star_period_ms);

            LCD_1IN14_Display(fb);
            last_min = now.min;
        }

        /* --- RTC set from UART (date '+%Y-%m-%d %H:%M:%S' > /dev/ttyS2) --- */
        datetime_t just_set;
        if (poll_and_set_rtc(&just_set)) {
            lcd_bg_draw(fb, portrait, &bg);
            date_overlay_draw(&just_set, 0, portrait ? 6 : 10, &Font20, WHITE, WHITE);
            sevenseg_draw_time(just_set, fb, portrait, portrait ? OFFWHITE : WHITE, BLACK);

            uint32_t t_ms = to_ms_since_boot(get_absolute_time());
            star_overlay_draw(fb, t_ms, g_stars, 200, star_period_ms);

            LCD_1IN14_Display(fb);
            now = just_set;
            last_min = just_set.min;
        }

        /* --- Minute tick --- */
        rtc_get_datetime(&now);
        if (now.min != last_min) {
            lcd_bg_draw(fb, portrait, &bg);
            date_overlay_draw(&now, 0, portrait ? 6 : 10, &Font20, WHITE, WHITE);
            sevenseg_draw_time(now, fb, portrait, portrait ? YELLOW : WHITE, BLACK);

            uint32_t t_ms = to_ms_since_boot(get_absolute_time());
            star_overlay_draw(fb, t_ms, g_stars, 200, star_period_ms);

            LCD_1IN14_Display(fb);
            last_min = now.min;
        }

        /* --- ⭐ Star-only refresh every ~30ms (no reseed, smooth fade) --- */
        uint32_t now_ms = to_ms_since_boot(get_absolute_time());
        if ((now_ms - last_star_tick_ms) >= star_tick_interval_ms) {
            last_star_tick_ms = now_ms;

            // Only redraw stars over the existing frame; then push
            star_overlay_draw(fb, now_ms, g_stars, 200, star_period_ms);
            LCD_1IN14_Display(fb);
        }

        sleep_ms(5);
    }
}
