/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/amd/stoneyridge/chip.h>
#include <amdblocks/agesawrapper.h>

#define DIMMS_PER_CHANNEL 1
#if DIMMS_PER_CHANNEL > MAX_DIMMS_PER_CH
#error "Too many DIMM sockets defined for the mainboard"
#endif

static const PSO_ENTRY DDR4PlatformMemoryConfiguration[] = {
	DRAM_TECHNOLOGY(ANY_SOCKET, DDR4_TECHNOLOGY),
	NUMBER_OF_DIMMS_SUPPORTED(ANY_SOCKET, CHANNEL_A, 1),
	NUMBER_OF_DIMMS_SUPPORTED(ANY_SOCKET, CHANNEL_B, 1),
	NUMBER_OF_CHANNELS_SUPPORTED(ANY_SOCKET, MAX_DRAM_CH),
	MOTHER_BOARD_LAYERS(LAYERS_6),
	MEMCLK_DIS_MAP(ANY_SOCKET, CHANNEL_A, 0x01, 0x02, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00),
	MEMCLK_DIS_MAP(ANY_SOCKET, CHANNEL_B, 0x01, 0x02, 0x08, 0x04,
						0x00, 0x00, 0x00, 0x00),
	CKE_TRI_MAP(ANY_SOCKET, CHANNEL_A, 0x01, 0x02, 0x00, 0x00),
	CKE_TRI_MAP(ANY_SOCKET, CHANNEL_B, 0x05, 0x0A, 0x00, 0x00),
	ODT_TRI_MAP(ANY_SOCKET, CHANNEL_A, 0x01, 0x00, 0x02, 0x00),
	ODT_TRI_MAP(ANY_SOCKET, CHANNEL_B, 0x01, 0x04, 0x02, 0x08),
	CS_TRI_MAP(ANY_SOCKET, CHANNEL_A, 0x01, 0x02, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00),
	CS_TRI_MAP(ANY_SOCKET, CHANNEL_B, 0x01, 0x02, 0x04, 0x08, 0x00,
						0x00, 0x00, 0x00),
	PSO_END
};

void OemPostParams(AMD_POST_PARAMS *PostParams)
{
	PostParams->MemConfig.PlatformMemoryConfiguration =
				(PSO_ENTRY *)DDR4PlatformMemoryConfiguration;
	PostParams->MemConfig.CfgUmaAbove4G = TRUE;
}
