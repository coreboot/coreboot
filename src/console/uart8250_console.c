#include <console/console.h>
#include <uart8250.h>
#include <pc80/mc146818rtc.h>

/* Base Address */
#ifndef CONFIG_TTYS0_BASE
#define CONFIG_TTYS0_BASE 0x3f8
#endif

#ifndef CONFIG_TTYS0_BAUD
#define CONFIG_TTYS0_BAUD 115200
#endif

#ifndef CONFIG_TTYS0_DIV
#if ((115200%CONFIG_TTYS0_BAUD) != 0)
#error Bad ttys0 baud rate
#endif
#define CONFIG_TTYS0_DIV	(115200/CONFIG_TTYS0_BAUD)
#endif

/* Line Control Settings */
#ifndef CONFIG_TTYS0_LCS
/* Set 8bit, 1 stop bit, no parity */
#define CONFIG_TTYS0_LCS	0x3
#endif

#define UART_LCS	CONFIG_TTYS0_LCS

static void ttyS0_init(void)
{
	static const unsigned char div[8]={1,2,3,6,12,24,48,96};
	int b_index=0;
	unsigned int divisor=CONFIG_TTYS0_DIV;

	if(get_option(&b_index,"baud_rate")==0) {
		divisor=div[b_index];
	}
	uart8250_init(CONFIG_TTYS0_BASE, divisor, CONFIG_TTYS0_LCS);
}

static void ttyS0_tx_byte(unsigned char data) 
{
	uart8250_tx_byte(CONFIG_TTYS0_BASE, data);
}

static unsigned char ttyS0_rx_byte(void) 
{
	return uart8250_rx_byte(CONFIG_TTYS0_BASE);
}

static int ttyS0_tst_byte(void) 
{
	return uart8250_can_rx_byte(CONFIG_TTYS0_BASE);
}

static const struct console_driver uart8250_console __console = {
	.init    = ttyS0_init,
	.tx_byte = ttyS0_tx_byte,
	.rx_byte = ttyS0_rx_byte,
	.tst_byte = ttyS0_tst_byte,
};

