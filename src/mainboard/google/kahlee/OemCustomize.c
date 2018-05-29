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

#include <chip.h>
#include <amdblocks/agesawrapper.h>

#define DIMMS_PER_CHANNEL 1
#if DIMMS_PER_CHANNEL > MAX_DIMMS_PER_CH
#error "Too many DIMM sockets defined for the mainboard"
#endif

static const PSO_ENTRY DDR4PlatformMemoryConfiguration[] = {
	DRAM_TECHNOLOGY(ANY_SOCKET, DDR4_TECHNOLOGY),
	NUMBER_OF_DIMMS_SUPPORTED(ANY_SOCKET, ANY_CHANNEL, DIMMS_PER_CHANNEL),
	NUMBER_OF_CHANNELS_SUPPORTED(ANY_SOCKET, MAX_DRAM_CH),
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

	/*
	 * Bank interleaving is enabled by default in AGESA. However, from AMD's
	 * explanation, bank interleaving is really chip select interleave,
	 * requiring 2 chip select arriving to the DIMM (rank interleaving). As
	 * both kahlee and grunt are hardware limited to a single chip select
	 * arriving at the DIMM, interleave will not work. This causes AGESA to
	 * throw a warning. To avoid the warning, interleaving needs to be
	 * disabled.
	 */
	PostParams->MemConfig.EnableBankIntlv = FALSE;
}
