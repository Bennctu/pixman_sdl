#include "sdl_manager.h"

struct _SDLWindow {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    int width;
    int height;
    const char *title;
};


SDLWindow *SDLWindow_create(const char *title, int width, int height)
{
    SDLWindow *sdl_window = (SDLWindow *) malloc(sizeof(SDLWindow));
    sdl_window->title = title;
    sdl_window->width = width;
    sdl_window->height = height;

    // Create SDL window
    sdl_window->window =
        SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         width, height, SDL_WINDOW_SHOWN);
    if (!sdl_window->window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        free(sdl_window);
        return NULL;
    }

    // Create SDL renderer
    sdl_window->renderer = SDL_CreateRenderer(
        sdl_window->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdl_window->renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(sdl_window->window);
        free(sdl_window);
        return NULL;
    }

    // Create SDL texture
    sdl_window->texture =
        SDL_CreateTexture(sdl_window->renderer, SDL_PIXELFORMAT_ARGB8888,
                          SDL_TEXTUREACCESS_STREAMING, width, height);
    if (!sdl_window->texture) {
        printf("SDL_CreateTexture Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(sdl_window->renderer);
        SDL_DestroyWindow(sdl_window->window);
        free(sdl_window);
        return NULL;
    }

    return sdl_window;
}

void SDLWindow_update_texture(SDLWindow *sdl_window, void *pixels)
{
    SDL_UpdateTexture(sdl_window->texture, NULL, pixels, sdl_window->width * 4);
    SDL_RenderClear(sdl_window->renderer);
    SDL_RenderCopy(sdl_window->renderer, sdl_window->texture, NULL, NULL);
    SDL_RenderPresent(sdl_window->renderer);
}

void SDLWindow_destroy(SDLWindow *sdl_window)
{
    if (sdl_window) {
        SDL_DestroyTexture(sdl_window->texture);
        SDL_DestroyRenderer(sdl_window->renderer);
        SDL_DestroyWindow(sdl_window->window);
        free(sdl_window);
    }
}