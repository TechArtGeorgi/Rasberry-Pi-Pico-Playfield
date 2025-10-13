#ifndef MOUNTAIN_OVERLAY_H
#define MOUNTAIN_OVERLAY_H
#include <stdint.h>
#include "DEV_Config.h"  // UWORD

// Draw mountains with explicit gradient colors (RGB565):
//   far_top, far_bottom   = back row gradient (top→bottom)
//   near_top, near_bottom = front row gradient (top→bottom)
void mountain_overlay_draw(UWORD *fb,
                           UWORD far_top, UWORD far_bottom,
                           UWORD near_top, UWORD near_bottom);

#endif
