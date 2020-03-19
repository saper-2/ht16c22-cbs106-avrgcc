/*
 * cbs106.h
 *
 * Created: 2017-12-30 16:21:33
 *  Author: saper
 */ 


#ifndef CBS106_H_
#define CBS106_H_

#define CBS106_DEBUG 0

#include "ht16c22.h"
#if CBS106_DEBUG>0
	#include "usart.h"
#endif


#if defined(HT16C22_SRAM_BUFF)
	#error SRAM framebuffer not supported by overlay driver for CBS106 panel.
#endif


#define CBS_BL_PN 3
#define CBS_BL_PORT PORTC
#define CBS_BL_DDR  DDRC

#define cbs_backlight_setup_port CBS_BL_DDR |= 1<<CBS_BL_PN
#define cbs_backlight_on  CBS_BL_PORT |=   1<<CBS_BL_PN
#define cbs_backlight_off CBS_BL_PORT &= ~(1<<CBS_BL_PN)


/* 
	Icon value format:
	bits [7:3] = HT16C22 ram address (0x00..0x11)
	bits [2:0] = COMx bit number in ram byte
	
	e.g.:  icon "start-arrow"
		COM=1
		SEG=28
		HT16C22: SEG28 is located at RAM address 0x0E (14dec) , COM1 is bit 1. ADDR=0x0E , BIT=1
		So: VALUE=(ADDR<<3) | BIT   =>   0x71 = (0x0E << 3) | 0x01 = 0x70 | 0x01
*/
#define CBS_ICON_PROGRAM 0x01 // COM-SEG: 1-0  / RAM_ADDR-BIT:  0 1 = 1 dec
#define CBS_ICON_KEY_STBY_RECT 0x02 // COM-SEG: 2-0  / RAM_ADDR-BIT:  0 2 = 2 dec -rec1
#define CBS_ICON_STANDBY 0x00 // COM-SEG: 0-0  / RAM_ADDR-BIT:  0 0 = 0 dec
#define CBS_ICON_WAIT 0x08 // COM-SEG: 0-2  / RAM_ADDR-BIT:  1 0 = 8 dec
#define CBS_ICON_KEY_OK 0x0A // COM-SEG: 2-2  / RAM_ADDR-BIT:  1 2 = 10 dec
#define CBS_ICON_KEY_OK_RECT 0x09 // COM-SEG: 1-2  / RAM_ADDR-BIT:  1 1 = 9 dec -rec2
#define CBS_ICON_SELECT 0x05 // COM-SEG: 1-1  / RAM_ADDR-BIT:  0 5 = 5 dec
#define CBS_ICON_NEW 0x06 // COM-SEG: 2-1  / RAM_ADDR-BIT:  0 6 = 6 dec
#define CBS_ICON_LARGE_ARROWS 0x04 // COM-SEG: 0-1  / RAM_ADDR-BIT:  0 4 = 4 dec
#define CBS_ICON_DELETE 0x0D // COM-SEG: 1-3  / RAM_ADDR-BIT:  1 5 = 13 dec
#define CBS_ICON_ECO_CIRCLE 0x0E // COM-SEG: 2-3  / RAM_ADDR-BIT:  1 6 = 14 dec -cut circle
#define CBS_ICON_ECO 0x0C // COM-SEG: 0-3  / RAM_ADDR-BIT:  1 4 = 12 dec
#define CBS_ICON_WEEK 0x26 // COM-SEG: 2-9  / RAM_ADDR-BIT:  4 6 = 38 dec
#define CBS_ICON_W1_MON 0x12 // COM-SEG: 2-4  / RAM_ADDR-BIT:  2 2 = 18 dec
#define CBS_ICON_W2_TUE 0x10 // COM-SEG: 0-4  / RAM_ADDR-BIT:  2 0 = 16 dec
#define CBS_ICON_W3_WED 0x76 // COM-SEG: 2-29  / RAM_ADDR-BIT:  14 6 = 118 dec
#define CBS_ICON_W4_THU 0x5A // COM-SEG: 2-22  / RAM_ADDR-BIT:  11 2 = 90 dec
#define CBS_ICON_W5_FRI 0x52 // COM-SEG: 2-20  / RAM_ADDR-BIT:  10 2 = 82 dec
#define CBS_ICON_W6_SAT 0x2A // COM-SEG: 2-10  / RAM_ADDR-BIT:  5 2 = 42 dec
#define CBS_ICON_W7_SUN 0x2E // COM-SEG: 2-11  / RAM_ADDR-BIT:  5 6 = 46 dec
#define CBS_ICON_CLOCK 0x11 // COM-SEG: 1-4  / RAM_ADDR-BIT:  2 1 = 17 dec
#define CBS_ICON_TEMP 0x7E // COM-SEG: 2-31  / RAM_ADDR-BIT:  15 6 = 126 dec
#define CBS_ICON_SCALE 0x66 // COM-SEG: 2-25  / RAM_ADDR-BIT:  12 6 = 102 dec
#define CBS_ICON_START_ARROW 0x72 // COM-SEG: 2-28  / RAM_ADDR-BIT:  14 2 = 114 dec
#define CBS_ICON_STOP_ARROW 0x5E // COM-SEG: 2-23  / RAM_ADDR-BIT:  11 6 = 94 dec
#define CBS_ICON_SMALL_DP 0x6A // COM-SEG: 2-26  / RAM_ADDR-BIT:  13 2 = 106 dec -col
#define CBS_ICON_SMALL_DEG_C 0x46 // COM-SEG: 2-17  / RAM_ADDR-BIT:  8 6 = 70 dec
#define CBS_ICON_KWH 0x4E // COM-SEG: 2-19  / RAM_ADDR-BIT:  9 6 = 78 dec
#define CBS_ICON_LARGE_DEG_C 0x3E // COM-SEG: 2-15  / RAM_ADDR-BIT:  7 6 = 62 dec
#define CBS_ICON_BOILER 0x2C // COM-SEG: 0-11  / RAM_ADDR-BIT:  5 4 = 44 dec
#define CBS_ICON_FIRE 0x2D // COM-SEG: 1-11  / RAM_ADDR-BIT:  5 5 = 45 dec
#define CBS_ICON_SMALL_ARROWS 0x38 // COM-SEG: 0-14  / RAM_ADDR-BIT:  7 0 = 56 dec
#define CBS_ICON_SMALL_CLOCK 0x39 // COM-SEG: 1-14  / RAM_ADDR-BIT:  7 1 = 57 dec
#define CBS_ICON_KEY_PLUS_RECT 0x3A // COM-SEG: 2-14  / RAM_ADDR-BIT:  7 2 = 58 dec -rec4
#define CBS_ICON_KEY_PLUS_ARROW 0x35 // COM-SEG: 1-13  / RAM_ADDR-BIT:  6 5 = 53 dec -up arrow
#define CBS_ICON_KEY_PLUS 0x34 // COM-SEG: 0-13  / RAM_ADDR-BIT:  6 4 = 52 dec
#define CBS_ICON_RESET 0x36 // COM-SEG: 2-13  / RAM_ADDR-BIT:  6 6 = 54 dec
#define CBS_ICON_KEY_MINUS_RECT 0x32 // COM-SEG: 2-12  / RAM_ADDR-BIT:  6 2 = 50 dec -rec3
#define CBS_ICON_KEY_MINUS 0x30 // COM-SEG: 0-12  / RAM_ADDR-BIT:  6 0 = 48 dec
#define CBS_ICON_KEY_MINUS_ARROW 0x31 // COM-SEG: 1-12  / RAM_ADDR-BIT:  6 1 = 49 dec -down arrow
#define CBS_ICON_CLOCK_HH_1 0x1D // COM-SEG: 1-7  / RAM_ADDR-BIT:  3 5 = 29 dec
#define CBS_ICON_CLOCK_HH_2 0x81 // COM-SEG: 1-32  / RAM_ADDR-BIT:  16 1 = 129 dec
#define CBS_ICON_CLOCK_HH_3 0x80 // COM-SEG: 0-32  / RAM_ADDR-BIT:  16 0 = 128 dec
#define CBS_ICON_CLOCK_HH_4 0x82 // COM-SEG: 2-32  / RAM_ADDR-BIT:  16 2 = 130 dec
#define CBS_ICON_CLOCK_HH_5 0x8E // COM-SEG: 2-35  / RAM_ADDR-BIT:  17 6 = 142 dec
#define CBS_ICON_CLOCK_HH_6 0x8C // COM-SEG: 0-35  / RAM_ADDR-BIT:  17 4 = 140 dec
#define CBS_ICON_CLOCK_HH_7 0x8D // COM-SEG: 1-35  / RAM_ADDR-BIT:  17 5 = 141 dec
#define CBS_ICON_CLOCK_HH_8 0x19 // COM-SEG: 1-6  / RAM_ADDR-BIT:  3 1 = 25 dec
#define CBS_ICON_CLOCK_HH_9 0x18 // COM-SEG: 0-6  / RAM_ADDR-BIT:  3 0 = 24 dec
#define CBS_ICON_CLOCK_HH_10 0x1A // COM-SEG: 2-6  / RAM_ADDR-BIT:  3 2 = 26 dec
#define CBS_ICON_CLOCK_HH_11 0x1E // COM-SEG: 2-7  / RAM_ADDR-BIT:  3 6 = 30 dec
#define CBS_ICON_CLOCK_HH_12 0x1C // COM-SEG: 0-7  / RAM_ADDR-BIT:  3 4 = 28 dec
#define CBS_ICON_CLOCK_MH_1 0x21 // COM-SEG: 1-8  / RAM_ADDR-BIT:  4 1 = 33 dec
#define CBS_ICON_CLOCK_MH_2 0x85 // COM-SEG: 1-33  / RAM_ADDR-BIT:  16 5 = 133 dec
#define CBS_ICON_CLOCK_MH_3 0x84 // COM-SEG: 0-33  / RAM_ADDR-BIT:  16 4 = 132 dec
#define CBS_ICON_CLOCK_MH_4 0x86 // COM-SEG: 2-33  / RAM_ADDR-BIT:  16 6 = 134 dec
#define CBS_ICON_CLOCK_MH_5 0x8A // COM-SEG: 2-34  / RAM_ADDR-BIT:  17 2 = 138 dec
#define CBS_ICON_CLOCK_MH_6 0x88 // COM-SEG: 0-34  / RAM_ADDR-BIT:  17 0 = 136 dec
#define CBS_ICON_CLOCK_MH_7 0x89 // COM-SEG: 1-34  / RAM_ADDR-BIT:  17 1 = 137 dec
#define CBS_ICON_CLOCK_MH_8 0x15 // COM-SEG: 1-5  / RAM_ADDR-BIT:  2 5 = 21 dec
#define CBS_ICON_CLOCK_MH_9 0x14 // COM-SEG: 0-5  / RAM_ADDR-BIT:  2 4 = 20 dec
#define CBS_ICON_CLOCK_MH_10 0x16 // COM-SEG: 2-5  / RAM_ADDR-BIT:  2 6 = 22 dec
#define CBS_ICON_CLOCK_MH_11 0x22 // COM-SEG: 2-8  / RAM_ADDR-BIT:  4 2 = 34 dec
#define CBS_ICON_CLOCK_MH_12 0x20 // COM-SEG: 0-8  / RAM_ADDR-BIT:  4 0 = 32 dec

// key bit definitions for AT42QT1070 touch module
#define CBS_KEY_UP 0x10
#define CBS_KEY_DOWN 0x04
#define CBS_KEY_BOILER 0x08
#define CBS_KEY_CLOCK 0x40
#define CBS_KEY_ECO 0x01
#define CBS_KEY_OK 0x02
#define CBS_KEY_STANDBY 0x20

// number definitions for cbs106_digit
// digits 0..9 are 0..9
#define CBS_DIGIT_A 0x0a
#define CBS_DIGIT_b 0x0b
#define CBS_DIGIT_c 0x0c
#define CBS_DIGIT_d 0x0d
#define CBS_DIGIT_E 0x0e
#define CBS_DIGIT_F 0x0f
#define CBS_DIGIT_C 0x10
#define CBS_DIGIT_u 0x11
#define CBS_DIGIT_U 0x12
#define CBS_DIGIT_r 0x13
#define CBS_DIGIT_o 0x14
#define CBS_DIGIT_DEG 0x15
#define CBS_DIGIT_t 0x16
#define CBS_DIGIT_j 0x17
#define CBS_DIGIT_L 0x18
#define CBS_DIGIT_H 0x19
#define CBS_DIGIT_P 0x1a
#define CBS_DIGIT_SPACE 0x1b
#define CBS_DIGIT_MINUS 0x1c
#define CBS_DIGIT_UNDERSCORE 0x1d
//#define CBS_DIGIT_ 0x

#define cbs_setup ht16c22_setup

#define cbs_clear ht16c22_clear
#define cbs_set_contrast ht16c22_set_bias(0x00) // best setting IVA: DA[3:0]=0x0

// Show digit (or letter) on digit display: digit=digit number[0..5], val=digit value[0..9A..F,...] see "cbs_digits" array
void cbs106_digit(uint8_t digit, uint8_t val);

void cbs106_icon(uint8_t icon, uint8_t state);
uint8_t cbs106_icon_get(uint8_t icon);

void cbs106_clock_face(uint8_t hh, uint8_t mh);
void cbs106_clock_face_fill(uint8_t hhs, uint8_t hhe, uint8_t mhs, uint8_t mhe);

void cbs106_weekday(uint8_t v);

void cbs106_small_clock(uint8_t h, uint8_t m);
void cbs106_small_num(int16_t num);
void cbs106_big_num(int8_t num);


#endif /* CBS106_H_ */