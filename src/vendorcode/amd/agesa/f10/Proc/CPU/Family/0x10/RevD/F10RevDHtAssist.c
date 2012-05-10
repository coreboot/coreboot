/**
 * @file
 *
 * AMD Family_10 RevD HT Assist feature support functions.
 *
 * Provides the functions necessary to initialize the HT Assist feature.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F10
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
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
#include "cpuF10PowerMgmt.h"
#include "cpuLateInit.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuHtAssist.h"
#include "F10PackageType.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FAMILY_0X10_REVD_F10REVDHTASSIST_FILECODE
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

/*---------------------------------------------------------------------------------------*/
/**
 *  Check to see if the input CPU supports HT Assist.
 *
 * @param[in]    HtAssistServices    HT Assist family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 * @retval       TRUE               HT Assist is supported.
 * @retval       FALSE              HT Assist cannot be enabled.
 *
 */
BOOLEAN
STATIC
F10IsHtAssistSupported (
  IN       HT_ASSIST_FAMILY_SERVICES *HtAssistServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32       Module;
  UINT32       PciRegister;
  BOOLEAN      IsSupported;
  PCI_ADDR     PciAddress;
  AGESA_STATUS IgnoredStatus;

  IsSupported = FALSE;
  for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
    if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredStatus)) {
      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = NB_CAPS_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
      if (((NB_CAPS_REGISTER *) &PciRegister)->L3Capable == 1) {
        IsSupported = TRUE;
      }
      break;
    }
  }
  return IsSupported;
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Enable the Probe filter feature.
 *
 * @param[in]    HtAssistServices    HT Assist family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
VOID
STATIC
F10HtAssistInit (
  IN       HT_ASSIST_FAMILY_SERVICES  *HtAssistServices,
  IN       UINT32  Socket,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32                     Module;
  UINT32                     PciRegister;
  PCI_ADDR                   PciAddress;
  AGESA_STATUS               IgnoredStatus;

  for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
    if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredStatus)) {
      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = L3_CACHE_PARAM_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
      ((L3_CACHE_PARAM_REGISTER *) &PciRegister)->L3TagInit = 1;
      LibAmdPciWrite (AccessWidth32, PciAddress, &PciRegister, StdHeader);
      do {
        LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
      } while (((L3_CACHE_PARAM_REGISTER *) &PciRegister)->L3TagInit != 0);

      PciAddress.Address.Register = PROBE_FILTER_CTRL_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
      ((PROBE_FILTER_CTRL_REGISTER *) &PciRegister)->PFMode = 0;
      LibAmdPciWrite (AccessWidth32, PciAddress, &PciRegister, StdHeader);

      F10RevDProbeFilterCritical (PciAddress, PciRegister);

      do {
        LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
      } while (((PROBE_FILTER_CTRL_REGISTER *) &PciRegister)->PFInitDone != 1);
      IDS_OPTION_HOOK (IDS_HT_ASSIST, &PciAddress, StdHeader);
    }
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Save the current settings of the scrubbers, and disabled them.
 *
 * @param[in]    HtAssistServices    HT Assist family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    ScrubSettings       Location to store current L3 scrubber settings.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
VOID
STATIC
F10GetL3ScrubCtrl (
  IN       HT_ASSIST_FAMILY_SERVICES *HtAssistServices,
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
 * @param[in]    HtAssistServices    HT Assist family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    ScrubSettings       Location to store current L3 scrubber settings.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
VOID
STATIC
F10SetL3ScrubCtrl (
  IN       HT_ASSIST_FAMILY_SERVICES *HtAssistServices,
  IN       UINT32 Socket,
  IN       UINT32 ScrubSettings[L3_SCRUBBER_CONTEXT_ARRAY_SIZE],
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32       Module;
  UINT32       PciRegister;
  PCI_ADDR     PciAddress;
  AGESA_STATUS IgnoredStatus;

  for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
    if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredStatus)) {

      ASSERT (Module < L3_SCRUBBER_CONTEXT_ARRAY_SIZE);

      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = SCRUB_RATE_CTRL_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
      ((SCRUB_RATE_CTRL_REGISTER *) &PciRegister)->DramScrub =
        ((F10_SCRUB_CONTEXT *) &ScrubSettings[Module])->DramScrub;
      ((SCRUB_RATE_CTRL_REGISTER *) &PciRegister)->L3Scrub =
        ((F10_SCRUB_CONTEXT *) &ScrubSettings[Module])->L3Scrub;
      LibAmdPciWrite (AccessWidth32, PciAddress, &PciRegister, StdHeader);

      PciAddress.Address.Register = DRAM_SCRUB_ADDR_LOW_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
      ((DRAM_SCRUB_ADDR_LOW_REGISTER *) &PciRegister)->ScrubReDirEn =
        ((F10_SCRUB_CONTEXT *) &ScrubSettings[Module])->Redirect;
      LibAmdPciWrite (AccessWidth32, PciAddress, &PciRegister, StdHeader);
    }
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Set MSR bits required for HT Assist on each core.
 *
 * @param[in]    HtAssistServices    HT Assist family services.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
VOID
STATIC
F10HookDisableCache (
  IN       HT_ASSIST_FAMILY_SERVICES *HtAssistServices,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64  MsrRegister;

  LibAmdMsrRead (MSR_BU_CFG2, &MsrRegister, StdHeader);
  MsrRegister |= BIT42;
  LibAmdMsrWrite (MSR_BU_CFG2, &MsrRegister, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Hook before the probe filter initialization sequence.
 *
 * @param[in]    HtAssistServices    HT Assist family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
VOID
STATIC
F10HookBeforeInit (
  IN       HT_ASSIST_FAMILY_SERVICES *HtAssistServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32          Module;
  UINT32          PciRegister;
  UINT32          PfCtrlRegister;
  PCI_ADDR        PciAddress;
  CPU_LOGICAL_ID  LogicalId;
  AGESA_STATUS    IgnoredStatus;
  UINT32          PackageType;

  GetLogicalIdOfSocket (Socket, &LogicalId, StdHeader);
  PackageType = LibAmdGetPackageType (StdHeader);

  PciRegister = 0;
  ((PROBE_FILTER_CTRL_REGISTER *) &PciRegister)->PFWayNum = 2;
  ((PROBE_FILTER_CTRL_REGISTER *) &PciRegister)->PFSubCacheEn = 15;
  ((PROBE_FILTER_CTRL_REGISTER *) &PciRegister)->PFLoIndexHashEn = 1;
  for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
    if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredStatus)) {
      PciAddress.Address.Function = FUNC_3;
      PciAddress.Address.Register = PROBE_FILTER_CTRL_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &PfCtrlRegister, StdHeader);
      ((PROBE_FILTER_CTRL_REGISTER *) &PciRegister)->PFPreferredSORepl =
        ((PROBE_FILTER_CTRL_REGISTER *) &PfCtrlRegister)->PFPreferredSORepl;
      LibAmdPciWrite (AccessWidth32, PciAddress, &PciRegister, StdHeader);

      // Assumption: all socket use the same CPU package.
      if (((LogicalId.Revision & AMD_F10_D0) != 0) && (PackageType == PACKAGE_TYPE_C32)) {
        // Apply erratum #384
        // Set F2x11C[13:12] = 11b
        PciAddress.Address.Function = FUNC_2;
        PciAddress.Address.Register = 0x11C;
        LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
        PciRegister |= 0x3000;
        LibAmdPciWrite (AccessWidth32, PciAddress, &PciRegister, StdHeader);
      }
    }
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Check to see if the input CPU is running in the optimal configuration.
 *
 * @param[in]    HtAssistServices    HT Assist family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 * @retval       TRUE               HT Assist is running sub-optimally.
 * @retval       FALSE              HT Assist is running optimally.
 *
 */
STATIC BOOLEAN
F10IsNonOptimalConfig (
  IN       HT_ASSIST_FAMILY_SERVICES *HtAssistServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN      IsNonOptimal;
  BOOLEAN      IsMemoryPresent;
  UINT32       Module;
  UINT32       PciRegister;
  PCI_ADDR     PciAddress;
  AGESA_STATUS IgnoredStatus;

  IsNonOptimal = FALSE;
  for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
    if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredStatus)) {
      IsMemoryPresent = FALSE;
      PciAddress.Address.Function = FUNC_2;
      PciAddress.Address.Register = DRAM_CFG_HI_REG0;

      LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
      if (((DRAM_CFG_HI_REGISTER *) &PciRegister)->MemClkFreqVal == 1) {
        IsMemoryPresent = TRUE;
        if (((DRAM_CFG_HI_REGISTER *) &PciRegister)->MemClkFreq < 4) {
          IsNonOptimal = TRUE;
          break;
        }
      }

      PciAddress.Address.Register = DRAM_CFG_HI_REG1;

      LibAmdPciRead (AccessWidth32, PciAddress, &PciRegister, StdHeader);
      if (((DRAM_CFG_HI_REGISTER *) &PciRegister)->MemClkFreqVal == 1) {
        IsMemoryPresent = TRUE;
        if (((DRAM_CFG_HI_REGISTER *) &PciRegister)->MemClkFreq < 4) {
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


CONST HT_ASSIST_FAMILY_SERVICES ROMDATA F10HtAssist =
{
  0,
  F10IsHtAssistSupported,
  F10HtAssistInit,
  F10GetL3ScrubCtrl,
  F10SetL3ScrubCtrl,
  F10HookBeforeInit,
  (PF_HT_ASSIST_AFTER_INIT) CommonVoid,
  F10HookDisableCache,
  (PF_HT_ASSIST_ENABLE_CACHE) CommonVoid,
  F10IsNonOptimalConfig
};
