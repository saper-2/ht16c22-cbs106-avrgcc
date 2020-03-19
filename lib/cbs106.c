/*
 * cbs106.c
 *
 * Created: 2017-12-30 16:21:18
 *  Author: saper
 */ 


#include "cbs106.h"
#include <inttypes.h>
#include <avr/pgmspace.h>

const uint8_t cbs_digits[] PROGMEM = {
	0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, // 0..9 [0x00..0x09]
	0x77, 0x7C, 0x58, 0x5E, 0x79, 0x71, // A, b, c, d, E, F [0x0a .. 0x0f]
	0x39, 0x1C, 0x3E, 0x50,  // C, u, U, r [0x10..0x13]
	0x5C, 0x63, 0x78, 0x0E,  // o, deg, t, j [0x14..0x17]
	0x38, 0x76, 0x73, 0x00, 0x40, 0x08 // L, H, P, " ", -, _ [0x18..0x1d]
};

// Show digit (or letter) on digit display: digit=digit number[0..5], val=digit value[0..9A..F,...] see "cbs_digits" array
void cbs106_digit(uint8_t digit, uint8_t val) {
	//uint8_t sa = 0;
	uint8_t dda = 0;
	uint8_t tmp[3];
	
	#if CBS106_DEBUG>0
	usart_send_char('D');
	usart_send_hex_byte(digit);
	usart_send_char('.');
	usart_send_hex_byte(val);
	usart_send_char('=');
	#endif

	// get digit bit map of value
	dda = pgm_read_byte((&cbs_digits[0])+val);

	#if CBS106_DEBUG>0
	usart_send_hex_byte(dda);
	usart_send_char('-');
	#endif

	tmp[0]=tmp[1]=tmp[2]=0;
	// digit mapping is screwed -_- , have to do this manually
	if (digit == 0) {
		ht16c22_read_block(0x0e, 2, &(tmp[0]) );
	} else if (digit == 1) {
		ht16c22_read_block(0x0d, 2, &(tmp[0]) );
	} else if (digit == 2) {
		ht16c22_read_block(0x0b, 2, &(tmp[0]) );
	} else if (digit == 3) {
		ht16c22_read_block(0x0a, 2, &(tmp[0]) );
	} else if (digit == 4) {
		ht16c22_read_block(0x08, 2, &(tmp[0]) );
	} else if (digit == 5) {
		tmp[0] = ht16c22_read1(0x05);
		ht16c22_read_block(0x07, 2, &(tmp[1]) );
	}
	
	#if CBS106_DEBUG>0
	usart_send_hex_byte(tmp[0]);
	usart_send_hex_byte(tmp[1]);
	usart_send_hex_byte(tmp[2]);
	usart_send_char('.');
	#endif

	// digits 1,3,5 have identical segment-bit map
	if ((digit == 0) || (digit == 2) || (digit == 4)) {
		// reset bits
		tmp[0] &= ~(0x30);
		tmp[1] &= ~(0x37);
		// set bits
		if (dda&0x01) tmp[1] |= 0x04; // seg. A
		if (dda&0x02) tmp[0] |= 0x10; // seg. B
		if (dda&0x04) tmp[0] |= 0x20; // seg. C
		if (dda&0x08) tmp[1] |= 0x02; // seg. D
		if (dda&0x10) tmp[1] |= 0x20; // seg. E
		if (dda&0x20) tmp[1] |= 0x10; // seg. F
		if (dda&0x40) tmp[1] |= 0x01; // seg. G
		
	// digits 2,4 have identical segment-bit map
	} else if ((digit == 1) || (digit == 3)) {
		// reset bits
		tmp[0] &= ~(0x73);
		tmp[1] &= ~(0x03);
		// set bits
		if (dda&0x01) tmp[0] |= 0x40; // seg. A
		if (dda&0x02) tmp[0] |= 0x01; // seg. B
		if (dda&0x04) tmp[0] |= 0x02; // seg. C
		if (dda&0x08) tmp[0] |= 0x20; // seg. D
		if (dda&0x10) tmp[1] |= 0x02; // seg. E
		if (dda&0x20) tmp[1] |= 0x01; // seg. F
		if (dda&0x40) tmp[0] |= 0x10; // seg. G
		
	// 6th digit is madness
	} else if (digit == 5) {
		// reset bits
		tmp[0] &= ~(0x03);
		tmp[1] &= ~(0x30);
		tmp[2] &= ~(0x07);
		
		if (dda&0x01) tmp[2] |= 0x04; // seg. A
		if (dda&0x02) tmp[1] |= 0x10; // seg. B
		if (dda&0x04) tmp[1] |= 0x20; // seg. C
		if (dda&0x08) tmp[2] |= 0x02; // seg. D
		if (dda&0x10) tmp[0] |= 0x02; // seg. E
		if (dda&0x20) tmp[0] |= 0x01; // seg. F
		if (dda&0x40) tmp[2] |= 0x01; // seg. G
	}
	
	#if CBS106_DEBUG>0
	usart_send_hex_byte(tmp[0]);
	usart_send_hex_byte(tmp[1]);
	usart_send_hex_byte(tmp[2]);
	usart_send_char(';');
	#endif

	// write back digit
	if (digit == 0) {
		ht16c22_wr_block(0x0e, 2, &(tmp[0]) );
	} else if (digit == 1) {
		ht16c22_wr_block(0x0d, 2, &(tmp[0]) );
	} else if (digit == 2) {
		ht16c22_wr_block(0x0b, 2, &(tmp[0]) );
	} else if (digit == 3) {
		ht16c22_wr_block(0x0a, 2, &(tmp[0]) );
	} else if (digit == 4) {
		ht16c22_wr_block(0x08, 2, &(tmp[0]) );
	} else if (digit == 5) {
		ht16c22_wr_data1(0x05, tmp[0]);		
		ht16c22_wr_block(0x07, 2, &(tmp[1]) );
	}
	
}

void cbs106_icon(uint8_t icon, uint8_t state) {
	uint8_t adr, bit, mask;

	adr = (icon>>3) & 0x1f;
	bit = icon & 0x07;
	mask = 0x01 << bit;

	uint8_t tmp = ht16c22_read1(adr);

	tmp &= ~(mask);

	if (state > 0) {
		tmp |= mask;
	}
	ht16c22_wr_data1(adr,tmp);
}

uint8_t cbs106_icon_get(uint8_t icon) {
	uint8_t adr, bit, mask;

	adr = (icon>>3) & 0x1f;
	bit = icon & 0x07;
	mask = 0x01 << bit;

	uint8_t tmp = ht16c22_read1(adr);

	tmp &= mask;

	if (tmp > 0) return 1;
	
	return 0;
}

void set_clock_face_digit_hh(uint8_t hh, uint8_t *arr) {
	
	if (hh ==  0) arr[1] |= 0x10;
	else if (hh ==  1) arr[1] |= 0x20;
	else if (hh ==  2) arr[3] |= 0x02;
	else if (hh ==  3) arr[3] |= 0x01;
	else if (hh ==  4) arr[3] |= 0x04;
	else if (hh ==  5) arr[4] |= 0x40;
	else if (hh ==  6) arr[4] |= 0x10;
	else if (hh ==  7) arr[4] |= 0x20;
	else if (hh ==  8) arr[1] |= 0x02;
	else if (hh ==  9) arr[1] |= 0x01;
	else if (hh == 10) arr[1] |= 0x04;
	else if (hh == 11) arr[1] |= 0x40;
	// why not switch-case: because switch-case devour 50bytes more code memory, than 12-if-else-if	
}

void set_clock_face_digit_mh(uint8_t mh, uint8_t *arr) {
	
	if (mh ==  0) arr[2] |= 0x01;
	else if (mh ==  1) arr[2] |= 0x02;
	else if (mh ==  2) arr[3] |= 0x20;
	else if (mh ==  3) arr[3] |= 0x10;
	else if (mh ==  4) arr[3] |= 0x40;
	else if (mh ==  5) arr[4] |= 0x04;
	else if (mh ==  6) arr[4] |= 0x01;
	else if (mh ==  7) arr[4] |= 0x02;
	else if (mh ==  8) arr[0] |= 0x20;
	else if (mh ==  9) arr[0] |= 0x10;
	else if (mh == 10) arr[0] |= 0x40;
	else if (mh == 11) arr[2] |= 0x04;
	// why not switch-case: because switch-case devour 50bytes more code memory, than 12-if-else-if
}

void clock_face_read_and_clear(uint8_t *arr) {
	ht16c22_read_block(0x02, 3, &arr[0] );
	ht16c22_read_block(0x10, 2, &arr[3] );
	// clear bits/segments
	arr[0] &= ~(0x70);
	arr[1] = 0x00;
	arr[2] &= ~(0x07);
	arr[3]=arr[4]=0x00;
}

// hh=mh> 12 = disabled
void cbs106_clock_face(uint8_t hh, uint8_t mh) {
	uint8_t t[5];

	clock_face_read_and_clear(&t[0]);

	if (hh < 12) {
		set_clock_face_digit_hh(hh, &t[0]);
	}
	if (mh < 12) {
		set_clock_face_digit_mh(mh, &t[0]);
	}
	ht16c22_wr_block(0x02, 3, &(t[0]));
	ht16c22_wr_block(0x10, 2, &(t[3]));

}


// show clock face with filled area (e.g. from 1hr to 5hr)
void cbs106_clock_face_fill(uint8_t hhs, uint8_t hhe, uint8_t mhs, uint8_t mhe) {
	uint8_t t[5];
	uint8_t x;
	clock_face_read_and_clear(&t[0]);
	
	// check bounds
	if (hhs > 11) hhs=11;
	if (hhe > 11) hhe=10;
	if (mhs > 11) mhs=11;
	if (mhe > 11) mhe=10;

	hhe++;
	mhe++;

	x=hhs;
	do {
		set_clock_face_digit_hh(x, &t[0]);
		x++;
		if (x>12) x=0;
	} while(x!=hhe);

	x=mhs;
	do {
		set_clock_face_digit_mh(x, &t[0]);
		x++;
		if (x>12) x=0;
	} while(x!=mhe) ;

	ht16c22_wr_block(0x02, 3, &(t[0]));
	ht16c22_wr_block(0x10, 2, &(t[3]));
}

uint8_t last_weekday=0xff;
void cbs106_weekday(uint8_t v) {
	
	// this is to prevent blinking weekday marker
	if (last_weekday == v) return;
	last_weekday=v;

	cbs106_icon(CBS_ICON_W1_MON, 0);
	cbs106_icon(CBS_ICON_W2_TUE, 0);
	cbs106_icon(CBS_ICON_W3_WED, 0);
	cbs106_icon(CBS_ICON_W4_THU, 0);
	cbs106_icon(CBS_ICON_W5_FRI, 0);
	cbs106_icon(CBS_ICON_W6_SAT, 0);
	cbs106_icon(CBS_ICON_W7_SUN, 0);

	if (v == 0) cbs106_icon(CBS_ICON_W1_MON, 1);
	else if (v == 1) cbs106_icon(CBS_ICON_W2_TUE, 1);
	else if (v == 2) cbs106_icon(CBS_ICON_W3_WED, 1);
	else if (v == 3) cbs106_icon(CBS_ICON_W4_THU, 1);
	else if (v == 4) cbs106_icon(CBS_ICON_W5_FRI, 1);
	else if (v == 5) cbs106_icon(CBS_ICON_W6_SAT, 1);
	else if (v == 6) cbs106_icon(CBS_ICON_W7_SUN, 1);
}

// this will display colon and digit numbers. use h/m >= 100 to display "--"
void cbs106_small_clock(uint8_t h, uint8_t m) {
	uint8_t tmp;
	// set colon
	cbs106_icon(CBS_ICON_SMALL_DP, 1);

	if (h>99) {
		cbs106_digit(0,CBS_DIGIT_MINUS);
		cbs106_digit(1,CBS_DIGIT_MINUS);
	} else {
		tmp = h/10;
		cbs106_digit(0,tmp);
		tmp=h%10;
		cbs106_digit(1,tmp);
	}

	if (m>99) {
		cbs106_digit(2,CBS_DIGIT_MINUS);
		cbs106_digit(3,CBS_DIGIT_MINUS);
	} else {
		tmp = m/10;
		cbs106_digit(2,tmp);
		tmp=m%10;
		cbs106_digit(3,tmp);
	}
}

// show number from -999 to 9999 on small clock
void cbs106_small_num(int16_t num) {
	int16_t tmp;

	if (num < -999) {
		cbs106_digit(0, CBS_DIGIT_MINUS);
		cbs106_digit(1, CBS_DIGIT_L);
		cbs106_digit(2, CBS_DIGIT_o);
		cbs106_digit(2, CBS_DIGIT_SPACE);
	} else if (num < 0) {
		if (num < -99) cbs106_digit(0,CBS_DIGIT_MINUS);
		if (num < -9) {
			cbs106_digit(0, CBS_DIGIT_SPACE);
			cbs106_digit(1,CBS_DIGIT_MINUS);
		} else {
			cbs106_digit(0, CBS_DIGIT_SPACE);
			cbs106_digit(1, CBS_DIGIT_SPACE);
			cbs106_digit(2,CBS_DIGIT_MINUS);
		}
		num *= -1; // reverse
		tmp = num/100;
		if (tmp>0) cbs106_digit(1, tmp);
		num = num - (tmp*100);
		tmp = num/10;
		if (tmp>0) cbs106_digit(2, tmp);
		tmp = num%10;
		cbs106_digit(3,tmp);
	} else if (num > 999) {
		cbs106_digit(0, CBS_DIGIT_H);
		cbs106_digit(1, 1);
		cbs106_digit(2, CBS_DIGIT_MINUS);
		cbs106_digit(2, CBS_DIGIT_MINUS);
	} else if (num > 0) {
		if (num < 1000) cbs106_digit(0, CBS_DIGIT_SPACE);
		if (num < 100) cbs106_digit(1, CBS_DIGIT_SPACE);
		if (num < 10) cbs106_digit(2, CBS_DIGIT_SPACE);
		tmp = num/1000;
		if (tmp > 0) cbs106_digit(0,tmp);
		num = num - (tmp*1000);
		tmp = num/100;
		if (tmp > 0) cbs106_digit(1,tmp);
		num = num - (tmp*100);
		tmp = num/10;
		if (tmp > 0) cbs106_digit(2, tmp);
		tmp = num % 10;
		cbs106_digit(3,num);
	} else {
		// zero
		cbs106_digit(0, CBS_DIGIT_SPACE);
		cbs106_digit(1, CBS_DIGIT_SPACE);
		cbs106_digit(2, CBS_DIGIT_SPACE);
		cbs106_digit(3, 0);
	}
}

// show number on big digits from -9 to 99. value below -9 will show "Lo" , over 99 will show "Hi"
void cbs106_big_num(int8_t num) {
	int8_t tmp;

	if (num < -9) {
		cbs106_digit(4, CBS_DIGIT_L);
		cbs106_digit(5, CBS_DIGIT_o);
	} else if (num < 0) {
		cbs106_digit(4,CBS_DIGIT_MINUS);
		num *= -1; // reverse
		tmp = num%10;
		cbs106_digit(5,tmp);
	} else if (num > 99) {
		cbs106_digit(4, CBS_DIGIT_H);
		cbs106_digit(5, 1);
	} else if (num > 0) {
		if (num < 10) cbs106_digit(4, CBS_DIGIT_SPACE);
		tmp = num/10;
		if (tmp > 0) cbs106_digit(4, tmp);
		tmp = num % 10;
		cbs106_digit(5,num);
	} else {
		// zero
		cbs106_digit(4, CBS_DIGIT_SPACE);
		cbs106_digit(5, 0);
	}
}
