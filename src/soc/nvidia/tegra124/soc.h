/*
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
 * Copyright 2013 Google Inc.
 *
 * (C) Copyright 2010,2011
 * NVIDIA Corporation <www.nvidia.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _TEGRA124_SOC_H

/* AP base physical address of internal SRAM */
#define NV_PA_BASE_SRAM		0x40000000
#define NV_PA_BASE_SRAM_SIZE	0x20000

/* Address at which WB code runs, it must not overlap Bootrom's IRAM usage */
#define NV_WB_RUN_ADDRESS	0x40020000

#define NV_PA_ARM_PERIPHBASE	0x50040000
#define NV_PA_PG_UP_BASE	0x60000000
#define NV_PA_TMRUS_BASE	0x60005010
#define NV_PA_CLK_RST_BASE	0x60006000
#define NV_PA_FLOW_BASE		0x60007000
#define NV_PA_GPIO_BASE		0x6000D000
#define NV_PA_EVP_BASE		0x6000F000
#define NV_PA_APB_MISC_BASE	0x70000000
#define NV_PA_APB_MISC_GP_BASE	(NV_PA_APB_MISC_BASE + 0x0800)
#define NV_PA_APB_UARTA_BASE	(NV_PA_APB_MISC_BASE + 0x6000)
#define NV_PA_APB_UARTB_BASE	(NV_PA_APB_MISC_BASE + 0x6040)
#define NV_PA_APB_UARTC_BASE	(NV_PA_APB_MISC_BASE + 0x6200)
#define NV_PA_APB_UARTD_BASE	(NV_PA_APB_MISC_BASE + 0x6300)
#define NV_PA_APB_UARTE_BASE	(NV_PA_APB_MISC_BASE + 0x6400)
#define NV_PA_NAND_BASE		(NV_PA_APB_MISC_BASE + 0x8000)
#define NV_PA_SPI_BASE		(NV_PA_APB_MISC_BASE + 0xC380)
#define NV_PA_SLINK1_BASE	(NV_PA_APB_MISC_BASE + 0xD400)
#define NV_PA_SLINK2_BASE	(NV_PA_APB_MISC_BASE + 0xD600)
#define NV_PA_SLINK3_BASE	(NV_PA_APB_MISC_BASE + 0xD800)
#define NV_PA_SLINK4_BASE	(NV_PA_APB_MISC_BASE + 0xDA00)
#define NV_PA_SLINK5_BASE	(NV_PA_APB_MISC_BASE + 0xDC00)
#define NV_PA_SLINK6_BASE	(NV_PA_APB_MISC_BASE + 0xDE00)
#define TEGRA_DVC_BASE		(NV_PA_APB_MISC_BASE + 0xD000)
#define NV_PA_PMC_BASE		(NV_PA_APB_MISC_BASE + 0xE400)
#define NV_PA_EMC_BASE		(NV_PA_APB_MISC_BASE + 0xF400)
#define NV_PA_FUSE_BASE		(NV_PA_APB_MISC_BASE + 0xF800)
#define NV_PA_CSITE_BASE	0x70040000
#define TEGRA_USB_ADDR_MASK	0xFFFFC000

#define NV_PA_SDRC_CS0		NV_PA_SDRAM_BASE

#define NVBOOTINFOTABLE_BCTSIZE	0x38	/* BCT size in BIT in IRAM */
#define NVBOOTINFOTABLE_BCTPTR	0x3C	/* BCT pointer in BIT in IRAM */

#endif	/* _TEGRA124_SOC_H_ */
