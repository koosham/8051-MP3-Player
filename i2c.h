#ifndef _I2C_H
#define _I2C_H

#include "main.h"
#include "port.h"

#ifndef NO_ERROR
#define NO_ERROR										0x0000
#endif

#define I2C_BUS_BUSY_ERROR					0x2000
#define I2C_NACK_ERROR							0x4000

#define START												0
#define CONTINUE										1
#define STOP												2

#define I2C_RELOAD_H								0xFF
#define I2C_RELOAD_L								0xC0

// -------- Public function prototypes --------

void I2C_Clock(uint8_t control);
uint16_t I2C_Write(uint8_t device_addr, uint8_t num_bytes, uint8_t * array);
uint16_t I2C_Read(uint8_t device_addr, uint8_t num_bytes, uint8_t * array);

#endif