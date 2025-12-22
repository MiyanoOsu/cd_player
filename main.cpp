#include "common.h"
#include "video.h"
#include "input.h"
#include "config.h"
#include "audio.h"
#include "font.h"
#include "cd.h"

u8 done = 0;

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

    current_directory = dirname(argv[1]);
    load_list_file();
    init_config();
    init_font();
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