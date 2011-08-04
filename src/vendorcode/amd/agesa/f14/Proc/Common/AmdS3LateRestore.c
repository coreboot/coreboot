/* $NoKeywords:$ */
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
 * @e \$Revision: 38446 $   @e \$Date: 2010-09-24 06:51:03 +0800 (Fri, 24 Sep 2010) $
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
#include "amdlib.h"
#include "Ids.h"
#include "S3.h"
#include "cpuFeatures.h"
#include "S3SaveState.h"
#include "CommonInits.h"
#include "CreateStruct.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_COMMON_AMDS3LATERESTORE_FILECODE
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
AGESA_STATUS
AmdS3LateRestorePlatformConfigInit (
  IN OUT   PLATFORM_CONFIGURATION    *PlatformConfig,
  IN OUT   AMD_CONFIG_PARAMS         *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern BUILD_OPT_CFG UserOptions;

/*---------------------------------------------------------------------------------------*/
/**
 * Main entry point for the AMD_S3LATE_RESTORE function.
 *
 * This entry point is responsible for restoring saved registers and preparing the
 * silicon components for OS restart.
 *
 * @param[in,out] S3LateParams   Required input parameters for the AMD_S3LATE_RESTORE
 *                                  entry point.
 *
 * @return        Aggregated status across all internal AMD S3 late restore calls invoked.
 *
 */
AGESA_STATUS
AmdS3LateRestore (
  IN OUT   AMD_S3LATE_PARAMS *S3LateParams
  )
{
  UINT8  *BufferPointer;
  VOID   *OrMaskPtr;
  VOID   *LateContextPtr;
  AGESA_STATUS ReturnStatus;
  AGESA_STATUS CalledStatus;

  AGESA_TESTPOINT (TpIfAmdS3LateRestoreEntry, &S3LateParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdS3LateRestore: Start\n\n");
  ReturnStatus = AGESA_SUCCESS;

  ASSERT (S3LateParams != NULL);

  BufferPointer = (UINT8 *) S3LateParams->S3DataBlock.VolatileStorage;
  S3LateParams->StdHeader.HeapBasePtr = &BufferPointer[((S3_VOLATILE_STORAGE_HEADER *) S3LateParams->S3DataBlock.VolatileStorage)->HeapOffset];
  ASSERT (S3LateParams->StdHeader.HeapBasePtr != NULL);

  IDS_OPTION_HOOK (IDS_PLATFORMCFG_OVERRIDE, &S3LateParams->PlatformConfig, &S3LateParams->StdHeader);
  IDS_OPTION_HOOK (IDS_BEFORE_S3_RESTORE, S3LateParams, &(S3LateParams->StdHeader));

  if (((S3_VOLATILE_STORAGE_HEADER *) S3LateParams->S3DataBlock.VolatileStorage)->RegisterDataSize != 0) {
    LateContextPtr = &BufferPointer[((S3_VOLATILE_STORAGE_HEADER *) S3LateParams->S3DataBlock.VolatileStorage)->RegisterDataOffset];
    // Restore registers before exiting self refresh
    RestorePreESRContext (&OrMaskPtr,
                          LateContextPtr,
                          S3_LATE_RESTORE,
                          &S3LateParams->StdHeader);
    // Restore registers after exiting self refresh
    RestorePostESRContext (OrMaskPtr,
                           LateContextPtr,
                           S3_LATE_RESTORE,
                           &S3LateParams->StdHeader);
  }

  // Dispatch any features needing to run at this time point
  IDS_HDT_CONSOLE (CPU_TRACE, "  Dispatch CPU features at S3 late restore end\n");
  CalledStatus = DispatchCpuFeatures (CPU_FEAT_S3_LATE_RESTORE_END,
                                      &S3LateParams->PlatformConfig,
                                      &S3LateParams->StdHeader);
  if (CalledStatus > ReturnStatus) {
    ReturnStatus = CalledStatus;
  }

  CalledStatus = S3ScriptRestore (&S3LateParams->StdHeader);
  if (CalledStatus > ReturnStatus) {
    ReturnStatus = CalledStatus;
  }

  IDS_OPTION_HOOK (IDS_AFTER_S3_RESTORE, S3LateParams, &S3LateParams->StdHeader);
  AGESA_TESTPOINT (TpIfAmdS3LateRestoreExit, &S3LateParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "\nAmdS3LateRestore: End\n\n");
  IDS_HDT_CONSOLE_S3_EXIT (&S3LateParams->StdHeader);
  return  ReturnStatus;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Constructor for the AMD_S3LATE_RESTORE function.
 *
 * This routine is responsible for setting default values for the
 * input parameters needed by the AMD_S3LATE_RESTORE entry point.
 *
 * @param[in]     StdHeader         AMD standard header config param.
 * @param[in,out] S3LateParams      Required input parameters for the
 *                                  AMD_S3LATE_RESTORE entry point.
 *
 * @retval        AGESA_SUCCESS     Always Succeeds.
 *
 */
AGESA_STATUS
AmdS3LateRestoreInitializer (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_S3LATE_PARAMS *S3LateParams
  )
{
  ASSERT (StdHeader != NULL);
  ASSERT (S3LateParams != NULL);

  S3LateParams->StdHeader = *StdHeader;

  AmdS3ParamsInitializer (&S3LateParams->S3DataBlock);

  AmdS3LateRestorePlatformConfigInit (&S3LateParams->PlatformConfig, &S3LateParams->StdHeader);

  return AGESA_SUCCESS;
}

/*------------------------------------------------------------------------------------*/
/**
 * Initialize AmdS3Save stage platform profile and user option input.
 *
 * @param[in,out]   PlatformConfig   Platform profile/build option config structure
 * @param[in,out]   StdHeader        AMD standard header config param
 *
 * @retval          AGESA_SUCCESS    Always Succeeds.
 *
 */
AGESA_STATUS
AmdS3LateRestorePlatformConfigInit (
  IN OUT   PLATFORM_CONFIGURATION    *PlatformConfig,
  IN OUT   AMD_CONFIG_PARAMS         *StdHeader
  )
{
  CommonPlatformConfigInit (PlatformConfig, StdHeader);

  return AGESA_SUCCESS;
}
