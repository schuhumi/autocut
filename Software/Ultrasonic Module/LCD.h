/*
 *  File:			LCD.h
 * 	Autor:			Simon Schumann
 * 	Description:	library for HD44780 LC-displays
 * 	
 * 			+++ See LCD.c for further details +++
 */

#ifndef _LCD_h_
#define _LCD_h_

typedef struct {
	volatile uint8_t		*port;
	volatile uint8_t		*pin;
	volatile uint8_t		*ddr;
	uint8_t				pinNr_EN;
	uint8_t				pinNr_RS;
	uint8_t				pinNr_D4;
	uint8_t				pinNr_D5;
	uint8_t				pinNr_D6;
	uint8_t				pinNr_D7;
} type_LCD;

void LCD_INIT(type_LCD *device);
void LCD_CLEAR(type_LCD *device);
void LCD_HOME(type_LCD *device);
void LCD_INT(type_LCD *device, uint16_t number);
void LCD_INTX(type_LCD *device, uint16_t number, uint8_t format, uint8_t digits);
void LCD_FLOAT(type_LCD *device, float number);
void LCD_FLOATX(type_LCD *device, float number, uint8_t digitsLeft, uint8_t digitsRight);
void LCD_STRING(type_LCD *device, char *string);
void LCD_POS(type_LCD *device, uint8_t line, uint8_t column);
void LCD_ENABLE(type_LCD *device);
void LCD_SETPINS (type_LCD *device, uint8_t data);
void LCD_WRITE(type_LCD *device, uint8_t data_cmd, uint8_t data);
void LCD_GENERATECHAR(type_LCD *device, uint8_t code, uint8_t *data);

#define	TIME_OF_INITS	3
#define	DELAY_EN_us		1
#define	LCD_DATA		0
#define	LCD_CMD			1
#define	DEC				10
#define	HEX				16
#define	BIN				2
#define	OCT				8

#define LCD_SET_CGADR           0x40
 
#define LCD_GC_CHAR0            0
#define LCD_GC_CHAR1            1
#define LCD_GC_CHAR2            2
#define LCD_GC_CHAR3            3
#define LCD_GC_CHAR4            4
#define LCD_GC_CHAR5            5
#define LCD_GC_CHAR6            6
#define LCD_GC_CHAR7            7 

#endif
