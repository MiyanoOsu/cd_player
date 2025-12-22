#include "common.h"
#include "cd.h"
#include "font.h"
#include <dirent.h>

u16 max_list = 0;
char *playlist[1024] = {NULL};
char *current_directory = NULL;

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

void get_list_file(char * open_directory, u16 *out_count, char **list) {
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
