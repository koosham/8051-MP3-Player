#include "main.h"
#include "port.h"
#include "uart.h"
#include <stdio.h>

/***********************************************************************
DESC:  Initialize UART
INPUT: Baurdate, baudrate_multiplier which can be either DOUBLE_BAUDRATE or NORMAL_BAUDRATE, and baudrate_speed which can be either FAST or SLOW
RETURNS: None
CAUTION: 
************************************************************************/

void UART_Init(uint32_t baudrate, uint8_t baudrate_multiplier, uint8_t baudrate_speed)
{
	// clear SMOD0
	PCON &= ~(1 << 6);
	
	// configure PCON for normal or double baudrate
	if (baudrate_multiplier == UART_DOUBLE_BAUDRATE)
	{
		PCON |= 1 << 7;
	}
	else if (baudrate_multiplier == UART_NORMAL_BAUDRATE)
	{
		PCON &= ~(1 << 7);
	}
	
	// 8-bit with variable baudrate
	// disable 9th bit
	// receive enable
	SCON = 0x50;
	
	// disable baudrate generator
	BDRCON = 0x00;
	
	// set baudrate generator value
	BRL = (uint8_t)(256 - (((uint32_t)(1 + 5 * baudrate_speed) * (1 + 1 * baudrate_multiplier) * OSC_FREQ) / (32 * OSC_PER_INST * baudrate)));
	
	// activate baudrate generator for Rx and Tx and start
	BDRCON = 0x1C;
	
	// configure BDRCON for slow or fast mode
	if (baudrate_speed == UART_FAST)
	{
		BDRCON |= 1 << 1;
	}
	else if (baudrate_speed == UART_SLOW)
	{
		BDRCON &= ~(1 << 1);
	}
	
	// enable transmitting
	TI = 1;
	
	// disable serial interrupt
	ES = 0;
}