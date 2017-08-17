#include "main.h"
#include "port.h"
#include "delay.h"
#include "lcd.h"

#define NULL	0

/***********************************************************************
DESC:  Write a byte to LCD module
INPUT: RS (register select) and byte to be written
RETURNS: None
CAUTION: 
************************************************************************/
void LCD_Write(bit reg_select, uint8_t LCD_data)
{
	LCD_RS_PIN = reg_select;
	LCD_E_PIN = 1;
	LCD_MODULE_DATA_PORT = LCD_data;
	LCD_E_PIN = 0;
	LCD_MODULE_DATA_PORT = 0xFF;
	LCD_RS_PIN = 1;
}

/***********************************************************************
DESC:  Initialize LCD module
INPUT: None
RETURNS: None
CAUTION: 
************************************************************************/
void LCD_Init(void)
{
	delay_ms(30);
	LCD_Write(LCD_CONTROL,LCD_FUNC_SET_BYTE);
	delay_us(4100);
	LCD_Write(LCD_CONTROL,LCD_FUNC_SET_BYTE);
	delay_us(100);
	LCD_Write(LCD_CONTROL,LCD_FUNC_SET_BYTE);
	delay_us(39);
	LCD_Write(LCD_CONTROL,LCD_DISPLAY_CONTROL_BYTE);
	delay_us(39);
	LCD_Write(LCD_CONTROL,LCD_CLEAR_BYTE);
	delay_us(1530);
	LCD_Write(LCD_CONTROL,LCD_ENTRY_MODE_SET_BYTE);
	delay_us(39);
}

/***********************************************************************
DESC:  Prints strings on LCD module
INPUT: number of bytes (if zero, then everything in the input array up to NULL character will be sent.) and input string
RETURNS: None
CAUTION: 
************************************************************************/
void LCD_Print(uint8_t num_bytes, uint8_t * input_array)
{
	uint8_t value, index;
	if (num_bytes == 0)
	{
		value = *input_array;
		index = 0;
		while (value != NULL)
		{
			LCD_Write(LCD_DDRAM,value);
			index++;
			value = *(input_array + index);
			delay_us(43);
		}
	}
	else
		for (index = 0 ; index < num_bytes ; index++)
	{
		value = *(input_array + index);
		LCD_Write(LCD_DDRAM,value);
		delay_us(43);
	}
}