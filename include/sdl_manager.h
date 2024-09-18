#ifndef _SDL_MANAGER_H_
#define _SDL_MANAGER_H_
#include <SDL2/SDL.h>

typedef struct _SDLWindow SDLWindow;

SDLWindow *SDLWindow_create(const char *title, int width, int height);
void SDLWindow_update_texture(SDLWindow *sdl_window, void *pixels);
void SDLWindow_destroy(SDLWindow *sdl_window);
#endif  //_SDL_MANAGER_H_
