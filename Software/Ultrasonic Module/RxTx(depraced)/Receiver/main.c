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

#define Rx_PORT         PORTD
#define Rx_DDR          DDRD
#define Rx_PIN          PIND
#define Rx_highFlanc    Pin(PD2)
#define Rx_highvalid    Pin(PD3)

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

void Rx_init (void)
{
        //// INT1
        //MCUCR |= (1 << ISC11) | (1 << ISC10);    // set INT1 to trigger on postive flanc
        //GICR |= (1 << INT1);      // Turns on INT0
        //sei();                    // turn on interrupts
        
        Clear(Rx_DDR, Rx_highFlanc | Rx_highvalid);

        // TIMER0
        TCCR0 |= (1 << CS02) | (0 << CS01) | (0 << CS00); // prescaler=256 -> 1 count = 32us
        TCNT0 = 0;
}

uint8_t Rx (void)
{
        DDRB |= (1<<PB1);
        uint8_t tries = 0;
        uint8_t parity=0, i=0;
        uint16_t downstream = 0;
        uint8_t data;
        uint8_t tmp, tmp2;
        uint16_t tmp3;
        
        Rx_start:
        PORTB |= (1<<PB1);
        //_delay_ms(4);
        tries++;
                TCNT0 = 0;
                while( (Rx_PIN&Rx_highvalid) == 0 )
                {
                        ///if( TCNT0 >= 234 ) // 20ms
                        ///        goto Rx_start;
                }
                
        // beginning bit ends
                TCNT0 = 0;
                while( (Rx_PIN&Rx_highvalid) || (TCNT0 <= 31) ) //1ms
                {
                        if( TCNT0 >= 94 ) // 3ms
                                goto Rx_start;
                }
        
        
                
        PORTB ^= (1<<PB1);
                
        // startbit 1: beginning
                TCNT0 = 0;
                while( (TCNT0 <= 3) ); // 0.1ms
                while( (Rx_PIN&Rx_highFlanc) == 0 )
                {
                        if( TCNT0 >= 15 ) // 0.5ms
                                goto Rx_start;
                }
                
        PORTB ^= (1<<PB1);
                
                if( (Rx_PIN&Rx_highvalid) )
                        goto Rx_start;
                        
                
                        
                TCNT0 = 0;
                
        
                
                
        
                        
        // startbit 1: end
                while( (Rx_PIN&Rx_highFlanc)  )
                {
                        if( TCNT0 >= 32 ) // 1ms
                                goto Rx_start;
                }
                
                
        
                
        // startbit 2: beginning
                while( (Rx_PIN&Rx_highFlanc) == 0 )
                {
                        if( TCNT0 > 32+4 ) // 1ms
                                goto Rx_start;
                }
                if( (Rx_PIN&Rx_highvalid) )
                        goto Rx_start;
                if( TCNT0 < 32-4 ) // 1ms
                                goto Rx_start;

        PORTB ^= (1<<PB1);
        //        _delay_ms(6);
        //        goto Rx_start;
        
        //goto Rx_start;
        
        //return tries;
        
       //_delay_us(200);
       //PORTB ^= (1<<PB1);
       TCNT0=0;
       downstream = 0;
       
       for( i=0; i<8+2; i++)
       {        //_delay_us(1000);
                while( TCNT0 < 25 );
                tmp2 = (Rx_PIN&Rx_highFlanc);
                while( TCNT0 < 36 ) //31 = 0.99ms
                {
                        tmp = (Rx_PIN&Rx_highFlanc);
                        if( (!tmp2) && (tmp) )
                        {       // 1-bit
                                tmp3 = TCNT0;
                                while( (Rx_PIN&Rx_highvalid) && (TCNT0<=tmp3+6 )); //192us
                                if( (Rx_PIN&Rx_highvalid) ==0 )
                                {
                                        downstream |= (1<<i);
                                        TCNT0=0;
                                        goto endofbitcycle;
                                }
                                else
                                {
                                        TCNT0=0;
                                        goto endofbitcycle;
                                }
                                
                        }
                        tmp2=tmp;
                        
                }

                // 0-bit
                TCNT0 -= 31;
                //TCNT0=5;
                
                
                endofbitcycle:;
                PORTB ^= (1<<PB1);
       }
       
       data = downstream&0xFF;
       parity = (downstream>>8)&0b11;
       
       if( data == 0)                   // ########### ??
                goto Rx_start;

       ///_delay_ms(8); goto Rx_start;
       ///
       ///parity = 0;
       ///
       ///while( TCNT0 < 31 );
       ///TCNT0=0;
       ///PORTB ^= (1<<PB1);
       ///if( (Rx_PIN&Rx_highvalid) ==0 )
       ///       parity |= 0b01;
       ///               
       ///while( TCNT0 < 31 );
       ///TCNT0=0;
       ///PORTB ^= (1<<PB1);
       ///if( (Rx_PIN&Rx_highvalid) ==0 )
       ///       parity |= 0b10;
        
        uint8_t myparity = 0;
        myparity ^= (data>>0)&0b11;
        myparity ^= (data>>2)&0b11;
        myparity ^= (data>>4)&0b11;
        myparity ^= (data>>6)&0b11;
        
        if( myparity != parity )
                goto Rx_start;
       
       
       _delay_ms(1);
       
       //goto Rx_start;
       
       //return tries;
       return data;
}



#define ReceiveCtrSignalValid 5
int main (void)
{
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
	
        DDRB |= Pin(PB2);
        
        uint8_t showupdate = 1;
        Rx_init();
        char TxBuffer[] = "________________";
        char dataRx;
        LCD_POS(&frontLCD, 0,0);
        LCD_STRING(&frontLCD, TxBuffer);
        while(1)
        {
                dataRx = Rx();
                for( uint8_t iterate = 1; iterate<16; iterate++)
                        TxBuffer[iterate-1] = TxBuffer[iterate];
                TxBuffer[15] = dataRx;
                
                LCD_POS(&frontLCD, 0,0);
                LCD_STRING(&frontLCD, TxBuffer);
                LCD_POS(&frontLCD, 1, 0);
                LCD_INTX(&frontLCD, dataRx, 2, 8);
                if( showupdate )
                        LCD_STRING(&frontLCD, "  #        ");
                else
                        LCD_STRING(&frontLCD, " #         ");
                showupdate = !showupdate;
                PORTB ^= Pin(PB2);
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



