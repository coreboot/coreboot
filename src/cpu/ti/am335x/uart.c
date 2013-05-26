/*
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 */

#include <types.h>
#include <uart.h>
#include <arch/io.h>

#include <console/console.h>	/* for __console definition */

#include <cpu/ti/am335x/uart.h>

#define RX_FIFO_COUNT_MASK	0xff
#define RX_FIFO_FULL_MASK	(1 << 8)
#define TX_FIFO_FULL_MASK	(1 << 24)

/*
 * Initialise the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 */
static void am335x_uart_init_dev(void)
{
	/* FIXME: implement this
	 * ref: section 19.4.1.1.1 for reset
	 * ref: section 19.4.1.1.2 for FIFO, skip DMA
	 * ref: section 19.4.1.1.3 for baud rate, skip
	 *      interrupts and protocol stuff.
	 * */
#if 0
	struct s5p_uart *uart = (struct s5p_uart *)base_port;

	// TODO initialize with correct peripheral id by base_port.
	exynos_pinmux_config(PERIPH_ID_UART3, PINMUX_FLAG_NONE);

	/* enable FIFOs */
	writel(0x1, &uart->ufcon);
	writel(0, &uart->umcon);
	/* 8N1 */
	writel(0x3, &uart->ulcon);
	/* No interrupts, no DMA, pure polling */
	writel(0x245, &uart->ucon);

	serial_setbrg_dev();
#endif
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
static unsigned char am335x_uart_rx_byte(void)
{
	/* FIXME: stub */
#if 0
	struct s5p_uart *uart = (struct s5p_uart *)base_port;

	/* wait for character to arrive */
	while (!(readl(&uart->ufstat) & (RX_FIFO_COUNT_MASK |
					 RX_FIFO_FULL_MASK))) {
		if (exynos5_uart_err_check(0))
			return 0;
	}

	return readb(&uart->urxh) & 0xff;
#endif
	return 0xaa;
}

/*
 * Output a single byte to the serial port.
 */
static void am335x_uart_tx_byte(unsigned char data)
{
	/* FIXME: stub */
#if 0
	struct s5p_uart *uart = (struct s5p_uart *)base_port;

	/* wait for room in the tx FIFO */
	while ((readl(&uart->ufstat) & TX_FIFO_FULL_MASK)) {
		if (exynos5_uart_err_check(1))
			return;
	}

	writeb(data, &uart->utxh);
#endif
}

uint32_t uartmem_getbaseaddr(void)
{
	return CONFIG_CONSOLE_SERIAL_UART_ADDRESS;
}

#if !defined(__PRE_RAM__)
static const struct console_driver exynos5_uart_console __console = {
	.init     = am335x_uart_init_dev,
	.tx_byte  = am335x_uart_tx_byte,
	.rx_byte  = am335x_uart_rx_byte,
};
#else
void uart_init(void)
{
	am335x_uart_init_dev();
}

unsigned char uart_rx_byte(void)
{
	return am335x_uart_rx_byte();
}

void uart_tx_byte(unsigned char data)
{
	am335x_uart_tx_byte(data);
}

void uart_tx_flush(void) {
}
#endif
