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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef UART8250_H
#define UART8250_H

#if CONFIG_CONSOLE_SERIAL8250 || CONFIG_CONSOLE_SERIAL8250MEM

/* Data */
#define UART_RBR 0x00
#define UART_TBR 0x00

/* Control */
#define UART_IER 0x01
#define   UART_IER_MSI		0x08 /* Enable Modem status interrupt */
#define   UART_IER_RLSI		0x04 /* Enable receiver line status interrupt */
#define   UART_IER_THRI		0x02 /* Enable Transmitter holding register int. */
#define   UART_IER_RDI		0x01 /* Enable receiver data interrupt */

#define UART_IIR 0x02
#define   UART_IIR_NO_INT	0x01 /* No interrupts pending */
#define   UART_IIR_ID		0x06 /* Mask for the interrupt ID */

#define   UART_IIR_MSI		0x00 /* Modem status interrupt */
#define   UART_IIR_THRI		0x02 /* Transmitter holding register empty */
#define   UART_IIR_RDI		0x04 /* Receiver data interrupt */
#define   UART_IIR_RLSI		0x06 /* Receiver line status interrupt */

#define UART_FCR 0x02
#define   UART_FCR_FIFO_EN	0x01 /* Fifo enable */
#define   UART_FCR_CLEAR_RCVR	0x02 /* Clear the RCVR FIFO */
#define   UART_FCR_CLEAR_XMIT	0x04 /* Clear the XMIT FIFO */
#define   UART_FCR_DMA_SELECT	0x08 /* For DMA applications */
#define   UART_FCR_TRIGGER_MASK	0xC0 /* Mask for the FIFO trigger range */
#define   UART_FCR_TRIGGER_1	0x00 /* Mask for trigger set at 1 */
#define   UART_FCR_TRIGGER_4	0x40 /* Mask for trigger set at 4 */
#define   UART_FCR_TRIGGER_8	0x80 /* Mask for trigger set at 8 */
#define   UART_FCR_TRIGGER_14	0xC0 /* Mask for trigger set at 14 */

#define   UART_FCR_RXSR		0x02 /* Receiver soft reset */
#define   UART_FCR_TXSR		0x04 /* Transmitter soft reset */

#define UART_LCR 0x03
#define   UART_LCR_WLS_MSK	0x03 /* character length select mask */
#define   UART_LCR_WLS_5	0x00 /* 5 bit character length */
#define   UART_LCR_WLS_6	0x01 /* 6 bit character length */
#define   UART_LCR_WLS_7	0x02 /* 7 bit character length */
#define   UART_LCR_WLS_8	0x03 /* 8 bit character length */
#define   UART_LCR_STB		0x04 /* Number of stop Bits, off = 1, on = 1.5 or 2) */
#define   UART_LCR_PEN		0x08 /* Parity enable */
#define   UART_LCR_EPS		0x10 /* Even Parity Select */
#define   UART_LCR_STKP		0x20 /* Stick Parity */
#define   UART_LCR_SBRK		0x40 /* Set Break */
#define   UART_LCR_BKSE		0x80 /* Bank select enable */
#define   UART_LCR_DLAB		0x80 /* Divisor latch access bit */

#define UART_MCR 0x04
#define   UART_MCR_DTR		0x01 /* DTR   */
#define   UART_MCR_RTS		0x02 /* RTS   */
#define   UART_MCR_OUT1		0x04 /* Out 1 */
#define   UART_MCR_OUT2		0x08 /* Out 2 */
#define   UART_MCR_LOOP		0x10 /* Enable loopback test mode */

#define UART_MCR_DMA_EN		0x04
#define UART_MCR_TX_DFR		0x08

#define UART_DLL 0x00
#define UART_DLM 0x01

/* Status */
#define UART_LSR 0x05
#define   UART_LSR_DR		0x01 /* Data ready */
#define   UART_LSR_OE		0x02 /* Overrun */
#define   UART_LSR_PE		0x04 /* Parity error */
#define   UART_LSR_FE		0x08 /* Framing error */
#define   UART_LSR_BI		0x10 /* Break */
#define   UART_LSR_THRE		0x20 /* Xmit holding register empty */
#define   UART_LSR_TEMT		0x40 /* Xmitter empty */
#define   UART_LSR_ERR		0x80 /* Error */

#define UART_MSR 0x06
#define   UART_MSR_DCD		0x80 /* Data Carrier Detect */
#define   UART_MSR_RI		0x40 /* Ring Indicator */
#define   UART_MSR_DSR		0x20 /* Data Set Ready */
#define   UART_MSR_CTS		0x10 /* Clear to Send */
#define   UART_MSR_DDCD		0x08 /* Delta DCD */
#define   UART_MSR_TERI		0x04 /* Trailing edge ring indicator */
#define   UART_MSR_DDSR		0x02 /* Delta DSR */
#define   UART_MSR_DCTS		0x01 /* Delta CTS */

#define UART_SCR 0x07
#define UART_SPR 0x07

#if ((115200 % CONFIG_TTYS0_BAUD) != 0)
#error Bad ttyS0 baud rate
#endif

/* Line Control Settings */
#define UART_LCS	CONFIG_TTYS0_LCS

#if CONFIG_CONSOLE_SERIAL8250
unsigned char uart8250_rx_byte(unsigned base_port);
int uart8250_can_rx_byte(unsigned base_port);
void uart8250_tx_byte(unsigned base_port, unsigned char data);
void uart8250_tx_flush(unsigned base_port);
/* Yes it is silly to have three different uart init functions. But we used to
 * have three different sets of uart code, so it's an improvement.
 */
void uart8250_init(unsigned base_port, unsigned divisor);
void uart_init(void);
#endif
#if CONFIG_CONSOLE_SERIAL8250MEM
void uartmem_init(void);

/* and the same for memory mapped uarts */
unsigned char uart8250_mem_rx_byte(unsigned base_port);
int uart8250_mem_can_rx_byte(unsigned base_port);
void uart8250_mem_tx_byte(unsigned base_port, unsigned char data);
void uart8250_mem_tx_flush(unsigned base_port);
void uart8250_mem_init(unsigned base_port, unsigned divisor);
u32 uart_mem_init(void);

#if defined(__PRE_RAM__) && CONFIG_DRIVERS_OXFORD_OXPCIE
/* and special init for OXPCIe based cards */
extern int oxford_oxpcie_present;

void oxford_init(void);
#endif
#endif

#endif /* CONFIG_CONSOLE_SERIAL8250 || CONFIG_CONSOLE_SERIAL8250MEM */

#endif /* UART8250_H */
