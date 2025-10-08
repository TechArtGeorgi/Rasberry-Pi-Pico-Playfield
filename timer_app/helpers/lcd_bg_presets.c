#include "lcd_bg_presets.h"

// Deep purple → red → orange → light pink
const UWORD GRADIENT_SUNSET[4] = { 0x780F, 0xF800, 0xFD20, 0xFBB7 };
const UWORD DOUBLE_GRADIENT_SUNSET[8] = {0x780F, 0xF800, 0xFD20, 0xFBB7,0x001F, 0x07FF, 0x5D9B, 0xFFFF};
// Red → muted grayish tones
const UWORD GRADIENT_SUNDOWN[3] = { 0xF800, 0x5555, 0x6666 };

// Blue → cyan → light blue → white
const UWORD GRADIENT_OCEAN[4] = { 0x001F, 0x07FF, 0x5D9B, 0xFFFF };

// Dark green → medium green → yellow-green → pale yellow
const UWORD GRADIENT_FOREST[4] = { 0x0320, 0x07E0, 0xAFE5, 0xFFF0 };

const UWORD SOLID_BLACK[1] = {0x0000}; 
const UWORD SOLID_WHITE[1] = {0xFFFF};
const UWORD MIDNIGHT_CITY[4] = {0x0149, 0x018A, 0x01CC,0x020D};