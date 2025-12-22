#include "common.h"
#include "font.h"
#include "config.h"
#include "audio.h"
#include "input.h"

#include <SDL/SDL_ttf.h>

TTF_Font *font;

int get_text_width(char *text) {
    int text_w = 0, text_h = 0;
    TTF_SizeText(font, text , &text_w, &text_h);
    return text_w;
}

void load_font() {
    font = TTF_OpenFont(option.font, option.font_size);
    if (!font) {
        printf( "Font load error: %s\n", TTF_GetError());
        exit(1);
    }
}

void init_font() {
    TTF_Init();
    load_font();
}

SDL_Rect dirty_rects[MAX_DIRTY];
u8 dirty_count = 0;
u8 state = 1;

void draw_string(const char *string, SDL_Surface *surface, s16 x, s16 y, SDL_Color font_color) {
    int w = 0, h = 0;
    
    TTF_SizeText(font, string, &w, &h);
    SDL_Rect dst = {x, y, (u16)w, (u16)h};

    // graphics dirty rectangles
    if(state) {
        dirty_rects[dirty_count].x = 5;
        dirty_rects[dirty_count].y = 20 + 9*20;
        dirty_rects[dirty_count].w = w;
        dirty_rects[dirty_count].h = h;
        dirty_count++;
        state = 0;
    } else if (dirty_count < MAX_DIRTY) {
        dirty_rects[dirty_count].x = 35;
        dirty_rects[dirty_count].y = y;
        dirty_rects[dirty_count].w = 320-35;
        dirty_rects[dirty_count].h = h;
        dirty_count++;
    }

    SDL_Surface *text_surface = TTF_RenderUTF8_Blended(font, string, font_color);
    SDL_BlitSurface(text_surface, NULL, surface, &dst);
    SDL_FreeSurface(text_surface);
}

void close_font() {
    TTF_CloseFont(font);
    TTF_Quit();
}