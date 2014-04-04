/*
 * Definitions for UART on Allwinner CPUs
 *
 * The UART on the A10 seems to be 8250-compatible, however, this has not been
 * verified. Our 8250mem code is specific to x86, and does not yet work, so we
 * have to re-implement it ARM-style for the time being. The register
 * definitions are present in <uart7250.h>, and are not redefined here.
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#ifndef CPU_ALLWINNER_A10_UART_H
#define CPU_ALLWINNER_A10_UART_H

#include <types.h>

struct a10_uart {
	union {
		/* operational mode */
		u32 rbr;	/* receiver buffer (read) */
		u32 thr;	/* transmit holding (write) */
		/* config mode (DLAB set) */
		u32 dll;	/* divisor latches low */
	};

	union {
		/* operational mode */
		u32 ier;	/* interrupt enable */
		/* config mode (DLAB set) */
		u32 dlh;	/* divisor latches high */
	};

	union {
		u32 iir;	/* interrupt ID (read) */
		u32 fcr;	/* FIFO control (write) */
	};

	u32 lcr;		/* line control */

	/* 0x10 */
	u32 mcr;		/* modem control */
	u32 lsr;		/* line status, read-only */
	u32 msr;		/* modem status */
	u32 sch;		/* scratch register */

	u8 reserved_0x20[0x50];

	/* 0x70 */
	u8 reserved_0x70[0xc];
	u32 usr;		/* UART status register */

	/* 0x80 */
	u32 tfl;		/* Transmit FIFO level */
	u32 rfl;		/* Receive FIFO level */
	u8 reserved_0x88[0x18];

	/* 0xa0 */
	u8 reserved_0xa0[4];
	u32 halt;		/* Halt register */

} __attribute__ ((packed));

enum uart_parity {
	UART_PARITY_NONE,
	UART_PARITY_EVEN,
	UART_PARITY_ODD,
};

#endif				/* CPU_ALLWINNER_A10_UART_H */
