#include <Cariad.h>
#include <stdarg.h>

/*!
 *  Returns the colour at (x,y) as seen by the screen.
 *
 *  Example:
 *
 *      unsigned int color = tft.colorAt(100, 100);
 */
color_t Cariad::colorAt(int __attribute__((unused)) x, int __attribute__((unused)) y) {
    return bgColor;
}

void Cariad::getRectangle(int __attribute__((unused)) x, int __attribute__((unused)) y, int w, int h, color_t *buf) {
    for (int i = 0; i < w*h; i++) {
        buf[i] = bgColor;
    }
}

