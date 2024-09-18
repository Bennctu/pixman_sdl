#include <assert.h>
#include <pixman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdl_manager.h"

#define WIDTH 520
#define HEIGHT 520
#define SCALE 70.0

int main(int argc, char **argv)
{
    pixman_trap_t trap;

    /**
     *    --->x      top_l     top_r
     *    |           *---------*
     *  y v           |          \
     *                |           \
     *                *------------*bot_r
     *               bot_l
     */
    trap.top.l = pixman_double_to_fixed(0.5 * SCALE);
    trap.top.r = pixman_double_to_fixed(1.5 * SCALE);
    trap.top.y = pixman_double_to_fixed(0.5 * SCALE);

    trap.bot.l = pixman_double_to_fixed(0.5 * SCALE);
    trap.bot.r = pixman_double_to_fixed(1.5 * SCALE);
    trap.bot.y = pixman_double_to_fixed(1.5 * SCALE);

    /*8bpp mask image*/
    pixman_image_t *mask_img;
    uint32_t *mbits = (uint32_t *) malloc(WIDTH * HEIGHT);
    memset(mbits, 0, WIDTH * HEIGHT);  // black
    mask_img = pixman_image_create_bits(PIXMAN_a8, WIDTH, HEIGHT, mbits,
                                        WIDTH);  // width

    // set mask
    pixman_add_traps(mask_img, 0, 0, 1, &trap);

    pixman_image_t *src_img;
    pixman_color_t red = {0xffff, 0x0000, 0x0000, 0xffff};  // 2Bytes
    src_img = pixman_image_create_solid_fill(&red);

    pixman_image_t *dest_img;
    uint32_t *bits = (uint32_t *) malloc(WIDTH * HEIGHT * 4);
    memset(bits, 0xff, WIDTH * HEIGHT * 4);  // white
    dest_img = pixman_image_create_bits(PIXMAN_a8r8g8b8 /*4Bytes*/, WIDTH,
                                        HEIGHT, bits, WIDTH * 4);  // width*4

    pixman_image_composite(PIXMAN_OP_OVER, src_img, mask_img, dest_img, 0, 0, 0,
                           0, 0, 0, WIDTH, HEIGHT);

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDLWindow *mask_win = SDLWindow_create("mask", WIDTH, HEIGHT);
    SDLWindow *result_win = SDLWindow_create("result", WIDTH, HEIGHT);

    // Convert 8bpp mask image to 32bpp ARGB format
    uint32_t *argb_data =
        (uint32_t *) malloc(WIDTH * HEIGHT * 4);  // 32bpp buffer
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            uint8_t mask_value =
                ((uint8_t *) mbits)[y * WIDTH + x];  // 8bpp value
            uint32_t argb_pixel = (mask_value << 24) | (mask_value << 16) |
                                  (mask_value << 8) |
                                  mask_value;  // Gray value expanded to ARGB
            argb_data[y * WIDTH + x] = argb_pixel;
        }
    }

    SDLWindow_update_texture(mask_win, argb_data);
    SDLWindow_update_texture(result_win, bits);

    // Wait to close window
    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }
    }

    // free resource
    free(bits);
    free(mbits);
    free(argb_data);
    pixman_image_unref(src_img);
    pixman_image_unref(mask_img);
    pixman_image_unref(dest_img);

    SDLWindow_destroy(mask_win);
    SDLWindow_destroy(result_win);

    SDL_Quit();

    return 0;
}
