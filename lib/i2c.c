// obs³uga I2C
#include <avr/io.h>
#include <inttypes.h>
#include <avr/pgmspace.h>

//#include "harddef.h"
#include "i2c.h"
#include "delay.h"

// --------------------------------------------------------
// --------------------------------------------------------
static inline void i2c_hdelay(void) {
	delay1us(I2C_HDELAY_US);
}
// --------------------------------------------------------
// --------------------------------------------------------

// set/reset data
static inline void i2c_sdaset(void) {
	I2C_SDA_DDR &= ~(1<<I2C_SDA);
	I2C_SDA_PORT |= 1<<I2C_SDA;
}

static inline void i2c_sdaclear(void) {
	I2C_SDA_PORT &= ~(1<<I2C_SDA);
	I2C_SDA_DDR |= 1<<I2C_SDA;
}

// pobieranie danej z portu danych
static inline uint8_t i2c_sdaget(void) {
	return (I2C_SDA_PIN & (1<<I2C_SDA));
}

// set/reset clock line
static inline void i2c_sclset(void) {
	I2C_SCL_PORT |= 1<<I2C_SCL;
}

static inline void i2c_sclclear(void) {
	I2C_SCL_PORT &= ~(1<<I2C_SCL);
}

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

void i2c_start(void) {
	#if I2C_DEBUG>0
		usart_send_char('s');
	#endif
	// start bez stopu
	i2c_sdaset();
	i2c_hdelay();
	i2c_sclset();
	i2c_hdelay();
	// "normalna" sekw. staru
	i2c_sdaclear();
	i2c_hdelay();
	i2c_sclclear();
}

void i2c_stop(void) {
	i2c_sdaclear();
	i2c_hdelay();
	i2c_sclset();
	i2c_hdelay();
	i2c_sdaset();
	i2c_hdelay();
	#if I2C_DEBUG>0
	usart_send_char('t');
	#endif
	
}

uint8_t i2c_send(uint8_t data) {
	uint8_t n;
	
	#if I2C_DEBUG>0
		usart_send_char('x');
		usart_send_hex_byte(data);
	#endif
	

	for (n=8; n>0; --n) {
		if (data & 0x80) i2c_sdaset(); 
			else i2c_sdaclear();
		data <<= 1;
		i2c_hdelay();
		i2c_sclset();
		i2c_hdelay();
		i2c_sclclear();
	}
	// ACK
	i2c_sdaset();
	i2c_hdelay();
	i2c_sclset();
	i2c_hdelay();
	n = i2c_sdaget();
	i2c_sclclear();
	
	return n;
}


uint8_t i2c_get(uint8_t ack) {
	uint8_t n, temp=0;

	#if I2C_DEBUG>0
		usart_send_char('g');
		if (ack==I2C_NACK) usart_send_char('N'); 
		else usart_send_char('A');
	#endif
	
	i2c_sdaset();
	for (n=8; n>0; --n) {
		i2c_hdelay();
		i2c_sclset();
		i2c_hdelay();
		temp <<= 1;
		if (i2c_sdaget()) temp++;
		i2c_sclclear();
	}
	if (ack == I2C_ACK) i2c_sdaclear();
		else i2c_sdaset();
	i2c_hdelay();
	i2c_sclset();
	i2c_hdelay();
	i2c_sclclear();
	

	#if I2C_DEBUG>0
		usart_send_hex_byte(temp);
	#endif

	return temp;
}

#if defined(_I2C_USE_PCF8574_)
void i2c_send8574(uint8_t adr, uint8_t data) {
	i2c_start();
	i2c_send(adr);
	i2c_send(data);
	i2c_stop();
}

uint8_t i2c_get8574(uint8_t adr) {
	uint8_t tmp;
	
	i2c_start();
	i2c_send(adr);
	tmp = i2c_get(I2C_NACK);
	i2c_stop();
	return tmp;
} //*/
#endif