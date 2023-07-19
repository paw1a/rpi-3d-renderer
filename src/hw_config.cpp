#include <cstring>
#include "hw_config.h"

static spi_t spis[] = {{
    .hw_inst = spi0,
    .miso_gpio = 16,
    .mosi_gpio = 19,
    .sck_gpio = 18,
    .baud_rate = 12500 * 1000
}};

static sd_card_t sd_cards[] = {{
    .pcName = "0:",
    .spi = &spis[0],
    .ss_gpio = 17,
}};

size_t sd_get_num() { return count_of(sd_cards); }

sd_card_t *sd_get_by_num(size_t num) {
    if (num <= sd_get_num()) {
        return &sd_cards[num];
    } else {
        return nullptr;
    }
}

size_t spi_get_num() { return count_of(spis); }

spi_t *spi_get_by_num(size_t num) {
    if (num <= sd_get_num()) {
        return &spis[num];
    } else {
        return nullptr;
    }
}
