/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef UART8250REG_H
#define UART8250REG_H

#include <types.h>

/* Data */
#define UART8250_RBR 0x00
#define UART8250_TBR 0x00

/* Control */
#define UART8250_IER 0x01
#define   UART8250_IER_MSI	BIT(3) /* Enable Modem status interrupt */
#define   UART8250_IER_RLSI	BIT(2) /* Enable receiver line status interrupt */
#define   UART8250_IER_THRI	BIT(1) /* Enable Transmitter holding register int. */
#define   UART8250_IER_RDI	BIT(0) /* Enable receiver data interrupt */

#define UART8250_IIR 0x02
#define   UART8250_IIR_NO_INT	0x01 /* No interrupts pending */
#define   UART8250_IIR_ID	0x06 /* Mask for the interrupt ID */

#define   UART8250_IIR_MSI	0x00 /* Modem status interrupt */
#define   UART8250_IIR_THRI	0x02 /* Transmitter holding register empty */
#define   UART8250_IIR_RDI	0x04 /* Receiver data interrupt */
#define   UART8250_IIR_RLSI	0x06 /* Receiver line status interrupt */
#define   UART8250_IIR_FIFO_EN	0xC0 /* FIFO enabled */

#define UART8250_FCR 0x02
#define   UART8250_FCR_FIFO_EN		BIT(0) /* Fifo enable */
#define   UART8250_FCR_CLEAR_RCVR	BIT(1) /* Clear the RCVR FIFO */
#define   UART8250_FCR_CLEAR_XMIT	BIT(2) /* Clear the XMIT FIFO */
#define   UART8250_FCR_DMA_SELECT	BIT(3) /* For DMA applications */
#define   UART8250_FCR_TRIGGER_MASK	(3 << 6) /* Mask for the FIFO trigger range */
#define   UART8250_FCR_TRIGGER_1	(0 << 6) /* Mask for trigger set at 1 */
#define   UART8250_FCR_TRIGGER_4	(1 << 6) /* Mask for trigger set at 4 */
#define   UART8250_FCR_TRIGGER_8	(2 << 6) /* Mask for trigger set at 8 */
#define   UART8250_FCR_TRIGGER_14	(3 << 6) /* Mask for trigger set at 14 */

#define UART8250_LCR 0x03
#define   UART8250_LCR_WLS_MSK	0x03 /* character length select mask */
#define   UART8250_LCR_WLS_5	0x00 /* 5 bit character length */
#define   UART8250_LCR_WLS_6	0x01 /* 6 bit character length */
#define   UART8250_LCR_WLS_7	0x02 /* 7 bit character length */
#define   UART8250_LCR_WLS_8	0x03 /* 8 bit character length */
#define   UART8250_LCR_STB	BIT(2) /* Number of stop Bits, off = 1, on = 1.5 or 2) */
#define   UART8250_LCR_PEN	BIT(3) /* Parity enable */
#define   UART8250_LCR_EPS	BIT(4) /* Even Parity Select */
#define   UART8250_LCR_STKP	BIT(5) /* Stick Parity */
#define   UART8250_LCR_SBRK	BIT(6) /* Set Break */
#define   UART8250_LCR_DLAB	BIT(7) /* Divisor latch access bit */

#define UART8250_MCR 0x04
#define   UART8250_MCR_DTR	BIT(0) /* DTR   */
#define   UART8250_MCR_RTS	BIT(1) /* RTS   */
#define   UART8250_MCR_OUT1	BIT(2) /* Out 1 */
#define   UART8250_MCR_OUT2	BIT(3) /* Out 2 */
#define   UART8250_MCR_LOOP	BIT(4) /* Enable loopback test mode */

#define UART8250_MCR_DMA_EN	0x04
#define UART8250_MCR_TX_DFR	0x08

#define UART8250_DLL 0x00
#define UART8250_DLM 0x01

/* Status */
#define UART8250_LSR 0x05
#define   UART8250_LSR_DR	BIT(0) /* Data ready */
#define   UART8250_LSR_OE	BIT(1) /* Overrun */
#define   UART8250_LSR_PE	BIT(2) /* Parity error */
#define   UART8250_LSR_FE	BIT(3) /* Framing error */
#define   UART8250_LSR_BI	BIT(4) /* Break */
#define   UART8250_LSR_THRE	BIT(5) /* Xmit holding register empty */
#define   UART8250_LSR_TEMT	BIT(6) /* Xmitter empty */
#define   UART8250_LSR_ERR	BIT(7) /* Error */

#define UART8250_MSR 0x06
#define   UART8250_MSR_DCD	BIT(7) /* Data Carrier Detect */
#define   UART8250_MSR_RI	BIT(6) /* Ring Indicator */
#define   UART8250_MSR_DSR	BIT(5) /* Data Set Ready */
#define   UART8250_MSR_CTS	BIT(4) /* Clear to Send */
#define   UART8250_MSR_DDCD	BIT(3) /* Delta DCD */
#define   UART8250_MSR_TERI	BIT(2) /* Trailing edge ring indicator */
#define   UART8250_MSR_DDSR	BIT(1) /* Delta DSR */
#define   UART8250_MSR_DCTS	BIT(0) /* Delta CTS */

#define UART8250_SCR 0x07
#define UART8250_SPR 0x07

#endif /* UART8250REG_H */
