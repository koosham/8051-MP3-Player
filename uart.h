#ifndef _UART_H
#define _UART_H

#include "main.h"
#include "port.h"

#define UART_SLOW 							0
#define UART_FAST 							1
#define UART_NORMAL_BAUDRATE 		0
#define UART_DOUBLE_BAUDRATE 		1

// ASCII characters
#define space 									0x20
#define CR											0x0D
#define LF											0x0A
#define BS											0x08
#define DEL											0x7F

// -------- Public function prototypes --------

void UART_Init(uint32_t baudrate, uint8_t baudrate_multiplier, uint8_t baudrate_speed);

#endif