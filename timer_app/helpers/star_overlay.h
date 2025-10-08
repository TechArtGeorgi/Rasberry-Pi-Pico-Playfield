#ifndef STAR_OVERLAY_H
#define STAR_OVERLAY_H

#include <stdint.h>
#include "DEV_Config.h"   // for UWORD

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t x;         // pixel position
    uint16_t y;
    uint16_t phase_ms;  // initial phase offset in ms
    uint8_t  size;      // drawn size in pixels (square, centered)
} Star;

/**
 * Seed a starfield once.
 * - stars: array you provide
 * - count: number of stars
 * - seed:  deterministic seed (any uint32)
 * - size_min/max: inclusive size range (clamped to >=1, size_min<=size_max)
 */
void star_overlay_init(Star *stars, int count,
                       uint32_t seed,
                       int size_min, int size_max);

/**
 * Draw twinkling stars over the existing framebuffer (assumes black bg or you want additive white).
 * - t_ms:        time in ms (monotonic)
 * - period_ms:   full fade in+out period per star (e.g. 4000)
 * - stars/count: star array seeded by star_overlay_init
 * All stars are rendered in white with brightness driven by a triangle wave.
 */
void star_overlay_draw(UWORD *fb, uint32_t t_ms,
                       const Star *stars, int count,
                       uint32_t period_ms);

#ifdef __cplusplus
}
#endif
#endif
