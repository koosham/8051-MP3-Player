#ifndef _SDCARD_H
#define _SDCARD_H

#include "main.h"
#include "port.h"

#define SD_TYPE1										1
#define SD_TYPE2										2
#define SD_CAPACITY_UNKNOWN					0
#define SCSD												1
#define HCSD												2

#ifndef NO_ERROR
#define NO_ERROR										0x0000
#endif

#define SD_ILLEGAL_CMD_ERROR				0x0020
#define SD_CARD_ERROR								0x0040
#define SD_TIMEOUT_ERROR						0x0080
#define SD_VOLTAGE_ERROR						0x0100
#define SD_INVALID_RESPONSE_ERROR		0x0200
#define SD_INCOMPATIBLE_CARD_ERROR	0x0400
#define SD_POWERUP_ERROR						0x0800
#define SD_DATA_ERROR								0x1000

// -------- Public function prototypes --------

uint16_t SD_Send_Command(uint8_t command, uint32_t argument);
uint16_t SD_Receive_Response(uint8_t number_of_bytes, uint8_t *received_array);
uint16_t SD_Card_Init(void);
uint8_t *SD_Card_Type_Pointer(void);
uint8_t *SD_Card_Capacity_Pointer(void);
uint16_t SD_Read_Block(uint16_t number_of_bytes, uint8_t xdata * block);
uint8_t *Buffer1_Pointer(void);
uint8_t *Buffer2_Pointer(void);

#endif