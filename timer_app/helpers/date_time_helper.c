#include "date_time_helper.h"
#include <stdio.h>
#include <string.h>

bool parse_datetime(const char *s, datetime_t *dt) {
    return sscanf(s, "%hd-%hhd-%hhd %hhd:%hhd:%hhd",
                  &dt->year, &dt->month, &dt->day,
                  &dt->hour, &dt->min, &dt->sec) == 6;
}

void print_time(void) {
    datetime_t now;
    rtc_get_datetime(&now);
    printf("%04d-%02d-%02d %02d:%02d:%02d\r\n",
           now.year, now.month, now.day, now.hour, now.min, now.sec);
}

void set_rtc_from_stdin(void) {
    char line[64];
    if (fgets(line, sizeof line, stdin)) {
        datetime_t dt;
        if (parse_datetime(line, &dt)) { dt.dotw = 0; rtc_set_datetime(&dt); }
    }
}

