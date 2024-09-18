#include "utility.h"

void convert_8bpp_32bpp(int width, int height, uint32_t *gray, uint32_t *argb)
{
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t mask_value =
                ((uint8_t *) gray)[y * width + x];  // 8bpp value
            uint32_t argb_pixel = (mask_value << 24) | (mask_value << 16) |
                                  (mask_value << 8) |
                                  mask_value;  // Gray value expanded to ARGB
            argb[y * width + x] = argb_pixel;
        }
    }
}
