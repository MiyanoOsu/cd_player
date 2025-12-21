#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "common.h"

typedef struct {
	u16 buttons[14];
    char font[256];
} t_config;

extern t_config option;

extern char home_path[256], config_path[256];
extern u8 done;

void init_config();
void save_config();
void load_config();

#endif // _CONFIG_H_