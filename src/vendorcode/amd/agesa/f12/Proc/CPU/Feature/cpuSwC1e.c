/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA CPU SW C1e feature support code.
 *
 * Contains code that declares the AGESA CPU C1e related APIs
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Feature
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
#include "cpuSwC1e.h"
#include "cpuHwC1e.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FEATURE_CPUSWC1E_FILECODE
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
extern CPU_FAMILY_SUPPORT_TABLE SwC1eFamilyServiceTable;

/*---------------------------------------------------------------------------------------*/
/**
 *  Should software C1e be enabled
 *
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               SW C1e is supported.
 * @retval       FALSE              SW C1e not supported.
 *
 */
BOOLEAN
STATIC
IsSwC1eFeatureEnabled (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  BOOLEAN                IsEnabled;
  BOOLEAN                IsOtherC1eEnabled;
  AP_MAILBOXES           ApMailboxes;
  SW_C1E_FAMILY_SERVICES *SwFamilyServices;

  ASSERT (PlatformConfig->C1eMode < MaxC1eMode);
  IsEnabled = FALSE;

  // Check whether software C1e is enabled only if other C1e methods is/are not supported
  // or if the platform specifically uses C1eModeSoftwareDeprecated.
  IsOtherC1eEnabled = (IsFeatureEnabled (HardwareC1e, PlatformConfig, StdHeader) ||
                       IsFeatureEnabled (MsgBasedC1e, PlatformConfig, StdHeader));
  if ((PlatformConfig->C1eMode == C1eModeSoftwareDeprecated) ||
     ((!IsOtherC1eEnabled) && ((PlatformConfig->C1eMode == C1eModeHardwareSoftwareDeprecated) || (PlatformConfig->C1eMode == C1eModeAuto)))) {
    ASSERT ((PlatformConfig->C1ePlatformData1 < 0x10000) && (PlatformConfig->C1ePlatformData1 != 0));
    ASSERT (PlatformConfig->C1ePlatformData2 < 0x100);
    if ((PlatformConfig->C1ePlatformData1 != 0) && (PlatformConfig->C1ePlatformData1 < 0xFFFE) && (PlatformConfig->C1ePlatformData2 < 0xFF)) {
      if (!IsNonCoherentHt1 (StdHeader)) {
        if (GetNumberOfProcessors (StdHeader) == 1) {
          GetApMailbox (&ApMailboxes.ApMailInfo.Info, StdHeader);
          if (ApMailboxes.ApMailInfo.Fields.ModuleType == 0) {
            GetFeatureServicesOfCurrentCore (&SwC1eFamilyServiceTable, (const VOID **)&SwFamilyServices, StdHeader);
            if (SwFamilyServices != NULL) {
              IsEnabled = SwFamilyServices->IsSwC1eSupported (SwFamilyServices, StdHeader);
            }
          }
        }
      }
    }
  }
  return IsEnabled;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable Software C1e
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
InitializeSwC1eFeature (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  AGESA_STATUS AgesaStatus;
  SW_C1E_FAMILY_SERVICES *FamilyServices;

  AgesaStatus = AGESA_SUCCESS;

  IDS_HDT_CONSOLE (CPU_TRACE, "    SW C1e is enabled\n");

  if (IsWarmReset (StdHeader)) {
    GetFeatureServicesOfCurrentCore (&SwC1eFamilyServiceTable, (const VOID **)&FamilyServices, StdHeader);
    AgesaStatus = FamilyServices->InitializeSwC1e (FamilyServices, EntryPoint, PlatformConfig, StdHeader);
  }

  return AgesaStatus;
}

CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureSwC1e =
{
  SoftwareC1e,
  CPU_FEAT_AFTER_PM_INIT,
  IsSwC1eFeatureEnabled,
  InitializeSwC1eFeature
};