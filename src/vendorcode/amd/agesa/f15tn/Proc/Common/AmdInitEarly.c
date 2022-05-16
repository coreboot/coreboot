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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*****************************************************************************
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
#include "cpuCacheInit.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuEarlyInit.h"
#include "AdvancedApi.h"
#include "cpuServices.h"
#include "CommonInits.h"
#include "GnbInterface.h"
#include "Filecode.h"
#include "heapManager.h"
#include "CreateStruct.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_COMMON_AMDINITEARLY_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
CONST EXECUTION_CACHE_REGION InitExeCacheMap[] =
{
  {0x00000000, 0x00000000},
  {0x00000000, 0x00000000},
  {0x00000000, 0x00000000}
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
AmdEarlyPlatformConfigInit (
  IN OUT   PLATFORM_CONFIGURATION    *PlatformConfig,
  IN OUT   AMD_CONFIG_PARAMS         *StdHeader
  );

AGESA_STATUS
AllocateExecutionCacheInitializer (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       EXECUTION_CACHE_REGION *AmdExeAddrMapPtr
  );
/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern BUILD_OPT_CFG UserOptions;
/*------------------------------------------------------------------------------------*/
/**
 * Initialize AmdInitEarly stage platform profile and user option input.
 *
 * @param[in,out]   PlatformConfig   Platform profile/build option config structure
 * @param[in,out]   StdHeader        AMD standard header config param
 *
 * @retval      AGESA_SUCCESS     Always Succeeds.
 *
 */
AGESA_STATUS
AmdEarlyPlatformConfigInit (
  IN OUT   PLATFORM_CONFIGURATION    *PlatformConfig,
  IN OUT   AMD_CONFIG_PARAMS         *StdHeader
  )
{
  CommonPlatformConfigInit (PlatformConfig, StdHeader);

  return AGESA_SUCCESS;
}
/*------------------------------------------------------------------------------------*/
/**
 * Initializer routine that will be invoked by the wrapper to initialize the input
 * structure for the AllocateExecutionCache.
 *
 * @param[in]   StdHeader         Opaque handle to standard config header
 * @param[in]   AmdExeAddrMapPtr  Our Service interface struct
 *
 * @retval      AGESA_SUCCESS     Always Succeeds.
 *
 */
AGESA_STATUS
AllocateExecutionCacheInitializer (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       EXECUTION_CACHE_REGION *AmdExeAddrMapPtr
  )
{
  UINT8 i;
  ASSERT (AmdExeAddrMapPtr != NULL);

  for (i = 0; i < MAX_CACHE_REGIONS; ++i) {
    AmdExeAddrMapPtr[i].ExeCacheStartAddr = InitExeCacheMap[i].ExeCacheStartAddr;
    AmdExeAddrMapPtr[i].ExeCacheSize = InitExeCacheMap[i].ExeCacheSize;
  }

  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 *  Initializer routine that will be invoked by the wrapper to initialize the input
 *  structure for the AmdInitEarly.
 *
 *  @param[in]      StdHeader       AMD standard header config param.
 *  @param[in,out]  EarlyParams     The service interface struct to initialize.
 *
 *  @retval         AGESA_SUCCESS   Always succeeds.
 */
AGESA_STATUS
AmdInitEarlyInitializer (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_EARLY_PARAMS  *EarlyParams
  )
{
  ASSERT (StdHeader != NULL);
  ASSERT (EarlyParams != NULL);

  EarlyParams->StdHeader = *StdHeader;

  // We don't check any AGESA_STATUS from the called constructors, since they MUST all SUCCEED.
  //

  AllocateExecutionCacheInitializer (&EarlyParams->StdHeader, &EarlyParams->CacheRegion[0]);

  AmdHtInterfaceConstructor (&EarlyParams->StdHeader, &EarlyParams->HtConfig);

  AmdEarlyPlatformConfigInit (&EarlyParams->PlatformConfig, &EarlyParams->StdHeader);

  return AGESA_SUCCESS;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Perform initialization services required at the Early Init POST time point.
 *
 * Execution Cache, HyperTransport, and AP Init advanced services are performed.
 *
 * @param[in]     EarlyParams  The interface struct for all early services
 *
 * @return        The most severe AGESA_STATUS returned by any called service.
 *
 */
AGESA_STATUS
AmdInitEarly (
  IN OUT   AMD_EARLY_PARAMS  *EarlyParams
  )
{
  AGESA_STATUS  CalledAgesaStatus;
  AGESA_STATUS  EarlyInitStatus;
  WARM_RESET_REQUEST Request;
  UINT8 PrevRequestBit;
  UINT8 PrevStateBits;

  IDS_PERF_TIMESTAMP (&EarlyParams->StdHeader);

  AGESA_TESTPOINT (TpIfAmdInitEarlyEntry, &EarlyParams->StdHeader);

  EarlyInitStatus = AGESA_SUCCESS;

  // Setup ROM execution cache
  IDS_HDT_CONSOLE (MAIN_FLOW, "AllocateExecutionCache: Start\n");
  CalledAgesaStatus = AllocateExecutionCache (&EarlyParams->StdHeader, &EarlyParams->CacheRegion[0]);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AllocateExecutionCache: End\n");
  if (CalledAgesaStatus > EarlyInitStatus) {
    EarlyInitStatus = CalledAgesaStatus;
  }

  IDS_HDT_CONSOLE_DEBUG_CODE (
    {
      extern CHAR8 *BldOptDebugOutput[];

      UINT8 i;
      for (i = 0; BldOptDebugOutput[i] != NULL; i++) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "\t%s\n", BldOptDebugOutput[i]);
      }
    }
  )

  //
  // WARNING: AGESA's own IDT is at heap which would be moved from one place to another
  //          so we MUST restore IDT every time before moving heap.
  //
  IDS_EXCEPTION_TRAP (IDS_IDT_REPLACE_IDTR_FOR_BSC, NULL, &EarlyParams->StdHeader);
  ASSERT (EarlyParams != NULL);
  PrevRequestBit = FALSE;
  PrevStateBits = WR_STATE_COLD;
  IDS_HDT_CONSOLE (MAIN_FLOW, "\nAmdInitEarly: Start %x \n\n", PrevStateBits);
  // If a previously requested warm reset cannot be triggered in the
  // current stage, store the previous state of request and reset the
  // request struct to the current post stage
  GetWarmResetFlag (&EarlyParams->StdHeader, &Request);
  if (Request.RequestBit == TRUE) {
    if (Request.StateBits >= Request.PostStage) {
      PrevRequestBit = Request.RequestBit;
      PrevStateBits = Request.StateBits;
      Request.RequestBit = FALSE;
      Request.StateBits = Request.PostStage - 1;
      SetWarmResetFlag (&EarlyParams->StdHeader, &Request);
    }
  }

  IDS_OPTION_HOOK (IDS_INIT_EARLY_BEFORE, EarlyParams, &EarlyParams->StdHeader);

  // Full Hypertransport Initialization
  // IMPORTANT: All AP cores call Ht Init.  HT Init handles full init for the BSC, and map init for APs.
  IDS_PERF_TIMESTAMP (&EarlyParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdHtInitialize: Start\n");
  CalledAgesaStatus = AmdHtInitialize (&EarlyParams->StdHeader, &EarlyParams->PlatformConfig, &EarlyParams->HtConfig);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdHtInitialize: End\n");
  if (CalledAgesaStatus > EarlyInitStatus) {
    EarlyInitStatus = CalledAgesaStatus;
  }

  IDS_PERF_TIMESTAMP (&EarlyParams->StdHeader);
  CalledAgesaStatus = GnbInitAtEarlier (EarlyParams);
  if (CalledAgesaStatus > EarlyInitStatus) {
    EarlyInitStatus = CalledAgesaStatus;
  }

  // AP launch
  IDS_PERF_TIMESTAMP (&EarlyParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdCpuEarly: Start\n");
  CalledAgesaStatus = AmdCpuEarly (&EarlyParams->StdHeader, &EarlyParams->PlatformConfig);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdCpuEarly: End\n");
  if (CalledAgesaStatus > EarlyInitStatus) {
    EarlyInitStatus = CalledAgesaStatus;
  }

  // Warm Reset, should be at the end of AmdInitEarly
  GetWarmResetFlag (&EarlyParams->StdHeader, &Request);
  // If a warm reset is requested in the current post stage, trigger the
  // warm reset and ignore the previous request
  if (Request.RequestBit == TRUE) {
    if (Request.StateBits < Request.PostStage) {
      AgesaDoReset (WARM_RESET_WHENEVER, &EarlyParams->StdHeader);
    }
  } else {
    // Otherwise, if there's a previous request, restore it
    // so that the subsequent post stage can trigger the warm reset
    if (PrevRequestBit == TRUE) {
      Request.RequestBit = PrevRequestBit;
      Request.StateBits = PrevStateBits;
      SetWarmResetFlag (&EarlyParams->StdHeader, &Request);
    }
  }

  IDS_PERF_TIMESTAMP (&EarlyParams->StdHeader);
  CalledAgesaStatus = GnbInitAtEarly (EarlyParams);
  if (CalledAgesaStatus > EarlyInitStatus) {
    EarlyInitStatus = CalledAgesaStatus;
  }
  // Check for Cache As Ram Corruption
  IDS_CAR_CORRUPTION_CHECK (&EarlyParams->StdHeader);

  IDS_OPTION_HOOK (IDS_INIT_EARLY_AFTER, EarlyParams, &EarlyParams->StdHeader);
  AGESA_TESTPOINT (TpIfAmdInitEarlyExit, &EarlyParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "\nAmdInitEarly: End\n\n");

  // Flush out all debug contents in case warm reset is triggered after this point
  IDS_HDT_CONSOLE_FLUSH_BUFFER (&EarlyParams->StdHeader);
  IDS_PERF_TIMESTAMP (&EarlyParams->StdHeader);

  return  EarlyInitStatus;
}
