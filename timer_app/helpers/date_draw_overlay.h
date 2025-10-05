#ifndef DATE_DRAW_OVERLAY_H
#define DATE_DRAW_OVERLAY_H

#include "DEV_Config.h"          // UWORD
#include "Fonts.h"               // sFONT*
#include "hardware/rtc.h"

void date_overlay_draw(const datetime_t *t, int x, int y,
                       const sFONT *font, UWORD fg, UWORD bg);

#endif
