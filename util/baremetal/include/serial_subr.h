#ifndef _SERIAL_SUBR_H_
#define _SERIAL_SUBR_H_

void ttys0_init(void);
void ttys0_tx_byte(unsigned char data);
unsigned char ttys0_rx_byte(void);
unsigned long ttys0_rx_bytes(char *buffer, unsigned long size);
int iskey(void);

void uart_init(unsigned base_port, unsigned divisor);
#endif /* _SERIAL_SUBR_H_ */
