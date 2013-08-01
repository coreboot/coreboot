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
	{
		0, //Descriptor flags  !!!IMPORTANT!!! Terminate last element of array
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 3, 3),
		PCIE_PORT_DATA_INITIALIZER_V2 (PortEnabled, ChannelTypeExt6db, 2, 5,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x01, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lanes 1, PCI Device Number 2, ...) */
	{
		0, //Descriptor flags  !!!IMPORTANT!!! Terminate last element of array
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 2, 2),
		PCIE_PORT_DATA_INITIALIZER_V2 (PortEnabled, ChannelTypeExt6db, 2, 4,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x02, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lanes 2, PCI Device Number 2, ...) */
	{
		0, //Descriptor flags  !!!IMPORTANT!!! Terminate last element of array
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 1, 1),
		PCIE_PORT_DATA_INITIALIZER_V2 (PortEnabled, ChannelTypeExt6db, 2, 3,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x03, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lanes 3, PCI Device Number 2, ...) */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 0, 0),
		PCIE_PORT_DATA_INITIALIZER_V2 (PortEnabled, ChannelTypeExt6db, 2, 2,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x04, 0)
	},
	/* Initialize Port descriptor (PCIe port, Lanes 4-7, PCI Device Number 4, ...) */
	{
		DESCRIPTOR_TERMINATE_LIST, //Descriptor flags  !!!IMPORTANT!!! Terminate last element of array
		PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 4, 7),
		PCIE_PORT_DATA_INITIALIZER_V2 (PortEnabled, ChannelTypeExt6db, 2, 1,
				HotplugDisabled,
				PcieGenMaxSupported,
				PcieGenMaxSupported,
				AspmDisabled, 0x05, 0)
	}
};

PCIe_DDI_DESCRIPTOR DdiList [] = {
	/* DP0 to HDMI0/DP */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 8, 11),
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDP, Aux1, Hdp1)
	},
	/* DP1 to FCH */
	{
		0,
		PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 12, 15),
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDP, Aux2, Hdp2)
	},
	/* DP2 to HDMI1/DP */
	{
		DESCRIPTOR_TERMINATE_LIST,
		PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 16, 19),
		PCIE_DDI_DATA_INITIALIZER (ConnectorTypeCrt, Aux3, Hdp3)
	},
};

PCIe_COMPLEX_DESCRIPTOR Kabini = {
        DESCRIPTOR_TERMINATE_LIST,
        0,
        PortList,
        DdiList
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
VOID
OemCustomizeInitEarly (
	IN  OUT AMD_EARLY_PARAMS    *InitEarly
	)
{
	AGESA_STATUS         Status;
	VOID                 *KabiniPcieComplexListPtr;
	VOID                 *KabiniPciePortPtr;
	VOID                 *KabiniPcieDdiPtr;

	ALLOCATE_HEAP_PARAMS AllocHeapParams;

	/* GNB PCIe topology Porting */

	/*  */
	/* Allocate buffer for PCIe_COMPLEX_DESCRIPTOR , PCIe_PORT_DESCRIPTOR and PCIe_DDI_DESCRIPTOR */
	/*  */
	AllocHeapParams.RequestedBufferSize = sizeof(Kabini) + sizeof(PortList) + sizeof(DdiList);

	AllocHeapParams.BufferHandle = AMD_MEM_MISC_HANDLES_START;
	AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
	Status = HeapAllocateBuffer (&AllocHeapParams, &InitEarly->StdHeader);
	if ( Status!= AGESA_SUCCESS) {
		/* Could not allocate buffer for PCIe_COMPLEX_DESCRIPTOR , PCIe_PORT_DESCRIPTOR and PCIe_DDI_DESCRIPTOR */
		ASSERT(FALSE);
		return;
	}

	KabiniPcieComplexListPtr  =  (PCIe_COMPLEX_DESCRIPTOR *) AllocHeapParams.BufferPtr;

	AllocHeapParams.BufferPtr += sizeof(Kabini);
	KabiniPciePortPtr         =  (PCIe_PORT_DESCRIPTOR *)AllocHeapParams.BufferPtr;

	AllocHeapParams.BufferPtr += sizeof(PortList);
	KabiniPcieDdiPtr          =  (PCIe_DDI_DESCRIPTOR *) AllocHeapParams.BufferPtr;

	LibAmdMemFill (KabiniPcieComplexListPtr,
		       0,
		       sizeof (Kabini),
		       &InitEarly->StdHeader);

	LibAmdMemFill (KabiniPciePortPtr,
		       0,
		       sizeof (PortList),
		       &InitEarly->StdHeader);

	LibAmdMemFill (KabiniPcieDdiPtr,
		       0,
		       sizeof (DdiList),
		       &InitEarly->StdHeader);

	LibAmdMemCopy  (KabiniPcieComplexListPtr, &Kabini, sizeof (Kabini), &InitEarly->StdHeader);
	LibAmdMemCopy  (KabiniPciePortPtr, &PortList[0], sizeof (PortList), &InitEarly->StdHeader);
	LibAmdMemCopy  (KabiniPcieDdiPtr, &DdiList[0], sizeof (DdiList), &InitEarly->StdHeader);

	((PCIe_COMPLEX_DESCRIPTOR*)KabiniPcieComplexListPtr)->PciePortList =  (PCIe_PORT_DESCRIPTOR*)KabiniPciePortPtr;
	((PCIe_COMPLEX_DESCRIPTOR*)KabiniPcieComplexListPtr)->DdiLinkList  =  (PCIe_DDI_DESCRIPTOR*)KabiniPcieDdiPtr;

	InitEarly->GnbConfig.PcieComplexList = KabiniPcieComplexListPtr;
}
