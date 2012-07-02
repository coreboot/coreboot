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
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
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

