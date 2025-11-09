#include "color.h"
#include "stdint.h"

uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (a << 24) | (r << 16) | (g << 8) | b;
}
uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return rgba(r, g, b, 255);
}
