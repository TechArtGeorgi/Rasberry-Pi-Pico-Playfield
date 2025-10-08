#ifndef STAR_OVERLAY_H
#define STAR_OVERLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "DEV_Config.h"   // UWORD
#include "LCD_1in14.h"    // for WIDTH/HEIGHT

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t phase_ms;   // per-star phase offset (desync)
    uint8_t  size;       // 1..N (keep small to avoid “big” stars)
} Star;

/* Seed once. size_min/max are inclusive. */
void star_overlay_init(Star *stars, int count, uint32_t seed,
                       uint8_t size_min, uint8_t size_max);

/* Draw over current framebuffer without reseeding.
   - t_ms:       ms since boot
   - period_ms:  full up+down fade (bigger = slower)
   - minB,maxB:  brightness range (0..255), e.g. 16..180
*/
void star_overlay_draw(UWORD *fb, uint32_t t_ms,
                       const Star *stars, int count,
                       uint32_t period_ms,
                       uint8_t minB, uint8_t maxB);

#endif
