#include <Cariad.h>
#include <stdarg.h>

/*!
 *  Opens the rectangle defined by (x0,y0) to (x1,y1) as a raw data window.
 *
 *  Example:
 *  
 *      tft.openWindow(0, 0, 100, 100);
 */
void Cariad::openWindow(int x0, int y0, int x1, int y1) {
    winx0 = x0;
    winy0 = y0;
    winx1 = x0 + x1;
    winy1 = y0 + y1;
    winpx = 0;
    winpy = 0;
    startBuffer();
}

/*!
 *  Sends the raw pixel data for one pixel to the currently opened window.
 *
 *  Example:
 *
 *      tft.windowData(Color::Red);
 */
void Cariad::windowData(color_t d) {
    setPixel(winx0 + winpx, winy0 + winpy, d);
    winpx++;
    if ((winx0 + winpx) >= winx1) {
        winpx = 0;
        winpy++;
    }
    if ((winy0 + winpy) >= winy1) {
        winpy = 0;
    }
}

/*!
 *  The array of pixel data (*d) ans size (l) is dumped verbatim
 *  to the currently opened window.
 *
 *  Example:
 *
 *      tft.windowData(myData, 1000);
 */
void Cariad::windowData(const color_t *d, int l) {
    for (int i = 0; i < l; i++) {
        windowData(d[i]);
    }
}

/*!
 *  Close the currently opened window and return to normal
 *  drawing operations.
 *
 *  Example:
 *
 *      tft.closeWindow();
 */

void Cariad::closeWindow() {
    endBuffer();
}




