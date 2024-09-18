#include <assert.h>
#include <pixman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdl_manager.h"
#include "utility.h"

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
    uint32_t *mask_gray_data = (uint32_t *) malloc(WIDTH * HEIGHT);
    memset(mask_gray_data, 0 /*black*/, WIDTH * HEIGHT);
    mask_img = pixman_image_create_bits(PIXMAN_a8, WIDTH, HEIGHT,
                                        mask_gray_data, WIDTH);

    // set mask
    pixman_add_traps(mask_img, 0, 0, 1, &trap);

    // source solid
    pixman_image_t *src_img;
    pixman_color_t red = {0xffff, 0x0000, 0x0000, 0xffff};
    src_img = pixman_image_create_solid_fill(&red);

    // destination image
    pixman_image_t *dest_img;
    uint32_t *dst_data = (uint32_t *) malloc(WIDTH * HEIGHT * 4);
    memset(dst_data, 0xff /*white*/, WIDTH * HEIGHT * 4);
    dest_img = pixman_image_create_bits(PIXMAN_a8r8g8b8 /*4Bytes*/, WIDTH,
                                        HEIGHT, dst_data, WIDTH * 4);

    // over the mask area
    pixman_image_composite(PIXMAN_OP_OVER, src_img, mask_img, dest_img, 0, 0, 0,
                           0, 0, 0, WIDTH, HEIGHT);

    // Convert 8bpp mask image to 32bpp ARGB format
    uint32_t *mask_argb_data =
        (uint32_t *) malloc(WIDTH * HEIGHT * 4);  // 32bpp buffer
    convert_8bpp_32bpp(WIDTH, HEIGHT, mask_gray_data, mask_argb_data);

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create windows
    SDLWindow *mask_win =
        SDLWindow_create("mask", WIDTH, HEIGHT, mask_argb_data);
    SDLWindow *result_win = SDLWindow_create("result", WIDTH, HEIGHT, dst_data);

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

    SDLWindow_destroy(mask_win);
    SDLWindow_destroy(result_win);

    SDL_Quit();

    // free resource
    free(dst_data);
    free(mask_gray_data);
    free(mask_argb_data);
    pixman_image_unref(src_img);
    pixman_image_unref(mask_img);
    pixman_image_unref(dest_img);

    return 0;
}
