#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pixman.h>
#include <SDL2/SDL.h>

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

    pixman_image_t *src_img;
    pixman_color_t red = {0xffff, 0x0000, 0x0000, 0xffff};
    src_img = pixman_image_create_solid_fill(&red);

    pixman_image_t *mask_img;
    uint32_t *mbits = (uint32_t *)malloc(WIDTH * HEIGHT);
    memset(mbits, 0, WIDTH * HEIGHT);
    mask_img = pixman_image_create_bits(PIXMAN_a1, WIDTH, HEIGHT, mbits, WIDTH);

    pixman_image_t *dest_img;
    uint32_t *bits = (uint32_t *)malloc(WIDTH * HEIGHT * 4);
    memset(bits, 0xff, WIDTH * HEIGHT * 4);
    dest_img = pixman_image_create_bits(PIXMAN_a8r8g8b8, WIDTH, HEIGHT, bits, WIDTH * 4);

    pixman_add_traps(mask_img, 0, 0, 1, &trap);

    pixman_image_composite(PIXMAN_OP_OVER, src_img, mask_img, dest_img, 0, 0, 0, 0, 0, 0, WIDTH, HEIGHT);

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create SDL window
    SDL_Window *win = SDL_CreateWindow("Pixman Image", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (win == NULL)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create SDL render
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL)
    {
        SDL_DestroyWindow(win);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create SDL 2D texture to show Pixman result
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    if (tex == NULL)
    {
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        printf("SDL_CreateTexture Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Texturing with pixman result
    SDL_UpdateTexture(tex, NULL, bits, WIDTH * 4);

    // Clear render and show image
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, tex, NULL, NULL);
    SDL_RenderPresent(ren);

    // Wait to close window
    SDL_Event e;
    int quit = 0;
    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = 1;
            }
        }
    }

    // free resource
    free(bits);
    free(mbits);
    pixman_image_unref(src_img);
    pixman_image_unref(mask_img);
    pixman_image_unref(dest_img);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
