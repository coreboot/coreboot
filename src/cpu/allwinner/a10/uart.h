/*
 * Definitions for UART on Allwinner CPUs
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#ifndef CPU_ALLWINNER_A10_UART_H
#define CPU_ALLWINNER_A10_UART_H

#include "memmap.h"
#include <types.h>

/* UART_FCR definitions */
#define UART_FCR_DMAM		(1 << 3)
#define UART_FCR_XFIFOR		(1 << 2)
#define UART_FCR_RFIFOR		(1 << 1)
#define UART_FCR_FIFOE		(1 << 0)

/* UART_LCR definitions */
#define UART_LCR_DLAB		(1 << 7)
#define UART_LCR_BC		(1 << 6)
#define UART_LCR_EPS		(1 << 4)
#define UART_LCR_PEN		(1 << 3)
#define UART_LCR_STOP_MASK	(0x3 << 2)
#define  UART_LCR_STOP_1BIT	(0 << 2)
#define  UART_LCR_STOP_2BIT	(1 << 2)
#define UART_LCR_DLS_MASK	(3 << 0)
#define UART_LCR_DATALEN(bits)	((bits - 5) & UART_LCR_DLS_MASK)

/* UART_LSR definitions */
#define UART_LSR_FIFOERR	(1 << 7)
#define UART_LSR_TEMT		(1 << 6)
#define UART_LSR_THRE		(1 << 5)
#define UART_LSR_BI		(1 << 4)
#define UART_LSR_FE		(1 << 3)
#define UART_LSR_PE		(1 << 2)
#define UART_LSR_OE		(1 << 1)
#define UART_LSR_DR		(1 << 0)

#define BAUD_115200    (0xd)

struct a10_uart {
	union {
		/* operational mode */
		u32 rbr;	/* receiver buffer (read) */
		u32 thr;	/* transmit holding (write) */
		/* config mode A and B */
		u32 dll;	/* divisor latches low */
	};

	union {
		/* operational mode */
		u32 ier;	/* interrupt enable */
		/* config mode A and B */
		u32 dlh;	/* divisor latches high */
	};

	union {
		/* operational mode, config mode A */
		u32 iir;	/* interrupt ID (read) */
		u32 fcr;	/* FIFO control (write) */
	};

	u32 lcr;		/* line control */

	/* 0x10 */
	u32 mcr;		/* modem control */
	u32 lsr;		/* line status, read-only */
	u32 msr;		/* modem status */
	u32 sch;		/* trigger level */

	u8 reserved_0x20[0x50];

	/* 0x70 */
	u8 reserved_0x70[0xc];
	u32 usr;		/* frequency select */

	/* 0x80 */
	u32 tfl;		/* mode definition register 3 */
	u32 rfl;
	u8 reserved_0x88[0x18];

	/* 0xa0 */
	u8 reserved_0xa0[4];
	u32 halt;		/* TX DMA threshold */

} __attribute__ ((packed));

enum uart_parity {
	UART_PARITY_NONE,
	UART_PARITY_EVEN,
	UART_PARITY_ODD,
};

void a10_uart_configure(void *uart_base, u32 baud_rate, u8 data_bits,
			enum uart_parity parity, u8 stop_bits);
void a10_uart_enable_fifos(void *uart_base);
u8 a10_uart_rx_blocking(void *uart_base);
void a10_uart_tx_blocking(void *uart_base, u8 data);

#endif				/* CPU_ALLWINNER_A10_UART_H */
