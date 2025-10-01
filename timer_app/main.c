#include "LCD_1in14.h"
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "helpers/date_time_helper.h"
#include "fonts.h"

int main(void) {
    stdio_init_all();
    rtc_init();

    set_rtc_from_stdin();

    uint32_t last = to_ms_since_boot(get_absolute_time());
    for (;;) {
        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now - last >= 1000) {
            print_time();
            last = now;
        }
    }
}




