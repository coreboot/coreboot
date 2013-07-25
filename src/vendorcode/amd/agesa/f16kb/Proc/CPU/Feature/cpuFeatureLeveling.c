/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Feature Leveling Function.
 *
 * Contains code to Level the Feature in a multi-socket system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
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
 *----------------------------------------------------------------------------
 */


/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuRegisters.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "cpuPostInit.h"
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FEATURE_CPUFEATURELEVELING_FILECODE
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

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
VOID
STATIC
SaveFeatures (
  IN OUT   VOID *cpuFeatureListPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
WriteFeatures (
  IN OUT   VOID *cpuFeatureListPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
GetGlobalCpuFeatureListAddress (
     OUT   UINT64            **Address,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          P U B L I C     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  FeatureLeveling
 *
 *    CPU feature leveling. Set least common features set of all CPUs
 *
 *    @param[in,out]   StdHeader   - Pointer to AMD_CONFIG_PARAMS struct.
 *
 */
VOID
FeatureLeveling (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 BscSocket;
  UINT32 Ignored;
  UINT32 BscCoreNum;
  UINT32 Socket;
  UINT32 Core;
  UINT32 NumberOfSockets;
  UINT32 NumberOfCores;
  BOOLEAN *FirstTime;
  BOOLEAN *NeedLeveling;
  AGESA_STATUS IgnoredSts;
  CPU_FEATURES_LIST *globalCpuFeatureList;
  AP_TASK  TaskPtr;

  ASSERT (IsBsp (StdHeader, &IgnoredSts));

  GetGlobalCpuFeatureListAddress ((UINT64 **) &globalCpuFeatureList, StdHeader);
  FirstTime = (BOOLEAN *) ((UINT8 *) globalCpuFeatureList + sizeof (CPU_FEATURES_LIST));
  NeedLeveling = (BOOLEAN *) ((UINT8 *) globalCpuFeatureList + sizeof (CPU_FEATURES_LIST) + sizeof (BOOLEAN));

  *FirstTime = TRUE;
  *NeedLeveling = FALSE;

  LibAmdMemFill (globalCpuFeatureList, 0xFF, sizeof (CPU_FEATURES_LIST), StdHeader);
  IdentifyCore (StdHeader, &BscSocket, &Ignored, &BscCoreNum, &IgnoredSts);
  NumberOfSockets = GetPlatformNumberOfSockets ();

  TaskPtr.FuncAddress.PfApTaskI = SaveFeatures;
  TaskPtr.DataTransfer.DataSizeInDwords = SIZE_IN_DWORDS (CPU_FEATURES_LIST);
  TaskPtr.ExeFlags = WAIT_FOR_CORE;
  TaskPtr.DataTransfer.DataPtr = globalCpuFeatureList;
  TaskPtr.DataTransfer.DataTransferFlags = DATA_IN_MEMORY;

  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (IsProcessorPresent (Socket, StdHeader)) {
      if (Socket !=  BscSocket) {
        ApUtilRunCodeOnSocketCore ((UINT8)Socket, 0, &TaskPtr, StdHeader);
      }
    }
  }
  ApUtilTaskOnExecutingCore (&TaskPtr, StdHeader, NULL);

  if (*NeedLeveling) {
    TaskPtr.FuncAddress.PfApTaskI  = WriteFeatures;
    for (Socket = 0; Socket < NumberOfSockets; Socket++) {
      if (GetActiveCoresInGivenSocket (Socket, &NumberOfCores, StdHeader)) {
        for (Core = 0; Core < NumberOfCores; Core++) {
          if ((Socket != BscSocket) || (Core != BscCoreNum)) {
            ApUtilRunCodeOnSocketCore ((UINT8)Socket, (UINT8)Core, &TaskPtr, StdHeader);
          }
        }
      }
    }
    ApUtilTaskOnExecutingCore (&TaskPtr, StdHeader, NULL);
  }
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L      F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  SaveFeatures
 *
 *    save least common features set of all CPUs
 *
 *    @param[in,out]  cpuFeatureListPtr  - Pointer to CPU Feature List.
 *    @param[in,out]  StdHeader          - Pointer to AMD_CONFIG_PARAMS struct.
 *
 */
VOID
STATIC
SaveFeatures (
  IN OUT   VOID *cpuFeatureListPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  FamilySpecificServices = NULL;

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  FamilySpecificServices->SaveFeatures (FamilySpecificServices, cpuFeatureListPtr, StdHeader);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  WriteFeatures
 *
 *    Write out least common features set of all CPUs
 *
 *    @param[in,out]  cpuFeatureListPtr  - Pointer to CPU Feature List.
 *    @param[in,out]  StdHeader          - Pointer to AMD_CONFIG_PARAMS struct.
 *
 */
VOID
STATIC
WriteFeatures (
  IN OUT   VOID *cpuFeatureListPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  FamilySpecificServices = NULL;

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  FamilySpecificServices->WriteFeatures (FamilySpecificServices, cpuFeatureListPtr, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 *  GetGlobalCpuFeatureListAddress
 *
 *   Determines the address in system DRAM that should be used for CPU feature leveling.
 *
 * @param[out] Address           Address to utilize
 * @param[in]  StdHeader         Config handle for library and services
 *
 *
 */
VOID
STATIC
GetGlobalCpuFeatureListAddress (
     OUT   UINT64            **Address,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 AddressValue;

  AddressValue = GLOBAL_CPU_FEATURE_LIST_TEMP_ADDR;

  *Address = (UINT64 *)(UINT32)(AddressValue);
}
