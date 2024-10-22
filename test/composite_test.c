#include <assert.h>
#include <pixman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdl_manager.h"
#include "utility.h"

#define WIDTH 520
// #define WIDTH 522
#define HEIGHT 520
#define SCALE 70.0

// Function to create an SDL_Surface from a raw ARGB array
SDL_Surface* create_surface_from_argb(void* argb_pixels, int width, int height, int pitch) {
    // ARGB masks for a 32-bit surface
    Uint32 rmask = 0x00ff0000;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x000000ff;
    Uint32 amask = 0xff000000;

    // Create SDL surface from the pixel array (void* argb_pixels)
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
        argb_pixels,     // Your pixel data
        width,           // Surface width
        height,          // Surface height
        32,              // Bits per pixel (ARGB is 32 bits)
        pitch,           // The number of bytes per row (pitch)
        rmask, gmask, bmask, amask // Masks for ARGB format
    );

    if (!surface) {
        printf("SDL_CreateRGBSurfaceFrom Error: %s\n", SDL_GetError());
        return NULL;
    }

    return surface;
}

int main(int argc, char **argv)
{
    /*8bpp mask image*/
    pixman_image_t *mask_img;
    uint32_t *mask_gray_data = (uint32_t *) malloc(WIDTH * HEIGHT);
    memset(mask_gray_data, 0 /*black*/, WIDTH * HEIGHT);  // every byte set to 0
    // int padding_stride = WIDTH + (4 - WIDTH % 4);
    int padding_stride = WIDTH - (WIDTH % 4);
    mask_img = pixman_image_create_bits(
        PIXMAN_a8, WIDTH, HEIGHT, mask_gray_data,
        padding_stride);  // array size: WIDTH * HEIGHT / 4

    /*Set mask*/
    // method 1: trapezoid
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
        pixman_add_traps(mask_img, 0, 0, 1, &trap);
    }
    
    // method 2: box
    {
        pixman_box32_t box;
        int left = 40, right = WIDTH - 40;
        box.x1 = left;
        box.y1 = HEIGHT / 2;
        box.x2 = right;
        box.y2 = box.y1 + 1;

        pixman_color_t color = {0xffff, 0xffff, 0xffff, 0xffff};
        pixman_image_fill_boxes(PIXMAN_OP_SRC,  mask_img, &color, 1, &box);
    }

    // method 3: rectangle
    {
        pixman_rectangle16_t rect;
        rect.x = 0;
        rect.y = 0;
        rect.width = 60;
        rect.height = 40;

        pixman_color_t color = {0xffff, 0xffff, 0xffff, 0xffff};
        pixman_image_fill_rectangles(PIXMAN_OP_SRC, mask_img, &color, 1, &rect);
    }

    // source solid
    pixman_image_t *src_img;
    pixman_color_t red = {0xffff, 0x0000, 0x0000, 0xffff};
    src_img = pixman_image_create_solid_fill(&red);

    // destination image
    pixman_image_t *dest_img;
    uint32_t *dst_data = (uint32_t *) malloc(WIDTH * HEIGHT * 4);
    // memset(dst_data, 0xff /*white*/, WIDTH * HEIGHT * 4);
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        dst_data[i] = 0xFF0000FF;  // AARRGGBB
    }
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

    // ARGB to ABGR
    SDL_Surface* argb_surf = create_surface_from_argb(dst_data, WIDTH, HEIGHT, 4 * WIDTH);
    SDL_Surface* abgr_surf = SDL_ConvertSurfaceFormat(argb_surf, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(argb_surf);

    // Create windows
    SDLWindow *mask_win =
        SDLWindow_create("mask", WIDTH, HEIGHT, mask_argb_data);
    SDLWindow *result_win = SDLWindow_create("result", WIDTH, HEIGHT, abgr_surf->pixels);

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

    SDL_FreeSurface(abgr_surf);

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
