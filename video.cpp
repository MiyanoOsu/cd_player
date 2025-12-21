#include "video.h"

SDL_Surface* screen;

void init_video() {
    if(SDL_Init(SDL_INIT_VIDEO)==-1) {
        printf("Could not initialize SDL: %s\n", SDL_GetError());
        return;
    }

    screen = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE);
    if(screen == NULL) {
        printf("Could not set video mode: %s\n",SDL_GetError());
        return;
    }

    SDL_WM_SetCaption("CD", NULL);
    SDL_ShowCursor(SDL_DISABLE);
}

void update_video() {
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format,73,106,204));
    SDL_Flip(screen);
}

void close_video() {
    SDL_FreeSurface(screen);
    SDL_Quit();
}