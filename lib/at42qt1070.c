/*
* at42qt1070.c
*
* Created: 2018-02-24 20:15:01
*  Author: saper
*/ 

#include <avr/io.h>
#include <inttypes.h>
#include "i2c.h"
#include "at42qt1070.h"

uint8_t atq1070_read_byte_reg(uint8_t reg) {
	uint8_t r;
	i2c_start();
	i2c_send(AT42QT1070_ADDR);
	i2c_send(reg);
	i2c_start();
	i2c_send(AT42QT1070_ADDR+1);
	r = i2c_get(I2C_NACK);
	i2c_stop();
	return r;
}

void atq1070_write_byte_reg(uint8_t reg, uint8_t val) {
	i2c_start();
	i2c_send(AT42QT1070_ADDR);
	i2c_send(reg);
	i2c_send(val);
	i2c_stop();
}

uint16_t atq1070_read_word_reg(uint8_t reg) {
		uint16_t w;
		i2c_start();
		i2c_send(AT42QT1070_ADDR);
		i2c_send(reg);
		i2c_start();
		i2c_send(AT42QT1070_ADDR+1);
		w = i2c_get(I2C_ACK);
		w <<= 8;
		w |= (0x00ff & i2c_get(I2C_NACK));
		i2c_stop();
		return w;
}

void atq1070_write_word_reg(uint8_t reg, uint16_t val) {
	i2c_start();
	i2c_send(AT42QT1070_ADDR);
	i2c_send(reg);
	i2c_send(val>>8);
	i2c_send(val&0x00ff);
	i2c_stop();
}
// **************************************************************
// ******   PUBLISHED ROUTINES   ******

void atq1070_read_id(uint8_t *cid, uint8_t *fwv) {
	uint16_t w = atq1070_read_word_reg(0x00);
	(*cid) = (w>>8)&0x00ff;
	(*fwv) = w&0x00ff;
}

uint8_t atq1070_status(void) {
	return atq1070_read_byte_reg(0x02);
}

uint8_t atq1070_get_keys(void) {
	return atq1070_read_byte_reg(0x03);
}

uint16_t atq1070_get_key_signal(uint8_t ch) {
	if (ch>6) ch=6;
	ch *= 2;
	ch += 4;
	return atq1070_read_word_reg(ch);
}

uint16_t atq1070_get_ref_data(uint8_t ch) {
	if (ch>6) ch=6;
	ch *= 2;
	ch += 18;
	return atq1070_read_word_reg(ch);
}

uint8_t aqt1070_get_nthr(uint8_t ch) {
	if (ch>6) ch=6;
	ch += 32;
	return atq1070_read_byte_reg(ch);
}
void atq1070_set_nthr(uint8_t ch, uint8_t nthr) {
	if (ch>6) ch=6;
	ch += 32;
	atq1070_write_byte_reg(ch, nthr);
}

uint8_t atq1070_get_ave_aks(uint8_t ch) {
	if (ch>6) ch=6;
	ch += 39;
	return atq1070_read_byte_reg(ch);
}

void atq1070_set_ave_aks(uint8_t ch, uint8_t aveaks) {
	if (ch>6) ch=6;
	ch += 39;
	atq1070_write_byte_reg(ch, aveaks);
}

uint8_t atq1070_get_di(uint8_t ch) {
	if (ch>6) ch=6;
	ch += 46;
	return atq1070_read_byte_reg(ch);
}

void atq1070_set_di(uint8_t ch, uint8_t di) {
	if (ch>6) ch=6;
	ch += 46;
	atq1070_write_byte_reg(ch, di);
}

uint8_t atq1070_get_fast_out(void) {
	uint8_t r;
	r = atq1070_read_byte_reg(53);
	if ((r & 0x20) == 0x20) return 1;
	return 0;
}

void atq1070_set_fast_out(uint8_t fo) {
	uint8_t r;
	r = atq1070_read_byte_reg(53);
	r &= ~(0x20); // mask bit5
	if (fo>0) r |= 0x20; // set bit5
	atq1070_write_byte_reg(53, r);
}

uint8_t atq1070_get_max_cal(void) {
	uint8_t r;
	r = atq1070_read_byte_reg(53);
	if ((r & 0x10) == 0x10) return 1;
	return 0;
}

void atq1070_set_max_cal(uint8_t maxcal) {
	uint8_t r;
	r = atq1070_read_byte_reg(53);
	r &= ~(0x10); // mask bit4
	if (maxcal>0) r |= 0x10; // set bit4
	atq1070_write_byte_reg(53, r);
}

uint8_t atq1070_get_guard(void) {
	uint8_t r;
	r = atq1070_read_byte_reg(53);
	return r&0x0f;
}

void atq1070_set_guard(uint8_t ch) {
	uint8_t r;
	r = atq1070_read_byte_reg(53);
	r &= 0xf0; // mask bit[3:0]
	ch &= 0x0f;
	r |= ch;
	atq1070_write_byte_reg(53, r);
}

uint8_t atq1070_get_lpmode(void) {
	return atq1070_read_byte_reg(54);
}

void atq1070_set_lpmode(uint8_t tim8ms) {
	atq1070_write_byte_reg(54, tim8ms);
}

uint8_t atq1070_get_max_on_duration(void) {
	return atq1070_read_byte_reg(55);
}
void atq1070_set_max_on_duration(uint8_t time160ms) {
	atq1070_write_byte_reg(55, time160ms);
}


void atq1070_calibrate(void) {
	atq1070_write_byte_reg(56, 1);
}

void atq1070_reset(void) {
	atq1070_write_byte_reg(57, 1);
}
