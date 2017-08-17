#include "main.h"
#include "port.h"
#include "delay.h"
#include "i2c.h"
#include "spi.h"
#include "sta013.h"

extern uint8_t code CONFIG;
extern uint8_t code CONFIG2;
extern uint8_t code CONFIG3;

/***********************************************************************
DESC:  Verify that STA013 MP3 decoder is working
INPUT: None
RETURNS: Error flag that is equal to NO_ERROR if no error occurs
CAUTION: 
************************************************************************/
uint16_t STA013_Test(void)
{
	uint16_t error_flag = NO_ERROR;
	uint8_t timeout = 0;
	uint8_t array[2];
	
	STA013_RST = 0;
	delay_ms(10);
	array[0] = 1;
	STA013_RST = 1;
	//TRIGGER = 0; //For debugging
	do
	{
		error_flag = I2C_Write(STA013_ADDR,1,array);
		timeout++;
	} while (error_flag != NO_ERROR && timeout!= 0);
	timeout = 0;
	if (error_flag == NO_ERROR)
	{
		do
		{
			error_flag = I2C_Read(STA013_ADDR,1,array);
			timeout++;
		} while (error_flag != NO_ERROR && timeout!= 0);
	}
	if (error_flag == NO_ERROR)
	{
		if (array[0] != 0xAC)
		{
			error_flag = STA013_ERROR;
		}
	}
	return error_flag;
}

/***********************************************************************
DESC:  Send patch file of STA013 (config.asm)
INPUT: None
RETURNS: Error flag that is equal to NO_ERROR if no error occurs
CAUTION: 
************************************************************************/
uint16_t STA013_Config(void)
{
	uint8_t * config_p;
	uint8_t send_array[3];
	uint16_t index, error_flag = NO_ERROR;
	
	config_p = &CONFIG;
	index = 0;
	
	do
	{
		if (LED_AMBER == 0) LED_AMBER = 1;
		else LED_AMBER = 0;
		send_array[0] = *(config_p + index);
		index++;
		send_array[1] = *(config_p + index);
		index++;
		if (send_array[0] != 0xFF)
		{
			error_flag = I2C_Write(STA013_ADDR,2,send_array);
		}
		delay_ms(1);
	} while(send_array[0] != 0xFF && error_flag == NO_ERROR);
	
	delay_ms(100);
	config_p = &CONFIG2;
	index = 0;
	do
	{
		if (LED_AMBER == 0) LED_AMBER = 1;
		else LED_AMBER = 0;
		send_array[0] = *(config_p + index);
		index++;
		send_array[1] = *(config_p + index);
		index++;
		if (send_array[0] != 0xFF)
		{
			error_flag = I2C_Write(STA013_ADDR,2,send_array);
		}
		delay_ms(1);
	} while(send_array[0] != 0xFF && error_flag == NO_ERROR);
	
	delay_ms(100);
	config_p = &CONFIG3;
	index = 0;
	do
	{
		if (LED_AMBER == 0) LED_AMBER = 1;
		else LED_AMBER = 0;
		send_array[0] = *(config_p + index);
		index++;
		send_array[1] = *(config_p + index);
		index++;
		if (send_array[0] != 0xFF)
		{
			error_flag = I2C_Write(STA013_ADDR,2,send_array);
		}
		delay_ms(1);
	} while(send_array[0] != 0xFF && error_flag == NO_ERROR);
	
	LED_AMBER = 1;
	return error_flag;
}