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

#include <northbridge/amd/pi/agesawrapper.h>

#define FILECODE PROC_GNB_PCIE_FAMILY_0X15_F15PCIECOMPLEXCONFIG_FILECODE
static const PCIe_PORT_DESCRIPTOR PortList [] = {
	/* Initialize Port descriptor (PCIe port, Lanes 7:4, D2F1) for x4 slot */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PcieUnusedEngine, 4, 7),
		PCIE_PORT_DATA_INITIALIZER_V2 (PortEnabled, ChannelTypeExt6db, 2, 1,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmL0sL1, 0x04, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lanes 1:0, D2F2) for M.2 */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PcieUnusedEngine, 0, 1),
		PCIE_PORT_DATA_INITIALIZER_V2 (PortDisabled, ChannelTypeExt6db, 2, 2,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmL0sL1, 0x17, 0)
	},
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PcieUnusedEngine, 1, 1),
		PCIE_PORT_DATA_INITIALIZER_V2 (PortDisabled, ChannelTypeExt6db, 2, 3,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmL0sL1, 0x17, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lane 2, D2F4) for x1 slot */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 2, 2),
		PCIE_PORT_DATA_INITIALIZER_V2 (PortEnabled, ChannelTypeExt6db, 2, 4,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmL0sL1, 0x13, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lane3, D2F5) for SD */
	{
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 3, 3),
		PCIE_PORT_DATA_INITIALIZER_V2 (PortEnabled, ChannelTypeExt6db, 2, 5,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmL0sL1, 0x16, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lane 1, D2F3) for M.2 */
};

static const PCIe_DDI_DESCRIPTOR DdiList [] = {
	/* DDI0 - eDP */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 8, 11),
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeEDP, Aux1, Hdp1)
	},
	/* DDI1 - DP */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 12, 15),
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDP, Aux2, Hdp2)
	},
	/* DDI2 - HDMI */
	{
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 16, 19),
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeHDMI, Aux3, Hdp3)
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
 *    This is the stub function will call the host environment through the binary block
 *    interface (call-out port) to provide a user hook opportunity
 *
 *  Parameters:
 *    @param[in]      **PeiServices
 *    @param[in]      *InitEarly
 *
 *    @retval         VOID
 *
 **/
/*---------------------------------------------------------------------------------------*/
VOID OemCustomizeInitEarly (
	IN  OUT AMD_EARLY_PARAMS    *InitEarly
	)
{
	InitEarly->GnbConfig.PcieComplexList = &PcieComplex;
}
