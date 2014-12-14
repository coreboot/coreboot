/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
#include <arch/io.h>

#define FILECODE UNASSIGNED_FILE_FILECODE

/* ACPI table pointers returned by AmdInitLate */
VOID *DmiTable = NULL;
VOID *AcpiPstate = NULL;
VOID *AcpiSrat = NULL;
VOID *AcpiSlit = NULL;

VOID *AcpiWheaMce = NULL;
VOID *AcpiWheaCmc = NULL;
VOID *AcpiAlib = NULL;

AGESA_STATUS agesawrapper_amdinitcpuio(VOID)
{
	UINT64 MsrReg;
	UINT32 PciData;
	PCI_ADDR PciAddress;
	AMD_CONFIG_PARAMS StdHeader;

	/* Enable MMIO on AMD CPU Address Map Controller */

	/* Start to set MMIO 0000A0000-0000BFFFF to Node0 Link0 */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0x84);
	PciData = 0x00000B00;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0x80);
	PciData = 0x00000A03;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	/* Set TOM-DFFFFFFF to Node0 Link0. */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0x8C);
	PciData = 0x00DFFF00;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	LibAmdMsrRead(0xC001001A, &MsrReg, &StdHeader);
	MsrReg = (MsrReg >> 8) | 3;
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0x88);
	PciData = (UINT32) MsrReg;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	/* Set E0000000-FFFFFFFF to Node0 Link0 with NP set. */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0xBC);
	PciData = 0x00FFFF00 | 0x80;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0xB8);
	PciData = (CONFIG_MMCONF_BASE_ADDRESS >> 8) | 03;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	/* Start to set PCIIO 0000-FFFF to Node0 Link0 with ISA&VGA set. */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0xC4);
//-  PciData = 0x0000F000;
	PciData = 0x00FFF000;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x18, 1, 0xC0);
	PciData = 0x00000013;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	return AGESA_SUCCESS;
}

AGESA_STATUS agesawrapper_amdinitmmio(VOID)
{
	UINT64 MsrReg;
	UINT32 PciData;
	PCI_ADDR PciAddress;
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
	MsrReg = MsrReg | 0x0000400000000000ull;
	LibAmdMsrWrite(0xC001001F, &MsrReg, &StdHeader);

	/* Enable Non-Post Memory in CPU */
	PciData = CONFIG_MMCONF_BASE_ADDRESS + (CONFIG_MMCONF_BUS_NUMBER * 0x100000) - 1;
	PciData = (PciData >> 8) & ~0xff;
	PciData |= 0x80;
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x018, 0x01, 0xA4);
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	PciData = ((CONFIG_MMCONF_BASE_ADDRESS >> 8) | 0x03);
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0x018, 0x01, 0xA0);
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	/* Enable memory access */
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0, 0, 0x04);
	LibAmdPciRead(AccessWidth8, PciAddress, &PciData, &StdHeader);
	PciData |= BIT1;
	PciAddress.AddressValue = MAKE_SBDFO(0, 0, 0, 0, 0x04);
	LibAmdPciWrite(AccessWidth8, PciAddress, &PciData, &StdHeader);

	/* Set ROM cache onto WP to decrease post time */
	MsrReg = (0x0100000000ull - CACHE_ROM_SIZE) | 5ull;
	LibAmdMsrWrite(0x20C, &MsrReg, &StdHeader);
	MsrReg = ((1ULL << CONFIG_CPU_ADDR_BITS) - CACHE_ROM_SIZE) | 0x800ull;
	LibAmdMsrWrite(0x20D, &MsrReg, &StdHeader);

	return AGESA_SUCCESS;
}

AGESA_STATUS agesawrapper_amdinitreset(VOID)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_RESET_PARAMS AmdResetParams;

	LibAmdMemFill(&AmdParamStruct,
		      0, sizeof(AMD_INTERFACE_PARAMS), &(AmdParamStruct.StdHeader));

	LibAmdMemFill(&AmdResetParams, 0, sizeof(AMD_RESET_PARAMS), &(AmdResetParams.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_RESET;
	AmdParamStruct.AllocationMethod = ByHost;
	AmdParamStruct.NewStructSize = sizeof(AMD_RESET_PARAMS);
	AmdParamStruct.NewStructPtr = &AmdResetParams;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = NULL;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct(&AmdParamStruct);
	AmdResetParams.HtConfig.Depth = 0;

	status = AmdInitReset((AMD_RESET_PARAMS *) AmdParamStruct.NewStructPtr);
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);
	AmdReleaseStruct(&AmdParamStruct);
	return status;
}

AGESA_STATUS agesawrapper_amdinitearly(VOID)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_EARLY_PARAMS *AmdEarlyParamsPtr;

	LibAmdMemFill(&AmdParamStruct,
		      0, sizeof(AMD_INTERFACE_PARAMS), &(AmdParamStruct.StdHeader));

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

AGESA_STATUS agesawrapper_amdinitpost(VOID)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	LibAmdMemFill(&AmdParamStruct,
		      0, sizeof(AMD_INTERFACE_PARAMS), &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_POST;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	AmdCreateStruct(&AmdParamStruct);
	status = AmdInitPost((AMD_POST_PARAMS *) AmdParamStruct.NewStructPtr);
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);
	AmdReleaseStruct(&AmdParamStruct);

	/* Initialize heap space */
	EmptyHeap();

	return status;
}

AGESA_STATUS agesawrapper_amdinitenv(VOID)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	LibAmdMemFill(&AmdParamStruct,
		      0, sizeof(AMD_INTERFACE_PARAMS), &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_ENV;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct(&AmdParamStruct);
	status = AmdInitEnv((AMD_ENV_PARAMS *) AmdParamStruct.NewStructPtr);
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);
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
}

AGESA_STATUS agesawrapper_amdinitmid(VOID)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	LibAmdMemFill(&AmdParamStruct,
		      0, sizeof(AMD_INTERFACE_PARAMS), &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_MID;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	AmdCreateStruct(&AmdParamStruct);

	status = AmdInitMid((AMD_MID_PARAMS *) AmdParamStruct.NewStructPtr);
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);
	AmdReleaseStruct(&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdinitlate(VOID)
{
	AGESA_STATUS status;
	AMD_LATE_PARAMS AmdLateParams;

	LibAmdMemFill(&AmdLateParams, 0, sizeof(AMD_LATE_PARAMS), &(AmdLateParams.StdHeader));

	AmdLateParams.StdHeader.AltImageBasePtr = 0;
	AmdLateParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdLateParams.StdHeader.Func = 0;
	AmdLateParams.StdHeader.ImageBasePtr = 0;

	status = AmdInitLate(&AmdLateParams);
	AGESA_EVENTLOG(status, &AmdLateParams.StdHeader);
	ASSERT(status == AGESA_SUCCESS);

	DmiTable = AmdLateParams.DmiTable;
	AcpiPstate = AmdLateParams.AcpiPState;
	AcpiSrat = AmdLateParams.AcpiSrat;
	AcpiSlit = AmdLateParams.AcpiSlit;

	AcpiWheaMce = AmdLateParams.AcpiWheaMce;
	AcpiWheaCmc = AmdLateParams.AcpiWheaCmc;
	AcpiAlib = AmdLateParams.AcpiAlib;

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

	status = AmdLateRunApTask(&ApExeParams);
	AGESA_EVENTLOG(status, &ApExeParams.StdHeader);
	ASSERT(status == AGESA_SUCCESS);

	return status;
}
