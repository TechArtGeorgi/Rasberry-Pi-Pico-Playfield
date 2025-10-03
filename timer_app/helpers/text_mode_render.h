#ifndef TEXT_MODE_RENDER_H
#define TEXT_MODE_RENDER_H

#include "pico/util/datetime.h"
#include "DEV_Config.h"

void textmode_draw_time(datetime_t t, UWORD *fb);

#endif
