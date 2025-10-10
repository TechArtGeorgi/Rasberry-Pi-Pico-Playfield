#ifndef STAR_OVERLAY_H
#define STAR_OVERLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "DEV_Config.h"   
#include "LCD_1in14.h"    

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t phase_ms;   
    uint8_t  size;      
} Star;

/* -------- Optional defaults (if you keep them here) -------- */
#define STAR_COUNT        200
#define STAR_TICK_MS      80
#define STAR_PERIOD_MS    8000
#define STAR_MIN_B        5
#define STAR_MAX_B        255

/* Seed once. size_min/max are inclusive. */
void star_overlay_init(Star *stars, int count, uint32_t seed,
                       uint8_t size_min, uint8_t size_max);

/* Draw over current framebuffer without reseeding. */
void star_overlay_draw(UWORD *fb, uint32_t t_ms,
                       const Star *stars, int count,
                       uint32_t period_ms,
                       uint8_t minB, uint8_t maxB);

/* Twinkle-only refresh using current time. */
void star_overlay_refresh(UWORD *fb, const Star *stars, int count);

#endif
