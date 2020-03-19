/*
 * ht16c22.h
 *
 * Created: 2017-12-30 14:15:34
 *  Author: saper
 */ 


#ifndef _HT16C22_H_
#define _HT16C22_H_


	#include <inttypes.h>
	#include <avr/io.h>
	#include <avr/pgmspace.h>
	#include "delay.h"

	// number of used COMx (1..8)
	#define HT16C22_COMS 3
	// number of used SEGx (1..64)
	#define HT16C22_SEGS 36

	// Enable SRAM display content buffer
	// Require at least of SRAM: (COMS*SEGS)/8
	//#define HT16C22_SRAM_BUFF

	#if defined(HT16C22_SRAM_BUFF)
		#error SRAM framebuffer not supported, need to optimize SRAM mapping for CBS106 lcd panel (used COM0..COM2, SEG0..SEG35)
		// BE CAREFUL if you have odd number of COMx or SEGx , calculate this manually , 
		// and correct if you get a value with "decimals" "round it up"! to full number
		#define HT16C22_SRAM_BUFF_SZ ((HT16C22_COMS*HT16C22_SEGS)/8)

		#if ( (HT16C22_SRAM_BUFF_SZ % 8) != 0 )
			#error HT16C22 Framebuffer is not aligned to full byte! SRAM Framebuffer size: HT16C22_SRAM_BUFF_SZ
		#endif

	#endif

	// The HT16C22 works on I2C bus, this one is configured in: i2c.h
	
	#define HT16C22_I2C_ADDR 0x7E // 7bit address: 0x3F
	
	// half-clock delay in us
	#define HT16C22_H_DELAY_AFTER_TRANSMISSION
	#define HT16C22_POWER_ON_DELAY delay1ms(20)
	#define HT16C22_OSC_ON_DELAY delay1ms(20)

	#define HT16C22_CMD_MODE_SET 0x80
		#define HT16C22_CMD_MODE_BIAS13 0x00
		#define HT16C22_CMD_MODE_BIAS12 0x01
		#define HT16C22_CMD_MODE_OFF_DIS_CK 0x00 // display off, disable int. clock
		#define HT16C22_CMD_MODE_OFF_ENA_CK 0x08 // display off, enable int. clock
		#define HT16C22_CMD_MODE_ON_ENA_CK 0x0C // disp on, enable int. clock
		#define HT16C22_CMD_MODE_F80HZ 0x00 // frame freq. 80Hz
		#define HT16C22_CMD_MODE_F160HZ 0x10 // frame freq. 160Hz
	#define HT16C22_CMD_BLINK 0xC0
		#define HT16C22_CMD_BLINK_OFF 0x00
		#define HT16C22_CMD_BLINK_2HZ 0x01
		#define HT16C22_CMD_BLINK_1HZ 0x02
		#define HT16C22_CMD_BLINK_05HZ 0x03
	#define HT16C22_CMD_SET_ADDR 0x00
		#define HT16C22_CMD_ADDR_MASK 0x1f
	#define HT16C22_CMD_IVA 0x40
		#define HT16C22_CMD_IVA_DA_MASK 0x0f // DA[3:0] = Internal voltage follower value
		#define HT16C22_CMD_IVA_DE0_VE0 0x00 // pin Seg44/VLCD=VLCD input (R to Vcc for bias setting), internal volt. follower must be enabled by setting DA[3:0]>0x00. Connecting VLCD pin to Vdd then DA[3:0] have to be set to 0 (disable int. volt. follower).
		#define HT16C22_CMD_IVA_DE0_VE1 0x10 // pin Seg44/VLCD=VLCD output (for reading bias voltage, e.g. MCU ADC), internal volt. adj is enabled 
		#define HT16C22_CMD_IVA_DE1_VE0 0x20 // pin Seg44/VLCD=Segment out, bias supplied from int. Vdd, int volt-follower is disabled (DA[3:0]=don't care)
		#define HT16C22_CMD_IVA_DE1_VE1 0x30 // pin Seg44/VLCD=Segment out, bias supplied from int. Vdd, internal voltage adj. enabled
		
		
	
void ht16c22_cmd(uint8_t cmd);
void ht16c22_wr_data1(uint8_t addr, uint8_t data);
void ht16c22_wr_block(uint8_t addr, uint8_t cnt, uint8_t *data);

void ht16c22_set_bias(uint8_t val); // internal voltage set (bias voltage set)

void ht16c22_setup(void);

#if !defined(HT16C22_SRAM_BUFF)
	uint8_t ht16c22_read1(uint8_t addr);
	void ht16c22_read_block(uint8_t addr, uint8_t cnt, uint8_t *buff);
#endif

#if defined(HT16C22_SRAM_BUFF)
	void ht16c22_update(void);
	void ht16c22_clear(void);
#else
	void ht16c22_clear(void);
#endif

#endif /* HT16C22_H_ */