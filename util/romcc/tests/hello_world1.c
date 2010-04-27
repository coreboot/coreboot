void outb(unsigned char value, unsigned short port)
{
	__builtin_outb(value, port);
}

unsigned char inb(unsigned short port)
{
	return __builtin_inb(port);
}

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

#if CONFIG_TTYS0_BAUD == 115200
#define CONFIG_TTYS0_DIV (1)
#else
#define CONFIG_TTYS0_DIV	(115200/CONFIG_TTYS0_BAUD)
#endif

/* Line Control Settings */
#ifndef CONFIG_TTYS0_LCS
/* Set 8bit, 1 stop bit, no parity */
#define CONFIG_TTYS0_LCS	0x3
#endif

#define UART_LCS	CONFIG_TTYS0_LCS

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

int uart_can_tx_byte(void)
{
	return inb(CONFIG_TTYS0_BASE + UART_LSR) & 0x20;
}

void uart_wait_to_tx_byte(void)
{
	while(!uart_can_tx_byte())
		;
}

void uart_wait_until_sent(void)
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
	outb(CONFIG_TTYS0_DIV & 0xFF,   CONFIG_TTYS0_BASE + UART_DLL);
	outb((CONFIG_TTYS0_DIV >> 8) & 0xFF,    CONFIG_TTYS0_BASE + UART_DLM);
	outb(UART_LCS, CONFIG_TTYS0_BASE + UART_LCR);
}


void __console_tx_char(unsigned char byte)
{
	uart_tx_byte(byte);

}

void __console_tx_string(char *str)
{
	unsigned char ch;
	while((ch = *str++) != '\0') {
		__console_tx_char(ch);
	}
}


void print_debug_char(unsigned char byte) { __console_tx_char(byte); }
void print_debug(char *str) { __console_tx_string(str); }

void main(void)
{
	static const char msg[] = "hello world\r\n";
	uart_init();
#if 0
	print_debug(msg);
#endif
#if 1
	print_debug("hello world\r\n");
	print_debug("how are you today\r\n");
#endif
	while(1) {
		;
	}
}
