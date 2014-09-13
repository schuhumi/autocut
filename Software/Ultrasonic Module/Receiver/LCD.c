/*
 *  File:			LCD.c
 * 	Autor:			Simon Schumann
 * 	Description:	library for HD44780 LC-displays
 * 	Contains:		routines for:	- initialisation
 * 									- write:
 * 											- characters
 * 											- strings
 * 											- integers
 * 											- floats
 * 									- setting cursor position
 * 									- set cursor to home position
 * 									- clear display
 * 									- generate custom characters
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "LCD.h"

void LCD_SETPINS (type_LCD *device, uint8_t data)
{
	*device->port	&=~((1<< device->pinNr_D4)	\
					|	(1<< device->pinNr_D5)	\
					|	(1<< device->pinNr_D6)	\
					|	(1<< device->pinNr_D7));
	if(	data & 1<<0 ) *device->port	|= 	(1<< device->pinNr_D4);
	if(	data & 1<<1 ) *device->port	|= 	(1<< device->pinNr_D5);
	if(	data & 1<<2 ) *device->port	|= 	(1<< device->pinNr_D6);
	if(	data & 1<<3 ) *device->port	|= 	(1<< device->pinNr_D7);
}

void LCD_ENABLE(type_LCD *device)
{
	*device->port |= (1<<device->pinNr_EN);
	_delay_us(DELAY_EN_us);
	*device->port &=~ (1<<device->pinNr_EN);
}

void LCD_WRITE(type_LCD *device, uint8_t data_cmd, uint8_t data)
{
	uint8_t data_lowNibble = data & 0x0F;
	uint8_t data_highNibble = (data >> 4) & 0x0F;
	LCD_SETPINS(device, 0x0F);
	if(data_cmd == LCD_CMD)
		*device->port	&=~	(1<< device->pinNr_RS);
	else
		*device->port	|=	(1<< device->pinNr_RS);
	*device->port	&=~	(1<< device->pinNr_EN);
	LCD_SETPINS(device, data_highNibble);
	LCD_ENABLE(device);
	_delay_us(45);
	LCD_SETPINS(device, data_lowNibble);
	LCD_ENABLE(device);
	_delay_ms(2);
	LCD_SETPINS(device, 0);
}

void LCD_STRING(type_LCD *device, char *string)
{
	while(*string)
		LCD_WRITE(device, LCD_DATA, *string++);
}


void LCD_CLEAR(type_LCD *device)
{
	LCD_WRITE(device, LCD_CMD, 0x01);
}

void LCD_HOME(type_LCD *device)
{
	LCD_WRITE(device, LCD_CMD, 0x02);
}

void LCD_POS(type_LCD *device, uint8_t line, uint8_t column)
{
	LCD_WRITE(device, LCD_CMD, 0b10000000 + line*64 + column);
}

void LCD_INT(type_LCD *device, uint16_t number)
{
	LCD_INTX(device, number, DEC, 0);
}

void LCD_INTX(type_LCD *device, uint16_t number, uint8_t format, uint8_t digits)
{
	// TODO: implement digits
	char string[16];
	itoa(number,string, format);
	LCD_STRING(device, string);
}


void LCD_FLOAT(type_LCD *device, float number)
{
	LCD_FLOATX(device, number, 0, 2);
}

void LCD_FLOATX(type_LCD *device, float number, uint8_t digitsLeft, uint8_t digitsRight)
{
	// TODO: implement digitsLeft
	char string[16];
	dtostrf(number,1,digitsRight,string);
	LCD_STRING(device, string);
}

void LCD_GENERATECHAR(type_LCD *device, uint8_t code, uint8_t *data)
{
	LCD_WRITE(device, LCD_CMD, LCD_SET_CGADR|(code<<3));
	for (uint8_t i=0; i<8; i++)
	{
		LCD_WRITE(device, LCD_DATA, data[i]);
	}
}

void LCD_INIT(type_LCD *device)
{
	*device->ddr	|= 	(1<< device->pinNr_RS)	\
					|	(1<< device->pinNr_EN)	\
					|	(1<< device->pinNr_D4)	\
					|	(1<< device->pinNr_D5)	\
					|	(1<< device->pinNr_D6)	\
					|	(1<< device->pinNr_D7);
	for(uint8_t inits=0; inits<TIME_OF_INITS; inits++)
	{
		*device->port	|= 	(1<< device->pinNr_RS)	\
						|	(1<< device->pinNr_EN)	\
						|	(1<< device->pinNr_D4)	\
						|	(1<< device->pinNr_D5)	\
						|	(1<< device->pinNr_D6)	\
						|	(1<< device->pinNr_D7);	
						
		*device->port	&=~((1<< device->pinNr_RS)	\
						|	(1<< device->pinNr_EN));
		_delay_ms(15);
		*device->port	|= 	(1<< device->pinNr_EN);
		*device->port	&=~	(1<< device->pinNr_RS);
		LCD_ENABLE(device);
		_delay_ms(5);
		LCD_ENABLE(device);
		_delay_us(100);
		LCD_ENABLE(device);
		_delay_us(100);
		LCD_SETPINS(device, 0x02);
		LCD_ENABLE(device);
		_delay_us(100);
		LCD_WRITE(device, LCD_CMD, 0x28);
		LCD_WRITE(device, LCD_CMD, 0x08);
		LCD_WRITE(device, LCD_CMD, 0x01);
		LCD_WRITE(device, LCD_CMD, 0x06);
		LCD_WRITE(device, LCD_CMD, 0x0C);
		LCD_WRITE(device, LCD_CMD, 0b00000001);
		_delay_ms(100);
	}
}
