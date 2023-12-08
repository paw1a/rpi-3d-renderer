#pragma once

#include "common.h"
#include "st7789.h"

void warnock_render(display_t *display, const window &window, uint16_t bg_color,
                    void set_pixel(display_t *, point2, uint16_t));
