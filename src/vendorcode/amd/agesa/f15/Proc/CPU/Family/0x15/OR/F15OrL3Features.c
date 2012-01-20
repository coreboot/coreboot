/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Orochi L3 dependent feature support functions.
 *
 * Provides the functions necessary to initialize L3 dependent features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/OR
 * @e \$Revision: 60552 $   @e \$Date: 2011-10-17 18:50:55 -0600 (Mon, 17 Oct 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
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
#include "cpuF15PowerMgmt.h"
#include "cpuF15OrPowerMgmt.h"
#include "cpuLateInit.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuL3Features.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_OR_F15ORL3FEATURES_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define L3Cache8_0M 0xCCCC

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/**
 * The family 15h background scrubber context structure.
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
} F15_SCRUB_CONTEXT;


/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
BOOLEAN
F15OrIsNonOptimalConfig (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------*/
/**
 *  Check to see if the input CPU supports L3 dependent features.
 *
 * @param[in]    L3FeatureServices   L3 feature family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 * @param[in]    PlatformConfig      Contains the runtime modifiable feature input data.
 *
 * @retval       TRUE                L3 dependent features are supported.
 * @retval       FALSE               L3 dependent features are not supported.
 *
 */
BOOLEAN
STATIC
F15OrIsL3FeatureSupported (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig
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
 *  Enable the Probe filter feature
 *
 * @param[in]    L3FeatureServices   L3 family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
VOID
STATIC
F15OrHtAssistInit (
  IN       L3_FEATURE_FAMILY_SERVICES  *L3FeatureServices,
  IN       UINT32  Socket,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32                     Module;
  UINT32                     L3CacheParamRegister;
  UINT32                     PfCtrlRegister;
  PCI_ADDR                   PciAddress;
  AGESA_STATUS               IgnoredStatus;

  for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
    if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredStatus)) {
      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = L3_CACHE_PARAM_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &L3CacheParamRegister, StdHeader);
      ((L3_CACHE_PARAM_REGISTER *) &L3CacheParamRegister)->L3TagInit = 1;
      LibAmdPciWrite (AccessWidth32, PciAddress, &L3CacheParamRegister, StdHeader);
      do {
        LibAmdPciRead (AccessWidth32, PciAddress, &L3CacheParamRegister, StdHeader);
      } while (((L3_CACHE_PARAM_REGISTER *) &L3CacheParamRegister)->L3TagInit != 0);

      PciAddress.Address.Register = PROBE_FILTER_CTRL_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &PfCtrlRegister, StdHeader);
      ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFWayHashEn = 1;
      ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFLoIndexHashEn = 1;
      ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFWayNum = 2;
      ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFSubCacheEn = 0xF;
      if ((L3CacheParamRegister & 0xFFFF) == L3Cache8_0M) {
        ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFSubCacheSize0 = 1;
        ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFSubCacheSize1 = 1;
        ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFSubCacheSize2 = 1;
        ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFSubCacheSize3 = 1;
        ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFMode = 3;
        ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFPreferredSORepl = 2;
      } else {
        ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFSubCacheSize0 = 0;
        ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFSubCacheSize1 = 0;
        ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFSubCacheSize2 = 0;
        ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFSubCacheSize3 = 0;
        ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFMode = 2;
        ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFPreferredSORepl = 0;
      }
      LibAmdPciWrite (AccessWidth32, PciAddress, &PfCtrlRegister, StdHeader);

      do {
        LibAmdPciRead (AccessWidth32, PciAddress, &PfCtrlRegister, StdHeader);
      } while (((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFInitDone != 1);
      IDS_OPTION_HOOK (IDS_HT_ASSIST, &PciAddress, StdHeader);
    }
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Enable the ATM Mode feature.
 *
 * @param[in]    L3FeatureServices    L3 feature family services.
 * @param[in]    Socket               Processor socket to check.
 * @param[in]    StdHeader            Config Handle for library, services.
 *
 */
VOID
STATIC
F15OrAtmModeInit (
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
      PciAddress.Address.Function = FUNC_0;
      PciAddress.Address.Register = LTC_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      ((LTC_REGISTER *) &LocalPciRegister)->ATMModeEn = 1;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = L3_CONTROL_1_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      ((L3_CONTROL_1_REGISTER *) &LocalPciRegister)->L3ATMModeEn = 1;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    }
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Save the current settings of the scrubbers, and disabled them.
 *
 * @param[in]    L3FeatureServices   L3 feature family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    ScrubSettings       Location to store current L3 scrubber settings.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
VOID
STATIC
F15OrGetL3ScrubCtrl (
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

      ((F15_SCRUB_CONTEXT *) &ScrubSettings[Module])->DramScrub =
        ((SCRUB_RATE_CTRL_REGISTER *) &ScrubCtrl)->DramScrub;
      ((F15_SCRUB_CONTEXT *) &ScrubSettings[Module])->L3Scrub =
        ((SCRUB_RATE_CTRL_REGISTER *) &ScrubCtrl)->L3Scrub;
      ((F15_SCRUB_CONTEXT *) &ScrubSettings[Module])->Redirect =
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
F15OrSetL3ScrubCtrl (
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
        ((F15_SCRUB_CONTEXT *) &ScrubSettings[Module])->DramScrub;
      ((SCRUB_RATE_CTRL_REGISTER *) &LocalPciRegister)->L3Scrub =
        ((F15_SCRUB_CONTEXT *) &ScrubSettings[Module])->L3Scrub;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

      PciAddress.Address.Register = DRAM_SCRUB_ADDR_LOW_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      ((DRAM_SCRUB_ADDR_LOW_REGISTER *) &LocalPciRegister)->ScrubReDirEn =
        ((F15_SCRUB_CONTEXT *) &ScrubSettings[Module])->Redirect;
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    }
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Set MSR bits required for L3 feature support on each core.
 *
 * @param[in]    L3FeatureServices    L3 Feature family services.
 * @param[in]    HtAssistEnabled      Indicates whether Ht Assist is enabled.
 * @param[in]    StdHeader            Config Handle for library, services.
 *
 */
VOID
STATIC
F15OrHookDisableCache (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       BOOLEAN HtAssistEnabled,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64  LocalMsrRegister;

  // This bit is set only if Probe Filter is enabled.
  if (HtAssistEnabled) {
    LibAmdMsrRead (MSR_BU_CFG2, &LocalMsrRegister, StdHeader);
    LocalMsrRegister |= BIT42;
    LibAmdMsrWrite (MSR_BU_CFG2, &LocalMsrRegister, StdHeader);
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
F15OrIsNonOptimalConfig (
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
        if (((DRAM_CFG_HI_REGISTER *) &LocalPciRegister)->MemClkFreq < 0x0a) {
          IsNonOptimal = TRUE;
          break;
        }
      }

      PciAddress.Address.Register = DRAM_CFG_HI_REG1;

      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      if (((DRAM_CFG_HI_REGISTER *) &LocalPciRegister)->MemClkFreqVal == 1) {
        IsMemoryPresent = TRUE;
        if (((DRAM_CFG_HI_REGISTER *) &LocalPciRegister)->MemClkFreq < 0x0a) {
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

/*---------------------------------------------------------------------------------------*/
/**
 *  Check to see if the input CPU supports HT Assist.
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    PlatformConfig      Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 * @retval       TRUE               HT Assist is supported.
 * @retval       FALSE              HT Assist cannot be enabled.
 *
 */
BOOLEAN
STATIC
F15OrIsHtAssistSupported (
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
 *  Check to see if the input CPU supports ATM Mode.
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    PlatformConfig      Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 * @retval       TRUE                ATM Mode is supported.
 * @retval       FALSE               ATM Mode cannot be enabled.
 *
 */
BOOLEAN
STATIC
F15OrIsAtmModeSupported (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN IsSupported;

  IsSupported = TRUE;

  if (!PlatformConfig->PlatformProfile.UseAtmMode) {
    IsSupported = FALSE;
  }
  return IsSupported;
}

CONST L3_FEATURE_FAMILY_SERVICES ROMDATA F15OrL3Features =
{
  0,
  F15OrIsL3FeatureSupported,
  F15OrGetL3ScrubCtrl,
  F15OrSetL3ScrubCtrl,
  (PF_L3_FEATURE_BEFORE_INIT) CommonVoid,
  (PF_L3_FEATURE_AFTER_INIT) CommonVoid,
  F15OrHookDisableCache,
  (PF_L3_FEATURE_ENABLE_CACHE) CommonVoid,
  F15OrIsHtAssistSupported,
  F15OrHtAssistInit,
  F15OrIsNonOptimalConfig,
  F15OrIsAtmModeSupported,
  F15OrAtmModeInit,
};
