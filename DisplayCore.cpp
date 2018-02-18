#include <DisplayCore.h>
#include <stdarg.h>

DisplayCore::DisplayCore() {
    cursor_y = cursor_x = 0;
    textcolor = 0xFFFF;
    textbgcolor = 0;
    wrap = true;
    font = Fonts::Default;
}

void DisplayCore::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    setPixel(x, y, rgb(r, g, b));
}
