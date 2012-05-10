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
#include "amdlib.h"
#include "mm.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "cpuPostInit.h"
#include "AdvancedApi.h"
#include "heapManager.h"
#include "CreateStruct.h"
#include "CommonInits.h"
#include "cpuServices.h"
#include "GnbInterface.h"
#include "Filecode.h"
#define FILECODE PROC_COMMON_AMDINITPOST_FILECODE
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

extern BUILD_OPT_CFG UserOptions;

/*------------------------------------------------------------------------------------*/
/**
 * Initialize AmdInitPost stage platform profile and user option input.
 *
 * @param[in,out]   PlatformConfig   Platform profile/build option config structure
 * @param[in,out]   StdHeader        AMD standard header config param
 *
 * @retval      AGESA_SUCCESS     Always Succeeds.
 *
 */
STATIC AGESA_STATUS
AmdPostPlatformConfigInit (
  IN OUT   PLATFORM_CONFIGURATION    *PlatformConfig,
  IN OUT   AMD_CONFIG_PARAMS         *StdHeader
  )
{
  CommonPlatformConfigInit (PlatformConfig, StdHeader);

  return AGESA_SUCCESS;
}

/*
 *---------------------------------------------------------------------------------------
 *
 *  AmdInitPostInitializer
 *
 *  Initializer routine that will be invoked by the wrapper
 *  to initialize the input structure for the AmdInitPost
 *
 *  @param[in, out]    IN OUT   AMD_POST_PARAMS *PostParamsPtr
 *
 *  @retval         AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
AGESA_STATUS
AmdInitPostInitializer (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_POST_PARAMS   *PostParamsPtr
  )
{
  AGESA_STATUS  AgesaStatus;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  ASSERT (StdHeader != NULL);
  ASSERT (PostParamsPtr != NULL);

  PostParamsPtr->StdHeader = *StdHeader;

  AllocHeapParams.RequestedBufferSize = sizeof (MEM_DATA_STRUCT);
  AllocHeapParams.BufferHandle = AMD_MEM_DATA_HANDLE;
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  AgesaStatus = HeapAllocateBuffer (&AllocHeapParams, &PostParamsPtr->StdHeader);

  if (AgesaStatus == AGESA_SUCCESS) {
    PostParamsPtr->MemConfig.MemData = (MEM_DATA_STRUCT *) AllocHeapParams.BufferPtr;
    PostParamsPtr->MemConfig.MemData->ParameterListPtr = &(PostParamsPtr->MemConfig);
    PostParamsPtr->MemConfig.MemData->StdHeader = PostParamsPtr->StdHeader;
    AmdPostPlatformConfigInit (&PostParamsPtr->PlatformConfig, &PostParamsPtr->StdHeader);
    AmdMemInitDataStructDef (PostParamsPtr->MemConfig.MemData, &PostParamsPtr->PlatformConfig);
  }
  return AgesaStatus;
}

/*
 *---------------------------------------------------------------------------------------
 *
 *  AmdInitPostDestructor
 *
 *  Destruct routine that provide a chance if something need to be done
 *  before the end of AmdInitPost.
 *
 *  @param[in]    StdHeader       The standard header.
 *  @param[in]    PostParamsPtr   AMD init post param.
 *
 *  @retval         AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
AGESA_STATUS
AmdInitPostDestructor (
  IN   AMD_CONFIG_PARAMS *StdHeader,
  IN   AMD_POST_PARAMS   *PostParamsPtr
  )
{

  ASSERT (PostParamsPtr != NULL);

  PostParamsPtr->StdHeader = *StdHeader;
  PostParamsPtr->MemConfig.MemData->StdHeader = *StdHeader;

  //
  // AmdMemAuto completed. Here, release heap space which is used for memory init.
  //
  MemAmdFinalize (PostParamsPtr->MemConfig.MemData);
  HeapDeallocateBuffer (AMD_MEM_DATA_HANDLE, StdHeader);

  //
  // AmdCpuPost completed.
  //
  FinalizeAtPost (StdHeader);

  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Main entry point for the AMD_INIT_POST function.
 *
 * This entry point is responsible for initializing all system memory,
 * gathering important data out of the pre-memory cache storage into a
 * temporary holding buffer in main memory. After that APs will be
 * shutdown in preparation for the host environment to take control.
 * Note: pre-memory stack will be disabled also.
 *
 * @param[in,out] PostParams     Required input parameters for the AMD_INIT_POST
 *                                  entry point.
 *
 * @return        Aggregated status across all internal AMD POST calls invoked.
 *
 */
AGESA_STATUS
AmdInitPost (
  IN OUT   AMD_POST_PARAMS *PostParams
  )
{
  AGESA_STATUS  AgesaStatus;
  AGESA_STATUS  AmdInitPostStatus;
  WARM_RESET_REQUEST Request;

  AGESA_TESTPOINT (TpIfAmdInitPostEntry, &PostParams->StdHeader);

  IDS_PERF_TIME_MEASURE (&PostParams->StdHeader);

  ASSERT (PostParams != NULL);
  AmdInitPostStatus = AGESA_SUCCESS;

  IDS_OPTION_HOOK (IDS_INIT_POST_BEFORE, PostParams, &PostParams->StdHeader);

  IDS_OPTION_HOOK (IDS_BEFORE_MEM_INIT, PostParams, &PostParams->StdHeader);

  GetWarmResetFlag (&PostParams->StdHeader, &Request);
  Request.RequestBit = FALSE;
  SetWarmResetFlag (&PostParams->StdHeader, &Request);

  AgesaStatus = GnbInitAtPost (
                  &PostParams->StdHeader,
                  &PostParams->MemConfig
                  );
  if (AgesaStatus > AmdInitPostStatus) {
    AmdInitPostStatus = AgesaStatus;
  }

  AgesaStatus = AmdMemAuto (PostParams->MemConfig.MemData);
  if (AgesaStatus > AmdInitPostStatus) {
    AmdInitPostStatus = AgesaStatus;
  }

  if (AgesaStatus != AGESA_FATAL) {

    IDS_OPTION_HOOK (IDS_INIT_POST_MID, PostParams, &PostParams->StdHeader);

    // Check BIST status
    AgesaStatus = CheckBistStatus (&PostParams->StdHeader);
    if (AgesaStatus > AmdInitPostStatus) {
      AmdInitPostStatus = AgesaStatus;
    }

    //
    // P-State data gathered, then, Relinquish APs
    //
    AgesaStatus = AmdCpuPost (&PostParams->StdHeader, &PostParams->PlatformConfig);
    if (AgesaStatus > AmdInitPostStatus) {
      AmdInitPostStatus = AgesaStatus;
    }

    // Warm Reset
    GetWarmResetFlag (&PostParams->StdHeader, &Request);
    if (Request.RequestBit == TRUE) {
      Request.RequestBit = FALSE;
      Request.StateBits = WR_STATE_POST;
      SetWarmResetFlag (&PostParams->StdHeader, &Request);
      AgesaDoReset (WARM_RESET_WHENEVER, &PostParams->StdHeader);
    } else {
      if (Request.StateBits < WR_STATE_POST) {
        Request.StateBits = WR_STATE_POST;
        SetWarmResetFlag (&PostParams->StdHeader, &Request);
      }
    }

    IDS_OPTION_HOOK (IDS_INIT_POST_AFTER, PostParams, &PostParams->StdHeader);

    IDS_PERF_TIME_MEASURE (&PostParams->StdHeader);
    AGESA_TESTPOINT (TpIfAmdInitPostExit, &PostParams->StdHeader);

    // Copies BSP heap content to RAM, and it should be at the end of AmdInitPost
    AgesaStatus = CopyHeapToTempRamAtPost (&(PostParams->StdHeader));
    if (AgesaStatus > AmdInitPostStatus) {
      AmdInitPostStatus = AgesaStatus;
    }
    PostParams->StdHeader.HeapStatus = HEAP_TEMP_MEM;
  }
  // Check for Cache As Ram Corruption
  IDS_CAR_CORRUPTION_CHECK (&PostParams->StdHeader);
  return  AmdInitPostStatus;
}

