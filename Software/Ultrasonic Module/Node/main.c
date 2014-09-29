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


#define ADDR_SELF       (2<<4)
#define ADDR_MASTER     (1<<4)

#define DATA_DOECHO     (1)
#define DATA_ECHOREPL   (2)

#define Rx_PORT         PORTD
#define Rx_DDR          DDRD
#define Rx_PIN          PIND
#define Rx_highFlanc    Pin(PD2)
#define Rx_highvalid    Pin(PD3)

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

void Tx (uint16_t data, uint16_t waitfortimer1)
{
 
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
        
        if( waitfortimer1 )
                while( TCNT1 <  waitfortimer1);
        
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
                        _delay_us(11);
                //else
                //        _delay_us(24);
                
        }
        
}

void Rx_init (void)
{
        //// INT1
        //MCUCR |= (1 << ISC11) | (1 << ISC10);    // set INT1 to trigger on postive flanc
        //GICR |= (1 << INT1);      // Turns on INT0
        //sei();                    // turn on interrupts
        
        Clear(Rx_DDR, Rx_highFlanc | Rx_highvalid);

        // TIMER0
        TCCR0 |= (1 << CS02) | (0 << CS01) | (0 << CS00); // prescaler=256 -> 1 count = 21.333us
        TCNT0 = 0;
        
        // TIMER1
        TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10); // prescaler=64 -> 1 count = 5.3333us
        TCNT1 = 0;
}

uint8_t Rx (uint16_t timeoutms)
{
        DDRB |= (1<<PB1);
        uint8_t tries = 0;
        uint8_t parity=0, i=0;
        uint16_t downstream = 0;
        uint8_t data;
        uint8_t tmp, tmp2;
        uint16_t tmp3;
        uint16_t timeoutctrms = 0;
        
        Rx_start:
        PORTB |= (1<<PB1);
        //_delay_ms(4);
        tries++;
                TCNT0 = 0;
                while( (Rx_PIN&Rx_highvalid) == 0 )
                {
                        if( TCNT0 >= 188 ) // 4ms
                        {
                                TCNT0 = 0;
                                timeoutctrms += 4;
                                if( timeoutctrms > timeoutms )
                                        return 0;
                        }
                }
                
        // beginning bit ends
                TCNT0 = 0;
                while( (Rx_PIN&Rx_highvalid) || (TCNT0 <= 47) ) //1ms
                {
                        if( TCNT0 >= 141 ) // 3ms
                        {
                                timeoutctrms += 3;
                                if( timeoutctrms > timeoutms )
                                        return 0;
                                goto Rx_start;
                        }
                }
        
        
                
        PORTB ^= (1<<PB1);
                
        // startbit 1: beginning
                TCNT0 = 0;
                while( (TCNT0 <= 5) ); // 0.1ms
                while( (Rx_PIN&Rx_highFlanc) == 0 )
                {
                        if( TCNT0 >= 23 ) // 0.5ms
                                goto Rx_start;
                }
                
        PORTB ^= (1<<PB1);
                
                if( (Rx_PIN&Rx_highvalid) )
                        goto Rx_start;
                        
                
                        
                TCNT0 = 0;
                
        
                
                
        
                        
        // startbit 1: end
                while( (Rx_PIN&Rx_highFlanc)  )
                {
                        if( TCNT0 >= 47 ) // 1ms
                                goto Rx_start;
                }
                
                
        
                
        // startbit 2: beginning
                while( (Rx_PIN&Rx_highFlanc) == 0 )
                {
                        if( TCNT0 > 47+5 ) // 1ms
                                goto Rx_start;
                }
                if( (Rx_PIN&Rx_highvalid) )
                        goto Rx_start;
                if( TCNT0 < 47-5 ) // 1ms
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
                while( TCNT0 < 38 );
                tmp2 = (Rx_PIN&Rx_highFlanc);
                while( TCNT0 < 54 ) //46 = 0.99ms
                {
                        tmp = (Rx_PIN&Rx_highFlanc);
                        if( (!tmp2) && (tmp) )
                        {       // 1-bit
                                tmp3 = TCNT0;
                                while( (Rx_PIN&Rx_highvalid) && (TCNT0<=tmp3+9 )); //192us
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
                TCNT0 -= 46;
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


int main (void)
{
        Clear(DDRB, Pin(PB0));
        Set(PORTB, Pin(PB0));
        Tx_init();
        Rx_init();
        //while( True )
        //{
        //        Tx(ADDR_MASTER | DATA_ECHOREPL);
        //        _delay_ms(10);
        //}
        
        while( True )
        {
                uint8_t RxData = Rx(500);
                Toggle(PORTB, Pin(PB0));
                if( (RxData&0xF0) == ADDR_SELF )
                {
                        TCNT1 = 0;
                        Set(DDRB, Pin(PB0));
                        Set(PORTB, Pin(PB0));  
                        
                        //_delay_ms(50);
                        if( (RxData&0x0F) == DATA_DOECHO )
                                Tx(ADDR_MASTER | DATA_ECHOREPL, 9188); // 9188=(50e-3-(40*25e-6))/((1/12e6)*64)
                        
                        Clear(DDRB, Pin(PB0));
                        Set(PORTB, Pin(PB0));
                }
        }
        
}
