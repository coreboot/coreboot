/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA CPU Core performance boost feature support code.
 *
 * Contains code that declares the AGESA CPU CPB related APIs
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Feature
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
#include "cpuFamilyTranslation.h"
#include "cpuFeatures.h"
#include "cpuCpb.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FEATURE_CPUCPB_FILECODE

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
extern CPU_FAMILY_SUPPORT_TABLE CpbFamilyServiceTable;

/*---------------------------------------------------------------------------------------*/
/**
 *  Should CPB be enabled
 *
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               CPB is supported.
 * @retval       FALSE              CPB cannot be enabled.
 *
 */
BOOLEAN
STATIC
IsCpbFeatureEnabled (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32              Socket;
  BOOLEAN             IsEnabled;
  CPB_FAMILY_SERVICES *FamilyServices;

  IsEnabled = FALSE;

  ASSERT (PlatformConfig->CpbMode < MaxCpbMode);

  if (PlatformConfig->CpbMode == CpbModeAuto) {
    for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
      if (IsProcessorPresent (Socket, StdHeader)) {
        GetFeatureServicesOfSocket (&CpbFamilyServiceTable, Socket, (CONST VOID **)&FamilyServices, StdHeader);
        if (FamilyServices != NULL) {
          if (FamilyServices->IsCpbSupported (FamilyServices, PlatformConfig, Socket, StdHeader)) {
            IsEnabled = TRUE;
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
 *  Enable core performance boost
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
InitializeCpbFeature (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32              Socket;
  AGESA_STATUS        AgesaStatus;
  AGESA_STATUS        CalledStatus;
  CPB_FAMILY_SERVICES *FamilyServices;

  AgesaStatus = AGESA_SUCCESS;
  CalledStatus = AGESA_SUCCESS;

  IDS_HDT_CONSOLE (CPU_TRACE, "    Boost is enabled\n");

  for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
    if (IsProcessorPresent (Socket, StdHeader)) {
      GetFeatureServicesOfSocket (&CpbFamilyServiceTable, Socket, (CONST VOID **)&FamilyServices, StdHeader);
      if (FamilyServices != NULL) {
        if (FamilyServices->IsCpbSupported (FamilyServices, PlatformConfig, Socket, StdHeader)) {
          CalledStatus = FamilyServices->EnableCpbOnSocket (FamilyServices, PlatformConfig, EntryPoint, Socket, StdHeader);
          if (CalledStatus > AgesaStatus) {
            AgesaStatus = CalledStatus;
          }
        }
      }
    }
  }

  return AgesaStatus;
}


CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureCpb =
{
  CoreBoost,
  (CPU_FEAT_AFTER_PM_INIT | CPU_FEAT_INIT_LATE_END | CPU_FEAT_AFTER_POST_MTRR_SYNC | CPU_FEAT_S3_LATE_RESTORE_END | CPU_FEAT_AFTER_RESUME_MTRR_SYNC),
  IsCpbFeatureEnabled,
  InitializeCpbFeature
};
