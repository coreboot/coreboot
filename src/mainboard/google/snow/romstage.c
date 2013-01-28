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

#if 0
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
#endif
#include <console/console.h>

void main(void);
void main(void)
{
//	volatile unsigned long *pshold = (unsigned long *)0x1004330c;
//	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
//	power_init();
//	clock_init();
//	exynos_pinmux_config(PERIPH_ID_UART3, PINMUX_FLAG_NONE);
	console_init();
	printk(BIOS_INFO, "hello from romstage\n");

//	*pshold &= ~0x100;	/* shut down */
	mmu_setup(CONFIG_SYS_SDRAM_BASE, CONFIG_DRAM_SIZE_MB * 1024);
}
