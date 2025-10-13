// helpers/mountain_overlay.c
#include "mountain_overlay.h"
#include "LCD_1in14.h"
#include <stdint.h>

/* ---------------- RGB565 helpers (integer) ---------------- */

static inline int c5(UWORD c)  { return (c >> 11) & 0x1F; }  // R (5)
static inline int c6(UWORD c)  { return (c >> 5)  & 0x3F; }  // G (6)
static inline int c5b(UWORD c) { return  c        & 0x1F; }  // B (5)

static inline UWORD pack565(int r5, int g6, int b5) {
    return (UWORD)((r5 << 11) | (g6 << 5) | b5);
}

// t256 in [0..256]: 0=top, 256=bottom
static inline UWORD lerp565(UWORD c0, UWORD c1, int t256)
{
    int inv = 256 - t256;
    int r = ((c5(c0)  * inv) + (c5(c1)  * t256)) >> 8;
    int g = ((c6(c0)  * inv) + (c6(c1)  * t256)) >> 8;
    int b = ((c5b(c0) * inv) + (c5b(c1) * t256)) >> 8;
    if (r > 31) r = 31; if (g > 63) g = 63; if (b > 31) b = 31;
    return pack565(r, g, b);
}

/* ---- Filled isosceles triangle with vertical gradient (integer scanlines) ----
   apex     = (cx, base_y - height)
   base L/R = (cx - half_base, base_y) .. (cx + half_base, base_y)
   Only per-triangle divisions: fixed-point slope + gradient step.
------------------------------------------------------------------------------- */
static void draw_iso_triangle_filled_gradient(UWORD *fb,
                                              int cx, int base_y,
                                              int half_base, int height,
                                              UWORD color_top, UWORD color_bottom)
{
    const int W = LCD_1IN14.WIDTH;
    const int H = LCD_1IN14.HEIGHT;
    if (!fb || half_base <= 0 || height <= 0) return;

    int apex_y = base_y - height;
    if (apex_y >= H || base_y < 0) return;
    if (apex_y < 0)  apex_y = 0;
    if (base_y >= H) base_y = H - 1;

    // Fixed-point half-width per scanline: 8.8
    const int dx_fixed = (half_base << 8) / height;
    // Gradient step: 0..256 across height
    const int t_step   = 256 / height;

    for (int y = apex_y; y <= base_y; ++y) {
        int dy = y - (base_y - height);          // 0..height
        int dx = (dx_fixed * dy) >> 8;

        int xL = cx - dx;
        int xR = cx + dx;
        if (xR < 0 || xL >= W) continue;
        if (xL < 0) xL = 0;
        if (xR >= W) xR = W - 1;

        int t256 = dy * t_step; if (t256 > 256) t256 = 256;
        UWORD scanColor = lerp565(color_top, color_bottom, t256);

        UBYTE *row = (UBYTE*)fb + (y * W * 2);
        for (int x = xL; x <= xR; ++x) {
            row[2*x + 0] = (UBYTE)(scanColor >> 8);
            row[2*x + 1] = (UBYTE)(scanColor & 0xFF);
        }
    }
}

/* ---------------- Layout: back=10, front=6 (integer-only placement) ---------------- */

static const int base_y_far  = 245;  // back row baseline (px from top)
static const int base_y_near = 240;  // front row baseline

// Front row centers (6)
static const int cx_near[6] = { 18, 41, 64, 87, 110, 122 };
// Back row centers (10), staggered
static const int cx_far[10] = { 8, 21, 34, 47, 60, 73, 86, 99, 112, 125 };

// Sizes (compact set from your last step)
static const int hb_far[10] = { 6, 8, 6, 8, 6, 7, 6, 7, 6, 7 };     // half-bases
static const int ht_far[10] = { 14,18,16,19,15,18,16,20,17,18 };    // heights

static const int hb_near[6] = { 9, 10, 9, 10, 9, 8 };
static const int ht_near[6] = { 22, 25, 23, 27, 24, 22 };

/* ------------------------------ Public API -------------------------------- */

void mountain_overlay_draw(UWORD *fb,
                           UWORD far_top, UWORD far_bottom,
                           UWORD near_top, UWORD near_bottom)
{
    if (!fb) return;

    // Back row (10)
    for (int i = 0; i < 10; ++i) {
        draw_iso_triangle_filled_gradient(
            fb, cx_far[i], base_y_far, hb_far[i], ht_far[i],
            far_top, far_bottom
        );
    }
    // Front row (6)
    for (int i = 0; i < 6; ++i) {
        draw_iso_triangle_filled_gradient(
            fb, cx_near[i], base_y_near, hb_near[i], ht_near[i],
            near_top, near_bottom
        );
    }
}
