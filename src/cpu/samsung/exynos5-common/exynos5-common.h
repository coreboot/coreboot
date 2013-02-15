/*
 * Copyright (C) 2012 Samsung Electronics
 *
 * Common configuration settings for EXYNOS5 based boards.
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

/* TODO(dhendrix): some #defines are commented out here and moved to Kconfig */

#ifndef __EXYNOS5_CONFIG_H
#define __EXYNOS5_CONFIG_H

/* High Level Configuration Options */
#define CONFIG_SAMSUNG			/* in a SAMSUNG core */
#define CONFIG_S5P			/* S5P Family */
#define CONFIG_EXYNOS5			/* which is in a Exynos5 Family */
#define BUILD_PART_FS_STUFF	1	/* Disk Partition Support */

#define CONFIG_ARCH_CPU_INIT		/* Used to check cpu type */

#include <cpu/samsung/exynos5250/cpu.h>		/* get chip and board defs */

/* Align LCD to 1MB boundary */
#define CONFIG_LCD_ALIGNMENT	MMU_SECTION_SIZE

#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO_LATE
#define CONFIG_SYS_CONSOLE_INFO_QUIET
#define CONFIG_BOARD_LATE_INIT

#define CONFIG_CMD_SHA256
//#define CONFIG_EXYNOS_ACE_SHA

//#define CONFIG_SYS_SDRAM_BASE		0x40000000
//#define CONFIG_SYS_TEXT_BASE		0x43e00000

#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG
#define CONFIG_CMDLINE_EDITING

/* Power Down Modes */
#define S5P_CHECK_SLEEP			0x00000BAD
#define S5P_CHECK_DIDLE			0xBAD00000
#define S5P_CHECK_LPA			0xABAD0000

/* Offset for inform registers */
#define INFORM0_OFFSET			0x800
#define INFORM1_OFFSET			0x804

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (4 << 20))

/* select serial console configuration */
#define CONFIG_SERIAL_MULTI
//#define CONFIG_BAUDRATE			115200
#define EXYNOS5_DEFAULT_UART_OFFSET	0x010000

#define CONFIG_BOARD_EARLY_INIT_F

/* PWM */
#define CONFIG_PWM

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

/* SPL */
#define CONFIG_SPL
#define CONFIG_SPL_GPIO_SUPPORT
#define CONFIG_SPL_POWER_SUPPORT
#define CONFIG_SPL_I2C_SUPPORT
#define CONFIG_SPL_SERIAL_SUPPORT
#define CONFIG_SPL_LIBCOMMON_SUPPORT

/* Number of GPIOS to use for board revision detection */
#define CONFIG_BOARD_REV_GPIO_COUNT	2

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER		/* use "hush" command parser	*/
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_CBSIZE		256	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE		384	/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS		16	/* max number of command args */
/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
/* memtest works on */
#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_SDRAM_BASE + 0x5E00000)
#define CONFIG_SYS_LOAD_ADDR		(CONFIG_SYS_SDRAM_BASE + 0x3E00000)

#define CONFIG_SYS_HZ			1000

/* valid baudrates */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/* Stack sizes */
#define CONFIG_STACKSIZE		(256 << 10)	/* 256KB */

#define CONFIG_SYS_MONITOR_BASE	0x00000000

#ifdef CONFIG_SPI_FLASH
/* Enable SPI H/W Controller Driver support */
#define CONFIG_EXYNOS_SPI

/* FIXME(dhendrix): We should be concerned with SPI flash parts here... */
#if 0
#define CONFIG_CMD_SF
#define CONFIG_CMD_SPI
#define CONFIG_SPI_FLASH_WINBOND
/* Enable Gigadevice SPI flash support for Snow board */
#define CONFIG_SPI_FLASH_GIGADEVICE
#define CONFIG_SF_DEFAULT_MODE SPI_MODE_0
/* Set speed for SPI flash */
#define CONFIG_SF_DEFAULT_SPEED	50000000
#endif
#endif

/* FLASH and environment organization */
#define CONFIG_SYS_NO_FLASH
#undef CONFIG_CMD_IMLS

#define CONFIG_SECURE_BL1_ONLY

/* Secure FW size configuration */
#ifdef CONFIG_SECURE_BL1_ONLY
#define CONFIG_SEC_FW_SIZE		(8 << 10)	/* 8KB */
#else
#define CONFIG_SEC_FW_SIZE		0
#endif

/* Configuration of BL1, BL2, ENV Blocks on mmc */
#define CONFIG_RES_BLOCK_SIZE	(512)
#define CONFIG_BL1_SIZE		(16 << 10) /*16 K reserved for BL1*/
#define CONFIG_BL2_SIZE		(512UL << 10UL)	/* 512 KB */
#define CONFIG_ENV_SIZE		(16 << 10)	/* 16 KB */

#define CONFIG_BL1_OFFSET	(CONFIG_RES_BLOCK_SIZE + CONFIG_SEC_FW_SIZE)
#define CONFIG_BL2_OFFSET	(CONFIG_BL1_OFFSET + CONFIG_BL1_SIZE)

#define SPI_FLASH_UBOOT_POS	(CONFIG_SEC_FW_SIZE + CONFIG_BL1_SIZE)

#ifdef CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_SPI_MODE	SPI_MODE_0
#define CONFIG_ENV_OFFSET	(CONFIG_SEC_FW_SIZE + CONFIG_BL1_SIZE + \
				 CONFIG_BL2_SIZE)
#define CONFIG_ENV_SECT_SIZE	CONFIG_ENV_SIZE
#define CONFIG_ENV_SPI_BUS	1
#else /* CONFIG_ENV_IS_IN_MMC */
#define CONFIG_ENV_OFFSET	(CONFIG_BL2_OFFSET + CONFIG_BL2_SIZE)
#endif

/* U-boot copy size from boot Media to DRAM.*/
#define BL2_START_OFFSET	(CONFIG_BL2_OFFSET/512)

/* Set the emmc bus width to 8 */
#define CONFIG_MSHCI_BUS_WIDTH	8
#define CONFIG_MSHCI_PERIPH_ID	PERIPH_ID_SDMMC0

#if BUILD_PART_FS_STUFF
#define CONFIG_DOS_PARTITION
#define CONFIG_EFI_PARTITION
#endif

/* Enable devicetree support */
#define CONFIG_OF_LIBFDT

#define CONFIG_SYS_THUMB_BUILD

/* We spend about 100us getting from reset to SPL */
#define CONFIG_SPL_TIME_US	100000

/* Stringify a token */
#ifndef STRINGIFY
#define _STRINGIFY(x)	#x
#define STRINGIFY(x)	_STRINGIFY(x)
#endif

#endif	/* __EXYNOS5_CONFIG_H */
