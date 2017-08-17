#include "main.h"
#include "port.h"
#include "delay.h"
#include <stdio.h>
#include "uart.h"
//#include "print_bytes.h"
#include "lcd.h"
#include "spi.h"
#include "sdcard.h"
#include "Long_Serial_In.h"
#include "read_sector.h"
#include "Directory_Functions_struct.h"
#include "i2c.h"
#include "sta013.h"
#include "play_song.h"

#define BAUDRATE								9600
#define BAUDRATE_MULTIPLIER			UART_NORMAL_BAUDRATE
#define BAUDRATE_SPEED					UART_FAST

//Global variables declaration

void main(void)
{
	//Local variables declaration
	uint8_t LCD_string[7];
	uint16_t error_status = NO_ERROR;
	uint16_t number_of_entries;
	uint32_t input, cluster, last_cluster = 0;
	
	STA013_EN = 0;
	AUXR = 0x0C;   // make all of XRAM available, ALE always on
	
	LED_RED = 0;
	LED_YELLOW = 0;
	LED_AMBER = 0;
	LED_GREEN = 0;
	delay_ms(1000);
	LED_RED = 1;
	LED_YELLOW = 1;
	LED_AMBER = 1;
	LED_GREEN = 1;
	
	LCD_Init();
	
	UART_Init(BAUDRATE, BAUDRATE_MULTIPLIER, BAUDRATE_SPEED);
	
	error_status = SPI_Master_Init(200000);
	delay_ms(1);
	error_status |= SD_Card_Init();
	error_status |= SPI_Master_Init(FREQ_PER / 2);
	error_status |= Mount_Drive(Buffer1_Pointer());
	error_status |= STA013_Test();
	error_status |= STA013_Config();
	
	if (error_status != NO_ERROR)
	{
		LED_RED = 0;
	}
	
	sprintf(LCD_string, "0x%04X", error_status);
	LCD_Print(0, LCD_string);
	printf("\n\r");
	
	while(1)
	{
		number_of_entries = Print_Directory(First_Sector(cluster), Buffer1_Pointer());
		last_cluster = cluster;
		printf("\nEnter entry number: ");
		input = long_serial_input();
		while ((input == 0) || (input > number_of_entries))
		{
			printf("Invalid entry number!\n");
			printf("Enter entry number: ");
			input = long_serial_input();
		}
		cluster = Read_Dir_Entry(First_Sector(cluster), (uint16_t)input, Buffer1_Pointer());
		if ((cluster & 0x10000000) == 0x10000000) //Directory
		{
			cluster &= 0x0FFFFFFF;
		}
		else //File
		{
			cluster &= 0x0FFFFFFF;
			Play_Song(cluster);
			cluster = last_cluster;
		}
	}
}