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
#include <util/twi.h> 	    //enthält z.B. die Bezeichnungen für die Statuscodes in TWSR
#include "TWISLAVE.h"
#include "LCD.h"


#define USC_PORT        PORTD
#define USC_DDR         DDRD
#define USC_PIN         PIND
#define USC_threshold   PD4
#define USC_signal      PD3     // INT1
#define USC_HVoff       PD2
// Tx1 Tx2 on OC1A/B

#define Set(Register, Flags)	(Register |= (Flags))
#define Clear(Register, Flags)	(Register &= ~(Flags))
#define Toggle(Register, Flags)	(Register ^= (Flags))

#define safe_Set(Register, Flags);	        {cli();(Register |= (Flags));sei(); }
#define safe_Clear(Register, Flags);	        {cli();(Register &= ~(Flags));sei();}
#define safe_Toggle(Register, Flags);	{cli();(Register ^= (Flags));sei(); }

#define Pin(Pxx) (1<<(Pxx))

#define True		1
#define False		0

uint8_t Receiveing40KHz=False;
uint8_t tmp = 0;


#define ReceiveCtrSignalValid 5
int main (void)
{
        Clear(USC_DDR, USC_signal);
        Set(USC_DDR, USC_threshold);
        Set(USC_PORT, USC_threshold); // hearing=yes
        Set(USC_DDR, USC_HVoff);
        Clear(USC_PORT, USC_HVoff);
        

	type_LCD frontLCD;
	
	/// Configure Devices //////////////////////////////////////
	// Display configuration:
		frontLCD.ddr = &DDRC;
		frontLCD.port = &PORTC;
		frontLCD.pin = &PINC;
		frontLCD.pinNr_EN = PC1;
		frontLCD.pinNr_RS = PC0;
		frontLCD.pinNr_D4 = PC3;
		frontLCD.pinNr_D5 = PC2;
		frontLCD.pinNr_D6 = PC5;
		frontLCD.pinNr_D7 = PC4;
	/// END Configure Devices //////////////////////////////////
	
        _delay_ms(200);
        
	/// INIT Devices ///////////////////////////////////////////
	// INIT display
		LCD_INIT(&frontLCD);
		LCD_CLEAR(&frontLCD);
	/// END INIT Devices ///////////////////////////////////////
	
        
        
        
        // PWM
        DDRB |= Pin(PB1) | Pin(PB2) ;
        ICR1 = 0;//200;
        OCR1A = ICR1/2;
        OCR1B = ICR1/2;        
        TCCR1A = (1<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (1<<COM1B0) | (1<<WGM11) | (0<<WGM10);
        TCCR1B |= (1 << CS10) | (1<<WGM13) | (1<<WGM12);
        
        // INT1
        MCUCR |= (1 << ISC11) | (1 << ISC10);    // set INT1 to trigger on postive flanc
        GICR |= (1 << INT1);      // Turns on INT0
        sei();                    // turn on interrupts

        // TIMER0
        TCCR0 |= (0 << CS02) | (0 << CS01) | (1 << CS00);
        TCNT0 = 0;
        
        // TIMER2
        TCCR2 |= (1 << CS22) | (0 << CS21) | (0 << CS20); // prescaler 64 -> 1=8us
        TCNT2 = 0;
        
        while(1)
        {
                ICR1 = 0;
                _delay_ms(100);
                float myCtr = -1;
                //Receiveing40KHz = False;
                ICR1 = 200;
                TCNT1 = -1;

                TCNT2 = 0;//-((ReceiveCtrSignalValid*25)/8);//0;
                TIFR |= (1<<TOV2); // Erase overflow flag
                while((Receiveing40KHz == False) )//&& ((TIFR&(1<<TOV2))==0) )
                {
                        _delay_us(1);
                }
                ICR1 = 0;
                myCtr = TCNT2;
                
                myCtr *= 8;
                myCtr -= ReceiveCtrSignalValid*25;
                myCtr /=2;
                //myCtr -= 8.6; // - eine wellenlänge
                
                
                LCD_POS(&frontLCD, 0, 0);
                LCD_FLOAT(&frontLCD, myCtr);
                LCD_STRING(&frontLCD, "us    ");
                LCD_POS(&frontLCD, 1, 0);
                LCD_FLOAT(&frontLCD, myCtr*0.343);
                LCD_STRING(&frontLCD, "mm    ");
                
                
                //LCD_POS(&frontLCD, 1, 0);
                //if( Receiveing40KHz )
                //        LCD_STRING(&frontLCD, "yes ");
                //else
                //        LCD_STRING(&frontLCD, "no  ");
                //LCD_INT(&frontLCD, tmp);
                //LCD_STRING(&frontLCD, "    ");
        }
        
        return 0;
}


ISR (INT1_vect)
{
        static uint8_t ReceiveCtr = 0;
        if( (TCNT0>170) && (TCNT0<230) && ((TIFR&(1<<TOV0))==0) ) // 6MHz/40KHz=200
        {
                if( ReceiveCtr<ReceiveCtrSignalValid)
                        ReceiveCtr++;
                
        }
        else
        {
                //if( ReceiveCtr>0)
                //        ReceiveCtr--;
                ReceiveCtr=0;
        }
        
        if( ReceiveCtr==ReceiveCtrSignalValid )
                Receiveing40KHz = True;
        else
                Receiveing40KHz = False;
        tmp = ReceiveCtr;
        
        TCNT0 = 0;
        TIFR |= (1<<TOV0); // Erase overflow flag
}
