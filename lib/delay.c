// ----------------------------------------------
// OpóŸnienia

#include <avr/io.h>

#include "delay.h"

// ---------------- teoretycznie dok³adniejsze funkcje :))

// set to 1 if F_CPU is 25MHz else must be 0
#define FCPU25MHZ_ON 0
// set to 1 if F_CPU is 20MHz else must be 0
#define FCPU20MHZ_ON 0

#if (!defined(F_CPU))
	#error F_CPU not defined in compiler parameters!
#endif

void delay1us(uint16_t t) {
	while (t>0) {
		#if FCPU25MHZ_ON == 1
		for (uint8_t f25mhz=20;f25mhz>0;f25mhz--) { 
			asm volatile("nop"::);
		}
		#elif FCPU20MHZ_ON == 1
		for (uint8_t f20mhz=20;f20mhz>0;f20mhz--) { 
			asm volatile("nop"::);
		}
		#else
			// ~250ns (271)
			#if F_CPU >= 4000000
				asm volatile("nop"::);
				asm volatile("nop"::);
				asm volatile("nop"::);
			#endif
			
			#if F_CPU >= 6000000
				asm volatile("nop"::);
				asm volatile("nop"::);
			#endif
			
			#if F_CPU > 8000000
				asm volatile("nop"::);
				asm volatile("nop"::);
			#endif
			
			#if F_CPU > 10000000
				asm volatile("nop"::);
				asm volatile("nop"::);
			#endif
			
			#if F_CPU > 12000000
				asm volatile("nop"::);
				asm volatile("nop"::);
			#endif
			
			#if F_CPU > 14000000
				asm volatile("nop"::);
				asm volatile("nop"::);
			#endif
			
			#if F_CPU > 16000000
				asm volatile("nop"::);
				asm volatile("nop"::);
			#endif
			
			// ~250ns (271)
			//asm volatile("nop"::);
			//asm volatile("nop"::);
			//asm volatile("nop"::);
			//asm volatile("nop"::);

			// ~250ns (271)
			//asm volatile("nop"::);
			//asm volatile("nop"::);
			//asm volatile("nop"::);
			//asm volatile("nop"::);

			// ~250ns (271)
			//asm volatile("nop"::);
			//asm volatile("nop"::);
			//asm volatile("nop"::);
			//asm volatile("nop"::);

		#endif
		--t;
	}
}

// /*
void delay1ms(uint16_t t) {
	while (t>0) {
		delay1us(995);
		--t;
	}
}
// */
