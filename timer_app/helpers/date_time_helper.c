#include "date_time_helper.h"
#include "hardware/rtc.h"
#include "tusb.h"
#include <stdint.h>
#include <stddef.h>

static int calc_dotw(int y, int m, int d) {
    if (m < 3) { m += 12; y -= 1; }
    int K = y % 100, J = y / 100;
    int h = (d + (13 * (m + 1)) / 5 + K + K / 4 + J / 4 + 5 * J) % 7;
    return (h + 6) % 7;
}

static const char *WEEKDAYS[7] = {
    "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"
};

const char* weekday_name(int dotw) {
    return (dotw >= 0 && dotw <= 6) ? WEEKDAYS[dotw] : "Unknown";
}

void ensure_dotw(datetime_t *dt) {
    if (dt && (dt->dotw < 0 || dt->dotw > 6)) {
        dt->dotw = calc_dotw(dt->year, dt->month, dt->day);
    }
}

bool parse_datetime(const char *s, datetime_t *dt) {
    if (!s || !dt) return false;
    dt->year  = (int16_t)((s[0]-'0')*1000 + (s[1]-'0')*100 +
                          (s[2]-'0')*10   + (s[3]-'0'));
    dt->month = (int8_t)((s[5]-'0')*10 + (s[6]-'0'));
    dt->day   = (int8_t)((s[8]-'0')*10 + (s[9]-'0'));
    dt->hour  = (int8_t)((s[11]-'0')*10 + (s[12]-'0'));
    dt->min   = (int8_t)((s[14]-'0')*10 + (s[15]-'0'));
    dt->sec   = (int8_t)((s[17]-'0')*10 + (s[18]-'0'));
    dt->dotw  = -1;
    return true;
}

bool poll_and_set_rtc(datetime_t *set_out) {
    static char   buf[19];
    static size_t pos = 0;
    bool updated = false;

    tud_task();
    while (tud_ready() && tud_cdc_available()) {
        int ch = tud_cdc_read_char();
        if (ch == '\r') continue;
        if (ch == '\n') {
            if (pos == 19) {
                datetime_t dt;
                if (parse_datetime(buf, &dt)) {
                    ensure_dotw(&dt);
                    rtc_set_datetime(&dt);
                    if (set_out) *set_out = dt;
                    updated = true;
                }
            }
            pos = 0;
            continue;
        }
        if (pos < 19) buf[pos++] = (char)ch;
    }
    return updated;
}
