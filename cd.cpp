#include "common.h"
#include "cd.h"
#include "font.h"
#include "audio.h"
#include "video.h"
#include <dirent.h>

u16 max_list = 0;
char *playlist[1024] = {NULL};
char *current_directory = NULL;
FILE *fp;
u16 current_song = 0;

char *short_path(const char *fullpath) {
    static char result[128];
    char pathcopy1[128];
    char pathcopy2[128];

    // make two separate copies because dirname() and basename() modify them
    strncpy(pathcopy1, fullpath, sizeof(pathcopy1));
    strncpy(pathcopy2, fullpath, sizeof(pathcopy2));

    char *dir = dirname(pathcopy1);   // modifies pathcopy1
    char *base = basename(pathcopy2); // modifies pathcopy2

    // find last folder name inside dir
    char *lastslash = strrchr(dir, '/');

    if (lastslash != NULL && *(lastslash + 1) != '\0') {
        snprintf(result, sizeof(result), "%s/%s", lastslash, base);
    } else {
        snprintf(result, sizeof(result), "/%s", base);
    }

    return result;
}

void load_song(char* song) {
    fp = freopen(song, "rb", stdin);
    if (fp == nullptr) {
        perror("Failed to open file");
        return;
    }
    
}

void play_song(int index) {
    fclose(fp);
    char next_song[512];
    snprintf(next_song, sizeof(next_song) ,"%s/%s",current_directory, playlist[index]);
    load_song(next_song);
    close_audio();
    init_audio();
}

void load_first_position(char *current_file) {
    for(s16 i = 0; i < max_list; i++) {
        // compare current loaded file with playplist
        if(strcmp(current_file, playlist[i]) == 0) {
            index_list = i;
            current_song = index_list;
            offset = i - 4;
            break;
        }
    }
}

s16 update_text_pos(char *filename, u16 index) {
    static s16 pos_x = 35;
    static u16 old_index = index;
#ifndef MIYOO
    static u8 frame = 0;
#endif
    // reset when index change
    if( old_index != index) {
        old_index = index;
        pos_x = 35;
#ifndef MIYOO
        frame = 0;
#endif
    }
    // check if legth text in pixel is long
    if(get_text_width(filename) >= 320 - 35) {
#ifndef MIYOO
        frame++;
        if(frame > 20) {
            pos_x -= 1;
            frame = 0;
        }
#else
        pos_x-=1;
#endif
        if(pos_x < 245 - get_text_width(filename))
            pos_x = 35;
    }
    return pos_x;
}

void get_list_file(char *open_directory, u16 *out_count, char **list) {
    struct dirent *directory_entry;

    DIR *directory = opendir(open_directory);

    if (directory == NULL)
    {
        return;
    }
    u16 count = 0;
    while ((directory_entry = readdir(directory)) != NULL) {
        if (strcmp(directory_entry->d_name, ".") == 0
        || strcmp(directory_entry->d_name, "..") == 0)
            continue;
        if(directory_entry->d_type == DT_REG) {
            list[count++] = strdup(directory_entry->d_name);
        }
    }
    closedir(directory);
    
    // sort alphabet
    qsort(list, count, sizeof(char *), [](const void *a, const void *b) {
        return strcmp(*(const char **)a, *(const char **)b);
    });

    *out_count = count;
    return;
}

void load_list_file() {
    u16 count_list = 0;
    get_list_file(current_directory, &count_list, playlist);
    max_list = count_list;
}
