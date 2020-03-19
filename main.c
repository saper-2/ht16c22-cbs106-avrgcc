/*
 * ht16c22-cbs106-drv.c
 *
 * Created: 2018-10-06 16:13:31
 * Author : saper
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#include "lib/delay.h"
#include "lib/i2c.h"
#include "lib/usart.h"
#include "lib/at42qt1070.h"
#include "lib/cbs106.h" // this include ht16c22 already.

#define led1_on PORTB |= 0x01
#define led1_off PORTB &= ~(0x01)
#define led1_toggle PORTB ^= (0x01)
#define led2_on PORTB |= 0x02
#define led2_off PORTB &= ~(0x02)
#define led2_toggle PORTB ^= (0x02)
#define led3_on PORTB |= 0x04
#define led3_off PORTB &= ~(0x04)
#define led3_toggle PORTB ^= (0x04)


#define RS_BUFF_SIZE 16
uint8_t rsBuffPtr = 0;
uint8_t rsBuff[RS_BUFF_SIZE];

uint8_t g_mode=0;;

void flushRSBuff(void) {
	for (uint8_t i=0;i<RS_BUFF_SIZE;i++) rsBuff[i]=0;
	rsBuffPtr=0;
	#ifdef DEBUG_MODE
	usart_send_strP(PSTR("<<<RS-FLUSH>>>"));
	#endif
}


uint8_t hex2dec(char hex) {
	if (hex > 0x2f && hex < 0x3a) return hex-0x30;
	if (hex > 0x40 && hex < 0x47) return hex-0x37;
	if (hex > 0x60 && hex < 0x67) return hex-0x57;
	return 0x0f;
}

void processRSBuff(void) {
	uint8_t tmp=0, tmp2=0;
	// all values are in hex 1 [X] or 2[XX] character format unless otherwise noted
	// v[X] - set bias
	// w[XX][XX] - write data byte to ht16c22 at address [ADR][DATA]
	// c - clear
	// C - clear only digits
	// i[XX] - set icon by value
	// b[00000000] - write data byte to ht16c22 from binary input (0 or 1)
	// d[X][XX] - set digit [x] to value [xx]
	// x[x][x] - set clock hands hh and mh in signle-digit-mode (0xC and above hide clock hand)
	// z[x][x][x][x] - set clock hands in range mode, params order (one hex char 0..b): hh-start, hh-end, hm-start, hm-end
	// j[xxxx] - display on small clock a signed int16 value (from 4-char hex)
	// t[xx][xx] - display on small clock a time [hh] (in hex) and [mm] (in hex)
	// ...
	// r - enter remote command mode (touch key wont perform any actions)
	// R - exit remote command mode
	if (rsBuff[0] == 'v') {
		tmp = hex2dec(rsBuff[1]); // 0..15 (0..F)
		ht16c22_set_bias(tmp);
		usart_send_char('v');
		usart_send_hex_byte(tmp&0x0f);
	} else if (rsBuff[0] == '?') {
		usart_send_strP(PSTR("CMDs: v[x],w[xx][xx],c,C,i[xx],b[00000000],d[x][xx],x[x][x],z[x][x][x][x],j[xxxx],t[xx][xx],r,R"));
	} else if (rsBuff[0] == 'w') {
		tmp = hex2dec(rsBuff[1]) << 4 | hex2dec(rsBuff[2]);
		tmp2 = hex2dec(rsBuff[3]) << 4 | hex2dec(rsBuff[4]);
		ht16c22_wr_data1(0x1f&tmp, tmp2);
		usart_send_char('w');
		usart_send_hex_byte(tmp);
		usart_send_hex_byte(tmp2);
	} else if (rsBuff[0] == 'c') {
		ht16c22_clear();
		usart_send_char('c');
	} else if (rsBuff[0] == 'C') {
		cbs106_digit(0, CBS_DIGIT_SPACE);
		cbs106_digit(1, CBS_DIGIT_SPACE);
		cbs106_digit(2, CBS_DIGIT_SPACE);
		cbs106_digit(3, CBS_DIGIT_SPACE);
		cbs106_digit(4, CBS_DIGIT_SPACE);
		cbs106_digit(5, CBS_DIGIT_SPACE);
		usart_send_char('C');
	} else if (rsBuff[0] == 'i') {
		tmp = hex2dec(rsBuff[1]) << 4 | hex2dec(rsBuff[2]);
		tmp2 = hex2dec(rsBuff[3]);
		cbs106_icon(tmp, tmp2);
		usart_send_char('i');
		usart_send_hex_byte(tmp);
		usart_send_hex_byte(tmp2);
	} else if (rsBuff[0] == 'b') {
		tmp = hex2dec(rsBuff[1]) << 4 | hex2dec(rsBuff[2]);
		tmp2=0;
		if (rsBuff[ 3] == '1') tmp2 |= 0x80;
		if (rsBuff[ 4] == '1') tmp2 |= 0x40;
		if (rsBuff[ 5] == '1') tmp2 |= 0x20;
		if (rsBuff[ 6] == '1') tmp2 |= 0x10;
		if (rsBuff[ 7] == '1') tmp2 |= 0x08;
		if (rsBuff[ 8] == '1') tmp2 |= 0x04;
		if (rsBuff[ 9] == '1') tmp2 |= 0x02;
		if (rsBuff[10] == '1') tmp2 |= 0x01;

		ht16c22_wr_data1(tmp, tmp2);

		usart_send_char('b');
		usart_send_hex_byte(tmp);
		usart_send_hex_byte(tmp2);
	} else if (rsBuff[0] == 'd') {
		tmp = hex2dec(rsBuff[1]);
		tmp2 = hex2dec(rsBuff[2]) << 4 | hex2dec(rsBuff[3]);
		
		cbs106_digit(tmp,tmp2);

		usart_send_char('d');
		usart_send_hex_byte(tmp);
		usart_send_hex_byte(tmp2);
	} else if (rsBuff[0] == 'x') {
		// clock face single digit
		tmp = hex2dec(rsBuff[1]);
		tmp2 = hex2dec(rsBuff[2]);

		cbs106_clock_face(tmp, tmp2);

		usart_send_char('x');
		usart_send_hex_byte(tmp);
		usart_send_hex_byte(tmp2);
	} else if (rsBuff[0] == 'z') {
		tmp = hex2dec(rsBuff[1]);
		tmp2 = hex2dec(rsBuff[2]);
		uint8_t tmp3, tmp4;
		tmp3 = hex2dec(rsBuff[3]);
		tmp4 = hex2dec(rsBuff[4]);
		cbs106_clock_face_fill(tmp,tmp2,tmp3,tmp4);
		usart_send_char('z');
		usart_send_hex_byte(tmp);
		usart_send_hex_byte(tmp2);
		usart_send_hex_byte(tmp3);
		usart_send_hex_byte(tmp4);
	} else if (rsBuff[0] == 'j') {
		int16_t v = 0;
		v = ((uint16_t)hex2dec(rsBuff[1]))<<12 |  ((uint16_t)hex2dec(rsBuff[2]))<<8 |  ((uint16_t)hex2dec(rsBuff[3]))<<4 | ((uint16_t)hex2dec(rsBuff[4]));
		cbs106_small_num(v);
		usart_send_char('j');
		usart_send_hex_byte(v>>8);
		usart_send_hex_byte(v);
		usart_send_char('/');
		usart_send_int(v);
	} else if (rsBuff[0] == 'k') {
		int8_t v = 0;
		v = ((uint8_t)hex2dec(rsBuff[1]))<<4 |  ((uint16_t)hex2dec(rsBuff[2]));
		cbs106_big_num(v);
		usart_send_char('k');
		usart_send_hex_byte(v);
		usart_send_char('/');
		usart_send_int(v);
	} else if (rsBuff[0] == 't') {
		tmp = hex2dec(rsBuff[1]) << 4 | hex2dec(rsBuff[2]);
		tmp2 = hex2dec(rsBuff[3]) << 4 | hex2dec(rsBuff[4]);
		cbs106_small_clock(tmp,tmp2);
		
		usart_send_char('t');
		usart_send_int(tmp);
		usart_send_char(':');
		usart_send_int(tmp2);
		
	//} else if (rsBuff[0] == 'c') {
	//} else if (rsBuff[0] == 'c') {
	//} else if (rsBuff[0] == 'c') {
	//} else if (rsBuff[0] == 'c') {
	} else if (rsBuff[0] == 'r') {
		g_mode=200; // remote mode
		cbs106_icon(CBS_ICON_KEY_STBY_RECT, 1);
		cbs106_icon(CBS_ICON_STANDBY, 1);
		cbs106_icon(CBS_ICON_KEY_OK_RECT, 1);
		cbs106_icon(CBS_ICON_KEY_OK, 1);
		cbs106_icon(CBS_ICON_ECO_CIRCLE, 1);
		cbs106_icon(CBS_ICON_ECO, 1);
		cbs106_icon(CBS_ICON_WEEK, 1);
		cbs106_icon(CBS_ICON_CLOCK, 1);
		cbs106_icon(CBS_ICON_BOILER, 1);
		cbs106_icon(CBS_ICON_KEY_PLUS_RECT, 1);
		cbs106_icon(CBS_ICON_KEY_PLUS_ARROW, 1);
		cbs106_icon(CBS_ICON_KEY_MINUS_RECT, 1);
		cbs106_icon(CBS_ICON_KEY_MINUS_ARROW, 1);
		usart_send_char('r');
	} else if (rsBuff[0] == 'R') {
		g_mode=0;
		cbs106_icon(CBS_ICON_KEY_STBY_RECT, 1);
		cbs106_icon(CBS_ICON_STANDBY, 1);
		cbs106_icon(CBS_ICON_KEY_OK_RECT, 1);
		cbs106_icon(CBS_ICON_KEY_OK, 1);
		cbs106_icon(CBS_ICON_ECO_CIRCLE, 1);
		cbs106_icon(CBS_ICON_ECO, 0);
		cbs106_icon(CBS_ICON_WEEK, 1);
		cbs106_icon(CBS_ICON_CLOCK, 1);
		cbs106_icon(CBS_ICON_BOILER, 1);
		cbs106_icon(CBS_ICON_KEY_PLUS_RECT, 1);
		cbs106_icon(CBS_ICON_KEY_PLUS_ARROW, 1);
		cbs106_icon(CBS_ICON_KEY_MINUS_RECT, 1);
		cbs106_icon(CBS_ICON_KEY_MINUS_ARROW, 1);
		usart_send_char('R');
	}

	usart_send_char(0x0d);
	usart_send_char(0x0a);
	
	flushRSBuff();
}



int main(void)
{
    uint8_t i;
	uint8_t t8;//, t82=0, t83=0;
	uint8_t hh, mh, week;
	
	DDRB |= 1<<0 | 1<<1 | 1<<2;
	led1_off; led2_off; led3_off;
	
	flushRSBuff();
	usart_config(25, USART_RX_ENABLE|USART_TX_ENABLE|USART_RX_INT_COMPLET, USART_MODE_8N1);
	sei();
	//usart_config(25, USART_TX_ENABLE | USART_RX_ENABLE , USART_MODE_8N1);//| USART_RX_INT_COMPLET
	usart_send_strP(PSTR("\r\nLCD-CBS106BB00 module test start...\r\n"));

	i2c_init();
	/*
	// search I2C
	usart_send_strP(PSTR("I2C bus scan...\r\n"));
	usart_send_strP(PSTR("     0  2  4  6  8  A  C  E\r\n00: "));
	for(uint16_t i16=0;i16<256;i16+=2) {
		i2c_start();
		t8 = i2c_send(i16);
		i2c_stop();
		if (t8 == 0) {
			if (i16==0x36) t82 |= 0x01; // mark AT42Q1070 presence
			if (i16==0x7e) t82 |= 0x02; // mark HT16C22 presence
			usart_send_hex_byte(i16);
			usart_send_char(' ');
		} else {
			usart_send_char('-');
			usart_send_char('-');
			usart_send_char(' ');
		}
		
		t83++;
		if (((t83%8) == 0) && (i16 < 254)) {
			usart_send_char(0x0d);
			usart_send_char(0x0a);
			usart_send_hex_byte(i16+2);
			usart_send_char(':');
			usart_send_char(' ');
		}
		delay1ms(10);
		
	}
	usart_send_strP(PSTR("\r\nI2C scan end.\r\n"));
	*/
	
	// if ST42QT1070 is present
	i2c_start();
	t8 = i2c_send(AT42QT1070_ADDR);
	i2c_stop();
	if (t8  == 0x00) {
		led1_on;
		// get chip info
		atq1070_read_id(&i, &t8);
		usart_send_strP(PSTR("AT42QT1070 found.\r\nChip ID="));
		usart_send_hex_byte(i);
		usart_send_strP(PSTR("\r\nFW Ver="));
		usart_send_hex_byte(t8);
		usart_send_strP(PSTR("\r\nDetection status=0x"));
		i = atq1070_status();
		usart_send_hex_byte(i);
		usart_send_strP(PSTR(" 0b"));
		usart_send_bin_byte(i);
		// setup
		for (i=0;i<7;i++) {
			atq1070_set_nthr(i, 20);
			atq1070_set_di(i, 30);
		}
		atq1070_set_ave_aks(0, 0x30 | 0x01);
		atq1070_set_ave_aks(1, 0x30 | 0x01);
		atq1070_set_ave_aks(2, 0x30 | 0x02);
		atq1070_set_ave_aks(3, 0x30 | 0x02);
		atq1070_set_ave_aks(4, 0x30 | 0x02);
		atq1070_set_ave_aks(5, 0x30 | 0x03);
		atq1070_set_ave_aks(6, 0x30 | 0x03);

		usart_send_char(0x0d);
		usart_send_char(0x0a);
	} else {
		while(1) {
			led1_toggle;
			led3_toggle;
			delay1ms(250);
		}
	}
	
	// check if HT16C22 is present
	i2c_start();
	t8 = i2c_send(HT16C22_I2C_ADDR);
	i2c_stop();
	if (t8 == 0x00) {
		led2_on;
		usart_send_strP(PSTR("HT16C22 found.\r\n"));
		// cbs_setup();
	} else {
		while(1) {
			led2_toggle;
			led3_toggle;
			delay1ms(250);
		}
	}

	cbs_backlight_setup_port;
	cbs_backlight_on;
	cbs_setup();
	cbs_set_contrast;
	cbs_clear();
	// segment test
	/*for(t8=0;t8<0x12;t8++) {
		t83=0x00;
		for (t82=0;t82<8;t82++) {
			t83 = (t83<<1) | 0x01;
			ht16c22_wr_data1(t8,t83);
			led1_toggle;
			delay1ms(250);
		}
	}*/

	delay1ms(2000);
	led1_off;
	led2_off;
	led3_off;
	// set on rectangles & buttons
	cbs106_icon(CBS_ICON_KEY_STBY_RECT, 1);
	cbs106_icon(CBS_ICON_STANDBY, 1);
	cbs106_icon(CBS_ICON_KEY_OK_RECT, 1);
	cbs106_icon(CBS_ICON_KEY_OK, 1);
	cbs106_icon(CBS_ICON_ECO_CIRCLE, 1);
	cbs106_icon(CBS_ICON_ECO, 0);
	cbs106_icon(CBS_ICON_WEEK, 1);
	cbs106_icon(CBS_ICON_CLOCK, 1);
	cbs106_icon(CBS_ICON_BOILER, 1);
	cbs106_icon(CBS_ICON_KEY_PLUS_RECT, 1);
	cbs106_icon(CBS_ICON_KEY_PLUS_ARROW, 1);
	cbs106_icon(CBS_ICON_KEY_MINUS_RECT, 1);
	cbs106_icon(CBS_ICON_KEY_MINUS_ARROW, 1);
	usart_send_strP(PSTR("For command list type: ?\r\nReady for command.\r\n"));
	g_mode=0;
	hh=mh=week=0;
    while (1) 
    {

		//i=0;
		i = atq1070_status();

		// touch detected
		if (i&0x01) {
			t8 = atq1070_get_keys();
			usart_send_strP(PSTR("\r\nTouch: "));
			usart_send_bin_byte(t8);
			if (g_mode < 200) {
				if (t8 == CBS_KEY_BOILER) {
					if (cbs106_icon_get(CBS_ICON_FIRE) == 0) {
						cbs106_icon(CBS_ICON_FIRE, 1);
					} else {
						cbs106_icon(CBS_ICON_FIRE, 0);
					}
				} else if (t8 == CBS_KEY_CLOCK) {
					g_mode=2;
				} else if (t8 == CBS_KEY_STANDBY) {
					if (g_mode == 0) g_mode=1;
					else g_mode = 0;
				} else if (t8 == CBS_KEY_ECO) {
					if (cbs106_icon_get(CBS_ICON_ECO) == 0) {
						cbs106_icon(CBS_ICON_ECO,1);
					} else {
						cbs106_icon(CBS_ICON_ECO,0);
					}
				} else if (t8 == CBS_KEY_OK) {
					if (cbs106_icon_get(CBS_ICON_WAIT) == 0) {
						cbs106_icon(CBS_ICON_WAIT, 1);		
					} else {
						cbs106_icon(CBS_ICON_WAIT, 0);
					}
				} else if (t8 == CBS_KEY_UP) {
					if (g_mode == 1) {
						week++;
					} else if (g_mode == 2) {
						hh++;
					}
				} else if (t8 == CBS_KEY_DOWN) {
					if (g_mode == 1) {
						week--;
					} else if (g_mode == 2) {
						mh--;
					}
				}
			}

			// wait for de-press button
			while( atq1070_status()&0x01 ) {
				led1_toggle;
				delay1ms(50);
			}
			
		}
		



		// overflow flag
		if (i&0x40) {
			usart_send_strP(PSTR("\r\nOVF"));
		}

		// internal at42qt1070 has started calibration
		if (i&0x80) {
			usart_send_strP(PSTR("\r\nCAL"));
		}

		// apply mode to display
		// mode=0 - clear
		// mode=1 - week
		// mode=2 - clock face single hand mode , "up" inc hour hand, "down" decr. min hand

		if (g_mode == 0) {
			// hide: clock, up, down, ok, week
			cbs106_icon(CBS_ICON_KEY_OK_RECT, 0);
			cbs106_icon(CBS_ICON_KEY_OK, 0);
			cbs106_icon(CBS_ICON_WEEK, 0);
			cbs106_icon(CBS_ICON_CLOCK, 0);
			cbs106_icon(CBS_ICON_BOILER, 1);
			cbs106_icon(CBS_ICON_KEY_PLUS_RECT, 0);
			cbs106_icon(CBS_ICON_KEY_PLUS_ARROW, 0);
			cbs106_icon(CBS_ICON_KEY_PLUS, 0);
			cbs106_icon(CBS_ICON_KEY_MINUS_RECT, 0);
			cbs106_icon(CBS_ICON_KEY_MINUS, 0);
			cbs106_icon(CBS_ICON_KEY_MINUS_ARROW, 0);
			cbs106_icon(CBS_ICON_START_ARROW, 0);
			cbs106_icon(CBS_ICON_STOP_ARROW, 0);
			cbs106_clock_face(0xff,0xff); // clear hands
			cbs106_weekday(0xf); // clear hands
			cbs106_digit(0,CBS_DIGIT_SPACE);
			cbs106_digit(1,CBS_DIGIT_SPACE);
			cbs106_digit(2,CBS_DIGIT_SPACE);
			cbs106_digit(3,CBS_DIGIT_SPACE);
			cbs106_digit(4,CBS_DIGIT_SPACE);
			cbs106_digit(5,CBS_DIGIT_SPACE);
		} else if (g_mode == 1) {
			// show buttons, clock face, week
			cbs106_icon(CBS_ICON_KEY_OK_RECT, 0);
			cbs106_icon(CBS_ICON_KEY_OK, 0);
			cbs106_icon(CBS_ICON_ECO_CIRCLE, 1);
			cbs106_icon(CBS_ICON_WEEK, 1);
			cbs106_icon(CBS_ICON_CLOCK, 1);
			cbs106_icon(CBS_ICON_BOILER, 1);
			cbs106_icon(CBS_ICON_KEY_PLUS_RECT, 1);
			cbs106_icon(CBS_ICON_KEY_PLUS, 1);
			cbs106_icon(CBS_ICON_KEY_MINUS_RECT, 1);
			cbs106_icon(CBS_ICON_KEY_MINUS, 1);
			cbs106_clock_face(0xf, 0xf);
			
			if (week > 250) week=7;
			if (week>7) week=0;
			cbs106_weekday(week);
			cbs106_big_num(week);
		} else if (g_mode == 2) { // clock - single hand mode
			cbs106_icon(CBS_ICON_WEEK, 0);
			cbs106_weekday(0x0f); // hide week
			cbs106_icon(CBS_ICON_KEY_PLUS, 0); // hide "+" and "-", show arrows
			cbs106_icon(CBS_ICON_KEY_MINUS, 0);
			cbs106_icon(CBS_ICON_KEY_MINUS_ARROW, 1);
			cbs106_icon(CBS_ICON_KEY_PLUS_ARROW, 1);
			
			if (hh > 12) hh=0;
			if (mh > 12) mh=12;
			cbs106_clock_face(hh, mh);	
		} 
		
			

		/*
		if (btn_down) {
			// print key config
			// -----------------------xxxxxxxxxx:|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|xxxx|
			usart_send_strP(PSTR("\r\nKEYS      :    0    1    2    3    4    5    6"));
			// ---- key status
			usart_send_strP(PSTR("\r\nKey status:    "));
			i = atq1070_get_keys();
			t8 = 0x01;
			for(t82=0;t82<7;t82++) {
				if ((i&t8) == t8) usart_send_char('A');
				else usart_send_char('-');
				usart_send_strP(PSTR("    "));
				t8 <<= 1;
			}
			// ---- key signal reg
			usart_send_strP(PSTR("\r\nKey signal: "));
			for(i=0;i<7;i++) {
				t16 = atq1070_get_key_signal(i);
				usart_send_hex_byte(t16>>8);
				usart_send_hex_byte(t16);
				usart_send_char(' ');
			}
			// ---- key ref data reg
			usart_send_strP(PSTR("\r\nKeyRefDATA: "));
			for(i=0;i<7;i++) {
				t16 = atq1070_get_ref_data(i);
				usart_send_hex_byte(t16>>8);
				usart_send_hex_byte(t16);
				usart_send_char(' ');
			}
			// ---- key NTHR reg
			usart_send_strP(PSTR("\r\nKey NTHR  : "));
			for(i=0;i<7;i++) {
				t8 = aqt1070_get_nthr(i);
				usart_send_char(' '); usart_send_char(' ');
				usart_send_hex_byte(t8);
				usart_send_char(' ');
			}
			// ---- key AVE/AKS reg
			usart_send_strP(PSTR("\r\nKeyAVE/AKS: "));
			for(i=0;i<7;i++) {
				t8 = atq1070_get_ave_aks(i);
				usart_send_char(' '); usart_send_char(' ');
				usart_send_hex_byte(t8);
				usart_send_char(' ');
			}
			// ---- key DI reg
			usart_send_strP(PSTR("\r\nKey DI    : "));
			for(i=0;i<7;i++) {
				t8 = atq1070_get_di(i);
				usart_send_char(' '); usart_send_char(' ');
				usart_send_hex_byte(t8);
				usart_send_char(' ');
			}
			// some globals
			usart_send_strP(PSTR("\r\nFastOutDI="));
			i = atq1070_get_fast_out();
			usart_send_int(i);
			usart_send_strP(PSTR("\r\nMAX_CAL="));
			i = atq1070_get_max_cal();
			usart_send_int(i);
			usart_send_strP(PSTR("\r\nGUARD_CH="));
			i = atq1070_get_guard();
			usart_send_int(i);
			usart_send_strP(PSTR("\r\nLP_MODE="));
			t16 = atq1070_get_lpmode();
			usart_send_hex_byte(t16); usart_send_char(' ');
			if (t16==0)t16=1;
			t16 *= 8;
			usart_send_int(t16);usart_send_char('m');usart_send_char('s');
			usart_send_strP(PSTR("\r\nMAX_ON_DURATION="));
			t16 = atq1070_get_max_on_duration();
			usart_send_hex_byte(t16); usart_send_char(' ');
			if (t16==0) {
				usart_send_char('o');
				usart_send_char('f');
				usart_send_char('f');
				} else {
				t16 *= 160;
				usart_send_int(t16);
				usart_send_char('m');
				usart_send_char('s');
			}
			usart_send_char(0x0d);
			usart_send_char(0x0a);
		}
		//} //end:else:if(i&0x80)
		delay1ms(50);
		*/

		led3_toggle;
		delay1ms(50);
		if (rsBuffPtr == 254) {
			processRSBuff();
		}
    }
}


ISR(USART_RXC_vect) {
	char c = UDR;

	if (c == 0x0d) { // CR
		rsBuffPtr=254; // mark as buffer to be processed
	} else if (c == 0x1b) { // ESC
			flushRSBuff();
	} else {
		if (rsBuffPtr<RS_BUFF_SIZE) {
			rsBuff[rsBuffPtr]=c;
			rsBuffPtr++;
		}
	}
}
