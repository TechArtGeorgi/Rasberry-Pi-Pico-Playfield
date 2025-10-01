#ifndef DATE_TIME_HELPER_H
#define DATE_TIME_HELPER_H

#include <stdbool.h>
#include "hardware/rtc.h" 

bool parse_datetime(const char *s, datetime_t *dt);
void print_time(void);
void set_rtc_from_stdin(void);

#endif
