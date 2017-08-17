#ifndef _STA013_H
#define _STA013_H

#include "main.h"
#include "port.h"

#ifndef NO_ERROR
#define NO_ERROR										0x0000
#endif

#define STA013_ERROR								0x8000

#define STA013_ADDR									0x43

// -------- Public function prototypes --------
uint16_t STA013_Test(void);
uint16_t STA013_Config(void);

#endif