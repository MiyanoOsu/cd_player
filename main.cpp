#include "common.h"
#include "video.h"
#include "input.h"
#include "config.h"
#include "audio.h"
#include "font.h"
#include "cd.h"

u8 done = 0;

void init_playlist(char *current_file) {
    char fullpath1[256];
    char fullpath2[256];
    
    strncpy(fullpath1, current_file, sizeof(fullpath1));
    strncpy(fullpath2, current_file, sizeof(fullpath2));
    
    current_directory = dirname(fullpath1);
    load_list_file();
    load_first_position(basename(fullpath2));
}


int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (std::freopen(argv[1], "rb", stdin) == nullptr) {
            std::perror("Failed to open file");
            return 1;
        }
    } else {
        std::cout << "Usage: " << argv[0] << " <flac_file>" << std::endl;
        return 1;
    }

    init_config();
    init_font();
    init_playlist(argv[1]);
    init_video();
    init_audio();

    while (!done) {
        handle_input();
        update_video();
        update_audio();
    }
    
    close_audio();
    close_video();
    close_font();
    return 0;
}