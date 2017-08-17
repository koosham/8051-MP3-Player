#include "main.h"
#include "port.h"
#include "i2c.h"

/***********************************************************************
DESC:  Depending on the control input, start, wait, or stop Timer 1
INPUT: Control byte that determines the mode of operation. If START, the function initializes Timer 1
			 with a preset reload value. If CONTINUE, the function will halt the code until Timer 1 overflows
			 and reload it. If STOP, the function will halt the code until Timer 1 overflows.
RETURNS: None
CAUTION: 
************************************************************************/

void I2C_Clock(uint8_t control)
{
	if (control == START)
	{
		//Time1 set to mode 1
		TMOD &= 0x0F;
		TMOD |= 0x10;
		ET1 = 0; //No interrupt
		TH1 = I2C_RELOAD_H;
		TL1 = I2C_RELOAD_L;
		TF1 = 0; //Clear overflow flag
		TR1 = 1; //Start Timer 1
	}
	else if (control == CONTINUE || control == STOP)
	{
		if (TR1 == 1)
		{
			while(TF1 == 0);
		}
		TR1 = 0; //Stop Timer 1
		if (control == CONTINUE)
		{
			TH1 = I2C_RELOAD_H;
			TL1 = I2C_RELOAD_L;
			TF1 = 0; //Clear overflow flag
			TR1 = 1; //Start Timer 1
		}
	}
}

/***********************************************************************
DESC:  Write a number of bytes to I2C port
INPUT: Address of the device to which data is being sent, number of bytes to send, and a
			 pointer to the array that stores the data.
RETURNS: Error flag that is equal to NO_ERROR if no error occurs
CAUTION: 
************************************************************************/

uint16_t I2C_Write(uint8_t device_addr, uint8_t num_bytes, uint8_t * array)
{
	bit send_bit, sent_bit;
	uint8_t send_val, num_bits, index = 0;
	uint16_t error_flag = NO_ERROR;
	
	SDA = 1;
	SCL = 1;
	if (SCL == 1 && SDA == 1)
	{
		I2C_Clock(START);
		SDA = 0; // Start Condition
		send_val = (device_addr << 1) & 0xFE;
		num_bytes++;
		do
		{
			num_bits = 8;
			do
			{
				I2C_Clock(CONTINUE);
				SCL = 0;
				num_bits--;
				send_bit = (bit)((send_val >> num_bits) & 0x01);
				SDA = send_bit;
				I2C_Clock(CONTINUE);
				SCL = 1;
				while(SCL != 1);
				sent_bit = SDA;
				if (sent_bit != send_bit)
				{
					error_flag = I2C_BUS_BUSY_ERROR;
				}
			} while(num_bits > 0 && error_flag == NO_ERROR);
			if (error_flag == NO_ERROR)
			{
				I2C_Clock(CONTINUE);
				SCL = 0;
				SDA = 1;
				I2C_Clock(CONTINUE);
				SCL = 1;
				while(SCL != 1);
				sent_bit = SDA;
				if (sent_bit != 0)
				{
					error_flag = I2C_NACK_ERROR;
				}
				send_val = array[index];
				index++;
				num_bytes--;
			}
		} while((num_bytes > 0) && (error_flag == NO_ERROR));
		if (error_flag != I2C_BUS_BUSY_ERROR)
		{
			I2C_Clock(CONTINUE);
			SCL = 0;
			SDA = 0;
			I2C_Clock(CONTINUE);
			SCL = 1;
			while(SCL != 1);
			I2C_Clock(CONTINUE);
			SDA = 1; // Stop condition
		}
	}
	else
	{
		error_flag = I2C_BUS_BUSY_ERROR;
	}
	return error_flag;
}

/***********************************************************************
DESC:  Read a number of bytes from I2C port
INPUT: Address of the device from which data is being read, number of bytes to read, and a
			 pointer to the array that will store the received data.
RETURNS: Error flag that is equal to NO_ERROR if no error occurs
CAUTION: 
************************************************************************/

uint16_t I2C_Read(uint8_t device_addr, uint8_t num_bytes, uint8_t * array)
{
	bit send_bit, sent_bit, receive_bit;
	uint8_t send_val, num_bits, receive_val, index = 0;
	uint16_t error_flag = NO_ERROR;
	
	SDA = 1;
	SCL = 1;
	if (SCL == 1 && SDA == 1)
	{
		I2C_Clock(START);
		SDA = 0; // Start Condition
		send_val = (device_addr << 1) | 0x01;
		num_bits = 8;
		do
		{
			I2C_Clock(CONTINUE);
			SCL = 0;
			num_bits--;
			send_bit = (bit)((send_val >> num_bits) & 0x01);
			SDA = send_bit;
			I2C_Clock(CONTINUE);
			SCL = 1;
			while(SCL != 1);
			sent_bit = SDA;
			if (sent_bit != send_bit)
			{
				error_flag = I2C_BUS_BUSY_ERROR;
			}
		} while(num_bits > 0 && error_flag == NO_ERROR);
		if (error_flag == NO_ERROR)
			{
				I2C_Clock(CONTINUE);
				SCL = 0;
				SDA = 1;
				I2C_Clock(CONTINUE);
				SCL = 1;
				while(SCL != 1);
				sent_bit = SDA;
				if (sent_bit != 0)
				{
					error_flag = I2C_NACK_ERROR;
				}
			}
		if (error_flag == NO_ERROR)
		{
			do
			{
				receive_val = 0;
				num_bits = 8;
				do
				{
					I2C_Clock(CONTINUE);
					SCL = 0;
					SDA = 1;
					I2C_Clock(CONTINUE);
					SCL = 1;
					while(SCL != 1);
					receive_bit = SDA;
					receive_val = (receive_val << 1) | receive_bit;
					num_bits--;
				} while(num_bits > 0);
				I2C_Clock(CONTINUE);
				SCL = 0;
				if (num_bytes == 1)
				{
					SDA = 1; //NACK
				}
				else
				{
					SDA = 0; //ACK
				}
				I2C_Clock(CONTINUE);
				SCL = 1;
				while(SCL != 1);
				array[index] = receive_val;
				index++;
				num_bytes--;
				
			} while((num_bytes > 0) && (error_flag == NO_ERROR));
		}
		if (error_flag != I2C_BUS_BUSY_ERROR)
		{
			I2C_Clock(CONTINUE);
			SCL = 0;
			SDA = 0;
			I2C_Clock(CONTINUE);
			SCL = 1;
			while(SCL != 1);
			I2C_Clock(CONTINUE);
			SDA = 1; // Stop condition
		}
	}
	else
	{
		error_flag = I2C_BUS_BUSY_ERROR;
	}
	return error_flag;
}