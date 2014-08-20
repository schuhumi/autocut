/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#include <util/twi.h>
#include "ADC.h"
#include "TWISLAVE.h"

#define True		        1
#define False		        0
        
        
#define LED_PORT	        PORTB
#define LED_DDR		        DDRB
#define LED_GN		        Pin(PB7)
        
#define RELAYS_PORT             PORTB
#define RELAYS_DDR              DDRB
#define RELAYS_CELL1            Pin(PB0)
#define RELAYS_CELL2            Pin(PB1)
#define RELAYS_CELL3            Pin(PB2)
#define RELAYS_CELL4            Pin(PB6)
#define CELL_FULL               4.21F
#define CELL_EMPTY              3.70F
#define CELL_CRITICAL           3.65F
#define CELL_UNREAL             3.00F

#define PIEZO_PORT              PORTD
#define PIEZO_DDR               DDRD
#define PIEZO                   Pin(PD7)

#define Set(Register, Flags)	(Register |= (Flags))
#define Clear(Register, Flags)	(Register &= ~(Flags))
#define Toggle(Register, Flags)	(Register ^= (Flags))

#define safe_Set(Register, Flags);	{cli();(Register |= (Flags));sei(); }
#define safe_Clear(Register, Flags);	{cli();(Register &= ~(Flags));sei();}
#define safe_Toggle(Register, Flags);	{cli();(Register ^= (Flags));sei(); }

#define Pin(Pxx) (1<<(Pxx))

#define SLAVE_ADRESSE           (0x30<<1)

#define I2C_STATUS		0
#define I2C_CMD			1
#define I2C_CMD_ARG1	        2
#define I2C_CMD_ARG2	        3
#define I2C_CMD_ARG3	        4
#define I2C_CMD_ARG4	        5
#define I2C_CELL1VOLTl          6
#define I2C_CELL1VOLTh          7
#define I2C_CELL2VOLTl          8
#define I2C_CELL2VOLTh          9
#define I2C_CELL3VOLTl          10
#define I2C_CELL3VOLTh          11
#define I2C_CELL4VOLTl          12
#define I2C_CELL4VOLTh          13
#define I2C_CELL1PERC           14
#define I2C_CELL2PERC           15
#define I2C_CELL3PERC           16
#define I2C_CELL4PERC           17

#define STATUS_OK		1
#define STATUS_ERR		2

#define NOCMD			0

type_ADC_CHANNEL ADC_CELLS;

void cellsMeas (void);
void beep (uint16_t T_half, uint16_t len);

float CELL1Volt=CELL_FULL, CELL2Volt=CELL_FULL, CELL3Volt=CELL_FULL, CELL4Volt=CELL_FULL;

int main (void)
{
        _delay_ms(500);
        
        ADC_CELLS.samples	= 10;
        ADC_CELLS.uref		= ADC_REF_AVCC;
        ADC_CELLS.channel	= 0;
        ADC_CELLS.minValueIs    = 0;
        ADC_CELLS.maxValueIs    = 5.01;
        ADC_CELLS.blocking	= True;
        ADC_INIT(&ADC_CELLS, True);
                
        Set(LED_DDR, LED_GN);
        Set(LED_PORT, LED_GN);
        
        Set(PIEZO_DDR, PIEZO);
        Clear(PIEZO_PORT, PIEZO);
        
        Set(RELAYS_DDR, RELAYS_CELL1 | RELAYS_CELL2 | RELAYS_CELL3 | RELAYS_CELL4);
        Clear(RELAYS_PORT, RELAYS_CELL1 | RELAYS_CELL2 | RELAYS_CELL3 | RELAYS_CELL4);

        init_twi_slave(SLAVE_ADRESSE);
        
        i2cdata[I2C_STATUS] = 0;
        
        uint8_t ErrorHistory = True;
        while(1)
        {
                Toggle(LED_PORT, LED_GN);
                i2cdata[I2C_STATUS]++;
                cellsMeas();
                //Toggle(RELAYS_PORT, RELAYS_CELL4);
                
                if( (CELL1Volt<=CELL_CRITICAL) || (CELL2Volt<=CELL_CRITICAL) || (CELL3Volt<=CELL_CRITICAL) || (CELL4Volt<=CELL_CRITICAL) )
                {
                        if( (CELL1Volt<=CELL_UNREAL) || (CELL2Volt<=CELL_UNREAL) || (CELL3Volt<=CELL_UNREAL) || (CELL4Volt<=CELL_UNREAL) )
                                beep(160, 300);
                        else
                                beep(180, 1000);
                        
                        _delay_ms(100);
                        ErrorHistory = True;
                }
                else
                {
                        if(ErrorHistory == True)
                        {
                                beep(100,400);
                                beep(150,200);
                                ErrorHistory = False;
                        }
                        else
                                _delay_ms(200);
                }
        }
        
        return 0;
}

void beep (uint16_t T, uint16_t len)
{
        for(uint16_t i=0; i<len; i++)
        {
                Toggle(PIEZO_PORT, PIEZO);
                for(uint16_t t=0; t<T; t++)
                        _delay_us(1);
        }
        Clear(PIEZO_PORT, PIEZO);
}

uint8_t cellsMeas_State = 0;
uint8_t cellsMeas_cellNr = 1;
void cellsMeas (void)
{
        float f_tmp;
        uint16_t i_tmp;
        uint8_t i_tmpl, i_tmph, i_perc;
        switch(cellsMeas_State)
        {
        case 0:
                Clear(RELAYS_PORT, RELAYS_CELL1 | RELAYS_CELL2 | RELAYS_CELL3 | RELAYS_CELL4);
                if(cellsMeas_cellNr == 1)
                        Set(RELAYS_PORT, RELAYS_CELL1);
                else if(cellsMeas_cellNr == 2)
                        Set(RELAYS_PORT, RELAYS_CELL2);
                else if(cellsMeas_cellNr == 3)
                        Set(RELAYS_PORT, RELAYS_CELL3);
                else if(cellsMeas_cellNr == 4)
                        Set(RELAYS_PORT, RELAYS_CELL4);
                break;
        case 3:
                ADC_BG_START(&ADC_CELLS);
                break;
        case 4:
                if(!ADC_BG_MEASDONE())  // ADC noch nicht fertig
                        cellsMeas_State--; // Zurueckspringen um Schritt beim nächsten mal zu wiederholen
                break;
        case 5:
                f_tmp = ADC_BG_GETVALUE(&ADC_CELLS);
                
                if( f_tmp<=CELL_EMPTY )
                        i_perc = 0;
                else if( f_tmp>=CELL_FULL )
                        i_perc = 100;
                else
                        i_perc = (uint8_t)(100*(f_tmp-CELL_EMPTY)/(CELL_FULL-CELL_EMPTY));
                        
                i_tmp = (uint16_t)(f_tmp*1000);
                i_tmpl = i_tmp&0x7F;
                i_tmph = (i_tmp>>7)&0x7F;
                if(cellsMeas_cellNr == 1) {
                        i2cdata[I2C_CELL1VOLTl] = i_tmpl;
                        i2cdata[I2C_CELL1VOLTh] = i_tmph; 
                        i2cdata[I2C_CELL1PERC]  = i_perc; 
                        CELL1Volt = f_tmp; }
                else if(cellsMeas_cellNr == 2) {
                        i2cdata[I2C_CELL2VOLTl] = i_tmpl;
                        i2cdata[I2C_CELL2VOLTh] = i_tmph;
                        i2cdata[I2C_CELL2PERC]  = i_perc; 
                        CELL2Volt = f_tmp; }
                else if(cellsMeas_cellNr == 3) {
                        i2cdata[I2C_CELL3VOLTl] = i_tmpl;
                        i2cdata[I2C_CELL3VOLTh] = i_tmph;
                        i2cdata[I2C_CELL3PERC]  = i_perc; 
                        CELL3Volt = f_tmp; }
                else if(cellsMeas_cellNr == 4) {
                        i2cdata[I2C_CELL4VOLTl] = i_tmpl;
                        i2cdata[I2C_CELL4VOLTh] = i_tmph;
                        i2cdata[I2C_CELL4PERC]  = i_perc; 
                        CELL4Volt = f_tmp; }
                break;
        }
        cellsMeas_State++;
        if(cellsMeas_State>5)
        {
                cellsMeas_cellNr++;
                if(cellsMeas_cellNr>4)
                        cellsMeas_cellNr=1;
                cellsMeas_State = 0;
        }
        return;
}
