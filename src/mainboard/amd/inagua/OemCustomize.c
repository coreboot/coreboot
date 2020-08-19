/* SPDX-License-Identifier: GPL-2.0-only */

#include <AGESA.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <PlatformMemoryConfiguration.h>

static const PCIe_PORT_DESCRIPTOR PortList[] = {
	// Initialize Port descriptor (PCIe port, Lanes 4, PCI Device Number 4, ...) MXM
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 4, 5),
		PCIE_PORT_DATA_INITIALIZER(PortEnabled, ChannelTypeExt6db, 4,
				HotplugDisabled,
				PcieGen2,
				PcieGen2,
				AspmL0sL1, 4)
	},
	// Initialize Port descriptor (PCIe port, Lanes 6, PCI Device Number 6, ...) PCIE LAN
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 6, 6),
		PCIE_PORT_DATA_INITIALIZER(PortEnabled, ChannelTypeExt6db, 6,
				HotplugDisabled,
				PcieGen2,
				PcieGen2,
				AspmL0sL1, 6)
	},
	// Initialize Port descriptor (PCIe port, Lanes 7, PCI Device Number 7, ...) MINIPCIE SLOT1
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 7, 7),
		PCIE_PORT_DATA_INITIALIZER(PortEnabled, ChannelTypeExt6db, 7,
				HotplugDisabled,
				PcieGen2,
				PcieGen2,
				AspmL0sL1, 7)
	},
	// Initialize Port descriptor (PCIe port, Lanes 8, PCI Device Number 8, ...)
	{
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 0, 3),
		PCIE_PORT_DATA_INITIALIZER(PortEnabled, ChannelTypeExt6db, 8,
				HotplugDisabled,
				PcieGen2,
				PcieGen2,
				AspmL0sL1, 0)
	}
};

static const PCIe_DDI_DESCRIPTOR DdiList[] = {
	// Initialize Ddi descriptor (DDI interface Lanes 8:11, DdA, ...) DP0 to LVDS
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PcieDdiEngine, 8, 11),
		PCIE_DDI_DATA_INITIALIZER(ConnectorTypeLvds, Aux1, Hdp1)
	},
	// Initialize Ddi descriptor (DDI interface Lanes 12:15, DdB, ...) DP1 to VGA
	{
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER(PcieDdiEngine, 12, 15),
		PCIE_DDI_DATA_INITIALIZER(ConnectorTypeAutoDetect, Aux2, Hdp2)
	}
};

static const PCIe_COMPLEX_DESCRIPTOR PcieComplex = {
	.Flags        = DESCRIPTOR_TERMINATE_LIST,
	.SocketId     = 0,
	.PciePortList = PortList,
	.DdiLinkList  = DdiList,
};

void board_BeforeInitEarly(struct sysinfo *cb, AMD_EARLY_PARAMS *InitEarly)
{
	InitEarly->GnbConfig.PcieComplexList = &PcieComplex;
	InitEarly->GnbConfig.PsppPolicy		= 0;
}

/*----------------------------------------------------------------------------------------
 *                        CUSTOMER OVERRIDES MEMORY TABLE
 *----------------------------------------------------------------------------------------
 */

/*
 * Platform Specific Overriding Table allows IBV/OEM to pass in platform information to AGESA
 * (e.g. MemClk routing, the number of DIMM slots per channel,...). If PlatformSpecificTable
 * is populated, AGESA will base its settings on the data from the table. Otherwise, it will
 * use its default conservative settings.
 */
static CONST PSO_ENTRY ROMDATA PlatformMemoryTable[] = {
	NUMBER_OF_DIMMS_SUPPORTED(ANY_SOCKET, ANY_CHANNEL, 2),
	NUMBER_OF_CHANNELS_SUPPORTED(ANY_SOCKET, 1),
	PSO_END
};

void board_BeforeInitPost(struct sysinfo *cb, AMD_POST_PARAMS *InitPost)
{
	InitPost->MemConfig.PlatformMemoryConfiguration = (PSO_ENTRY *)PlatformMemoryTable;
}
