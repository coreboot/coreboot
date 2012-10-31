/*
 * Copyright (c) 2012 The Chromium OS Authors.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __ASM_ARCH_EXYNOS_SPL_H__
#define __ASM_ARCH_EXYNOS_SPL_H__

#include <asm/arch-exynos/cpu.h>
#include <asm/arch-exynos5/dmc.h>

/* Parameters of early board initialization in SPL */
struct spl_machine_param {
	/* Add fields as and when required */
	u32		signature;
	u32		version;	/* Version number */
	u32		size;		/* Size of block */
	/**
	 * Parameters we expect, in order, terminated with \0. Each parameter
	 * is a single character representing one 32-bit word in this
	 * structure.
	 *
	 * Valid characters in this string are:
	 *
	 * Code		Name
	 * v		mem_iv_size
	 * m		mem_type
	 * u		uboot_size
	 * b		boot_source
	 * f		frequency_mhz (memory frequency in MHz)
	 * a		ARM clock frequency in MHz
	 * s		serial base address
	 * i		i2c base address for early access (meant for PMIC)
	 * r		board rev GPIO numbers used to read board revision
	 *			(lower halfword=bit 0, upper=bit 1)
	 * M		Memory Manufacturer name
	 * w		Bad Wake GPIO number
	 * \0		termination
	 */
	char		params[12];	/* Length must be word-aligned */
	u32		mem_iv_size;	/* Memory channel interleaving size */
	enum ddr_mode	mem_type;	/* Type of on-board memory */
	/*
	 * U-boot size - The iROM mmc copy function used by the SPL takes a
	 * block count paramter to describe the u-boot size unlike the spi
	 * boot copy function which just uses the u-boot size directly. Align
	 * the u-boot size to block size (512 bytes) when populating the SPL
	 * table only for mmc boot.
	 */
	u32		uboot_size;
	enum boot_mode	boot_source;	/* Boot device */
	unsigned	frequency_mhz;	/* Frequency of memory in MHz */
	unsigned	arm_freq_mhz;	/* ARM Frequency in MHz */
	u32		serial_base;	/* Serial base address */
	u32		i2c_base;	/* i2c base address */
	u32		board_rev_gpios;	/* Board revision GPIOs */
	enum mem_manuf	mem_manuf;	/* Memory Manufacturer */
	u32		bad_wake_gpio;	/* If high at wake time disallow wake */
} __attribute__((__packed__));

/**
 * Validate signature and return a pointer to the parameter table.  If the
 * signature is invalid, call panic() and never return.
 *
 * @return pointer to the parameter table if signature matched or never return.
 */
struct spl_machine_param *spl_get_machine_params(void);

/*
 * Initialize the timer and serial driver in SPL u-boot.
 * Besides the serial driver, it also setup the minimal set of its dependency,
 * like gd struct, pinmux, and serial.
 */
void spl_early_init(void);

#endif /* __ASM_ARCH_EXYNOS_SPL_H__ */
