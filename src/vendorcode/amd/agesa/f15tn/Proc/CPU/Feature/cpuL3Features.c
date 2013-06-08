/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU L3 Features Initialization functions.
 *
 * Contains code for initializing L3 features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
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


/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuLateInit.h"
#include "cpuFamilyTranslation.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFeatures.h"
#include "cpuL3Features.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FEATURE_CPUL3FEATURES_FILECODE
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
extern CPU_FAMILY_SUPPORT_TABLE L3FeatureFamilyServiceTable;

/*---------------------------------------------------------------------------------------*/
/**
 *  Should L3 features be enabled
 *
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               L3 Features are supported
 * @retval       FALSE              L3 Features are not supported
 *
 */
BOOLEAN
STATIC
IsL3FeatureEnabled (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  BOOLEAN          IsEnabled;
  UINT32           Socket;
  L3_FEATURE_FAMILY_SERVICES *FamilyServices;

  IsEnabled = FALSE;
  if (PlatformConfig->PlatformProfile.UseHtAssist ||
      PlatformConfig->PlatformProfile.UseAtmMode) {
    IsEnabled = TRUE;
    for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
      if (IsProcessorPresent (Socket, StdHeader)) {
        GetFeatureServicesOfSocket (&L3FeatureFamilyServiceTable, Socket, (CONST VOID **)&FamilyServices, StdHeader);
        if ((FamilyServices == NULL) || !FamilyServices->IsL3FeatureSupported (FamilyServices, Socket, StdHeader)) {
          IsEnabled = FALSE;
          break;
        }
      }
    }
  }
  return IsEnabled;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable L3 dependent features.
 *
 * L3 features initialization requires the following series of steps.
 *  1. Disable L3 and DRAM scrubbers on all nodes
 *  2. Wait 40us for outstanding scrub results to complete
 *  3. Disable all cache activity in the system
 *  4. Issue WBINVD on all active cores
 *  5. Initialize Probe Filter, if supported
 *  6. Initialize ATM Mode, if supported
 *  7. Enable all cache activity in the system
 *  8. Restore L3 and DRAM scrubber register values
 *
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       AGESA_SUCCESS      Always succeeds.
 *
 */
AGESA_STATUS
STATIC
InitializeL3Feature (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32                      CpuCount;
  UINT32                      Socket;
  BOOLEAN                     HtAssistEnabled;
  BOOLEAN                     AtmModeEnabled;
  AGESA_STATUS                AgesaStatus;
  AP_MAILBOXES                ApMailboxes;
  AP_EXE_PARAMS               ApParams;
  UINT32                      Scrubbers[MAX_SOCKETS_SUPPORTED][L3_SCRUBBER_CONTEXT_ARRAY_SIZE];
  L3_FEATURE_FAMILY_SERVICES  *FamilyServices[MAX_SOCKETS_SUPPORTED];

  AgesaStatus     = AGESA_SUCCESS;
  HtAssistEnabled = TRUE;
  AtmModeEnabled  = TRUE;

  IDS_HDT_CONSOLE (CPU_TRACE, "    Enabling L3 dependent features\n");

  // There are many family service call outs.  Initialize the family service array while
  // cache is still enabled.
  for (Socket = 0; Socket < MAX_SOCKETS_SUPPORTED; Socket++) {
    if (IsProcessorPresent (Socket, StdHeader)) {
      GetFeatureServicesOfSocket (&L3FeatureFamilyServiceTable, Socket, (CONST VOID **)&FamilyServices[Socket], StdHeader);
    } else {
      FamilyServices[Socket] = NULL;
    }
  }

  if (EntryPoint == CPU_FEAT_AFTER_POST_MTRR_SYNC) {
    // Check for optimal settings
    GetApMailbox (&ApMailboxes.ApMailInfo.Info, StdHeader);
    CpuCount = GetNumberOfProcessors (StdHeader);
    if (((CpuCount == 1) && (ApMailboxes.ApMailInfo.Fields.ModuleType == 1)) ||
        ((CpuCount == 2) && (ApMailboxes.ApMailInfo.Fields.ModuleType == 0))) {
      for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
        // Only check for non-optimal HT Assist setting is if's supported.
        if ((FamilyServices[Socket] != NULL) &&
            (FamilyServices[Socket]->IsHtAssistSupported (FamilyServices[Socket], PlatformConfig, StdHeader))) {
          if (FamilyServices[Socket]->IsNonOptimalConfig (FamilyServices[Socket], Socket, StdHeader)) {
            // Non-optimal settings.  Log an event.
            AgesaStatus = AGESA_WARNING;
            PutEventLog (AgesaStatus, CPU_WARNING_NONOPTIMAL_HT_ASSIST_CFG, 0, 0, 0, 0, StdHeader);
            break;
          }
        }
      }
    }
  } else {
    // Disable the scrubbers.
    for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
      if (FamilyServices[Socket] != NULL) {
        FamilyServices[Socket]->GetL3ScrubCtrl (FamilyServices[Socket], Socket, &Scrubbers[Socket][0], StdHeader);

        // If any node in the system does not support Probe Filter, disable it on the system
        if (!FamilyServices[Socket]->IsHtAssistSupported (FamilyServices[Socket], PlatformConfig, StdHeader)) {
          HtAssistEnabled = FALSE;
        }
        // If any node in the system does not support ATM mode, disable it on the system
        if (!FamilyServices[Socket]->IsAtmModeSupported (FamilyServices[Socket], PlatformConfig, StdHeader)) {
          AtmModeEnabled = FALSE;
        }
      }
    }

    // Wait for 40us
    WaitMicroseconds ((UINT32) 40, StdHeader);

    // Run DisableAllCaches on AP cores.
    ApParams.StdHeader = *StdHeader;
    ApParams.FunctionNumber = AP_LATE_TASK_DISABLE_CACHE;
    ApParams.RelatedDataBlock = (VOID *) &HtAssistEnabled;
    ApParams.RelatedBlockLength = sizeof (BOOLEAN);
    RunLateApTaskOnAllAPs (&ApParams, StdHeader);

    // Run DisableAllCaches on core 0.
    DisableAllCaches (&ApParams);

    // Family hook before initialization.
    for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
      if (FamilyServices[Socket] != NULL) {
        FamilyServices[Socket]->HookBeforeInit (FamilyServices[Socket], Socket, StdHeader);
      }
    }

    // Activate Probe Filter & ATM mode.
    for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
      if (FamilyServices[Socket] != NULL) {
        if (HtAssistEnabled) {
          FamilyServices[Socket]->HtAssistInit (FamilyServices[Socket], Socket, StdHeader);
        }
        if (AtmModeEnabled) {
          FamilyServices[Socket]->AtmModeInit (FamilyServices[Socket], Socket, StdHeader);
        }
      }
    }

    // Family hook after initialization.
    for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
      if (FamilyServices[Socket] != NULL) {
        FamilyServices[Socket]->HookAfterInit (FamilyServices[Socket], Socket, StdHeader);
      }
    }

    // Run EnableAllCaches on core 0.
    EnableAllCaches (&ApParams);

    // Run EnableAllCaches on every core.
    ApParams.FunctionNumber = AP_LATE_TASK_ENABLE_CACHE;
    RunLateApTaskOnAllAPs (&ApParams, StdHeader);

    // Restore the scrubbers.
    for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
      if (FamilyServices[Socket] != NULL) {
        FamilyServices[Socket]->SetL3ScrubCtrl (FamilyServices[Socket], Socket, &Scrubbers[Socket][0], StdHeader);
      }
    }
  }

  return AgesaStatus;
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * Disable all the caches on current core.
 *
 * @param[in] ApExeParams   Handle to config for library and services.
 *
 * @retval    AGESA_SUCCESS  Always succeeds.
 *
 */
AGESA_STATUS
DisableAllCaches (
  IN       AP_EXE_PARAMS *ApExeParams
  )
{
  UINT32   CR0Data;
  L3_FEATURE_FAMILY_SERVICES  *FamilyServices;

  // Disable cache through CR0.
  LibAmdReadCpuReg (0, &CR0Data);
  CR0Data |= (0x60000000);
  LibAmdWriteCpuReg (0, CR0Data);

  // Execute wbinvd
  LibAmdWriteBackInvalidateCache ();

  GetFeatureServicesOfCurrentCore (&L3FeatureFamilyServiceTable, (CONST VOID **)&FamilyServices, &ApExeParams->StdHeader);

  FamilyServices->HookDisableCache (FamilyServices, *(BOOLEAN *) ApExeParams->RelatedDataBlock, &ApExeParams->StdHeader);

  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * Enable all the caches on current core.
 *
 * @param[in] ApExeParams    Handle to config for library and services.
 *
 * @retval    AGESA_SUCCESS  Always succeeds.
 *
 */
AGESA_STATUS
EnableAllCaches (
  IN       AP_EXE_PARAMS *ApExeParams
  )
{
  UINT32   CR0Data;
  L3_FEATURE_FAMILY_SERVICES  *FamilyServices;

  // Enable cache through CR0.
  LibAmdReadCpuReg (0, &CR0Data);
  CR0Data &= ~(0x60000000);
  LibAmdWriteCpuReg (0, CR0Data);

  GetFeatureServicesOfCurrentCore (&L3FeatureFamilyServiceTable, (CONST VOID **)&FamilyServices, &ApExeParams->StdHeader);

  FamilyServices->HookEnableCache (FamilyServices, &ApExeParams->StdHeader);

  return AGESA_SUCCESS;
}

CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuL3Features =
{
  L3Features,
  (CPU_FEAT_AFTER_POST_MTRR_SYNC | CPU_FEAT_INIT_MID_END | CPU_FEAT_S3_LATE_RESTORE_END),
  IsL3FeatureEnabled,
  InitializeL3Feature
};
