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

#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "heapManager.h"
#include "PlatformGnbPcieComplex.h"
#include "Filecode.h"

#define FILECODE PROC_GNB_PCIE_FAMILY_0X15_F15PCIECOMPLEXCONFIG_FILECODE

PCIe_PORT_DESCRIPTOR PortList [] = {
	/* PCIe port, Lanes 8:23, PCI Device Number 2 */
	{
		0, /* Descriptor flags */
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 8, 23),
		PCIE_PORT_DATA_INITIALIZER (PortEnabled, ChannelTypeExt6db, 2, HotplugDisabled, PcieGenMaxSupported, PcieGenMaxSupported, AspmDisabled, 1)
	},
	/* PCIe port, Lanes 16:23, PCI Device Number 3 */
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

	/* PCIe port, Lanes 6, PCI Device Number 6, PCIE SLOT1 */
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

	/* Initialize Port descriptor (PCIe port, Lanes ?, PCI Device Number 8, ...) */
	{
		DESCRIPTOR_TERMINATE_LIST, /* Descriptor flags  !!!IMPORTANT!!! Terminate last element of array */
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 0, 3),
		PCIE_PORT_DATA_INITIALIZER (PortEnabled, ChannelTypeExt6db, 8, HotplugDisabled, PcieGenMaxSupported, PcieGenMaxSupported, AspmDisabled, 0)
	},
};

PCIe_DDI_DESCRIPTOR DdiList [] = {
	/* DP0 to HDMI0/DP */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 24, 27),
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeHDMI, Aux1, Hdp1)
	},
	/* DP1 to FCH */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 28, 31),
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeNutmegDpToVga, Aux2, Hdp2)
	},
	/* DP2 to HDMI1/DP */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 32, 35),
		/* PCIE_DDI_DATA_INITIALIZER (ConnectorTypeEDP, Aux3, Hdp3) */
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeHDMI, Aux3, Hdp3)
	},
	/* GFX Lane 15-12 */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PcieUnusedEngine, 12, 15),
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeHDMI, Aux4, Hdp4)
	},
	/* GFX Lane 11-8 */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PcieUnusedEngine, 16, 19),
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeHDMI, Aux5, Hdp5)
	},
	/* GFX Lane 7-4 */
	{
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER (PcieUnusedEngine, 20, 23),
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeHDMI, Aux6, Hdp6)
	}
};

PCIe_COMPLEX_DESCRIPTOR Trinity = {
        DESCRIPTOR_TERMINATE_LIST,
        0,
        &PortList[0],
        &DdiList[0]
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
	AGESA_STATUS         Status;
	VOID                 *TrinityPcieComplexListPtr;
	VOID                 *TrinityPciePortPtr;
	VOID                 *TrinityPcieDdiPtr;

	ALLOCATE_HEAP_PARAMS AllocHeapParams;

	/* GNB PCIe topology Porting */

	/*  */
	/* Allocate buffer for PCIe_COMPLEX_DESCRIPTOR , PCIe_PORT_DESCRIPTOR and PCIe_DDI_DESCRIPTOR */
	/*  */
	AllocHeapParams.RequestedBufferSize = (sizeof (PCIe_COMPLEX_DESCRIPTOR)  +
					       sizeof (PCIe_PORT_DESCRIPTOR) * 7 +
					       sizeof (PCIe_DDI_DESCRIPTOR)) * 6;

	AllocHeapParams.BufferHandle = AMD_MEM_MISC_HANDLES_START;
	AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
	Status = HeapAllocateBuffer (&AllocHeapParams, &InitEarly->StdHeader);
	if ( Status!= AGESA_SUCCESS) {
		/* Could not allocate buffer for PCIe_COMPLEX_DESCRIPTOR , PCIe_PORT_DESCRIPTOR and PCIe_DDI_DESCRIPTOR */
		ASSERT(FALSE);
		return;
	}

	TrinityPcieComplexListPtr  =  (PCIe_COMPLEX_DESCRIPTOR *) AllocHeapParams.BufferPtr;

	AllocHeapParams.BufferPtr += sizeof (PCIe_COMPLEX_DESCRIPTOR);
	TrinityPciePortPtr         =  (PCIe_PORT_DESCRIPTOR *)AllocHeapParams.BufferPtr;

	AllocHeapParams.BufferPtr += sizeof (PCIe_PORT_DESCRIPTOR) * 7;
	TrinityPcieDdiPtr          =  (PCIe_DDI_DESCRIPTOR *) AllocHeapParams.BufferPtr;

	LibAmdMemFill (TrinityPcieComplexListPtr,
		       0,
		       sizeof (PCIe_COMPLEX_DESCRIPTOR),
		       &InitEarly->StdHeader);

	LibAmdMemFill (TrinityPciePortPtr,
		       0,
		       sizeof (PCIe_PORT_DESCRIPTOR) * 7,
		       &InitEarly->StdHeader);

	LibAmdMemFill (TrinityPcieDdiPtr,
		       0,
		       sizeof (PCIe_DDI_DESCRIPTOR) * 6,
		       &InitEarly->StdHeader);

	LibAmdMemCopy  (TrinityPcieComplexListPtr, &Trinity, sizeof (PCIe_COMPLEX_DESCRIPTOR), &InitEarly->StdHeader);
	LibAmdMemCopy  (TrinityPciePortPtr, &PortList[0], sizeof (PCIe_PORT_DESCRIPTOR) * 7, &InitEarly->StdHeader);
	LibAmdMemCopy  (TrinityPcieDdiPtr, &DdiList[0], sizeof (PCIe_DDI_DESCRIPTOR) * 6, &InitEarly->StdHeader);

	((PCIe_COMPLEX_DESCRIPTOR*)TrinityPcieComplexListPtr)->PciePortList =  (PCIe_PORT_DESCRIPTOR*)TrinityPciePortPtr;
	((PCIe_COMPLEX_DESCRIPTOR*)TrinityPcieComplexListPtr)->DdiLinkList  =  (PCIe_DDI_DESCRIPTOR*)TrinityPcieDdiPtr;

	InitEarly->GnbConfig.PcieComplexList = TrinityPcieComplexListPtr;
}
