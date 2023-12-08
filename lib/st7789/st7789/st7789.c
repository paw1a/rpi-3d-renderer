#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "st7789.h"

static const uint8_t generic_st7789[] = { // Init commands for 7789 screens
    9,									  //  9 commands in list:
    ST77XX_SWRESET, ST_CMD_DELAY,		  //  1: Software reset, no args, w/delay
    150,								  //     ~150 ms delay
    ST77XX_SLPOUT, ST_CMD_DELAY,		  //  2: Out of sleep mode, no args, w/delay
    10,									  //      10 ms delay
    ST77XX_COLMOD, 1 + ST_CMD_DELAY,	  //  3: Set color mode, 1 arg + delay:
    0x55,								  //     16-bit color
    10,									  //     10 ms delay
    ST77XX_MADCTL, 1,					  //  4: Mem access ctrl (directions), 1 arg:
    0x08,								  //     Row/col addr, bottom-top refresh
    ST77XX_CASET, 4,					  //  5: Column addr set, 4 args, no delay:
    0x00,
    0, //     XSTART = 0
    0,
    240,			 //     XEND = 240
    ST77XX_RASET, 4, //  6: Row addr set, 4 args, no delay:
    0x00,
    0, //     YSTART = 0
    320 >> 8,
    320 & 0xFF,					//     YEND = 320
    ST77XX_INVON, ST_CMD_DELAY, //  7: hack
    10,
    ST77XX_NORON, ST_CMD_DELAY,	 //  8: Normal display on, no args, w/delay
    10,							 //     10 ms delay
    ST77XX_DISPON, ST_CMD_DELAY, //  9: Main screen turn on, no args, delay
    10
};						 //    10 ms delay

void initSPI(display_t *display) {
    spi_init(display->spi, 1000 * 40000);
    spi_set_format(display->spi, 16, SPI_CPOL_1, SPI_CPOL_1, SPI_MSB_FIRST);
    gpio_set_function(display->pinSCK, GPIO_FUNC_SPI);
    gpio_set_function(display->pinTX, GPIO_FUNC_SPI);

    gpio_init(display->pinDC);
    gpio_set_dir(display->pinDC, GPIO_OUT);
    gpio_put(display->pinDC, 1);

    if (display->pinRST != -1) {
        gpio_init(display->pinRST);
        gpio_set_dir(display->pinRST, GPIO_OUT);
        gpio_put(display->pinRST, 1);
    }
}

void ST7789_Reset(display_t *display) {
    if (display->pinRST != -1) {
        gpio_put(display->pinRST, 0);
        sleep_ms(5);
        gpio_put(display->pinRST, 1);
    }
}

void ST7789_RegCommand(display_t *display) {
    gpio_put(display->pinDC, 0);
}

void ST7789_RegData(display_t *display) {
    gpio_put(display->pinDC, 1);
}

void ST7789_WriteCommand(display_t *display, uint8_t cmd) {
    ST7789_RegCommand(display);
    spi_set_format(display->spi, 8, SPI_CPOL_1, SPI_CPOL_1, SPI_MSB_FIRST);
    spi_write_blocking(display->spi, &cmd, sizeof(cmd));
}

void ST7789_WriteData(display_t *display, const uint8_t *buff, size_t buff_size) {
    ST7789_RegData(display);
    spi_set_format(display->spi, 8, SPI_CPOL_1, SPI_CPOL_1, SPI_MSB_FIRST);
    spi_write_blocking(display->spi, buff, buff_size);
}

void ST7789_SendCommand(display_t *display, uint8_t commandByte,
                        const uint8_t *dataBytes, uint8_t numDataBytes) {
    ST7789_WriteCommand(display, commandByte);
    ST7789_WriteData(display, dataBytes, numDataBytes);
}

void ST7789_displayInit(display_t *display, const uint8_t *addr) {
    uint8_t numCommands, cmd, numArgs;
    uint16_t ms;

    numCommands = *(addr++); // Number of commands to follow
    while (numCommands--) {
        cmd = *(addr++);		 // Read command
        numArgs = *(addr++);		 // Number of args to follow
        ms = numArgs & ST_CMD_DELAY;     // If hibit set, delay follows args
        numArgs &= ~ST_CMD_DELAY;	 // Mask out delay bit
        ST7789_SendCommand(display, cmd, addr, numArgs);
        addr += numArgs;

        if (ms) {
            ms = *(addr++); // Read post-command delay time (ms)
            if (ms == 255)
                ms = 500; // If 255, delay for 500 ms
            sleep_ms(ms);
        }
    }
}

void LCD_setRotation(display_t *display, uint8_t m) {
    uint8_t madctl = 0;
    display->rotation = m & 3; // can't be higher than 3

    int16_t rowstart = 320 - display->height;
    int16_t rowstart2 = 0;
    int16_t colstart = 240 - display->width;

    switch (display->rotation) {
    case 0:
        madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB;
        display->xstart = colstart;
        display->ystart = rowstart;
        break;
    case 1:
        madctl = ST77XX_MADCTL_MY | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
        display->xstart = rowstart;
        display->ystart = colstart;
        break;
    case 2:
        madctl = ST77XX_MADCTL_RGB;
        display->xstart = colstart;
        display->ystart = rowstart2;
        break;
    case 3:
        madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
        display->xstart = rowstart2;
        display->ystart = colstart;
        break;
    }

    ST7789_SendCommand(display, ST77XX_MADCTL, &madctl, 1);
}

void LCD_initDisplay(display_t *display, uint16_t width, uint16_t height) {
    initSPI(display);
    display->width = 240;
    display->height = 240;
    ST7789_Reset(display);
    ST7789_displayInit(display, generic_st7789);
    LCD_setRotation(display, 2);
}

void LCD_setAddrWindow(display_t *display, uint16_t x, uint16_t y,
                       uint16_t w, uint16_t h) {
    x += display->xstart;
    y += display->ystart;

    uint32_t xa = ((uint32_t)x << 16) | (x + w - 1);
    uint32_t ya = ((uint32_t)y << 16) | (y + h - 1);

    xa = __builtin_bswap32(xa);
    ya = __builtin_bswap32(ya);

    ST7789_WriteCommand(display, ST77XX_CASET);
    ST7789_WriteData(display, &xa, sizeof(xa));

    // row address set
    ST7789_WriteCommand(display, ST77XX_RASET);
    ST7789_WriteData(display, &ya, sizeof(ya));

    // write to RAM
    ST7789_WriteCommand(display, ST77XX_RAMWR);
}

void LCD_WriteBitmap(display_t *display, uint16_t x, uint16_t y,
                     uint16_t w, uint16_t h, uint16_t *bitmap) {
    LCD_setAddrWindow(display, x, y, w, h); // Clipped area
    ST7789_RegData(display);
    spi_set_format(display->spi, 16, SPI_CPOL_1, SPI_CPOL_1, SPI_MSB_FIRST);
    spi_write16_blocking(display->spi, bitmap, w * h);
}

void LCD_WritePixel(display_t *display, int x, int y, uint16_t col) {
    LCD_setAddrWindow(display, x, y, 1, 1); // Clipped area
    ST7789_RegData(display);
    spi_set_format(display->spi, 16, SPI_CPOL_1, SPI_CPOL_1, SPI_MSB_FIRST);
    spi_write16_blocking(display->spi, &col, 1);
}
