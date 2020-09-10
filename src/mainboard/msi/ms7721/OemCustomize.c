/* SPDX-License-Identifier: GPL-2.0-only */

#include <Porting.h>
#include <AGESA.h>

#include <northbridge/amd/agesa/state_machine.h>
#include <PlatformMemoryConfiguration.h>

/*
 * Lane ID Mapping (from Fam15h BKDG: Table 45: Lane Id Mapping)
 *
 * Lane Id
 * 0 P_UMI_[T,R]X[P,N]0 - southbridge link, connect via dev 8
 * 1 P_UMI_[T,R]X[P,N]1 - southbridge link, connect via dev 8
 * 2 P_UMI_[T,R]X[P,N]2 - southbridge link, connect via dev 8
 * 3 P_UMI_[T,R]X[P,N]3 - southbridge link, connect via dev 8
 * 4 P_GPP_[T,R]X[P,N]0 - may connect to PCI dev 4 - 7
 * 5 P_GPP_[T,R]X[P,N]1 - may connect to PCI dev 4 - 7
 * 6 P_GPP_[T,R]X[P,N]2 - may connect to PCI dev 4 - 7
 * 7 P_GPP_[T,R]X[P,N]3 - may connect to PCI dev 4 - 7
 * 8 P_GFX_[T,R]X[P,N]0 - may be used to form GFX slot or DDI
 * 9 P_GFX_[T,R]X[P,N]1 - may be used to form GFX slot or DDI
 * 10 P_GFX_[T,R]X[P,N]2 - may be used to form GFX slot or DDI
 * 11 P_GFX_[T,R]X[P,N]3 - may be used to form GFX slot or DDI
 * 12 P_GFX_[T,R]X[P,N]4 - may be used to form GFX slot or DDI
 * 13 P_GFX_[T,R]X[P,N]5 - may be used to form GFX slot or DDI
 * 14 P_GFX_[T,R]X[P,N]6 - may be used to form GFX slot or DDI
 * 15 P_GFX_[T,R]X[P,N]7 - may be used to form GFX slot or DDI
 * 16 P_GFX_[T,R]X[P,N]8 - may be used to form GFX slot or DDI
 * 17 P_GFX_[T,R]X[P,N]9 - may be used to form GFX slot or DDI
 * 18 P_GFX_[T,R]X[P,N]10 - may be used to form GFX slot or DDI
 * 19 P_GFX_[T,R]X[P,N]11 - may be used to form GFX slot or DDI
 * 20 P_GFX_[T,R]X[P,N]12 - may be used to form GFX slot or DDI
 * 21 P_GFX_[T,R]X[P,N]13 - may be used to form GFX slot or DDI
 * 22 P_GFX_[T,R]X[P,N]14 - may be used to form GFX slot or DDI
 * 23 P_GFX_[T,R]X[P,N]15 - may be used to form GFX slot or DDI
 * 24 DP0_TX[P,N]0 - rest is just for DDI (graphics outputs)
 * 25 DP0_TX[P,N]1
 * 26 DP0_TX[P,N]2
 * 27 DP0_TX[P,N]3
 * 28 DP1_TX[P,N]0
 * 29 DP1_TX[P,N]1
 * 30 DP1_TX[P,N]2
 * 31 DP1_TX[P,N]3
 * 32 DP2_TX[P,N]0
 * 33 DP2_TX[P,N]1
 * 34 DP2_TX[P,N]2
 * 35 DP2_TX[P,N]3
 * 36 DP2_TX[P,N]4
 * 37 DP2_TX[P,N]5
 * 38 DP2_TX[P,N]6
 */

static const PCIe_PORT_DESCRIPTOR PortList[] = {
	/* PCIe port, Lanes 8:23, PCI Device Number 2, x16 slot */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 8, 23),
		PCIE_PORT_DATA_INITIALIZER(PortEnabled, ChannelTypeExt6db, 2,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 1)
	},
	/* PCIe port, Lane 4, PCI Device Number 4, Realtek LAN */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 4, 4),
		PCIE_PORT_DATA_INITIALIZER(PortEnabled, ChannelTypeExt6db, 4,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 1)
	},
	/* PCIe port, Lane 5, PCI Device Number 5, x1 slot (1) */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 5, 5),
		PCIE_PORT_DATA_INITIALIZER(PortEnabled, ChannelTypeExt6db, 5,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 1)
	},
	/* PCIe port, Lane 6, PCI Device Number 6, x1 slot (2) */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 6, 6),
		PCIE_PORT_DATA_INITIALIZER(PortEnabled, ChannelTypeExt6db, 6,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 1)
	},
	/* PCIe port, Lanes 0:3, UMI link to SB, PCI Device Number 8 */
	{
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER(PciePortEngine, 0, 3),
		PCIE_PORT_DATA_INITIALIZER(PortEnabled, ChannelTypeExt6db, 8,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0)
	},
};

static const PCIe_DDI_DESCRIPTOR DdiList[] = {
	// DP0 to HDMI0/DP
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PcieDdiEngine, 24, 27),
		PCIE_DDI_DATA_INITIALIZER(ConnectorTypeHDMI, Aux1, Hdp1)
	},
	// DP1 to FCH
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PcieDdiEngine, 28, 31),
		PCIE_DDI_DATA_INITIALIZER(ConnectorTypeNutmegDpToVga, Aux2, Hdp2)
	},
	// DP2 to HDMI1/DP
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER(PcieDdiEngine, 32, 35),
		PCIE_DDI_DATA_INITIALIZER(ConnectorTypeHDMI, Aux3, Hdp3)
	},
};

static const PCIe_COMPLEX_DESCRIPTOR PcieComplex = {
	.Flags        = DESCRIPTOR_TERMINATE_LIST,
	.SocketId     = 0,
	.PciePortList = PortList,
	.DdiLinkList  = DdiList,
};

void board_BeforeInitReset(struct sysinfo *cb, AMD_RESET_PARAMS *Reset)
{
	FCH_RESET_INTERFACE *FchReset = &Reset->FchInterface;
	FchReset->Xhci0Enable = CONFIG(HUDSON_XHCI_ENABLE);
	FchReset->Xhci1Enable = CONFIG(HUDSON_XHCI_ENABLE);
}

void board_BeforeInitEarly(struct sysinfo *cb, AMD_EARLY_PARAMS *InitEarly)
{
	InitEarly->GnbConfig.PcieComplexList = &PcieComplex;
}

/*----------------------------------------------------------------------------------------
 *                        CUSTOMER OVERRIDES MEMORY TABLE
 *----------------------------------------------------------------------------------------
 */

/*
 *  Platform Specific Overriding Table allows IBV/OEM to pass in platform information to AGESA
 *  (e.g. MemClk routing, the number of DIMM slots per channel,...). If PlatformSpecificTable
 *  is populated, AGESA will base its settings on the data from the table. Otherwise, it will
 *  use its default conservative settings.
 */
static CONST PSO_ENTRY ROMDATA PlatformMemoryTable[] = {

  NUMBER_OF_DIMMS_SUPPORTED(ANY_SOCKET, ANY_CHANNEL, 2),
  NUMBER_OF_CHANNELS_SUPPORTED(ANY_SOCKET, 2),
/*
  TODO: is this OK for DDR3 socket FM2?
  MEMCLK_DIS_MAP(ANY_SOCKET, ANY_CHANNEL, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
  CKE_TRI_MAP(ANY_SOCKET, ANY_CHANNEL, 0x05, 0x0A),
  ODT_TRI_MAP(ANY_SOCKET, ANY_CHANNEL, 0x01, 0x02, 0x00, 0x00),
  CS_TRI_MAP(ANY_SOCKET, ANY_CHANNEL, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
  */
  PSO_END
};

void board_BeforeInitPost(struct sysinfo *cb, AMD_POST_PARAMS *InitPost)
{
	InitPost->MemConfig.PlatformMemoryConfiguration = (PSO_ENTRY *)PlatformMemoryTable;
}

void board_BeforeInitMid(struct sysinfo *cb, AMD_MID_PARAMS *InitMid)
{
	/* 0 iGpuVgaAdapter, 1 iGpuVgaNonAdapter; */
	InitMid->GnbMidConfiguration.iGpuVgaMode = 0;
}
