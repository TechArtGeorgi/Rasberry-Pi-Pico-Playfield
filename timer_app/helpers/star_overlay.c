#include "star_overlay.h"

/* simple LCG for stable pseudo-random in init only */
static inline uint32_t lcg(uint32_t *s) {
    *s = (*s * 1664525u) + 1013904223u;
    return *s;
}

/* strict grayscale: map 0..255 -> RGB565 gray (no tint) */
static inline UWORD gray565(uint8_t g) {
    uint16_t r5 = g >> 3;       // 5 bits
    uint16_t g6 = g >> 2;       // 6 bits
    uint16_t b5 = g >> 3;       // 5 bits
    return (UWORD)((r5 << 11) | (g6 << 5) | b5);
}

/* Clamp helper */
static inline uint8_t u8clamp(int v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return (uint8_t)v;
}

void star_overlay_init(Star *stars, int count, uint32_t seed,
                       uint8_t size_min, uint8_t size_max) {
    if (!stars || count <= 0) return;
    if (size_min == 0) size_min = 1;
    if (size_max < size_min) size_max = size_min;

    uint32_t s = seed;
    const int W = LCD_1IN14.WIDTH;
    const int H = LCD_1IN14.HEIGHT;

    for (int i = 0; i < count; ++i) {
        uint32_t r = lcg(&s);
        stars[i].x = (uint16_t)(r % W);

        r = lcg(&s);
        stars[i].y = (uint16_t)(r % H);

        // random phase 0..period-1 (we’ll mod by period at draw time)
        r = lcg(&s);
        stars[i].phase_ms = (uint16_t)(r & 0xFFFFu);

        // tiny sizes to avoid “big” stars look
        r = lcg(&s);
        uint8_t span = (uint8_t)(size_max - size_min + 1);
        stars[i].size = (uint8_t)(size_min + (span ? (r % span) : 0));
    }
}

void star_overlay_draw(UWORD *fb, uint32_t t_ms,
                       const Star *stars, int count,
                       uint32_t period_ms,
                       uint8_t minB, uint8_t maxB) {
    if (!fb || !stars || count <= 0 || period_ms == 0) return;

    if (minB > maxB) { uint8_t tmp = minB; minB = maxB; maxB = tmp; }
    const int W = LCD_1IN14.WIDTH;
    const int H = LCD_1IN14.HEIGHT;

    // For each star: compute brightness with a triangle wave
    // phase = (t_ms + phase_ms) % period; wave 0..1..0
    for (int i = 0; i < count; ++i) {
        uint32_t phase = (t_ms + (uint32_t)stars[i].phase_ms) % period_ms;
        // triangle in [0..1]: up on first half, down on second
        float u = (float)phase / (float)period_ms;
        float tri = (u < 0.5f) ? (u * 2.0f) : (2.0f - u * 2.0f);

        int br = (int)(minB + tri * (float)(maxB - minB) + 0.5f);
        uint8_t g = u8clamp(br);
        UWORD c = gray565(g);

        // Plot as a tiny square (size ~1 or 2), clipped
        int sz = stars[i].size;
        int x0 = stars[i].x;
        int y0 = stars[i].y;

        int x1 = x0 + sz - 1;
        int y1 = y0 + sz - 1;
        if (x0 < 0) x0 = 0;
        if (y0 < 0) y0 = 0;
        if (x1 >= W) x1 = W - 1;
        if (y1 >= H) y1 = H - 1;

        for (int y = y0; y <= y1; ++y) {
            UBYTE *row = (UBYTE*)fb + (y * W * 2);
            for (int x = x0; x <= x1; ++x) {
                row[2*x + 0] = (UBYTE)(c >> 8);
                row[2*x + 1] = (UBYTE)(c & 0xFF);
            }
        }
    }
}
