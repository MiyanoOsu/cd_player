#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <alsa/asoundlib.h>
#include "decoder.h"
#include "common.h"

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>

// 4KB Read buffer (Streaming from disk)
#define DISK_BUFFER_SIZE 65536
// Refill buffer when less than 1KB remains
#define MIN_BUFFER_SIZE 1024


void init_audio();
void update_audio();
void close_audio();

#endif // _AUDIO_H_