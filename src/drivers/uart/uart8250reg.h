/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric Biederman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef UART8250REG_H
#define UART8250REG_H

/* Data */
#define UART8250_RBR 0x00
#define UART8250_TBR 0x00

/* Control */
#define UART8250_IER 0x01
#define   UART8250_IER_MSI	0x08 /* Enable Modem status interrupt */
#define   UART8250_IER_RLSI	0x04 /* Enable receiver line status interrupt */
#define   UART8250_IER_THRI	0x02 /* Enable Transmitter holding register int. */
#define   UART8250_IER_RDI	0x01 /* Enable receiver data interrupt */

#define UART8250_IIR 0x02
#define   UART8250_IIR_NO_INT	0x01 /* No interrupts pending */
#define   UART8250_IIR_ID	0x06 /* Mask for the interrupt ID */

#define   UART8250_IIR_MSI	0x00 /* Modem status interrupt */
#define   UART8250_IIR_THRI	0x02 /* Transmitter holding register empty */
#define   UART8250_IIR_RDI	0x04 /* Receiver data interrupt */
#define   UART8250_IIR_RLSI	0x06 /* Receiver line status interrupt */

#define UART8250_FCR 0x02
#define   UART8250_FCR_FIFO_EN		0x01 /* Fifo enable */
#define   UART8250_FCR_CLEAR_RCVR	0x02 /* Clear the RCVR FIFO */
#define   UART8250_FCR_CLEAR_XMIT	0x04 /* Clear the XMIT FIFO */
#define   UART8250_FCR_DMA_SELECT	0x08 /* For DMA applications */
#define   UART8250_FCR_TRIGGER_MASK	0xC0 /* Mask for the FIFO trigger range */
#define   UART8250_FCR_TRIGGER_1	0x00 /* Mask for trigger set at 1 */
#define   UART8250_FCR_TRIGGER_4	0x40 /* Mask for trigger set at 4 */
#define   UART8250_FCR_TRIGGER_8	0x80 /* Mask for trigger set at 8 */
#define   UART8250_FCR_TRIGGER_14	0xC0 /* Mask for trigger set at 14 */

#define   UART8250_FCR_RXSR		0x02 /* Receiver soft reset */
#define   UART8250_FCR_TXSR		0x04 /* Transmitter soft reset */

#define UART8250_LCR 0x03
#define   UART8250_LCR_WLS_MSK	0x03 /* character length select mask */
#define   UART8250_LCR_WLS_5	0x00 /* 5 bit character length */
#define   UART8250_LCR_WLS_6	0x01 /* 6 bit character length */
#define   UART8250_LCR_WLS_7	0x02 /* 7 bit character length */
#define   UART8250_LCR_WLS_8	0x03 /* 8 bit character length */
#define   UART8250_LCR_STB	0x04 /* Number of stop Bits, off = 1, on = 1.5 or 2) */
#define   UART8250_LCR_PEN	0x08 /* Parity enable */
#define   UART8250_LCR_EPS	0x10 /* Even Parity Select */
#define   UART8250_LCR_STKP	0x20 /* Stick Parity */
#define   UART8250_LCR_SBRK	0x40 /* Set Break */
#define   UART8250_LCR_BKSE	0x80 /* Bank select enable */
#define   UART8250_LCR_DLAB	0x80 /* Divisor latch access bit */

#define UART8250_MCR 0x04
#define   UART8250_MCR_DTR	0x01 /* DTR   */
#define   UART8250_MCR_RTS	0x02 /* RTS   */
#define   UART8250_MCR_OUT1	0x04 /* Out 1 */
#define   UART8250_MCR_OUT2	0x08 /* Out 2 */
#define   UART8250_MCR_LOOP	0x10 /* Enable loopback test mode */

#define UART8250_MCR_DMA_EN	0x04
#define UART8250_MCR_TX_DFR	0x08

#define UART8250_DLL 0x00
#define UART8250_DLM 0x01

/* Status */
#define UART8250_LSR 0x05
#define   UART8250_LSR_DR	0x01 /* Data ready */
#define   UART8250_LSR_OE	0x02 /* Overrun */
#define   UART8250_LSR_PE	0x04 /* Parity error */
#define   UART8250_LSR_FE	0x08 /* Framing error */
#define   UART8250_LSR_BI	0x10 /* Break */
#define   UART8250_LSR_THRE	0x20 /* Xmit holding register empty */
#define   UART8250_LSR_TEMT	0x40 /* Xmitter empty */
#define   UART8250_LSR_ERR	0x80 /* Error */

#define UART8250_MSR 0x06
#define   UART8250_MSR_DCD	0x80 /* Data Carrier Detect */
#define   UART8250_MSR_RI	0x40 /* Ring Indicator */
#define   UART8250_MSR_DSR	0x20 /* Data Set Ready */
#define   UART8250_MSR_CTS	0x10 /* Clear to Send */
#define   UART8250_MSR_DDCD	0x08 /* Delta DCD */
#define   UART8250_MSR_TERI	0x04 /* Trailing edge ring indicator */
#define   UART8250_MSR_DDSR	0x02 /* Delta DSR */
#define   UART8250_MSR_DCTS	0x01 /* Delta CTS */

#define UART8250_SCR 0x07
#define UART8250_SPR 0x07

#endif /* UART8250REG_H */
