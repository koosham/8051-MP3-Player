#include "main.h"
#include "delay.h"

/***********************************************************************
DESC: Delays the program for some milliseconds
INPUT: delay in milliseconds
RETURNS: None
CAUTION: 
************************************************************************/

void delay_ms(const uint16_t delay_in_ms)
{
	uint16_t ms_index, cycle_index;
	const uint16_t inst_per_ms = (uint32_t)OSC_FREQ / ((uint32_t)12 * OSC_PER_INST * 1000);
	for (ms_index = 0 ; ms_index < delay_in_ms ; ms_index++)
	{
		for (cycle_index = 0 ; cycle_index < inst_per_ms ; cycle_index++);
	}
}

/***********************************************************************
DESC: Delays the program for some microseconds
INPUT: delay in microseconds
RETURNS: None
CAUTION: 
************************************************************************/

void delay_us(const uint16_t delay_in_us)
{
	uint16_t index, num_inst;
	const uint8_t us_per_inst = 8;
	//us_per_inst = (uint32_t)((uint32_t)12 * OSC_PER_INST * 10000000) / OSC_FREQ;
	num_inst = delay_in_us / us_per_inst;
	for (index = 0 ; index < num_inst ; index++);
}