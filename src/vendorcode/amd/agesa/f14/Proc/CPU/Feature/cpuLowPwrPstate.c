/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA CPU create low power P-state for PROCHOT_L throttling support code.
 *
 * Contains code that declares the AGESA CPU low power P-state related APIs
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Feature
 * @e \$Revision: 35155 $   @e \$Date: 2010-07-16 17:11:52 +0800 (Fri, 16 Jul 2010) $
 *
 */
/*
 *****************************************************************************
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
 * ***************************************************************************
 *
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "cpuFamilyTranslation.h"
#include "OptionMultiSocket.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuFeatures.h"
#include "cpuLowPwrPstate.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FEATURE_CPULOWPWRPSTATE_FILECODE

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
EnableLowPwrPstateOnSocket (
  IN       VOID *EntryPoint,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE LowPwrPstateFamilyServiceTable;
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;

/*---------------------------------------------------------------------------------------*/
/**
 *  Should Low Power P-state be enabled
 *  If all processors support Low Power P-state, reture TRUE, otherwise reture FALSE
 *
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               Low Power P-state is supported.
 * @retval       FALSE              Low Power P-state cannot be enabled.
 *
 */
BOOLEAN
STATIC
IsLowPwrPstateFeatureSupported (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32              Socket;
  BOOLEAN             IsSupported;
  LOW_PWR_PSTATE_FAMILY_SERVICES *FamilyServices;

  IsSupported = FALSE;
  for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
    if (IsProcessorPresent (Socket, StdHeader)) {
      GetFeatureServicesOfSocket (&LowPwrPstateFamilyServiceTable, Socket, &FamilyServices, StdHeader);
      if (FamilyServices != NULL) {
        if (FamilyServices->IsLowPwrPstateSupported (FamilyServices, PlatformConfig, Socket, StdHeader)) {
          IsSupported = TRUE;
        } else {
          IsSupported = FALSE;
          break;
        }
      } else {
        IsSupported = FALSE;
        break;
      }
    }
  }
  IDS_OPTION_HOOK (IDS_LOW_POWER_PSTATE, &IsSupported, StdHeader);
  return IsSupported;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable low power P-state
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
InitializeLowPwrPstateFeature (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  AP_TASK      TaskPtr;
  AMD_CPU_EARLY_PARAMS CpuEarlyParams;

  IDS_HDT_CONSOLE (CPU_TRACE, "    Low pwr P-state is enabled\n");

  CpuEarlyParams.PlatformConfig = *PlatformConfig;

  TaskPtr.FuncAddress.PfApTaskIC = EnableLowPwrPstateOnSocket;
  TaskPtr.DataTransfer.DataSizeInDwords = 2;
  TaskPtr.DataTransfer.DataPtr = &EntryPoint;
  TaskPtr.DataTransfer.DataTransferFlags = 0;
  TaskPtr.ExeFlags = PASS_EARLY_PARAMS;
  OptionMultiSocketConfiguration.BscRunCodeOnAllSystemCore0s (&TaskPtr, StdHeader, &CpuEarlyParams);

  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  'Local' core 0 task to enable low power P-state
 *
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    StdHeader          Config Handle for library, services.
 * @param[in]    CpuEarlyParams     Service parameters.
 *
 */
VOID
STATIC
EnableLowPwrPstateOnSocket (
  IN       VOID *EntryPoint,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams
  )
{
  LOW_PWR_PSTATE_FAMILY_SERVICES *FamilyServices;

  GetFeatureServicesOfCurrentCore (&LowPwrPstateFamilyServiceTable, &FamilyServices, StdHeader);
  FamilyServices->EnableLowPwrPstate (FamilyServices,
                                         &CpuEarlyParams->PlatformConfig,
                                         *((UINT64 *) EntryPoint),
                                         StdHeader);
}


CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureLowPwrPstate =
{
  LowPwrPstate,
  CPU_FEAT_AFTER_PM_INIT,
  IsLowPwrPstateFeatureSupported,
  InitializeLowPwrPstateFeature
};
