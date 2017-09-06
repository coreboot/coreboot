/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#include <AGESA.h>
#include <PlatformMemoryConfiguration.h>


static const PCIe_PORT_DESCRIPTOR PortList[] = {
	/* Initialize Port descriptor (PCIe port, Lane 3, PCI Device 2, Function 5) */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 3, 3),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db, 2, 5,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x01, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lane 2, PCI Device 2, Function 4) */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 2, 2),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db, 2, 4,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x02, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lane 1, PCI Device 2, Function 3) */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 1, 1),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db, 2, 3,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x03, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lane 0, PCI Device 2, Function 2) */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 0, 0),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db, 2, 2,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x04, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lanes 4-7, PCI Device 2, Function 1) */
	{
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 4, 7),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db, 2, 1,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x05, 0)
	}
};

static const PCIe_DDI_DESCRIPTOR DdiList[] = {
	/* DP0 to HDMI0/DP */
	{
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER(PcieDdiEngine, 8, 11),
		PCIE_DDI_DATA_INITIALIZER(ConnectorTypeHDMI, Aux1, Hdp1)
	},
};

static const PCIe_COMPLEX_DESCRIPTOR PcieComplex = {
	.Flags        = DESCRIPTOR_TERMINATE_LIST,
	.SocketId     = 0,
	.PciePortList = PortList,
	.DdiLinkList  = DdiList
};

/*---------------------------------------------------------------------------------------*/
/**
 *  OemCustomizeInitEarly
 *
 *  Description:
 *    This stub function will call the host environment through the binary block
 *    interface (call-out port) to provide a user hook opportunity
 *
 *  Parameters:
 *    @param[in]      *InitEarly
 *
 *    @retval         VOID
 *
 **/
/*---------------------------------------------------------------------------------------*/
VOID
OemCustomizeInitEarly (
	IN  OUT AMD_EARLY_PARAMS    *InitEarly
	)
{
	InitEarly->GnbConfig.PcieComplexList = &PcieComplex;
}

/*
 *  Platform Specific Overriding Table allows IBV/OEM to pass in platform information to AGESA
 *  (e.g. MemClk routing, the number of DIMM slots per channel,...). If PlatformSpecificTable
 *  is populated, AGESA will base its settings on the data from the table. Otherwise, it will
 *  use its default conservative settings.
 */
static const PSO_ENTRY ROMDATA PlatformMemoryConfiguration[] = {
  /*
   * The following macros are supported (use comma to separate macros):
   *
   * MEMCLK_DIS_MAP(SocketID, ChannelID, MemClkDisBit0CSMap,..., MemClkDisBit7CSMap)
   *      The MemClk pins are identified based on BKDG definition of Fn2x88[MemClkDis] bitmap.
   *      AGESA will base on this value to disable unused MemClk to save power.
   *      Example:
   *      BKDG definition of Fn2x88[MemClkDis] bitmap for AM3 package is like below:
   *           Bit AM3/S1g3 pin name
   *           0   M[B,A]_CLK_H/L[0]
   *           1   M[B,A]_CLK_H/L[1]
   *           2   M[B,A]_CLK_H/L[2]
   *           3   M[B,A]_CLK_H/L[3]
   *           4   M[B,A]_CLK_H/L[4]
   *           5   M[B,A]_CLK_H/L[5]
   *           6   M[B,A]_CLK_H/L[6]
   *           7   M[B,A]_CLK_H/L[7]
   *      And platform has the following routing:
   *           CS0   M[B,A]_CLK_H/L[4]
   *           CS1   M[B,A]_CLK_H/L[2]
   *           CS2   M[B,A]_CLK_H/L[3]
   *           CS3   M[B,A]_CLK_H/L[5]
   *      Then platform can specify the following macro:
   *      MEMCLK_DIS_MAP(ANY_SOCKET, ANY_CHANNEL, 0x00, 0x00, 0x02, 0x04, 0x01, 0x08, 0x00, 0x00)
   *
   * CKE_TRI_MAP(SocketID, ChannelID, CKETriBit0CSMap, CKETriBit1CSMap)
   *      The CKE pins are identified based on BKDG definition of Fn2x9C_0C[CKETri] bitmap.
   *      AGESA will base on this value to tristate unused CKE to save power.
   *
   * ODT_TRI_MAP(SocketID, ChannelID, ODTTriBit0CSMap,..., ODTTriBit3CSMap)
   *      The ODT pins are identified based on BKDG definition of Fn2x9C_0C[ODTTri] bitmap.
   *      AGESA will base on this value to tristate unused ODT pins to save power.
   *
   * CS_TRI_MAP(SocketID, ChannelID, CSTriBit0CSMap,..., CSTriBit7CSMap)
   *      The Chip select pins are identified based on BKDG definition of Fn2x9C_0C[ChipSelTri] bitmap.
   *      AGESA will base on this value to tristate unused Chip select to save power.
   *
   * NUMBER_OF_DIMMS_SUPPORTED(SocketID, ChannelID, NumberOfDimmSlotsPerChannel)
   *      Specifies the number of DIMM slots per channel.
   *
   * NUMBER_OF_CHIP_SELECTS_SUPPORTED(SocketID, ChannelID, NumberOfChipSelectsPerChannel)
   *      Specifies the number of Chip selects per channel.
   *
   * NUMBER_OF_CHANNELS_SUPPORTED(SocketID, NumberOfChannelsPerSocket)
   *      Specifies the number of channels per socket.
   *
   * OVERRIDE_DDR_BUS_SPEED(SocketID, ChannelID, USER_MEMORY_TIMING_MODE, MEMORY_BUS_SPEED)
   *      Specifies DDR bus speed of channel ChannelID on socket SocketID.
   *
   * DRAM_TECHNOLOGY(SocketID, TECHNOLOGY_TYPE)
   *      Specifies the DRAM technology type of socket SocketID (DDR2, DDR3,...)
   *
   * WRITE_LEVELING_SEED(SocketID, ChannelID, DimmID, Byte0Seed, Byte1Seed, Byte2Seed, Byte3Seed, Byte4Seed, Byte5Seed,
   *      Byte6Seed, Byte7Seed, ByteEccSeed)
   *      Specifies the write leveling seed for a channel of a socket.
   *
   * HW_RXEN_SEED(SocketID, ChannelID, DimmID, Byte0Seed, Byte1Seed, Byte2Seed, Byte3Seed, Byte4Seed, Byte5Seed,
   *      Byte6Seed, Byte7Seed, ByteEccSeed)
   *      Speicifes the HW RXEN training seed for a channel of a socket
   */

#define SEED_WL 0x0E
WRITE_LEVELING_SEED(
       ANY_SOCKET, CHANNEL_A, ALL_DIMMS,
       SEED_WL,SEED_WL,SEED_WL,SEED_WL,SEED_WL,SEED_WL,SEED_WL,SEED_WL,
       SEED_WL),

#define SEED_A 0x12
HW_RXEN_SEED(
       ANY_SOCKET, CHANNEL_A, ALL_DIMMS,
       SEED_A, SEED_A, SEED_A, SEED_A, SEED_A, SEED_A, SEED_A, SEED_A,
       SEED_A),

  NUMBER_OF_DIMMS_SUPPORTED(ANY_SOCKET, ANY_CHANNEL, 1),
  NUMBER_OF_CHANNELS_SUPPORTED(ANY_SOCKET, 1),
  MOTHER_BOARD_LAYERS(LAYERS_6),

  MEMCLK_DIS_MAP(ANY_SOCKET, ANY_CHANNEL, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
  CKE_TRI_MAP(ANY_SOCKET, ANY_CHANNEL, 0x01, 0x02, 0x04, 0x08), /* TODO: bit2map, bit3map */
  ODT_TRI_MAP(ANY_SOCKET, ANY_CHANNEL, 0x01, 0x02, 0x04, 0x08),
  CS_TRI_MAP(ANY_SOCKET, ANY_CHANNEL, 0x01, 0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00),

  PSO_END
};

void OemPostParams(AMD_POST_PARAMS *PostParams)
{
	/* Add the memory configuration table needed for soldered down memory */
	PostParams->MemConfig.PlatformMemoryConfiguration = (PSO_ENTRY *)PlatformMemoryConfiguration;
}
