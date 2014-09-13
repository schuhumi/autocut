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
#define F_CPU 12000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#include <util/twi.h> 	    //enthält z.B. die Bezeichnungen für die Statuscodes in TWSR
#include "TWISLAVE.h"
#include "LCD.h"


#define USC_PORT        PORTD
#define USC_DDR         DDRD
#define USC_PIN         PIND
#define USC_threshold   Pin(PD4)
#define USC_signal      Pin(PD3)     // INT1
#define USC_HVoff       Pin(PD2)
// Tx1 Tx2 on OC1A/B

#define Tx_DDR          DDRB
#define Tx_PORT         PORTB
#define Tx1             Pin(PB1)         
#define Tx2             Pin(PB2)

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

void Tx_init (void)
{
        Set(Tx_DDR, Tx1);
        Set(Tx_DDR, Tx2);
        Set(Tx_PORT, Tx1);
        Clear(Tx_PORT, Tx2);
}

void Tx (uint16_t data)
{
        /*uint8_t stream[(8+3)*wavesPerBit];
        uint8_t oldBit;
        uint8_t i, j;
        
        for( i=0; i<wavesPerBit; i++)
        {
                stream[i] = lowtohigh;
        }
        for( i=wavesPerBit; i<wavesPerBit*2; i++)
        {
                stream[i] = hightolow;
        }
        for( i=wavesPerBit*2; i<wavesPerBit*3; i++)
        {
                stream[i] = lowtohigh;
        }
        
        
        for(i=0; i<3*wavesPerBit; i++)
        {
                if( stream[i]==lowtohigh )
                {
                        Clear(Tx_PORT, Tx1);
                        Set(Tx_PORT, Tx2);
                        _delay_us(12);
                        Set(Tx_PORT, Tx1);
                        Clear(Tx_PORT, Tx2);
                        _delay_us(12);
                }
                else
                {
                        Set(Tx_PORT, Tx1);
                        Clear(Tx_PORT, Tx2);
                        _delay_us(12);
                        Clear(Tx_PORT, Tx1);
                        Set(Tx_PORT, Tx2);
                        _delay_us(12);
                }
                
        }*/
        
        uint8_t stream[1+2+8+2+1];
        uint8_t i, j;
        stream[0]=0;
        stream[1]=1;
        stream[2]=1;
        for(i=0; i<8; i++)
        {
                stream[1+2+i] = (data&(1<<i))?1:0;
        }
        uint8_t parity;
        //parity = 3-(data&0b11);
        parity = 0;
        parity ^= (data>>0)&0b11;
        parity ^= (data>>2)&0b11;
        parity ^= (data>>4)&0b11;
        parity ^= (data>>6)&0b11;
        stream[11]= (parity&0b01)?1:0;
        stream[12]= (parity&0b10)?1:0;
        stream[13]=1;
        
        for(i=0; i<1+2+8+2+1; i++)
        {
                for(j=0; j<40; j++)
                {
                        Clear(Tx_PORT, Tx1);
                        Set(Tx_PORT, Tx2);
                        _delay_us(12);
                        Set(Tx_PORT, Tx1);
                        Clear(Tx_PORT, Tx2);
                        _delay_us(12);
                }
                if(stream[i])
                        _delay_us(10);
                //else
                //        _delay_us(24);
                
        }
        
}


#define ReceiveCtrSignalValid 5
int main (void)
{
        Clear(USC_DDR, USC_signal);
        //Set(USC_PORT, USC_signal);
        //Set(USC_DDR, USC_threshold);
        //Set(USC_PORT, USC_threshold); // hearing=yes
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
	
        LCD_STRING(&frontLCD, "Hello");
        
        Tx_init();
        _delay_ms(50);
        while(1)
        {
                Tx(0b11000110);
                _delay_ms(10);
        }
        
        /// // PWM
        /// DDRB |= Pin(PB1) | Pin(PB2) ;
        /// ICR1 = 200;
        /// OCR1A = ICR1/2;
        /// OCR1B = ICR1/2;        
        /// TCCR1A = (1<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (1<<COM1B0) | (1<<WGM11) | (0<<WGM10);
        /// TCCR1B |= (1 << CS10) | (1<<WGM13) | (1<<WGM12);
        
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
        
        
        //ICR1 = 200;
        //while(1);
        
        
        while(1)
        {
                //ICR1 = 5;
                TCCR1B &= ~(1 << CS10);
                _delay_ms(100);
                float myCtr = -1;
                //Receiveing40KHz = False;
                //ICR1 = 200;
                TCNT1 = -1;
                TCCR1B |= (1 << CS10);

                TCNT2 = 0;//-((ReceiveCtrSignalValid*25)/8);//0;
                TIFR |= (1<<TOV2); // Erase overflow flag
                _delay_us(10);
                
                
                while((Receiveing40KHz == False) )//&& ((TIFR&(1<<TOV2))==0) )
                {
                        _delay_us(1);
                }
                //ICR1 = 5;
                //TCCR1B &= ~(1 << CS10);
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
