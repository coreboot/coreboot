#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <arch/io.h>
#include <serial_subr.h>

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

#ifndef TTYS0_BAUD
#define TTYS0_BAUD 115200
#endif

#if ((115200%TTYS0_BAUD) != 0)
#error Bad ttys0 baud rate
#endif

#define TTYS0_DIV	(115200/TTYS0_BAUD)

void ttys0_init(void)
{
	/* disable interrupts */
	outb(0x0, TTYS0_IER);
	/* disable fifo's */
	outb(0x0, TTYS0_FCR);
	outb(0x83, TTYS0_LCR);
	/* Set Baud Rate Divisor to 12 ==> 115200 Baud */
	outb(TTYS0_DIV,   TTYS0_DLL);
	outb(0,    TTYS0_DLM);
	outb(0x03, TTYS0_LCR);
}

void ttys0_tx_byte(unsigned char data) 
{
	while (!(inb(TTYS0_LSR) & 0x20))
		;
	outb(data, TTYS0_TBR);
}

