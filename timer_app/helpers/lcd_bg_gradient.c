#include <stdint.h>
#include "LCD_1in14.h"
#include "lcd_bg_gradient.h"

static inline UWORD lerp565(UWORD c0, UWORD c1, uint16_t t, uint16_t den) {
    uint32_t r0 = (c0 >> 11) & 0x1F, r1 = (c1 >> 11) & 0x1F;
    uint32_t g0 = (c0 >>  5) & 0x3F, g1 = (c1 >>  5) & 0x3F;
    uint32_t b0 =  c0        & 0x1F, b1 =  c1        & 0x1F;

    uint32_t r = (r0*(den - t) + r1*t + den/2) / den;
    uint32_t g = (g0*(den - t) + g1*t + den/2) / den;
    uint32_t b = (b0*(den - t) + b1*t + den/2) / den;

    return (UWORD)((r << 11) | (g << 5) | b);
}

void lcd_bg_init_gradient(LcdBgGradient *dst, const UWORD *colors, int ncolors) {
    if (!dst || !colors) return;
    if (ncolors < 1) ncolors = 1;
    if (ncolors > 4) ncolors = 4;
    for (int i = 0; i < ncolors; ++i) dst->stops[i] = colors[i];
    dst->nstops = ncolors;
}

void lcd_bg_draw(UWORD *fb, bool vertical, const LcdBgGradient *g) {
    if (!fb || !g) return;

    const int W = LCD_1IN14.WIDTH;
    const int H = LCD_1IN14.HEIGHT;

    if (g->nstops == 1) {
        UWORD c = g->stops[0];
        UBYTE hi = (UBYTE)(c >> 8), lo = (UBYTE)(c & 0xFF);
        for (int y = 0; y < H; ++y) {
            UBYTE *row = (UBYTE*)fb + (y * W * 2);
            for (int x = 0; x < W; ++x) {
                row[2*x + 0] = hi;
                row[2*x + 1] = lo;
            }
        }
        return;
    }

    const int span   = vertical ? H : W;         // interpolation axis length
    const int segcnt = g->nstops - 1;            // number of segments
    const int den    = span > 1 ? (span - 1) : 1;

    for (int y = 0; y < H; ++y) {
        UBYTE *row = (UBYTE*)fb + (y * W * 2);
        for (int x = 0; x < W; ++x) {
            int p = vertical ? y : x;            // 0..span-1

            int numer = p * segcnt;
            int seg   = numer / den;
            if (seg >= segcnt) seg = segcnt - 1;
            int base  = seg * den;
            int t     = numer - base;            // 0..den'

            UWORD c = lerp565(g->stops[seg], g->stops[seg + 1], (uint16_t)t, (uint16_t)den);
            row[2*x + 0] = (UBYTE)(c >> 8);
            row[2*x + 1] = (UBYTE)(c & 0xFF);
        }
    }
}
