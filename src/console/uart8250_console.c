#include <console/console.h>
#include <uart8250.h>
#include <pc80/mc146818rtc.h>

/* Base Address */
#ifndef TTYS0_BASE
#define TTYS0_BASE 0x3f8
#endif

#ifndef TTYS0_BAUD
#define TTYS0_BAUD 115200
#endif

#ifndef TTYS0_DIV
#if ((115200%TTYS0_BAUD) != 0)
#error Bad ttys0 baud rate
#endif
#define TTYS0_DIV	(115200/TTYS0_BAUD)
#endif

/* Line Control Settings */
#ifndef TTYS0_LCS
/* Set 8bit, 1 stop bit, no parity */
#define TTYS0_LCS	0x3
#endif

#define UART_LCS	TTYS0_LCS

void ttyS0_init(void)
{
	static unsigned char div[8]={1,2,3,6,12,24,48,96};
	int b_index=0;
	unsigned int divisor=TTYS0_DIV;

	if(get_option(&b_index,"baud_rate")==0) {
		divisor=div[b_index];
	}
	uart8250_init(TTYS0_BASE, divisor, TTYS0_LCS);
}

void ttyS0_tx_byte(unsigned char data) 
{
	uart8250_tx_byte(TTYS0_BASE, data);
}

static struct console_driver uart8250_console __console = {
	.init    = ttyS0_init,
	.tx_byte = ttyS0_tx_byte,
};

