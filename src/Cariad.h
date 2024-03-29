#ifndef _DISPLAY_CORE_H
#define _DISPLAY_CORE_H

#include <Arduino.h>

#define RGB(r,g,b) ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

#include <stdint.h>

typedef uint16_t color_t;

#include <Color.h>
#include <DefaultFont.h>

#include "DCPinDefs.h"


typedef struct {
    uint8_t linesPerCharacter;
    uint8_t bytesPerLine;
    uint8_t startGlyph;
    uint8_t endGlyph;
    uint8_t bitsPerPixel;
} FontHeader;

struct coord {
    int x;
    int y;
};

class point3d {
    public:
        double x;
        double y;
        double z;
        point3d() : x(0), y(0), z(0) {}
        point3d(double _x,double _y, double _z) : x(_x), y(_y), z(_z) {}

        point3d operator -(const point3d &other) const {
            return point3d(x - other.x, y - other.y, z - other.z);
        }
        point3d operator +(const point3d &other) const {
            return point3d(x + other.x, y + other.y, z + other.z);
        }
        // Cross product

        point3d operator *(const point3d &other) const {
            return point3d(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }
        // Scale
        point3d operator *(double other) const {
            return point3d(
                y * other,
                z * other,
                x * other
            );
        }
        // Dot product
        double dot(const point3d &other) const {
            return x*other.x + y*other.y + z*other.z;
        }
        float Q_rsqrt( float number ) {
            int32_t i;
            float x2, y2;
            const float threehalfs = 1.5F;

            x2 = number * 0.5F;
            y2  = number;
            float *y2fp = &y2;
            int32_t *y2lp = (int32_t *)y2fp;
            i  = *y2lp;
            i  = 0x5f3759df - ( i >> 1 );
            int32_t *ilp = &i;
            float *ifp = (float *)ilp;
            y2  = *ifp;
            y2  = y2 * ( threehalfs - ( x2 * y2 * y2 ) );

            return y2;
        }
        double length() {
            return 1.0/Q_rsqrt(x*x+y*y+z*z);
        }
        point3d norm() {
            double l = Q_rsqrt(x*x+y*y+z*z);
            return point3d(x*l, y*l, z*l);
        }
};

typedef struct {
    point3d a;
    point3d b;
} ray;

typedef struct {
    double x;
    double y;
} point2d;

typedef struct {
    union {
        color_t value;
        struct {
            unsigned r:5;
            unsigned g:6;
            unsigned b:5;
        } __attribute__((packed));
    } __attribute__((packed));
} __attribute__((packed)) Color565;

extern color_t rgb(uint32_t c);
extern color_t rgb(uint8_t r, uint8_t g, uint8_t b);

class Cariad;

// The new font abstration base class. Implements the normal font system as default.
class Font {
    protected:
        const uint8_t *_fontData;

    public:
        Font(const uint8_t *fd) : _fontData(fd) {}
        virtual int getStringWidth(const char *str);
        virtual int getStringHeight(const char *str);
        virtual uint8_t getCharacterWidth(uint8_t glyph);
        virtual uint8_t getStartGlyph();
        virtual uint8_t getEndGlyph();
        virtual int drawChar(Cariad *dev, int x, int y, uint8_t c, color_t fg, color_t bg);
};


class Cariad : public Print
{
    public:
        Cariad();

        /*! \name Primitives
         * @{
         */
        virtual void drawCircle(int x0, int y0, int r, color_t color);
        virtual void fillCircle(int x0, int y0, int r, color_t color);
        virtual void drawLine(int x0, int y0, int x1, int y1, color_t color);
        virtual void drawLine(int x0, int y0, int x1, int y1, int width, color_t color);
        void drawPolygon(point2d *nodes, int numpoints, color_t color);
        void fillPolygon(point2d *nodes, int numpoints, color_t color);
        virtual void drawRectangle(int x, int y, int w, int h, color_t color);
        virtual void drawRoundRect(int x, int y, int w, int h, int r, color_t color);
        virtual void fillRoundRect(int x, int y, int w, int h, int r, color_t color);
        virtual void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color);
        virtual void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color);
        virtual void drawBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, int resolution, color_t color);
        virtual void fillBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, int resolution, color_t color);
        virtual void fillRectangle(int x, int y, int w, int h, color_t color);
        virtual void fillScreen(color_t color);
        virtual void setPixel(int x, int y, color_t color) = 0;
        virtual void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
        virtual void drawHorizontalLine(int x, int y, int w, color_t color);
        virtual void drawVerticalLine(int x, int y, int h, color_t color);

        /* @} */

        /*! \name 3D 
         * @{
         */

        virtual void setPixel(int x, int y, int z, color_t color);
        void drawLine3D(int x1, int y1, int z1, const int x2, const int y2, const int z2, color_t color);
        point2d map3Dto2D(point3d &p);
        void drawPolygon3D(point3d *nodes, int numpoints, color_t color);
        void fillPolygon3D(point3d *nodes, int numpoints, color_t color);
        void setCamera(point3d pos, point3d angle) { _camera = pos; _camang = angle; }
        void setFOV(int fov) { _fov = fov; }
        int getFOV() { return _fov; }

        /*! @} */


        /*! \name Image drawing
         *  These routines are used for drawing basic bitmap images to the screen.
         *  @{
         */
        virtual void drawBitmap(int x, int y, const uint8_t *bitmap, int w, int h, color_t color);
        virtual void drawRGB(int x, int y, const color_t *bitmap, int w, int h);
        virtual void drawRGBA(int x, int y, const color_t *bitmap, int w, int h, color_t trans);
        /*! @} */

        /*! \name Text Handling
         * @{
         */

        virtual void setCursor(int x, int y);
        virtual void setCursorX(int x);
        virtual void setCursorY(int y);
        virtual int getCursorX();
        virtual int getCursorY();
        virtual int getCursor(bool x);
        virtual void setTextColor(color_t c);
        virtual void setTextColor(color_t fg, color_t bg);
        virtual color_t getTextColor();
        virtual void invertTextColor();
        virtual void setTextWrap(bool w);
        virtual void setFont(const uint8_t *f);
        virtual void setFont(Font &f);
        virtual void setFont(Font *f);
        virtual int stringWidth(const char *text);
        virtual int stringHeight(const char *text);
        int drawChar(int x, int y, unsigned char c, color_t color, color_t bg);
#if ARDUINO >= 100
        virtual size_t write(uint8_t c);
        virtual size_t write(const uint8_t *buffer, size_t size);
#else
        virtual void write(uint8_t c);
        virtual void write(const uint8_t *buffer, size_t size);
#endif


        /*! @} */

        /*! \name Colour Handling 
         *  @{
         */
        virtual color_t color565(uint8_t r, uint8_t g, uint8_t b);
        virtual color_t colorAt(int x, int y);
        virtual void getRectangle(int x, int y, int w, int h, color_t *buf);
        point3d rgb2xyz(color_t c);
        point3d xyz2lab(point3d c);
        double deltaE(point3d labA, point3d labB);
        uint32_t deltaOrth(color_t c1, color_t c2);
        static uint32_t rgb2hsv(color_t rgb);
        static color_t hsv2rgb(uint32_t hsv);
        color_t mix(color_t a, color_t b, int pct);
        uint32_t color2rgb(color_t c);

        /*! @} */

        /*! \name Screen Control
         *  @{
         */
        virtual void setRotation(int rotation) = 0;
        /*! 
         *  The display is configured and made ready to work.  This function *must* be called
         *  before anything can happen on the screen, and it *should* be called before any other function.
         *
         *  Example:
         *
         *      tft.initializeDevice();
         */
        virtual void initializeDevice() = 0;
        /*!
         *  Enable the video output of the display (if supported).
         *
         *  Example:
         *
         *      tft.displayOn();
         */
        virtual void displayOn() = 0;
        /*!
         *  Disable the video output of the display (if supported).
         *
         *  Example:
         *
         *      tft.displayOff();
         */
        virtual void displayOff() = 0;
        /*!
         *  All colours become reversed.  Black becomes white, red becomes cyan, etc.
         *
         *  Example:
         *
         *      tft.invertDisplay(true);
         */
        virtual void invertDisplay(bool i) = 0;
        /*!
         *  For devices with their own backlight control this function will turn
         *  the backlight on. The brightness should be either the default brightness
         *  (typically full on) or the last brightness set with `setBacklight()`.
         */
        virtual void enableBacklight() {}
        /*!
         *  For devices with their own backlight control this function will turn
         *  the backlight off.
         */
        virtual void disableBacklight() {}
        /*!
         *  For devices with their own backlight control this function will set the
         *  brightness of the backlight.
         */
        virtual void setBacklight(int __attribute__((unused)) b) {}
        virtual int getWidth();
        virtual int getHeight();

        virtual void setBrightness(int b);
        virtual int getBrightness();
        virtual void setContrast(int c);
        virtual int getContrast();

        virtual bool sync();

        /*! @} */


        /*! \name Window functions
         *  @{
         */
        virtual void openWindow(int x0, int y0, int x1, int y1);
        virtual void windowData(color_t d);
        virtual void windowData(const color_t *d, int l);
        virtual void closeWindow();
        /*! @} */


        /*! \name Buffering
         * @{
         */

        /*! 
         *  In buffered mode, where applicable, any data that would be sent to
         *  the screen is delayed until buffered mode is ended.  This generally
         *  has no effect on most screens, but those that use their own driver
         *  level framebuffer may use this to delay pushing out of the buffer 
         *  to the screen.
         */
        virtual void startBuffer();

        /*!
         *  Any changes that are pending will be pushed out to the screen. See
         *  `startBuffer()` for more information.
         */
        virtual void endBuffer();

        /*! @} */


        /*!
         *  Utility function to get the information about an IO port for high speed
         *  access.
         */
#if defined(__PIC32MX__) || defined(__PIC32MZ__)
        p32_ioport *getPortInformation(uint8_t pin, uint32_t *mask);
#endif

        /*! The text cursor X position */
        int cursor_x;
        /*! The text cursor Y position */
        int cursor_y;
        /*! Whether or not text wrapping is enabled */
        bool wrap;
        /*! Text foreground colour */
        color_t textcolor;
        /*! Text background colour */
        color_t textbgcolor;
        /*! Width of the TFT screen */
        int _width;
        /*! Height of the TFT screen */
        int _height;
        /*! Current rotation */
        int rotation;

        /*! \name Clipping
         * @{
         */
        bool clipToScreen(int &x, int &y, int &w, int &h);
        void setClipping(int x0, int y0, int x1, int y1);
        void clearClipping();
        /*! @} */

        /*! \name Helper functions
         * @{
         */

        void drawCircleHelper( int x0, int y0, int r, int cornername, color_t color);
        void fillCircleHelper(int x0, int y0, int r, int cornername, int delta, color_t color);
        void fatalError(const char *title, const char *message);
        virtual void translateCoordinates(int *x, int *y);
        /*! @} */


        int _clip_x0;
        int _clip_x1;
        int _clip_y0;
        int _clip_y1;

        point3d _camera;
        point3d _camang;
        int _fov;

    protected:
        /*! A pointer to the currently selected font table */
        const uint8_t *font;
        Font *advancedFont;

        int winx0;
        int winy0;
        int winx1;
        int winy1;
        int winpx;
        int winpy;

        color_t bgColor;

};

class Touch {
    public:
        /*! 
         *  This takes a pointer to a communication device, and the width and height of the touch screen.
         */
        Touch(int w, int h) : _width(w), _height(h) {}
        /*!
         *  This configures and enables the touch screen device. It should be called before any other touch screen
         *  functions.
         */
        virtual void initializeDevice() = 0;
        /*!
         *  This returns the X coordinate of the current touch position.
         */
        virtual int x() = 0;
        /*!
         *  This returns the Y coordinate of the current touch position.
         */
        virtual int y() = 0;
        /*!
         *  Returns true if the touch screen is pressed, false otherwise.
         */
        virtual int rawX() { return x(); }
        virtual int rawY() { return y(); }
        virtual bool isPressed() = 0;
        /*!
         *  For touch screens that can calculate how hard you are pressing them, this returns the pressure value.  For others it returns 0.
         *
         *  Example:
         *
         *      int pressure = ts.pressure();
         */
        virtual int pressure() { return 0; }
        /*! 
         *  This sets the screen orientation of the touch screen.  It should be set to the same as the
         *  rotation used for the screen.
         */
        virtual void setRotation(int r) = 0;

        /*! 
         *  This performs a sampling of the touch screen to get the current coordinates and touch status.
         *  It should be called regularly to update the current touch screen data.
         */
        virtual void sample() = 0;

        virtual void scaleX(double x) { _scale_x = x; }
        virtual void scaleY(double y) { _scale_y = y; }
        virtual void offsetX(double x) { _offset_x = x; }
        virtual void offsetY(double y) { _offset_y = y; }

        double _scale_x;
        double _scale_y;
        int _offset_x;
        int _offset_y;

    protected:
        /*! The width of the toush screen in pixels */
        int _width;  
        /*! The height of the touch screen in pixels */
        int _height; 

};

class Filter;

class Filter {
    protected:
        Filter *_next;
    public:
        Filter() : _next(NULL) {};
        color_t process(color_t);
        virtual color_t function(color_t) = 0;
        void chain(Filter &f) { _next = &f; }
        void chain(Filter *f) { _next = f; }
        void endChain() { _next = NULL; }
};

class Image : public Cariad {
    protected:
        Filter *_filter;
    public:
        static const uint8_t MirrorH = 0x01;
        static const uint8_t MirrorV = 0x02;
        static const uint8_t Rotate180 = 0x03;

        Image() : _filter(NULL) {};

        virtual int getWidth() { return _width; }
        virtual int getHeight() { return _height; }


        virtual void draw(Cariad *dev, int x, int y) = 0;
        virtual void draw(Cariad *dev, int x, int y, color_t t) = 0;
        virtual void drawTransformed(Cariad *dev, int x, int y, int transform) = 0;
        virtual void drawTransformed(Cariad *dev, int x, int y, int transform, color_t t) = 0;

        void draw(Cariad &dev, int x, int y) { draw(&dev, x, y); }
        void draw(Cariad &dev, int x, int y, color_t t) { draw(&dev, x, y, t); }
        void drawTransformed(Cariad &dev, int x, int y, int transform) { drawTransformed(&dev, x, y, transform); }
        void drawTransformed(Cariad &dev, int x, int y, int __attribute__((unused)) transform, color_t t) { drawTransformed(&dev, x, y, t); }

        void draw(Cariad *dev) { draw(dev, 0, 0); }
        void draw(Cariad &dev) { draw(&dev, 0, 0); }


        void setFilter(Filter &f) { _filter = &f; };
        void removeFilter() { _filter = NULL; };

        void setRotation(int __attribute__((unused)) r) {}
        void setPixel(int __attribute__((unused)) x, int __attribute__((unused)) y, color_t __attribute__((unused)) c) {}
        void initializeDevice() {}
        void displayOn() {}
        void displayOff() {}
        void invertDisplay(bool __attribute__((unused)) i) {}

        int _width;
        int _height;

};

// An abstraction of an event including the type, location, widget, etc;
class Widget;

struct Event {
    Widget *source;
    int x;
    int y;
    int dx;
    int dy;
    uint32_t type;
};

enum {
    EVENT_PRESS,
    EVENT_RELEASE,
    EVENT_TAP,
    EVENT_DRAG,
    EVENT_REPEAT
};

// A widget is an extension of an image. It's effectively an interactive
// image.  How you build it up on screen is up to you.
class Widget : public Image {
    protected:
        Touch *_ts;
        Cariad *_dev;
        int _x;
        int _y;
        int _value;
        uint32_t _user;
        bool _redraw;
        uint32_t _dbStart;
        bool _dbPressed;

        int _sx;
        int _sy;
        int _ex;
        int _ey;
        int _rx;
        int _ry;
        uint32_t _st;
        uint32_t _rt;
        uint32_t _et;
        int _rc;
        int _rp;
        int _tx;
        int _ty;

        bool _active;
        bool _touch;
        bool _enabled;

        void (*_press)(Event *);
        void (*_release)(Event *);
        void (*_drag)(Event *);
        void (*_tap)(Event *);
        void (*_repeat)(Event *);
        Widget *_eventObject;

        virtual void handlePress(Event *e);
        virtual void handleRelease(Event *e);
        virtual void handleDrag(Event *e);
        virtual void handleTap(Event *e);
        virtual void handleRepeat(Event *e);

    public:
        Widget(Touch &t, Cariad &d, int x, int y) : Image(),
            _ts(&t), _dev(&d), _x(x), _y(y), _redraw(true), _touch(false), _enabled(true),
            _press(NULL), _release(NULL), _drag(NULL), _tap(NULL), _repeat(NULL), _eventObject(NULL)
            {}
        virtual void setValue(int v);
        virtual int getValue();
        virtual void render();
        int _sense_x;
        int _sense_y;
        int _sense_w;
        int _sense_h;

        void setUserValue(uint32_t v) { _user = v; }
        uint32_t getUserValue() { return _user; }

        // Event registering functions
        void onPress(void (*func)(Event *)) { _press = func; }
        void onRelease(void (*func)(Event *)) { _release = func; }
        void onDrag(void (*func)(Event *)) { _drag = func; }
        void onTap(void (*func)(Event *)) { _tap = func; }
        void onRepeat(void (*func)(Event *)) { _repeat = func; }

        void setEventObject(Widget *o) { _eventObject = o; }

        void handleTouch();

        virtual void draw(Cariad *dev, int x, int y) = 0;

        void draw(Cariad *dev, int x, int y, color_t t);
        void drawTransformed(Cariad *dev, int x, int y, int transform);
        void drawTransformed(Cariad *dev, int x, int y, int transform, color_t t);

        void draw(Cariad &dev, int x, int y);
        void draw(Cariad &dev, int x, int y, color_t t);
        void drawTransformed(Cariad &dev, int x, int y, int transform);
        void drawTransformed(Cariad &dev, int x, int y, int transform, color_t t);

        virtual void redraw();

        virtual void setEnabled(bool e);
        virtual bool isEnabled();

        virtual bool isActive();

        void setLocation(int x, int y) { _x = x; _y = y; }

};

// A form is a collection of widgets.  Build widgets into a form, and then
// render the whole form processing any events that may occur within that
// form.
class Form {
    private:
        Widget **_list;
        int _size;

    public:
        Form(int num...);
        ~Form();
        void render();
        void redraw();
        // Event registering functions
        void onPress(void (*func)(Event *));
        void onRelease(void (*func)(Event *));
        void onDrag(void (*func)(Event *));
        void onTap(void (*func)(Event *));
        void onRepeat(void (*func)(Event *));
        Widget **getList() { return _list; }
        int getSize() { return _size; }
};

#define TRIANGLE_HIDDEN 1
#define TRIANGLE_FOUND 2

struct triangle {
    point3d a;
    point3d b;
    point3d c;
    color_t color;
    int flags;
};

class Scene {
    private:
        const triangle *_triangles;
        int _numtriangles;
        point3d _camera;
        point3d _camang;
        point3d _light;
        double _ambient;
        bool _wireframe;
        void (*_traceCallBack)(int);

    public:
        void setTraceCallback(void (*tcb)(int));
        point3d translatePoint(point3d p);
        Scene(const triangle *t, int numt) : _triangles(t), _numtriangles(numt), _ambient(1.0) {}
        void setCameraPosition(point3d c) { _camera = c; }
        void setCameraAngle(point3d a) { _camang = a; }
        void setCameraPosition(int x, int y, int z) { _camera.x = x; _camera.y = y; _camera.z = z; }
        void setCameraAngle(int x, int y, int z) { _camang.x = x; _camang.y = y; _camang.z = z; }
        void setLightPosition(point3d l) { _light = l; }
        void setLightPosition(int x, int y, int z) { _light.x = x; _light.y = y; _light.z = z; }
        void setAmbientLight(double l) { _ambient = l; }

        int render(Cariad *dev);
        int render(Cariad &dev) { return render(&dev); }
        void setWireFrame(bool b);
        void trace(Cariad *dev, float depth, bool smooth = false);
};


#endif


