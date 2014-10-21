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

#include <stdint.h>
#include <string.h>
#include <cpu/x86/mtrr.h>
#include <northbridge/amd/agesa/agesawrapper.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
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
#include <cpuFamilyTranslation.h>	/* CPU_SPECIFIC_SERVICES */

#define FILECODE UNASSIGNED_FILE_FILECODE

/* ACPI table pointers returned by AmdInitLate */
VOID *DmiTable = NULL;
VOID *AcpiPstate = NULL;
VOID *AcpiSrat = NULL;
VOID *AcpiSlit = NULL;

VOID *AcpiWheaMce = NULL;
VOID *AcpiWheaCmc = NULL;
VOID *AcpiAlib = NULL;

/* TODO: Function body should be in mainboard directory. */
void OemCustomizeInitEarly(AMD_EARLY_PARAMS *InitEarly)
{
}

void OemCustomizeInitPost(AMD_POST_PARAMS *InitPost)
{
#if IS_ENABLED(CONFIG_BOARD_AMD_DINAR)
	InitPost->MemConfig.UmaMode = UMA_AUTO;
	InitPost->MemConfig.BottomIo = 0xE0;
	InitPost->MemConfig.UmaSize = 0xE0-0xC0;
#endif
}

#if !IS_ENABLED(CONFIG_BOARD_AMD_DINAR)
AGESA_STATUS agesawrapper_amdinitcpuio(void)
{
	UINT32 PciData;
	PCI_ADDR PciAddress;
	AMD_CONFIG_PARAMS StdHeader;
	UINT32 nodes;
	UINT32 node;
	UINT32 sblink;
	UINT32 i;
	UINT32 TOM;

	/* get the number of coherent nodes in the system */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB, FUNC_0, 0x60);
	LibAmdPciRead(AccessWidth32, PciAddress, &PciData, &StdHeader);
	nodes = ((PciData >> 4) & 7) + 1;	//NodeCnt[2:0]

	/* Find out the Link ID of Node0 that connects to the
	 * Southbridge (system IO hub). e.g. family10 MCM Processor,
	 * sbLink is Processor0 Link2, internal Node0 Link3
	 */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB, FUNC_0, 0x64);
	LibAmdPciRead(AccessWidth32, PciAddress, &PciData, &StdHeader);
	sblink = (PciData >> 8) & 3;	//assume ganged

	/* Enable MMIO on AMD CPU Address Map Controller for all nodes */
	for (node = 0; node < nodes; node++) {
		/* clear all MMIO Mapped Base/Limit Registers */
		for (i = 0; i < 8; i++) {
			PciData = 0x00000000;
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x80 + i * 8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x84 + i * 8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		}

		/* clear all IO Space Base/Limit Registers */
		for (i = 0; i < 4; i++) {
			PciData = 0x00000000;
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xC4 + i * 8);
			LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
			PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xC0 + i * 8);
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
		TOM = (UINT32) MsrRead(TOP_MEM);
		PciData = (TOM >> 8) | 0x03;
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0x88);
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

		/* Set MMCONF space to Node0 sbLink with NP set.
		 * default E0000000-EFFFFFFF
		 * Just have all mmio set to non-posted,
		 * coreboot not implemente the range by range setting yet.
		 */
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xBC);
		PciData = CONFIG_MMCONF_BASE_ADDRESS + (CONFIG_MMCONF_BUS_NUMBER * 0x100000) - 1;	//1MB each bus
		PciData = (PciData >> 8) & 0xFFFFFF00;
		PciData |= 0x80;	//NP
		PciData |= sblink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xB8);
		PciData = (PCIE_BASE_ADDRESS >> 8) | 0x03;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

		/* Set PCIO: 0x0 - 0xFFF000 to Node0 sbLink  and enabled VGA IO */
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xC4);
		PciData = 0x00FFF000;
		PciData |= sblink << 4;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
		PciAddress.AddressValue = MAKE_SBDFO(0, 0, CONFIG_CDB + node, FUNC_1, 0xC0);
		PciData = 0x00000033;
		LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	}

	return AGESA_SUCCESS;
}

AGESA_STATUS agesawrapper_amdinitmmio(void)
{
	UINT64 MsrReg;
	AMD_CONFIG_PARAMS StdHeader;

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
	MsrReg = (0x0100000000 - CACHE_ROM_SIZE) | 5;
	LibAmdMsrWrite(0x20C, &MsrReg, &StdHeader);
	MsrReg = ((1ULL << CONFIG_CPU_ADDR_BITS) - CACHE_ROM_SIZE) | 0x800ull;
	LibAmdMsrWrite(0x20D, &MsrReg, &StdHeader);

	return AGESA_SUCCESS;
}
#endif

AGESA_STATUS agesawrapper_amdinitreset(void)
{
	AGESA_STATUS status = AGESA_SUCCESS;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_RESET_PARAMS AmdResetParams;

	LibAmdMemFill(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS), &(AmdParamStruct.StdHeader));

	LibAmdMemFill(&AmdResetParams, 0, sizeof(AMD_RESET_PARAMS), &(AmdResetParams.StdHeader));

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
		return status;
	}
	AmdResetParams.HtConfig.Depth = 0;

	//MARG34PI disabled AGESA_ENTRY_INIT_RESET by default
	//but we need to call AmdCreateStruct to call HeapManagerInit, or the event log not work
#if (defined AGESA_ENTRY_INIT_RESET) && (AGESA_ENTRY_INIT_RESET == TRUE)
	status = AmdInitReset((AMD_RESET_PARAMS *) AmdParamStruct.NewStructPtr);
#endif
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);
	AmdReleaseStruct(&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdinitearly(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_EARLY_PARAMS *AmdEarlyParamsPtr;
	UINT32 TscRateInMhz;
	CPU_SPECIFIC_SERVICES *FamilySpecificServices;

	LibAmdMemFill(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS), &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_EARLY;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	status = AmdCreateStruct(&AmdParamStruct);
	if (status != AGESA_SUCCESS) {
		return status;
	}

	AmdEarlyParamsPtr = (AMD_EARLY_PARAMS *) AmdParamStruct.NewStructPtr;

	/* OEM Should Customize the defaults through this hook */
	OemCustomizeInitEarly(AmdEarlyParamsPtr);

	status = AmdInitEarly(AmdEarlyParamsPtr);
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);

	GetCpuServicesOfCurrentCore((CONST CPU_SPECIFIC_SERVICES **) & FamilySpecificServices,
				    &AmdParamStruct.StdHeader);
	FamilySpecificServices->GetTscRate(FamilySpecificServices, &TscRateInMhz, &AmdParamStruct.StdHeader);
	printk(BIOS_DEBUG, "BSP Frequency: %uMHz\n", (unsigned int)TscRateInMhz);

	AmdReleaseStruct(&AmdParamStruct);
	return status;
}

AGESA_STATUS agesawrapper_amdinitpost(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_POST_PARAMS *PostParams;
	UINT32 TscRateInMhz;
	CPU_SPECIFIC_SERVICES *FamilySpecificServices;

	LibAmdMemFill(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS), &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_POST;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	status = AmdCreateStruct(&AmdParamStruct);
	if (status != AGESA_SUCCESS) {
		return status;
	}
	PostParams = (AMD_POST_PARAMS *) AmdParamStruct.NewStructPtr;

	/* OEM Should Customize the defaults through this hook */
	OemCustomizeInitPost(PostParams);

	status = AmdInitPost(PostParams);
	AGESA_EVENTLOG(status, &PostParams->StdHeader);

	AmdReleaseStruct(&AmdParamStruct);

	/* Initialize heap space */
	EmptyHeap();

	GetCpuServicesOfCurrentCore((CONST CPU_SPECIFIC_SERVICES **) & FamilySpecificServices,
				    &AmdParamStruct.StdHeader);
	FamilySpecificServices->GetTscRate(FamilySpecificServices, &TscRateInMhz, &AmdParamStruct.StdHeader);
	printk(BIOS_DEBUG, "BSP Frequency: %uMHz\n", (unsigned int)TscRateInMhz);

	return status;
}

AGESA_STATUS agesawrapper_amdinitenv(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_ENV_PARAMS *EnvParams;

	LibAmdMemFill(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS), &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_ENV;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	status = AmdCreateStruct(&AmdParamStruct);
	if (status != AGESA_SUCCESS) {
		return status;
	}
	EnvParams = (AMD_ENV_PARAMS *) AmdParamStruct.NewStructPtr;
	status = AmdInitEnv(EnvParams);
	AGESA_EVENTLOG(status, &EnvParams->StdHeader);

	AmdReleaseStruct(&AmdParamStruct);
	return status;
}

VOID *agesawrapper_getlateinitptr(int pick)
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

AGESA_STATUS agesawrapper_amdinitmid(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	LibAmdMemFill(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS), &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_MID;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	status = AmdCreateStruct(&AmdParamStruct);
	if (status != AGESA_SUCCESS) {
		return status;
	}
	status = AmdInitMid((AMD_MID_PARAMS *) AmdParamStruct.NewStructPtr);
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);
	AmdReleaseStruct(&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdinitlate(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_LATE_PARAMS *AmdLateParamsPtr;

	LibAmdMemFill(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS), &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_LATE;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	AmdCreateStruct(&AmdParamStruct);
	AmdLateParamsPtr = (AMD_LATE_PARAMS *) AmdParamStruct.NewStructPtr;

	printk(BIOS_DEBUG, "agesawrapper_amdinitlate: AmdLateParamsPtr = %X\n", (u32) AmdLateParamsPtr);

	status = AmdInitLate(AmdLateParamsPtr);
	AGESA_EVENTLOG(status, &AmdLateParamsPtr->StdHeader);
	ASSERT(status == AGESA_SUCCESS);

	DmiTable = AmdLateParamsPtr->DmiTable;
	AcpiPstate = AmdLateParamsPtr->AcpiPState;
	AcpiSrat = AmdLateParamsPtr->AcpiSrat;
	AcpiSlit = AmdLateParamsPtr->AcpiSlit;
	AcpiWheaMce = AmdLateParamsPtr->AcpiWheaMce;
	AcpiWheaCmc = AmdLateParamsPtr->AcpiWheaCmc;
	AcpiAlib = AmdLateParamsPtr->AcpiAlib;

	printk(BIOS_DEBUG, "In %s, AGESA generated ACPI tables:\n"
	       "   DmiTable:%p\n   AcpiPstate: %p\n   AcpiSrat:%p\n   AcpiSlit:%p\n"
	       "   Mce:%p\n   Cmc:%p\n   Alib:%p\n",
	       __func__, DmiTable, AcpiPstate, AcpiSrat, AcpiSlit, AcpiWheaMce, AcpiWheaCmc, AcpiAlib);

	/* Don't release the structure until coreboot has copied the ACPI tables.
	 * AmdReleaseStruct (&AmdLateParams);
	 */

	return status;
}

/**
 * @param[in] UINTN ApicIdOfCore,
 * @param[in] AP_EXE_PARAMS *LaunchApParams
 */
AGESA_STATUS agesawrapper_amdlaterunaptask(UINT32 Func, UINT32 Data, VOID * ConfigPtr)
{
	AGESA_STATUS status;
	AMD_LATE_PARAMS AmdLateParams;

	LibAmdMemFill(&AmdLateParams, 0, sizeof(AMD_LATE_PARAMS), &(AmdLateParams.StdHeader));

	AmdLateParams.StdHeader.AltImageBasePtr = 0;
	AmdLateParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdLateParams.StdHeader.Func = 0;
	AmdLateParams.StdHeader.ImageBasePtr = 0;
	AmdLateParams.StdHeader.HeapStatus = HEAP_TEMP_MEM;

	printk(BIOS_DEBUG, "AmdLateRunApTask on Core: %x\n", (uint32_t) Data);
	status = AmdLateRunApTask((AP_EXE_PARAMS *) ConfigPtr);
	AGESA_EVENTLOG(status, &AmdLateParams.StdHeader);
	ASSERT((status == AGESA_SUCCESS) || (status == AGESA_UNSUPPORTED));

	return status;
}
