/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2013-2015, NVIDIA CORPORATION.  All rights reserved.
 * Copyright 2014 Google Inc.
 *
 * (C) Copyright 2010,2011
 * NVIDIA Corporation <www.nvidia.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SOC_NVIDIA_TEGRA210_INCLUDE_SOC_ADDRESS_MAP_H__
#define __SOC_NVIDIA_TEGRA210_INCLUDE_SOC_ADDRESS_MAP_H__

#include <stddef.h>
#include <stdint.h>

enum {
	TEGRA_SRAM_BASE = 0x40000000,
	TEGRA_SRAM_SIZE = 0x40000
};

enum {
	TEGRA_ARM_PCIE_A1_BASE =	0x01000000,
	TEGRA_ARM_LOWEST_PERIPH =	TEGRA_ARM_PCIE_A1_BASE,
	TEGRA_ARM_PERIPHBASE =		0x50040000,
	TEGRA_GICD_BASE =		0x50041000,
	TEGRA_GICC_BASE =		0x50042000,
	TEGRA_MSELECT_CONFIG =		0x50060000,
	TEGRA_ARM_DISPLAYA =            0x54200000,
	TEGRA_ARM_DISPLAYB =            0x54240000,
	TEGRA_DSIA_BASE =               0x54300000,
	TEGRA_DSIB_BASE =               0x54400000,
	TEGRA_ARM_SOR =                 0x54540000,
	TEGRA_ARM_DPAUX =               0x545c0000,
	TEGRA_PG_UP_BASE =		0x60000000,
	TEGRA_TMRUS_BASE =		0x60005010,
	TEGRA_CLK_RST_BASE =		0x60006000,
	TEGRA_FLOW_BASE =		0x60007000,
	TEGRA_SB_BASE =			0x6000C200,
	TEGRA_GPIO_BASE =		0x6000D000,
	TEGRA_EVP_BASE =		0x6000F000,
	TEGRA_APB_DMA_BASE =		0x60020000,
	TEGRA_APB_MISC_BASE =		0x70000000,
	TEGRA_APB_MISC_GP_BASE =	TEGRA_APB_MISC_BASE + 0x0800,
	TEGRA_APB_PINGROUP_BASE =	TEGRA_APB_MISC_BASE + 0x0868,
	TEGRA_APB_PINMUX_BASE =		TEGRA_APB_MISC_BASE + 0x3000,
	TEGRA_APB_UARTA_BASE =		TEGRA_APB_MISC_BASE + 0x6000,
	TEGRA_APB_UARTB_BASE =		TEGRA_APB_MISC_BASE + 0x6040,
	TEGRA_APB_UARTC_BASE =		TEGRA_APB_MISC_BASE + 0x6200,
	TEGRA_APB_UARTD_BASE =		TEGRA_APB_MISC_BASE + 0x6300,
	TEGRA_APB_UARTE_BASE =		TEGRA_APB_MISC_BASE + 0x6400,
	TEGRA_NAND_BASE =		TEGRA_APB_MISC_BASE + 0x8000,
	TEGRA_PWM_BASE =		TEGRA_APB_MISC_BASE + 0xA000,
	TEGRA_I2C1_BASE =		TEGRA_APB_MISC_BASE + 0xC000,
	TEGRA_SPI_BASE =		TEGRA_APB_MISC_BASE + 0xC380,
	TEGRA_I2C2_BASE =		TEGRA_APB_MISC_BASE + 0xC400,
	TEGRA_I2C3_BASE =		TEGRA_APB_MISC_BASE + 0xC500,
	TEGRA_I2C4_BASE =		TEGRA_APB_MISC_BASE + 0xC700,
	TEGRA_I2C5_BASE =		TEGRA_APB_MISC_BASE + 0xD000,
	TEGRA_I2C6_BASE =		TEGRA_APB_MISC_BASE + 0xD100,
	TEGRA_SPI1_BASE =		TEGRA_APB_MISC_BASE + 0xD400,
	TEGRA_SPI2_BASE =		TEGRA_APB_MISC_BASE + 0xD600,
	TEGRA_SPI3_BASE =		TEGRA_APB_MISC_BASE + 0xD800,
	TEGRA_SPI4_BASE =		TEGRA_APB_MISC_BASE + 0xDA00,
	TEGRA_SPI5_BASE =		TEGRA_APB_MISC_BASE + 0xDC00,
	TEGRA_SPI6_BASE =		TEGRA_APB_MISC_BASE + 0xDE00,
	TEGRA_SBC1_BASE =		TEGRA_SPI1_BASE,
	TEGRA_SBC2_BASE =		TEGRA_SPI2_BASE,
	TEGRA_SBC3_BASE =		TEGRA_SPI3_BASE,
	TEGRA_SBC4_BASE =		TEGRA_SPI4_BASE,
	TEGRA_SBC5_BASE =		TEGRA_SPI5_BASE,
	TEGRA_SBC6_BASE =		TEGRA_SPI6_BASE,
	TEGRA_PMC_BASE =		TEGRA_APB_MISC_BASE + 0xE400,
	TEGRA_FUSE_BASE =		TEGRA_APB_MISC_BASE + 0xF800,
	TEGRA_MC_BASE =			0x70019000,
	TEGRA_EMC_BASE =		0x7001B000,
	TEGRA_CLUSTER_CLOCK_BASE =	0x70040000,
	TEGRA_QSPI_BASE =		0x70410000,
	TEGRA_CSITE_BASE =		0x70800000,
	TEGRA_SDMMC_BASE =		0x700b0000,
	TEGRA_SDMMC1_BASE =		TEGRA_SDMMC_BASE + 0x0000,
	TEGRA_SDMMC2_BASE =		TEGRA_SDMMC_BASE + 0x0200,
	TEGRA_SDMMC3_BASE =		TEGRA_SDMMC_BASE + 0x0400,
	TEGRA_SDMMC4_BASE =		TEGRA_SDMMC_BASE + 0x0600,
	TEGRA_MIPI_CAL_BASE =		0x700E3000,
	TEGRA_SYSCTR0_BASE =		0x700F0000,
	TEGRA_I2S1_BASE =		0x70301100,
	TEGRA_USBD_BASE =		0x7D000000,
	TEGRA_USB2_BASE =		0x7D004000,
	TEGRA_USB3_BASE =		0x7D008000,
};

enum {
	TEGRA_I2C_BASE_COUNT = 6,
};

#define GPU_CARVEOUT_SIZE_MB		1
#define NVDEC_CARVEOUT_SIZE_MB		1
#define TSEC_CARVEOUT_SIZE_MB		2
#define VPR_CARVEOUT_SIZE_MB		128

/* Return total size of DRAM memory configured on the platform. */
int sdram_size_mb(void);

/* Find memory below and above 4GiB boundary repsectively. All units 1MiB. */
void memory_in_range_below_4gb(uintptr_t *base_mib, uintptr_t *end_mib);
void memory_in_range_above_4gb(uintptr_t *base_mib, uintptr_t *end_mib);

enum {
	CARVEOUT_TZ,
	CARVEOUT_SEC,
	CARVEOUT_MTS,
	CARVEOUT_VPR,
	CARVEOUT_GPU,
	CARVEOUT_NVDEC,
	CARVEOUT_TSEC,
	CARVEOUT_NUM,
};

/* Provided the careout id, obtain the base and size in 1MiB units. */
void carveout_range(int id, uintptr_t *base_mib, size_t *size_mib);
void print_carveouts(void);

/*
 * There are complications accessing the Trust Zone carveout region. The
 * AVP cannot access these registers and the CPU can't access this register
 * as a non-secure access. When the page tables live in non-secure memory
 * these registers cannot be accessed either. Thus, this function handles
 * both the AVP case and non-secured access case by keeping global state.
 */
void trustzone_region_init(void);
void gpu_region_init(void);
void nvdec_region_init(void);
void tsec_region_init(void);
void vpr_region_init(void);

#endif /* __SOC_NVIDIA_TEGRA210_INCLUDE_SOC_ADDRESS_MAP_H__ */
