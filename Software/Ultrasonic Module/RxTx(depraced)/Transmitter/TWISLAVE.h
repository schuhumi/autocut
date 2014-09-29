// Codesource: http://rn-wissen.de/wiki/index.php/TWI_Slave_mit_avr-gcc

#ifndef _TWISLAVE_H
#define _TWISLAVE_H

#include <util/twi.h> 		  //enthaelt z.B. die Bezeichnungen fuer die Statuscodes in TWSR
#include <avr/interrupt.h>  //dient zur behandlung der Interrupts
#include <stdint.h> 		    //definiert den Datentyp uint8_t


/** 
 *  @defgroup twislave TWI-Slave
 *  @code #include "twislave.h" @endcode
 * 
 *  @brief Betrieb eines AVRs mit Hardware-TWI-Schnittstelle als Slave.
 *  Zu Beginn muss init_twi_slave mit der gewuenschten Slave-Adresse als
 *  Parameter aufgerufen werden.
 *
 * Der Datenaustausch mit dem Master erfolgt ueber den Buffer i2cdata, 
 * auf den von Master und Slave zugegriffen werden kann. 
 * Dies ist fuer den Slave eine globale Variable (Array aus uint8_t). 

 * Der Zugriff durch den Master erfolgt aehnlich wie bei einem
 * normalen I2C-EEPROM.
 * Man sendet zunaechst die Bufferposition, an die man schreiben will,
 * und dann die Daten. 
 * Die Bufferposition wird automatisch hochgezaehlt, sodass man mehrere
 * Datenbytes hintereinander schreiben kann, ohne jedesmal die 
 * Bufferadresse zu schreiben.
 *
 * Um vom Master aus zu lesen, uebertraegt man zunaechst in einem 
 * Schreibzugriff die gewuenschte Bufferposition und liest dann nach
 * einem repeated start die Daten aus. Die Bufferposition wird 
 * automatisch hochgezaehlt, sodass man mehrere Datenbytes
 * hintereinander lesen kann, ohne jedesmal die Bufferposition zu
 * schreiben.
 *
 * Abgefangene Fehlbedienung durch den Master:
 * - Lesen ueber die Grenze des txbuffers hinaus
 * - Schreiben ueber die Grenzen des rxbuffers hinaus
 * - Angabe einer ungueltigen Schreib/Lese-Adresse
 * - Lesezugriff, ohne vorher Leseadresse geschrieben zu haben
 * 
 *  @author uwegw
 */

/*@{*/

//%%%%%%%% von Benutzer konfigurierbare Einstellungen %%%%%%%%
/**@brief Groesse des Buffers in Byte (2..254) */
#define i2c_buffer_size 25// I2C_REG_ANZAHL 254 Hier kann eingestellt werden wieviele Register ausgegeben werden


//%%%%%%%% Globale Variablen, die vom Hauptprogramm genutzt werden %%%%%%%%
/**@brief Der Buffer, in dem die Daten gespeichert werden.
 * Aus Sicht des Masters laeuft der Zugrif auf den Buffer genau wie
 *  bei einem I2C-EEPROm ab.
 * Fuer den Slave ist es eine globale Variable
*/
volatile uint8_t i2cdata[i2c_buffer_size];


/**@brief Initaliserung des TWI-Inteface. Muss zu Beginn aufgerufen werden, sowie bei einem Wechsel der Slave Adresse
 * @param adr gewuenschte Slave-Adresse */
void init_twi_slave(uint8_t adr);

//%%%%%%%% ab hier sind normalerweise keine weiteren Aenderungen erforderlich! %%%%%%%%//
//____________________________________________________________________________________//


//Bei zu alten AVR-GCC-Versionen werden die Interrupts anders genutzt, daher in diesem Fall mit Fehlermeldung abbrechen
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4.5 or later, update to newer AVR-GCC compiler !"
#endif

//Schutz vor unsinnigen Buffergroessen
#if (i2c_buffer_size > 254)
	#error Buffer zu gross gewaehlt! Maximal 254 Bytes erlaubt.
#endif

#if (i2c_buffer_size < 2)
	#error Buffer muss mindestens zwei Byte gross sein!
#endif



#endif //#ifdef _TWISLAVE_H
////Ende von twislave.h////
