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
s16 current_song = 0;
u8 mode = 0;

#ifdef MIYOO

#define MIYOO_LID_FILE  "/mnt/.backlight.conf"
#define MIYOO_LID_CONF  "/sys/class/backlight/backlight/brightness"

static int read_conf(const char *file) {
  int i, fd;
  int val = 5;
  char buf[10]={0};
  
  fd = open(file, O_RDWR);
  if(fd < 0){
    val = -1;
  }
  else{
    read(fd, buf, sizeof(buf));
    for(i=0; i<strlen(buf); i++){
      if(buf[i] == '\r'){
        buf[i] = 0;
      }
      if(buf[i] == '\n'){
        buf[i] = 0;
      }
      if(buf[i] == ' '){
        buf[i] = 0;
      }
    }
    val = atoi(buf);
  }
  close(fd);
  return val;
}

void screen_on() {
	int val;
	FILE *f;
  	char buf[3]={0};
	val = read_conf(MIYOO_LID_FILE);
	if (f = fopen(MIYOO_LID_CONF, "w")) {
    	sprintf(buf, "%d\n", val);
		fprintf(f, buf);
		fclose(f);
	}
}

void screen_off() {
	FILE *f;
	if ((f = fopen(MIYOO_LID_CONF, "w"))) {
    	fprintf(f, "0\n");
    	fclose(f);
  }
}

#endif

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
            if (i > max_list - 5) {
                offset = max_list - 9;
                break;
            }
            if (i < 4) break;
            offset = i - 4;
            break;
        }
    }
}

s16 update_text_pos(char *filename, u16 index) {
    static s16 pos_x = 35;
    static u16 old_index = index;
    // reset when index change
    if( old_index != index) {
        old_index = index;
        pos_x = 35;
    }
    // check if legth text in pixel is long
    if(get_text_width(filename) >= 320 - 35) {
        pos_x-=1;
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
        char *extension = strrchr(directory_entry->d_name, '.');
        if(directory_entry->d_type == DT_REG && extension != NULL && strcmp(extension, ".flac") == 0) {
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
