/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA CPU create Pstate HPC mode support code.
 *
 * Contains code that declares the AGESA CPU Pstate HPC mode related APIs
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
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuApicUtilities.h"
#include "OptionMultiSocket.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuFeatures.h"
#include "cpuPstateHpcMode.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FEATURE_CPUPSTATEHPCMODE_FILECODE

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
EnablePstateHpcModeOnAps (
  IN       VOID *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE PstateHpcModeFamilyServiceTable;

/*---------------------------------------------------------------------------------------*/
/**
 *  Should P-state HPC mode be enabled
 *  If PlatformConfig->PStatesInHpcMode is TRUE, return TRUE, otherwise reture FALSE
 *
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               P-state HPC mode is supported.
 * @retval       FALSE              P-state HPC mode cannot be enabled.
 *
 */
BOOLEAN
STATIC
IsPstateHpcModeFeatureSupported (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  BOOLEAN          IsEnabled;
  UINT32           Socket;
  PSTATE_HPC_MODE_FAMILY_SERVICES *FamilyServices;

  IsEnabled = TRUE;

  if (PlatformConfig->PStatesInHpcMode) {
    for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
      if (IsProcessorPresent (Socket, StdHeader)) {
        GetFeatureServicesOfSocket (&PstateHpcModeFamilyServiceTable, Socket, (CONST VOID **)&FamilyServices, StdHeader);
        if (FamilyServices == NULL) {
          IsEnabled = FALSE;
          break;
        }
      }
    }
  } else {
    IsEnabled = FALSE;
  }
  return IsEnabled;
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Enable high performance computing (HPC mode)
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
InitializePstateHpcModeFeature (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32       BscSocket;
  UINT32       Ignored;
  UINT32       BscCoreNum;
  UINT32       Core;
  UINT32       Socket;
  UINT32       NumberOfSockets;
  UINT32       NumberOfCores;
  AP_TASK      TaskPtr;
  AGESA_STATUS IgnoredSts;

  if (!IsWarmReset (StdHeader)) {
    IDS_HDT_CONSOLE (CPU_TRACE, "    P-state HPC mode is enabled\n");

    IdentifyCore (StdHeader, &BscSocket, &Ignored, &BscCoreNum, &IgnoredSts);
    NumberOfSockets = GetPlatformNumberOfSockets ();

    TaskPtr.FuncAddress.PfApTaskI = EnablePstateHpcModeOnAps;
    TaskPtr.DataTransfer.DataSizeInDwords = 2;
    TaskPtr.DataTransfer.DataPtr = PlatformConfig;
    TaskPtr.DataTransfer.DataTransferFlags = 0;
    TaskPtr.ExeFlags = WAIT_FOR_CORE;

    for (Socket = 0; Socket < NumberOfSockets; Socket++) {
      if (GetActiveCoresInGivenSocket (Socket, &NumberOfCores, StdHeader)) {
        for (Core = 0; Core < NumberOfCores; Core++) {
          if ((Socket != BscSocket) || (Core != BscCoreNum)) {
            ApUtilRunCodeOnSocketCore ((UINT8) Socket, (UINT8) Core, &TaskPtr, StdHeader);
          }
        }
      }
    }

    EnablePstateHpcModeOnAps (PlatformConfig, StdHeader);
  }
  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  AP task to enable Pstate HPC mode
 *
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 */
VOID
STATIC
EnablePstateHpcModeOnAps (
  IN       VOID *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  PSTATE_HPC_MODE_FAMILY_SERVICES *FamilyServices;

  GetFeatureServicesOfCurrentCore (&PstateHpcModeFamilyServiceTable, (CONST VOID **)&FamilyServices, StdHeader);
  FamilyServices->EnablePstateHpcMode (FamilyServices,
                                       PlatformConfig,
                                       StdHeader);

}


CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeaturePstateHpcMode =
{
  PstateHpcMode,
  (CPU_FEAT_BEFORE_RELINQUISH_AP | CPU_FEAT_AFTER_RESUME_MTRR_SYNC),
  IsPstateHpcModeFeatureSupported,
  InitializePstateHpcModeFeature
};
