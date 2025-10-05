#include "seven_seg_render.h"
#include <stdio.h>
#include "GUI_Paint.h"
#include "LCD_1in14.h"
#include "Fonts.h"
#include "date_time_helper.h"

void sevenseg_fill_rect(int x, int y, int w, int h, UWORD color) {
    if (w <= 0 || h <= 0) return;
    for (int i = 0; i < h; ++i) {
        Paint_DrawLine(x, y + i, x + w - 1, y + i, color,
                       DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    }
}

/* Individual segments (A..G) */
void sevenseg_seg_A(int x,int y,int seg,int thick,UWORD c){ sevenseg_fill_rect(x+thick,        y,                   seg,   thick, c); }
void sevenseg_seg_B(int x,int y,int seg,int thick,UWORD c){ sevenseg_fill_rect(x+thick+seg,    y+thick,             thick, seg,   c); }
void sevenseg_seg_C(int x,int y,int seg,int thick,UWORD c){ sevenseg_fill_rect(x+thick+seg,    y+2*thick+seg,       thick, seg,   c); }
void sevenseg_seg_D(int x,int y,int seg,int thick,UWORD c){ sevenseg_fill_rect(x+thick,        y+2*(thick+seg),     seg,   thick, c); }
void sevenseg_seg_E(int x,int y,int seg,int thick,UWORD c){ sevenseg_fill_rect(x,              y+2*thick+seg,       thick, seg,   c); }
void sevenseg_seg_F(int x,int y,int seg,int thick,UWORD c){ sevenseg_fill_rect(x,              y+thick,             thick, seg,   c); }
void sevenseg_seg_G(int x,int y,int seg,int thick,UWORD c){ sevenseg_fill_rect(x+thick,        y+thick+seg,         seg,   thick, c); }

enum { SEG_A=1<<0, SEG_B=1<<1, SEG_C=1<<2, SEG_D=1<<3, SEG_E=1<<4, SEG_F=1<<5, SEG_G=1<<6 };

static const uint8_t DIGIT_BITS[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

void sevenseg_draw_digit(int x, int y, int seg, int thick, int d, UWORD fg, UWORD bg) {
    (void)bg; // no box fill — digits overlay the existing background/gradient
    uint8_t bits = (d >= 0 && d <= 9) ? DIGIT_BITS[d] : 0;

    if (bits & SEG_A) sevenseg_seg_A(x,y,seg,thick,fg);
    if (bits & SEG_B) sevenseg_seg_B(x,y,seg,thick,fg);
    if (bits & SEG_C) sevenseg_seg_C(x,y,seg,thick,fg);
    if (bits & SEG_D) sevenseg_seg_D(x,y,seg,thick,fg);
    if (bits & SEG_E) sevenseg_seg_E(x,y,seg,thick,fg);
    if (bits & SEG_F) sevenseg_seg_F(x,y,seg,thick,fg);
    if (bits & SEG_G) sevenseg_seg_G(x,y,seg,thick,fg);
}

void sevenseg_draw_colon(int x, int y, int dot, int gap, UWORD fg) {
    sevenseg_fill_rect(x, y,             dot, dot, fg);
    sevenseg_fill_rect(x, y + dot + gap, dot, dot, fg);
}

void sevenseg_draw_time(datetime_t t, UWORD *fb, bool vertical, UWORD fg, UWORD bg) {
    (void)fb;
    ensure_dotw(&t);

    char date_line[48];
    snprintf(date_line, sizeof date_line, "%s %02d/%02d/%02d",
             weekday_name(t.dotw), t.day, t.month, t.year % 100);

    int h1 = (t.hour / 10) % 10;
    int h2 = t.hour % 10;
    int m1 = (t.min / 10) % 10;
    int m2 = t.min % 10;

    if (!vertical) {
        Paint_DrawString_EN(0, 10, date_line, &Font20, bg, MAGENTA);

        const int seg   = 18;
        const int thick = 6;
        const int gap   = 6;
        const int colon_gap = 8;

        const int dw = 2*seg + thick;
        const int x0 = 25;
        const int y0 = 50;

        sevenseg_draw_digit(x0,                 y0, seg, thick, h1, fg, bg);
        sevenseg_draw_digit(x0 + dw + gap,      y0, seg, thick, h2, fg, bg);

        int colon_x = x0 + (dw*2) + gap*2 + 4;
        int colon_y = y0 + thick + (seg/2) - (thick/2);
        sevenseg_draw_colon(colon_x, colon_y, thick, colon_gap, fg);

        int mm_x = colon_x + thick + gap + 6;
        sevenseg_draw_digit(mm_x,               y0, seg, thick, m1, fg, bg);
        sevenseg_draw_digit(mm_x + dw + gap,    y0, seg, thick, m2, fg, bg);
    } else {
        Paint_DrawString_EN(0, 6, date_line, &Font20, bg, MAGENTA);

        const int seg   = 16;
        const int thick = 5;
        const int gap   = 6;
        const int vgap  = 14;
        const int colon_gap = 8;

        const int dw = 2*seg + thick;
        const int dh = 3*seg + 2*thick;

        int total_w = 2*dw + gap;
        int x0 = (LCD_1IN14.WIDTH  - total_w) / 2;
        int y0 = 75;

        sevenseg_draw_digit(x0,               y0, seg, thick, h1, fg, bg);
        sevenseg_draw_digit(x0 + dw + gap,    y0, seg, thick, h2, fg, bg);

        int colon_x = x0 + dw - thick/2;
        int colon_y = y0 + dh + (vgap - (2*thick + colon_gap))/2;
        sevenseg_draw_colon(colon_x, colon_y, thick, colon_gap, fg);

        int y1 = y0 + dh + vgap + (2*thick + colon_gap);
        sevenseg_draw_digit(x0,               y1, seg, thick, m1, fg, bg);
        sevenseg_draw_digit(x0 + dw + gap,    y1, seg, thick, m2, fg, bg);
    }
}
