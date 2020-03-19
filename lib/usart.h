/* ****************************************************************************
   *                                                                          *
   * Name    : Basic USART control routines - header file                     *
   * Author  : saper_2                                                        *
   * Contact :          @gmail.com                                            *
   * Date    : 2016.11.28                                                     *
   * Version : 0.1                                                            *
   *                                                                          *
   * More    : Check out main file (*.c / *.cpp)                              *
   *                                                                          *
   **************************************************************************** */
#ifndef USART_H_INCLUDED_
#define USART_H_INCLUDED_

// UBRR = ( F_CPU / (16 * BAUD) ) - 1
/* 
SOME USEFUL UBRR VALUES FOR DIFFRENT F_CPU:

 F_CPU  |  2X   | BAUD RATE | UBRR | ERROR |
 [MHz]  | SPEED |   [bps]   |      |  [%]  |
--------+-------+-----------+------+-------+  
 4,0000 |   0   |     9 600 |   25 |  0,2  |
 4,0000 |   0   |    19 200 |   12 |  0,2  |
 8,0000 |   0   |    38 400 |   12 |  0,2  |
14,7456 |   0   |   115 200 |    7 |  0,0  |
16,0000 |   0   |    38 400 |   25 |  0,2  |
        |       |           |      |       |

*/

// usart flags - enablers ( UCSRB reg)
#define USART_RX_ENABLE 0x10 // [4] RXENn:UCSRnB
#define USART_TX_ENABLE 0x08 // [3] TXENn:UCSRnB
#define USART_RX_INT_COMPLET 0x80 // [7] RXCIEn:UCSRnB
#define USART_TX_INT_COMPLET 0x40 // [6] TXCIEn:UCSRnB
#define USART_DATA_EMPTY_INT 0x20 // [5] UDRIEn:UCSRnB

#define USART_MODE_2X_SPEED 0x02  // this doubles speed of usart | [1] U2Xn:UCSRnA
#define USART_MODE_MPCM 0x01  // this enables multiprocessor communication mode - check datasheet :) | [0] MPCMn:UCSRnA

// USART MODE ( UCSRC reg.)
#define USART_FRAME_5BIT 0x00
#define USART_FRAME_6BIT 0x02
#define USART_FRAME_7BIT 0x04
#define USART_FRAME_8BIT 0x06

#define USART_PARITY_NONE 0x00
#define USART_PARITY_EVEN 0x20
#define USART_PARITY_ODD 0x30

#define USART_STOP_BITS1 0x00
#define USART_STOP_BITS2 0x08

#define USART_MODE_ASYNC 0x00
#define USART_MODE_SYNC 0x40
#define USART_MODE_MASTER_SPI 0xC0 // UMSELn1:0=11 - check ATMega644p datasheet
// synchronous usart mode only
#define USART_MODE_SYNC_XCK_RISING_FALLING 0x00
#define USART_MODE_SYNC_XCK_FALLING_RISING 0x01

// defined modes templates :D
#define USART_MODE_8N1 USART_FRAME_8BIT|USART_PARITY_NONE|USART_STOP_BITS1

// 
void usart_config(uint16_t ubrr, uint8_t flags_enablers, uint8_t flags_mode);
void usart_send_char(uint8_t c);

// comment out unwanted functions
#define USART_SEND_STRING
#define USART_SEND_PROG_STRING
#define USART_SEND_INT
#define USART_SEND_UNSIGNED_INT
//#define USART_SEND_LONG
//#define USART_SEND_UNSIGNED_LONG
#define USART_SEND_HEX_BYTE
#define USART_SEND_BIN_BYTE


#if defined(USART_SEND_STRING) || defined(USART_SEND_PROG_STRING) \
	|| defined(USART_SEND_INT) || defined(USART_SEND_UNSIGNED_INT) || defined(USART_SEND_LONG) || defined(USART_SEND_UNSIGNED_LONG) 
void usart_send_str(char* str);
#endif

#if defined(USART_SEND_PROG_STRING)
void usart_send_strP(const char* str);
#endif

#if defined(USART_SEND_INT)
void usart_send_int(int val);
#endif

#if defined(USART_SEND_UNSIGNED_INT)
void usart_send_uint(unsigned int val);
#endif

#if defined(USART_SEND_LONG)
void usart_send_long(long val);
#endif

#if defined(USART_SEND_UNSIGNED_LONG) 
void usart_send_ulong(unsigned long val);
#endif


#if defined(USART_SEND_HEX_BYTE)
void usart_send_hex_byte(uint8_t data);
#endif

#if defined(USART_SEND_BIN_BYTE)
void usart_send_bin_byte(uint8_t bin);
#endif

#endif
