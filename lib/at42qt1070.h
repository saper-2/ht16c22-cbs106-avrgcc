/*
 * at42qt1070.h
 *
 * Created: 2018-02-24 19:48:04
 *  Author: saper
 */ 


#ifndef AT42QT1070_H_
#define AT42QT1070_H_

// AT42QT1070 I2C address (8bit address)
#define AT42QT1070_ADDR 0x36 // 7bit address: 0x1B

#define ATQ1070_REG_STATUS_MASK_TOUCH 0x01
#define ATQ1070_REG_STATUS_MASK_OVERFLOW 0x40
#define ATQ1070_REG_STATUS_MASK_CALIBRATE 0x80

void atq1070_read_id(uint8_t *cid, uint8_t *fwv); // Chip info & firmware version (reg 0x00..0x01)

uint8_t atq1070_status(void); // Read detection status (reg 0x02)

uint8_t atq1070_get_keys(void); // Get key-bit status (reg 0x03)

uint16_t atq1070_get_key_signal(uint8_t ch); // read key signal (16bit , ch(key#)=0..6)

uint16_t atq1070_get_ref_data(uint8_t ch); // read ref. data for key channel

uint8_t aqt1070_get_nthr(uint8_t ch); // read Negative Threshold level for key(ch=0..6)
void atq1070_set_nthr(uint8_t ch, uint8_t nthr); // set Negative Threshold level for key(ch=0..6)

uint8_t atq1070_get_ave_aks(uint8_t ch); // get Adjacent key suppression level for key(ch=0..6)
void atq1070_set_ave_aks(uint8_t ch, uint8_t aveaks); // set Adjacent key suppression level for key(ch0..6)

uint8_t atq1070_get_di(uint8_t ch); // get Detection integrator counter for key(ch=0..6)
void atq1070_set_di(uint8_t ch, uint8_t di); // set Detection integrator counter for key(ch=0..6)

// bit5 - Fast Out (FO) of register 0x53
uint8_t atq1070_get_fast_out(void); // return 0 or 1
void atq1070_set_fast_out(uint8_t fo); // fo=0:bit5=0 / fo>0: bit5=1

// bit4 - MAX CAL of register 0x53
uint8_t atq1070_get_max_cal(void); // reeturn 0 or 1
void atq1070_set_max_cal(uint8_t maxcal); 

// guard channel (bits[3:0]) from reg 0x53 - select which channel is a GUARD, value greater than 6 disable GUARD
uint8_t atq1070_get_guard(void);
void atq1070_set_guard(uint8_t ch);

// Low Power mode (define time between each key scan) - multiply of 8ms (0 and 1 is equal of 8ms)
uint8_t atq1070_get_lpmode(void);
void atq1070_set_lpmode(uint8_t tim8ms);

// Define max time how long key can be touch, after which channel get recalibrated. 0=disable, default 180 (160ms * 180 = 28.8s)
uint8_t atq1070_get_max_on_duration(void);
void atq1070_set_max_on_duration(uint8_t time160ms);

// force AT42QT1070 to recalibrate all channels
void atq1070_calibrate(void);
// perform a software reset
void atq1070_reset(void);


#endif /* AT42QT1070_H_ */