#ifndef UART8250_H
#define UART8250_H

void uart8250_tx_byte(unsigned base_port, unsigned char data);
void uart8250_init(unsigned base_port, unsigned divisor, unsigned lcs);

#endif /* UART8250_H */
