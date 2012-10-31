/*
 * (C) Copyright 2010 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
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
 * Get the U-boot size for SPL copy functions
 *
 * @return size of U-Boot code/data that needs to be loaded by the SPL stage
 */
unsigned int exynos_get_uboot_size(void);

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

int s5p_get_cpu_rev(void);
void s5p_set_cpu_id(void);
int s5p_get_cpu_id(void);

#define cpu_is_exynos4()	(s5p_get_cpu_id() == 0xc210)
#define cpu_is_exynos5()	(s5p_get_cpu_id() == 0xc520)

/**
 * Init subsystems according to the reset status
 *
 * @return 0 for a normal boot, non-zero for a resume
 */
int lowlevel_init_subsystems(void);

#endif	/* _EXYNOS_COMMON_CPU_H */
