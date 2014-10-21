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
#include "FchPlatform.h"
#include "Fch.h"
#include <cpu/amd/agesa/s3_resume.h>
#include <arch/io.h>
#include <device/device.h>
#include "hudson.h"

#define FILECODE UNASSIGNED_FILE_FILECODE

/* ACPI table pointers returned by AmdInitLate */
VOID *DmiTable = NULL;
VOID *AcpiPstate = NULL;
VOID *AcpiSrat = NULL;
VOID *AcpiSlit = NULL;

VOID *AcpiWheaMce = NULL;
VOID *AcpiWheaCmc = NULL;
VOID *AcpiAlib = NULL;
VOID *AcpiIvrs = NULL;

AGESA_STATUS agesawrapper_amdinitcpuio(void)
{
	UINT64 MsrReg;
	UINT32 PciData;
	PCI_ADDR PciAddress;
	AMD_CONFIG_PARAMS StdHeader;

	/* Enable legacy video routing: D18F1xF4 VGA Enable */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0xF4);
	PciData = 1;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	/* The platform BIOS needs to ensure the memory ranges of Hudson legacy
	 * devices (TPM, HPET, BIOS RAM, Watchdog Timer, I/O APIC and ACPI) are
	 * set to non-posted regions.
	 */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0x84);
	PciData = 0x00FEDF00;	/* last address before processor local APIC at FEE00000 */
	PciData |= 1 << 7;	/* set NP (non-posted) bit */
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0x80);
	PciData = (0xFED00000 >> 8) | 3;	/* lowest NP address is HPET at FED00000 */
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	/* Map the remaining PCI hole as posted MMIO */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0x8C);
	PciData = 0x00FECF00;	/* last address before non-posted range */
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	LibAmdMsrRead(0xC001001A, &MsrReg, &StdHeader);
	MsrReg = (MsrReg >> 8) | 3;
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0x88);
	PciData = (UINT32) MsrReg;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	/* Send all IO (0000-FFFF) to southbridge. */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0xC4);
	PciData = 0x0000F000;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0xC0);
	PciData = 0x00000003;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	return AGESA_SUCCESS;
}

AGESA_STATUS agesawrapper_amdinitmmio(void)
{
	UINT64 MsrReg;
	AMD_CONFIG_PARAMS StdHeader;

	/*
	   Set the MMIO Configuration Base Address and Bus Range onto MMIO configuration base
	   Address MSR register.
	 */
	MsrReg = CONFIG_MMCONF_BASE_ADDRESS | (LibAmdBitScanReverse(CONFIG_MMCONF_BUS_NUMBER) << 2) | 1;
	LibAmdMsrWrite(0xC0010058, &MsrReg, &StdHeader);

	/*
	   Set the NB_CFG MSR register. Enable CF8 extended configuration cycles.
	 */
	LibAmdMsrRead(0xC001001F, &MsrReg, &StdHeader);
	MsrReg = MsrReg | 0x0000400000000000;
	LibAmdMsrWrite(0xC001001F, &MsrReg, &StdHeader);

	/* Set ROM cache onto WP to decrease post time */
	MsrReg = (0x0100000000ull - CACHE_ROM_SIZE) | 5ull;
	LibAmdMsrWrite(0x20C, &MsrReg, &StdHeader);
	MsrReg = ((1ULL << CONFIG_CPU_ADDR_BITS) - CACHE_ROM_SIZE) | 0x800ull;
	LibAmdMsrWrite(0x20D, &MsrReg, &StdHeader);

	return AGESA_SUCCESS;
}

AGESA_STATUS agesawrapper_amdinitreset(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_RESET_PARAMS AmdResetParams;

	LibAmdMemFill(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS), &(AmdParamStruct.StdHeader));

	LibAmdMemFill(&AmdResetParams, 0, sizeof(AMD_RESET_PARAMS), &(AmdResetParams.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_RESET;
	AmdParamStruct.AllocationMethod = ByHost;
	AmdParamStruct.NewStructSize = sizeof(AMD_RESET_PARAMS);
	AmdParamStruct.NewStructPtr = &AmdResetParams;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct(&AmdParamStruct);
	AmdResetParams.HtConfig.Depth = 0;

	status = AmdInitReset((AMD_RESET_PARAMS *) AmdParamStruct.NewStructPtr);
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);
	AmdReleaseStruct(&AmdParamStruct);
	return status;
}

AGESA_STATUS agesawrapper_amdinitearly(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_EARLY_PARAMS *AmdEarlyParamsPtr;

	LibAmdMemFill(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS), &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_EARLY;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct(&AmdParamStruct);

	AmdEarlyParamsPtr = (AMD_EARLY_PARAMS *) AmdParamStruct.NewStructPtr;
	OemCustomizeInitEarly(AmdEarlyParamsPtr);

	status = AmdInitEarly((AMD_EARLY_PARAMS *) AmdParamStruct.NewStructPtr);
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);
	AmdReleaseStruct(&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdinitpost(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_POST_PARAMS *PostParams;

	LibAmdMemFill(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS), &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_POST;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	AmdCreateStruct(&AmdParamStruct);
	PostParams = (AMD_POST_PARAMS *) AmdParamStruct.NewStructPtr;
	status = AmdInitPost(PostParams);
	AGESA_EVENTLOG(status, &PostParams->StdHeader);
	AmdReleaseStruct(&AmdParamStruct);
	/* Initialize heap space */
	EmptyHeap();

	return status;
}

AGESA_STATUS agesawrapper_amdinitenv(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_ENV_PARAMS *EnvParam;

	LibAmdMemFill(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS), &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_ENV;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	status = AmdCreateStruct(&AmdParamStruct);
	EnvParam = (AMD_ENV_PARAMS *) AmdParamStruct.NewStructPtr;

	status = AmdInitEnv(EnvParam);
	AGESA_EVENTLOG(status, &EnvParam->StdHeader);
	/* Initialize Subordinate Bus Number and Secondary Bus Number
	 * In platform BIOS this address is allocated by PCI enumeration code
	 Modify D1F0x18
	 */

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
	case PICK_IVRS:
		return AcpiIvrs;
	default:
		return NULL;
	}
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

	AmdCreateStruct(&AmdParamStruct);

	((AMD_MID_PARAMS *) AmdParamStruct.NewStructPtr)->GnbMidConfiguration.iGpuVgaMode = 0;	/* 0 iGpuVgaAdapter, 1 iGpuVgaNonAdapter; */
	status = AmdInitMid((AMD_MID_PARAMS *) AmdParamStruct.NewStructPtr);
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);
	AmdReleaseStruct(&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdinitlate(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_LATE_PARAMS *AmdLateParams;

	LibAmdMemFill(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS), &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_LATE;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.HeapStatus = HEAP_SYSTEM_MEM;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	/* NOTE: if not call amdcreatestruct, the initializer(AmdInitLateInitializer) would not be called */
	AmdCreateStruct(&AmdParamStruct);
	AmdLateParams = (AMD_LATE_PARAMS *) AmdParamStruct.NewStructPtr;
	status = AmdInitLate(AmdLateParams);
	AGESA_EVENTLOG(status, &AmdLateParams->StdHeader);
	ASSERT(status == AGESA_SUCCESS);

	DmiTable = AmdLateParams->DmiTable;
	AcpiPstate = AmdLateParams->AcpiPState;
	AcpiSrat = AmdLateParams->AcpiSrat;
	AcpiSlit = AmdLateParams->AcpiSlit;

	AcpiWheaMce = AmdLateParams->AcpiWheaMce;
	AcpiWheaCmc = AmdLateParams->AcpiWheaCmc;
	AcpiAlib = AmdLateParams->AcpiAlib;
	AcpiIvrs = AmdLateParams->AcpiIvrs;

	printk(BIOS_DEBUG, "DmiTable:%x, AcpiPstatein: %x, AcpiSrat:%x,"
	       "AcpiSlit:%x, Mce:%x, Cmc:%x,"
	       "Alib:%x, AcpiIvrs:%x in %s\n",
	       (unsigned int)DmiTable, (unsigned int)AcpiPstate, (unsigned int)AcpiSrat,
	       (unsigned int)AcpiSlit, (unsigned int)AcpiWheaMce, (unsigned int)AcpiWheaCmc,
	       (unsigned int)AcpiAlib, (unsigned int)AcpiIvrs, __func__);

	/* AmdReleaseStruct (&AmdParamStruct); */
	return status;
}

AGESA_STATUS agesawrapper_amdlaterunaptask(UINT32 Func, UINT32 Data, VOID * ConfigPtr)
{
	AGESA_STATUS status;
	AP_EXE_PARAMS ApExeParams;

	LibAmdMemFill(&ApExeParams, 0, sizeof(AP_EXE_PARAMS), &(ApExeParams.StdHeader));

	ApExeParams.StdHeader.AltImageBasePtr = 0;
	ApExeParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	ApExeParams.StdHeader.Func = 0;
	ApExeParams.StdHeader.ImageBasePtr = 0;
	ApExeParams.FunctionNumber = Func;
	ApExeParams.RelatedDataBlock = ConfigPtr;

	status = AmdLateRunApTask(&ApExeParams);
	ASSERT(status == AGESA_SUCCESS);

	return status;
}
