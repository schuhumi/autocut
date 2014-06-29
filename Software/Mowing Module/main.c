#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#include <util/twi.h> 	    //enthält z.B. die Bezeichnungen für die Statuscodes in TWSR
#include "ADC.h"

#include "TWISLAVE.h"

#define True		1
#define False		0

#define EN_PORT		PORTC
#define EN_DDR		DDRC
#define CTRL_PORT	PORTC
#define CTRL_DDR	DDRC

#define CTRL_A1	        Pin(PC7)
#define CTRL_A2	        Pin(PC5)
#define CTRL_B1	        Pin(PC4)
#define CTRL_B2	        Pin(PC2)

#define EN_A		Pin(PC6)
#define EN_B		Pin(PC3)

#define FET_PORT        PORTB
#define FET_DDR         DDRB
#define FET_1           Pin(PB3)
#define FET_2           Pin(PB2)
#define FET_3           Pin(PB1)
#define FET_4           Pin(PB0)


#define LED_PORT	PORTB
#define LED_DDR		DDRB
#define	LED		Pin(PB4)

// Ist-Zustand (negativer Wert bei zu langsam, positiver bei zu schnell)
#define speedCorrA			0.00
#define speedCorrB			0.00
#define speedCorrFet1			0.00
#define speedCorrFet2			0.00
#define speedCorrFet3			0.00
#define speedCorrFet4			0.00


#define Set(Register, Flags)	(Register |= (Flags))
#define Clear(Register, Flags)	(Register &= ~(Flags))
#define Toggle(Register, Flags)	(Register ^= (Flags))

#define safe_Set(Register, Flags);	        {cli();(Register |= (Flags));sei(); }
#define safe_Clear(Register, Flags);	        {cli();(Register &= ~(Flags));sei();}
#define safe_Toggle(Register, Flags);	{cli();(Register ^= (Flags));sei(); }

#define Pin(Pxx) (1<<(Pxx))

#define SLAVE_ADRESSE (0x32<<1) //Die Slave-Adresse

#define I2C_STATUS		0
#define I2C_CMD			1
#define I2C_CMD_ARG1	        2
#define I2C_CMD_ARG2	        3
#define I2C_CMD_ARG3	        4
#define I2C_CMD_ARG4	        5
#define I2C_FET1power		6
#define I2C_FET1speed		7
#define I2C_FET1spdmeas	        8
#define I2C_FET2power		9
#define I2C_FET2speed		10
#define I2C_FET2spdmeas	        11
#define I2C_FET3power		12
#define I2C_FET3speed		13
#define I2C_FET3spdmeas	        14
#define I2C_FET4power		15
#define I2C_FET4speed		16
#define I2C_FET4spdmeas	        17
#define I2C_Apower		18
#define I2C_Aspeed		19
#define I2C_Aspdmeas	        20
#define I2C_Bpower		21
#define I2C_Bspeed		22
#define I2C_Bspdmeas	        23

#define STATUS_OK		1
#define STATUS_ERR		2

#define NOCMD			0
#define CMD_STOP		1
#define CMD_FET1SPD             2
#define CMD_FET2SPD             3
#define CMD_FET3SPD             4
#define CMD_FET4SPD             5


int8_t power_A=0, power_B=0, power_Fet1=0, power_Fet2=0, power_Fet3=0, power_Fet4=0;
int8_t speed_A=0, speed_B=0, speed_Fet1=0, speed_Fet2=0, speed_Fet3=0, speed_Fet4=0;
int8_t accl_A=0, accl_B=0, accl_Fet1=0, accl_Fet2=0, accl_Fet3=0, accl_Fet4=0;
int8_t spdmeas_A=0, spdmeas_B=0, spdmeas_Fet1=0, spdmeas_Fet2=0, spdmeas_Fet3=0, spdmeas_Fet4=0;


type_ADC_CHANNEL ADC_A, ADC_B, ADC_FET1, ADC_FET2, ADC_FET3, ADC_FET4;

void pwm (void);

int main (void)
{
    
        // ADC A
                ADC_A.samples	        = 3;
                ADC_A.uref		= ADC_REF_AVCC;
                ADC_A.channel	        = 5;
                ADC_A.minValueIs        = 0;
                ADC_A.maxValueIs        = 150;
                ADC_A.blocking	        = True;
                
        // ADC B
                ADC_B.samples	        = 3;
                ADC_B.uref		= ADC_REF_AVCC;
                ADC_B.channel	        = 4;
                ADC_B.minValueIs        = 0;
                ADC_B.maxValueIs        = 150;
                ADC_B.blocking	        = True;
                
        // ADC FET1
                ADC_FET1.samples	= 1;
                ADC_FET1.uref		= ADC_REF_AVCC;
                ADC_FET1.channel	= 3;
                ADC_FET1.minValueIs     = 0;
                ADC_FET1.maxValueIs     = 100;
                ADC_FET1.blocking	= True;
                
        // ADC FET2
                ADC_FET2.samples	= 1;
                ADC_FET2.uref		= ADC_REF_AVCC;
                ADC_FET2.channel	= 2;
                ADC_FET2.minValueIs     = 0;
                ADC_FET2.maxValueIs     = 100;
                ADC_FET2.blocking	= True;
                
        // ADC FET3
                ADC_FET3.samples	= 1;
                ADC_FET3.uref		= ADC_REF_AVCC;
                ADC_FET3.channel	= 1;
                ADC_FET3.minValueIs     = 0;
                ADC_FET3.maxValueIs     = 100;
                ADC_FET3.blocking	= True;
                
        // ADC FET4
                ADC_FET4.samples	= 1;
                ADC_FET4.uref		= ADC_REF_AVCC;
                ADC_FET4.channel	= 0;
                ADC_FET4.minValueIs     = 0;
                ADC_FET4.maxValueIs     = 100;
                ADC_FET4.blocking	= True;

        ADC_INIT(&ADC_A, True);
        ADC_INIT(&ADC_B, False);
        ADC_INIT(&ADC_FET1, False);
        ADC_INIT(&ADC_FET2, False);
        ADC_INIT(&ADC_FET3, False);
        ADC_INIT(&ADC_FET4, False);
        
        safe_Set(LED_DDR, LED);
        safe_Set(LED_PORT, LED);
        
        safe_Set(FET_DDR, FET_1 | FET_2 | FET_3 | FET_4);
        safe_Clear(FET_PORT, FET_1 | FET_2 | FET_3 | FET_4); // Motoren aus
        
        _delay_ms(1000);
        
        {
                ADC_FET1.samples	= 10;
                ADC_FET2.samples	= 10;
                ADC_FET3.samples	= 10;
                ADC_FET4.samples	= 10;
                float tmp1 = ADC_MEASURE(&ADC_FET1);
                float tmp2 = ADC_MEASURE(&ADC_FET2);
                float tmp3 = ADC_MEASURE(&ADC_FET3);
                float tmp4 = ADC_MEASURE(&ADC_FET4);
                float tmp_avgctr = 0;
                float tmp_avg = 0;
                
                if(tmp1 > 30)
                {
                        tmp_avg += tmp1;
                        tmp_avgctr++;
                }
                
                if(tmp2 > 30)
                {
                        tmp_avg += tmp2;
                        tmp_avgctr++;
                }
                
                if(tmp3 > 30)
                {
                        tmp_avg += tmp3;
                        tmp_avgctr++;
                }
                
                if(tmp4 > 30)
                {
                        tmp_avg += tmp4;
                        tmp_avgctr++;
                }
                
                tmp_avg /= tmp_avgctr;
                
                ADC_FET1.maxValueIs     = (100/tmp_avg)*100;
                ADC_FET2.maxValueIs     = (100/tmp_avg)*100;
                ADC_FET3.maxValueIs     = (100/tmp_avg)*100;
                ADC_FET4.maxValueIs     = (100/tmp_avg)*100;
                
                ADC_FET1.samples	= 2;
                ADC_FET2.samples	= 2;
                ADC_FET3.samples	= 2;
                ADC_FET4.samples	= 2;
        }
        
        //safe_Set(EN_DDR, EN_A | EN_B);
        //safe_Set(CTRL_DDR, CTRL_A1 | CTRL_A2 | CTRL_B1 | CTRL_B2);
        
        
   ///     // Timer 0 -> 100us angepeilt
   ///             /// FCPU = 16Mhz -> 62.5ns
   ///             // Vorteiler 8 -> 2Mhz bzw. 0.5us
   ///                     TCCR0 = (1<CS02) | (0<<CS01) | (1<<CS00);      /// /8
   ///             // CTC-Modus (bis OCR0 zaehlen, isr und dann wieder bei 0 anfangen)
   ///                     TCCR0 |= (1<<WGM01) | (0<<WGM00);
   ///             // Von 0-199 in 0.5us-Schritten zaehlen -> 200*0.5us = 0.1ms
   ///                     OCR0 = 40-1;
   ///             // Output compare match interrupt enable
   ///                     TIMSK |= (1<<OCIE0);
   ///     
   ///     sei();
        
        

        //TWI als Slave mit Adresse slaveadr starten
        init_twi_slave(SLAVE_ADRESSE);

        i2cdata[I2C_FET1power	] = 100;
        i2cdata[I2C_FET1speed	] = 100;
        i2cdata[I2C_FET1spdmeas	] = 100;
        i2cdata[I2C_FET2power	] = 100;
        i2cdata[I2C_FET2speed	] = 100;
        i2cdata[I2C_FET2spdmeas	] = 100;
        i2cdata[I2C_FET3power	] = 100;
        i2cdata[I2C_FET3speed	] = 100;
        i2cdata[I2C_FET3spdmeas	] = 100;
        i2cdata[I2C_FET4power	] = 100;
        i2cdata[I2C_FET4speed	] = 100;
        i2cdata[I2C_FET4spdmeas	] = 100;
        i2cdata[I2C_Apower	] = 100;
        i2cdata[I2C_Aspeed	] = 100;
        i2cdata[I2C_Aspdmeas	] = 100;
        i2cdata[I2C_Bpower	] = 100;
        i2cdata[I2C_Bspeed	] = 100;
        i2cdata[I2C_Bspdmeas	] = 100;
        	
                
        while(1)
        {
                if(i2cdata[I2C_CMD] != NOCMD)
                {
                        switch(i2cdata[I2C_CMD])
                        {
                                case CMD_STOP:
                                        speed_Fet1 = 0;
                                        speed_Fet2 = 0;
                                        speed_Fet3 = 0;
                                        speed_Fet4 = 0;
                                        speed_A = 0;
                                        speed_B = 0;
                                        power_Fet1 = 0;
                                        power_Fet2 = 0;
                                        power_Fet3 = 0;
                                        power_Fet4 = 0;
                                        power_A = 0;
                                        power_B = 0;
                                        break;
                                case CMD_FET1SPD:
                                        speed_Fet1 = i2cdata[I2C_CMD_ARG1];
                                        break;
                                case CMD_FET2SPD:
                                        speed_Fet2 = i2cdata[I2C_CMD_ARG1];
                                        break;
                                case CMD_FET3SPD:
                                        speed_Fet3 = i2cdata[I2C_CMD_ARG1];
                                        break;
                                case CMD_FET4SPD:
                                        speed_Fet4 = i2cdata[I2C_CMD_ARG1];
                                        break;
                        }
                        i2cdata[I2C_CMD] = NOCMD;
                }
                //else
                //        _delay_us(1); // Grund: der rapsi schreibt zuerst I2C_CMD und dann I2C_CMD_ARG.. D.h. der Request wird schon verarbeitet wenn ernoch garnicht vollständig gesendet wurde. -> Shit
                
                
                
                i2cdata[I2C_FET1power	] = 100+  power_Fet1;
                i2cdata[I2C_FET1speed	] = 000+  speed_Fet1;
                i2cdata[I2C_FET1spdmeas	] = 100+spdmeas_Fet1;
                i2cdata[I2C_FET2power	] = 100+  power_Fet2;
                i2cdata[I2C_FET2speed	] = 000+  speed_Fet2;
                i2cdata[I2C_FET2spdmeas	] = 100+spdmeas_Fet2;
                i2cdata[I2C_FET3power	] = 100+  power_Fet3;
                i2cdata[I2C_FET3speed	] = 000+  speed_Fet3;
                i2cdata[I2C_FET3spdmeas	] = 100+spdmeas_Fet3;
                i2cdata[I2C_FET4power	] = 100+  power_Fet4;
                i2cdata[I2C_FET4speed	] = 000+  speed_Fet4;
                i2cdata[I2C_FET4spdmeas	] = 100+spdmeas_Fet4;
                i2cdata[I2C_Apower	] = 100+  power_A;
                i2cdata[I2C_Aspeed	] = 100+  speed_A;
                i2cdata[I2C_Aspdmeas	] = 100+spdmeas_A;
                i2cdata[I2C_Bpower	] = 100+  power_B;
                i2cdata[I2C_Bspeed	] = 100+  speed_B;
                i2cdata[I2C_Bspdmeas	] = 100+spdmeas_B;
                
                pwm();
                
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

#define FET_MAXPWR      70
#define FET_MAXPWRABOVE 10 // speed
#define FET_PWRONSTART  30
#define FetPowerTolerance  0
int8_t calcPowerFET(int16_t powerNow, int8_t spdMeas, int8_t speed, int8_t accl)
{
        if( speed == 0)
                return 0;
                
        //return speed/2;
                
        int8_t thismax = 0;
        if( spdMeas >= FET_MAXPWRABOVE )
                thismax = FET_MAXPWR;
        else
        {
                float tmp=0;
                tmp = spdMeas;
                tmp /= FET_MAXPWRABOVE;
                tmp *= (FET_MAXPWR - FET_PWRONSTART);
                tmp += FET_PWRONSTART;
                thismax = tmp;
                //((float)(FET_MAXPWR-FET_PWRONSTART)*(float)(spdMeas/FET_MAXPWRABOVE))+FET_PWRONSTART;
        }
        
        int8_t diff = (speed-spdMeas);
        if( diff > 0 )
        {
                if( accl <= diff/4)
                        powerNow += diff/4+1;
        }
        else
        {
                if( accl >= diff/4)
                        powerNow += diff/4-1;
        }
        
        
        //if(speed > spdMeas && accl < 2)
        //        powerNow++;
        //if(speed < spdMeas && accl > -2)
        //        powerNow--;
	
        ///if((speed-spdMeas)>FetPowerTolerance)
	///	powerNow += (speed-spdMeas);
	///else if((speed-spdMeas)<-FetPowerTolerance)
	///	powerNow += (speed-spdMeas);
        
        //int8_t diff = (speed-spdMeas);
        //if( diff < 10 )
        //        powerNow = diff*2;
        //else
        //        powerNow = diff;
        
        
        if(powerNow>thismax)
                powerNow = thismax;
	if(powerNow>100)
		powerNow=100;
	if(powerNow<0)
		powerNow=0;
	///if(abs(powerNow)>abs(speed*speed*speed))
	///{
	///	powerNow = speed*speed*speed;
	///}
        
	return powerNow;
}


uint8_t Channelselect = 0;
#define TimeDiff	1

#define postscaler  32
uint8_t postsclctr = 0;

///ISR (TIMER0_COMP_vect)  // Wird alle 0.1ms aufgerufen
void pwm (void)
{
        
        static uint8_t FET1_EN=True, FET2_EN=True, FET3_EN=True, FET4_EN=True;
        
        // ********************** PWM start ***********
                static uint8_t pwmCtr=0;
                
                pwmCtr+=5;
                if(pwmCtr>100)
                        pwmCtr=0;
                
                ////// FET 1
                if(FET1_EN)
                {
                        if(pwmCtr<power_Fet1)
                                Set(FET_PORT, FET_1);
                        else
                                Clear(FET_PORT, FET_1);
                }
                        
                ////// FET 2
                if(FET2_EN)
                {
                        if(pwmCtr<power_Fet2)
                                Set(FET_PORT, FET_2);
                        else
                                Clear(FET_PORT, FET_2);
                }
                        
                ////// FET 3
                if(FET3_EN)
                {
                        if(pwmCtr<power_Fet3)
                                Set(FET_PORT, FET_3);
                        else
                                Clear(FET_PORT, FET_3);
                }
                
                if(FET4_EN)
                {
                ////// FET 4
                        if(pwmCtr<power_Fet4)
                                Set(FET_PORT, FET_4);
                        else
                                Clear(FET_PORT, FET_4);
                }
                        
                ////// A
                if(power_A>0)
                {
                        Set(CTRL_PORT, CTRL_A1);
                        Clear(CTRL_PORT, CTRL_A2);
                }
                else
                {
                        Set(CTRL_PORT, CTRL_A2);
                        Clear(CTRL_PORT, CTRL_A1);
                }
                if(pwmCtr<abs(power_A))
                        Set(EN_PORT, EN_A);
                else
                        Clear(EN_PORT, EN_A);
                        
                ////// B
                if(power_B>0)
                {
                        Set(CTRL_PORT, CTRL_B1);
                        Clear(CTRL_PORT, CTRL_B2);
                }
                else
                {
                        Set(CTRL_PORT, CTRL_B2);
                        Clear(CTRL_PORT, CTRL_B1);
                }
                if(pwmCtr<abs(power_B))
                        Set(EN_PORT, EN_B);
                else
                        Clear(EN_PORT, EN_B);
                
        // ********************** PWM stop ************
        
        postsclctr++;
        if(postscaler != postsclctr)
                return;
        
        postsclctr=0;
        
        // ********************** SpeedMeas start *****
        
                // Statemachine
                static uint8_t SM_Pos = 0;
                static uint8_t SM_Pos_old = -1;
                static uint8_t SM_Pos_changed = True;
                static uint16_t TimerCTR = 0;
                static int8_t powerBackup = 0;
                
                #define SM_FET1_halt               0
                #define SM_FET1_wait               1
                #define SM_FET1_start_ADC          2
                #define SM_FET1_measure            3
                                  
                #define SM_FET2_halt               4
                #define SM_FET2_wait               5
                #define SM_FET2_start_ADC          6
                #define SM_FET2_measure            7
                                          
                #define SM_FET3_halt               8
                #define SM_FET3_wait               9
                #define SM_FET3_start_ADC          10
                #define SM_FET3_measure            11
                                      
                #define SM_FET4_halt               12
                #define SM_FET4_wait               13
                #define SM_FET4_start_ADC          14
                #define SM_FET4_measure            15
                
                #define SM_A_halt                  16
                #define SM_A_wait                  17
                #define SM_A_start_ADC             18
                #define SM_A_measure               19
                                                   
                #define SM_B_halt                  20
                #define SM_B_wait                  21
                #define SM_B_start_ADC             22
                #define SM_B_measure               23
                
                #define SM_Pause                   24
                
                #define StateJustSwitched (SM_Pos-SM_Pos_old)
                
                #define DelayToMeasureBackEMF   100
                
                float adc;
                SM_Pos_old = SM_Pos;
                switch(SM_Pos)
                {
                        // FET 1
                                case SM_FET1_halt:
                                        powerBackup=power_Fet1;
                                        FET1_EN = False;
                                        power_Fet1 = 0;
                                        Clear(FET_PORT, FET_1);
                                        //Clear(LED_PORT, LED_RD);
                                        SM_Pos++;
                                        break;
                                case SM_FET1_wait:
                                        if(SM_Pos_changed)
                                            TimerCTR = 0;
                                        TimerCTR++;
                                        if(TimerCTR>(DelayToMeasureBackEMF/100)) // 10*0.1ms = 1ms
                                                SM_Pos++;
                                        break;
                                case SM_FET1_start_ADC:
                                        //Set(LED_PORT, LED_RD);
                                        ADC_BG_START(&ADC_FET1);
                                        SM_Pos++;
                                        break;
                                case SM_FET1_measure:
                                        if(ADC_BG_MEASDONE())
                                        {
                                                adc = ADC_BG_GETVALUE(&ADC_FET1)*(1+speedCorrFet1);
                                                if(adc < 2)
                                                        adc = 100;
                                                else
                                                        adc = 100-adc;
                                                adc = (spdmeas_Fet1*3+adc)/4;      //////////////////////////////////////////////////////////////7
                                                accl_Fet1 = (3*accl_Fet1+(adc-spdmeas_Fet1)*10)/4;
                                                spdmeas_Fet1 = adc;
                                                power_Fet1 = calcPowerFET(powerBackup, spdmeas_Fet1, speed_Fet1, accl_Fet1);
                                                FET1_EN=True;
                                                SM_Pos++;
                                        }
                                        break;
                                
                        // FET 2
                                case SM_FET2_halt:
                                        powerBackup=power_Fet2;
                                        FET2_EN = False;
                                        power_Fet2 = 0;
                                        Clear(FET_PORT, FET_2);
                                        //Clear(LED_PORT, LED_RD);
                                        SM_Pos++;
                                        break;
                                case SM_FET2_wait:
                                        if(SM_Pos_changed)
                                            TimerCTR = 0;
                                        TimerCTR++;
                                        if(TimerCTR>(DelayToMeasureBackEMF/100)) // 10*0.1ms = 1ms
                                                SM_Pos++;
                                        break;
                                case SM_FET2_start_ADC:
                                        //Set(LED_PORT, LED_RD);
                                        ADC_BG_START(&ADC_FET2);
                                        SM_Pos++;
                                        break;
                                case SM_FET2_measure:
                                        if(ADC_BG_MEASDONE())
                                        {
                                                adc = ADC_BG_GETVALUE(&ADC_FET2)*(1+speedCorrFet2);
                                                if(adc < 2)
                                                        adc = 100;
                                                else
                                                        adc = 100-adc;
                                                adc = (spdmeas_Fet2*3+adc)/4;      //////////////////////////////////////////////////////////////7
                                                accl_Fet2 = (3*accl_Fet2+(adc-spdmeas_Fet2)*10)/4;
                                                spdmeas_Fet2 = adc;
                                                power_Fet2 = calcPowerFET(powerBackup, spdmeas_Fet2, speed_Fet2, accl_Fet2);
                                                FET2_EN=True;
                                                SM_Pos++;
                                        }
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
} // */

/*
int main (void)
{
        safe_Set(FET_DDR, FET_1);
        safe_Set(FET_PORT, FET_1);
        while(True);
        return 0;
}*/
/*
int main (void)
{
        Set(FET_DDR, FET_1 | FET_2 | FET_3 | FET_4);
        Clear(FET_PORT, FET_1 | FET_2 | FET_3 | FET_4);
        
        Set(LED_DDR, LED);
        while(True)
        {
                Toggle(LED_PORT, LED);
                Set(FET_PORT, FET_4);
                _delay_ms(1000);
                Clear(FET_PORT, FET_4);
                _delay_ms(1000);
        }
        return 0;
}
//*/
