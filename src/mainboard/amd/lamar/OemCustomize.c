/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012        Advanced Micro Devices, Inc.
 *               2013 - 2014 Sage Electronic Engineering, LLC
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


static const PCIe_PORT_DESCRIPTOR PortList[] = {

	/*
	 * Lanes to pins to PCI device mapping can be found in section 2.12 of the
	 * BIOS and Kernel Developer's Guide for AMD Family 15h Models 30h-3Fh
	 */

	{	/* PCIe x16 Connector J119, DP4/5/6, GFX[15:0], Lanes [31:16], PCI 00:02.1 */
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 16, 31),
		PCIE_PORT_DATA_INITIALIZER_V2(
				PortEnabled,
				ChannelTypeExt6db, 0, 0,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled,
				175,
				0
		)
	},

	{	/* PCIe x4 Connector J118, GPP[3:0], Lanes [11:8], PCI 00:03.2 */
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 8, 11),
		PCIE_PORT_DATA_INITIALIZER_V2(
				PortEnabled,
				ChannelTypeExt6db, 0, 0,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled,
				176,
				0
		)
	},

	{	/* PCIe x4 Connector J120, GPP[7:4], Lanes [15:12] */
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER(
				CONFIG(ENABLE_DP3_DAUGHTER_CARD_IN_J120) ? PcieUnusedEngine : PciePortEngine,
				12, 15
		),
		PCIE_PORT_DATA_INITIALIZER_V2(
				PortEnabled,
				ChannelTypeExt6db, 0, 0,
				HotplugDisabled,
				PcieGenMaxSupported, PcieGenMaxSupported,
				AspmDisabled,
				177,
				0
		)
	},

};

static const PCIe_DDI_DESCRIPTOR DdiList[] = {
	{	/* DP3 */
		0,
		PCIE_ENGINE_DATA_INITIALIZER(
				CONFIG(ENABLE_DP3_DAUGHTER_CARD_IN_J120) ? PcieDdiEngine : PcieUnusedEngine,
				12, 15
		),
		PCIE_DDI_DATA_INITIALIZER(ConnectorTypeDP, Aux4, Hdp4)
	},

	{	/* DP2 */
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PcieDdiEngine, 36, 39),
		PCIE_DDI_DATA_INITIALIZER(ConnectorTypeDP, Aux3, Hdp3)
	},

	{	/* DP1 */
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PcieDdiEngine, 32, 35),
		PCIE_DDI_DATA_INITIALIZER(ConnectorTypeDP, Aux2, Hdp2)
	},

	{	/* DP0 */
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER(PcieDdiEngine, 4, 7),
		PCIE_DDI_DATA_INITIALIZER(ConnectorTypeDP, Aux1, Hdp1)
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
