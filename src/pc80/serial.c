#include <part/fallback_boot.h>

/* Base Address */
#ifndef TTYS0_BASE
#define TTYS0_BASE 0x3f8
#endif

#ifndef TTYS0_BAUD
#define TTYS0_BAUD 115200
#endif

#if ((115200%TTYS0_BAUD) != 0)
#error Bad ttys0 baud rate
#endif

#define TTYS0_DIV	(115200/TTYS0_BAUD)

/* Line Control Settings */
#ifndef TTYS0_LCS
/* Set 8bit, 1 stop bit, no parity */
#define TTYS0_LCS	0x3
#endif

#define UART_LCS	TTYS0_LCS


#if CONFIG_USE_PRINTK_IN_CAR == 0

/* Data */
#define UART_RBR 0x00
#define UART_TBR 0x00

/* Control */
#define UART_IER 0x01
#define UART_IIR 0x02
#define UART_FCR 0x02
#define UART_LCR 0x03
#define UART_MCR 0x04
#define UART_DLL 0x00
#define UART_DLM 0x01

/* Status */
#define UART_LSR 0x05
#define UART_MSR 0x06
#define UART_SCR 0x07

static int uart_can_tx_byte(void)
{
	return inb(TTYS0_BASE + UART_LSR) & 0x20;
}

static void uart_wait_to_tx_byte(void)
{
	while(!uart_can_tx_byte()) 
	;
}

static void uart_wait_until_sent(void)
{
	while(!(inb(TTYS0_BASE + UART_LSR) & 0x40))
	; 
}

static void uart_tx_byte(unsigned char data)
{
	uart_wait_to_tx_byte();
	outb(data, TTYS0_BASE + UART_TBR);
	/* Make certain the data clears the fifos */
	uart_wait_until_sent();
}

static void uart_init(void)
{
	/* disable interrupts */
	outb(0x0, TTYS0_BASE + UART_IER);
	/* enable fifo's */
	outb(0x01, TTYS0_BASE + UART_FCR);
	/* Set Baud Rate Divisor to 12 ==> 115200 Baud */
	outb(0x80 | UART_LCS, TTYS0_BASE + UART_LCR);
#if USE_OPTION_TABLE == 1
	static const unsigned char divisor[] = { 1,2,3,6,12,24,48,96 };
	unsigned ttys0_div, ttys0_index;
	ttys0_index = read_option(CMOS_VSTART_baud_rate, CMOS_VLEN_baud_rate, 0);
	ttys0_index &= 7;
	ttys0_div = divisor[ttys0_index];
	outb(ttys0_div & 0xff, TTYS0_BASE + UART_DLL);
	outb(0, TTYS0_BASE + UART_DLM);
#else
	outb(TTYS0_DIV & 0xFF,   TTYS0_BASE + UART_DLL);
	outb((TTYS0_DIV >> 8) & 0xFF,    TTYS0_BASE + UART_DLM);
#endif
	outb(UART_LCS, TTYS0_BASE + UART_LCR);
}

#else
/* CONFIG_USE_PRINTK_IN_CAR == 1 */
#if CONFIG_USE_INIT == 0
#include "../lib/uart8250.c"
#endif

extern void uart8250_init(unsigned base_port, unsigned divisor, unsigned lcs);
static void uart_init(void)
{
#if USE_OPTION_TABLE == 1
        static const unsigned char divisor[] = { 1,2,3,6,12,24,48,96 };
        unsigned ttys0_div, ttys0_index;
        ttys0_index = read_option(CMOS_VSTART_baud_rate, CMOS_VLEN_baud_rate, 0);
        ttys0_index &= 7;
        ttys0_div = divisor[ttys0_index];
	uart8250_init(TTYS0_BASE, ttys0_div, UART_LCS);
#else
	uart8250_init(TTYS0_BASE, TTYS0_DIV, UART_LCS);
#endif	
}
#endif
