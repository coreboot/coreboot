/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Advanced Micro Devices, Inc.
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
	/* Initialize Port descriptor (PCIe port, Lanes 8-15, PCI Device Number 3, ...) */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 8, 15),
		PCIE_PORT_DATA_INITIALIZER_V2 (PortEnabled, ChannelTypeExt6db, 3, 1,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x02, 0)
	},

	/* Initialize Port descriptor (PCIe port, Lanes 2, PCI Device Number 2, ...) */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 7, 7),
		PCIE_PORT_DATA_INITIALIZER_V2 (PortEnabled, ChannelTypeExt6db, 2, 5,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x03, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lanes 3, PCI Device Number 2, ...) */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PcieUnusedEngine, 6, 6),
		PCIE_PORT_DATA_INITIALIZER_V2 (PortDisabled, ChannelTypeExt6db, 2, 4,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x04, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lanes 4-7, PCI Device Number 2, ...) */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PcieUnusedEngine, 5, 5),
		PCIE_PORT_DATA_INITIALIZER_V2 (PortEnabled, ChannelTypeExt6db, 2, 3,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x05, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lanes 4-5, PCI Device Number 2, ...) */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 4, 5),
		PCIE_PORT_DATA_INITIALIZER_V2 (PortEnabled, ChannelTypeExt6db, 2, 2,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x06, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lanes 0-3, PCI Device Number 2, ...) */
	{
		DESCRIPTOR_TERMINATE_LIST, // Descriptor flags  !!!IMPORTANT!!! Terminate last element of array
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 0, 3),
		PCIE_PORT_DATA_INITIALIZER_V2 (PortEnabled, ChannelTypeExt6db, 2, 1,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x07, 0)
	},

};

static const PCIe_DDI_DESCRIPTOR DdiList [] = {
	/* DP0 */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 16, 19),
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDP, Aux1, Hdp1)
	},
	/* DP1 */
	{
		0, //DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 20, 23),
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDP, Aux2, Hdp2)
	},
	/* DP2 */
	{
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 24, 27),
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDP, Aux3, Hdp3)
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
