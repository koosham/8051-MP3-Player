#include "main.h"
#include "port.h"
#include "spi.h"
#include "sdcard.h"

static uint8_t idata SD_Card_Type = SD_TYPE2;
static uint8_t idata SD_Card_Capacity = SD_CAPACITY_UNKNOWN;
static uint8_t xdata Buffer1[512], Buffer2[512];

/***********************************************************************
DESC:  Get the pointer of SD_Card_Type
INPUT: None
RETURNS: A pointer that gives the address of the global variable that holds the type of SD card
CAUTION: 
************************************************************************/

uint8_t *SD_Card_Type_Pointer(void)
{
	return &SD_Card_Type;
}

/***********************************************************************
DESC:  Get the pointer of SD_Card_Capacity
INPUT: None
RETURNS: A pointer that gives the address of the global variable that holds the capacity type (standard and high) of SD card
CAUTION: 
************************************************************************/
uint8_t *SD_Card_Capacity_Pointer(void)
{
	return &SD_Card_Capacity;
}

/***********************************************************************
DESC:  Get the pointer of Buffer1
INPUT: None
RETURNS: A pointer that gives the address of the global variable that holds Buffer1
CAUTION: 
************************************************************************/

uint8_t * Buffer1_Pointer(void)
{
	return &Buffer1;
}

/***********************************************************************
DESC:  Get the pointer of Buffer2
INPUT: None
RETURNS: A pointer that gives the address of the global variable that holds Buffer2
CAUTION: 
************************************************************************/

uint8_t * Buffer2_Pointer(void)
{
	return &Buffer2;
}

/***********************************************************************
DESC:  Send command to SD card
INPUT: Command number (0 to 63) and argument
RETURNS: Error flag that is equal to NO_ERROR if no error occurs
CAUTION: 
************************************************************************/

uint16_t SD_Send_Command(uint8_t command, uint32_t argument)
{
	uint16_t error_flag = NO_ERROR;
	uint8_t send_val, return_val;
	uint8_t index;
	
	if (command < 64)
	{
		send_val = 0x40 | command;
		error_flag = SPI_Transfer(send_val, &return_val);
		index = 24;
		while (error_flag == NO_ERROR && index < 25)
		{
			send_val = (uint8_t)(argument >> index);
			error_flag = SPI_Transfer(send_val, &return_val);
			index -= 8;
		}
		//CRC7
		if (command == 0)
		{
			send_val = 0x95;
		}
		else if (command == 8)
		{
			send_val = 0x87;
		}
		else
		{
			send_val = 0x01;
		}
		if (error_flag == NO_ERROR)
		{
			error_flag = SPI_Transfer(send_val, &return_val);
		}
	}
	else
	{
		error_flag |= SD_ILLEGAL_CMD_ERROR;
	}
	return error_flag;
}

/***********************************************************************
DESC:  Receive response of SD card
INPUT: Number of bytes to expect and a pointer to the array that will hold received response
RETURNS: Error flag that is equal to NO_ERROR if no error occurs
CAUTION: 
************************************************************************/

uint16_t SD_Receive_Response(uint8_t number_of_bytes, uint8_t *received_array)
{
	uint16_t error_flag = NO_ERROR;
	uint8_t received_val;
	uint8_t index, timeout = 0;
	
	do
	{
		error_flag = SPI_Transfer(0xFF, &received_val);
		timeout++;
	} while (timeout != 0 && error_flag == NO_ERROR && received_val == 0xFF);
	*received_array = received_val;
	if (timeout == 0)
	{
		error_flag |= SD_TIMEOUT_ERROR;
	}
	else if (received_val != 0x00 && received_val != 0x01)
	{
		
		error_flag |= SD_CARD_ERROR;
	}
	else
	{
		for (index = 1 ; index < number_of_bytes ; index++)
		{
			SPI_Transfer(0xFF, &received_val);
			received_array[index] = received_val;
		}
	}
	SPI_Transfer(0xFF, &received_val);
	return error_flag;
}

/***********************************************************************
DESC:  Initialize SD card
INPUT: None
RETURNS: Error flag that is equal to NO_ERROR if no error occurs
CAUTION: 
************************************************************************/

uint16_t SD_Card_Init(void)
{
	uint16_t error_flag = NO_ERROR;
	uint16_t error_status = NO_ERROR;
	uint32_t ACMD41_argument = 0x40000000;
	uint8_t return_vals[5];
	uint8_t index, timeout;
	
	nCS = 1;
	for (index = 0 ; index < 10 ; index++)
	{
		SPI_Transfer(0xFF,return_vals);
	}
	
	nCS = 0;
	error_flag = SD_Send_Command(0,0x00);
	if (error_flag == NO_ERROR)
	{
		error_flag = SD_Receive_Response(1,return_vals);
	}
	if (error_flag != NO_ERROR)
	{
		error_status |= error_flag;
	}
	nCS = 1;
	if (error_status == NO_ERROR)
	{
		nCS = 0;
		error_flag = SD_Send_Command(8,0x000001AA);
		if (error_flag == NO_ERROR)
		{
			error_flag = SD_Receive_Response(5,return_vals);
		}
		if (error_flag != NO_ERROR)
		{
			error_status |= error_flag;
		}
		if (return_vals[0] == 0x05) // Old Card (not supported currently)
		{
			SD_Card_Type = SD_TYPE1;
			error_status |= SD_INCOMPATIBLE_CARD_ERROR;
			//error_status == NO_ERROR;
			//ACMD41_argument = 0;
		}
		if (return_vals[3] != 0x01)
		{
			error_status |= SD_VOLTAGE_ERROR;
		}
		if (return_vals[4] != 0xAA)
		{
			error_status |= SD_INVALID_RESPONSE_ERROR;
		}
		nCS = 1;
	}
	
	if (error_status == NO_ERROR)
	{
		nCS = 0;
		error_flag = SD_Send_Command(58,0x00);
		if (error_flag == NO_ERROR)
		{
			error_flag = SD_Receive_Response(5,return_vals);
		}
		if (error_flag != NO_ERROR)
		{
			error_status |= error_flag;
		}
		nCS = 1;
	}
	
	if (error_status == NO_ERROR)
	{
		nCS = 0;
		timeout = 0;
		do
		{
			error_flag = SD_Send_Command(55,0x00);
			if (error_flag == NO_ERROR)
			{
				error_flag = SD_Receive_Response(1,return_vals);
			}
			if (error_flag != NO_ERROR)
			{
				error_status |= error_flag;
			}
			if (error_flag == NO_ERROR)
			{
				error_flag = SD_Send_Command(41,ACMD41_argument);
			}
			if (error_flag == NO_ERROR)
			{
				error_flag = SD_Receive_Response(1,return_vals);
			}
			if (error_flag != NO_ERROR)
			{
				error_status |= error_flag;
			}
			timeout++;
		} while (return_vals[0] != 0x00 && timeout != 0 && error_status == NO_ERROR);
		nCS = 1;
		if (timeout == 0)
		{
			error_status |= SD_TIMEOUT_ERROR;
		}
	}
	
	if (SD_Card_Type == SD_TYPE2 && error_status == NO_ERROR)
	{
		nCS = 0;
		timeout = 0;
		do
		{
			error_flag = SD_Send_Command(58,0x00);
			if (error_flag == NO_ERROR)
			{
				error_flag = SD_Receive_Response(5,return_vals);
			}
			if (error_flag != NO_ERROR)
			{
				error_status |= error_flag;
			}
			timeout++;
		} while ((return_vals[1] & 0x80) != 0x80 && timeout != 0 && error_status == NO_ERROR);
		nCS = 1;
		if (timeout == 0)
		{
			error_status |= SD_POWERUP_ERROR;
		}
		if (error_status == NO_ERROR && (return_vals[1] & 0x40) == 0x40)
		{
			SD_Card_Capacity = HCSD;
		}
		else if (error_status == NO_ERROR && (return_vals[1] & 0x40) == 0x00)
		{
			SD_Card_Capacity = SCSD;
		}
	}
	
	if (SD_Card_Capacity == SCSD && error_status == NO_ERROR)
	{
		nCS = 0;
		error_flag = SD_Send_Command(16,0x200);
		if (error_flag == NO_ERROR)
		{
			error_flag = SD_Receive_Response(1,return_vals);
		}
		if (error_flag != NO_ERROR)
		{
			error_status |= error_flag;
		}
		nCS = 1;
	}
	return error_status;
}

/***********************************************************************
DESC:			Read a block of data from SD card
INPUT:		Number of bytes to read and a pointer to the variable that will hold the read data
RETURNS:	Error flag that is equal to NO_ERROR if no error occurs
CAUTION:	CMD17 along with block address should be sent to SD card before this function is called.
					nCS must be set/cleared outside of this function.
************************************************************************/
uint16_t SD_Read_Block(uint16_t number_of_bytes, uint8_t xdata * block)
{
	uint16_t error_flag = NO_ERROR;
	uint16_t error_status = NO_ERROR;
	uint8_t *return_val;
	uint8_t timeout = 0;
	uint16_t index = 0;
	//uint16_t checksum;
	
	do
	{
		error_flag = SPI_Transfer(0xFF, return_val);
		if (error_flag != NO_ERROR)
		{
			error_status |= error_flag;
		}
		timeout++;
	} while ((*return_val & 0x80) == 0x80 && timeout != 0 && error_status == NO_ERROR);
	if (timeout == 0)
	{
		error_status |= SD_TIMEOUT_ERROR;
	}
	if (error_status == NO_ERROR)
	{
		do
		{
			error_flag = SPI_Transfer(0xFF, return_val);
			if (error_flag != NO_ERROR)
			{
				error_status |= error_flag;
			}
			if ((*return_val & 0xF0) == 0x00) //Data error token
			{
				error_status |= SD_DATA_ERROR;
			}
			timeout++;
		} while (*return_val != 0xFE && timeout != 0 && error_status == NO_ERROR);
		if (timeout == 0)
		{
			error_status |= SD_TIMEOUT_ERROR;
		}
	}
	if (error_status == NO_ERROR)
	{
		while (index < number_of_bytes && error_status == NO_ERROR)
		{
			error_flag = SPI_Transfer(0xFF, return_val);
			error_status |= error_flag;
			block[index] = *return_val;
			index++;
		}
	}
	if (error_status == NO_ERROR)
	{
		SPI_Transfer(0xFF, return_val);
		//checksum = *return_val;
		SPI_Transfer(0xFF, return_val);
		//checksum <<= 8;
		//checksum |= *return_val;
		SPI_Transfer(0xFF, return_val);
	}
	return error_status;
}
