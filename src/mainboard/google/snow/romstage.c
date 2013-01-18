/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
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

#include <types.h>
#include <system.h>
#include <cache.h>

#include <arch/io.h>

/* FIXME: make i2c.h use standard types */
#define uchar unsigned char
#define uint  unsigned int
#include <device/i2c.h>

#include <cpu/samsung/s5p-common/s3c24x0_i2c.h>
#include "cpu/samsung/exynos5250/dmc.h"
#include <cpu/samsung/exynos5250/power.h>
#include <cpu/samsung/exynos5250/clock_init.h>
#include <cpu/samsung/exynos5-common/uart.h>
#include <console/console.h>

static void mmu_setup(void)
{
	dram_bank_mmu_setup(CONFIG_SYS_SDRAM_BASE, CONFIG_DRAM_SIZE_MB * 1024);
}

#define RX_FIFO_COUNT_MASK	0xff
#define RX_FIFO_FULL_MASK	(1 << 8)
#define TX_FIFO_FULL_MASK	(1 << 24)
static uint32_t uart3_base = CONFIG_CONSOLE_SERIAL_UART_ADDRESS;

static int exynos5_uart_err_check(int op)
{
	//struct s5p_uart *const uart = s5p_get_base_uart(dev_index);
	struct s5p_uart *uart = (struct s5p_uart *)uart3_base;
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

/*
 * Output a single byte to the serial port.
 */
static void exynos5_uart_tx_byte(unsigned char data)
{
//	struct s5p_uart *const uart = s5p_get_base_uart(dev_index);
	struct s5p_uart *uart = (struct s5p_uart *)uart3_base;

	if (data == '\n')
		exynos5_uart_tx_byte('\r');

	/* wait for room in the tx FIFO */
	while ((readl(uart->ufstat) & TX_FIFO_FULL_MASK)) {
		if (exynos5_uart_err_check(1))
			return;
	}

	writeb(data, &uart->utxh);
}

void main(void)
{
	int i;
	char hello[] = "hello from romstage";
//	volatile unsigned long *pshold = (unsigned long *)0x1004330c;
//	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
//	power_init();
//	clock_init();
//	exynos_pinmux_config(PERIPH_ID_UART3, PINMUX_FLAG_NONE);
//	console_init();
	printk(BIOS_INFO, "hello from romstage\n");
	for (i = 0; i < 19; i++)
		exynos5_uart_tx_byte(hello[i]);
	exynos5_uart_tx_byte('\r');

//	*pshold &= ~0x100;	/* shut down */
	mmu_setup();
}
