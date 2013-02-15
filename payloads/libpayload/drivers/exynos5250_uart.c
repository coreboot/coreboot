/*
 * (C) Copyright 2013 Google, Inc.
 * (C) Copyright 2009 SAMSUNG Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 * Heungjun Kim <riverful.kim@samsung.com>
 *
 * based on drivers/serial/s3c64xx.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
 * this is a seriously cut-down UART implementation, the assumption
 * being that you should let coreboot set it up. It's quite
 * messy to keep doing UART setup everywhere on these ARM SOCs.
 */

#include <libpayload-config.h>
#include <libpayload.h>

/* baudrate rest value */
union br_rest {
	unsigned short	slot;		/* udivslot */
	unsigned char	value;		/* ufracval */
};

struct s5p_uart {
	unsigned int	ulcon;
	unsigned int	ucon;
	unsigned int	ufcon;
	unsigned int	umcon;
	unsigned int	utrstat;
	unsigned int	uerstat;
	unsigned int	ufstat;
	unsigned int	umstat;
	unsigned char	utxh;
	unsigned char	res1[3];
	unsigned char	urxh;
	unsigned char	res2[3];
	unsigned int	ubrdiv;
	union br_rest	rest;
	unsigned char	res3[0xffd0];
};

#define MEMBASE (struct s5p_uart *)(phys_to_virt(lib_sysinfo.serial->baseaddr))
#define RX_FIFO_COUNT_MASK	0xff
#define RX_FIFO_FULL_MASK	(1 << 8)
#define TX_FIFO_FULL_MASK	(1 << 24)

/*
 * Initialise the serial port.
 * This hardware is really complex, and we're not going to pretend
 * it's a good idea to mess with it here. So, take what coreboot did
 * and leave it at that.
 */
void serial_init(void)
{
}

static int exynos5_uart_err_check(int op)
{
	struct s5p_uart *uart = MEMBASE;
	unsigned int mask;

	/*
	 * UERSTAT
	 * Break Detect	[3]
	 * Frame Err	[2] : receive operation
	 * Parity Err	[1] : receive operation
	 * Overrun Err	[0] : receive operation
	 */
	if (op)
		mask = 0x8;
	else
		mask = 0xf;

	return readl(&uart->uerstat) & mask;
}

int serial_havechar(void)
{
	struct s5p_uart *uart = MEMBASE;
	return (readl(&uart->ufstat) & (RX_FIFO_COUNT_MASK |
					RX_FIFO_FULL_MASK));
}

int serial_getchar(void)
{
	struct s5p_uart *uart = MEMBASE;

	/* wait for character to arrive */
	while (!(readl(&uart->ufstat) & (RX_FIFO_COUNT_MASK |
					 RX_FIFO_FULL_MASK))) {
		if (exynos5_uart_err_check(0))
			return 0;
	}

	return readb(&uart->urxh) & 0xff;
}

/*
 * Output a single byte to the serial port.
 */
void serial_putchar(unsigned int c)
{
	struct s5p_uart *uart = MEMBASE;

	/* wait for room in the tx FIFO */
	while ((readl(uart->ufstat) & TX_FIFO_FULL_MASK)) {
		if (exynos5_uart_err_check(1))
			return;
	}

	writeb(c, &uart->utxh);
}
