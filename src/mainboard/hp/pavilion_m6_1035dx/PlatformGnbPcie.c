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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "PlatformGnbPcieComplex.h"

#include <vendorcode/amd/agesa/f15tn/Proc/CPU/heapManager.h>

#define FILECODE PROC_GNB_PCIE_FAMILY_0X15_F15PCIECOMPLEXCONFIG_FILECODE

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

static const PCIe_PORT_DESCRIPTOR PortList [] = {
	/* PCIe port, Lanes 8:23, PCI Device Number 2, PCIE SLOT0 x16 */
	{
		0, /* Descriptor flags */
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 8, 23),
		PCIE_PORT_DATA_INITIALIZER (PortEnabled, ChannelTypeExt6db, 2, HotplugDisabled, PcieGenMaxSupported, PcieGenMaxSupported, AspmDisabled, 1)
	},
	/* PCIe port, Lanes 16:23, PCI Device Number 3, Disabled */
	{
		0, /* Descriptor flags */
		PCIE_ENGINE_DATA_INITIALIZER (PcieUnusedEngine, 16, 23),
		PCIE_PORT_DATA_INITIALIZER (PortDisabled, ChannelTypeExt6db, 3, HotplugDisabled, PcieGenMaxSupported, PcieGenMaxSupported, AspmDisabled, 1)
	},

	/* PCIe port, Lanes 4, PCI Device Number 4, PCIE MINI0 */
	{
		0, /* Descriptor flags */
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 4, 4),
		PCIE_PORT_DATA_INITIALIZER (PortEnabled, ChannelTypeExt6db, 4, HotplugDisabled, PcieGenMaxSupported, PcieGenMaxSupported, AspmDisabled, 1)
	},

	/* PCIe port, Lanes 5, PCI Device Number 5, PCIE MINI1 */
	{
		0, /* Descriptor flags */
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 5, 5),
		PCIE_PORT_DATA_INITIALIZER (PortEnabled, ChannelTypeExt6db, 5, HotplugDisabled, PcieGenMaxSupported, PcieGenMaxSupported, AspmDisabled, 1)
	},

	/* PCIe port, Lanes 6, PCI Device Number 6, PCIE SLOT1 x1 */
	{
		0, /* Descriptor flags */
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 6, 6),
		PCIE_PORT_DATA_INITIALIZER (PortEnabled, ChannelTypeExt6db, 6, HotplugDisabled, PcieGenMaxSupported, PcieGenMaxSupported, AspmDisabled, 1)
	},

	/* PCIe port, Lanes 7, PCI Device Number 7, LAN */
	{
		0, /* Descriptor flags */
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 7, 7),
		PCIE_PORT_DATA_INITIALIZER (PortEnabled, ChannelTypeExt6db, 7, HotplugDisabled, PcieGenMaxSupported, PcieGenMaxSupported, AspmDisabled, 1)
	},

	/* PCIe port, Lanes 0:3, PCI Device Number 8, Bridge to FCH */
	{
		DESCRIPTOR_TERMINATE_LIST, /* Descriptor flags  !!!IMPORTANT!!! Terminate last element of array */
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 0, 3),
		PCIE_PORT_DATA_INITIALIZER (PortEnabled, ChannelTypeExt6db, 8, HotplugDisabled, PcieGenMaxSupported, PcieGenMaxSupported, AspmDisabled, 0)
	},
};

static const PCIe_DDI_DESCRIPTOR DdiList [] = {
	/* DP0 to HDMI0/DP */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 24, 27),
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDP, Aux1, Hdp1)
	},
	/* DP1 to FCH */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 28, 31),
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeNutmegDpToVga, Aux2, Hdp2)
	},
	/* DP2 to HDMI1/DP */
	{
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 32, 35),
		/* PCIE_DDI_DATA_INITIALIZER (ConnectorTypeEDP, Aux3, Hdp3) */
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDP, Aux3, Hdp3)
	},
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
 *    @param[in]      **PeiServices
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
	AGESA_STATUS            Status;
	PCIe_COMPLEX_DESCRIPTOR *PcieComplexListPtr;

	ALLOCATE_HEAP_PARAMS AllocHeapParams;

	/* GNB PCIe topology Porting */

	/*  */
	/* Allocate buffer for PCIe_COMPLEX_DESCRIPTOR , PCIe_PORT_DESCRIPTOR and PCIe_DDI_DESCRIPTOR */
	/*  */
	AllocHeapParams.RequestedBufferSize = sizeof(PCIe_COMPLEX_DESCRIPTOR);

	AllocHeapParams.BufferHandle = AMD_MEM_MISC_HANDLES_START;
	AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
	Status = HeapAllocateBuffer (&AllocHeapParams, &InitEarly->StdHeader);
	if ( Status!= AGESA_SUCCESS) {
		/* Could not allocate buffer for PCIe_COMPLEX_DESCRIPTOR */
		ASSERT(FALSE);
		return;
	}

	PcieComplexListPtr  =  (PCIe_COMPLEX_DESCRIPTOR *) AllocHeapParams.BufferPtr;

	LibAmdMemFill (PcieComplexListPtr,
		       0,
		       sizeof(PCIe_COMPLEX_DESCRIPTOR),
		       &InitEarly->StdHeader);

	PcieComplexListPtr->Flags        = DESCRIPTOR_TERMINATE_LIST;
	PcieComplexListPtr->SocketId     = 0;
	PcieComplexListPtr->PciePortList = PortList;
	PcieComplexListPtr->DdiLinkList  = DdiList;

	InitEarly->GnbConfig.PcieComplexList = PcieComplexListPtr;
}
