#include "seven_seg_render.h"

#include "GUI_Paint.h"
#include "LCD_1in14.h"
#include "Fonts.h"

#include "date_time_helper.h"

static inline void fill_rect(int x, int y, int w, int h, UWORD color) {
    if (w <= 0 || h <= 0) return;
    for (int i = 0; i < h; ++i) {
        Paint_DrawLine(x, y + i, x + w - 1, y + i, color,
                       DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    }
}

static void seg_A(int x,int y,int seg,int thick,UWORD c){ fill_rect(x+thick,        y,                   seg,   thick, c); }
static void seg_B(int x,int y,int seg,int thick,UWORD c){ fill_rect(x+thick+seg,    y+thick,             thick, seg,   c); }
static void seg_C(int x,int y,int seg,int thick,UWORD c){ fill_rect(x+thick+seg,    y+2*thick+seg,       thick, seg,   c); }
static void seg_D(int x,int y,int seg,int thick,UWORD c){ fill_rect(x+thick,        y+2*(thick+seg),     seg,   thick, c); }
static void seg_E(int x,int y,int seg,int thick,UWORD c){ fill_rect(x,              y+2*thick+seg,       thick, seg,   c); }
static void seg_F(int x,int y,int seg,int thick,UWORD c){ fill_rect(x,              y+thick,             thick, seg,   c); }
static void seg_G(int x,int y,int seg,int thick,UWORD c){ fill_rect(x+thick,        y+thick+seg,         seg,   thick, c); }

enum { SEG_A=1<<0, SEG_B=1<<1, SEG_C=1<<2, SEG_D=1<<3, SEG_E=1<<4, SEG_F=1<<5, SEG_G=1<<6 };

static const uint8_t DIGIT_BITS[10] = {
    0x3F, // 0: a b c d e f
    0x06, // 1:   b c
    0x5B, // 2: a b   d e   g
    0x4F, // 3: a b c d     g
    0x66, // 4:   b c     f g
    0x6D, // 5: a   c d   f g
    0x7D, // 6: a   c d e f g
    0x07, // 7: a b c
    0x7F, // 8: a b c d e f g
    0x6F  // 9: a b c d   f g
};

void sevenseg_draw_digit(int x, int y, int seg, int thick, int d, UWORD fg, UWORD bg) {
    int w = 2*seg + thick;
    int h = 3*seg + 2*thick;
    fill_rect(x, y, w, h, bg);

    uint8_t bits = (d >= 0 && d <= 9) ? DIGIT_BITS[d] : 0;

    if (bits & SEG_A) seg_A(x,y,seg,thick,fg);
    if (bits & SEG_B) seg_B(x,y,seg,thick,fg);
    if (bits & SEG_C) seg_C(x,y,seg,thick,fg);
    if (bits & SEG_D) seg_D(x,y,seg,thick,fg);
    if (bits & SEG_E) seg_E(x,y,seg,thick,fg);
    if (bits & SEG_F) seg_F(x,y,seg,thick,fg);
    if (bits & SEG_G) seg_G(x,y,seg,thick,fg);
}

void sevenseg_draw_colon(int x, int y, int dot, int gap, UWORD fg) {
    fill_rect(x, y,             dot, dot, fg);
    fill_rect(x, y + dot + gap, dot, dot, fg);
}

void sevenseg_draw_time(datetime_t t, UWORD *fb) {
    ensure_dotw(&t);
    Paint_Clear(BLACK);

    char date_line[48];
    snprintf(date_line, sizeof date_line, "%s %02d/%02d/%02d",
             weekday_name(t.dotw), t.day, t.month, t.year % 100);
    Paint_DrawString_EN(0, 10, date_line, &Font20, BLACK, MAGENTA);

    const int seg   = 18;
    const int thick = 6;
    const int gap   = 6;
    const int colon_gap = 8;
    const UWORD FG = WHITE;
    const UWORD BG = BLACK;

    const int dw = 2*seg + thick;
    const int x0 = 8;
    const int y0 = 36;

    int h1 = (t.hour / 10) % 10;
    int h2 = t.hour % 10;
    int m1 = (t.min / 10) % 10;
    int m2 = t.min % 10;

    sevenseg_draw_digit(x0,                 y0, seg, thick, h1, FG, BG);
    sevenseg_draw_digit(x0 + dw + gap,      y0, seg, thick, h2, FG, BG);

    int colon_x = x0 + (dw*2) + gap*2 + 4;
    int colon_y = y0 + thick + (seg/2) - (thick/2);
    sevenseg_draw_colon(colon_x, colon_y, thick, colon_gap, FG);

    int mm_x = colon_x + thick + gap + 6;
    sevenseg_draw_digit(mm_x,               y0, seg, thick, m1, FG, BG);
    sevenseg_draw_digit(mm_x + dw + gap,    y0, seg, thick, m2, FG, BG);

    LCD_1IN14_Display(fb);
}
