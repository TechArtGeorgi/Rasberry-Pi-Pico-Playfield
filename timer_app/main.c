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

    LcdBgGradient bg;
    lcd_bg_init_gradient(&bg, MIDNIGHT_CITY, 4);

    datetime_t now;
    rtc_get_datetime(&now);

    // --- seed stars once
    enum { STAR_COUNT = 180 };
    static Star g_stars[STAR_COUNT];
    star_overlay_init(g_stars, STAR_COUNT, 0x12345678u);

    // first paint
    lcd_bg_draw(fb, portrait, &bg);
    date_overlay_draw(&now, 0, portrait ? 6 : 10, &Font20, WHITE, WHITE);
    sevenseg_draw_time(now, fb, portrait, WHITE, BLACK);
    // draw stars on top
    uint32_t t_ms = to_ms_since_boot(get_absolute_time());
    star_overlay_draw(fb, t_ms, g_stars, STAR_COUNT, 12000); // 4s cycle
    LCD_1IN14_Display(fb);

    int last_min = now.min;

    // twinkle timing
    uint32_t last_twinkle_ms = t_ms;
    const uint32_t TWINKLE_DT_MS = 30;  // update every ~30ms

    for (;;) {
        // orientation toggle
        if (button_falling_edge()) {
            portrait = !portrait;
            fb = setup_display(portrait, fb);
            if (!fb) return -1;

            rtc_get_datetime(&now);

            lcd_bg_draw(fb, portrait, &bg);
            date_overlay_draw(&now, 0, portrait ? 6 : 10, &Font16, WHITE, WHITE);
            sevenseg_draw_time(now, fb, portrait, portrait ? WHITE : WHITE, BLACK);
            // stars over new orientation (positions adapt automatically)
            t_ms = to_ms_since_boot(get_absolute_time());
            star_overlay_draw(fb, t_ms, g_stars, STAR_COUNT, 12000);
            LCD_1IN14_Display(fb);

            last_min = now.min;
            last_twinkle_ms = t_ms;
        }

        // RTC set via UART
        datetime_t just_set;
        if (poll_and_set_rtc(&just_set)) {
            lcd_bg_draw(fb, portrait, &bg);
            date_overlay_draw(&just_set, 0, portrait ? 6 : 10, &Font16, WHITE, WHITE);
            sevenseg_draw_time(just_set, fb, portrait, portrait ? WHITE : WHITE, BLACK);
            t_ms = to_ms_since_boot(get_absolute_time());
            star_overlay_draw(fb, t_ms, g_stars, STAR_COUNT, 12000);
            LCD_1IN14_Display(fb);
            now = just_set;
            last_min = just_set.min;
            last_twinkle_ms = t_ms;
        }

        // minute tick
        rtc_get_datetime(&now);
        if (now.min != last_min) {
            lcd_bg_draw(fb, portrait, &bg);
            date_overlay_draw(&now, 0, portrait ? 6 : 10, &Font16, WHITE, WHITE);
            sevenseg_draw_time(now, fb, portrait, portrait ? WHITE : WHITE, BLACK);
            t_ms = to_ms_since_boot(get_absolute_time());
            star_overlay_draw(fb, t_ms, g_stars, STAR_COUNT, 12000);
            LCD_1IN14_Display(fb);
            last_min = now.min;
            last_twinkle_ms = t_ms;
        }

        // twinkle-only refresh (no bg/digits redraw)
        t_ms = to_ms_since_boot(get_absolute_time());
        if (t_ms - last_twinkle_ms >= TWINKLE_DT_MS) {
            last_twinkle_ms = t_ms;
            star_overlay_draw(fb, t_ms, g_stars, STAR_COUNT, 12000);
            LCD_1IN14_Display(fb);
        }

        sleep_ms(5);
    }
}
