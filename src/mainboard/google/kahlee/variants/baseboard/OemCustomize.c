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

#include <compiler.h>
#include <amdblocks/agesawrapper.h>
#include <variant/gpio.h>
#include <boardid.h>

static const PCIe_PORT_DESCRIPTOR PortList[] = {
	/* Initialize Port descriptor (PCIe port, Lanes 7:4, D2F1) for NC*/
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PcieUnusedEngine, 4, 7),
		PCIE_PORT_DATA_INITIALIZER_V2(
				PortDisabled,		/* mPortPresent */
				ChannelTypeExt6db,	/* mChannelType */
				2,			/* mDevAddress */
				1,			/* mDevFunction */
				HotplugDisabled,	/* mHotplug */
				PcieGenMaxSupported,	/* mMaxLinkSpeed */
				PcieGenMaxSupported,	/* mMaxLinkCap */
				AspmL0sL1,		/* mAspm */
				0,			/* mResetId */
				0)			/* mClkPmSupport */
	},
	/* Initialize Port descriptor (PCIe port, Lanes 0:0, D2F2) for WLAN */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 0, 0),
		PCIE_PORT_DATA_INITIALIZER_V2(
				PortEnabled,		/* mPortPresent */
				ChannelTypeExt6db,	/* mChannelType */
				2,			/* mDevAddress */
				2,			/* mDevFunction */
				HotplugDisabled,	/* mHotplug */
				PcieGenMaxSupported,	/* mMaxLinkSpeed */
				PcieGenMaxSupported,	/* mMaxLinkCap */
				AspmL0sL1,		/* mAspm */
				PCIE_0_RST,		/* mResetId */
				0)			/* mClkPmSupport */
	},
	/* Init Port descriptor (PCIe port, Lanes 1:1, D2F3) NC */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PcieUnusedEngine, 1, 1),
		PCIE_PORT_DATA_INITIALIZER_V2(
				PortDisabled,		/* mPortPresent */
				ChannelTypeExt6db,	/* mChannelType */
				2,			/* mDevAddress */
				3,			/* mDevFunction */
				HotplugDisabled,	/* mHotplug */
				PcieGenMaxSupported,	/* mMaxLinkSpeed */
				PcieGenMaxSupported,	/* mMaxLinkCap */
				AspmL0sL1,		/* mAspm */
				PCIE_1_RST,		/* mResetId */
				0)			/* mClkPmSupport */
	},
	/* Initialize Port descriptor (PCIe port, Lane 2, D2F4) for EMMC */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 2, 2),
		PCIE_PORT_DATA_INITIALIZER_V2(
				PortEnabled,		/* mPortPresent */
				ChannelTypeExt6db,	/* mChannelType */
				2,			/* mDevAddress */
				4,			/* mDevFunction */
				HotplugDisabled,	/* mHotplug */
				PcieGenMaxSupported,	/* mMaxLinkSpeed */
				PcieGenMaxSupported,	/* mMaxLinkCap */
				AspmL0sL1,		/* mAspm */
				PCIE_2_RST,		/* mResetId */
				0)			/* mClkPmSupport */
	},
	/* Initialize Port descriptor (PCIe port, Lane3, D2F5) for NC */
	{
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER(PcieUnusedEngine, 3, 3),
		PCIE_PORT_DATA_INITIALIZER_V2(
				PortDisabled,		/* mPortPresent */
				ChannelTypeExt6db,	/* mChannelType */
				2,			/* mDevAddress */
				5,			/* mDevFunction */
				HotplugDisabled,	/* mHotplug */
				PcieGenMaxSupported,	/* mMaxLinkSpeed */
				PcieGenMaxSupported,	/* mMaxLinkCap */
				AspmL0sL1,		/* mAspm */
				PCIE_3_RST,		/* mResetId */
				0)			/* mClkPmSupport */
	},
};

static const PCIe_DDI_DESCRIPTOR DdiList[] = {
	/* DDI0 - eDP */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PcieDdiEngine, 8, 11),
		PCIE_DDI_DATA_INITIALIZER(ConnectorTypeEDP, Aux1, Hdp1)
	},
	/* DDI1 - DP */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PcieDdiEngine, 12, 15),
		PCIE_DDI_DATA_INITIALIZER(ConnectorTypeDP, Aux2, Hdp2)
	},
	/* DDI2 - DP */
	{
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER(PcieDdiEngine, 16, 19),
		PCIE_DDI_DATA_INITIALIZER(ConnectorTypeDP, Aux3, Hdp3)
	},
};

static const PCIe_COMPLEX_DESCRIPTOR PcieComplex = {
	.Flags        = DESCRIPTOR_TERMINATE_LIST,
	.SocketId     = 0,
	.PciePortList = (void *)PortList,
	.DdiLinkList  = (void *)DdiList
};

/*---------------------------------------------------------------------------*/
/**
 *  OemCustomizeInitEarly
 *
 *  Description:
 *    This is the stub function will call the host environment through the
 *    binary block interface (call-out port) to provide a user hook opportunity.
 *
 *  Parameters:
 *    @param[in]      **PeiServices
 *    @param[in]      *InitEarly
 *
 *    @retval         VOID
 *
 **/
/*---------------------------------------------------------------------------*/
VOID __weak OemCustomizeInitEarly(IN OUT AMD_EARLY_PARAMS *InitEarly)
{
	InitEarly->GnbConfig.PcieComplexList = (void *)&PcieComplex;
	InitEarly->GnbConfig.PsppPolicy = PsppBalanceLow;
	InitEarly->PlatformConfig.GnbAzI2sBusSelect = GnbAcpI2sBus;
	InitEarly->PlatformConfig.GnbAzI2sBusPinConfig = GnbAcp2Tx4RxBluetooth;
}
