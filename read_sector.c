#include "main.h"
#include "port.h"
#include "sdcard.h"
#include "read_sector.h"


/***********************************************************************
DESC:  Read a sector of data from data storage medium
INPUT: Sector number, number of bytes per sector, and a pointer to the variable that will hold the read data
RETURNS: Error flag that is equal to NO_ERROR if no error occurs
CAUTION: 
************************************************************************/
uint16_t Read_Sector(uint32_t sector_number, uint16_t sector_size, uint8_t xdata * data_array)
{
	uint16_t error_flag = NO_ERROR;
	uint32_t block;
	
	nCS = 0;
	if (*SD_Card_Type_Pointer() == SD_TYPE2 && *SD_Card_Capacity_Pointer() == SCSD)
	{
		block = sector_number << 9;
	}
	else
	{
		block = sector_number;
	}
	error_flag = SD_Send_Command(17,block);
	if (error_flag == NO_ERROR)
	{
		error_flag = SD_Read_Block(sector_size, data_array);
	}
	nCS = 1;
	return error_flag;
}
