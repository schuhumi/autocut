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

/*
 *  File:			ADC.c
 * 	Autor:			Simon Schumann
 * 	Description:	library for easy usage of internal analog-digital converter
 * 	Contains:		routines for:	- initialisation
 * 									- measure
 */
 
// !- TODO -! //
// implement non-blocking measurement
 
#include <avr/io.h>
#include "ADC.h"

void ADC_INIT (type_ADC_CHANNEL *device, uint8_t isFirstADC)			// Es werden nur die Randbedingungen geschaffen, die Initialisierung
																		// des ADC an sich erfolgt in der Auslesen-Funktion
{

	/** Pinkonfiguration vornehmen **/
	ADC_DDR  &= ~(1<< device->channel);									// Entsprechender Pin -> Eingang
	ADC_PORT &= ~(1<< device->channel);									// Entsprechender Pin -> (eventuell aktivierte) Pullups deaktivieren

	// ISR aktivieren
	
	if(isFirstADC)
	{
		device_Last = 0;
		ADC_is_used = False;
		ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1);					// ADC einschalten + Prescaler=64
	}
}

float ADC_MEASURE (type_ADC_CHANNEL *device)
{
	if(ADC_is_used)
		return 0;
	ADC_is_used = True;
	if(device_Last != device)											// ADC muss für anderen Kanal umconfiguriert werden
	{
		// initititit
		ADMUX = (device->channel) | (device->uref);
		
		
	}

	float temp = 0;
	if(device->blocking)
	{
		
		for(uint8_t samples=0; samples < device->samples; samples++)
		{
			ADCSRA |= (1<<ADSC);										// ADC starten
			while(ADCSRA & (1<< ADSC));
			temp += ADCW;
		}
		temp /= (device->samples);
		
		temp = (temp/ADC_MAXVALUE)*((device->maxValueIs)-(device->minValueIs))+(device->minValueIs);
		
		
	}
	else
	{
		
		
	}
	
	

	device_Last = device;
	ADC_is_used = False;
	return temp;
}

uint8_t ADC_BG_START(type_ADC_CHANNEL *device)
{
        if(ADC_is_used)
		return 0;
	ADC_is_used = True;
	if(device_Last != device)											// ADC muss für anderen Kanal umconfiguriert werden
	{
		// initititit
		ADMUX = (device->channel) | (device->uref);
		
		
	}   
        ADCSRA |= (1<<ADSC);										// ADC starten 
}

uint8_t ADC_BG_MEASDONE(void)
{
        return !(ADCSRA & (1<< ADSC));
}

float ADC_BG_GETVALUE(type_ADC_CHANNEL *device)
{
        float temp = ADCW;
        ADC_is_used = False;
        return (temp/ADC_MAXVALUE)*((device->maxValueIs)-(device->minValueIs))+(device->minValueIs);
}
