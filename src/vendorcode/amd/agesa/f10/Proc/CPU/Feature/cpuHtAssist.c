/**
 * @file
 *
 * AMD CPU HT Assist Initialization functions.
 *
 * Contains code for doing probe filter initialization.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 7284 $   @e \$Date: 2008-08-08 23:29:33 +0800 (Fri, 08 Aug 2008) $
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
#include "Topology.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuLateInit.h"
#include "cpuFamilyTranslation.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFeatures.h"
#include "cpuHtAssist.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FEATURE_CPUHTASSIST_FILECODE
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
extern CPU_FAMILY_SUPPORT_TABLE HtAssistFamilyServiceTable;


/*---------------------------------------------------------------------------------------*/
/**
 *  Should HT Assist be enabled
 *
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               HT Assist is supported.
 * @retval       FALSE              HT Assist cannot be enabled.
 *
 */
BOOLEAN
STATIC
IsHtAssistEnabled (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  BOOLEAN          IsEnabled;
  UINT32           CpuCount;
  UINT32           Socket;
  AP_MAILBOXES     ApMailboxes;
  HT_ASSIST_FAMILY_SERVICES *FamilyServices;

  IsEnabled = FALSE;
  if (PlatformConfig->PlatformProfile.UseHtAssist) {
    CpuCount = GetNumberOfProcessors (StdHeader);
    ASSERT (CpuCount != 0);

    if (CpuCount == 1) {
      GetApMailbox (&ApMailboxes.ApMailInfo.Info, StdHeader);
      if (ApMailboxes.ApMailInfo.Fields.ModuleType != 0) {
        IsEnabled = TRUE;
      }
    } else if (CpuCount > 1) {
      IsEnabled = TRUE;
    }
    if (IsEnabled) {
      for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
        if (IsProcessorPresent (Socket, StdHeader)) {
          GetFeatureServicesOfSocket (&HtAssistFamilyServiceTable, Socket, (CONST VOID **)&FamilyServices, StdHeader);
          if ((FamilyServices == NULL) || !FamilyServices->IsHtAssistSupported (FamilyServices, Socket, StdHeader)) {
            IsEnabled = FALSE;
            break;
          }
        }
      }
    }
  }
  return IsEnabled;
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Enable the HT Assist feature.
 *
 * HT Assist initialization requires the following series of steps.
 * -# Disable Cache on @b all cores.
 * -# Initialize Probe Filter PCI regs
 *    -# Save L3 Scrub Rate
 *    -# On each node:
 *       -# Turn off L3Scrubber and Disable L3 cache
 *    -# On each node:
 *       -# Enable probe filter
 *    -# On each node:
 *       -# Enable L3 cache and turn on Scrubber.
 *    -# Restore L3 Scrub Rate
 * -# Enable Cache on @b all cores.
 *
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       AGESA_SUCCESS      HT Assist feature is running optimally.
 * @retval       AGESA_WARNING      HT Assist feature is not running optimally.
 *
 */
AGESA_STATUS
STATIC
InitializeHtAssistFeature (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32                     CpuCount;
  UINT32                     Socket;
  AGESA_STATUS               AgesaStatus;
  AP_MAILBOXES               ApMailboxes;
  AP_EXE_PARAMS              ApParams;
  UINT32                     Scrubbers[MAX_SOCKETS_SUPPORTED][L3_SCRUBBER_CONTEXT_ARRAY_SIZE];
  HT_ASSIST_FAMILY_SERVICES  *FamilyServices[MAX_SOCKETS_SUPPORTED];

  AgesaStatus = AGESA_SUCCESS;

  // There are many family service call outs.  Initialize the family service array while
  // cache is still enabled.
  for (Socket = 0; Socket < MAX_SOCKETS_SUPPORTED; Socket++) {
    if (IsProcessorPresent (Socket, StdHeader)) {
      GetFeatureServicesOfSocket (&HtAssistFamilyServiceTable, Socket, (CONST VOID **)&FamilyServices[Socket], StdHeader);
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
        if (FamilyServices[Socket] != NULL) {
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
      }
    }

    // Wait for 40us
    WaitMicroseconds ((UINT32) 40, StdHeader);

    // Run DisableAllCaches on AP cores.
    ApParams.StdHeader = *StdHeader;
    ApParams.FunctionNumber = AP_LATE_TASK_DISABLE_CACHE;
    ApParams.RelatedDataBlock = NULL;
    ApParams.RelatedBlockLength = 0;
    RunLateApTaskOnAllAPs (&ApParams, StdHeader);

    // Run DisableAllCaches on core 0.
    DisableAllCaches (&ApParams);

    // Family hook before initialization.
    for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
      if (FamilyServices[Socket] != NULL) {
        FamilyServices[Socket]->HookBeforeInit (FamilyServices[Socket], Socket, StdHeader);
      }
    }

    // Activate probe filter.
    for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
      if (FamilyServices[Socket] != NULL) {
        FamilyServices[Socket]->HtAssistInit (FamilyServices[Socket], Socket, StdHeader);
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
  HT_ASSIST_FAMILY_SERVICES  *FamilyServices;

  GetFeatureServicesOfCurrentCore (&HtAssistFamilyServiceTable, (CONST VOID **)&FamilyServices, &ApExeParams->StdHeader);

  FamilyServices->HookDisableCache (FamilyServices, &ApExeParams->StdHeader);

  // Disable cache through CR0.
  LibAmdReadCpuReg (0, &CR0Data);
  CR0Data |= (0x60000000);
  LibAmdWriteCpuReg (0, CR0Data);

  // Execute wbinvd
  LibAmdWriteBackInvalidateCache ();

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
  HT_ASSIST_FAMILY_SERVICES  *FamilyServices;

  // Enable cache through CR0.
  LibAmdReadCpuReg (0, &CR0Data);
  CR0Data &= ~(0x60000000);
  LibAmdWriteCpuReg (0, CR0Data);

  GetFeatureServicesOfCurrentCore (&HtAssistFamilyServiceTable, (CONST VOID **)&FamilyServices, &ApExeParams->StdHeader);

  FamilyServices->HookEnableCache (FamilyServices, &ApExeParams->StdHeader);

  return AGESA_SUCCESS;
}

CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureHtAssist =
{
  HtAssist,
  (CPU_FEAT_AFTER_POST_MTRR_SYNC | CPU_FEAT_INIT_MID_END | CPU_FEAT_S3_LATE_RESTORE_END),
  IsHtAssistEnabled,
  InitializeHtAssistFeature
};
