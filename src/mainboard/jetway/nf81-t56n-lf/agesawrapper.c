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

/*-----------------------------------------------------------------------------
 *						M O D U L E S		U S E D
 *-----------------------------------------------------------------------------
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
#include "heapManager.h"
#include "amdlib.h"
#include "PlatformGnbPcieComplex.h"
#include "Filecode.h"
#include <arch/io.h>
#include <cpu/amd/agesa/s3_resume.h>
#include <cbmem.h>
#include <arch/acpi.h>

#define FILECODE UNASSIGNED_FILE_FILECODE

/*------------------------------------------------------------------------------
 *					D E F I N I T I O N S		A N D		M A C R O S
 *------------------------------------------------------------------------------
 */

#define MMCONF_ENABLE 1

/* ACPI table pointers returned by AmdInitLate */
VOID *DmiTable		= NULL;
VOID *AcpiPstate	= NULL;
VOID *AcpiSrat		= NULL;
VOID *AcpiSlit		= NULL;

VOID *AcpiWheaMce	= NULL;
VOID *AcpiWheaCmc	= NULL;
VOID *AcpiAlib		= NULL;

/*------------------------------------------------------------------------------
 *				T Y P E D E F S		 A N D		 S T R U C T U R E S
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 *	P R O T O T Y P E S		 O F		 L O C A L		 F U N C T I O N S
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 *						E X P O R T E D		F U N C T I O N S
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 *						L O C A L		F U N C T I O N S
 *------------------------------------------------------------------------------
 */
UINT32
agesawrapper_amdinitcpuio (
	VOID
	)
{
	AGESA_STATUS		Status;
	UINT64				MsrReg;
	UINT32				PciData;
	PCI_ADDR			PciAddress;
	AMD_CONFIG_PARAMS	StdHeader;

	/* Enable legacy video routing: D18F1xF4 VGA Enable */
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0xF4);
	PciData = 1;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	/* The platform BIOS needs to ensure the memory ranges of SB800 legacy
	 * devices (TPM, HPET, BIOS RAM, Watchdog Timer, I/O APIC and ACPI) are
	 * set to non-posted regions.
	 */
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0x84);
	PciData = 0x00FEDF00; // last address before processor local APIC at FEE00000
	PciData |= 1 << 7;		// set NP (non-posted) bit
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0x80);
	PciData = (0xFED00000 >> 8) | 3; // lowest NP address is HPET at FED00000
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	/* Map the remaining PCI hole as posted MMIO */
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0x8C);
	PciData = 0x00FECF00; // last address before non-posted range
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	LibAmdMsrRead (0xC001001A, &MsrReg, &StdHeader);
	MsrReg = (MsrReg >> 8) | 3;
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0x88);
	PciData = (UINT32)MsrReg;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	/* Send all IO (0000-FFFF) to southbridge. */
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0xC4);
	PciData = 0x0000F000;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0xC0);
	PciData = 0x00000003;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	Status = AGESA_SUCCESS;
	return (UINT32)Status;
}

UINT32
agesawrapper_amdinitmmio (
	VOID
	)
{
	AGESA_STATUS		Status;
	UINT64				MsrReg;
	UINT32				PciData;
	PCI_ADDR			PciAddress;
	AMD_CONFIG_PARAMS	StdHeader;

	UINT8				BusRangeVal = 0;
	UINT8				BusNum;
	UINT8				Index;

	/*
	 Set the MMIO Configuration Base Address and Bus Range onto MMIO configuration base
	 Address MSR register.
	*/

	for (Index = 0; Index < 8; Index++) {
		BusNum = CONFIG_MMCONF_BUS_NUMBER >> Index;
		if (BusNum == 1) {
			BusRangeVal = Index;
			break;
		}
	}

	MsrReg = (CONFIG_MMCONF_BASE_ADDRESS | (UINT64)(BusRangeVal << 2) | MMCONF_ENABLE);
	LibAmdMsrWrite (0xC0010058, &MsrReg, &StdHeader);

	/*
	 Set the NB_CFG MSR register. Enable CF8 extended configuration cycles.
	*/
	LibAmdMsrRead (0xC001001F, &MsrReg, &StdHeader);
	MsrReg = MsrReg | 0x0000400000000000ull;
	LibAmdMsrWrite (0xC001001F, &MsrReg, &StdHeader);

	/* Set Ontario Link Data */
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0xE0);
	PciData = 0x01308002;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0xE4);
	PciData = (AMD_APU_SSID<<0x10)|AMD_APU_SVID;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	Status = AGESA_SUCCESS;
	return (UINT32)Status;
}

UINT32
agesawrapper_amdinitreset (
	VOID
	)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_RESET_PARAMS AmdResetParams;

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
	return (UINT32)status;
}

UINT32
agesawrapper_amdinitearly (
	VOID
	)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_EARLY_PARAMS		 *AmdEarlyParamsPtr;

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

UINT32 GetHeapBase(
	AMD_CONFIG_PARAMS *StdHeader
	)
{
	UINT32 heap;

#if CONFIG_HAVE_ACPI_RESUME
	/* Both romstage and ramstage has this S3 detect. */
	if (acpi_get_sleep_type() == 3)
		heap = (UINT32)cbmem_find(CBMEM_ID_RESUME_SCRATCH) + (CONFIG_HIGH_SCRATCH_MEMORY_SIZE - BIOS_HEAP_SIZE); /* himem_heap_base + high_stack_size */
	else
#endif
		heap = BIOS_HEAP_START_ADDRESS; /* low mem */

	return heap;
}

UINT32
agesawrapper_amdinitpost (
	VOID
	)
{
	AGESA_STATUS status;
	UINT16					i;
	UINT32					*HeadPtr;
	AMD_INTERFACE_PARAMS	AmdParamStruct;
	BIOS_HEAP_MANAGER		*BiosManagerPtr;

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
	status = AmdInitPost ((AMD_POST_PARAMS *)AmdParamStruct.NewStructPtr);
	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog();
	AmdReleaseStruct (&AmdParamStruct);

	/* Initialize heap space */
	BiosManagerPtr = (BIOS_HEAP_MANAGER *)GetHeapBase(&AmdParamStruct.StdHeader);

	HeadPtr = (UINT32 *) ((UINT8 *) BiosManagerPtr + sizeof (BIOS_HEAP_MANAGER));
	for (i = 0; i < ((BIOS_HEAP_SIZE/4) - (sizeof (BIOS_HEAP_MANAGER)/4)); i++) {
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
	PCI_ADDR			 PciAddress;
	UINT32				 PciValue;

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
	/* Initialize Subordinate Bus Number and Secondary Bus Number
	 * In platform BIOS this address is allocated by PCI enumeration code
		 Modify D1F0x18
	 */
	PciAddress.Address.Bus = 0;
	PciAddress.Address.Device = 1;
	PciAddress.Address.Function = 0;
	PciAddress.Address.Register = 0x18;
	/* Write to D1F0x18 */
	LibAmdPciRead (AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x00010100;
	LibAmdPciWrite (AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize GMM Base Address for Legacy Bridge Mode
	*	Modify B1D5F0x18
	*/
	PciAddress.Address.Bus = 1;
	PciAddress.Address.Device = 5;
	PciAddress.Address.Function = 0;
	PciAddress.Address.Register = 0x18;

	LibAmdPciRead (AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x96000000;
	LibAmdPciWrite (AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize FB Base Address for Legacy Bridge Mode
	* Modify B1D5F0x10
	*/
	PciAddress.Address.Register = 0x10;
	LibAmdPciRead (AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x80000000;
	LibAmdPciWrite (AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize GMM Base Address for Pcie Mode
	*	Modify B0D1F0x18
	*/
	PciAddress.Address.Bus = 0;
	PciAddress.Address.Device = 1;
	PciAddress.Address.Function = 0;
	PciAddress.Address.Register = 0x18;

	LibAmdPciRead (AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x96000000;
	LibAmdPciWrite (AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize FB Base Address for Pcie Mode
	*	Modify B0D1F0x10
	*/
	PciAddress.Address.Register = 0x10;
	LibAmdPciRead (AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x80000000;
	LibAmdPciWrite (AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize MMIO Base and Limit Address
	*	Modify B0D1F0x20
	*/
	PciAddress.Address.Bus = 0;
	PciAddress.Address.Device = 1;
	PciAddress.Address.Function = 0;
	PciAddress.Address.Register = 0x20;

	LibAmdPciRead (AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x96009600;
	LibAmdPciWrite (AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);

	/* Initialize MMIO Prefetchable Memory Limit and Base
	*	Modify B0D1F0x24
	*/
	PciAddress.Address.Register = 0x24;
	LibAmdPciRead (AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
	PciValue |= 0x8FF18001;
	LibAmdPciWrite (AccessWidth32, PciAddress, &PciValue, &AmdParamStruct.StdHeader);
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

	/* Enable MMIO on AMD CPU Address Map Controller */
	agesawrapper_amdinitcpuio ();

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
agesawrapper_amdinitlate (
	VOID
	)
{
	AGESA_STATUS Status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_LATE_PARAMS * AmdLateParamsPtr;

	LibAmdMemFill (&AmdParamStruct,
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

	printk (BIOS_DEBUG, "agesawrapper_amdinitlate: AmdLateParamsPtr = %X\n", (u32)AmdLateParamsPtr);

	Status = AmdInitLate (AmdLateParamsPtr);
	if (Status != AGESA_SUCCESS) {
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

#if CONFIG_HAVE_ACPI_RESUME
UINT32
agesawrapper_amdinitresume (
  VOID
  )
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_RESUME_PARAMS     *AmdResumeParamsPtr;
	S3_DATA_TYPE            S3DataType;

	LibAmdMemFill (&AmdParamStruct,
		       0,
		       sizeof (AMD_INTERFACE_PARAMS),
		       &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_RESUME;
	AmdParamStruct.AllocationMethod = PreMemHeap;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct (&AmdParamStruct);

	AmdResumeParamsPtr = (AMD_RESUME_PARAMS *)AmdParamStruct.NewStructPtr;

	AmdResumeParamsPtr->S3DataBlock.NvStorageSize = 0;
	AmdResumeParamsPtr->S3DataBlock.VolatileStorageSize = 0;
	S3DataType = S3DataTypeNonVolatile;

	OemAgesaGetS3Info (S3DataType,
			   (u32 *) &AmdResumeParamsPtr->S3DataBlock.NvStorageSize,
			   (void **) &AmdResumeParamsPtr->S3DataBlock.NvStorage);

	status = AmdInitResume ((AMD_RESUME_PARAMS *)AmdParamStruct.NewStructPtr);

	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog();
	AmdReleaseStruct (&AmdParamStruct);

	return (UINT32)status;
}

UINT32
agesawrapper_amds3laterestore (
  VOID
  )
{
	AGESA_STATUS Status;
	AMD_INTERFACE_PARAMS    AmdInterfaceParams;
	AMD_S3LATE_PARAMS       AmdS3LateParams;
	AMD_S3LATE_PARAMS       *AmdS3LateParamsPtr;
	S3_DATA_TYPE          S3DataType;

	LibAmdMemFill (&AmdS3LateParams,
		       0,
		       sizeof (AMD_S3LATE_PARAMS),
		       &(AmdS3LateParams.StdHeader));
	AmdInterfaceParams.StdHeader.ImageBasePtr = 0;
	AmdInterfaceParams.AllocationMethod = ByHost;
	AmdInterfaceParams.AgesaFunctionName = AMD_S3LATE_RESTORE;
	AmdInterfaceParams.NewStructPtr = &AmdS3LateParams;
	AmdInterfaceParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdS3LateParamsPtr = &AmdS3LateParams;
	AmdInterfaceParams.NewStructSize = sizeof (AMD_S3LATE_PARAMS);

	AmdCreateStruct (&AmdInterfaceParams);

	AmdS3LateParamsPtr->S3DataBlock.VolatileStorageSize = 0;
	S3DataType = S3DataTypeVolatile;

	OemAgesaGetS3Info (S3DataType,
			   (u32 *) &AmdS3LateParamsPtr->S3DataBlock.VolatileStorageSize,
			   (void **) &AmdS3LateParamsPtr->S3DataBlock.VolatileStorage);

	Status = AmdS3LateRestore (AmdS3LateParamsPtr);
	if (Status != AGESA_SUCCESS) {
		agesawrapper_amdreadeventlog();
		ASSERT(Status == AGESA_SUCCESS);
	}

	return (UINT32)Status;
}

#ifndef __PRE_RAM__
UINT32
agesawrapper_amdS3Save (
	VOID
	)
{
	AGESA_STATUS Status;
	AMD_S3SAVE_PARAMS *AmdS3SaveParamsPtr;
	AMD_INTERFACE_PARAMS  AmdInterfaceParams;
	S3_DATA_TYPE          S3DataType;

	LibAmdMemFill (&AmdInterfaceParams,
		       0,
		       sizeof (AMD_INTERFACE_PARAMS),
		       &(AmdInterfaceParams.StdHeader));

	AmdInterfaceParams.StdHeader.ImageBasePtr = 0;
	AmdInterfaceParams.StdHeader.HeapStatus = HEAP_SYSTEM_MEM;
	AmdInterfaceParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdInterfaceParams.AllocationMethod = PostMemDram;
	AmdInterfaceParams.AgesaFunctionName = AMD_S3_SAVE;
	AmdInterfaceParams.StdHeader.AltImageBasePtr = 0;
	AmdInterfaceParams.StdHeader.Func = 0;
	AmdCreateStruct(&AmdInterfaceParams);

	AmdS3SaveParamsPtr = (AMD_S3SAVE_PARAMS *)AmdInterfaceParams.NewStructPtr;
	AmdS3SaveParamsPtr->StdHeader = AmdInterfaceParams.StdHeader;

	Status = AmdS3Save (AmdS3SaveParamsPtr);
	if (Status != AGESA_SUCCESS) {
		agesawrapper_amdreadeventlog();
		ASSERT(Status == AGESA_SUCCESS);
	}

	S3DataType = S3DataTypeNonVolatile;

	Status = OemAgesaSaveS3Info (
		S3DataType,
		AmdS3SaveParamsPtr->S3DataBlock.NvStorageSize,
		AmdS3SaveParamsPtr->S3DataBlock.NvStorage);

	if (AmdS3SaveParamsPtr->S3DataBlock.VolatileStorageSize != 0) {
		S3DataType = S3DataTypeVolatile;

		Status = OemAgesaSaveS3Info (
			S3DataType,
			AmdS3SaveParamsPtr->S3DataBlock.VolatileStorageSize,
			AmdS3SaveParamsPtr->S3DataBlock.VolatileStorage
			);
	}

	OemAgesaSaveMtrr();
	AmdReleaseStruct (&AmdInterfaceParams);

	return (UINT32)Status;
}
#endif	/* #ifndef __PRE_RAM__ */
#endif	/* CONFIG_HAVE_ACPI_RESUME */

UINT32
agesawrapper_amdlaterunaptask (
	UINT32 Func,
	UINT32 Data,
	VOID *ConfigPtr
	)
{
	AGESA_STATUS Status;
	AP_EXE_PARAMS ApExeParams;

	LibAmdMemFill (&ApExeParams,
					0,
					sizeof (AP_EXE_PARAMS),
					&(ApExeParams.StdHeader));

	ApExeParams.StdHeader.AltImageBasePtr = 0;
	ApExeParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	ApExeParams.StdHeader.Func = 0;
	ApExeParams.StdHeader.ImageBasePtr = 0;
	ApExeParams.FunctionNumber = Func;
	ApExeParams.RelatedDataBlock = ConfigPtr;

	Status = AmdLateRunApTask (&ApExeParams);
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
		printk(BIOS_DEBUG,"\nEventLog:	EventClass = %lx, EventInfo = %lx.\n",AmdEventParams.EventClass,AmdEventParams.EventInfo);
		printk(BIOS_DEBUG,"	Param1 = %lx, Param2 = %lx.\n",AmdEventParams.DataParam1,AmdEventParams.DataParam2);
		printk(BIOS_DEBUG,"	Param3 = %lx, Param4 = %lx.\n",AmdEventParams.DataParam3,AmdEventParams.DataParam4);
		Status = AmdReadEventLog (&AmdEventParams);
	}

	return (UINT32)Status;
}
