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

/* --------------------------- Draw helpers (DRY) -------------------------- */
static inline void paint_all(const datetime_t* t,
                             UWORD* fb,
                             bool portrait,
                             LcdBgGradient* bg,
                             Star* stars)
{
    lcd_bg_draw(fb, portrait, bg);
    uint32_t t_ms = to_ms_since_boot(get_absolute_time());
    star_overlay_draw(fb, t_ms, stars, STAR_COUNT, STAR_PERIOD_MS, STAR_MIN_B, STAR_MAX_B);
    date_overlay_draw(t, 0, 6, &Font16, OFFWHITE, WHITE);   // portrait only
    sevenseg_draw_time(*t, fb, portrait, OFFWHITE, BLACK);


    LCD_1IN14_Display(fb);
}

int main(void)
{
    stdio_init_all();
    rtc_init();

    DEV_Delay_ms(100);
    if (DEV_Module_Init() != 0) return -1;
    DEV_SET_PWM(50);

    button_init();

    /* Portrait only */
    const bool portrait = true;

    UWORD* fb = setup_display(portrait, NULL);
    if (!fb) return -1;

    /* Background preset */
    LcdBgGradient bg;
    lcd_bg_init_gradient(&bg, MIDNIGHT_CITY, 4);

    /* Stars (seed once) */
    static Star g_stars[STAR_COUNT];
    star_overlay_init(g_stars, STAR_COUNT, 0x12345678u, 1, 2.75); // size range 1..1

    datetime_t now;
    rtc_get_datetime(&now);

    /* Initial full paint */
    paint_all(&now, fb, portrait, &bg, g_stars);

    int last_min = now.min;
    uint32_t last_star_ms = 0;

    for (;;)
    {
        /* RTC set from UART */
        datetime_t just_set;
        if (poll_and_set_rtc(&just_set)) {
            paint_all(&just_set, fb, portrait, &bg, g_stars);
            now = just_set;
            last_min = just_set.min;
        }

        /* Minute tick */
        rtc_get_datetime(&now);
        if (now.min != last_min) {
            paint_all(&now, fb, portrait, &bg, g_stars);
            last_min = now.min;
        }

        /* Twinkle-only refresh (brightness update only) */
        uint32_t t_ms = to_ms_since_boot(get_absolute_time());
        if (t_ms - last_star_ms >= STAR_TICK_MS) {
            last_star_ms = t_ms;
            star_overlay_refresh(fb, g_stars, STAR_COUNT);
        }

        sleep_ms(5);
    }
}
