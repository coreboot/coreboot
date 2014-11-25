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

#define __SIMPLE_DEVICE__

#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <stdint.h>
#include <string.h>

#include <cpu/amd/agesa/s3_resume.h>
#include <northbridge/amd/agesa/agesawrapper.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>

#include "amdlib.h"
#include <vendorcode/amd/agesa/f14/Proc/CPU/heapManager.h>

#define FILECODE UNASSIGNED_FILE_FILECODE

/* ACPI table pointers returned by AmdInitLate */
VOID *DmiTable = NULL;
VOID *AcpiPstate = NULL;
VOID *AcpiSrat = NULL;
VOID *AcpiSlit = NULL;

VOID *AcpiWheaMce = NULL;
VOID *AcpiWheaCmc = NULL;
VOID *AcpiAlib = NULL;

AGESA_STATUS agesawrapper_amdinitreset(VOID)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_RESET_PARAMS AmdResetParams;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));
	memset(&AmdResetParams, 0, sizeof(AMD_RESET_PARAMS));

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

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

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

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

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
	PCI_ADDR PciAddress;
	UINT32 PciValue;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_ENV;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct(&AmdParamStruct);

	status = AmdInitEnv((AMD_ENV_PARAMS *) AmdParamStruct.NewStructPtr);
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);

	/* Initialize Subordinate Bus Number and Secondary Bus Number
	 * In platform BIOS this address is allocated by PCI enumeration code
	 Modify D1F0x18
	 */
	PciAddress.Address.Bus = 0;
	PciAddress.Address.Device = 1;
	PciAddress.Address.Function = 0;
	PciAddress.Address.Register = 0x18;
	/* Write to D1F0x18 */
	LibAmdPciRead(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x00010100;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize GMM Base Address for Legacy Bridge Mode
	 *      Modify B1D5F0x18
	 */
	PciAddress.Address.Bus = 1;
	PciAddress.Address.Device = 5;
	PciAddress.Address.Function = 0;
	PciAddress.Address.Register = 0x18;

	LibAmdPciRead(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x96000000;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize FB Base Address for Legacy Bridge Mode
	 * Modify B1D5F0x10
	 */
	PciAddress.Address.Register = 0x10;
	LibAmdPciRead(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x80000000;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize GMM Base Address for Pcie Mode
	 *      Modify B0D1F0x18
	 */
	PciAddress.Address.Bus = 0;
	PciAddress.Address.Device = 1;
	PciAddress.Address.Function = 0;
	PciAddress.Address.Register = 0x18;

	LibAmdPciRead(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x96000000;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize FB Base Address for Pcie Mode
	 *      Modify B0D1F0x10
	 */
	PciAddress.Address.Register = 0x10;
	LibAmdPciRead(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x80000000;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize MMIO Base and Limit Address
	 *      Modify B0D1F0x20
	 */
	PciAddress.Address.Bus = 0;
	PciAddress.Address.Device = 1;
	PciAddress.Address.Function = 0;
	PciAddress.Address.Register = 0x20;

	LibAmdPciRead(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x96009600;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize MMIO Prefetchable Memory Limit and Base
	 *      Modify B0D1F0x24
	 */
	PciAddress.Address.Register = 0x24;
	LibAmdPciRead(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x8FF18001;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
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

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

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
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_LATE_PARAMS *AmdLateParamsPtr;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_LATE;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	AmdCreateStruct(&AmdParamStruct);
	AmdLateParamsPtr = (AMD_LATE_PARAMS *) AmdParamStruct.NewStructPtr;

	printk(BIOS_DEBUG, "agesawrapper_amdinitlate: AmdLateParamsPtr = %X\n",
	       (u32) AmdLateParamsPtr);

	status = AmdInitLate(AmdLateParamsPtr);
	AGESA_EVENTLOG(status, &AmdParamStruct.StdHeader);
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
	       __func__, DmiTable, AcpiPstate, AcpiSrat, AcpiSlit, AcpiWheaMce, AcpiWheaCmc,
	       AcpiAlib);

	/* Don't release the structure until coreboot has copied the ACPI tables.
	 * AmdReleaseStruct (&AmdLateParams);
	 */

	return status;
}

AGESA_STATUS agesawrapper_amdlaterunaptask(UINT32 Func, UINT32 Data, VOID * ConfigPtr)
{
	AGESA_STATUS status;
	AP_EXE_PARAMS ApExeParams;

	memset(&ApExeParams, 0, sizeof(AP_EXE_PARAMS));

	ApExeParams.StdHeader.AltImageBasePtr = 0;
	ApExeParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) & GetBiosCallout;
	ApExeParams.StdHeader.Func = 0;
	ApExeParams.StdHeader.ImageBasePtr = 0;
	ApExeParams.FunctionNumber = Func;
	ApExeParams.RelatedDataBlock = ConfigPtr;

	status = AmdLateRunApTask(&ApExeParams);
	AGESA_EVENTLOG(status, &ApExeParams.StdHeader);
	ASSERT(status == AGESA_SUCCESS);

	return status;
}
