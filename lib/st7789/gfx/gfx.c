#include "malloc.h"
#include "pico/stdlib.h"
#include "stdarg.h"

#include "font.h"
#include "gfx.h"
#include "gfxfont.h"
#include "st7789.h"

#ifndef swap
#define swap(a, b)                                                             \
    {                                                                          \
        int16_t t = a;                                                         \
        a = b;                                                                 \
        b = t;                                                                 \
    }
#endif

#define GFX_BLACK 0x0000
#define GFX_WHITE 0xFFFF

uint16_t *gfxFramebuffer = NULL;

static int16_t cursor_y = 0;
int16_t cursor_x = 0;
uint8_t textsize_x = 1;
uint8_t textsize_y = 1;
uint16_t textcolor = GFX_WHITE;
uint16_t textbgcolor = GFX_BLACK;
uint16_t clearColour = GFX_BLACK;
uint8_t wrap = 1;

GFXfont *gfxFont = NULL;

void GFX_setClearColor(uint16_t color) {
    clearColour = color;
}

void GFX_clearScreen(display_t *display) {
    GFX_fillRect(display, 0, 0, display->width, display->height, clearColour);
}

void GFX_fillScreen(display_t *display, uint16_t color) {
    GFX_fillRect(display, 0, 0, display->width, display->height, color);
}

void GFX_drawPixel(display_t *display, int16_t x, int16_t y, uint16_t color) {
    if (gfxFramebuffer != NULL) {
        if ((x < 0) || (y < 0) || (x >= display->width) || (y >= display->width))
            return;
        gfxFramebuffer[x + y * display->width] = color; //(color >> 8) | (color << 8);
    } else
        LCD_WritePixel(display, x, y, color);
}

void GFX_drawLine(display_t *display, int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                  uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }

    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0 <= x1; x0++) {
        if (steep) {
            GFX_drawPixel(display, y0, x0, color);
        } else {
            GFX_drawPixel(display, x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void GFX_drawFastVLine(display_t *display, int16_t x, int16_t y, int16_t h, uint16_t color) {
    GFX_drawLine(display, x, y, x, y + h - 1, color);
}

void GFX_drawFastHLine(display_t *display, int16_t x, int16_t y, int16_t l, uint16_t color) {
    GFX_drawLine(display, x, y, x + l - 1, y, color);
}

void GFX_fillRect(display_t *display, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    for (int16_t i = x; i < x + w; i++) {
        GFX_drawFastVLine(display, i, y, h, color);
    }
}

void GFX_createFramebuf(display_t *display) {
    gfxFramebuffer = malloc(display->width * display->height * sizeof(uint16_t));
}

void GFX_flush(display_t *display) {
    if (gfxFramebuffer != NULL)
        LCD_WriteBitmap(display, 0, 0, display->width, display->height, gfxFramebuffer);
}
