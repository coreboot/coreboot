#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <arch/io.h>
#include <serial_subr.h>

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

static inline int uart_can_tx_byte(unsigned base_port)
{
	return inb(base_port + UART_LSR) & 0x20;
}

static inline void uart_wait_to_tx_byte(unsigned base_port)
{
	while(!uart_can_tx_byte(base_port))
		;
}

static inline void uart_wait_until_sent(unsigned base_port)
{
	while(!(inb(base_port + UART_LSR) & 0x40)) 
		;
}

static inline void uart_tx_byte(unsigned base_port, unsigned char data)
{
	uart_wait_to_tx_byte(base_port);
	outb(data, base_port + UART_TBR);
	/* Make certain the data clears the fifos */
	uart_wait_until_sent(base_port);
}

static inline void uart_tx_bytes(unsigned base_port, char *data, unsigned len)
{
	do {
		uart_wait_to_tx_byte(base_port);
		outb(*data, base_port + UART_TBR);
		data++;
		len--;
	} while(len);
	uart_wait_until_sent(base_port);
}


static inline int uart_have_rx_byte(unsigned base_port)
{
	return inb(base_port + UART_LSR) & 0x1;
}

static inline void uart_enable_rx_byte(unsigned base_port)
{
	unsigned char byte;
	/* say we are ready for a byte */
	byte = inb(base_port + UART_MCR);
	byte |= 0x02;
	outb(byte, base_port + UART_MCR);
}

static inline void uart_disable_rx_byte(unsigned base_port)
{
	unsigned char byte;
	/* say we aren't ready for another byte */
	byte = inb(base_port + UART_MCR);
	byte &= ~0x02;
	outb(byte, base_port + UART_MCR);
}

static inline void uart_wait_for_rx_byte(unsigned base_port)
{
	uart_enable_rx_byte(base_port);
	while(!uart_have_rx_byte(base_port))
		;
	uart_disable_rx_byte(base_port);
}

static inline unsigned char uart_rx_byte(unsigned base_port)
{
	unsigned char data;
	if (!uart_have_rx_byte(base_port)) {
		uart_wait_for_rx_byte(base_port);
	}
	data = inb(base_port + UART_RBR);
	return data;
}

static inline unsigned long uart_rx_bytes(unsigned base_port, 
	char * buffer, unsigned long size)
{
	unsigned long bytes = 0;
	if (size == 0) {
		return 0;
	}
	if (!uart_have_rx_byte(base_port)) {
		uart_wait_for_rx_byte(base_port);
	}
	do {
		buffer[bytes++] = inb(base_port + UART_RBR);
	} while((bytes < size) && uart_have_rx_byte(base_port));
	return bytes;
}

inline void uart_init(unsigned base_port, unsigned divisor)
{
	/* disable interrupts */
	outb(0x0, base_port + UART_IER);
	/* enable fifo's */
	outb(0x01, base_port + UART_FCR);
	/* Set Baud Rate Divisor to 12 ==> 115200 Baud */
	outb(0x80 | UART_LCS, base_port + UART_LCR);
	outb(divisor & 0xFF,   base_port + UART_DLL);
	outb((divisor >> 8) & 0xFF,    base_port + UART_DLM);
	outb(UART_LCS, base_port + UART_LCR);
}

void ttys0_init(void)
{
	uart_init(TTYS0_BASE, TTYS0_DIV);
}

void ttys0_tx_byte(unsigned char data) 
{
	uart_tx_byte(TTYS0_BASE, data);
}

unsigned char ttys0_rx_byte(void)
{
	return uart_rx_byte(TTYS0_BASE);
}

unsigned long ttys0_rx_bytes(char *buffer, unsigned long size)
{
	return uart_rx_bytes(TTYS0_BASE, buffer, size);
}

#ifdef PYRO_SERIAL
/* experimental serial read stuffs */
int iskey(void) 
{
	return uart_have_rx_byte(TTYS0_BASE);
}

char ttys0_rx_char(void) 
{
	return ttys0_rx_byte();
}

void ttys0_rx_line(char *buffer, int *len) 
{
	int pos=0;
	char chargot=0;

	while(chargot != '\r' && chargot != '\n' && pos< *len) {
		chargot = ttys0_rx_char();
		buffer[pos++] = chargot;
	}

	*len = pos-1;
}
#endif
