/*
 * (C) Copyright 2009-2010 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 * Heungjun Kim <riverful.kim@samsung.com>
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
 *
 */

#ifndef _EXYNOS_COMMON_CPU_H
#define _EXYNOS_COMMON_CPU_H

#define S5PC1XX_ADDR_BASE	0xE0000000

/* S5PC100 */
#define S5PC100_PRO_ID		0xE0000000
#define S5PC100_CLOCK_BASE	0xE0100000
#define S5PC100_GPIO_BASE	0xE0300000
#define S5PC100_VIC0_BASE	0xE4000000
#define S5PC100_VIC1_BASE	0xE4100000
#define S5PC100_VIC2_BASE	0xE4200000
#define S5PC100_DMC_BASE	0xE6000000
#define S5PC100_SROMC_BASE	0xE7000000
#define S5PC100_ONENAND_BASE	0xE7100000
#define S5PC100_PWMTIMER_BASE	0xEA000000
#define S5PC100_WATCHDOG_BASE	0xEA200000
#define S5PC100_UART_BASE	0xEC000000
#define S5PC100_MMC_BASE	0xED800000

/* S5PC110 */
#define S5PC110_PRO_ID		0xE0000000
#define S5PC110_CLOCK_BASE	0xE0100000
#define S5PC110_GPIO_BASE	0xE0200000
#define S5PC110_PWMTIMER_BASE	0xE2500000
#define S5PC110_WATCHDOG_BASE	0xE2700000
#define S5PC110_UART_BASE	0xE2900000
#define S5PC110_SROMC_BASE	0xE8000000
#define S5PC110_MMC_BASE	0xEB000000
#define S5PC110_DMC0_BASE	0xF0000000
#define S5PC110_DMC1_BASE	0xF1400000
#define S5PC110_VIC0_BASE	0xF2000000
#define S5PC110_VIC1_BASE	0xF2100000
#define S5PC110_VIC2_BASE	0xF2200000
#define S5PC110_VIC3_BASE	0xF2300000
#define S5PC110_OTG_BASE	0xEC000000
#define S5PC110_PHY_BASE	0xEC100000
#define S5PC110_USB_PHY_CONTROL 0xE010E80C


#include <arch/io.h>

#define DEVICE_NOT_AVAILABLE		0

#define EXYNOS_PRO_ID			0x10000000

/* Address of address of function that copys data from SD or MMC */
#define EXYNOS_COPY_MMC_FNPTR_ADDR	0x02020030

/* Address of address of function that copys data from SPI */
#define EXYNOS_COPY_SPI_FNPTR_ADDR	0x02020058

/* Address of address of function that copys data through USB */
#define EXYNOS_COPY_USB_FNPTR_ADDR	0x02020070

/* Boot mode values */
#define EXYNOS_USB_SECONDARY_BOOT	0xfeed0002

#define EXYNOS_IRAM_SECONDARY_BASE	0x02020018

#define EXYNOS_I2C_SPACING		0x10000

enum boot_mode {
	/*
	 * Assign the OM pin values for respective boot modes.
	 * Exynos4 does not support spi boot and the mmc boot OM
	 * pin values are the same across Exynos4 and Exynos5.
	 */
	BOOT_MODE_MMC = 4,
	BOOT_MODE_SERIAL = 20,
	/* Boot based on Operating Mode pin settings */
	BOOT_MODE_OM = 32,
	BOOT_MODE_USB,		/* Boot using USB download */
};

/**
 * Get the boot device containing BL1, BL2 (SPL) and U-boot
 *
 * @return boot device
 */
enum boot_mode exynos_get_boot_device(void);

/**
 * Check if a wakeup is permitted.
 *
 * On some boards we need to look at a special GPIO to ensure that the wakeup
 * from sleep was valid.  If the wakeup is not valid we need to go through a
 * full reset.
 *
 * The default implementation of this function allows all wakeups.
 *
 * @return 1 if wakeup is permitted; 0 otherwise
 */
int board_wakeup_permitted(void);

/**
 * Init subsystems according to the reset status
 *
 * @return 0 for a normal boot, non-zero for a resume
 */
int lowlevel_init_subsystems(void);

int arch_cpu_init(void);


#endif	/* _EXYNOS_COMMON_CPU_H */
