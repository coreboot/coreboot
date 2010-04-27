/* Should support 8250, 16450, 16550, 16550A type uarts */
#include <arch/io.h>
#include <uart8250.h>

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

static inline int uart8250_can_tx_byte(unsigned base_port)
{
	return inb(base_port + UART_LSR) & 0x20;
}

static inline void uart8250_wait_to_tx_byte(unsigned base_port)
{
	while(!uart8250_can_tx_byte(base_port))
		;
}

static inline void uart8250_wait_until_sent(unsigned base_port)
{
	while(!(inb(base_port + UART_LSR) & 0x40))
		;
}

void uart8250_tx_byte(unsigned base_port, unsigned char data)
{
	uart8250_wait_to_tx_byte(base_port);
	outb(data, base_port + UART_TBR);
	/* Make certain the data clears the fifos */
	uart8250_wait_until_sent(base_port);
}

int uart8250_can_rx_byte(unsigned base_port)
{
	return inb(base_port + UART_LSR) & 0x01;
}

unsigned char uart8250_rx_byte(unsigned base_port)
{
	while(!uart8250_can_rx_byte(base_port))
		;
	return inb(base_port + UART_RBR);
}

void uart8250_init(unsigned base_port, unsigned divisor, unsigned lcs)
{
	lcs &= 0x7f;
	/* disable interrupts */
	outb(0x0, base_port + UART_IER);
	/* enable fifo's */
	outb(0x01, base_port + UART_FCR);
	/* assert DTR and RTS so the other end is happy */
	outb(0x03, base_port + UART_MCR);
	/* Set Baud Rate Divisor to 12 ==> 115200 Baud */
	outb(0x80 | lcs, base_port + UART_LCR);
	outb(divisor & 0xFF,   base_port + UART_DLL);
	outb((divisor >> 8) & 0xFF,    base_port + UART_DLM);
	outb(lcs, base_port + UART_LCR);
}

/* Initialize a generic uart */
void init_uart8250(unsigned base_port, struct uart8250 *uart)
{
	int divisor;
	int lcs;
	divisor = 115200/(uart->baud ? uart->baud: 1);
	lcs = 3;
	if (base_port == CONFIG_TTYS0_BASE) {
		/* Don't reinitialize the console serial port,
		 * This is espeically nasty in SMP.
		 */
		return;
	}
	uart8250_init(base_port, divisor, lcs);
}
