#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <SDL/SDL.h>
#include "common.h"

extern SDL_Surface *screen;
extern s16 index_list;
extern u16 offset;
extern u8 update_bg;
extern u8 state;

void init_video();
void update_video();
void close_video();

#endif // _VIDEO_H_