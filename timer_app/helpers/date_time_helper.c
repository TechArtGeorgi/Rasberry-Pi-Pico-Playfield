#include "date_time_helper.h"
#include "hardware/rtc.h"  
#include <stdio.h>
#include <string.h>
#include <stdint.h>


static int calc_dotw(int y, int m, int d) {
    if (m < 3) { m += 12; y -= 1; }
    int K = y % 100;
    int J = y / 100;
    int h = (d + (13 * (m + 1)) / 5 + K + K / 4 + J / 4 + 5 * J) % 7;
    return (h + 6) % 7;
}

static const char *WEEKDAYS[7] = {
    "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"
};

bool parse_datetime(const char *s, datetime_t *dt) {
    if (!s || !dt) return false;
    return sscanf(s, "%hd-%hhd-%hhd %hhd:%hhd:%hhd",
                  &dt->year, &dt->month, &dt->day,
                  &dt->hour, &dt->min, &dt->sec) == 6;
}

void ensure_dotw(datetime_t *dt) {
    if (!dt) return;
    if (dt->dotw < 0 || dt->dotw > 6) {
        dt->dotw = calc_dotw(dt->year, dt->month, dt->day);
    }
}

const char* weekday_name(int dotw) {
    if (dotw < 0 || dotw > 6) return "Unknown";
    return WEEKDAYS[dotw];
}

void print_time(void) {
    datetime_t now;
    rtc_get_datetime(&now);
    ensure_dotw(&now);

    printf("%s %04d-%02d-%02d %02d:%02d:%02d\r\n",
           WEEKDAYS[now.dotw],
           now.year, now.month, now.day,
           now.hour, now.min, now.sec);
}

void set_rtc_from_stdin(void) {
    char line[64];
    if (fgets(line, sizeof line, stdin)) {
        size_t len = strlen(line);
        while (len && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[--len] = '\0';
        }

        datetime_t dt;
        if (parse_datetime(line, &dt)) {
            ensure_dotw(&dt);
            rtc_set_datetime(&dt);
        }
    }
}
