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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include <stdint.h>
#include <string.h>
#include "agesawrapper.h"
#include "BiosCallOuts.h"
#include "cpuRegisters.h"
#include "cpuCacheInit.h"
#include "cpuApicUtilities.h"
#include "cpuEarlyInit.h"
#include "cpuLateInit.h"
#include "Dispatcher.h"
#include "cpuCacheInit.h"
#include "amdlib.h"
#include "heapManager.h"
#include "Filecode.h"
#include <arch/io.h>

#define FILECODE UNASSIGNED_FILE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/* ACPI table pointers returned by AmdInitLate */
VOID *DmiTable    = NULL;
VOID *AcpiPstate  = NULL;
VOID *AcpiSrat    = NULL;
VOID *AcpiSlit    = NULL;

VOID *AcpiWheaMce = NULL;
VOID *AcpiWheaCmc = NULL;
VOID *AcpiAlib    = NULL;


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */
extern VOID OemCustomizeInitEarly(IN  OUT AMD_EARLY_PARAMS *InitEarly);
extern VOID OemCustomizeInitPost(IN  AMD_POST_PARAMS *InitPost);

/*Get the Bus Number from CONFIG_MMCONF_BUS_NUMBER, Please reference AMD BIOS BKDG docuemt about it*/
/*
BusRange: bus range identifier. Read-write. Reset: X. This specifies the number of buses in the
MMIO configuration space range. The size of the MMIO configuration space range varies with this
field as follows: the size is 1 Mbyte times the number of buses. This field is encoded as follows:
Bits    Buses  Bits    Buses
0h      1       5h      32
1h      2       6h      64
2h      4       7h      128
3h      8       8h      256
4h      16      Fh-9h   Reserved
*/
STATIC
UINT8
GetEndBusNum (
		VOID
	     )
{
	UINT64  BusNum;
	UINT8   Index;
	for (Index = 1; Index <= 8; Index ++ ) {
		BusNum = CONFIG_MMCONF_BUS_NUMBER >> Index;
		if (BusNum == 1 ) {
			break;
		}
	}
	return Index;
}

static UINT32 amdinitcpuio(VOID)
{
	AGESA_STATUS                  Status;
	UINT64                        MsrReg;
	UINT32                        PciData;
	PCI_ADDR                      PciAddress;
	AMD_CONFIG_PARAMS             StdHeader;
	UINT32                        TopMem;
	UINT32                        NodeCnt;
	UINT32                        Node;
	UINT32                        SbLink;
	UINT32                        Index;

	/* get the number of coherent nodes in the system */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 0, 0x60);
	LibAmdPciRead(AccessWidth32, PciAddress, &PciData, &StdHeader);
	NodeCnt = ((PciData >> 4) & 7) + 1; //NodeCnt[6:4]
	/* Find out the Link ID of Node0 that connects to the
	 * Southbridge (system IO hub). e.g. family10 MCM Processor,
	 * SbLink is Processor0 Link2, internal Node0 Link3
	 */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 0, 0x64);
	LibAmdPciRead(AccessWidth32, PciAddress, &PciData, &StdHeader);
	SbLink = (PciData >> 8) & 3; //assume ganged
	/* Enable MMIO on AMD CPU Address Map Controller for all nodes */
	for (Node = 0; Node < NodeCnt; Node ++) {
		/* clear all MMIO Mapped Base/Limit Registers */
		for (Index = 0; Index < 8; Index ++) {
			PciData = 0x00000000;
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18 + Node, 1, 0x80 + Index * 8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18 + Node, 1, 0x84 + Index * 8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		}
		/* clear all IO Space Base/Limit Registers */
		for (Index = 0; Index < 4; Index ++) {
			PciData = 0x00000000;
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18 + Node, 1, 0xC0 + Index * 8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18 + Node, 1, 0xC4 + Index * 8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		}

		/* Enable MMIO on AMD CPU Address Map Controller */

		/* Set VGA Ram MMIO 0000A0000-0000BFFFF to Node0 sbLink */
		PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18 + Node, 1, 0x80);
		PciData = (0xA0000 >> 8) |3;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18 + Node, 1, 0x84);
		PciData = 0xB0000 >> 8;
		PciData &= (~0xFF);
		PciData |= SbLink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

		/* Set UMA MMIO. */
		PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18 + Node, 1, 0x88);
		LibAmdMsrRead (0xC001001A, &MsrReg, &StdHeader);
		TopMem = (UINT32)MsrReg;
		MsrReg = (MsrReg >> 8) | 3;
		PciData = (UINT32)MsrReg;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18 + Node, 1, 0x8c);
		if (TopMem <= CONFIG_MMCONF_BASE_ADDRESS) {
			PciData = (CONFIG_MMCONF_BASE_ADDRESS  - 1) >> 8;
		}
		else {
			PciData = (0x100000000ull  - 1) >> 8;
		}
		PciData &= (~0xFF);
		PciData |= SbLink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

		/* Set PCIE MMIO. */
		PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18 + Node, 1, 0x90);
		PciData = (CONFIG_MMCONF_BASE_ADDRESS >> 8) |3;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18 + Node, 1, 0x94);
		PciData = (( CONFIG_MMCONF_BASE_ADDRESS + CONFIG_MMCONF_BUS_NUMBER * 4096 *256 - 1) >> 8) & (~0xFF);
		PciData &= (~0xFF);
		PciData |= MMIO_NP_BIT;
		PciData |= SbLink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

		/* Set XAPIC MMIO. 24K */
		PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18 + Node, 1, 0x98);
		PciData = (0xFEC00000 >> 8) |3;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18 + Node, 1, 0x9c);
		PciData = ((0xFEC00000 + 6 * 4096 - 1) >> 8);
		PciData &= (~0xFF);
		PciData |= MMIO_NP_BIT;
		PciData |= SbLink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

		/* Set Local APIC MMIO. 4K*4= 16K, Llano CPU are 4 cores */
		PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18 + Node, 1, 0xA0);
		PciData = (0xFEE00000 >> 8) |3;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18 + Node, 1, 0xA8);
		PciData = (0xFEE00000 + 4 * 4096 - 1) >> 8;
		PciData &= (~0xFF);
		PciData |= MMIO_NP_BIT;
		PciData |= SbLink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

		/* Set PCIO: 0x0 - 0xFFF000  and enabled VGA IO*/
		PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18 + Node, 1, 0xC0);
		PciData = 0x13;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18 + Node, 1, 0xC4);
		PciData = 0x00FFF000;
		PciData &= (~0x7F);
		PciData |= SbLink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	}
	Status = AGESA_SUCCESS;
	return (UINT32)Status;
}

UINT32
agesawrapper_amdinitmmio (
		VOID
		)
{
	AGESA_STATUS                  Status;
	UINT64                        MsrReg;
	UINT32                        PciData;
	PCI_ADDR                      PciAddress;
	AMD_CONFIG_PARAMS             StdHeader;

	/*
	   Set the MMIO Configuration Base Address and Bus Range onto MMIO configuration base
	   Address MSR register.
	   */
	MsrReg = CONFIG_MMCONF_BASE_ADDRESS | (GetEndBusNum () << 2) | 1;
	LibAmdMsrWrite (0xC0010058, &MsrReg, &StdHeader);

	/*
	   Set the NB_CFG MSR register. Enable CF8 extended configuration cycles.
	   */
	LibAmdMsrRead (0xC001001F, &MsrReg, &StdHeader);
	MsrReg = MsrReg | BIT46;
	LibAmdMsrWrite (0xC001001F, &MsrReg, &StdHeader);

	/* Set PCIE MMIO. */
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0x90);

	PciData = (CONFIG_MMCONF_BASE_ADDRESS >> 8) |3;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0x94);
	PciData = (( CONFIG_MMCONF_BASE_ADDRESS + CONFIG_MMCONF_BUS_NUMBER * 4096 *256 - 1) >> 8) | MMIO_NP_BIT;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	/* Enable memory access */
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0x04);
	LibAmdPciRead(AccessWidth8, PciAddress, &PciData, &StdHeader);

	PciData |= BIT1;
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0x04);
	LibAmdPciWrite(AccessWidth8, PciAddress, &PciData, &StdHeader);

	/* Set ROM cache onto WP to decrease post time */
	MsrReg = (0x0100000000 - CONFIG_ROM_SIZE) | 5;
	LibAmdMsrWrite (0x20E, &MsrReg, &StdHeader);
	MsrReg = ((1ULL << CONFIG_CPU_ADDR_BITS) - CONFIG_ROM_SIZE) | 0x800ull;
	LibAmdMsrWrite (0x20F, &MsrReg, &StdHeader);

	Status = AGESA_SUCCESS;
	return (UINT32)Status;
}

UINT32
agesawrapper_amdinitreset (
		VOID
		)
{
	AGESA_STATUS status;
#if (defined AGESA_ENTRY_INIT_RESET) && (AGESA_ENTRY_INIT_RESET == TRUE)
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_RESET_PARAMS AmdResetParams;
#endif

#if (defined AGESA_ENTRY_INIT_RESET) && (AGESA_ENTRY_INIT_RESET == TRUE)
	LibAmdMemFill (&AmdParamStruct,
			0,
			sizeof (AMD_INTERFACE_PARAMS),
			&(AmdParamStruct.StdHeader));


	LibAmdMemFill (&AmdResetParams,
			0,
			sizeof (AMD_RESET_PARAMS),
			&(AmdResetParams.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_RESET;
	AmdParamStruct.AllocationMethod = ByHost;
	AmdParamStruct.NewStructSize = sizeof(AMD_RESET_PARAMS);
	AmdParamStruct.NewStructPtr = &AmdResetParams;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = NULL;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct (&AmdParamStruct);
	AmdResetParams.HtConfig.Depth = 0;
	status = AmdInitReset ((AMD_RESET_PARAMS *)AmdParamStruct.NewStructPtr);
	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog();
	AmdReleaseStruct (&AmdParamStruct);
#else
	status = AGESA_SUCCESS;
#endif

	return (UINT32)status;
}

UINT32
agesawrapper_amdinitearly (
		VOID
		)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_EARLY_PARAMS     *AmdEarlyParamsPtr;

	LibAmdMemFill (&AmdParamStruct,
			0,
			sizeof (AMD_INTERFACE_PARAMS),
			&(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_EARLY;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct (&AmdParamStruct);

	AmdEarlyParamsPtr = (AMD_EARLY_PARAMS *)AmdParamStruct.NewStructPtr;
	OemCustomizeInitEarly (AmdEarlyParamsPtr);

	status = AmdInitEarly ((AMD_EARLY_PARAMS *)AmdParamStruct.NewStructPtr);
	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog();
	AmdReleaseStruct (&AmdParamStruct);

	return (UINT32)status;
}
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
VOID OemCustomizeInitEarly(IN OUT AMD_EARLY_PARAMS *InitEarly)
{
	//InitEarly->PlatformConfig.CoreLevelingMode = CORE_LEVEL_TWO;
}

VOID
OemCustomizeInitPost (
		IN  AMD_POST_PARAMS     *InitPost
		)
{
	InitPost->MemConfig.UmaMode = UMA_AUTO;
	InitPost->MemConfig.BottomIo = 0xE0;
	InitPost->MemConfig.UmaSize = 0xE0-0xC0;
}

UINT32
agesawrapper_amdinitpost (
		VOID
		)
{
	AGESA_STATUS status;
	UINT16                  i;
	UINT32          *HeadPtr;
	AMD_INTERFACE_PARAMS  AmdParamStruct;
	BIOS_HEAP_MANAGER    *BiosManagerPtr;

	LibAmdMemFill (&AmdParamStruct,
			0,
			sizeof (AMD_INTERFACE_PARAMS),
			&(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_POST;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	AmdCreateStruct (&AmdParamStruct);

	/* OEM Should Customize the defaults through this hook */
	OemCustomizeInitPost ((AMD_POST_PARAMS *)AmdParamStruct.NewStructPtr);

	status = AmdInitPost ((AMD_POST_PARAMS *)AmdParamStruct.NewStructPtr);
	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog();
	AmdReleaseStruct (&AmdParamStruct);

	/* Initialize heap space */
	BiosManagerPtr = (BIOS_HEAP_MANAGER *)BIOS_HEAP_START_ADDRESS;

	HeadPtr = (UINT32 *) ((UINT8 *) BiosManagerPtr + sizeof (BIOS_HEAP_MANAGER));
	for (i = 0; i < ((BIOS_HEAP_SIZE/4) - (sizeof (BIOS_HEAP_MANAGER)/4)); i++)
	{
		*HeadPtr = 0x00000000;
		HeadPtr++;
	}
	BiosManagerPtr->StartOfAllocatedNodes = 0;
	BiosManagerPtr->StartOfFreedNodes = 0;

	return (UINT32)status;
}

UINT32
agesawrapper_amdinitenv (
		VOID
		)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	LibAmdMemFill (&AmdParamStruct,
			0,
			sizeof (AMD_INTERFACE_PARAMS),
			&(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_ENV;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct (&AmdParamStruct);
	status = AmdInitEnv ((AMD_ENV_PARAMS *)AmdParamStruct.NewStructPtr);
	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog();
	AmdReleaseStruct (&AmdParamStruct);

	return (UINT32)status;
}

VOID *
agesawrapper_getlateinitptr (
		int pick
		)
{
	switch (pick) {
		case PICK_DMI:
			return DmiTable;

		case PICK_PSTATE:
			return AcpiPstate;

		case PICK_SRAT:
			return AcpiSrat;

		case PICK_SLIT:
			return AcpiSlit;
		case PICK_WHEA_MCE:
			return AcpiWheaMce;
		case PICK_WHEA_CMC:
			return AcpiWheaCmc;
		case PICK_ALIB:
			return AcpiAlib;
		default:
			return NULL;
	}
}

UINT32
agesawrapper_amdinitmid (
		VOID
		)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	printk(BIOS_DEBUG, "file '%s',line %d, %s()\n", __FILE__, __LINE__, __func__);
	/* Enable MMIO on AMD CPU Address Map Controller */
	amdinitcpuio ();

	LibAmdMemFill (&AmdParamStruct,
			0,
			sizeof (AMD_INTERFACE_PARAMS),
			&(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_MID;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	AmdCreateStruct (&AmdParamStruct);

	status = AmdInitMid ((AMD_MID_PARAMS *)AmdParamStruct.NewStructPtr);
	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog();
	AmdReleaseStruct (&AmdParamStruct);

	return (UINT32)status;
}

UINT32
agesawrapper_amdinitlate(VOID)
{
	AGESA_STATUS		Status;
	AMD_INTERFACE_PARAMS	AmdParamStruct;
	AMD_LATE_PARAMS		*AmdLateParamsPtr;

	LibAmdMemFill(&AmdParamStruct,
		       0,
		       sizeof (AMD_INTERFACE_PARAMS),
		       &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_LATE;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdParamStruct.StdHeader.HeapStatus = HEAP_SYSTEM_MEM;

	AmdCreateStruct (&AmdParamStruct);
	AmdLateParamsPtr = (AMD_LATE_PARAMS *) AmdParamStruct.NewStructPtr;

	printk(BIOS_DEBUG, "agesawrapper_amdinitlate: AmdLateParamsPtr = %X\n", (u32)AmdLateParamsPtr);

	Status = AmdInitLate(AmdLateParamsPtr);
	if (Status != AGESA_SUCCESS) {
		//agesawrapper_amdreadeventlog(AmdLateParamsPtr->StdHeader.HeapStatus);
		agesawrapper_amdreadeventlog();
		ASSERT(Status == AGESA_SUCCESS);
	}
	DmiTable    = AmdLateParamsPtr->DmiTable;
	AcpiPstate  = AmdLateParamsPtr->AcpiPState;
	AcpiSrat    = AmdLateParamsPtr->AcpiSrat;
	AcpiSlit    = AmdLateParamsPtr->AcpiSlit;
	AcpiWheaMce = AmdLateParamsPtr->AcpiWheaMce;
	AcpiWheaCmc = AmdLateParamsPtr->AcpiWheaCmc;
	AcpiAlib    = AmdLateParamsPtr->AcpiAlib;

	printk(BIOS_DEBUG, "In %s, AGESA generated ACPI tables:\n"
		"   DmiTable:%p\n   AcpiPstate: %p\n   AcpiSrat:%p\n   AcpiSlit:%p\n"
		"   Mce:%p\n   Cmc:%p\n   Alib:%p\n",
		 __func__, DmiTable, AcpiPstate, AcpiSrat, AcpiSlit,
		 AcpiWheaMce, AcpiWheaCmc, AcpiAlib);

	/* Don't release the structure until coreboot has copied the ACPI tables.
	 * AmdReleaseStruct (&AmdLateParams);
	 */

	return (UINT32)Status;
}

UINT32
agesawrapper_amdlaterunaptask (
		UINT32 Data,
		VOID *ConfigPtr
		)
{
	AGESA_STATUS Status;
	AP_EXE_PARAMS AmdLateParams;

	LibAmdMemFill (&AmdLateParams,
			0,
			sizeof (AP_EXE_PARAMS),
			&(AmdLateParams.StdHeader));

	AmdLateParams.StdHeader.AltImageBasePtr = 0;
	AmdLateParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdLateParams.StdHeader.Func = 0;
	AmdLateParams.StdHeader.ImageBasePtr = 0;

	Status = AmdLateRunApTask (&AmdLateParams);
	if (Status != AGESA_SUCCESS) {
		agesawrapper_amdreadeventlog();
		ASSERT(Status == AGESA_SUCCESS);
	}

	return (UINT32)Status;
}

UINT32
agesawrapper_amdreadeventlog (
		VOID
		)
{
	AGESA_STATUS Status;
	EVENT_PARAMS AmdEventParams;

	LibAmdMemFill (&AmdEventParams,
			0,
			sizeof (EVENT_PARAMS),
			&(AmdEventParams.StdHeader));

	AmdEventParams.StdHeader.AltImageBasePtr = 0;
	AmdEventParams.StdHeader.CalloutPtr = NULL;
	AmdEventParams.StdHeader.Func = 0;
	AmdEventParams.StdHeader.ImageBasePtr = 0;
	Status = AmdReadEventLog (&AmdEventParams);
	while (AmdEventParams.EventClass != 0) {
		printk(BIOS_DEBUG,"\nEventLog:  EventClass = %x, EventInfo = %x.\n",AmdEventParams.EventClass,AmdEventParams.EventInfo);
		printk(BIOS_DEBUG,"  Param1 = %x, Param2 = %x.\n",AmdEventParams.DataParam1,AmdEventParams.DataParam2);
		printk(BIOS_DEBUG,"  Param3 = %x, Param4 = %x.\n",AmdEventParams.DataParam3,AmdEventParams.DataParam4);
		Status = AmdReadEventLog (&AmdEventParams);
	}

	return (UINT32)Status;
}
