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

#include <AGESA.h>
#include <northbridge/amd/agesa/state_machine.h>

#define PCIE_NIC_RESET_ID	1

#if CONFIG(BOARD_PCENGINES_APU2)
#define	PCIE_GFX_RESET_ID	55
#define PCIE_PORT3_RESET_ID	51
#else
#define	PCIE_GFX_RESET_ID	PCIE_NIC_RESET_ID
#define PCIE_PORT3_RESET_ID	PCIE_NIC_RESET_ID
#endif

static const PCIe_PORT_DESCRIPTOR PortList[] = {
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 3, 3),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db, 2, 5,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmL0sL1,
				PCIE_PORT3_RESET_ID,
				0)
	},
	/* Initialize Port descriptor (PCIe port, Lanes 1, PCI Device Number 2, ...) */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 2, 2),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db, 2, 4,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmL0sL1,
				PCIE_NIC_RESET_ID,
				0)
	},
	/* Initialize Port descriptor (PCIe port, Lanes 2, PCI Device Number 2, ...) */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 1, 1),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db, 2, 3,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmL0sL1,
				PCIE_NIC_RESET_ID,
				0)
	},
	/* Initialize Port descriptor (PCIe port, Lanes 3, PCI Device Number 2, ...) */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 0, 0),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db, 2, 2,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmL0sL1,
				PCIE_NIC_RESET_ID,
				0)
	},
	/* Initialize Port descriptor (PCIe port, Lanes 4-7, PCI Device Number 4, ...) */
	{
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 4, 7),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db, 2, 1,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmL0sL1,
				PCIE_GFX_RESET_ID,
				0)
	}
};

static const PCIe_COMPLEX_DESCRIPTOR PcieComplex = {
	.Flags        = DESCRIPTOR_TERMINATE_LIST,
	.SocketId     = 0,
	.PciePortList = PortList,
	.DdiLinkList  = NULL,
};

void board_BeforeInitEarly(struct sysinfo *cb, AMD_EARLY_PARAMS *InitEarly)
{
	InitEarly->GnbConfig.PcieComplexList = &PcieComplex;
	InitEarly->PlatformConfig.CStateMode = CStateModeC6;
	InitEarly->PlatformConfig.CpbMode = CpbModeAuto;
}
