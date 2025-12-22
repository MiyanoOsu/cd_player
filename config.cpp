#include "config.h"
#include <SDL/SDL.h>

char home_path[256], config_path[256];
char *font_path = "DejaVuSans.ttf";

void init_config() {
    snprintf(home_path, sizeof(home_path), "%s/cd", getenv("HOME"));
    snprintf(config_path, sizeof(config_path), "cd.conf");
    load_config();
}

void save_config() {
	FILE* fp;
	fp = fopen(config_path, "wb");
	if (fp)
	{
		fwrite(&option, sizeof(option), sizeof(int8_t), fp);
		fclose(fp);
	}
}

void load_config() {
    FILE *fp;
    fp = fopen(config_path,"rb");
    if(fp) {
		fread(&option, sizeof(option), sizeof(int8_t), fp);
		fclose(fp);
    } else {
        option.buttons[0] = SDLK_UP;
        option.buttons[1] = SDLK_DOWN;
        option.buttons[2] = SDLK_LEFT;
        option.buttons[3] = SDLK_RIGHT;
        option.buttons[4] = SDLK_LALT;
        option.buttons[5] = SDLK_LCTRL;
        option.buttons[6] = SDLK_LSHIFT;
        option.buttons[7] = SDLK_SPACE;
        option.buttons[8] = SDLK_BACKSPACE;
        option.buttons[9] = SDLK_TAB;
        option.buttons[10] = SDLK_RETURN;
        option.buttons[11] = SDLK_ESCAPE;
        option.buttons[12] = SDLK_RCTRL;
        option.buttons[13] = SDLK_RALT;
        strcpy(option.font, font_path);
        option.font_size = 15;
        save_config();
    }
}
