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
#include "ADC.h"
#include "VECTOR.h"
#include "TWISLAVE.h"

#define True		1
#define False		0

#define EN_PORT		PORTC
#define EN_DDR		DDRC
#define CTRL_PORT	PORTD
#define CTRL_DDR	DDRD

#define CTRL_FL_F	Pin(PD4)
#define CTRL_FL_B	Pin(PD5)
#define CTRL_FR_F	Pin(PD6)
#define CTRL_FR_B	Pin(PD7)
#define CTRL_BL_F	Pin(PD0)
#define CTRL_BL_B	Pin(PD1)
#define CTRL_BR_F	Pin(PD2)
#define CTRL_BR_B	Pin(PD3)

#define EN_FL		Pin(PC6)
#define EN_FR		Pin(PC7)
#define EN_BL		Pin(PC4)
#define EN_BR		Pin(PC5)

#define LED_PORT	PORTC
#define LED_DDR		DDRC
#define LED_GN		Pin(PC3)
#define	LED_RD		Pin(PC2)

// in mm:
#define wheelSpacing		22.0
#define hingeSpacingFront	8.5
#define	hingeSpacingBack	20.5

// Ist-Zustand (negativer Wert bei zu langsam, positiver bei zu schnell)
#define speedCorrFL			-0.015
#define speedCorrFR			0.03
#define speedCorrBL			-0.020
#define speedCorrBR			0.030


#define Set(Register, Flags)	(Register |= (Flags))
#define Clear(Register, Flags)	(Register &= ~(Flags))
#define Toggle(Register, Flags)	(Register ^= (Flags))

#define safe_Set(Register, Flags);	        {cli();(Register |= (Flags));sei(); }
#define safe_Clear(Register, Flags);	        {cli();(Register &= ~(Flags));sei();}
#define safe_Toggle(Register, Flags);	{cli();(Register ^= (Flags));sei(); }

#define Pin(Pxx) (1<<(Pxx))

#define SLAVE_ADRESSE (0x28<<1) //Die Slave-Adresse

#define I2C_STATUS		0
#define I2C_CMD			1
#define I2C_CMD_ARG1	2
#define I2C_CMD_ARG2	3
#define I2C_CMD_ARG3	4
#define I2C_CMD_ARG4	5
#define I2C_ANGLE		6
#define I2C_FLpower		7
#define I2C_FLspeed		8
#define I2C_FLspdmeas	9
#define I2C_FLdistance	10
#define I2C_FRpower		11
#define I2C_FRspeed		12
#define I2C_FRspdmeas	13
#define I2C_FRdistance	14
#define I2C_BLpower		15
#define I2C_BLspeed		16
#define I2C_BLspdmeas	17
#define I2C_BLdistance	18
#define I2C_BRpower		19
#define I2C_BRspeed		20
#define I2C_BRspdmeas	21
#define I2C_BRdistance	22
#define I2C_MoveMode	23


#define STATUS_OK		1
#define STATUS_ERR		2

#define NOCMD			0
#define CMD_STOP		1
#define CMD_WRITEREG	2
#define CMD_USERANGLE	3
#define CMD_USERSPEED	4
#define CMD_MOTORL		5
#define CMD_MOTORR		6
#define CMD_MOVEMODE	7

#define MoveMode_Torque 1
#define MoveMode_Speed   2

float ADCangleStraight	= 570;
float ADCangleMin		= 570-200;
float ADCangleMax		= 570+200;
float AngleMin			= -54;
float AngleMax			= 54;

float Radian(float degree)
{
        return degree/(360/6.28318530717958647693);
}
float Degree(float radian)
{
        return radian*(360/6.28318530717958647693);
}

uint8_t RearSpeedControl = True;
int8_t power_FL=0, power_FR=0, power_BL=0, power_BR=0;
int8_t speed_FL=0, speed_FR=0, speed_BL=0, speed_BR=0;
int8_t spdmeas_FL=0, spdmeas_FR=0, spdmeas_BL=0, spdmeas_BR=0;
int16_t distance_FL=0, distance_FR=0, distance_BL=0, distance_BR=0;


/// ########## DIRTY
        // between the two front wheels (basis of all moving calculations):
        float mainPointAngle = 0;
        ///float mainPoint
        float mainPointAngleSet = 0;


        float Angle_hinge=0;
        float Vect_hinge_x=0;
        float Vect_hinge_y=0;
        float Vect_rotateBack=0;
        float Vect_moveBack=0;



type_ADC_CHANNEL ADC_FL, ADC_FR, ADC_BL, ADC_BR, ADC_hingeAngle;


//							FrontLeft		FrontRight			Knickwinkel			BackLeft			BackRight
void calculateBackMotors (float FL_speed, float FR_speed, float hingeAngle, float *Result_BL, float *Result_BR)
{
	// Bei den Berechnungen gilt:
	// - Hinterteil steht senkrecht im Koordinatensystem (Vorderteil ist oben)
	// - Drehpunkt des Knickgelenks ist im Punkt (0;0) (zu Anfang, wenn sich der Robo bewegt änderst sich das natürlich)
	// - Bei hingeAngle=0° -> Keine Knickung; Wenn nach links geknickt -> hingeAngle wird größer; nach rechts wird kleiner (negativ)
	/*
	 * Beispiel für hingeAngle=0:
	 *       y				
	 *       |					'Z' = Reifen
	 *       |                  '=' und 'H' = Verbindung
	 *    Z==M==Z               'O' = Knickgelenk
	 *       H                  'M' = MainPoint
	 * ------O--------x
	 *       H
	 *       H
	 *    Z=====Z
	 *       |
	 *       |
	 */
	/// Zustand des Vorderteils vor der Bewegung:
		type_VECTOR NullToFL, NullToFR; // Von 'O' zum jeweiligen 'Z'
		// Vektoren ohne Knickung:
			// Beide gehen in Y-Richtung von 0 bis zum MainPoint 
				NullToFL.y = hingeSpacingFront;
				NullToFR.y = hingeSpacingFront;
			// Der für links geht dann im Halben Radabstand nach links
				NullToFL.x = -(wheelSpacing/2);
			// Und der für rechts entsprechend nach rechts
				NullToFR.x = wheelSpacing/2;
		type_VECTOR MainPtToHinge;	// Von 'M' nach 'O'
		// Vektor ohne Knickung:
			MainPtToHinge.x = 0;	// Vektor steht senkrecht
			MainPtToHinge.y = -hingeSpacingFront; // und zeigt nach unten
		// Die drei Vektoren nach aktueller Knickung drehen:
			VECTOR_ROTATE(&NullToFL, Radian(hingeAngle));
			VECTOR_ROTATE(&NullToFR, Radian(hingeAngle));
			VECTOR_ROTATE(&MainPtToHinge, Radian(hingeAngle));
	
	
	/// Vorderteil bewegt sich:
		type_VECTOR MoveFL, MoveFR; // Vektoren, die die Ortsveränderung der Vorderräder zeigen
		// OrtsveränderungsVektoren der Vorderräder berechnen (+90 Weil Robo senkrecht im KO-System steht (hingeAngle=0° -> keine Knickung); Geschwindigkeit = Länge des Vektors)
			VECTOR_SETPOLAR(&MoveFL, Radian(hingeAngle+90), FL_speed);
			VECTOR_SETPOLAR(&MoveFR, Radian(hingeAngle+90), FR_speed);
			//printf("Vector MoveFL: ( %f ; %f )\n", MoveFL.x, MoveFL.y);
			//printf("Vector MoveFR: ( %f ; %f )\n", MoveFR.x, MoveFR.y);
		// Rotation des Vorderteils berechnen:	
			type_VECTOR FLminusFR;
			float rotationangle;	// Winkel, um den der Vorderteil durch die Räder gedreht wird
			VECTOR_SUBSTRACT(&MoveFL, &MoveFR, &FLminusFR);	// Nur die Differenz der beiden Bewegungen ergibt sich eine Rotation, der Rest ist eine Translation (Verschiebung)
			//printf("Vector FL-FR: ( %f ; %f )\n", FLminusFR.x, FLminusFR.y);
			rotationangle = asin(VECTOR_GETLENGTH(&FLminusFR)/wheelSpacing); // Da die Bewegungsvektoren der Räder Senkrecht auf die Radachse (wheelSpacing) stehen, lässt sich der
																			  // Rotationswinkel über Trigonometrie (inverssinus) berechnen (näherungsweise zumindest, da in Wirklichkeit
																			  // eine Kreisbahn beschrieben wird (evtl. liegt hier mein Problem?))
			// Wenn das linke Rad schneller dreht als das rechte -> Rechtskurve -> Drehwinkel negativ
			if(FL_speed>FR_speed)
				rotationangle = -rotationangle;
			//printf("rotation: %f radian / %f degrees\n", rotationangle , Degree(rotationangle));
	
		
	/// Vorderteil nach der Bewegung:
		// Vektor vom Mittelpunkt der Vorderräder (=MainPoint) zum Knickpunkt wurde durch die Bewegung gedreht
			VECTOR_ROTATE(&MainPtToHinge, rotationangle);
			//printf("Vector mainPointToHinge rotated: ( %f ; %f )\n", MainPtToHinge.x, MainPtToHinge.y);	
		
		// Die neue Position der Räder = alte Position + Bewegung
			type_VECTOR NullToFLMoved, NullToFRMoved;
			VECTOR_ADD(&NullToFL, &MoveFL, &NullToFLMoved);
			VECTOR_ADD(&NullToFR, &MoveFR, &NullToFRMoved);
			//printf("Vector NullToFLMoved: ( %f ; %f )\n", NullToFLMoved.x, NullToFLMoved.y);
			//printf("Vector NullToFRMoved: ( %f ; %f )\n", NullToFRMoved.x, NullToFRMoved.y);
		// Damit ergibt sich auch ein neuer MainPoint, der in der Mitte zwischen den Vorderrädern liegt
			type_VECTOR NullToMainPt;
			NullToMainPt.x = (NullToFLMoved.x + NullToFRMoved.x)/2;
			NullToMainPt.y = (NullToFLMoved.y + NullToFRMoved.y)/2;
			//printf("Vector NullToMainPt: ( %f ; %f )\n", NullToMainPt.x, NullToMainPt.y);
		// Wenn man jetzt den Vektor <vom MainPoint zum Knickpunkt> an den Ortsvektor des neuen MainPoints anstückelt, erhält man den Ortsvektor zum Knickgelenk
			type_VECTOR NullToHinge;
			VECTOR_ADD(&NullToMainPt, &MainPtToHinge, &NullToHinge);
			//printf("Vector NullToHinge: ( %f ; %f )\n", NullToHinge.x, NullToHinge.y);

	/// Hinterteil vor der Bewegung:
		// Vektoren vom Nullpunkt zu den Hinterrädern
			type_VECTOR NullToBL, NullToBR;
			// Beide gehen in Y-Richtung von 0 bis zum hinteren Mittelpunkt (nach unten)
				NullToBL.y = -hingeSpacingBack;
				NullToBR.y = -hingeSpacingBack;
			// Der für links geht dann im Halben Radabstand nach links
				NullToBL.x = -(wheelSpacing/2);
			// Und der für rechts entsprechend nach rechts
				NullToBR.x = wheelSpacing/2;
				
	/// Hinterteil nach der Bewegung:
		// Vektor vom Knickgelenk zum hinteren Mittelpunkt
			type_VECTOR HingeToBackMidPt;
			// Da das untere Ende des Vektors auf der Y-Achse liegen bleibt, das obere Ende aber mit dem Knickgelenk nach links/rechts bewegt wird,
			// lässt sich der x-Abschnitt des Vektors direkt aus dem Ortsvektor des Knickgelenks ableiten
			HingeToBackMidPt.x = -NullToHinge.x;
			// Jetzt muss der Y-Abschnitt noch so berechnet werden, dass der Vektor <hingeSpacingBack> (Abstand vom Knickgelenk zum hinteren Mittelpunkt) lang ist
			// Dabei gilt length=sqrt(x²+y²), das lässt ich nach y umformen: y=sqrt(length²-x²)
			HingeToBackMidPt.y = sqrt( (hingeSpacingBack*hingeSpacingBack) - (HingeToBackMidPt.x*HingeToBackMidPt.x) );
			// Es gibt zwei Lösungen für y, von den Berechnungen wird die positive Lösung zurückgegeben. Da der Vektor aber nach unten zeigt, muss das y umgedreht werden:
			HingeToBackMidPt.y = -HingeToBackMidPt.y;
			//printf("Vector HingeToBackMidPt: ( %f ; %f )\n", HingeToBackMidPt.x, HingeToBackMidPt.y);
		// Vektor vom hinteren Mittelpunkt zum rechten Hinterrad
			type_VECTOR BackMidPtToBR;
			// Der Vektor lässt sich ermitteln, indem man den Vektor HingeToBackMidPt um 90° gegen den Uhrzeigersinn dreht und dann auf wheelSpacing/2 stutzt
			BackMidPtToBR.x = HingeToBackMidPt.x;
			BackMidPtToBR.y = HingeToBackMidPt.y;
			VECTOR_ROTATE(&BackMidPtToBR, Radian(90));
			VECTOR_SETLENGTH(&BackMidPtToBR, wheelSpacing/2);
			//printf("Vector BackMidPtToBR: ( %f ; %f )\n", BackMidPtToBR.x, BackMidPtToBR.y);
		// Das selbe für das linke Hinterrad, nur dass diesmal um -90° bzw. 270° gedreht wird
			type_VECTOR BackMidPtToBL;
			BackMidPtToBL.x = HingeToBackMidPt.x;
			BackMidPtToBL.y = HingeToBackMidPt.y;
			VECTOR_ROTATE(&BackMidPtToBL, Radian(270));
			VECTOR_SETLENGTH(&BackMidPtToBL, wheelSpacing/2);
			//printf("Vector BackMidPtToBL: ( %f ; %f )\n", BackMidPtToBL.x, BackMidPtToBL.y);
		
		// Vektoren vom Nullpunkt zu den Hinterrädern:
			type_VECTOR NullToBLMoved, NullToBRMoved;
			// Vektorkette hinten links: Null -> Knickgelenk -> hinterer Mittelpunkt -> Rad hinten Links
				VECTOR_ADD(&NullToHinge, &HingeToBackMidPt, &NullToBLMoved);
				VECTOR_ADD(&NullToBLMoved, &BackMidPtToBL, &NullToBLMoved);
				//printf("Vector NullToBLMoved: ( %f ; %f )\n", NullToBLMoved.x, NullToBLMoved.y);
			// Vektorkette hinten rechts: Null -> Knickgelenk -> hinterer Mittelpunkt -> Rad hinten rechts
				VECTOR_ADD(&NullToHinge, &HingeToBackMidPt, &NullToBRMoved);
				VECTOR_ADD(&NullToBRMoved, &BackMidPtToBR, &NullToBRMoved);
				//printf("Vector NullToBRMoved: ( %f ; %f )\n", NullToBRMoved.x, NullToBRMoved.y);
		
	/// Hinterteil bewegt sich:
		// Da die Ortsvektoren zu den Hinterrädern nun vor und nach der Bewegung bekannt sind, lassen sich die Bewegungsvektoren mithilfe der Differenz berechnen:
			type_VECTOR MoveBL, MoveBR;
			VECTOR_SUBSTRACT(&NullToBLMoved, &NullToBL, &MoveBL);
			VECTOR_SUBSTRACT(&NullToBRMoved, &NullToBR, &MoveBR);
			//printf("Vector MoveBL: ( %f ; %f )\n", MoveBL.x, MoveBL.y);
			//printf("Vector MoveBR: ( %f ; %f )\n", MoveBR.x, MoveBR.y);
			
		// Die Länge der Move-Vektoren entspricht der Reifengeschwindigkeit, aber wenn der y-wert negativ ist müssen dei Reifen rückwärts laufen:
		*Result_BL = VECTOR_GETLENGTH(&MoveBL);
		if(MoveBL.y<0)
			*Result_BL = -*Result_BL;
		*Result_BR = VECTOR_GETLENGTH(&MoveBR);
		if(MoveBR.y<0)
			*Result_BR = -*Result_BR;
}

int main (void)
{
    
        // ADC for Front Left
                ADC_FL.samples	= 3;
                ADC_FL.uref		= ADC_REF_AVCC;
                ADC_FL.channel	= 2;
                ADC_FL.minValueIs = 0;
                ADC_FL.maxValueIs = 150;
                ADC_FL.blocking	= True;
                
        // ADC for Front Right
                ADC_FR.samples	= 3;
                ADC_FR.uref		= ADC_REF_AVCC;
                ADC_FR.channel	= 3;
                ADC_FR.minValueIs = 0;
                ADC_FR.maxValueIs = 150;
                ADC_FR.blocking	= True;
                
        // ADC for Back Left
                ADC_BL.samples	= 3;
                ADC_BL.uref		= ADC_REF_AVCC;
                ADC_BL.channel	= 0;
                ADC_BL.minValueIs = 0;
                ADC_BL.maxValueIs = 150;
                ADC_BL.blocking	= True;
                
        // ADC for Back Right
                ADC_BR.samples	= 3;
                ADC_BR.uref		= ADC_REF_AVCC;
                ADC_BR.channel	= 1;
                ADC_BR.minValueIs = 0;
                ADC_BR.maxValueIs = 150;
                ADC_BR.blocking	= True;
                
        // ADC for hinge angle
                ADC_hingeAngle.samples	= 3;
                ADC_hingeAngle.uref		= ADC_REF_AVCC;
                ADC_hingeAngle.channel	= 4;
                ADC_hingeAngle.minValueIs = ADCangleStraight/(ADCangleStraight-ADCangleMin)*AngleMin;
                ADC_hingeAngle.maxValueIs = (1023-ADCangleStraight)/(ADCangleMax-ADCangleStraight)*AngleMax;
                ADC_hingeAngle.blocking	= True;

        ADC_INIT(&ADC_FL, True);
        ADC_INIT(&ADC_FR, False);
        ADC_INIT(&ADC_BL, False);
        ADC_INIT(&ADC_BR, False);
        ADC_INIT(&ADC_hingeAngle, False);
        
        safe_Set(LED_DDR, LED_GN | LED_RD);
        safe_Set(LED_PORT, LED_RD | LED_GN);

        safe_Set(EN_DDR, EN_FL | EN_FR | EN_BL | EN_BR);
        safe_Set(CTRL_DDR, CTRL_FL_F | CTRL_FL_B | CTRL_FR_F | CTRL_FR_B | CTRL_BL_F | CTRL_BL_B | CTRL_BR_F | CTRL_BR_B);
        
        
        // Timer 0 -> 100us angepeilt
                /// FCPU = 8Mhz -> 125ns
                // Vorteiler 8 -> 1Mhz bzw. 1us
                        TCCR0 = (0<<CS02) | (1<<CS01) | (0<<CS00);
                // CTC-Modus (bis OCR0 zaehlen, isr und dann wieder bei 0 anfangen)
                        TCCR0 |= (1<<WGM01) | (0<<WGM00);
                // Von 0-99 in 1us-Schritten zaehlen -> 100us = 0.1ms
                        OCR0 = 100-1;
                // Output compare match interrupt enable
                        TIMSK |= (1<<OCIE0);
        
        sei();
        
        

        //TWI als Slave mit Adresse slaveadr starten
        init_twi_slave(SLAVE_ADRESSE);

        i2cdata[I2C_MoveMode] = MoveMode_Torque;
        i2cdata[I2C_ANGLE    ] = 100;
        i2cdata[I2C_FLpower  ] = 100;
        i2cdata[I2C_FLspeed  ] = 100;
        i2cdata[I2C_FLspdmeas] = 100;
        i2cdata[I2C_FRpower  ] = 100;
        i2cdata[I2C_FRspeed  ] = 100;
        i2cdata[I2C_FRspdmeas] = 100;
        i2cdata[I2C_BLpower  ] = 100;
        i2cdata[I2C_BLspeed  ] = 100;
        i2cdata[I2C_BLspdmeas] = 100;
        i2cdata[I2C_BRpower  ] = 100;
        i2cdata[I2C_BRspeed  ] = 100;
        i2cdata[I2C_BRspdmeas] = 100;
        
        uint8_t RedLEDCountdown = 10;
        
        speed_FL = 0;
        speed_FR = 0;
        speed_BL = 0;
        speed_BR = 0;
        
        
        float Ilangle=0, Irangle=0;
        float BL, BR, FL, FR, oldBL, oldBR, userangle, userspeed;	
        while(1)
        {
                if(i2cdata[I2C_CMD] != NOCMD)
                {
                        switch(i2cdata[I2C_CMD])
                        {
                                case CMD_STOP:
                                        speed_FL = 0;
                                        speed_FR = 0;
                                        speed_BL = 0;
                                        speed_BR = 0;
                                        power_FL = 0;
                                        power_FR = 0;
                                        power_BL = 0;
                                        power_BR = 0;
                                        userspeed = 0;
                                        userangle = 0;
                                        break;
                                case CMD_WRITEREG:
                                        switch(i2cdata[I2C_CMD_ARG1])
                                        {

                                        }
                                        break;
                                case CMD_MOVEMODE:
                                        power_FL = 0;
                                        power_FR = 0;
                                        power_BL = 0;
                                        power_BR = 0;
                                        userspeed = 0;
                                        userangle = 0;
                                        BL = 0;
                                        BR = 0;
                                        oldBL = 0;
                                        oldBR = 0;
                                        i2cdata[I2C_MoveMode] = i2cdata[I2C_CMD_ARG1];
                                case CMD_USERANGLE:
                                        userangle = i2cdata[I2C_CMD_ARG1]-100;
                                        break;
                                case CMD_USERSPEED:
                                        userspeed = i2cdata[I2C_CMD_ARG1]-100;
                                        break;
                                case CMD_MOTORL:
                                        speed_FL = i2cdata[I2C_CMD_ARG1]-100;
                                        break;
                                case CMD_MOTORR:
                                        speed_FR = i2cdata[I2C_CMD_ARG1]-100;
                                        break;
                        }
                        i2cdata[I2C_CMD] = NOCMD;
                        RedLEDCountdown = 2;
                }
                //else
                //        _delay_ms(1);
                
                
                
                
                
                if( i2cdata[I2C_MoveMode] == MoveMode_Speed )
                {
                        RearSpeedControl = True;
                        calculateBackMotors ((float)speed_FL, (float)speed_FR, Angle_hinge, &BL, &BR);
                        speed_BL = (int8_t)BL;
                        speed_BR = (int8_t)BR;
                }
                else if( i2cdata[I2C_MoveMode] == MoveMode_Torque )
                {
                        RearSpeedControl = False;
                        oldBL = BL;
                        oldBR = BR;
                        calculateBackMotors (((float)power_FL)/10, ((float)power_FR)/10, Angle_hinge, &BL, &BR);
                        //BL = (BL+oldBL*3)/4;
                        //BR = (BR+oldBR*3)/4;
                        power_FL -= (BL-oldBL)*5;
                        power_FR -= (BR-oldBR)*5;
                        if(BL>10)
                                BL=10;
                        if(BL<-10)
                                BL=-10;
                        if(BR>10)
                                BR=10;
                        if(BR<-10)
                                BR=-10;
                        power_BL = (int8_t)(BL*10);
                        power_BR = (int8_t)(BR*10);
                }
                else 		// No proper MoveMode defined -> Emergency stop
                {
                        speed_FL = 0;
                        speed_FR = 0;
                        speed_BL = 0;
                        speed_BR = 0;
                        power_FL = 0;
                        power_FR = 0;
                        power_BL = 0;
                        power_BR = 0;
                        userspeed = 0;
                        userangle = 0;
                }
                
                //calculateBackMotors (10, 10, Angle_hinge, &BL, &BR);
                
                //
                
                //speed_BL = speed_FR;
                //speed_BR = speed_FL;
                
                
                
                
                
                
                
                i2cdata[I2C_ANGLE    ] = Angle_hinge+100;
                i2cdata[I2C_FLpower  ] = power_FL+100;
                i2cdata[I2C_FLspeed  ] = speed_FL+100;
                i2cdata[I2C_FLspdmeas] = spdmeas_FL+100;
                i2cdata[I2C_FRpower  ] = power_FR+100;
                i2cdata[I2C_FRspeed  ] = speed_FR+100;
                i2cdata[I2C_FRspdmeas] = spdmeas_FR+100;
                i2cdata[I2C_BLpower  ] = power_BL+100;
                i2cdata[I2C_BLspeed  ] = speed_BL+100;
                i2cdata[I2C_BLspdmeas] = spdmeas_BL+100;
                i2cdata[I2C_BRpower  ] = power_BR+100;
                i2cdata[I2C_BRspeed  ] = speed_BR+100;
                i2cdata[I2C_BRspdmeas] = spdmeas_BR+100;
                
                /////_delay_ms(100);
                ///if(RedLEDCountdown > 0)
                ///{
                ///        RedLEDCountdown--;
                ///        Set(LED_PORT, LED_RD); // Red LED on
                ///}
                ///else
                ///        Clear(LED_PORT, LED_RD); // Red LED off
                ///        
                
                //cli();
                //safe_Toggle(LED_PORT, LED_GN);
                //sei();
                //_delay_ms(10);
        }
        
        return 0;
}

#define PowerTolerance  0
int8_t calcPower(int16_t powerNow, int8_t spdMeas, int8_t speed)
{
	if((speed-spdMeas)>PowerTolerance)
		powerNow += (speed-spdMeas)/4;
	else if((speed-spdMeas)<-PowerTolerance)
		powerNow += (speed-spdMeas)/4;
	if(powerNow>100)
		powerNow=100;
	if(powerNow<-100)
		powerNow=-100;
	if(abs(powerNow)>abs(speed*speed*speed))
	{
		powerNow = speed*speed*speed;
	}
        
	return powerNow;
}

uint8_t Channelselect = 0;
#define TimeDiff	1


ISR (TIMER0_COMP_vect)  // Wird alle 0.1ms aufgerufen
{
    
        // ********************** PWM start ***********
                static uint8_t pwmCtr=0;
                
                pwmCtr++;
                if(pwmCtr>100)
                        pwmCtr=0;
                
                ////// Front Left
                if(power_FL>0)
                {
                        Set(CTRL_PORT, CTRL_FL_F);
                        Clear(CTRL_PORT, CTRL_FL_B);
                }
                else
                {
                        Set(CTRL_PORT, CTRL_FL_B);
                        Clear(CTRL_PORT, CTRL_FL_F);
                }
                if(pwmCtr<abs(power_FL))
                        Set(EN_PORT, EN_FL);
                else
                        Clear(EN_PORT, EN_FL);
                        
                ////// Front Right
                if(power_FR>0)
                {
                        Set(CTRL_PORT, CTRL_FR_F);
                        Clear(CTRL_PORT, CTRL_FR_B);
                }
                else
                {
                        Set(CTRL_PORT, CTRL_FR_B);
                        Clear(CTRL_PORT, CTRL_FR_F);
                }
                if(pwmCtr<abs(power_FR))
                        Set(EN_PORT, EN_FR);
                else
                        Clear(EN_PORT, EN_FR);
                
                ////// Back Left	
                if(power_BL>0)
                {
                        Set(CTRL_PORT, CTRL_BL_F);
                        Clear(CTRL_PORT, CTRL_BL_B);
                }
                else
                {
                        Set(CTRL_PORT, CTRL_BL_B);
                        Clear(CTRL_PORT, CTRL_BL_F);
                }
                if(pwmCtr<abs(power_BL))
                        Set(EN_PORT, EN_BL);
                else
                        Clear(EN_PORT, EN_BL);
                        
                ////// Back Right
                if(power_BR>0)
                {
                        Set(CTRL_PORT, CTRL_BR_F);
                        Clear(CTRL_PORT, CTRL_BR_B);
                }
                else
                {
                        Set(CTRL_PORT, CTRL_BR_B);
                        Clear(CTRL_PORT, CTRL_BR_F);
                }
                if(pwmCtr<abs(power_BR))
                        Set(EN_PORT, EN_BR);
                else
                        Clear(EN_PORT, EN_BR);
        // ********************** PWM stop ************
        
        // ********************** SpeedMeas start *****
        
                // Statemachine
                static uint8_t SM_Pos = 0;
                static uint8_t SM_Pos_old = -1;
                static uint8_t SM_Pos_changed = True;
                static uint16_t TimerCTR = 0;
                static int8_t powerBackup = 0;
                
                #define SM_FL_halt               0
                #define SM_FL_wait               1
                #define SM_FL_start_ADC          2
                #define SM_FL_measure            3
                                  
                #define SM_FR_halt               4
                #define SM_FR_wait               5
                #define SM_FR_start_ADC          6
                #define SM_FR_measure            7
                                          
                #define SM_BL_halt               8
                #define SM_BL_wait               9
                #define SM_BL_start_ADC          10
                #define SM_BL_measure            11
                                      
                #define SM_BR_halt               12
                #define SM_BR_wait               13
                #define SM_BR_start_ADC          14
                #define SM_BR_measure            15
                
                #define SM_ANGLE_start_ADC       16
                #define SM_ANGLE_wait_ADC        17
                #define SM_ANGLE_measure         18
                
                #define SM_Pause                 19
                
                #define StateJustSwitched (SM_Pos-SM_Pos_old)
                
                #define DelayToMeasureBackEMF   600
                
                float adc;
                SM_Pos_old = SM_Pos;
                switch(SM_Pos)
                {
                        // Front left
                                case SM_FL_halt:
                                        powerBackup=power_FL;
                                        power_FL = 0;
                                        Clear(EN_PORT, EN_FL);
                                        Clear(LED_PORT, LED_RD);
                                        SM_Pos++;
                                        break;
                                case SM_FL_wait:
                                        if(SM_Pos_changed)
                                            TimerCTR = 0;
                                        TimerCTR++;
                                        if(TimerCTR>(DelayToMeasureBackEMF/100)) // 10*0.1ms = 1ms
                                                SM_Pos++;
                                        break;
                                case SM_FL_start_ADC:
                                        Set(LED_PORT, LED_RD);
                                        ADC_BG_START(&ADC_FL);
                                        SM_Pos++;
                                        break;
                                case SM_FL_measure:
                                        if(ADC_BG_MEASDONE())
                                        {
                                                adc = ADC_BG_GETVALUE(&ADC_FL)*(1+speedCorrFL);
                                                //if(adc>60)
                                                //        adc=60;
                                                if(powerBackup<0)
                                                        adc = -adc;
                                                spdmeas_FL = adc;
                                                distance_FL += adc/TimeDiff;
                                                power_FL = calcPower(powerBackup, spdmeas_FL, speed_FL);
                                                SM_Pos++;
                                        }
                                        break;
                                
                        // Front right
                                case SM_FR_halt:
                                        powerBackup=power_FR;
                                        power_FR = 0;
                                        Clear(EN_PORT, EN_FR);
                                        SM_Pos++;
                                        break;
                                case SM_FR_wait:
                                        if(SM_Pos_changed)
                                            TimerCTR = 0;
                                        TimerCTR++;
                                        if(TimerCTR>(DelayToMeasureBackEMF/100)) // 10*0.1ms = 1ms
                                                SM_Pos++;
                                        break;
                                case SM_FR_start_ADC:
                                        ADC_BG_START(&ADC_FR);
                                        SM_Pos++;
                                        break;
                                case SM_FR_measure:
                                        if(ADC_BG_MEASDONE())
                                        {
                                                adc = ADC_BG_GETVALUE(&ADC_FR)*(1+speedCorrFR);
                                                //if(adc>60)
                                                //        adc=60;
                                                if(powerBackup<0)
                                                        adc = -adc;
                                                spdmeas_FR = adc;
                                                distance_FR += adc/TimeDiff;
                                                power_FR = calcPower(powerBackup, spdmeas_FR, speed_FR);
                                                SM_Pos++;
                                        }
                                        break;
                                        
                        // Back left
                                case SM_BL_halt:
                                        powerBackup=power_BL;
                                        power_BL = 0;
                                        Clear(EN_PORT, EN_BL);
                                        SM_Pos++;
                                        break;
                                case SM_BL_wait:
                                        if(SM_Pos_changed)
                                            TimerCTR = 0;
                                        TimerCTR++;
                                        if(TimerCTR>(DelayToMeasureBackEMF/100)) // 10*0.1ms = 1ms
                                                SM_Pos++;
                                        break;
                                case SM_BL_start_ADC:
                                        ADC_BG_START(&ADC_BL);
                                        SM_Pos++;
                                        break;
                                case SM_BL_measure:
                                        if(ADC_BG_MEASDONE())
                                        {
                                                adc = ADC_BG_GETVALUE(&ADC_BL)*(1+speedCorrBL);
                                                //if(adc>60)
                                                //        adc=60;
                                                if(powerBackup<0)
                                                        adc = -adc;
                                                spdmeas_BL = adc;
                                                distance_BL += adc/TimeDiff;
                                                if(RearSpeedControl)
                                                        power_BL = calcPower(powerBackup, spdmeas_BL, speed_BL);
                                                SM_Pos++;
                                        }
                                        break;
                                        
                        // Back right
                                case SM_BR_halt:
                                        powerBackup=power_BR;
                                        power_BR = 0;
                                        Clear(EN_PORT, EN_BR);
                                        SM_Pos++;
                                        break;
                                case SM_BR_wait:
                                        if(SM_Pos_changed)
                                            TimerCTR = 0;
                                        TimerCTR++;
                                        if(TimerCTR>(DelayToMeasureBackEMF/100)) // 10*0.1ms = 1ms
                                                SM_Pos++;
                                        break;
                                case SM_BR_start_ADC:
                                        ADC_BG_START(&ADC_BR);
                                        SM_Pos++;
                                        break;
                                case SM_BR_measure:
                                        if(ADC_BG_MEASDONE())
                                        {
                                                adc = ADC_BG_GETVALUE(&ADC_BR)*(1+speedCorrBR);
                                                //if(adc>60)
                                                //        adc=60;
                                                if(powerBackup<0)
                                                        adc = -adc;
                                                spdmeas_BR = adc;
                                                distance_BR += adc/TimeDiff;
                                                if(RearSpeedControl)
                                                        power_BR = calcPower(powerBackup, spdmeas_BR, speed_BR);
                                                SM_Pos++;
                                        }
                                        break;
                                        
                        
                        case SM_ANGLE_start_ADC:
                                ADC_BG_START(&ADC_hingeAngle);
                                SM_Pos++;
                                break;
                        case SM_ANGLE_wait_ADC:
                                if(ADC_BG_MEASDONE())
                                        SM_Pos++;
                                break;
                        case SM_ANGLE_measure:
                                Angle_hinge = ADC_BG_GETVALUE(&ADC_hingeAngle);
                                SM_Pos++;
                                break;
                        
                        
                        case SM_Pause:
                                if(SM_Pos_changed)
                                    TimerCTR = 0;
                                TimerCTR++;
                                if(TimerCTR>=500) // 1000*0.1ms = 100ms
                                {
                                        SM_Pos=0;
                                        
                                }
                                break;
                        default:
                                
                                if(SM_Pos>=SM_Pause)
                                        SM_Pos=0;
                                else
                                        SM_Pos++;
                                break;
                }
                if(StateJustSwitched)
                        SM_Pos_changed = True;
                else
                        SM_Pos_changed = False;
                
        
        // ********************** SpeedMeas stop ******
}
