#ifndef UART8250_H
#define UART8250_H

struct uart8250 {
	unsigned int baud;
	/* Do I need an lcs parameter here? */
};

void uart8250_tx_byte(unsigned base_port, unsigned char data);
void uart8250_init(unsigned base_port, unsigned divisor, unsigned lcs);
void init_uart8250(unsigned base_port, struct uart8250 *uart);

#endif /* UART8250_H */
