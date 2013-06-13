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
#include "PlatformGnbPcieComplex.h"
#include "Filecode.h"
#include "heapManager.h"
#include "FchPlatform.h"
#include "Fch.h"
#include <cpu/amd/agesa/s3_resume.h>
#include <cbmem.h>
#include <arch/acpi.h>
#include <arch/io.h>

VOID FchInitS3LateRestore (IN FCH_DATA_BLOCK *FchDataPtr);
VOID FchInitS3EarlyRestore (IN FCH_DATA_BLOCK *FchDataPtr);

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
VOID *AcpiIvrs    = NULL;

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
UINT32
agesawrapper_amdinitcpuio (
	VOID
	)
{
	AGESA_STATUS                  Status;
	UINT64                        MsrReg;
	UINT32                        PciData;
	PCI_ADDR                      PciAddress;
	AMD_CONFIG_PARAMS             StdHeader;

	/* Enable legacy video routing: D18F1xF4 VGA Enable */
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0xF4);
	PciData = 1;
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	/* The platform BIOS needs to ensure the memory ranges of Hudson legacy
	 * devices (TPM, HPET, BIOS RAM, Watchdog Timer, I/O APIC and ACPI) are
	 * set to non-posted regions.
	 */
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0x84);
	PciData = 0x00FEDF00; /* last address before processor local APIC at FEE00000 */
	PciData |= 1 << 7;    /* set NP (non-posted) bit */
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0x80);
	PciData = (0xFED00000 >> 8) | 3; /* lowest NP address is HPET at FED00000 */
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	/* Map the remaining PCI hole as posted MMIO */
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, 1, 0x8C);
	PciData = 0x00FECF00; /* last address before non-posted range */
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
	AGESA_STATUS                  Status;
	UINT64                        MsrReg;
	UINT32                        PciData;
	PCI_ADDR                      PciAddress;
	AMD_CONFIG_PARAMS             StdHeader;

	/*
	  Set the MMIO Configuration Base Address and Bus Range onto MMIO configuration base
	  Address MSR register.
	*/
	MsrReg = CONFIG_MMCONF_BASE_ADDRESS | (LibAmdBitScanReverse (CONFIG_MMCONF_BUS_NUMBER) << 2) | 1;
	LibAmdMsrWrite (0xC0010058, &MsrReg, &StdHeader);

	/*
	  Set the NB_CFG MSR register. Enable CF8 extended configuration cycles.
	*/
	LibAmdMsrRead (0xC001001F, &MsrReg, &StdHeader);
	MsrReg = MsrReg | 0x0000400000000000;
	LibAmdMsrWrite (0xC001001F, &MsrReg, &StdHeader);

	/* For serial port */
	PciData = 0xFF03FFD5;
	PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x14, 0x3, 0x44);
	LibAmdPciWrite(AccessWidth32, PciAddress, &PciData, &StdHeader);

	/* Set ROM cache onto WP to decrease post time */
	MsrReg = (0x0100000000ull - CONFIG_ROM_SIZE) | 5ull;
	LibAmdMsrWrite (0x20C, &MsrReg, &StdHeader);
	MsrReg = ((1ULL << CONFIG_CPU_ADDR_BITS) - CONFIG_ROM_SIZE) | 0x800ull;
	LibAmdMsrWrite (0x20D, &MsrReg, &StdHeader);

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
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;
	AmdCreateStruct (&AmdParamStruct);
	AmdResetParams.HtConfig.Depth = 0;
#if !CONFIG_HUDSON_XHCI_ENABLE
	AmdResetParams.FchInterface.Xhci0Enable = FALSE;
#endif
	AmdResetParams.FchInterface.Xhci1Enable = FALSE;

	status = AmdInitReset ((AMD_RESET_PARAMS *)AmdParamStruct.NewStructPtr);
	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog(AmdParamStruct.StdHeader.HeapStatus);
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
	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog(AmdParamStruct.StdHeader.HeapStatus);
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
		heap = (UINT32)cbmem_find(CBMEM_ID_RESUME_SCRATCH) + (CONFIG_HIGH_SCRATCH_MEMORY_SIZE - BIOS_HEAP_SIZE); /* base + high_stack_size */
	else
#endif
		heap = BIOS_HEAP_START_ADDRESS; /* Low mem */

	return heap;
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
	AMD_POST_PARAMS       *PostParams;
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
	PostParams = (AMD_POST_PARAMS *)AmdParamStruct.NewStructPtr;
	status = AmdInitPost (PostParams);
	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog(PostParams->StdHeader.HeapStatus);
	AmdReleaseStruct (&AmdParamStruct);
	/* Initialize heap space */
	BiosManagerPtr = (BIOS_HEAP_MANAGER *)GetHeapBase(&AmdParamStruct.StdHeader);

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
	AMD_ENV_PARAMS       *EnvParam;

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
	status = AmdCreateStruct (&AmdParamStruct);
	EnvParam = (AMD_ENV_PARAMS *)AmdParamStruct.NewStructPtr;

	status = AmdInitEnv (EnvParam);
	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog(EnvParam->StdHeader.HeapStatus);
	/* Initialize Subordinate Bus Number and Secondary Bus Number
	 * In platform BIOS this address is allocated by PCI enumeration code
	 Modify D1F0x18
	*/

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
	case PICK_IVRS:
		return AcpiIvrs;
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

	((AMD_MID_PARAMS *)AmdParamStruct.NewStructPtr)->GnbMidConfiguration.iGpuVgaMode = 0;/* 0 iGpuVgaAdapter, 1 iGpuVgaNonAdapter; */
	status = AmdInitMid ((AMD_MID_PARAMS *)AmdParamStruct.NewStructPtr);
	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog(AmdParamStruct.StdHeader.HeapStatus);
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
	AMD_LATE_PARAMS *AmdLateParams;

	LibAmdMemFill (&AmdParamStruct,
		       0,
		       sizeof (AMD_INTERFACE_PARAMS),
		       &(AmdParamStruct.StdHeader));

	AmdParamStruct.AgesaFunctionName = AMD_INIT_LATE;
	AmdParamStruct.AllocationMethod = PostMemDram;
	AmdParamStruct.StdHeader.AltImageBasePtr = 0;
	AmdParamStruct.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdParamStruct.StdHeader.HeapStatus = HEAP_SYSTEM_MEM;
	AmdParamStruct.StdHeader.Func = 0;
	AmdParamStruct.StdHeader.ImageBasePtr = 0;

	/* NOTE: if not call amdcreatestruct, the initializer(AmdInitLateInitializer) would not be called */
	AmdCreateStruct(&AmdParamStruct);
	AmdLateParams = (AMD_LATE_PARAMS *)AmdParamStruct.NewStructPtr;
	Status = AmdInitLate(AmdLateParams);
	/* CDIT table is not created. */
	if (Status != AGESA_SUCCESS) {
		agesawrapper_amdreadeventlog(AmdLateParams->StdHeader.HeapStatus);
		ASSERT(Status == AGESA_SUCCESS);
	}

	DmiTable    = AmdLateParams->DmiTable;
	AcpiPstate  = AmdLateParams->AcpiPState;
	AcpiSrat    = AmdLateParams->AcpiSrat;
	AcpiSlit    = AmdLateParams->AcpiSlit;

	AcpiWheaMce = AmdLateParams->AcpiWheaMce;
	AcpiWheaCmc = AmdLateParams->AcpiWheaCmc;
	AcpiAlib    = AmdLateParams->AcpiAlib;
	AcpiIvrs    = AmdLateParams->AcpiIvrs;

	printk(BIOS_DEBUG, "DmiTable:%x, AcpiPstatein: %x, AcpiSrat:%x,"
	       "AcpiSlit:%x, Mce:%x, Cmc:%x,"
	       "Alib:%x, AcpiIvrs:%x in %s\n",
	       (unsigned int)DmiTable, (unsigned int)AcpiPstate, (unsigned int)AcpiSrat,
	       (unsigned int)AcpiSlit, (unsigned int)AcpiWheaMce, (unsigned int)AcpiWheaCmc,
	       (unsigned int)AcpiAlib, (unsigned int)AcpiIvrs, __func__);

	/* AmdReleaseStruct (&AmdParamStruct); */
	return (UINT32)Status;
}

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
		/* agesawrapper_amdreadeventlog(); */
		ASSERT(Status == AGESA_SUCCESS);
	}

	return (UINT32)Status;
}

#if CONFIG_HAVE_ACPI_RESUME

UINT32 agesawrapper_amdinitresume(VOID)
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

	if (status != AGESA_SUCCESS) agesawrapper_amdreadeventlog(AmdParamStruct.StdHeader.HeapStatus);
	AmdReleaseStruct (&AmdParamStruct);

	return (UINT32)status;
}

#ifndef __PRE_RAM__

extern FCH_DATA_BLOCK InitEnvCfgDefault;
STATIC VOID s3_resume_init_data(FCH_DATA_BLOCK *FchParams)
{
	FchParams->Gpp.GppLinkConfig           = UserOptions.FchBldCfg->CfgFchGppLinkConfig;
	FchParams->Gpp.PortCfg[0].PortPresent  = UserOptions.FchBldCfg->CfgFchGppPort0Present;
	FchParams->Gpp.PortCfg[1].PortPresent  = UserOptions.FchBldCfg->CfgFchGppPort1Present;
	FchParams->Gpp.PortCfg[2].PortPresent  = UserOptions.FchBldCfg->CfgFchGppPort2Present;
	FchParams->Gpp.PortCfg[3].PortPresent  = UserOptions.FchBldCfg->CfgFchGppPort3Present;
	FchParams->Gpp.PortCfg[0].PortHotPlug  = UserOptions.FchBldCfg->CfgFchGppPort0HotPlug;
	FchParams->Gpp.PortCfg[1].PortHotPlug  = UserOptions.FchBldCfg->CfgFchGppPort1HotPlug;
	FchParams->Gpp.PortCfg[2].PortHotPlug  = UserOptions.FchBldCfg->CfgFchGppPort2HotPlug;
	FchParams->Gpp.PortCfg[3].PortHotPlug  = UserOptions.FchBldCfg->CfgFchGppPort3HotPlug;
	FchParams->Gpp.GppFunctionEnable = TRUE; /* GppEnable */
	FchParams->Gpp.GppPhyPllPowerDown = TRUE;
	FchParams->Gpp.GppDynamicPowerSaving = TRUE;
	FchParams->Gpp.UmiPhyPllPowerDown = TRUE;
	FchParams->Gpp.NewGppAlgorithm = TRUE;
	FchParams->Gpp.GppPortMinPollingTime = 40;

	FchParams->Spi.SpiSpeed = 2;
	FchParams->Ir.IrConfig = 3;

	FchParams->HwAcpi.Smbus0BaseAddress    = UserOptions.FchBldCfg->CfgSmbus0BaseAddress;
	FchParams->HwAcpi.Smbus1BaseAddress    = UserOptions.FchBldCfg->CfgSmbus1BaseAddress;
	FchParams->HwAcpi.SioPmeBaseAddress    = UserOptions.FchBldCfg->CfgSioPmeBaseAddress;
	FchParams->HwAcpi.AcpiPm1EvtBlkAddr    = UserOptions.FchBldCfg->CfgAcpiPm1EvtBlkAddr;
	FchParams->HwAcpi.AcpiPm1CntBlkAddr    = UserOptions.FchBldCfg->CfgAcpiPm1CntBlkAddr;
	FchParams->HwAcpi.AcpiPmTmrBlkAddr     = UserOptions.FchBldCfg->CfgAcpiPmTmrBlkAddr;
	FchParams->HwAcpi.CpuControlBlkAddr    = UserOptions.FchBldCfg->CfgCpuControlBlkAddr;
	FchParams->HwAcpi.AcpiGpe0BlkAddr      = UserOptions.FchBldCfg->CfgAcpiGpe0BlkAddr;
	FchParams->HwAcpi.SmiCmdPortAddr       = UserOptions.FchBldCfg->CfgSmiCmdPortAddr;
	FchParams->HwAcpi.AcpiPmaCntBlkAddr    = UserOptions.FchBldCfg->CfgAcpiPmaCntBlkAddr;
	FchParams->HwAcpi.WatchDogTimerBase    = UserOptions.FchBldCfg->CfgWatchDogTimerBase;
	FchParams->Sata.SataRaid5Ssid          = UserOptions.FchBldCfg->CfgSataRaid5Ssid;
	FchParams->Sata.SataRaidSsid           = UserOptions.FchBldCfg->CfgSataRaidSsid;
	FchParams->Sata.SataAhciSsid           = UserOptions.FchBldCfg->CfgSataAhciSsid;
	FchParams->Sata.SataIdeSsid            = UserOptions.FchBldCfg->CfgSataIdeSsid;
	FchParams->Spi.RomBaseAddress          = UserOptions.FchBldCfg->CfgSpiRomBaseAddress;
	FchParams->Sd.SdSsid                   = UserOptions.FchBldCfg->CfgSdSsid;
	FchParams->Spi.LpcSsid                 = UserOptions.FchBldCfg->CfgLpcSsid;
	FchParams->Hpet.HpetBase               = UserOptions.FchBldCfg->CfgHpetBaseAddress;
	FchParams->Azalia.AzaliaSsid           = UserOptions.FchBldCfg->CfgAzaliaSsid;
	FchParams->Smbus.SmbusSsid             = UserOptions.FchBldCfg->CfgSmbusSsid;
	FchParams->Ide.IdeSsid                 = UserOptions.FchBldCfg->CfgIdeSsid;
	FchParams->Usb.EhciSsid                = UserOptions.FchBldCfg->CfgEhciSsid;
	FchParams->Usb.OhciSsid                = UserOptions.FchBldCfg->CfgOhciSsid;
	FchParams->Usb.XhciSsid                = UserOptions.FchBldCfg->CfgXhciSsid;
	FchParams->Ir.IrPinControl             = UserOptions.FchBldCfg->CfgFchIrPinControl;
	FchParams->Sd.SdClockControl           = UserOptions.FchBldCfg->CfgFchSdClockControl;
}

UINT32 agesawrapper_fchs3earlyrestore (VOID)
{
	AGESA_STATUS status = AGESA_SUCCESS;

	FCH_DATA_BLOCK      FchParams;
	AMD_CONFIG_PARAMS StdHeader;

	StdHeader.HeapStatus = HEAP_SYSTEM_MEM;
	StdHeader.HeapBasePtr = GetHeapBase(&StdHeader) + 0x10;
	StdHeader.AltImageBasePtr = 0;
	StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	StdHeader.Func = 0;
	StdHeader.ImageBasePtr = 0;

	FchParams = InitEnvCfgDefault;
	FchParams.StdHeader = &StdHeader;
	s3_resume_init_data(&FchParams);

	FchInitS3EarlyRestore(&FchParams);

	return status;
}
#endif

UINT32 agesawrapper_amds3laterestore (VOID)
{
	AGESA_STATUS Status;
	AMD_INTERFACE_PARAMS    AmdInterfaceParams;
	AMD_S3LATE_PARAMS       AmdS3LateParams;
	AMD_S3LATE_PARAMS       *AmdS3LateParamsPtr;
	S3_DATA_TYPE          S3DataType;

	agesawrapper_amdinitcpuio();
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
		agesawrapper_amdreadeventlog(AmdInterfaceParams.StdHeader.HeapStatus);
		ASSERT(Status == AGESA_SUCCESS);
	}

	return (UINT32)Status;
}

#ifndef __PRE_RAM__

extern UINT8 picr_data[0x54], intr_data[0x54];

UINT32 agesawrapper_fchs3laterestore (VOID)
{
	AGESA_STATUS status = AGESA_SUCCESS;

	FCH_DATA_BLOCK      FchParams;
	AMD_CONFIG_PARAMS StdHeader;
	UINT8 byte;

	StdHeader.HeapStatus = HEAP_SYSTEM_MEM;
	StdHeader.HeapBasePtr = GetHeapBase(&StdHeader) + 0x10;
	StdHeader.AltImageBasePtr = 0;
	StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	StdHeader.Func = 0;
	StdHeader.ImageBasePtr = 0;

	FchParams = InitEnvCfgDefault;
	FchParams.StdHeader = &StdHeader;
	s3_resume_init_data(&FchParams);
	FchInitS3LateRestore(&FchParams);
	/* PIC IRQ routine */
	for (byte = 0x0; byte < sizeof(picr_data); byte ++) {
		outb(byte, 0xC00);
		outb(picr_data[byte], 0xC01);
	}

	/* APIC IRQ routine */
	for (byte = 0x0; byte < sizeof(intr_data); byte ++) {
		outb(byte | 0x80, 0xC00);
		outb(intr_data[byte], 0xC01);
	}

	return status;
}
#endif

#ifndef __PRE_RAM__

UINT32 agesawrapper_amdS3Save(VOID)
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

	Status = AmdS3Save(AmdS3SaveParamsPtr);
	if (Status != AGESA_SUCCESS) {
		agesawrapper_amdreadeventlog(AmdInterfaceParams.StdHeader.HeapStatus);
		ASSERT(Status == AGESA_SUCCESS);
	}

	S3DataType = S3DataTypeNonVolatile;
	printk(BIOS_DEBUG, "NvStorageSize=%x, NvStorage=%x\n",
	       (unsigned int)AmdS3SaveParamsPtr->S3DataBlock.NvStorageSize,
	       (unsigned int)AmdS3SaveParamsPtr->S3DataBlock.NvStorage);

	Status = OemAgesaSaveS3Info (
		S3DataType,
		AmdS3SaveParamsPtr->S3DataBlock.NvStorageSize,
		AmdS3SaveParamsPtr->S3DataBlock.NvStorage);

	printk(BIOS_DEBUG, "VolatileStorageSize=%x, VolatileStorage=%x\n",
	       (unsigned int)AmdS3SaveParamsPtr->S3DataBlock.VolatileStorageSize,
	       (unsigned int)AmdS3SaveParamsPtr->S3DataBlock.VolatileStorage);

	if (AmdS3SaveParamsPtr->S3DataBlock.VolatileStorageSize != 0) {
		S3DataType = S3DataTypeVolatile;

		Status = OemAgesaSaveS3Info (
			S3DataType,
			AmdS3SaveParamsPtr->S3DataBlock.VolatileStorageSize,
			AmdS3SaveParamsPtr->S3DataBlock.VolatileStorage);
	}
	OemAgesaSaveMtrr();

	AmdReleaseStruct (&AmdInterfaceParams);

	return (UINT32)Status;
}

#endif  /* #ifndef __PRE_RAM__ */
#endif  /* CONFIG_HAVE_ACPI_RESUME */

UINT32
agesawrapper_amdreadeventlog (
	UINT8 HeapStatus
	)
{
	AGESA_STATUS Status;
	EVENT_PARAMS AmdEventParams;

	LibAmdMemFill (&AmdEventParams,
		       0,
		       sizeof (EVENT_PARAMS),
		       &(AmdEventParams.StdHeader));

	AmdEventParams.StdHeader.AltImageBasePtr = 0;
	AmdEventParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdEventParams.StdHeader.Func = 0;
	AmdEventParams.StdHeader.ImageBasePtr = 0;
	AmdEventParams.StdHeader.HeapStatus = HeapStatus;
	Status = AmdReadEventLog (&AmdEventParams);
	while (AmdEventParams.EventClass != 0) {
		printk(BIOS_DEBUG,"\nEventLog:  EventClass = %x, EventInfo = %x.\n", (unsigned int)AmdEventParams.EventClass,(unsigned int)AmdEventParams.EventInfo);
		printk(BIOS_DEBUG,"  Param1 = %x, Param2 = %x.\n",(unsigned int)AmdEventParams.DataParam1, (unsigned int)AmdEventParams.DataParam2);
		printk(BIOS_DEBUG,"  Param3 = %x, Param4 = %x.\n",(unsigned int)AmdEventParams.DataParam3, (unsigned int)AmdEventParams.DataParam4);
		Status = AmdReadEventLog (&AmdEventParams);
	}

	return (UINT32)Status;
}
