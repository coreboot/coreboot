/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/agesawrapper.h>

#define FILECODE PROC_GNB_PCIE_FAMILY_0X15_F15PCIECOMPLEXCONFIG_FILECODE

/* Port descriptor list for Gardenia Rev. B */
static const PCIe_PORT_DESCRIPTOR PortList[] = {
	/* Init port descriptor (PCIe port, Lanes 7:4, D2F1) for x4 slot */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 4, 7),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db,
				2, 1,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmL0sL1, 0x04, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lanes 1:0, D2F2) for M.2 */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 0, 1),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db,
				2, 2,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmL0sL1, 0x17, 0)
	},
	/* Disable M.2 x1 on lane 1, D2F3 */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PcieUnusedEngine, 1, 1),
		PCIE_PORT_DATA_INITIALIZER_V2(PortDisabled, ChannelTypeExt6db,
				2, 3,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmL0sL1, 0x17, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lane 2, D2F4) for x1 slot */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 2, 2),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db,
				2, 4,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmL0sL1, 0x13, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lane3, D2F5) for SD */
	{
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 3, 3),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db,
				2, 5,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmL0sL1, 0x16, 0)
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
	/* DDI2 - HDMI */
	{
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER(PcieDdiEngine, 16, 19),
		PCIE_DDI_DATA_INITIALIZER(ConnectorTypeHDMI, Aux3, Hdp3)
	},
};

static const PCIe_COMPLEX_DESCRIPTOR PcieComplex = {
	.Flags        = DESCRIPTOR_TERMINATE_LIST,
	.SocketId     = 0,
	.PciePortList = (void *)PortList,
	.DdiLinkList  = (void *)DdiList
};

static const UINT32 AzaliaCodecAlc286Table[] = {
	0x00172051, 0x001721C7, 0x00172222, 0x00172310,
	0x0017FF00, 0x0017FF00, 0x0017FF00, 0x0017FF00,
	0x01271C50, 0x01271D01, 0x01271EA6, 0x01271FB7,
	0x01371C00, 0x01371D00, 0x01371E00, 0x01371F40,
	0x01471C10, 0x01471D01, 0x01471E17, 0x01471F90,
	0x01771CF0, 0x01771D11, 0x01771E11, 0x01771F41,
	0x01871C40, 0x01871D10, 0x01871EA1, 0x01871F04,
	0x01971CF0, 0x01971D11, 0x01971E11, 0x01971F41,
	0x01A71CF0, 0x01A71D11, 0x01A71E11, 0x01A71F41,
	0x01D71C2D, 0x01D71DA5, 0x01D71E67, 0x01D71F40,
	0x01E71C30, 0x01E71D11, 0x01E71E45, 0x01E71F04,
	0x02171C20, 0x02171D10, 0x02171E21, 0x02171F04,
	0x02050071, 0x02040014, 0x02050010, 0x02040C22,
	0x0205004F, 0x0204B029, 0x0205002B, 0x02040C50,
	0x0205002D, 0x02041020, 0x02050020, 0x02040000,
	0x02050019, 0x02040817, 0x02050035, 0x02041AA5,
	0x02050063, 0x02042906, 0x02050063, 0x02042906,
	0xffffffff
};

static CONST CODEC_VERB_TABLE_LIST CodecTableList[] = {
	{ 0x10ec0286, AzaliaCodecAlc286Table},
	{ 0x0FFFFFFFF, (void *)0x0FFFFFFFF}
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
VOID OemCustomizeInitEarly(IN OUT AMD_EARLY_PARAMS *InitEarly)
{
	InitEarly->GnbConfig.PcieComplexList = (void *)&PcieComplex;
	InitEarly->PlatformConfig.AzaliaCodecVerbTable =
					(uint64_t)(uintptr_t)CodecTableList;
}
