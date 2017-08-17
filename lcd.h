#ifndef _LCD_H
#define _LCD_H

#include "main.h"
#include "port.h"
#include "delay.h"

#define LCD_CONTROL									0
#define LCD_DDRAM										1
#define LCD_MODE										1 // 8-bit
#define LCD_NUMBER_OF_LINES					1 // 2-line
#define LCD_FONT										0 // 5x8
#define LCD_DISPLAY									1 // ON
#define LCD_CURSOR									0 // OFF
#define LCD_CURSOR_BLINK						0 // OFF
#define LCD_DDRAM_ADDR							1 // incremental
#define LCD_SHIFT										0 // shift cursor
#define LCD_FUNC_SET_BYTE						0x20 | LCD_MODE << 4 | LCD_NUMBER_OF_LINES << 3 | LCD_FONT << 2
#define LCD_DISPLAY_CONTROL_BYTE		0x08 | LCD_DISPLAY << 2 | LCD_CURSOR << 1 | LCD_CURSOR_BLINK << 0
#define LCD_CLEAR_BYTE							0x01
#define LCD_ENTRY_MODE_SET_BYTE			0x04 | LCD_DDRAM_ADDR << 1 | LCD_SHIFT << 0

// -------- Public function prototypes --------

void LCD_Write(bit reg_select, uint8_t LCD_data);
void LCD_Init(void);
void LCD_Print(uint8_t num_bytes, uint8_t * input_array);

#endif