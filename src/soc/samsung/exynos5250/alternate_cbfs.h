/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#ifndef CPU_SAMSUNG_EXYNOS5250_ALTERNATE_CBFS_H
#define CPU_SAMSUNG_EXYNOS5250_ALTERNATE_CBFS_H

/* These are pointers to function pointers. Double indirection! */
static void * * const irom_sdmmc_read_blocks_ptr = (void * *)0x02020030;
static void * * const irom_msh_read_from_fifo_emmc_ptr = (void * *)0x02020044;
static void * * const irom_msh_end_boot_op_emmc_ptr = (void * *)0x02020048;
static void * * const irom_spi_sf_read_ptr = (void * *)0x02020058;
static void * * const irom_load_image_from_usb_ptr = (void * *)0x02020070;

#define SECONDARY_BASE_BOOT_USB 0xfeed0002
static u32 * const iram_secondary_base = (u32 *)0x02020018;

/* Values pulled from U-Boot, I think the manual is wrong here (for SPI) */
#define OM_STAT_SDMMC 0x4
#define OM_STAT_EMMC  0x8
#define OM_STAT_SPI  0x14
#define OM_STAT_MASK 0x7f

#if defined(__PRE_RAM__) && !defined(__ROMSTAGE__)
	/* A small space in IRAM to hold the romstage-only image */
	static void * const alternate_cbfs_buffer =
			(void *)CONFIG_CBFS_CACHE_ADDRESS;
	static size_t const alternate_cbfs_size = CONFIG_CBFS_CACHE_SIZE;
#else
	/* Just put this anywhere in RAM that's far enough from anything else */
	/* TODO: Find a better way to "reserve" this region? */
	static void * const alternate_cbfs_buffer = (void *)0x77400000;
	static size_t const alternate_cbfs_size = 0xc00000;
#endif

#endif
