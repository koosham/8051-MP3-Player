#ifndef _PLAY_SONG_H
#define _PLAY_SONG_H

#include "main.h"
#include "port.h"

typedef enum {IDLE1, LOAD_BUFFER1, DATA_SEND1, FIND_CLUSTER1, IDLE2, LOAD_BUFFER2, DATA_SEND2, FIND_CLUSTER2, STOP_PLAYING}
						 state_t;

// -------- Public function prototypes --------
void Play_Song(uint32_t cluster);

#endif