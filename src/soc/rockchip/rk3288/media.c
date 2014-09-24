/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
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

#include <cbfs.h>
#include <string.h>
#include <console/console.h>
#include "soc/rockchip/rk3288/spi.h"

int init_default_cbfs_media(struct cbfs_media *media)
{
#if defined(__BOOT_BLOCK__) || defined(__VER_STAGE__)
	return initialize_rockchip_spi_cbfs_media(media,
		(void *)CONFIG_CBFS_SRAM_CACHE_ADDRESS,
		CONFIG_CBFS_SRAM_CACHE_SIZE);
#else
	return initialize_rockchip_spi_cbfs_media(media,
		(void *)CONFIG_CBFS_DRAM_CACHE_ADDRESS,
		CONFIG_CBFS_DRAM_CACHE_SIZE);
#endif
}
