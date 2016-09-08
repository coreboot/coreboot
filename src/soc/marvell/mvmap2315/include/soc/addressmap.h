/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#ifndef __SOC_MARVELL_MVMAP2315_ADDRESS_MAP_H__
#define __SOC_MARVELL_MVMAP2315_ADDRESS_MAP_H__

#include <stdint.h>

#define MAX_DRAM_ADDRESS		0x73000000

#define MVMAP2315_CBFS_BASE		0x00400000

#define MVMAP2315_BOOTBLOCK_CB1		0xE0009510
#define MVMAP2315_BOOTBLOCK_CB2		0xE0009514

#define MVMAP2315_PINMUX_BASE		0xE0140000
#define MVMAP2315_TIMER0_BASE		0xE1020000
#define MVMAP2315_GPIOF_BASE		0xE0142000
#define MVMAP2315_GPIOG_BASE		0xE0142100
#define MVMAP2315_GPIOH_BASE		0xE0142200

#define	MVMAP2315_BCM_GICD_BASE		0xE0111000
#define MVMAP2315_BCM_GICC_BASE		0xE0112000

#define MVMAP2315_MAIN_PLL_BASE		0xE0125000
#define MVMAP2315_APMU_CLK_BASE		0xE0125400
#define MVMAP2315_GENTIMER_BASE		0xE0137000
#define MVMAP2315_MPMU_CLK_BASE		0xEF000800
#define MVMAP2315_MCU_SECCONFIG_BASE	0xED600000
#define MVMAP2315_APMU_PWRCTL_BASE	0xE012C000
#define MVMAP2315_LCM_REGS_BASE		0xE0130000

#define MVMAP2315_RAM_BASE		0x00000000
#define MVMAP2315_DEVICE_BASE		0x80000000
#define MVMAP2315_FLASH_BASE		0xFE000000
#define MVMAP2315_LCM_BASE		0xE0000000

#endif /*  __SOC_MARVELL_MVMAP2315_ADDRESS_MAP_H__ */
