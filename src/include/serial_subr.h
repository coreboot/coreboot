#ifndef _SERIAL_SUBR_H_
#define _SERIAL_SUBR_H_

void ttys0_tx_char(char data);
void ttys0_tx_string(char *data);
void ttys0_tx_hex_digit(char data);
void ttys0_tx_hex(unsigned int data, int digits);
void ttys0_tx_int(int data);

#endif /* _SERIAL_SUBR_H_ */
