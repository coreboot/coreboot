#ifndef SUBR_H_
#define SUBR_H_

#include <stdint.h>

void displayinit(void);
void display(char msg[]);
void display_tx_byte(unsigned char byte);
void display_tx_break(void);

void error(char errmsg[]);
void post_code(uint8_t value);

#endif /* SUBR_H_ */
