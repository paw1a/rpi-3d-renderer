#ifndef gfx_H
#define gfx_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gfxfont.h"
#include "pico/stdlib.h"
#include "st7789.h"

void GFX_createFramebuf(display_t *display);

void GFX_drawPixel(display_t *display, int16_t x, int16_t y, uint16_t color);
void GFX_drawLine(display_t *display, int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                  uint16_t color);
void GFX_drawFastVLine(display_t *display, int16_t x, int16_t y, int16_t h, uint16_t color);
void GFX_drawFastHLine(display_t *display, int16_t x, int16_t y, int16_t l, uint16_t color);

void GFX_fillRect(display_t *display, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void GFX_clearScreen(display_t *display);
void GFX_flush(display_t *display);
void GFX_flush_block(display_t *display, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

#ifdef __cplusplus
}
#endif

#endif