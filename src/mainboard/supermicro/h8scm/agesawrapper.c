/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
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
#include "Filecode.h"
#include "heapManager.h"
#include <cpuFamilyTranslation.h> /* CPU_SPECIFIC_SERVICES */

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

static UINT32 agesawrapper_amdinitcpuio(VOID)
{
	AGESA_STATUS		Status;
	UINT32			PciData;
	PCI_ADDR		PciAddress;
	AMD_CONFIG_PARAMS	StdHeader;
	UINT32			nodes;
	UINT32			node;
	UINT32			sblink;
	UINT32			i;
	UINT32			TOM;

	/* get the number of coherent nodes in the system */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB, FUNC_0, 0x60);
	LibAmdPciRead(AccessWidth32, PciAddress, &PciData, &StdHeader);
	nodes = ((PciData >> 4) & 7) + 1; //NodeCnt[2:0]

	/* Find out the Link ID of Node0 that connects to the
	 * Southbridge (system IO hub). e.g. family10 MCM Processor,
	 * sbLink is Processor0 Link2, internal Node0 Link3
	 */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB, FUNC_0, 0x64);
	LibAmdPciRead(AccessWidth32, PciAddress, &PciData, &StdHeader);
	sblink = (PciData >> 8) & 3; //assume ganged

	/* Enable MMIO on AMD CPU Address Map Controller for all nodes */
	for (node = 0; node < nodes; node++) {
		/* clear all MMIO Mapped Base/Limit Registers */
		for (i = 0; i < 8; i++) {
			PciData = 0x00000000;
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x80 + i*8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x84 + i*8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		}

		/* clear all IO Space Base/Limit Registers */
		for (i = 0; i < 4; i++) {
			PciData = 0x00000000;
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xC4 + i*8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xC0 + i*8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		}

		/* Set VGA Ram MMIO 0000A0000-0000BFFFF to Node0 sbLink */
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x84);
		PciData = 0x00000B00;
		PciData |= sblink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x80);
		PciData = 0x00000A03;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

		/* Set TOM1-FFFFFFFF to Node0 sbLink. */
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x8C);
		PciData = 0x00FFFF00;
		PciData |= sblink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		TOM = (UINT32)MsrRead(TOP_MEM);
		PciData = (TOM >> 8) | 0x03;
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x88);
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

		/* Set MMCONF space to Node0 sbLink with NP set.
		 * default E0000000-EFFFFFFF
		 * Just have all mmio set to non-posted,
		 * coreboot not implemente the range by range setting yet.
		 */
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xBC);
		PciData = CONFIG_MMCONF_BASE_ADDRESS + (CONFIG_MMCONF_BUS_NUMBER * 0x100000);//1MB each bus
		PciData = (PciData >> 8) & 0xFFFFFF00;
		PciData |= 0x80; //NP
		PciData |= sblink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xB8);
		PciData = (PCIE_BASE_ADDRESS >> 8) | 0x03;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);


		/* Set PCIO: 0x0 - 0xFFF000 to Node0 sbLink  and enabled VGA IO*/
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xC4);
		PciData = 0x00FFF000;
		PciData |= sblink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xC0);
		PciData = 0x00000033;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	}

	Status = AGESA_SUCCESS;
	return (UINT32)Status;
}

UINT32 agesawrapper_amdinitmmio(VOID)
{
	AGESA_STATUS                  Status;
	UINT64                        MsrReg;
	AMD_CONFIG_PARAMS             StdHeader;

	/*
	 * Set the MMIO Configuration Base Address and Bus Range onto
	 * MMIO configuration base Address MSR register.
	 */
	MsrReg = CONFIG_MMCONF_BASE_ADDRESS | (LibAmdBitScanReverse(CONFIG_MMCONF_BUS_NUMBER) << 2) | 1;
	LibAmdMsrWrite(0xC0010058, &MsrReg, &StdHeader);

	/*
	 * Set the NB_CFG MSR register. Enable CF8 extended configuration cycles.
	 */
	LibAmdMsrRead(0xC001001F, &MsrReg, &StdHeader);
	MsrReg = MsrReg | (1ULL << 46);
	LibAmdMsrWrite(0xC001001F, &MsrReg, &StdHeader);

	/* Set ROM cache onto WP to decrease post time */
	MsrReg = (0x0100000000 - CONFIG_ROM_SIZE) | 5;
	LibAmdMsrWrite (0x20C, &MsrReg, &StdHeader);
	MsrReg = ((1ULL << CONFIG_CPU_ADDR_BITS) - CONFIG_ROM_SIZE) | 0x800ull;
	LibAmdMsrWrite(0x20D, &MsrReg, &StdHeader);

	Status = AGESA_SUCCESS;
	return (UINT32)Status;
}

UINT32 agesawrapper_amdinitreset(VOID)
{
	AGESA_STATUS status = AGESA_SUCCESS;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_RESET_PARAMS AmdResetParams;

	LibAmdMemFill(&AmdParamStruct,
			0,
			sizeof(AMD_INTERFACE_PARAMS),
			&(AmdParamStruct.StdHeader));

	LibAmdMemFill(&AmdResetParams,
			0,
			sizeof(AMD_RESET_PARAMS),
			&(AmdResetParams.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_RESET;
	AmdParamStruct.AllocationMethod = ByHost;
	AmdParamStruct.NewStructSize = sizeof(AMD_RESET_PARAMS);
	AmdParamStruct.NewStructPtr = &AmdResetParams;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = NULL;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	status = AmdCreateStruct(&AmdParamStruct);
	if (status != AGESA_SUCCESS) {
		return (UINT32)status;
	}
	AmdResetParams.HtConfig.Depth = 0;

	//MARG34PI disabled AGESA_ENTRY_INIT_RESET by default
	//but we need to call AmdCreateStruct to call HeapManagerInit, or the event log not work
#if (defined AGESA_ENTRY_INIT_RESET) && (AGESA_ENTRY_INIT_RESET == TRUE)
	status = AmdInitReset((AMD_RESET_PARAMS *)AmdParamStruct.NewStructPtr);
#endif
	if (status != AGESA_SUCCESS)
		agesawrapper_amdreadeventlog(AmdParamStruct.StdHeader.HeapStatus);
	AmdReleaseStruct(&AmdParamStruct);

	return (UINT32)status;
}

UINT32 agesawrapper_amdinitearly(VOID)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_EARLY_PARAMS     *AmdEarlyParamsPtr;
	UINT32 TscRateInMhz;
	CPU_SPECIFIC_SERVICES *FamilySpecificServices;

	LibAmdMemFill(&AmdParamStruct,
			0,
			sizeof(AMD_INTERFACE_PARAMS),
			&(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_EARLY;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	status = AmdCreateStruct(&AmdParamStruct);
	if (status != AGESA_SUCCESS) {
		return (UINT32)status;
	}

	AmdEarlyParamsPtr = (AMD_EARLY_PARAMS *)AmdParamStruct.NewStructPtr;
	OemCustomizeInitEarly(AmdEarlyParamsPtr);

	status = AmdInitEarly(AmdEarlyParamsPtr);
	if (status != AGESA_SUCCESS)
		agesawrapper_amdreadeventlog(AmdParamStruct.StdHeader.HeapStatus);

	GetCpuServicesOfCurrentCore((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, &AmdParamStruct.StdHeader);
	FamilySpecificServices->GetTscRate(FamilySpecificServices, &TscRateInMhz, &AmdParamStruct.StdHeader);
	printk(BIOS_DEBUG, "BSP Frequency: %uMHz\n", (unsigned int)TscRateInMhz);

	AmdReleaseStruct(&AmdParamStruct);
	return (UINT32)status;
}

UINT32 agesawrapper_amdinitpost(VOID)
{
	AGESA_STATUS status;
	UINT16 i;
	UINT32 *HeadPtr;
	AMD_INTERFACE_PARAMS  AmdParamStruct;
	AMD_POST_PARAMS *PostParams;
	BIOS_HEAP_MANAGER    *BiosManagerPtr;
	UINT32 TscRateInMhz;
	CPU_SPECIFIC_SERVICES *FamilySpecificServices;

	LibAmdMemFill(&AmdParamStruct,
			0,
			sizeof(AMD_INTERFACE_PARAMS),
			&(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_POST;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	status = AmdCreateStruct(&AmdParamStruct);
	if (status != AGESA_SUCCESS) {
		return (UINT32)status;
	}
	PostParams = (AMD_POST_PARAMS *)AmdParamStruct.NewStructPtr;
	status = AmdInitPost(PostParams);
	if (status != AGESA_SUCCESS) {
		agesawrapper_amdreadeventlog(PostParams->StdHeader.HeapStatus);
	}
	AmdReleaseStruct(&AmdParamStruct);

	/* Initialize heap space */
	BiosManagerPtr = (BIOS_HEAP_MANAGER *)BIOS_HEAP_START_ADDRESS;

	HeadPtr = (UINT32 *) ((UINT8 *) BiosManagerPtr + sizeof(BIOS_HEAP_MANAGER));
	for (i = 0; i < ((BIOS_HEAP_SIZE/4) - (sizeof(BIOS_HEAP_MANAGER)/4)); i++) {
		*HeadPtr = 0x00000000;
		HeadPtr++;
	}
	BiosManagerPtr->StartOfAllocatedNodes = 0;
	BiosManagerPtr->StartOfFreedNodes = 0;

	GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, &AmdParamStruct.StdHeader);
	FamilySpecificServices->GetTscRate (FamilySpecificServices, &TscRateInMhz, &AmdParamStruct.StdHeader);
	printk(BIOS_DEBUG, "BSP Frequency: %uMHz\n", (unsigned int)TscRateInMhz);

	return (UINT32)status;
}

UINT32 agesawrapper_amdinitenv(VOID)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_ENV_PARAMS *EnvParams;

	LibAmdMemFill(&AmdParamStruct,
			0,
			sizeof(AMD_INTERFACE_PARAMS),
			&(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_ENV;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	status = AmdCreateStruct(&AmdParamStruct);
	if (status != AGESA_SUCCESS) {
		return (UINT32)status;
	}
	EnvParams = (AMD_ENV_PARAMS *)AmdParamStruct.NewStructPtr;
	status = AmdInitEnv(EnvParams);
	if (status != AGESA_SUCCESS)
		agesawrapper_amdreadeventlog(EnvParams->StdHeader.HeapStatus);

	AmdReleaseStruct(&AmdParamStruct);
	return (UINT32)status;
}

VOID * agesawrapper_getlateinitptr(int pick)
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

	return NULL;
}

UINT32 agesawrapper_amdinitmid(VOID)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	/* Enable MMIO on AMD CPU Address Map Controller */
	agesawrapper_amdinitcpuio();

	LibAmdMemFill(&AmdParamStruct,
			0,
			sizeof(AMD_INTERFACE_PARAMS),
			&(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_MID;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	status = AmdCreateStruct(&AmdParamStruct);
	if (status != AGESA_SUCCESS) {
		return (UINT32)status;
	}
	status = AmdInitMid((AMD_MID_PARAMS *)AmdParamStruct.NewStructPtr);
	if (status != AGESA_SUCCESS)
		agesawrapper_amdreadeventlog(AmdParamStruct.StdHeader.HeapStatus);
	AmdReleaseStruct(&AmdParamStruct);

	return (UINT32)status;
}

UINT32 agesawrapper_amdinitlate(VOID)
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

	AmdCreateStruct (&AmdParamStruct);
	AmdLateParamsPtr = (AMD_LATE_PARAMS *) AmdParamStruct.NewStructPtr;

	printk(BIOS_DEBUG, "agesawrapper_amdinitlate: AmdLateParamsPtr = %X\n", (u32)AmdLateParamsPtr);

	Status = AmdInitLate(AmdLateParamsPtr);
	if (Status != AGESA_SUCCESS) {
		agesawrapper_amdreadeventlog(AmdLateParamsPtr->StdHeader.HeapStatus);
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

/**
 * @param[in] UINTN ApicIdOfCore,
 * @param[in] AP_EXE_PARAMS *LaunchApParams
 */
UINT32 agesawrapper_amdlaterunaptask(UINT32 Data, VOID *ConfigPtr)
{
	AGESA_STATUS Status;
	AMD_LATE_PARAMS AmdLateParams;

	LibAmdMemFill(&AmdLateParams,
			0,
			sizeof(AMD_LATE_PARAMS),
			&(AmdLateParams.StdHeader));

	AmdLateParams.StdHeader.AltImageBasePtr = 0;
	AmdLateParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdLateParams.StdHeader.Func = 0;
	AmdLateParams.StdHeader.ImageBasePtr = 0;
	AmdLateParams.StdHeader.HeapStatus = HEAP_TEMP_MEM;

	printk(BIOS_DEBUG, "AmdLateRunApTask on Core: %x\n", (uint32_t)Data);
	Status = AmdLateRunApTask((AP_EXE_PARAMS *)ConfigPtr);
	if (Status != AGESA_SUCCESS) {
		agesawrapper_amdreadeventlog(AmdLateParams.StdHeader.HeapStatus);
		ASSERT(Status <= AGESA_UNSUPPORTED);
	}

	return (UINT32)Status;
}

/**
 *
 */
static void agesa_bound_check(EVENT_PARAMS *event)
{
	switch (event->EventInfo) {
		case CPU_ERROR_HEAP_IS_FULL:
			printk(BIOS_DEBUG, "Heap allocation for specified buffer handle failed as heap is full\n");
			break;

		case CPU_ERROR_HEAP_BUFFER_HANDLE_IS_ALREADY_USED:
			printk(BIOS_DEBUG, "Allocation incomplete as buffer has previously been allocated\n");
			break;

		case CPU_ERROR_HEAP_BUFFER_HANDLE_IS_NOT_PRESENT:
			printk(BIOS_DEBUG, "Unable to locate buffer handle or deallocate heap as buffer handle cannot be located\n");
			break;

		case CPU_ERROR_HEAP_BUFFER_IS_NOT_PRESENT:
			printk(BIOS_DEBUG, "Unable to locate pointer to the heap buffer\n");
			break;

		default:
			break;
	}
}

/**
 *
 */
static void agesa_alert(EVENT_PARAMS *event)
{
	switch (event->EventInfo) {
		case MEM_ALERT_USER_TMG_MODE_OVERRULED:
			printk(BIOS_DEBUG, "Socket %x Dct %x Channel %x "
					"TIMING_MODE_SPECIFIC is requested but can not be applied to current configurations.\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case MEM_ALERT_ORG_MISMATCH_DIMM:
			printk(BIOS_DEBUG, "Socket %x Dct %x Channel %x "
					"DIMM organization miss-match\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case MEM_ALERT_BK_INT_DIS:
			printk(BIOS_DEBUG, "Socket %x Dct %x Channel %x "
					"Bank interleaving disable for internal issue\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case CPU_EVENT_BIST_ERROR:
			printk(BIOS_DEBUG, "BIST error: %x reported on Socket %x Core %x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case HT_EVENT_HW_SYNCFLOOD:
			printk(BIOS_DEBUG, "HT_EVENT_DATA_HW_SYNCFLOOD error on Socket %x Link %x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2);
			break;

		case HT_EVENT_HW_HTCRC:
			printk(BIOS_DEBUG, "HT_EVENT_HW_HTCRC error on Socket %x Link %x Lanemask:%x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		default:
			break;
	}
}

/**
 *
 */
static void agesa_warning(EVENT_PARAMS *event)
{
/*
	if (event->EventInfo == CPU_EVENT_STACK_REENTRY) {
		printk(BIOS_DEBUG,
				"The stack has already been enabled and this is a
				redundant invocation of AMD_ENABLE_STACK. There is no event logged and
				no data values. The event sub-class is returned along with the status code\n");
		return;
	}
*/

	switch (event->EventInfo >> 24) {
		case 0x04:
			printk(BIOS_DEBUG, "Memory: Socket %x Dct %x Channel%x ",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case 0x08:
			printk(BIOS_DEBUG, "Processor: ");
			break;

		case 0x10:
			printk(BIOS_DEBUG, "Hyper Transport: ");
			break;

		default:
			break;
	}

	switch (event->EventInfo) {
		case MEM_WARNING_UNSUPPORTED_QRDIMM:
			printk(BIOS_DEBUG, "QR DIMMs detected but not supported\n");
			break;

		case MEM_WARNING_UNSUPPORTED_UDIMM:
			printk(BIOS_DEBUG, "Unbuffered DIMMs detected but not supported\n");
			break;

		case MEM_WARNING_UNSUPPORTED_SODIMM:
			printk(BIOS_DEBUG, "SO-DIMMs detected but not supported");
			break;

		case MEM_WARNING_UNSUPPORTED_X4DIMM:
			printk(BIOS_DEBUG, "x4 DIMMs detected but not supported");
			break;

		case MEM_WARNING_UNSUPPORTED_RDIMM:
			printk(BIOS_DEBUG, "Registered DIMMs detected but not supported");
			break;

/*
		case MEM_WARNING_UNSUPPORTED_LRDIMM:
			printk(BIOS_DEBUG, "Load Reduced DIMMs detected but not supported");
			break;
*/

		case MEM_WARNING_NO_SPDTRC_FOUND:
			printk(BIOS_DEBUG, "NO_SPDTRC_FOUND");
			break;

		case MEM_WARNING_EMP_NOT_SUPPORTED:
			printk(BIOS_DEBUG, "Processor is not capable for EMP");//
			break;

		case MEM_WARNING_EMP_CONFLICT:
			printk(BIOS_DEBUG, "EMP cannot be enabled if channel interleaving bank interleaving, or bank swizzle is enabled\n");//
			break;

		case MEM_WARNING_EMP_NOT_ENABLED:
			printk(BIOS_DEBUG, "Memory size is not power of two\n");//
			break;

		case MEM_WARNING_PERFORMANCE_ENABLED_BATTERY_LIFE_PREFERRED:
			printk(BIOS_DEBUG, "MEM_WARNING_PERFORMANCE_ENABLED_BATTERY_LIFE_PREFERRED\n");
			break;

		case MEM_WARNING_NODE_INTERLEAVING_NOT_ENABLED:
			printk(BIOS_DEBUG, "MEM_WARNING_NODE_INTERLEAVING_NOT_ENABLED\n");
			break;

		case MEM_WARNING_CHANNEL_INTERLEAVING_NOT_ENABLED:
			printk(BIOS_DEBUG, "MEM_WARNING_CHANNEL_INTERLEAVING_NOT_ENABLED\n");
			break;

		case MEM_WARNING_BANK_INTERLEAVING_NOT_ENABLED:
			printk(BIOS_DEBUG, "MEM_WARNING_BANK_INTERLEAVING_NOT_ENABLED\n");
			break;

		case MEM_WARNING_VOLTAGE_1_35_NOT_SUPPORTED:
			printk(BIOS_DEBUG, "MEM_WARNING_VOLTAGE_1_35_NOT_SUPPORTED\n");
			break;

/*
		case MEM_WARNING_INITIAL_DDR3VOLT_NONZERO:
			printk(BIOS_DEBUG, "MEM_WARNING_INITIAL_DDR3VOLT_NONZERO\n");
			break;

		case MEM_WARNING_NO_COMMONLY_SUPPORTED_VDDIO:
			printk(BIOS_DEBUG, "MEM_WARNING_NO_COMMONLY_SUPPORTED_VDDIO\n");
			break;
*/

		case CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR:
			printk(BIOS_DEBUG, "Allocation rule number that has been violated:");
			if ((event->EventInfo & 0x000000FF) == 0x01) {
				printk(BIOS_DEBUG, "AGESA_CACHE_SIZE_REDUCED\n");
			} else if ((event->EventInfo & 0x000000FF) == 0x02) {
				printk(BIOS_DEBUG, "AGESA_CACHE_REGIONS_ACROSS_1MB\n");
			} else if ((event->EventInfo & 0x000000FF) == 0x03) {
				printk(BIOS_DEBUG, "AGESA_CACHE_REGIONS_ACROSS_4GB\n");
			}
			printk(BIOS_DEBUG, "cache region index:%x, start:%x size:%x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case CPU_WARNING_ADJUSTED_LEVELING_MODE:
			printk(BIOS_DEBUG, "CPU_WARNING_ADJUSTED_LEVELING_MODE "
					"requested: %x, actual level:%x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2);
			break;

		case CPU_EVENT_PM_PSTATE_OVERCURRENT:
			printk(BIOS_DEBUG, "CPU_EVENT_PM_PSTATE_OVERCURRENT "
						"Socket: %x, Pstate:%x\n",
						(unsigned int)event->DataParam1,
						(unsigned int)event->DataParam2);
			break;

		case CPU_WARNING_NONOPTIMAL_HT_ASSIST_CFG:
			printk(BIOS_DEBUG, "CPU_WARNING_NONOPTIMAL_HT_ASSIST_CFG\n");
			break;

/*
		case CPU_EVENT_UNKNOWN_PROCESSOR_REVISION:
			printk(BIOS_DEBUG, "CPU_EVENT_UNKNOWN_PROCESSOR_REVISION, socket: %lx, cpuid:%lx\n",
				event->DataParam1,
				event->DataParam2);
			break;
*/

		case HT_EVENT_OPT_REQUIRED_CAP_RETRY:
			printk(BIOS_DEBUG, "HT_EVENT_OPT_REQUIRED_CAP_RETRY, Socket %x Link %x Depth %x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case HT_EVENT_OPT_REQUIRED_CAP_GEN3:
			printk(BIOS_DEBUG, "HT_EVENT_OPT_REQUIRED_CAP_GEN3, Socket %x Link %x Depth %x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case HT_EVENT_OPT_UNUSED_LINKS:
			printk(BIOS_DEBUG, "HT_EVENT_OPT_UNUSED_LINKS, SocketA%x LinkA%x SocketB%x LinkB%x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3,
					(unsigned int)event->DataParam4);
			break;

		case HT_EVENT_OPT_LINK_PAIR_EXCEED:
			printk(BIOS_DEBUG, "HT_EVENT_OPT_LINK_PAIR_EXCEED, SocketA%x MasterLink%x SocketB%x AltLink%x\n",

					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3,
					(unsigned int)event->DataParam4);
		default:
			break;
	}
}

/**
 *
 */
static void agesa_error(EVENT_PARAMS *event)
{

	switch (event->EventInfo >> 24) {
		case 0x04:
			printk(BIOS_DEBUG, "Memory: Socket %x Dct %x Channel%x ",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case 0x08:
			printk(BIOS_DEBUG, "Processor: ");
			break;

		case 0x10:
			printk(BIOS_DEBUG, "Hyper Transport: ");
			break;

		default:
			break;
	}

	switch (event->EventInfo) {
		case MEM_ERROR_NO_DQS_POS_RD_WINDOW:
			printk(BIOS_DEBUG, "No DQS Position window for RD DQS\n");
			break;

		case MEM_ERROR_SMALL_DQS_POS_RD_WINDOW:
			printk(BIOS_DEBUG, "Small DQS Position window for RD DQS\n");
			break;

		case MEM_ERROR_NO_DQS_POS_WR_WINDOW:
			printk(BIOS_DEBUG, "No DQS Position window for WR DQS\n");
			break;

		case MEM_ERROR_SMALL_DQS_POS_WR_WINDOW:
			printk(BIOS_DEBUG, "Small DQS Position window for WR DQS\n");
			break;

		case MEM_ERROR_DIMM_SPARING_NOT_ENABLED:
			printk(BIOS_DEBUG, "DIMM sparing has not been enabled for an internal issues\n");
			break;

		case MEM_ERROR_RCVR_EN_VALUE_TOO_LARGE:
			printk(BIOS_DEBUG, "Receive Enable value is too large\n");
			break;
		case MEM_ERROR_RCVR_EN_NO_PASSING_WINDOW:
			printk(BIOS_DEBUG, "There is no DQS receiver enable window\n");
			break;

		case MEM_ERROR_DRAM_ENABLED_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling DramEnabled bit\n");
			break;

		case MEM_ERROR_DCT_ACCESS_DONE_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling DctAccessDone bit\n");
			break;

		case MEM_ERROR_SEND_CTRL_WORD_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling SendCtrlWord bit\n");
			break;

		case MEM_ERROR_PREF_DRAM_TRAIN_MODE_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling PrefDramTrainMode bit\n");
			break;

		case MEM_ERROR_ENTER_SELF_REF_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling EnterSelfRef bit\n");
			break;

		case MEM_ERROR_FREQ_CHG_IN_PROG_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling FreqChgInProg bit\n");
			break;

		case MEM_ERROR_EXIT_SELF_REF_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling ExitSelfRef bit\n");
			break;

		case MEM_ERROR_SEND_MRS_CMD_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling SendMrsCmd bit\n");
			break;

		case MEM_ERROR_SEND_ZQ_CMD_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling SendZQCmd bit\n");
			break;

		case MEM_ERROR_DCT_EXTRA_ACCESS_DONE_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling DctExtraAccessDone bit\n");
			break;

		case MEM_ERROR_MEM_CLR_BUSY_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling MemClrBusy bit\n");
			break;

		case MEM_ERROR_MEM_CLEARED_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling MemCleared bit\n");
			break;

		case MEM_ERROR_FLUSH_WR_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling FlushWr bit\n");
			break;

		case MEM_ERROR_MAX_LAT_NO_WINDOW:
			printk(BIOS_DEBUG, "Fail to find pass during Max Rd Latency training\n");
			break;

		case MEM_ERROR_PARALLEL_TRAINING_LAUNCH_FAIL:
			printk(BIOS_DEBUG, "Fail to launch training code on an AP\n");
			break;

		case MEM_ERROR_PARALLEL_TRAINING_TIME_OUT:
			printk(BIOS_DEBUG, "Fail to finish parallel training\n");
			break;

		case MEM_ERROR_NO_ADDRESS_MAPPING:
			printk(BIOS_DEBUG, "No address mapping found for a dimm\n");
			break;

		case MEM_ERROR_RCVR_EN_NO_PASSING_WINDOW_EQUAL_LIMIT:
			printk(BIOS_DEBUG, "There is no DQS receiver enable window and the value is equal to the largest value\n");
			break;

		case MEM_ERROR_RCVR_EN_VALUE_TOO_LARGE_LIMIT_LESS_ONE:
			printk(BIOS_DEBUG, "Receive Enable value is too large and is 1 less than limit\n");
			break;

		case MEM_ERROR_CHECKSUM_NV_SPDCHK_RESTRT_ERROR:
			printk(BIOS_DEBUG, "SPD Checksum error for NV_SPDCHK_RESTRT\n");
			break;

		case MEM_ERROR_NO_CHIPSELECT:
			printk(BIOS_DEBUG, "No chipselects found\n");
			break;

		case MEM_ERROR_UNSUPPORTED_333MHZ_UDIMM:
			printk(BIOS_DEBUG, "Unbuffered dimm is not supported at 333MHz\n");
			break;

		case MEM_ERROR_WL_PRE_OUT_OF_RANGE:
			printk(BIOS_DEBUG, "Returned PRE value during write levelizzation was out of range\n");
			break;

		case CPU_ERROR_BRANDID_HEAP_NOT_AVAILABLE:
			printk(BIOS_DEBUG, "No heap is allocated for BrandId structure\n");
			break;

		case CPU_ERROR_MICRO_CODE_PATCH_IS_NOT_LOADED:
			printk(BIOS_DEBUG, "Unable to load micro code patch\n");
			break;

		case CPU_ERROR_PSTATE_HEAP_NOT_AVAILABLE:
			printk(BIOS_DEBUG, "No heap is allocated for the Pstate structure\n");
			break;

/*
		case CPU_ERROR_PM_NB_PSTATE_MISMATCH:
			printk(BIOS_DEBUG, "NB P-state indicated by Index was disabled due to mismatch between processors\n");
			break;
*/

		case CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR:
			printk(BIOS_DEBUG, "Allocation rule number that has been violated:");
			if ((event->EventInfo & 0x000000FF) == 0x04) {
				printk(BIOS_DEBUG, "AGESA_REGION_NOT_ALIGNED_ON_BOUNDARY\n");
			} else if ((event->EventInfo & 0x000000FF) == 0x05) {
				printk(BIOS_DEBUG, "AGESA_START_ADDRESS_LESS_D0000\n");
			} else if ((event->EventInfo & 0x000000FF) == 0x06) {
				printk(BIOS_DEBUG, "AGESA_THREE_CACHE_REGIONS_ABOVE_1MB\n");
			} else if ((event->EventInfo & 0x000000FF) == 0x07) {
				printk(BIOS_DEBUG, "AGESA_DEALLOCATE_CACHE_REGIONS\n");
			}
			printk(BIOS_DEBUG, "cache region index:%x, start:%x size:%x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case HT_EVENT_COH_NO_TOPOLOGY:
			printk(BIOS_DEBUG, "no Matching Topology was found during coherent initializatio TotalHtNodes: %x\n",
					(unsigned int)event->DataParam1);
			break;

		case HT_EVENT_NCOH_BUID_EXCEED:
			printk(BIOS_DEBUG, "there is a limit of 32 unit IDs per chain Socket%x Link%x Depth%x"
					"Current BUID: %x, Unit Count: %x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3,
					(unsigned int)event->DataParam4 >> 16,
					(unsigned int)event->DataParam4 & 0x0000FFFF);
			break;

		 case HT_EVENT_NCOH_BUS_MAX_EXCEED:
			printk(BIOS_DEBUG, "maximum auto bus limit exceeded, Socket %x Link %x Bus %x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case HT_EVENT_NCOH_CFG_MAP_EXCEED:
			printk(BIOS_DEBUG, "there is a limit of four non-coherent chains, Socket %x Link %x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2);
			break;

		case HT_EVENT_NCOH_DEVICE_FAILED:
			printk(BIOS_DEBUG, "after assigning an IO Device an ID, it does not respond at the new ID"
					"Socket %x Link %x Depth %x DeviceID %x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3,
					(unsigned int)event->DataParam4);
		default:
			break;
	}
}
/**
 *
 */
static void agesa_critical(EVENT_PARAMS *event)
{
	switch (event->EventInfo) {
		case MEM_ERROR_HEAP_ALLOCATE_FOR_DMI_TABLE_DDR3:
			printk(BIOS_DEBUG, "Socket: %x, Heap allocation error for DMI table for DDR3\n",
					(unsigned int)event->DataParam1);
			break;

		case MEM_ERROR_HEAP_ALLOCATE_FOR_DMI_TABLE_DDR2:
			printk(BIOS_DEBUG, "Socket: %x, Heap allocation error for DMI table for DDR2\n",
					(unsigned int)event->DataParam1);
			break;

		case MEM_ERROR_UNSUPPORTED_DIMM_CONFIG:
			printk(BIOS_DEBUG, "Socket: %x, Dimm population is not supported\n",
					(unsigned int)event->DataParam1);
			break;

		case HT_EVENT_COH_PROCESSOR_TYPE_MIX:
			printk(BIOS_DEBUG, "Socket %x Link %x TotalSockets %x, HT_EVENT_COH_PROCESSOR_TYPE_MIX \n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case HT_EVENT_COH_MPCAP_MISMATCH:
			printk(BIOS_DEBUG, "Socket %x Link %x MpCap %x TotalSockets %x, HT_EVENT_COH_MPCAP_MISMATCH\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3,
					(unsigned int)event->DataParam4);
		default:
			break;
	}
}

/**
 *
 */
static void agesa_fatal(EVENT_PARAMS *event)
{

	switch (event->EventInfo >> 24) {
		case 0x04:
			printk(BIOS_DEBUG, "Memory: Socket %x Dct %x Channel%x ",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case 0x08:
			printk(BIOS_DEBUG, "Processor: ");
			break;

		case 0x10:
			printk(BIOS_DEBUG, "Hyper Transport: ");
			break;

		default:
			break;
	}

	switch (event->EventInfo) {
		case MEM_ERROR_MINIMUM_MODE:
			printk(BIOS_DEBUG, "Running in minimum mode\n");
			break;

		case MEM_ERROR_MODULE_TYPE_MISMATCH_DIMM:
			printk(BIOS_DEBUG, "DIMM modules are missmatched\n");
			break;

		case MEM_ERROR_NO_DIMM_FOUND_ON_SYSTEM:
			printk(BIOS_DEBUG, "No DIMMs have been foun\n");
			break;

		case MEM_ERROR_MISMATCH_DIMM_CLOCKS:
			printk(BIOS_DEBUG, "DIMM clocks miss-matched\n");
			break;

		case MEM_ERROR_NO_CYC_TIME:
			printk(BIOS_DEBUG, "No cycle time found\n");
			break;
		case MEM_ERROR_HEAP_ALLOCATE_DYN_STORING_OF_TRAINED_TIMINGS:
			printk(BIOS_DEBUG, "Heap allocation error with dynamic storing of trained timings\n");
			break;

		case MEM_ERROR_HEAP_ALLOCATE_FOR_DCT_STRUCT_AND_CH_DEF_STRUCTs:
			printk(BIOS_DEBUG, "Heap allocation error for DCT_STRUCT and CH_DEF_STRUCT\n");
			break;

		case MEM_ERROR_HEAP_ALLOCATE_FOR_REMOTE_TRAINING_ENV:
			printk(BIOS_DEBUG, "Heap allocation error with REMOTE_TRAINING_ENV\n");
			break;

		case MEM_ERROR_HEAP_ALLOCATE_FOR_SPD:
			printk(BIOS_DEBUG, "Heap allocation error for SPD data\n");
			break;

		case MEM_ERROR_HEAP_ALLOCATE_FOR_RECEIVED_DATA:
			printk(BIOS_DEBUG, "Heap allocation error for RECEIVED_DATA during parallel training\n");
			break;

		case MEM_ERROR_HEAP_ALLOCATE_FOR_S3_SPECIAL_CASE_REGISTERS:
			printk(BIOS_DEBUG, "Heap allocation error for S3 \"SPECIAL_CASE_REGISTER\"\n");
			break;

		case MEM_ERROR_HEAP_ALLOCATE_FOR_TRAINING_DATA:
			printk(BIOS_DEBUG, "Heap allocation error for Training Data\n");
			break;

		case MEM_ERROR_HEAP_ALLOCATE_FOR_IDENTIFY_DIMM_MEM_NB_BLOCK:
			printk(BIOS_DEBUG, "Heap allocation error for  DIMM Identify \"MEM_NB_BLOCK\"\n");
			break;

		case MEM_ERROR_NO_CONSTRUCTOR_FOR_IDENTIFY_DIMM:
			printk(BIOS_DEBUG, "No Constructor for DIMM Identify\n");
			break;

		case MEM_ERROR_VDDIO_UNSUPPORTED:
			printk(BIOS_DEBUG, "VDDIO of the dimms on the board is not supported\n");
			break;

		case CPU_EVENT_PM_ALL_PSTATE_OVERCURRENT:
			printk(BIOS_DEBUG, "Socket: %x, All PStates exceeded the motherboard current limit on specified socket\n",
				(unsigned int)event->DataParam1);
			break;

		default:
			break;
	}
}

/**
 *
 * Interprte the agesa event log to an user readable string
 */
static void interpret_agesa_eventlog(EVENT_PARAMS *event)
{
	switch (event->EventClass) {
		case AGESA_BOUNDS_CHK:
			agesa_bound_check(event);
			break;

		case AGESA_ALERT:
			agesa_alert(event);
			break;

		case AGESA_WARNING:
			agesa_warning(event);
			break;

		case AGESA_ERROR:
			agesa_error(event);
			break;

		case AGESA_CRITICAL:
			agesa_critical(event);
			break;

		case AGESA_FATAL:
			agesa_fatal(event);
			break;

		default:
			break;
	}
}

/**
 * @param  HeapStatus -the current HeapStatus
 */
UINT32 agesawrapper_amdreadeventlog(UINT8 HeapStatus)
{
	printk(BIOS_DEBUG, "enter in %s\n", __func__);
	AGESA_STATUS Status;
	EVENT_PARAMS AmdEventParams;

	LibAmdMemFill(&AmdEventParams,
			0,
			sizeof(EVENT_PARAMS),
			&(AmdEventParams.StdHeader));

	AmdEventParams.StdHeader.AltImageBasePtr = 0;
	AmdEventParams.StdHeader.CalloutPtr = NULL;
	AmdEventParams.StdHeader.Func = 0;
	AmdEventParams.StdHeader.ImageBasePtr = 0;
	/* I have to know the current HeapStatus to Locate the EventLogHeapPointer */
	AmdEventParams.StdHeader.HeapStatus = HeapStatus;
	Status = AmdReadEventLog(&AmdEventParams);
	while (AmdEventParams.EventClass != 0) {
		printk(BIOS_DEBUG,"\nEventLog:  EventClass = %x, EventInfo = %x.\n",
				(unsigned int)AmdEventParams.EventClass,
				(unsigned int)AmdEventParams.EventInfo);
		printk(BIOS_DEBUG,"  Param1 = %x, Param2 = %x.\n",
				(unsigned int)AmdEventParams.DataParam1,
				(unsigned int)AmdEventParams.DataParam2);
		printk(BIOS_DEBUG,"  Param3 = %x, Param4 = %x.\n",
				(unsigned int)AmdEventParams.DataParam3,
				(unsigned int)AmdEventParams.DataParam4);
		interpret_agesa_eventlog(&AmdEventParams);
		Status = AmdReadEventLog(&AmdEventParams);
	}

	printk(BIOS_DEBUG, "exit %s \n", __func__);
	return (UINT32)Status;
}

