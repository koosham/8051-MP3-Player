#ifndef _SPI_H
#define _SPI_H

#include "main.h"
#include "port.h"

#ifndef NO_ERROR
#define NO_ERROR										0x0000
#endif

#define SPI_FREQ_ERROR							0x0001
#define SPI_MODF_ERROR							0x0002
#define SPI_SSERR_ERROR							0x0004
#define SPI_WCOL_ERROR							0x0008
#define SPI_TIMEOUT_ERROR						0x0010

#define SPI_MAX_CLK									(FREQ_PER / 2)

// -------- Public function prototypes --------

uint16_t SPI_Master_Init(uint32_t clock_freq);
uint16_t SPI_Transfer(uint8_t send_value, uint8_t *received_value);

#endif