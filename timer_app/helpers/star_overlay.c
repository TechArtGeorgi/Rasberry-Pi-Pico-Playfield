#include "star_overlay.h"
#include "LCD_1in14.h"

static inline uint32_t rng_next(uint32_t *s) { *s = *s * 1664525u + 1013904223u; return *s; }

void star_overlay_init(Star *stars, int count, uint32_t seed) {
    if (!stars || count <= 0) return;
    uint32_t s = seed ? seed : 0x9E3779B9u;
    for (int i = 0; i < count; ++i) {
        uint32_t r1 = rng_next(&s);
        uint32_t r2 = rng_next(&s);
        uint32_t r3 = rng_next(&s);
        stars[i].ux    = (uint16_t)(r1 >> 16);   // 0..65535
        stars[i].uy    = (uint16_t)(r2 >> 16);   // 0..65535
        stars[i].phase = (uint16_t)(r3 >> 16);   // 0..65535
    }
}

static inline UWORD white_at_alpha(uint8_t a) {
    uint32_t r5 = (a * 31 + 127) / 255;
    uint32_t g6 = (a * 63 + 127) / 255;
    uint32_t b5 = (a * 31 + 127) / 255;
    return (UWORD)((r5 << 11) | (g6 << 5) | b5);
}

void star_overlay_draw(UWORD *fb, uint32_t t_ms,
                       const Star *stars, int count,
                       uint32_t period_ms)
{
    if (!fb || !stars || count <= 0) return;
    if (period_ms < 2) period_ms = 2;

    const int W = LCD_1IN14.WIDTH;
    const int H = LCD_1IN14.HEIGHT;
    const uint32_t half = period_ms >> 1;

    for (int i = 0; i < count; ++i) {
        // Map normalized coords to current framebuffer size
        int x = (int)(((uint32_t)stars[i].ux * (uint32_t)(W - 1)) >> 16);
        int y = (int)(((uint32_t)stars[i].uy * (uint32_t)(H - 1)) >> 16);
        if ((unsigned)x >= (unsigned)W || (unsigned)y >= (unsigned)H) continue;

        // Per-star phase as time offset in ms
        uint32_t phase_ms = ((uint32_t)stars[i].phase * period_ms) >> 16;
        uint32_t u = (t_ms + phase_ms) % period_ms;

        // Triangle wave 255->0->255 (invert so we start at 255)
        uint32_t ramp = (u <= half) ? u : (period_ms - u); // 0..half
        uint32_t a    = 255u - (ramp * 255u / half);       // 255..0..255
        UWORD c = white_at_alpha((uint8_t)a);

        // Write pixel
        UBYTE *row = (UBYTE*)fb + (y * W * 2);
        row[2 * x + 0] = (UBYTE)(c >> 8);
        row[2 * x + 1] = (UBYTE)(c & 0xFF);
    }
}
