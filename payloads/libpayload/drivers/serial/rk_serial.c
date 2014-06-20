/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Rockchip Electronics
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
#include <libpayload-config.h>
#include <libpayload.h>
struct rk_uart {
	union {
		u32 uart_thr;	/* Transmit holding register. */
		u32 uart_rbr;	/* Receive buffer register. */
		u32 uart_dll;	/* Divisor latch lsb. */
	};
	union {
		u32 uart_ier;	/* Interrupt enable register. */
		u32 uart_dlh;	/* Divisor latch msb. */
	};
	union {
		uint32_t uart_iir;	/* Interrupt identification register. */
		uint32_t uart_fcr;	/* FIFO control register. */
	};
	u32 uart_lcr;
	u32 uart_mcr;
	u32 uart_lsr;
	u32 uart_msr;
	u32 uart_scr;
	u32 reserved1[(0x30 - 0x20) / 4];
	u32 uart_srbr[(0x70 - 0x30) / 4];
	u32 uart_far;
	u32 uart_tfr;
	u32 uart_rfw;
	u32 uart_usr;
	u32 uart_tfl;
	u32 uart_rfl;
	u32 uart_srr;
	u32 uart_srts;
	u32 uart_sbcr;
	u32 uart_sdmam;
	u32 uart_sfe;
	u32 uart_srt;
	u32 uart_stet;
	u32 uart_htx;
	u32 uart_dmasa;
	u32 reserver2[(0xf4 - 0xac) / 4];
	u32 uart_cpr;
	u32 uart_ucv;
	u32 uart_ctr;
};
enum {
	UART_LSR_DR = 0x1 << 0,	/* Data ready. */
	UART_LSR_OE = 0x1 << 1,	/* Overrun. */
	UART_LSR_PE = 0x1 << 2,	/* Parity error. */
	UART_LSR_FE = 0x1 << 3,	/* Framing error. */
	UART_LSR_BI = 0x1 << 4,	/* Break. */
	UART_LSR_THRE = 0x1 << 5,	/* Xmit holding register empty. */
	UART_LSR_TEMT = 0x1 << 6,	/* Xmitter empty. */
	UART_LSR_ERR = 0x1 << 7	/* Error. */
};

static struct rk_uart *uart_regs;
void serial_putchar(unsigned int c)
{
	while (!(readl(&uart_regs->uart_lsr) & UART_LSR_THRE));
	writel((c & 0xff), &uart_regs->uart_thr);
	if (c == '\n')
		serial_putchar('\r');
}

int serial_havechar(void)
{
	uint8_t lsr = readl(&uart_regs->uart_lsr);
	return (lsr & UART_LSR_DR) == UART_LSR_DR;
}

int serial_getchar(void)
{
	while (!serial_havechar());
	return readl(&uart_regs->uart_rbr)&0xff;
}

static struct console_input_driver consin = {
	.havekey = &serial_havechar,
	.getchar = &serial_getchar
};

static struct console_output_driver consout = {.putchar = &serial_putchar
};

void serial_init(void)
{
	if (!lib_sysinfo.serial || !lib_sysinfo.serial->baseaddr)
		return;

	uart_regs = (struct rk_uart *)lib_sysinfo.serial->baseaddr;
}

void serial_console_init(void)
{
	serial_init();
	console_add_input_driver(&consin);
	console_add_output_driver(&consout);
}
