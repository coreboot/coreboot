/**
 * @file
 *
 * AMD AGESA Basic Level Public APIs
 *
 * Contains basic Level Initialization routines.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Interface
 * @e \$Revision: 7735 $   @e \$Date: 2008-08-27 14:49:19 -0500 (Wed, 27 Aug 2008) $
 *
 */
/*****************************************************************************
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
#include "Ids.h"
#include "cpuEnvInit.h"
#include "heapManager.h"
#include "CreateStruct.h"
#include "GnbInterface.h"
#include "CommonInits.h"
#include "S3SaveState.h"
#include "Filecode.h"
#define FILECODE PROC_COMMON_AMDINITENV_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/*
 *---------------------------------------------------------------------------------------
 *
 *  Initializer routine that will be invoked by the wrapper
 *  to initialize the input structure for the AmdInitEnv
 *
 *  @param[in,out]  EnvParamsPtr   Newly created interface parameters for AmdInitEnv
 *
 *  @retval         AGESA_SUCCESS  Always succeeds
 *
 *---------------------------------------------------------------------------------------
 */
AGESA_STATUS
AmdInitEnvInitializer (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_ENV_PARAMS    *EnvParamsPtr
  )
{
  ASSERT (StdHeader != NULL);
  ASSERT (EnvParamsPtr != NULL);

  EnvParamsPtr->StdHeader = *StdHeader;

  CommonPlatformConfigInit (&EnvParamsPtr->PlatformConfig, &EnvParamsPtr->StdHeader);

  return AGESA_SUCCESS;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Main entry point for the AMD_INIT_ENV function.
 *
 * This entry point is responsible for copying the heap contents from the
 * temp RAM area to main memory.
 *
 * @param[in,out] EnvParams         Required input parameters for the AMD_INIT_ENV
 *                                  entry point.
 *
 * @return        Aggregated status across all internal AMD env calls invoked.
 *
 */
AGESA_STATUS
AmdInitEnv (
  IN OUT   AMD_ENV_PARAMS  *EnvParams
  )
{
  AGESA_STATUS  AgesaStatus;
  AGESA_STATUS  AmdInitEnvStatus;

  AGESA_TESTPOINT (TpIfAmdInitEnvEntry, &EnvParams->StdHeader);

  ASSERT (EnvParams != NULL);
  AmdInitEnvStatus = AGESA_SUCCESS;

  IDS_OPTION_HOOK (IDS_BEFORE_PCI_INIT, EnvParams, &(EnvParams->StdHeader));

  //Copy Temp Ram heap content to Main Ram
  AgesaStatus = CopyHeapToMainRamAtPost (&(EnvParams->StdHeader));
  if (AgesaStatus > AmdInitEnvStatus) {
    AmdInitEnvStatus = AgesaStatus;
  }
  EnvParams->StdHeader.HeapStatus = HEAP_SYSTEM_MEM;

  AgesaStatus = S3ScriptInit (&EnvParams->StdHeader);
  if (AgesaStatus > AmdInitEnvStatus) {
    AmdInitEnvStatus = AgesaStatus;
  }

  AgesaStatus = GnbInitAtEnv (&EnvParams->StdHeader);
  if (AgesaStatus > AmdInitEnvStatus) {
    AmdInitEnvStatus = AgesaStatus;
  }

  AGESA_TESTPOINT (TpIfAmdInitEnvExit, &EnvParams->StdHeader);
  return  AmdInitEnvStatus;
}


