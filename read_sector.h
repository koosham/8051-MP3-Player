#ifndef _READ_SECTOR_H
#define _READ_SECTOR_H

#include "main.h"

// -------- Public function prototypes --------

uint16_t Read_Sector(uint32_t sector_number, uint16_t sector_size, uint8_t xdata * data_array);

#endif