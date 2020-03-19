#ifndef DELAY_H_INCLUDED
#define DELAY_H_INCLUDED

#define NOP() {asm volatile("nop"::);}

#define nop asm volatile("nop"::);

void delay1ms(uint16_t t);
void delay1us(uint16_t t);

#endif // delay_h_included

