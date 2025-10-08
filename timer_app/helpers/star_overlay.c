#include "star_overlay.h"
#include "LCD_1in14.h"

static inline uint32_t lcg(uint32_t *state) {
    *state = (*state * 1664525u) + 1013904223u;
    return *state;
}

void star_overlay_init(Star *stars, int count,
                       uint32_t seed,
                       int size_min, int size_max)
{
    if (!stars || count <= 0) return;

    if (size_min < 1) size_min = 1;
    if (size_max < size_min) size_max = size_min;

    const int W = LCD_1IN14.WIDTH;
    const int H = LCD_1IN14.HEIGHT;

    uint32_t rng = seed ? seed : 0xA5A5F00Du;

    for (int i = 0; i < count; ++i) {
        uint32_t r1 = lcg(&rng);
        uint32_t r2 = lcg(&rng);
        uint32_t r3 = lcg(&rng);

        stars[i].x = (uint16_t)(r1 % W);
        stars[i].y = (uint16_t)(r2 % H);
        stars[i].phase_ms = (uint16_t)(r3 % 4000u); // default spread; period is passed at draw time
        uint32_t span = (uint32_t)(size_max - size_min + 1);
        stars[i].size = (uint8_t)(size_min + (lcg(&rng) % span));
    }
}

void star_overlay_draw(UWORD *fb, uint32_t t_ms,
                       const Star *stars, int count,
                       uint32_t period_ms)
{
    if (!fb || !stars || count <= 0 || period_ms < 2) return;

    const int W = LCD_1IN14.WIDTH;
    const int H = LCD_1IN14.HEIGHT;

    // helper to write one pixel
    #define PX(py, px, color) do { \
        UBYTE *row__ = (UBYTE*)fb + ((py) * W * 2); \
        row__[2 * (px) + 0] = (UBYTE)((color) >> 8); \
        row__[2 * (px) + 1] = (UBYTE)((color) & 0xFF); \
    } while (0)

    for (int i = 0; i < count; ++i) {
        int x = stars[i].x;
        int y = stars[i].y;
        int s = stars[i].size;

        // Triangle-wave brightness 0..255..0 over period_ms, with per-star phase
        uint32_t ph = (t_ms + stars[i].phase_ms) % period_ms;
        uint32_t half = period_ms >> 1;
        uint32_t a = (ph < half)
                   ? (ph * 255u) / (half ? half : 1u)
                   : ((period_ms - ph) * 255u) / (half ? half : 1u);

        // White at brightness 'a' on black background => packed RGB565
        // r,b use 5 bits (>>3), g uses 6 bits (>>2)
        UWORD c = (UWORD)(((a >> 3) << 11) | ((a >> 2) << 5) | (a >> 3));

        // Draw a small centered square (size s)
        int r = s / 2;               // radius on each side
        int x0 = x - r;
        int y0 = y - r;
        int x1 = x0 + s - 1;
        int y1 = y0 + s - 1;

        if (x0 > W - 1 || y0 > H - 1 || x1 < 0 || y1 < 0) continue; // fully offscreen

        if (x0 < 0) x0 = 0;
        if (y0 < 0) y0 = 0;
        if (x1 >= W) x1 = W - 1;
        if (y1 >= H) y1 = H - 1;

        for (int py = y0; py <= y1; ++py) {
            for (int px = x0; px <= x1; ++px) {
                PX(py, px, c);
            }
        }
    }

    #undef PX
}
