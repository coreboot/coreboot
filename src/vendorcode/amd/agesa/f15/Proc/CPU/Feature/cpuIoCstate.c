/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA CPU IO Cstate function declarations.
 *
 * Contains code that declares the AGESA CPU IO Cstate related APIs
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Feature
 * @e \$Revision: 56279 $   @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
#include "cpuFeatures.h"
#include "cpuIoCstate.h"
#include "cpuServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuApicUtilities.h"
#include "OptionMultiSocket.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FEATURE_CPUIOCSTATE_FILECODE

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
EnableIoCstateOnSocket (
  IN       VOID *EntryPoint,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams
  );
/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE    IoCstateFamilyServiceTable;
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;

/*---------------------------------------------------------------------------------------*/
/**
 *  Should IO Cstate be enabled
 *  If all processors support IO Cstate, return TRUE. Otherwise, return FALSE
 *
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               IO Cstate is supported.
 * @retval       FALSE              IO Cstate cannot be enabled.
 *
 */
BOOLEAN
STATIC
IsIoCstateFeatureSupported (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32                    Socket;
  BOOLEAN                   IsSupported;
  IO_CSTATE_FAMILY_SERVICES *IoCstateServices;

  IsSupported = FALSE;
  if ((PlatformConfig->CStateIoBaseAddress != 0) && (PlatformConfig->CStateIoBaseAddress <= 0xFFF8)) {
    for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
      if (IsProcessorPresent (Socket, StdHeader)) {
        GetFeatureServicesOfSocket (&IoCstateFamilyServiceTable, Socket, (CONST VOID **)&IoCstateServices, StdHeader);
        if (IoCstateServices != NULL) {
          if (IoCstateServices->IsIoCstateSupported (IoCstateServices, Socket, StdHeader)) {
            IsSupported = TRUE;
          } else {
            // Stop checking remaining socket(s) once we find one that does not support IO Cstates
            IsSupported = FALSE;
            break;
          }
        } else {
          // Exit the for loop if we found a socket that does not have the IO Cstates feature installed
          IsSupported = FALSE;
          break;
        }
      }
    }
  }
  return IsSupported;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable IO Cstate feature
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
InitializeIoCstateFeature (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  AP_TASK TaskPtr;
  AMD_CPU_EARLY_PARAMS CpuEarlyParams;

  IDS_HDT_CONSOLE (CPU_TRACE, "    IO C-state is enabled\n");

  CpuEarlyParams.PlatformConfig = *PlatformConfig;

  TaskPtr.FuncAddress.PfApTaskIC = EnableIoCstateOnSocket;
  TaskPtr.DataTransfer.DataSizeInDwords = 2;
  TaskPtr.DataTransfer.DataPtr = &EntryPoint;
  TaskPtr.DataTransfer.DataTransferFlags = 0;
  TaskPtr.ExeFlags = PASS_EARLY_PARAMS;
  OptionMultiSocketConfiguration.BscRunCodeOnAllSystemCore0s (&TaskPtr, StdHeader, &CpuEarlyParams);

  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  'Local' core 0 task to enable IO Cstate on it's socket.
 *
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    StdHeader          Config Handle for library, services.
 * @param[in]    CpuEarlyParams     Service parameters.
 *
 */
VOID
STATIC
EnableIoCstateOnSocket (
  IN       VOID *EntryPoint,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams
  )
{
  IO_CSTATE_FAMILY_SERVICES *FamilyServices;

  GetFeatureServicesOfCurrentCore (&IoCstateFamilyServiceTable, (CONST VOID **)&FamilyServices, StdHeader);
  FamilyServices->InitializeIoCstate (FamilyServices,
                                     *((UINT64 *) EntryPoint),
                                     &CpuEarlyParams->PlatformConfig,
                                     StdHeader);
}

CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeatureIoCstate =
{
  IoCstate,
  (CPU_FEAT_AFTER_PM_INIT),
  IsIoCstateFeatureSupported,
  InitializeIoCstateFeature
};
