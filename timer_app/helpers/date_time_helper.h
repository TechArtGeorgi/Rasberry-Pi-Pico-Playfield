#ifndef DATE_TIME_HELPER_H
#define DATE_TIME_HELPER_H

#include <stdbool.h>
#include "pico/util/datetime.h"

/* ================================================================
   date_time_helper.h
   ---------------------------------------------------------------
   Helper functions for parsing and handling date/time strings and
   synchronizing the RP2040 RTC.

   Features:
   - Parse "YYYY-MM-DD HH:MM:SS" into datetime_t
   - Calculate and validate day-of-week (dotw)
   - Map day-of-week number to string name
   - Poll USB CDC input for incoming datetime strings and update RTC

   Intended Usage:
   - Call poll_and_set_rtc() in the main loop to keep RTC synced
     from host commands like:
        date '+%Y-%m-%d %H:%M:%S' > /dev/ttyS2

   ================================================================ */

/**
 * @brief Parse a string of the form "YYYY-MM-DD HH:MM:SS" into a datetime_t.
 * @param s   Input string
 * @param dt  Output datetime_t structure
 * @return true if parsing succeeded
 */
bool parse_datetime(const char *s, datetime_t *dt);

/**
 * @brief Return the weekday name string for a given day-of-week number.
 * @param dotw  Day of week (0=Sunday .. 6=Saturday)
 * @return Pointer to static string (e.g. "Monday"), or "Unknown"
 */
const char* weekday_name(int dotw);

/**
 * @brief Ensure datetime_t.dotw is valid; if not, calculate it.
 * @param dt  Pointer to datetime_t to update
 */
void ensure_dotw(datetime_t *dt);

/**
 * @brief Poll non-blocking USB CDC input for datetime strings.
 *
 * Reads characters until newline, then attempts to parse a full
 * "YYYY-MM-DD HH:MM:SS" timestamp. If valid, sets RTC and optionally
 * returns the datetime.
 *
 * @param set_out  If not NULL, receives the last parsed datetime
 * @return true if at least one datetime string was parsed and applied
 */
bool poll_and_set_rtc(datetime_t *set_out);

#endif