#include "pico/stdlib.h"
#include "hardware/rtc.h"
#include "hardware/gpio.h"

#include "DEV_Config.h"
#include "LCD_1in14.h"
#include "GUI_Paint.h"
#include "Fonts.h"

#include "helpers/date_time_helper.h"
#include "helpers/seven_seg_render.h"
#include "helpers/text_mode_render.h"g
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* ================= Button config ================= */
#define BTN_PIN 15
#define DEBOUNCE_MS 25

/* ================= Mode toggle ================= */
static bool seg_mode = true;  // true = 7-seg; false = text


/* ================= Button init & edge-detect ================= */
static void button_init(void) {
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN); // active-low button to GND
}

static bool button_falling_edge(void) {
    static uint32_t last_ms = 0;
    static bool last_level = true; // idle high (pull-up)
    bool level = gpio_get(BTN_PIN);
    uint32_t ms = to_ms_since_boot(get_absolute_time());

    if (ms - last_ms < DEBOUNCE_MS) return false;

    bool edge = (last_level == true && level == false);
    if (edge) last_ms = ms;
    last_level = level;
    return edge;
}


int main(void) {
    stdio_init_all();
    rtc_init();

    DEV_Delay_ms(100);
    if (DEV_Module_Init() != 0) return -1;
    DEV_SET_PWM(50);

    LCD_1IN14_Init(HORIZONTAL);
    LCD_1IN14_Clear(BLACK);

    UDOUBLE Imagesize = LCD_1IN14_HEIGHT * LCD_1IN14_WIDTH * 2;
    UWORD *BlackImage = (UWORD*)malloc(Imagesize);
    if (!BlackImage) return -1;

    Paint_NewImage((UBYTE*)BlackImage, LCD_1IN14.WIDTH, LCD_1IN14.HEIGHT, 0, WHITE);
    Paint_SetScale(65);
    Paint_Clear(WHITE);

    button_init();

    datetime_t now;
    rtc_get_datetime(&now);
    sevenseg_draw_time(now,BlackImage);

    int last_min = now.min;

    for (;;) {
        if (button_falling_edge()) {
            rtc_get_datetime(&now);
            sevenseg_draw_time(now,BlackImage);
        }

        datetime_t just_set;
        if (poll_and_set_rtc(&just_set)) {
            sevenseg_draw_time(just_set,BlackImage);
            last_min = just_set.min;
        }

        rtc_get_datetime(&now);
        if (now.min != last_min) {
            sevenseg_draw_time(now,BlackImage);
            last_min = now.min;
        }

        sleep_ms(10);
    }
}
