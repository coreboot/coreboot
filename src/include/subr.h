#ifndef SUBR_H_
#define SUBR_H_

#include <stdint.h>

void displayinit(void);
void display(char msg[]);
void error(char errmsg[]);
void post_code(uint8_t value);

#endif /* SUBR_H_ */
