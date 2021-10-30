#include <Cariad.h>
#include <stdarg.h>

Cariad::Cariad() {
    cursor_y = cursor_x = 0;
    textcolor = 0xFFFF;
    textbgcolor = 0;
    wrap = true;
    font = Fonts::Default;
}

void Cariad::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    setPixel(x, y, rgb(r, g, b));
}

void Cariad::setContrast(int c) {
}

void Cariad::setBrightness(int b) {
}

int Cariad::getContrast() { return 0; }
int Cariad::getBrightness() { return 0; }

bool Cariad::sync() { return true; }
