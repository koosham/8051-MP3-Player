#ifndef _MAIN_H
#define _MAIN_H

#include "AT89C51RC2.h"
#define OSC_FREQ 										(18432000UL)
#define OSC_PER_INST 								(6)
#define FREQ_PER 										((OSC_FREQ * 6) / OSC_PER_INST)

typedef unsigned char		uint8_t;
typedef unsigned int		uint16_t;
typedef unsigned long		uint32_t;
typedef signed char			int8_t;
typedef signed int 			int16_t;
typedef signed long 		int32_t;

#endif
