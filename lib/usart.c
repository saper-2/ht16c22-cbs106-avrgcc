/* ****************************************************************************
   *                                                                            *
   * Name      : Basic USART control routines                                   *
   * Author    : saper_2                                                        *
   * Contact   :          @gmail.com                                            *
   * Date      : 2016.11.28                                                     *
   * Version   : 0.2                                                            *
   * License   : You are free to use this library in any kind project, but at   *
   *             least send me a info that YOU are using it. I'll be happy      *
   *             for memory :)                                                  *
   * About     :                                                                *
   *             Some basic functions for controling single USART               *
   *                                                                            *
   * Bugs      :                                                                *
   *                                                                            *
   * Changelog :                                                                *
   *             - v0.2 - Added ATMega644P USART0 support                       *
   *             - v0.1 - initial release supporting ATMega32                   *
   *                                                                            *
   *                                                                            *
   **************************************************************************** */

#include <avr/io.h>
#include <inttypes.h>
#include <avr/pgmspace.h>

#include "usart.h"

#if defined(USART_SEND_INT) || defined(USART_SEND_UNSIGNED_INT) || defined(USART_SEND_LONG) || defined(USART_SEND_UNSIGNED_LONG) 
	#include <stdlib.h>
#endif
//#include <avr/interrupt.h>

/* 
-----------------------------------
// UBRR = ( F_CPU / (16 * BAUD) ) - 1
*/


void usart_config(uint16_t ubrr, uint8_t flags_enablers, uint8_t flags_mode) {
	
	#if defined(__AVR_ATmega32__) || defined(__AVR_ATmega8__)
		UDR = 0x00; // reset data in udr
		UBRRL = ubrr; // Assign UBRRL
		UBRRH = 0x0F & (ubrr>>8); // Assign UBRRH, URSEL must be 0 for writing into UBRRH
		UCSRA = 0x03 & flags_enablers;
		UCSRB = 0xF8 & flags_enablers;
		UCSRC = (1<<URSEL) | flags_mode;
	#elif defined(__AVR_ATmega644P__)
		UDR0 = 0x00; // reset data in udr
		UBRR0L = ubrr; // Assign UBRRL
		UBRR0H = 0x0F & (ubrr>>8); // Assign UBRRH
		UCSR0A = 0x03 & flags_enablers;
		UCSR0B = 0xF8 & flags_enablers;
		UCSR0C = flags_mode;
	#else
		#error Selected MCU is not supported or not added as supported. [usart_config]
	#endif
}

// most basic transmision function
void usart_send_char(uint8_t c) {
	#if defined(__AVR_ATmega32__) || defined(__AVR_ATmega8__)
		// wait until UDR will be empty
		while ( ! (UCSRA & (1 << UDRE) ) ) { }
		// insert char into UDR (this oper. sends char)
		UDR = c;
	#elif defined(__AVR_ATmega644P__)
		// wait until UDR0 will be empty
		while ( ! (UCSR0A & (1 << UDRE0) ) ) { }
		// insert char into UDR (this oper. sends char)
		UDR0 = c;
	#else
		#error Selected MCU is not supported or not added as supported. [usart_send_char]
	#endif
	
}

#if defined(USART_SEND_STRING) || defined(USART_SEND_PROG_STRING) \
	|| defined(USART_SEND_INT) || defined(USART_SEND_UNSIGNED_INT) || defined(USART_SEND_LONG) || defined(USART_SEND_UNSIGNED_LONG) 
void usart_send_str(char* str) {
	char znak;
	while(0 != (znak = *(str++))) usart_send_char(znak);
}
#endif

#if defined(USART_SEND_PROG_STRING)
void usart_send_strP(const char* str) {
	char znak;
	while (0 != (znak = pgm_read_byte(str++))) usart_send_char(znak);
}
#endif

#if defined(USART_SEND_INT)
void usart_send_int(int val) {
	char bufor[7];
	usart_send_str(itoa(val, bufor, 10));
}
#endif

#if defined(USART_SEND_UNSIGNED_INT)
void usart_send_uint(unsigned int val) {
	char bufor[6];
	usart_send_str(utoa(val, bufor, 10));
}
#endif

#if defined(USART_SEND_LONG)
void usart_send_long(long val) {
	char bufor[12];
	usart_send_str(ltoa(val, bufor, 10));
}
#endif

#if defined(USART_SEND_UNSIGNED_LONG) 
void usart_send_ulong(unsigned long val) {
	char bufor[11];
	usart_send_str(ultoa(val, bufor, 10));
}
#endif


#if defined(USART_SEND_HEX_BYTE)
const uint8_t usart_CharTab[16] PROGMEM = { 
											0x30, 0x31, 0x32, 0x33, 
											0x34, 0x35, 0x36, 0x37, 
											0x38, 0x39, 0x41, 0x42, 
											0x43, 0x44, 0x45, 0x46 };


uint8_t usartDecToHex(uint8_t z) {
	z &= 0x0f;
	return pgm_read_byte(&usart_CharTab[z]);
}

void usart_send_hex_byte(uint8_t data) {
	usart_send_char(usartDecToHex((data >> 4) & 0x0f));
	usart_send_char(usartDecToHex(data & 0x0f));
}
#endif

#if defined(USART_SEND_BIN_BYTE)
void usart_send_bin_byte(uint8_t bin) {
	uint8_t i;
	for (i=0;i<8;i++) {
		if (bin & 0x80) usart_send_char('1'); else usart_send_char('0');
		bin <<= 1;
	}
}
#endif
