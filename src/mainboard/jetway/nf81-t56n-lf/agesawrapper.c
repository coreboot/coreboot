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

#include "agesawrapper.h"
#include "BiosCallOuts.h"
#include "PlatformGnbPcieComplex.h"

#define __SIMPLE_DEVICE__

#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <stdint.h>
#include <string.h>

#include <cpu/amd/agesa/s3_resume.h>
#include <vendorcode/amd/agesa/f14/Proc/CPU/heapManager.h>

#define FILECODE UNASSIGNED_FILE_FILECODE

#define MMCONF_ENABLE 1

/* ACPI table pointers returned by AmdInitLate */
void *DmiTable		= NULL;
void *AcpiPstate	= NULL;
void *AcpiSrat		= NULL;
void *AcpiSlit		= NULL;

void *AcpiWheaMce	= NULL;
void *AcpiWheaCmc	= NULL;
void *AcpiAlib		= NULL;

AGESA_STATUS agesawrapper_amdinitcpuio(void)
{
	pci_devfn_t dev;
	msr_t msr;
	uint32_t reg32;

	dev = PCI_DEV(0, 0x18, 1);

	/* Enable legacy video routing: D18F1xF4 VGA Enable */
	pci_io_write_config32(dev, 0xf4, 1);

	/* The platform BIOS needs to ensure the memory ranges of SB800 legacy
	 * devices (TPM, HPET, BIOS RAM, Watchdog Timer, I/O APIC and ACPI) are
	 * set to non-posted regions.
	 * Last address before processor local APIC at FEE00000
	 */
	pci_io_write_config32(dev, 0x84, 0x00fedf00 | (1 << 7));

	/* Lowest NP address is HPET at FED00000 */
	pci_io_write_config32(dev, 0x80, (0xfed00000 >> 8) | 3);

	/* Map the remaining PCI hole as posted MMIO */
	pci_io_write_config32(dev, 0x8C, 0x00fecf00);

	msr = rdmsr(0xc001001a);
	reg32 = (msr.hi << 24) | (msr.lo >> 8) | 3; /* Equivalent to msr >> 8 */
	pci_io_write_config32(dev, 0x88, reg32);

	/* Send all IO (0000-FFFF) to southbridge. */
	pci_io_write_config32(dev, 0xc4, 0x0000f000);
	pci_io_write_config32(dev, 0xc0, 0x00000003);

	return AGESA_SUCCESS;
}

AGESA_STATUS agesawrapper_amdinitmmio(void)
{
	uint64_t			MsrReg;
	uint32_t			PciData;
	PCI_ADDR			PciAddress;
	AMD_CONFIG_PARAMS	StdHeader;

	uint8_t				BusRangeVal = 0;
	uint8_t				BusNum;
	uint8_t				Index;

	/*
	 * Set the MMIO Configuration Base Address and Bus Range onto MMIO
	 * configuration base Address MSR register.
	 */

	for (Index = 0; Index < 8; Index++) {
		BusNum = CONFIG_MMCONF_BUS_NUMBER >> Index;
		if (BusNum == 1) {
			BusRangeVal = Index;
			break;
		}
	}

	MsrReg = (CONFIG_MMCONF_BASE_ADDRESS | (uint64_t)(BusRangeVal << 2) | MMCONF_ENABLE);
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

	return AGESA_SUCCESS;
}

AGESA_STATUS agesawrapper_amdinitreset(void)
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
	AmdCreateStruct (&AmdParamStruct);
	AmdResetParams.HtConfig.Depth = 0;

	status = AmdInitReset ((AMD_RESET_PARAMS *)AmdParamStruct.NewStructPtr);
	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog();
	AmdReleaseStruct (&AmdParamStruct);

	return status;
}

AGESA_STATUS agesawrapper_amdinitearly(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_EARLY_PARAMS *AmdEarlyParamsPtr;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

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

	return status;
}

AGESA_STATUS agesawrapper_amdinitpost(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS	AmdParamStruct;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

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
	EmptyHeap();

	return status;
}

AGESA_STATUS agesawrapper_amdinitenv(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	PCI_ADDR			 PciAddress;
	uint32_t				 PciValue;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

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

	return status;
}

void * agesawrapper_getlateinitptr(int pick)
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

AGESA_STATUS agesawrapper_amdinitmid(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;

	/* Enable MMIO on AMD CPU Address Map Controller */
	agesawrapper_amdinitcpuio ();

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

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

	return status;
}

AGESA_STATUS agesawrapper_amdinitlate(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_LATE_PARAMS * AmdLateParamsPtr;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_LATE;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	AmdCreateStruct (&AmdParamStruct);
	AmdLateParamsPtr = (AMD_LATE_PARAMS *) AmdParamStruct.NewStructPtr;

	printk (BIOS_DEBUG, "agesawrapper_amdinitlate: AmdLateParamsPtr = %X\n", (u32)AmdLateParamsPtr);

	status = AmdInitLate (AmdLateParamsPtr);
	if (status != AGESA_SUCCESS) {
		agesawrapper_amdreadeventlog();
		ASSERT(status == AGESA_SUCCESS);
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

	return status;
}

#if CONFIG_HAVE_ACPI_RESUME
AGESA_STATUS agesawrapper_amdinitresume(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS AmdParamStruct;
	AMD_RESUME_PARAMS     *AmdResumeParamsPtr;
	S3_DATA_TYPE            S3DataType;

	memset(&AmdParamStruct, 0, sizeof(AMD_INTERFACE_PARAMS));

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

	return status;
}

AGESA_STATUS agesawrapper_amds3laterestore(void)
{
	AGESA_STATUS status;
	AMD_INTERFACE_PARAMS    AmdInterfaceParams;
	AMD_S3LATE_PARAMS       AmdS3LateParams;
	AMD_S3LATE_PARAMS       *AmdS3LateParamsPtr;
	S3_DATA_TYPE          S3DataType;

	memset(&AmdS3LateParams, 0, sizeof(AMD_S3LATE_PARAMS));

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

	status = AmdS3LateRestore (AmdS3LateParamsPtr);
	if (status != AGESA_SUCCESS) {
		agesawrapper_amdreadeventlog();
		ASSERT(status == AGESA_SUCCESS);
	}

	return status;
}

#ifndef __PRE_RAM__
AGESA_STATUS agesawrapper_amdS3Save(void)
{
	AGESA_STATUS status;
	AMD_S3SAVE_PARAMS *AmdS3SaveParamsPtr;
	AMD_INTERFACE_PARAMS  AmdInterfaceParams;
	S3_DATA_TYPE          S3DataType;

	memset(&AmdInterfaceParams, 0, sizeof(AMD_INTERFACE_PARAMS));

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

	status = AmdS3Save (AmdS3SaveParamsPtr);
	if (status != AGESA_SUCCESS) {
		agesawrapper_amdreadeventlog();
		ASSERT(status == AGESA_SUCCESS);
	}

	S3DataType = S3DataTypeNonVolatile;

	status = OemAgesaSaveS3Info (
		S3DataType,
		AmdS3SaveParamsPtr->S3DataBlock.NvStorageSize,
		AmdS3SaveParamsPtr->S3DataBlock.NvStorage);

	if (AmdS3SaveParamsPtr->S3DataBlock.VolatileStorageSize != 0) {
		S3DataType = S3DataTypeVolatile;

		status = OemAgesaSaveS3Info (
			S3DataType,
			AmdS3SaveParamsPtr->S3DataBlock.VolatileStorageSize,
			AmdS3SaveParamsPtr->S3DataBlock.VolatileStorage
			);
	}

	OemAgesaSaveMtrr();
	AmdReleaseStruct (&AmdInterfaceParams);

	return status;
}
#endif	/* #ifndef __PRE_RAM__ */
#endif	/* CONFIG_HAVE_ACPI_RESUME */

AGESA_STATUS agesawrapper_amdlaterunaptask (
	uint32_t Func,
	uint32_t Data,
	void *ConfigPtr
	)
{
	AGESA_STATUS status;
	AP_EXE_PARAMS ApExeParams;

	memset(&ApExeParams, 0, sizeof(AP_EXE_PARAMS));

	ApExeParams.StdHeader.AltImageBasePtr = 0;
	ApExeParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	ApExeParams.StdHeader.Func = 0;
	ApExeParams.StdHeader.ImageBasePtr = 0;
	ApExeParams.FunctionNumber = Func;
	ApExeParams.RelatedDataBlock = ConfigPtr;

	status = AmdLateRunApTask (&ApExeParams);
	if (status != AGESA_SUCCESS) {
		agesawrapper_amdreadeventlog();
		ASSERT(status == AGESA_SUCCESS);
	}

	return status;
}

AGESA_STATUS agesawrapper_amdreadeventlog(void)
{
	AGESA_STATUS status;
	EVENT_PARAMS AmdEventParams;

	memset(&AmdEventParams, 0, sizeof(EVENT_PARAMS));

	AmdEventParams.StdHeader.AltImageBasePtr = 0;
	AmdEventParams.StdHeader.CalloutPtr = NULL;
	AmdEventParams.StdHeader.Func = 0;
	AmdEventParams.StdHeader.ImageBasePtr = 0;
	status = AmdReadEventLog (&AmdEventParams);
	while (AmdEventParams.EventClass != 0) {
		printk(BIOS_DEBUG,"\nEventLog:	EventClass = %lx, EventInfo = %lx.\n",AmdEventParams.EventClass,AmdEventParams.EventInfo);
		printk(BIOS_DEBUG,"	Param1 = %lx, Param2 = %lx.\n",AmdEventParams.DataParam1,AmdEventParams.DataParam2);
		printk(BIOS_DEBUG,"	Param3 = %lx, Param4 = %lx.\n",AmdEventParams.DataParam3,AmdEventParams.DataParam4);
		status = AmdReadEventLog (&AmdEventParams);
	}

	return status;
}
