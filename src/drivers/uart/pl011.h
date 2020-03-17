/*
 * This file is part of the coreboot project.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __DRIVERS_UART_PL011_H
#define __DRIVERS_UART_PL011_H

#include <stdint.h>
#include <types.h>

/* PL011 r1p5 registers */
struct pl011_uart {
	u32 dr;
	u32 rsr_ecr;
	u8 rsvd1[0x10];
	u32 fr;
	u8 rsvd2[0x4];
	u32 ilpr;
	u32 ibrd;
	u32 fbrd;
	u32 lcr_h;
	u32 cr;
	u32 ifls;
	u32 imsc;
	u32 ris;
	u32 mis;
	u32 icr;
	u32 dmacr;
	u8 rsvd3[0xf94];
	u32 periphid0;
	u32 periphid1;
	u32 periphid2;
	u32 periphid3;
	u32 cellid0;
	u32 cellid1;
	u32 cellid2;
	u32 cellid3;
};
check_member(pl011_uart, cellid3, 0xffc);

/*************************************************************************/
/* Bit definitions from arm-trusted-firmware/include/drivers/arm/pl011.h */
/*************************************************************************/
/* Flag reg bits */
#define PL011_UARTFR_RI		(1 << 8)	/* Ring indicator */
#define PL011_UARTFR_TXFE	(1 << 7)	/* Transmit FIFO empty */
#define PL011_UARTFR_RXFF	(1 << 6)	/* Receive  FIFO full */
#define PL011_UARTFR_TXFF	(1 << 5)	/* Transmit FIFO full */
#define PL011_UARTFR_RXFE	(1 << 4)	/* Receive FIFO empty */
#define PL011_UARTFR_BUSY	(1 << 3)	/* UART busy */
#define PL011_UARTFR_DCD	(1 << 2)	/* Data carrier detect */
#define PL011_UARTFR_DSR	(1 << 1)	/* Data set ready */
#define PL011_UARTFR_CTS	(1 << 0)	/* Clear to send */

#define PL011_UARTFR_TXFF_BIT	5		/* Transmit FIFO full bit in
						   UARTFR register */
#define PL011_UARTFR_RXFE_BIT	4		/* Receive FIFO empty bit in
						   UARTFR register */
#define PL011_UARTFR_BUSY_BIT	3		/* UART busy bit in UARTFR
						   register */

/* Control reg bits */
#define PL011_UARTCR_CTSEN	(1 << 15)	/* CTS hardware flow control
						   enable */
#define PL011_UARTCR_RTSEN	(1 << 14)	/* RTS hardware flow control
						   enable */
#define PL011_UARTCR_RTS	(1 << 11)	/* Request to send */
#define PL011_UARTCR_DTR	(1 << 10)	/* Data transmit ready. */
#define PL011_UARTCR_RXE	(1 << 9)	/* Receive enable */
#define PL011_UARTCR_TXE	(1 << 8)	/* Transmit enable */
#define PL011_UARTCR_LBE	(1 << 7)	/* Loopback enable */
#define PL011_UARTCR_UARTEN	(1 << 0)	/* UART Enable */

/* FIFO Enabled / No Parity / 8 Data bit / One Stop Bit */
#define PL011_LINE_CONTROL	(PL011_UARTLCR_H_FEN | PL011_UARTLCR_H_WLEN_8)

/* Line Control Register Bits */
#define PL011_UARTLCR_H_SPS	(1 << 7)	/* Stick parity select */
#define PL011_UARTLCR_H_WLEN_8	(3 << 5)
#define PL011_UARTLCR_H_WLEN_7	(2 << 5)
#define PL011_UARTLCR_H_WLEN_6	(1 << 5)
#define PL011_UARTLCR_H_WLEN_5	(0 << 5)
#define PL011_UARTLCR_H_FEN	(1 << 4)	/* FIFOs Enable */
#define PL011_UARTLCR_H_STP2	(1 << 3)	/* Two stop bits select */
#define PL011_UARTLCR_H_EPS	(1 << 2)	/* Even parity select */
#define PL011_UARTLCR_H_PEN	(1 << 1)	/* Parity Enable */
#define PL011_UARTLCR_H_BRK	(1 << 0)	/* Send break */

#endif				/* ! __DRIVERS_UART_PL011_H */
