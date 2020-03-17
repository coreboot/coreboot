/*
 * This file is part of the coreboot project.
 *
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

#ifndef __SOC_NVIDIA_TEGRA124_PINGROUP_H__
#define __SOC_NVIDIA_TEGRA124_PINGROUP_H__

#include <soc/nvidia/tegra/pingroup.h>
#include <stdint.h>

enum {
	PINGROUP_AO1_INDEX = 0,		/* offset 0x868 */
	PINGROUP_AO2_INDEX = 1,
	PINGROUP_AT1_INDEX = 2,
	PINGROUP_AT2_INDEX = 3,
	PINGROUP_AT3_INDEX = 4,
	PINGROUP_AT4_INDEX = 5,
	PINGROUP_AT5_INDEX = 6,
	PINGROUP_CDEV1_INDEX = 7,
	PINGROUP_CDEV2_INDEX = 8,
	PINGROUP_DAP1_INDEX = 10,	/* offset 0x890 */
	PINGROUP_DAP2_INDEX = 11,
	PINGROUP_DAP3_INDEX = 12,
	PINGROUP_DAP4_INDEX = 13,
	PINGROUP_DBG_INDEX = 14,
	PINGROUP_SDIO3_INDEX = 18,	/* offset 0x8B0 */
	PINGROUP_SPI_INDEX = 19,
	PINGROUP_UAA_INDEX = 20,
	PINGROUP_UAB_INDEX = 21,
	PINGROUP_UART2_INDEX = 22,
	PINGROUP_UART3_INDEX = 23,
	PINGROUP_SDIO1_INDEX = 33,	/* offset 0x8EC */
	PINGROUP_DDC_INDEX = 37,	/* offset 0x8FC */
	PINGROUP_GMA_INDEX = 38,
	PINGROUP_GME_INDEX = 42,	/* offset 0x910 */
	PINGROUP_GMF_INDEX = 43,
	PINGROUP_GMG_INDEX = 44,
	PINGROUP_GMH_INDEX = 45,
	PINGROUP_OWR_INDEX = 46,
	PINGROUP_UAD_INDEX = 47,
	PINGROUP_DEV3_INDEX = 49,	/* offset 0x92c */
	PINGROUP_CEC_INDEX = 52,	/* offset 0x938 */
	PINGROUP_AT6_INDEX = 75,	/* offset 0x994 */
	PINGROUP_DAP5_INDEX = 76,
	PINGROUP_VBUS_INDEX = 77,
	PINGROUP_AO3_INDEX = 78,
	PINGROUP_HVC_INDEX = 79,
	PINGROUP_SDIO4_INDEX = 80,
	PINGROUP_AO0_INDEX = 81,
};

#endif	/* __SOC_NVIDIA_TEGRA124_PINGROUP_H__ */
