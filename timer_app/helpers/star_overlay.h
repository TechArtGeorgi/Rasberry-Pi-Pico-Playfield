#ifndef STAR_OVERLAY_H
#define STAR_OVERLAY_H

#include <stdint.h>
#include "DEV_Config.h"   // UWORD

typedef struct {
    uint16_t ux;     // normalized X in [0..65535]
    uint16_t uy;     // normalized Y in [0..65535]
    uint16_t phase;  // per-star phase in [0..65535]
} Star;

void star_overlay_init(Star *stars, int count, uint32_t seed);

/* period_ms: full 255->0->255 cycle length */
void star_overlay_draw(UWORD *fb, uint32_t t_ms,
                       const Star *stars, int count,
                       uint32_t period_ms);

#endif
