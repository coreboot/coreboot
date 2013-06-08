/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA CPU Message-based C1e feature support code.
 *
 * Contains code that declares the AGESA CPU C1e related APIs
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Feature
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "OptionMultiSocket.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuFeatures.h"
#include "cpuMsgBasedC1e.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FEATURE_CPUMSGBASEDC1E_FILECODE
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
STATIC
EnableMsgC1eOnSocket (
  IN       VOID *EntryPoint,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE MsgBasedC1eFamilyServiceTable;
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;

/*---------------------------------------------------------------------------------------*/
/**
 *  Should message-based C1e be enabled
 *
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               Message-based C1e is supported.
 * @retval       FALSE              Message-based C1e cannot be enabled.
 *
 */
BOOLEAN
STATIC
IsMsgBasedC1eFeatureEnabled (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  BOOLEAN          IsEnabled;
  UINT32           Socket;
  MSG_BASED_C1E_FAMILY_SERVICES *FamilyServices;

  ASSERT (PlatformConfig->C1eMode < MaxC1eMode);

  IsEnabled = FALSE;
  if ((PlatformConfig->C1eMode == C1eModeMsgBased) || (PlatformConfig->C1eMode == C1eModeAuto)) {
    ASSERT (PlatformConfig->C1ePlatformData < 0x10000);
    ASSERT (PlatformConfig->C1ePlatformData != 0);
    if ((PlatformConfig->C1ePlatformData != 0) && (PlatformConfig->C1ePlatformData < 0xFFFE)) {
      IsEnabled = TRUE;
      if (IsNonCoherentHt1 (StdHeader)) {
        IsEnabled = FALSE;
      } else {
        for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
          if (IsProcessorPresent (Socket, StdHeader)) {
            GetFeatureServicesOfSocket (&MsgBasedC1eFamilyServiceTable, Socket, (CONST VOID **)&FamilyServices, StdHeader);
            if ((FamilyServices == NULL) || !FamilyServices->IsMsgBasedC1eSupported (FamilyServices, Socket, StdHeader)) {
              IsEnabled = FALSE;
              break;
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
 *  Enable Message-based C1e
 *
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @return       AGESA_SUCCESS      Always succeeds.
 *
 */
AGESA_STATUS
STATIC
InitializeMsgBasedC1eFeature (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  AP_TASK      TaskPtr;
  AMD_CPU_EARLY_PARAMS CpuEarlyParams;

  IDS_HDT_CONSOLE (CPU_TRACE, "    MT C1e is enabled\n");

  if ((EntryPoint != CPU_FEAT_AFTER_PM_INIT) || (IsWarmReset (StdHeader))) {
    CpuEarlyParams.PlatformConfig = *PlatformConfig;

    TaskPtr.FuncAddress.PfApTaskIC = EnableMsgC1eOnSocket;
    TaskPtr.DataTransfer.DataSizeInDwords = 2;
    TaskPtr.DataTransfer.DataPtr = &EntryPoint;
    TaskPtr.DataTransfer.DataTransferFlags = 0;
    TaskPtr.ExeFlags = PASS_EARLY_PARAMS;
    OptionMultiSocketConfiguration.BscRunCodeOnAllSystemCore0s (&TaskPtr, StdHeader, &CpuEarlyParams);
  }
  return AGESA_SUCCESS;
}


/*---------------------------------------------------------------------------------------*/
/**
 *  'Local' core 0 task to enable message-based C1e on it's socket.
 *
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    StdHeader          Config Handle for library, services.
 * @param[in]    CpuEarlyParams     Service parameters.
 *
 */
VOID
STATIC
EnableMsgC1eOnSocket (
  IN       VOID *EntryPoint,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams
  )
{
  MSG_BASED_C1E_FAMILY_SERVICES *FamilyServices;

  GetFeatureServicesOfCurrentCore (&MsgBasedC1eFamilyServiceTable, (CONST VOID **)&FamilyServices, StdHeader);
  FamilyServices->InitializeMsgBasedC1e (FamilyServices,
                                         *((UINT64 *) EntryPoint),
                                         &CpuEarlyParams->PlatformConfig,
                                         StdHeader);
}

CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureMsgBasedC1e =
{
  MsgBasedC1e,
  (CPU_FEAT_AFTER_PM_INIT | CPU_FEAT_AFTER_POST_MTRR_SYNC | CPU_FEAT_AFTER_RESUME_MTRR_SYNC),
  IsMsgBasedC1eFeatureEnabled,
  InitializeMsgBasedC1eFeature
};
