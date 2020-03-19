#ifndef I2C_H_INCLUDED
#define I2C_H_INCLUDED

#define I2C_DEBUG 0

// 50us=10kHz , 10us=50kHz, 5us=100kHz
#define I2C_HDELAY_US 20

#define I2C_SDA_PORT PORTC
#define I2C_SDA_PIN PINC
#define I2C_SDA_DDR DDRC
#define I2C_SDA 0

#define I2C_SCL_PORT PORTC
#define I2C_SCL_PIN PINC
#define I2C_SCL_DDR DDRC
#define I2C_SCL 1


#define i2c_init() I2C_SCL_PORT |= 1<<I2C_SCL; \
	I2C_SDA_PORT |= 1<<I2C_SDA; \
	I2C_SCL_DDR |= 1<<I2C_SCL; \
	I2C_SDA_DDR |= 1<<I2C_SDA;

#define I2C_ACK 1
#define I2C_NACK 0

#if I2C_DEBUG>0
	#include "usart.h"
#endif

void i2c_start(void);
void i2c_stop(void);
uint8_t i2c_send(uint8_t data);
uint8_t i2c_get(uint8_t ack);

// uncomment to enable routines
//#define _I2C_USE_PCF8574_

#if defined(_I2C_USE_PCF8574_)
void i2c_send8574(uint8_t adr, uint8_t data); // for PCF8574
uint8_t i2c_get8574(uint8_t adr); // for PCF8574
#endif

#endif // I2C_H_INCLUDED
