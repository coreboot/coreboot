#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <cpu/p5/io.h>
#include "serial_subr.h"

/* Base Address */
#define TTYS0 0x3f8

/* Data */
#define TTYS0_RBR (TTYS0+0x00)

/* Control */
#define TTYS0_TBR TTYS0_RBR
#define TTYS0_IER (TTYS0+0x01)
#define TTYS0_IIR (TTYS0+0x02)
#define TTYS0_FCR TTYS0_IIR
#define TTYS0_LCR (TTYS0+0x03)
#define TTYS0_MCR (TTYS0+0x04)
#define TTYS0_DLL TTYS0_RBR
#define TTYS0_DLM TTYS0_IER

/* Status */
#define TTYS0_LSR (TTYS0+0x05)
#define TTYS0_MSR (TTYS0+0x06)
#define TTYS0_SCR (TTYS0+0x07)

void ttys0_tx_char(char data) {
  while (!(inb(TTYS0_LSR) & 0x20));
  outb(data, TTYS0_TBR);
}

void ttys0_tx_string(char *data) {

  while (*data) {
    if (*data=='\n') ttys0_tx_char('\r');
    ttys0_tx_char(*data++);
  }
}

void ttys0_tx_hex_digit(char data) {

  data &= 0x0f;

  if (data>9) {
    data += ('a'-10);
  }
  else {
    data += '0';
  }

  ttys0_tx_char(data);
}

void ttys0_tx_hex(unsigned int data, int digits) {
  int ii;

  /* ttys0_tx_string("0x"); */

  for (ii = 0; ii<digits; ii++) {
    ttys0_tx_hex_digit((char)(data >> ((digits - ii - 1)*4)));
  }
}

void ttys0_tx_int(int data) {
  int ii, i2;
  int digit[30];

  ii = 0;

  do {
    digit[ii] = data % 10;
    data = data/10;
  } while ((ii++ < 30) && data);

  for (i2=0; i2<ii; i2++) {
    ttys0_tx_hex_digit((char)digit[ii-i2-1]);
  }
}
