#include "main.h"
#include "port.h"
#include "spi.h"

/***********************************************************************
DESC:  Initialize SPI
INPUT: Clock frequency (SCLK) for initialization (should be less than SPI_MAX_CLK)
RETURNS: Error flag that is equal to NO_ERROR if no error occurs
CAUTION: 
************************************************************************/

uint16_t SPI_Master_Init(uint32_t clock_freq)
{
	uint8_t n;
	uint8_t SPR2, SPR1, SPR0;
	uint16_t error_flag = NO_ERROR;
	
	if (clock_freq > SPI_MAX_CLK)
	{
		error_flag = SPI_FREQ_ERROR;
	}
	if (error_flag == NO_ERROR)
	{
		if (FREQ_PER / clock_freq <= 2)
		{
			n = 0;
		}
		else if (FREQ_PER / clock_freq <= 4)
		{
			n = 1;
		}
		else if (FREQ_PER / clock_freq <= 8)
		{
			n = 2;
		}
		else if (FREQ_PER / clock_freq <= 16)
		{
			n = 3;
		}
		else if (FREQ_PER / clock_freq <= 32)
		{
			n = 4;
		}
		else if (FREQ_PER / clock_freq <= 64)
		{
			n = 5;
		}
		else
		{
			n = 6;
		}
		
		SPR2 = (n >> 2) & 0x01;
		SPR1 = (n >> 1) & 0x01;
		SPR0 = (n >> 0) & 0x01;
		
		// configure SPCON:
		//-----------------------------//
		// SPEN = 1
		// clear SPR2, SPR1, and SPR0
		// SSDIS = 1
		// MSTR = 1 (set to master mode)
		// CPOL = 0
		// CPHA = 0
		SPCON = 0x70;
		
		// set SPR2, SPR1, and SPR0 according to the clock frequency
		SPCON |= (SPR2 << 7) | (SPR1 << 1) | (SPR0 << 0);
	}
	return error_flag;
}

/***********************************************************************
DESC:  Send and receive data over SPI
INPUT: Value to send and pointer to the variable that will hold received data
RETURNS: Error flag that is equal to NO_ERROR if no error occurs
CAUTION: 
************************************************************************/

uint16_t SPI_Transfer(uint8_t send_value, uint8_t *received_value)
{
	uint8_t test, timeout = 0;
	uint16_t error_flag = NO_ERROR;
	SPDAT = send_value;
	
	do
	{
		test = SPSTA;
		timeout++;
	} while ((test & 0xF0) == 0x00 && timeout != 0);
	if (timeout != 0)
	{
		*received_value = SPDAT;
		error_flag = (test & 0x70) >> 3;
		if (error_flag != NO_ERROR)
		{
			*received_value = 0xFF;
		}
	}
	else
	{
		error_flag = SPI_TIMEOUT_ERROR;
		*received_value = 0xFF;
	}
	return error_flag;
}