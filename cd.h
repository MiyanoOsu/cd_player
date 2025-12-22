#ifndef _CD_H_
#define _CD_H_

#include <libgen.h>

#include "common.h"

extern u16 max_list;
extern char *playlist[1024];
extern char *current_directory;

s16 update_text_pos(char *filename, u16 index);
void load_first_position(char *file);
void load_list_file();

#endif // _CD_H_