/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 RevD L3 dependent feature support functions.
 *
 * Provides the functions necessary to initialize L3 dependent feature.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F10
 * @e \$Revision: 49216 $   @e \$Date: 2011-03-19 11:34:39 +0800 (Sat, 19 Mar 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
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


/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "CommonReturns.h"
#include "cpuRegisters.h"
#include "cpuF10PowerMgmt.h"
#include "cpuLateInit.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuL3Features.h"
#include "F10PackageType.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FAMILY_0X10_REVD_F10REVDL3FEATURES_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/**
 * The family 10h background scrubber context structure.
 *
 * These fields need to be saved, modified, then restored
 * per die as part of HT Assist initialization.
 */
typedef struct {
  UINT32  DramScrub:5;               ///< DRAM scrub rate
  UINT32  :3;                        ///< Reserved
  UINT32  L3Scrub:5;                 ///< L3 scrub rate
  UINT32  :3;                        ///< Reserved
  UINT32  Redirect:1;                ///< DRAM scrubber redirect enable
  UINT32  :15;                       ///< Reserved
} F10_SCRUB_CONTEXT;


/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
extern
VOID
F10RevDProbeFilterCritical (
  IN       PCI_ADDR PciAddress,
  IN       UINT32   LocalPciRegister
  );

/*---------------------------------------------------------------------------------------*/
/**
 *  Check to see if the input CPU supports L3 dependent features.
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 * @retval       TRUE                L3 dependent features are supported.
 * @retval       FALSE               L3 dependent features are not supported.
 *
 */
BOOLEAN
STATIC
F10IsL3FeatureSupported (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32       Module;
  UINT32       LocalPciRegister;
  BOOLEAN      IsSupported;
  PCI_ADDR     PciAddress;
  AGESA_STATUS IgnoredStatus;

  IsSupported = FALSE;
  for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
    if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredStatus)) {
      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = NB_CAPS_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      if (((NB_CAPS_REGISTER *) &LocalPciRegister)->L3Capable == 1) {
        IsSupported = TRUE;
      }
      break;
    }
  }
  return IsSupported;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Check to see if the input CPU supports HT Assist.
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    PlatformConfig      Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 * @retval       TRUE                HT Assist is supported.
 * @retval       FALSE               HT Assist cannot be enabled.
 *
 */
BOOLEAN
STATIC
F10IsHtAssistSupported (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN          IsSupported;
  UINT32           CpuCount;
  AP_MAILBOXES     ApMailboxes;

  IsSupported = FALSE;

  if (PlatformConfig->PlatformProfile.UseHtAssist) {
    CpuCount = GetNumberOfProcessors (StdHeader);
    ASSERT (CpuCount != 0);

    if (CpuCount == 1) {
      GetApMailbox (&ApMailboxes.ApMailInfo.Info, StdHeader);
      if (ApMailboxes.ApMailInfo.Fields.ModuleType != 0) {
        IsSupported = TRUE;
      }
    } else if (CpuCount > 1) {
      IsSupported = TRUE;
    }
  }
  return IsSupported;
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Enable the Probe filter feature.
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
VOID
STATIC
F10HtAssistInit (
  IN       L3_FEATURE_FAMILY_SERVICES  *L3FeatureServices,
  IN       UINT32  Socket,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32                     Module;
  UINT32                     LocalPciRegister;
  PCI_ADDR                   PciAddress;
  AGESA_STATUS               IgnoredStatus;

  for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
    if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredStatus)) {
      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = L3_CACHE_PARAM_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      ((L3_CACHE_PARAM_REGISTER *) &LocalPciRegister)->L3TagInit = 1;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      do {
        LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      } while (((L3_CACHE_PARAM_REGISTER *) &LocalPciRegister)->L3TagInit != 0);

      PciAddress.Address.Register = PROBE_FILTER_CTRL_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      ((PROBE_FILTER_CTRL_REGISTER *) &LocalPciRegister)->PFMode = 0;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

      F10RevDProbeFilterCritical (PciAddress, LocalPciRegister);

      do {
        LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      } while (((PROBE_FILTER_CTRL_REGISTER *) &LocalPciRegister)->PFInitDone != 1);
      IDS_OPTION_HOOK (IDS_HT_ASSIST, &PciAddress, StdHeader);
    }
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Save the current settings of the scrubbers, and disabled them.
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    ScrubSettings       Location to store current L3 scrubber settings.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
VOID
STATIC
F10GetL3ScrubCtrl (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       UINT32 Socket,
  IN       UINT32 ScrubSettings[L3_SCRUBBER_CONTEXT_ARRAY_SIZE],
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32       Module;
  UINT32       ScrubCtrl;
  UINT32       ScrubAddr;
  PCI_ADDR     PciAddress;
  AGESA_STATUS IgnoredStatus;

  for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
    if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredStatus)) {

      ASSERT (Module < L3_SCRUBBER_CONTEXT_ARRAY_SIZE);

      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = DRAM_SCRUB_ADDR_LOW_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &ScrubAddr, StdHeader);

      PciAddress.Address.Register = SCRUB_RATE_CTRL_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &ScrubCtrl, StdHeader);

      ((F10_SCRUB_CONTEXT *) &ScrubSettings[Module])->DramScrub =
        ((SCRUB_RATE_CTRL_REGISTER *) &ScrubCtrl)->DramScrub;
      ((F10_SCRUB_CONTEXT *) &ScrubSettings[Module])->L3Scrub =
        ((SCRUB_RATE_CTRL_REGISTER *) &ScrubCtrl)->L3Scrub;
      ((F10_SCRUB_CONTEXT *) &ScrubSettings[Module])->Redirect =
        ((DRAM_SCRUB_ADDR_LOW_REGISTER *) &ScrubAddr)->ScrubReDirEn;

      ((SCRUB_RATE_CTRL_REGISTER *) &ScrubCtrl)->DramScrub = 0;
      ((SCRUB_RATE_CTRL_REGISTER *) &ScrubCtrl)->L3Scrub = 0;
      ((DRAM_SCRUB_ADDR_LOW_REGISTER *) &ScrubAddr)->ScrubReDirEn = 0;
      LibAmdPciWrite (AccessWidth32, PciAddress, &ScrubCtrl, StdHeader);
      PciAddress.Address.Register = DRAM_SCRUB_ADDR_LOW_REG;
      LibAmdPciWrite (AccessWidth32, PciAddress, &ScrubAddr, StdHeader);
    }
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Restore the initial settings for the scrubbers.
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    ScrubSettings       Location to store current L3 scrubber settings.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
VOID
STATIC
F10SetL3ScrubCtrl (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       UINT32 Socket,
  IN       UINT32 ScrubSettings[L3_SCRUBBER_CONTEXT_ARRAY_SIZE],
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32       Module;
  UINT32       LocalPciRegister;
  PCI_ADDR     PciAddress;
  AGESA_STATUS IgnoredStatus;

  for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
    if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredStatus)) {

      ASSERT (Module < L3_SCRUBBER_CONTEXT_ARRAY_SIZE);

      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = SCRUB_RATE_CTRL_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      ((SCRUB_RATE_CTRL_REGISTER *) &LocalPciRegister)->DramScrub =
        ((F10_SCRUB_CONTEXT *) &ScrubSettings[Module])->DramScrub;
      ((SCRUB_RATE_CTRL_REGISTER *) &LocalPciRegister)->L3Scrub =
        ((F10_SCRUB_CONTEXT *) &ScrubSettings[Module])->L3Scrub;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

      PciAddress.Address.Register = DRAM_SCRUB_ADDR_LOW_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      ((DRAM_SCRUB_ADDR_LOW_REGISTER *) &LocalPciRegister)->ScrubReDirEn =
        ((F10_SCRUB_CONTEXT *) &ScrubSettings[Module])->Redirect;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    }
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Set MSR bits required for L3 dependent features on each core.
 *
 * @param[in]    L3FeatureServices   L3 feature family services.
 * @param[in]    HtAssistEnabled     Indicates whether Ht Assist is enabled.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
VOID
STATIC
F10HookDisableCache (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       BOOLEAN HtAssistEnabled,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64  LocalMsrRegister;

  LibAmdMsrRead (MSR_BU_CFG2, &LocalMsrRegister, StdHeader);
  LocalMsrRegister |= BIT42;
  LibAmdMsrWrite (MSR_BU_CFG2, &LocalMsrRegister, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Hook before L3 features initialization sequence.
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
VOID
STATIC
F10HookBeforeInit (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32          Module;
  UINT32          LocalPciRegister;
  UINT32          PfCtrlRegister;
  PCI_ADDR        PciAddress;
  CPU_LOGICAL_ID  LogicalId;
  AGESA_STATUS    IgnoredStatus;
  UINT32          PackageType;

  GetLogicalIdOfSocket (Socket, &LogicalId, StdHeader);
  PackageType = LibAmdGetPackageType (StdHeader);

  LocalPciRegister = 0;
  ((PROBE_FILTER_CTRL_REGISTER *) &LocalPciRegister)->PFWayNum = 2;
  ((PROBE_FILTER_CTRL_REGISTER *) &LocalPciRegister)->PFSubCacheEn = 15;
  ((PROBE_FILTER_CTRL_REGISTER *) &LocalPciRegister)->PFLoIndexHashEn = 1;
  for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
    if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredStatus)) {
      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = PROBE_FILTER_CTRL_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &PfCtrlRegister, StdHeader);
      ((PROBE_FILTER_CTRL_REGISTER *) &LocalPciRegister)->PFPreferredSORepl =
        ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFPreferredSORepl;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

      // Assumption: all socket use the same CPU package.
      if (((LogicalId.Revision & AMD_F10_D0) != 0) && (PackageType == PACKAGE_TYPE_C32)) {
        // Apply erratum #384
        // Set F2x11C[13:12] = 11b
        PciAddress.Address.Function = FUNC_2;
        PciAddress.Address.Register = 0x11C;
        LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
        LocalPciRegister |= 0x3000;
        LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      }
    }
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Check to see if the input CPU is running in the optimal configuration.
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 * @retval       TRUE               HT Assist is running sub-optimally.
 * @retval       FALSE              HT Assist is running optimally.
 *
 */
BOOLEAN
F10IsNonOptimalConfig (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN      IsNonOptimal;
  BOOLEAN      IsMemoryPresent;
  UINT32       Module;
  UINT32       LocalPciRegister;
  PCI_ADDR     PciAddress;
  AGESA_STATUS IgnoredStatus;

  IsNonOptimal = FALSE;
  for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
    if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredStatus)) {
      IsMemoryPresent = FALSE;
      PciAddress.Address.Function = FUNC_2;
      PciAddress.Address.Register = DRAM_CFG_HI_REG0;

      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      if (((DRAM_CFG_HI_REGISTER *) &LocalPciRegister)->MemClkFreqVal == 1) {
        IsMemoryPresent = TRUE;
        if (((DRAM_CFG_HI_REGISTER *) &LocalPciRegister)->MemClkFreq < 4) {
          IsNonOptimal = TRUE;
          break;
        }
      }

      PciAddress.Address.Register = DRAM_CFG_HI_REG1;

      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      if (((DRAM_CFG_HI_REGISTER *) &LocalPciRegister)->MemClkFreqVal == 1) {
        IsMemoryPresent = TRUE;
        if (((DRAM_CFG_HI_REGISTER *) &LocalPciRegister)->MemClkFreq < 4) {
          IsNonOptimal = TRUE;
          break;
        }
      }
      if (!IsMemoryPresent) {
        IsNonOptimal = TRUE;
        break;
      }
    }
  }
  return IsNonOptimal;
}


CONST L3_FEATURE_FAMILY_SERVICES ROMDATA F10L3Features =
{
  0,
  F10IsL3FeatureSupported,
  F10GetL3ScrubCtrl,
  F10SetL3ScrubCtrl,
  F10HookBeforeInit,
  (PF_L3_FEATURE_AFTER_INIT) CommonVoid,
  F10HookDisableCache,
  (PF_L3_FEATURE_ENABLE_CACHE) CommonVoid,
  F10IsHtAssistSupported,
  F10HtAssistInit,
  F10IsNonOptimalConfig,
  (PF_ATM_MODE_IS_SUPPORTED) CommonReturnFalse,
  (PF_ATM_MODE_INIT) CommonVoid
};
