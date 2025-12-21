#include <SDL/SDL.h>
#include "input.h"

t_config option;

void handle_input() {
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type) {
            case SDL_QUIT:
                done = 1;
            break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == BTN_PLUS) {
                    done = 1;
                }
            break;
        }
    }
}