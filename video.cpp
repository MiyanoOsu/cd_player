#include "video.h"
#include "font.h"
#include "audio.h"
#include "cd.h"

SDL_Surface* screen, *layout;

SDL_Color black = {0,0,0};
SDL_Color white = {255,255,255};
SDL_Color orange = {245,73,39};
SDL_Color yellow = {219,196,10};

s16 index_list = 0;
u16 offset = 0;
u8 MAX_VISIBLE_LIST = 9;

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

    layout = SDL_CreateRGBSurface(SDL_SWSURFACE,320,240,16,0,0,0,0);
    if(layout == NULL) {
        printf("Could not create surface: %s\n",SDL_GetError());
        return;
    }

    SDL_WM_SetCaption("CD", NULL);
    SDL_ShowCursor(SDL_DISABLE);
}

void draw_layout() {
    SDL_FillRect(layout, NULL, SDL_MapRGB(layout->format,73,106,204));

    if (index_list < offset) 
        offset = index_list;
    
    if (index_list >= offset + MAX_VISIBLE_LIST)
        offset = index_list - MAX_VISIBLE_LIST + 1;

    if (max_list < MAX_VISIBLE_LIST) {
        for(u8 i = 0; i < max_list; i++) {
            if(i == index_list)
                draw_string(playlist[i], layout, update_text_pos(playlist[i], i), 15 + (i - offset) * 20, white);
            else
                draw_string(playlist[i], layout, 35, 15 + (i - offset) * 20, black);
        }
    } else {
        for(u16 i = offset; i < offset + MAX_VISIBLE_LIST; i++) {
            if(i == index_list)
                draw_string(playlist[i], layout, update_text_pos(playlist[i], i), 15 + (i - offset) * 20, white);
            else
                draw_string(playlist[i], layout, 35, 15 + (i - offset) * 20, black);
        }
    }
    
    if (!is_paused) {
        draw_string("Playing...", layout, 5, 20 + 9*20, white);
    } else {
        draw_string("Stopped", layout, 5, 20 + 9*20, white);
    }
    
    if (!mode) {
        draw_string("Mode: continuous", layout, 155, 20 + 9*20, white);
    } else {
        draw_string("Mode: repeat", layout, 155, 20 + 9*20, white);
    }

    draw_string(playlist[current_song], layout, 50, 20 + 10*20, white);
}

void lock_fps(uint8_t fps)
{
    // Use uint32_t to match SDL_GetTicks and prevent overflow every 65 seconds
    static uint32_t lastTicks = 0; 
    uint32_t currentTicks = SDL_GetTicks();
    
    // Calculate how many milliseconds one frame should take (e.g., 1000 / 60 = 16ms)
    uint32_t targetTicks = 1000 / fps;

    // Check how much time has passed since the last frame
    uint32_t elapsedTicks = currentTicks - lastTicks;

    // If the frame finished early, wait for the remaining time
    if (elapsedTicks < targetTicks) {
        SDL_Delay(targetTicks - elapsedTicks);
    }

    // Update lastTicks for the next cycle
    lastTicks = SDL_GetTicks(); 
}

u8 update_bg = 1;

void update_video() {
    draw_layout();
    SDL_BlitSurface(layout,NULL,screen,NULL);
    if(update_bg) {
        SDL_Flip(screen);
        update_bg = 0;
    } else if (dirty_count > 0) {
        SDL_UpdateRects(screen, dirty_count, dirty_rects);
        dirty_count = 0; // reset for the next frame
    }
    lock_fps(10); // due to the default block size of the FLAC format
                  // and audio frames are different from video frames
                  // it keep the program main loop run around 9-10 frame per seconds
                  // so when pause music, it should lock program main loop at 10fps
                  // Sample Rate: 44100 Hz (Samples per second)
                  // Block Size: 4096 Samples (Samples per frame)
                  // Duration of 1 frame = block size / sample rate = 4096 / 44100 = 0.0928 seconds
                  // -> frame per second = 1/0928 = 10.7fps
}

void close_video() {
    SDL_FreeSurface(layout);
    SDL_FreeSurface(screen);
    SDL_Quit();
}