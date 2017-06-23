/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Advanced Micro Devices, Inc.
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

#include <agesawrapper.h>
#include <PlatformMemoryConfiguration.h>

#define FILECODE PROC_GNB_PCIE_FAMILY_0X15_F15PCIECOMPLEXCONFIG_FILECODE

static const PSO_ENTRY DDR4PlatformMemoryConfiguration[] = {
	DRAM_TECHNOLOGY(ANY_SOCKET, DDR4_TECHNOLOGY),
	NUMBER_OF_DIMMS_SUPPORTED(ANY_SOCKET, ANY_CHANNEL, 1),
	NUMBER_OF_CHANNELS_SUPPORTED(ANY_SOCKET, 2),
	MOTHER_BOARD_LAYERS(LAYERS_6),
	MEMCLK_DIS_MAP(ANY_SOCKET, ANY_CHANNEL,
				0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00),
	CKE_TRI_MAP(ANY_SOCKET, ANY_CHANNEL, 0xff, 0xff, 0xff, 0xff),
	ODT_TRI_MAP(ANY_SOCKET, ANY_CHANNEL, 0xff, 0xff, 0xff, 0xff),
	CS_TRI_MAP(ANY_SOCKET, ANY_CHANNEL,
				0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00),
	PSO_END
};

void OemPostParams(AMD_POST_PARAMS *PostParams)
{
	PostParams->MemConfig.PlatformMemoryConfiguration =
				(PSO_ENTRY *)DDR4PlatformMemoryConfiguration;
}
