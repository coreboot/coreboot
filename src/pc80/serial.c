#include <lib.h> /* Prototypes */

/* Base Address */
#ifndef CONFIG_TTYS0_BASE
#define CONFIG_TTYS0_BASE 0x3f8
#endif

#ifndef CONFIG_TTYS0_BAUD
#define CONFIG_TTYS0_BAUD 115200
#endif

#if ((115200%CONFIG_TTYS0_BAUD) != 0)
#error Bad ttys0 baud rate
#endif

#ifndef CONFIG_TTYS0_DIV
#define CONFIG_TTYS0_DIV	(115200/CONFIG_TTYS0_BAUD)
#endif

/* Line Control Settings */
#ifndef CONFIG_TTYS0_LCS
/* Set 8bit, 1 stop bit, no parity */
#define CONFIG_TTYS0_LCS	0x3
#endif

#define UART_LCS	CONFIG_TTYS0_LCS


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
	return inb(CONFIG_TTYS0_BASE + UART_LSR) & 0x20;
}

static void uart_wait_to_tx_byte(void)
{
	while(!uart_can_tx_byte()) 
	;
}

static void uart_wait_until_sent(void)
{
	while(!(inb(CONFIG_TTYS0_BASE + UART_LSR) & 0x40))
	; 
}

static void uart_tx_byte(unsigned char data)
{
	uart_wait_to_tx_byte();
	outb(data, CONFIG_TTYS0_BASE + UART_TBR);
	/* Make certain the data clears the fifos */
	uart_wait_until_sent();
}

void uart_init(void)
{
	/* disable interrupts */
	outb(0x0, CONFIG_TTYS0_BASE + UART_IER);
	/* enable fifo's */
	outb(0x01, CONFIG_TTYS0_BASE + UART_FCR);
	/* Set Baud Rate Divisor to 12 ==> 115200 Baud */
	outb(0x80 | UART_LCS, CONFIG_TTYS0_BASE + UART_LCR);
#if CONFIG_USE_OPTION_TABLE == 1
	static const unsigned char divisor[] = { 1,2,3,6,12,24,48,96 };
	unsigned ttys0_div, ttys0_index;
	ttys0_index = read_option(CMOS_VSTART_baud_rate, CMOS_VLEN_baud_rate, 0);
	ttys0_index &= 7;
	ttys0_div = divisor[ttys0_index];
	outb(ttys0_div & 0xff, CONFIG_TTYS0_BASE + UART_DLL);
	outb(0, CONFIG_TTYS0_BASE + UART_DLM);
#else
	outb(CONFIG_TTYS0_DIV & 0xFF,   CONFIG_TTYS0_BASE + UART_DLL);
	outb((CONFIG_TTYS0_DIV >> 8) & 0xFF,    CONFIG_TTYS0_BASE + UART_DLM);
#endif
	outb(UART_LCS, CONFIG_TTYS0_BASE + UART_LCR);
}

#else
/* CONFIG_USE_PRINTK_IN_CAR == 1 */

extern void uart8250_init(unsigned base_port, unsigned divisor, unsigned lcs);
void uart_init(void)
{
#if CONFIG_USE_OPTION_TABLE == 1
        static const unsigned char divisor[] = { 1,2,3,6,12,24,48,96 };
        unsigned ttys0_div, ttys0_index;
        ttys0_index = read_option(CMOS_VSTART_baud_rate, CMOS_VLEN_baud_rate, 0);
        ttys0_index &= 7;
        ttys0_div = divisor[ttys0_index];
	uart8250_init(CONFIG_TTYS0_BASE, ttys0_div, UART_LCS);
#else
	uart8250_init(CONFIG_TTYS0_BASE, CONFIG_TTYS0_DIV, UART_LCS);
#endif	
}
#endif
