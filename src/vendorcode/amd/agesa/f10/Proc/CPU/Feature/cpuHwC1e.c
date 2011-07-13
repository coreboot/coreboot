/**
 * @file
 *
 * AMD AGESA CPU HW C1e feature support code.
 *
 * Contains code that declares the AGESA CPU C1e related APIs
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Feature
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "Topology.h"
#include "cpuFeatures.h"
#include "cpuHwC1e.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FEATURE_CPUHWC1E_FILECODE
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

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE HwC1eFamilyServiceTable;

/*---------------------------------------------------------------------------------------*/
/**
 *  Should hardware C1e be enabled
 *
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               HW C1e is supported.
 * @retval       FALSE              HW C1e cannot be enabled.
 *
 */
BOOLEAN
STATIC
IsHwC1eFeatureEnabled (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  BOOLEAN          IsEnabled;
  AP_MAILBOXES     ApMailboxes;
  HW_C1E_FAMILY_SERVICES *FamilyServices;

  ASSERT (PlatformConfig->C1eMode < MaxC1eMode);
  IsEnabled = FALSE;
  if (PlatformConfig->C1eMode == C1eModeHardware) {
    ASSERT (PlatformConfig->C1ePlatformData < 0x10000);
    ASSERT (PlatformConfig->C1ePlatformData != 0);
    if ((PlatformConfig->C1ePlatformData != 0) && (PlatformConfig->C1ePlatformData < 0xFFFE)) {
      if (GetNumberOfProcessors (StdHeader) == 1) {
        GetApMailbox (&ApMailboxes.ApMailInfo.Info, StdHeader);
        if (ApMailboxes.ApMailInfo.Fields.ModuleType == 0) {
          GetFeatureServicesOfCurrentCore (&HwC1eFamilyServiceTable, (CONST VOID **)&FamilyServices, StdHeader);
          if (FamilyServices != NULL) {
            IsEnabled = FamilyServices->IsHwC1eSupported (FamilyServices, StdHeader);
          }
        }
      }
    }
  }
  return IsEnabled;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable Hardware C1e
 *
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @return       The most severe status of any family specific service.
 *
 */
AGESA_STATUS
STATIC
InitializeHwC1eFeature (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  AGESA_STATUS CalledStatus;
  AGESA_STATUS AgesaStatus;
  HW_C1E_FAMILY_SERVICES *FamilyServices;

  AgesaStatus = AGESA_SUCCESS;

  if (IsWarmReset (StdHeader)) {
    GetFeatureServicesOfCurrentCore (&HwC1eFamilyServiceTable, (CONST VOID **)&FamilyServices, StdHeader);
    CalledStatus = FamilyServices->InitializeHwC1e (FamilyServices, EntryPoint, PlatformConfig, StdHeader);
    if (CalledStatus > AgesaStatus) {
      AgesaStatus = CalledStatus;
    }
  }
  return AgesaStatus;
}

CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureHwC1e =
{
  HardwareC1e,
  CPU_FEAT_AFTER_PM_INIT,
  IsHwC1eFeatureEnabled,
  InitializeHwC1eFeature
};
