
#include "font.h"
#include "font_6x11_data.h"


Font font_get_6x11(void) {
    Font font;
    font.char_height = 11;
    font.char_width = 6;
    font.char_start = 4;
    font.mask = 0x80;
    font.fontdata = fontdata_font6x11;
    return font;

}

