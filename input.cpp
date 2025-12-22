#include "input.h"
#include "video.h"
#include "audio.h"
#include "cd.h"

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
                } else if(event.key.keysym.sym == BTN_UP) {
                    index_list--;
                    if(index_list < 0) {
                        index_list = max_list-1;
                    }
                    update_bg = 1;
                } else if(event.key.keysym.sym == BTN_DOWN) {
                    index_list++;
                    if(index_list > max_list-1) {
                        index_list = 0;
                    }
                    update_bg = 1;
                } else if(event.key.keysym.sym == BTN_LEFT) {
                    play_song(--current_song);
                } else if(event.key.keysym.sym == BTN_RIGHT) {
                    play_song(++current_song);
                } else if(event.key.keysym.sym == BTN_A) {
                    current_song = index_list;
                    play_song(index_list);
                } else if(event.key.keysym.sym == BTN_B) {
                    toggle_pause();
                    state = 1;
                }
            break;
        }
    }
}