#ifndef _PORT_H
#define _PORT_H

#include "main.h"

/* Byte Registers */
sfr LCD_MODULE_DATA_PORT				= 0x80;

/* Bit Definitions */

sbit nCS												= P1^0;
sbit STA013_RST									= P1^1;
sbit STA013_DATA_REQ						= P1^2;
sbit SDA												= P1^3;
sbit SCL												= P1^4;

sbit SW1												= P2^0;
sbit SW2												= P2^1;
sbit SW3												= P2^2;
sbit SW4												= P2^3;
sbit LED_RED										= P2^4;
sbit LED_YELLOW									= P2^5;
sbit LED_AMBER									= P2^6;
sbit LED_GREEN									= P2^7;

sbit RXD_PIN										= P3^0;
sbit TXD_PIN										= P3^1;
sbit STA013_EN									= P3^2;
sbit TRIGGER										= P3^5;
sbit LCD_RS_PIN									= P3^6;
sbit LCD_E_PIN									= P3^7;

#endif