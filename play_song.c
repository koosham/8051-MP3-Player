#include "main.h"
#include "port.h"
#include "spi.h"
#include "read_sector.h"
#include "Directory_Functions_struct.h"
#include <stdio.h>
#include "sdcard.h"
#include "play_song.h"

/***********************************************************************
DESC:  Play a song that starts from the given cluster
INPUT: Cluster number of the beginning of the song file
RETURNS: None
CAUTION: 
************************************************************************/
void Play_Song(uint32_t cluster)
{
	uint8_t return_val;
	uint8_t sector_index = 0;
	uint16_t byte_index1 = 0;
	uint16_t byte_index2 = 0;
	uint32_t first_sector;
	state_t state;
	
	first_sector = First_Sector(cluster);
	TRIGGER = 0; //For debugging
	
	LED_AMBER = 0;
	Read_Sector(first_sector + sector_index, 512, Buffer1_Pointer());
	LED_AMBER = 1;
	sector_index++;
	LED_AMBER = 0;
	Read_Sector(first_sector + sector_index, 512, Buffer2_Pointer());
	LED_AMBER = 1;
	sector_index++;
	state = IDLE1;
	
	while((SW1 == 1) && (state != STOP_PLAYING)) // Stop playing when SW1 is pressed
	{
		switch (state)
		{
			case IDLE1:
				if (STA013_DATA_REQ == 0)
				{
					state = DATA_SEND1;
				}
				break;
			case DATA_SEND1:
				STA013_EN = 1;
				LED_GREEN = 0;
				SPI_Transfer(*(Buffer1_Pointer() + byte_index1), &return_val);
				LED_GREEN = 1;
				STA013_EN = 0;
				byte_index1++;
				if ((byte_index2 > 511 && STA013_DATA_REQ == 1) || (byte_index1 > 511 && byte_index2 > 511))
				{
					state = LOAD_BUFFER2;
				}
				else if (byte_index1 > 511)
				{
					state = DATA_SEND2;
				}
				else if (STA013_DATA_REQ == 1)
				{
					state = IDLE1;
				}
				break;
			case LOAD_BUFFER1:
				if (sector_index >= Export_Drive_Values()->SecPerClus) //Last sector of the cluster
				{
					if (LED_YELLOW == 1)	LED_YELLOW = 0; else LED_YELLOW = 1;
					cluster = Find_Next_Clus(cluster, Buffer1_Pointer());
					first_sector = First_Sector(cluster);
					sector_index = 0;
					if (cluster == 0x0FFFFFFF) //Last cluster of the file
					{
						state = STOP_PLAYING;
						printf("\n###End of File###\n\n");
						break;
					}
				}
				LED_AMBER = 0;
				Read_Sector(first_sector + sector_index, 512, Buffer1_Pointer());
				LED_AMBER = 1;
				sector_index++;
				byte_index1 = 0;
				state = IDLE2;
				break;
			case IDLE2:
				if (STA013_DATA_REQ == 0)
				{
					state = DATA_SEND2;
				}
				break;
			case DATA_SEND2:
				STA013_EN = 1;
				LED_GREEN = 0;
				SPI_Transfer(*(Buffer2_Pointer() + byte_index2), &return_val);
				LED_GREEN = 1;
				STA013_EN = 0;
				byte_index2++;
				if ((byte_index1 > 511 && STA013_DATA_REQ == 1) || (byte_index1 > 511 && byte_index2 > 511))
				{
					state = LOAD_BUFFER1;
				}
				else if (byte_index2 > 511)
				{
					state = DATA_SEND1;
				}
				else if (STA013_DATA_REQ == 1)
				{
					state = IDLE2;
				}
				break;
			case LOAD_BUFFER2:
				if (sector_index >= Export_Drive_Values()->SecPerClus) //Last sector of the cluster
				{
					if (LED_YELLOW == 1)	LED_YELLOW = 0; else LED_YELLOW = 1;
					cluster = Find_Next_Clus(cluster, Buffer2_Pointer());
					first_sector = First_Sector(cluster);
					sector_index = 0;
					if (cluster == 0x0FFFFFFF) //Last cluster of the file
					{
						state = STOP_PLAYING;
						printf("\n###End of File###\n\n");
						break;
					}
				}
				LED_AMBER = 0;
				Read_Sector(first_sector + sector_index, 512, Buffer2_Pointer());
				LED_AMBER = 1;
				sector_index++;
				byte_index2 = 0;
				state = IDLE1;
				break;
		}
	}
	LED_YELLOW = 1;
}