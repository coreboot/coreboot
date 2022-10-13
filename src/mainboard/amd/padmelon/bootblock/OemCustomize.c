/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/agesawrapper.h>

/*
 * TODO:
 * Check if a separate PCIe port list is needed for Prairie Falcon APUs. Only Merlin Falcon has
 * PCIe root ports on the functions of bus 0 device 3.
 */

static const PCIe_PORT_DESCRIPTOR PortList[] = {
	/*
	 * Init Port descriptor (PCIe port, Lanes 8-15,
	 * PCI Device Number 3, ...)
	 */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 8, 15),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db,
				3, 1,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x02, 0)
	},

	/*
	 * Initialize Port descriptor (PCIe port, Lane 7,
	 * PCI Device Number 2, ...)
	 */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 7, 7),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db,
				2, 5,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x03, 0)
	},
	/*
	 * Initialize Port descriptor (PCIe port, Lane 6,
	 * PCI Device Number 2, ...)
	 */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 6, 6),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db,
				2, 4,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x04, 0)
	},
	/*
	 * Initialize Port descriptor (PCIe port, Lane 5,
	 * PCI Device Number 2, ...)
	 */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PcieUnusedEngine, 5, 5),
		PCIE_PORT_DATA_INITIALIZER_V2(PortDisabled, ChannelTypeExt6db,
				2, 3,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x04, 0)
	},
	/*
	 * Initialize Port descriptor (PCIe port, Lane4,
	 * PCI Device Number 2, ...)
	 */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 4, 4),
		PCIE_PORT_DATA_INITIALIZER_V2(PortEnabled, ChannelTypeExt6db,
				2, 2,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x06, 0)
	},
	/*
	 * Initialize Port descriptor (PCIe port, Lanes 0-3,
	 * PCI Device Number 2, ...)
	 */
	{
		/*
		 * Descriptor flags  !!!IMPORTANT!!! Terminate last element
		 * of array
		 */
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER(PcieUnusedEngine, 0, 3),
		PCIE_PORT_DATA_INITIALIZER_V2(PortDisabled, ChannelTypeExt6db,
				2, 1,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x07, 0)
	},

};

static const PCIe_DDI_DESCRIPTOR DdiList[] = {
	/* DP0 */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PcieDdiEngine, 16, 19),
		PCIE_DDI_DATA_INITIALIZER(ConnectorTypeDP, Aux1, Hdp1)
	},
	/* DP1 */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PcieDdiEngine, 20, 23),
		PCIE_DDI_DATA_INITIALIZER(ConnectorTypeDP, Aux2, Hdp2)
	},
	/* DP2 */
	{
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER(PcieDdiEngine, 24, 27),
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
VOID OemCustomizeInitEarly(AMD_EARLY_PARAMS *InitEarly)
{
	InitEarly->GnbConfig.PcieComplexList = (void *)&PcieComplex;
}
