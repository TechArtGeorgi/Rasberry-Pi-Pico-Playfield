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

#define BTN_PIN      15
#define DEBOUNCE_MS  25

static void button_init(void) {
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);
}

static bool button_falling_edge(void) {
    static uint32_t last_ms = 0;
    static bool last_level = true;
    bool level = gpio_get(BTN_PIN);
    uint32_t ms = to_ms_since_boot(get_absolute_time());
    if (ms - last_ms < DEBOUNCE_MS) return false;
    bool edge = (last_level == true && level == false);
    if (edge) last_ms = ms;
    last_level = level;
    return edge;
}

static UWORD *setup_display(bool portrait, UWORD *fb_current) {
    if (portrait) LCD_1IN14_Init(VERTICAL);
    else          LCD_1IN14_Init(HORIZONTAL);
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

    datetime_t now;
    rtc_get_datetime(&now);
    sevenseg_draw_time(now, fb, portrait, WHITE, BLACK);

    int last_min = now.min;

    for (;;) {
        if (button_falling_edge()) {
            portrait = !portrait;
            fb = setup_display(portrait, fb);
            if (!fb) return -1;
            rtc_get_datetime(&now);
            sevenseg_draw_time(now, fb, portrait, portrait ? YELLOW : WHITE, BLACK);
            last_min = now.min;
        }

        datetime_t just_set;
        if (poll_and_set_rtc(&just_set)) {
            sevenseg_draw_time(just_set, fb, portrait, portrait ? YELLOW : WHITE, BLACK);
            last_min = just_set.min;
        }

        rtc_get_datetime(&now);
        if (now.min != last_min) {
            sevenseg_draw_time(now, fb, portrait, portrait ? YELLOW : WHITE, BLACK);
            last_min = now.min;
        }

        sleep_ms(10);
    }
}
