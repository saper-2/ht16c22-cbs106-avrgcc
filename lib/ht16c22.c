/*
 * ht16c22.c
 *
 * Created: 2017-12-30 14:19:43
 *  Author: saper
 */ 

#include "ht16c22.h"
#include "i2c.h"

#if defined(HT16C22_SRAM_BUFF)
uint8_t ht16c22_ram[HT16C22_SRAM_BUFF_SZ];
#endif

void ht16c22_cmd(uint8_t cmd) {
	i2c_start();
	i2c_send(HT16C22_I2C_ADDR);
	i2c_send(cmd);
	i2c_stop();
}


void ht16c22_wr_data1(uint8_t addr, uint8_t data) {
	i2c_start();
	i2c_send(HT16C22_I2C_ADDR);
	i2c_send( (HT16C22_CMD_ADDR_MASK & addr) );
	i2c_send(data);
	i2c_stop();
}

void ht16c22_wr_block(uint8_t addr, uint8_t cnt, uint8_t *data) {
	i2c_start();
	i2c_send(HT16C22_I2C_ADDR);
	i2c_send( (HT16C22_CMD_ADDR_MASK & addr) );
	while (cnt>0) {
		i2c_send( (*data) );
		data++;
		cnt--;
	}
	i2c_stop();
}


void ht16c22_set_bias(uint8_t val) {
	i2c_start();
	i2c_send(HT16C22_I2C_ADDR);
	i2c_send(HT16C22_CMD_IVA | (HT16C22_CMD_IVA_DA_MASK & val) | HT16C22_CMD_IVA_DE1_VE1);
	i2c_stop();
}


void ht16c22_setup(void) {
	#if defined(HT16C22_SRAM_BUFF)
		// reset RAM
		for (uint8_t i=0;i<HT16C22_SRAM_BUFF_SZ;i++) ht16c22_ram[HT16C22_SRAM_BUFF_SZ]=0;
	#endif

	// setup IOs
	
	// end I/O setup
	HT16C22_POWER_ON_DELAY;
	// init controller
	ht16c22_cmd(HT16C22_CMD_IVA | (HT16C22_CMD_IVA_DA_MASK & 7) | HT16C22_CMD_IVA_DE1_VE1);
	// turn on display
	ht16c22_cmd(HT16C22_CMD_MODE_SET | HT16C22_CMD_MODE_BIAS13 | HT16C22_CMD_MODE_ON_ENA_CK | HT16C22_CMD_MODE_F80HZ);
	HT16C22_OSC_ON_DELAY;
	
	ht16c22_clear();
	#if defined(HT16C22_SRAM_BUFF)
		ht16c22_update();
	#endif
}


#if !defined(HT16C22_SRAM_BUFF)
uint8_t ht16c22_read1(uint8_t addr) {
	uint8_t res=0;
	
	i2c_start();
	i2c_send(HT16C22_I2C_ADDR);
	i2c_send( (HT16C22_CMD_ADDR_MASK & addr) );
	i2c_start();
	i2c_send(HT16C22_I2C_ADDR|0x01);
	res = i2c_get(I2C_NACK);
	i2c_stop();

	return res;
}

void ht16c22_read_block(uint8_t addr, uint8_t cnt, uint8_t *buff) {
	uint8_t ack=0;
	i2c_start();
	i2c_send(HT16C22_I2C_ADDR);
	i2c_send( (HT16C22_CMD_ADDR_MASK & addr) );
	i2c_start();
	i2c_send(HT16C22_I2C_ADDR|0x01);
	while(cnt > 0) {
		if ((cnt-1)>0) ack=I2C_ACK;
		else ack=I2C_NACK;
		
		*(buff) = i2c_get(ack);
		buff++;
		cnt--;
	} 
	i2c_stop();
}
#endif

#if defined(HT16C22_SRAM_BUFF)
void ht16c22_update(void) { // enabled SRAM framebuffer
	
	i2c_start();
	i2c_send(HT16C22_I2C_ADDR);
	i2c_send( (HT16C22_CMD_ADDR_MASK & 0x00 ) ); // address 0x00
	
	// now send the buffer to the lcd
	for (uint8_t a=0;a<HT16C22_SRAM_BUFF_SZ;a++) {
		i2c_send(ht16c22_ram[a]);
	}
	i2c_stop();
}

void ht16c22_clear(void) { // enabled SRAM framebuffer
	for(uint8_t i=0;i<HT16C22_SRAM_BUFF_SZ;i++) ht16c22_ram[i]=0;
}

#else 
void ht16c22_clear(void) { // no framebuffer
	i2c_start();
	i2c_send(HT16C22_I2C_ADDR);
	i2c_send( (HT16C22_CMD_ADDR_MASK & 0x00 ) ); // address 0x00
	// now send 0x00 to fill whole mem (44x4bit = 22 bytes)
	for (uint8_t a=0;a<22;a++) {
		i2c_send(0x00);
	}
	i2c_stop();
}
#endif