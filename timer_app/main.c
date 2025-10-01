#include "LCD_1in14.h"
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "ImageData.h"
#include "Debug.h"
#include "Infrared.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "hardware/rtc.h"
#include "helpers/date_time_helper.h"

int main(void) {
    stdio_init_all();
    sleep_ms(2000);   // USB CDC up
    rtc_init();

    // 1) Block until Bash sends "YYYY-MM-DD HH:MM:SS"
    set_rtc_from_stdin();

    // 2) Print RTC every second
    uint32_t last = to_ms_since_boot(get_absolute_time());
    for (;;) {
        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now - last >= 1000) {
            print_time();
            last = now;
        }
    }
}




