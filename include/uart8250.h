#ifndef UART8250_H
#define UART8250_H

/* Base Address */
#if defined(CONFIG_CONSOLE_SERIAL_COM1)
#define TTYSx_BASE 0x3f8
#elif defined(CONFIG_CONSOLE_SERIAL_COM2)
#define TTYSx_BASE 0x2f8
#else
#define TTYSx_BASE 0x3f8
#warning no serial port set
#endif

#if defined(CONFIG_CONSOLE_SERIAL_115200)
#define TTYSx_BAUD 115200
#elif defined(CONFIG_CONSOLE_SERIAL_57600)
#define TTYSx_BAUD 57600
#elif defined(CONFIG_CONSOLE_SERIAL_38400)
#define TTYSx_BAUD 38400
#elif defined(CONFIG_CONSOLE_SERIAL_19200)
#define TTYSx_BAUD 19200
#elif defined(CONFIG_CONSOLE_SERIAL_9600)
#define TTYSx_BAUD 9600
#else				// default
#define TTYSx_BAUD 115200
#warning no serial speed set
#endif

#if ((115200%TTYSx_BAUD) != 0)
#error Bad ttys0 baud rate
#endif

#define TTYSx_DIV	(115200/TTYSx_BAUD)

/* Line Control Settings */
#ifndef TTYSx_LCS
/* Set 8bit, 1 stop bit, no parity */
#define TTYSx_LCS	0x3
#endif

#define UART_LCS	TTYSx_LCS

unsigned char uart8250_rx_byte(unsigned base_port);
int uart8250_can_rx_byte(unsigned base_port);
void uart8250_tx_byte(unsigned base_port, unsigned char data);
void uart8250_init(unsigned base_port, unsigned divisor, unsigned lcs);

#endif	/* UART8250_H */
