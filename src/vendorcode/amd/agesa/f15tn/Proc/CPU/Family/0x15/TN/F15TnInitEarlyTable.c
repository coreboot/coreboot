/* $NoKeywords:$ */
/**
 * @file
 *
 * Initialize the Family 15h Trinity specific way of running early initialization.
 *
 * Returns the table of initialization steps to perform at
 * AmdInitEarly.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x15/TN
 * @e \$Revision: 64491 $   @e \$Date: 2012-01-23 12:37:30 -0600 (Mon, 23 Jan 2012) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
#include "GeneralServices.h"
#include "heapManager.h"
#include "Fch.h"
#include "Gnb.h"
#include "GnbLib.h"
#include "cpuEarlyInit.h"
#include "cpuF15TnPowerMgmt.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)
#define FILECODE PROC_CPU_FAMILY_0X15_TN_F15TNINITEARLYTABLE_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
F15TnLoadMicrocodePatchAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

VOID
GetF15TnEarlyInitOnCoreTable (
  IN       CPU_SPECIFIC_SERVICES                *FamilyServices,
     OUT   CONST S_PERFORM_EARLY_INIT_ON_CORE   **Table,
  IN       AMD_CPU_EARLY_PARAMS                 *EarlyParams,
  IN       AMD_CONFIG_PARAMS                    *StdHeader
  );

VOID
ApplyWorkaroundForFchErratum39 (
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

VOID
F15TnNbPstateForceBeforeApLaunchAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern F_PERFORM_EARLY_INIT_ON_CORE SetRegistersFromTablesAtEarly;
extern F_PERFORM_EARLY_INIT_ON_CORE F15SetBrandIdRegistersAtEarly;
extern F_PERFORM_EARLY_INIT_ON_CORE LocalApicInitializationAtEarly;

CONST S_PERFORM_EARLY_INIT_ON_CORE ROMDATA F15TnEarlyInitOnCoreTable[] =
{
  {SetRegistersFromTablesAtEarly, PERFORM_EARLY_ANY_CONDITION},
  {F15SetBrandIdRegistersAtEarly, PERFORM_EARLY_ANY_CONDITION},
  {LocalApicInitializationAtEarly, PERFORM_EARLY_ANY_CONDITION},
  {F15TnLoadMicrocodePatchAtEarly, PERFORM_EARLY_ANY_CONDITION},
  {F15TnNbPstateForceBeforeApLaunchAtEarly, PERFORM_EARLY_WARM_RESET},
  {NULL, 0}
};

/*------------------------------------------------------------------------------------*/
/**
 * Initializer routine that may be invoked at AmdCpuEarly to return the steps that a
 * processor that uses the standard initialization steps should take.
 *
 *  @CpuServiceMethod{::F_GET_EARLY_INIT_TABLE}.
 *
 * @param[in]       FamilyServices    The current Family Specific Services.
 * @param[out]      Table             Table of appropriate init steps for the executing core.
 * @param[in]       EarlyParams       Service Interface structure to initialize.
 * @param[in]       StdHeader         Opaque handle to standard config header.
 *
 */
VOID
GetF15TnEarlyInitOnCoreTable (
  IN       CPU_SPECIFIC_SERVICES                *FamilyServices,
     OUT   CONST S_PERFORM_EARLY_INIT_ON_CORE   **Table,
  IN       AMD_CPU_EARLY_PARAMS                 *EarlyParams,
  IN       AMD_CONFIG_PARAMS                    *StdHeader
  )
{
  *Table = F15TnEarlyInitOnCoreTable;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Update microcode patch in current processor for Family15h TN.
 *
 * This function acts as a wrapper for calling the LoadMicrocodePatch
 * routine at AmdInitEarly.
 *
 *  @param[in]   FamilyServices      The current Family Specific Services.
 *  @param[in]   EarlyParams         Service parameters.
 *  @param[in]   StdHeader           Config handle for library and services.
 *
 */
VOID
F15TnLoadMicrocodePatchAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  BOOLEAN        IsPatchLoaded;

  AGESA_TESTPOINT (TpProcCpuLoadUcode, StdHeader);

  if (IsCorePairPrimary (FirstCoreIsComputeUnitPrimary, StdHeader)) {
    IsPatchLoaded = LoadMicrocodePatch (StdHeader);
  }

  // After microcode patch has been loaded, apply the workaround for FCH erratum 39
  ApplyWorkaroundForFchErratum39 (StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Apply the workaround for FCH H2/H3 erratum #39.
 *
 * This function detects the FCH version and applies the appropriate workaround, if
 * required.
 *
 *  @param[in]   StdHeader           Config handle for library and services.
 *
 */
VOID
ApplyWorkaroundForFchErratum39 (
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT8          MiscReg51;
  UINT8          RevisionId;
  UINT16         AcpiPmTmrBlk;
  UINT32         VendorIdDeviceId;
  UINT64         MsrValue;
  PCI_ADDR       PciAddress;
  AGESA_STATUS   IgnoredSts;
  CPU_LOGICAL_ID LogicalId;

  // Read Vendor ID / Device ID
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x14, 0, 0);
  LibAmdPciRead (AccessWidth32, PciAddress, &VendorIdDeviceId, StdHeader);

  // For Hudson based system, perform workaround
  if (VendorIdDeviceId == 0x780B1022) {
    PciAddress.Address.Register = 0x8;
    LibAmdPciRead (AccessWidth8, PciAddress, &RevisionId, StdHeader);
    if ((RevisionId == 0x14) && IsBsp (StdHeader, &IgnoredSts)) {
      // Enable hardware workaround by setting Misc_reg x51[0]
      LibAmdMemRead (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + MISC_BASE + 0x51), &MiscReg51, StdHeader);
      MiscReg51 |= BIT0;
      LibAmdMemWrite (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + MISC_BASE + 0x51), &MiscReg51, StdHeader);
    } else if (RevisionId == 0x13) {
      GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
      if ((LogicalId.Revision & AMD_F15_TN_GT_A0) != 0) {
        // For revs A1+, set up the C0010055 MSR
        GnbLibIndirectIoBlockRead (0xCD6, 0xCD7, AccessWidth8, 0x64, 2, &AcpiPmTmrBlk, StdHeader);
        LibAmdMsrRead (0xC0010055, &MsrValue, StdHeader);
        MsrValue |= BIT30;
        MsrValue |= AcpiPmTmrBlk;
        LibAmdMsrWrite (0xC0010055, &MsrValue, StdHeader);
      }
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Prevent NB P-state transitions prior to AP launch on Family 15h TN.
 *
 * This function determines the current NB P-state and forces the NB to remain
 * in that P-state.
 *
 *  @param[in]   FamilyServices      The current Family Specific Services.
 *  @param[in]   EarlyParams         Service parameters.
 *  @param[in]   StdHeader           Config handle for library and services.
 *
 */
VOID
F15TnNbPstateForceBeforeApLaunchAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT64                    MsrValue;
  UINT64                    PerfCtrlSave;
  UINT64                    PerfStsSave;
  PCI_ADDR                  PciAddress;
  AGESA_STATUS              IgnoredSts;
  ALLOCATE_HEAP_PARAMS      Alloc;
  NB_PSTATE_CTRL_REGISTER   NbPsCtrl;

  if (IsBsp (StdHeader, &IgnoredSts) && FamilyServices->IsNbPstateEnabled (FamilyServices, &EarlyParams->PlatformConfig, StdHeader)) {
    LibAmdMsrRead (MSR_NB_PERF_CTL3, &PerfCtrlSave, StdHeader);
    MsrValue = 0x00000006004004E9;
    LibAmdMsrRead (MSR_NB_PERF_CTR3, &PerfStsSave, StdHeader);
    LibAmdMsrWrite (MSR_NB_PERF_CTL3, &MsrValue, StdHeader);
    MsrValue = 0;
    LibAmdMsrWrite (MSR_NB_PERF_CTR3, &MsrValue, StdHeader);
    PciAddress.AddressValue = NB_PSTATE_CTRL_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);
    Alloc.RequestedBufferSize = sizeof (NB_PSTATE_CTRL_REGISTER);
    Alloc.BufferHandle = AMD_CPU_NB_PSTATE_FIXUP_HANDLE;
    Alloc.Persist = 0;
    if (HeapAllocateBuffer (&Alloc, StdHeader) == AGESA_SUCCESS) {
      *((NB_PSTATE_CTRL_REGISTER *) Alloc.BufferPtr) = NbPsCtrl;
    } else {
      ASSERT (FALSE);
    }
    LibAmdMsrRead (MSR_NB_PERF_CTR3, &MsrValue, StdHeader);
    if (MsrValue == 0) {
      NbPsCtrl.SwNbPstateLoDis = 1;
    } else {
      NbPsCtrl.SwNbPstateLoDis = 0;
      NbPsCtrl.NbPstateDisOnP0 = 0;
      NbPsCtrl.NbPstateThreshold = 0;
    }
    LibAmdPciWrite (AccessWidth32, PciAddress, &NbPsCtrl, StdHeader);
    LibAmdMsrWrite (MSR_NB_PERF_CTL3, &PerfCtrlSave, StdHeader);
    LibAmdMsrWrite (MSR_NB_PERF_CTR3, &PerfStsSave, StdHeader);
  }
}

