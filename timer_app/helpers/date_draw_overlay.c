#include "date_draw_overlay.h"
#include "GUI_Paint.h"
#include "date_time_helper.h"

void date_overlay_draw(const datetime_t *t_in, int x, int y,
                       const sFONT *font, UWORD fg, UWORD bg)
{
    if (!t_in || !font) return;

    datetime_t t = *t_in;
    ensure_dotw(&t);

    char line[48];
    snprintf(line, sizeof line, "%s %02d/%02d/%02d",
             weekday_name(t.dotw), t.day, t.month, t.year % 100);

    Paint_DrawString_EN(x, y, line, (sFONT*)font, bg, fg);
}
