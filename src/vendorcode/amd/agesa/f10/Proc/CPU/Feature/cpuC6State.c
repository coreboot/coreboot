/**
 * @file
 *
 * AMD AGESA CPU C6 feature support code.
 *
 * Contains code that declares the AGESA CPU C6 related APIs
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
#include "cpuFamilyTranslation.h"
#include "OptionMultiSocket.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "cpuFeatures.h"
#include "cpuC6State.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FEATURE_CPUC6STATE_FILECODE

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
EnableC6OnSocket (
  IN       VOID *EntryPoint,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE C6FamilyServiceTable;
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;

/*---------------------------------------------------------------------------------------*/
/**
 *  Should C6 be enabled
 *
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               C6 is supported.
 * @retval       FALSE              C6 cannot be enabled.
 *
 */
BOOLEAN
STATIC
IsC6FeatureEnabled (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32             Socket;
  BOOLEAN            IsEnabled;
  C6_FAMILY_SERVICES *FamilyServices;

  IsEnabled = FALSE;
  if (PlatformConfig->CStateMode == CStateModeC6) {
    ASSERT (PlatformConfig->CStatePlatformData < 0x10000);
    ASSERT (PlatformConfig->CStatePlatformData != 0);
    if ((PlatformConfig->CStatePlatformData != 0) && (PlatformConfig->CStatePlatformData <= 0xFFF8)) {
      IsEnabled = TRUE;
      for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
        if (IsProcessorPresent (Socket, StdHeader)) {
          GetFeatureServicesOfSocket (&C6FamilyServiceTable, Socket, (CONST VOID **)&FamilyServices, StdHeader);
          if ((FamilyServices == NULL) || !FamilyServices->IsC6Supported (FamilyServices, Socket, StdHeader)) {
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
 *  Enable the C6 C-state
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
InitializeC6Feature (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  AP_TASK      TaskPtr;
  AMD_CPU_EARLY_PARAMS CpuEarlyParams;

  CpuEarlyParams.PlatformConfig = *PlatformConfig;

  TaskPtr.FuncAddress.PfApTaskIC = EnableC6OnSocket;
  TaskPtr.DataTransfer.DataSizeInDwords = 2;
  TaskPtr.DataTransfer.DataPtr = &EntryPoint;
  TaskPtr.DataTransfer.DataTransferFlags = 0;
  TaskPtr.ExeFlags = PASS_EARLY_PARAMS;
  OptionMultiSocketConfiguration.BscRunCodeOnAllSystemCore0s (&TaskPtr, StdHeader, &CpuEarlyParams);

  return AGESA_SUCCESS;
}


/*---------------------------------------------------------------------------------------*/
/**
 *  'Local' core 0 task to enable C6 on it's socket.
 *
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    StdHeader          Config Handle for library, services.
 * @param[in]    CpuEarlyParams     Service parameters.
 *
 */
VOID
STATIC
EnableC6OnSocket (
  IN       VOID *EntryPoint,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams
  )
{
  C6_FAMILY_SERVICES *FamilyServices;

  GetFeatureServicesOfCurrentCore (&C6FamilyServiceTable, (CONST VOID **)&FamilyServices, StdHeader);
  FamilyServices->InitializeC6 (FamilyServices,
                                *((UINT64 *) EntryPoint),
                                &CpuEarlyParams->PlatformConfig,
                                StdHeader);
}


CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureC6State =
{
  C6Cstate,
  (CPU_FEAT_AFTER_PM_INIT | CPU_FEAT_AFTER_POST_MTRR_SYNC | CPU_FEAT_AFTER_RESUME_MTRR_SYNC),
  IsC6FeatureEnabled,
  InitializeC6Feature
};
