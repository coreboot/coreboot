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
#include "Ids.h"
#include "cpuCacheInit.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuEarlyInit.h"
#include "AdvancedApi.h"
#include "cpuServices.h"
#include "heapManager.h"
#include "CreateStruct.h"
#include "CommonInits.h"
#include "GnbInterface.h"
#include "Filecode.h"
#define FILECODE PROC_COMMON_AMDINITEARLY_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
EXECUTION_CACHE_REGION InitExeCacheMap[] =
{
  {0x00000000, 0x00000000},
  {0x00000000, 0x00000000},
  {0x00000000, 0x00000000}
};

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
 * Initialize AmdInitEarly stage platform profile and user option input.
 *
 * @param[in,out]   PlatformConfig   Platform profile/build option config structure
 * @param[in,out]   StdHeader        AMD standard header config param
 *
 * @retval      AGESA_SUCCESS     Always Succeeds.
 *
 */
STATIC AGESA_STATUS
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
STATIC AGESA_STATUS
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

  AGESA_TESTPOINT (TpIfAmdInitEarlyEntry, &EarlyParams->StdHeader);
  IDS_PERF_TIME_MEASURE (&EarlyParams->StdHeader);
  ASSERT (EarlyParams != NULL);
  EarlyInitStatus = AGESA_SUCCESS;

  GetWarmResetFlag (&EarlyParams->StdHeader, &Request);
  Request.RequestBit = FALSE;
  SetWarmResetFlag (&EarlyParams->StdHeader, &Request);

  IDS_OPTION_HOOK (IDS_INIT_EARLY_BEFORE, EarlyParams, &EarlyParams->StdHeader);

  // Setup ROM execution cache
  CalledAgesaStatus = AllocateExecutionCache (&EarlyParams->StdHeader, &EarlyParams->CacheRegion[0]);
  if (CalledAgesaStatus > EarlyInitStatus) {
    EarlyInitStatus = CalledAgesaStatus;
  }

  // Full Hypertransport Initialization
  // IMPORTANT: All AP cores call Ht Init.  HT Init handles full init for the BSC, and map init for APs.
  CalledAgesaStatus = AmdHtInitialize (&EarlyParams->StdHeader, &EarlyParams->PlatformConfig, &EarlyParams->HtConfig);
  if (CalledAgesaStatus > EarlyInitStatus) {
    EarlyInitStatus = CalledAgesaStatus;
  }

  // AP launch
  CalledAgesaStatus = AmdCpuEarly (&EarlyParams->StdHeader, &EarlyParams->PlatformConfig);
  if (CalledAgesaStatus > EarlyInitStatus) {
    EarlyInitStatus = CalledAgesaStatus;
  }

  // Warm Rest, should be at the end of AmdInitEarly
  GetWarmResetFlag (&EarlyParams->StdHeader, &Request);
  if (Request.RequestBit == TRUE) {
    Request.RequestBit = FALSE;
    Request.StateBits = WR_STATE_EARLY;
    SetWarmResetFlag (&EarlyParams->StdHeader, &Request);
    AgesaDoReset (WARM_RESET_WHENEVER, &EarlyParams->StdHeader);
  } else {
    if (Request.StateBits < WR_STATE_EARLY) {
      Request.StateBits = WR_STATE_EARLY;
      SetWarmResetFlag (&EarlyParams->StdHeader, &Request);
    }
  }

  CalledAgesaStatus = GnbInitAtEarly (
                        &EarlyParams->StdHeader,
                        &EarlyParams->PlatformConfig,
                        &EarlyParams->GnbConfig
                        );
  if (CalledAgesaStatus > EarlyInitStatus) {
    EarlyInitStatus = CalledAgesaStatus;
  }
  // Check for Cache As Ram Corruption
  IDS_CAR_CORRUPTION_CHECK (&EarlyParams->StdHeader);

  IDS_OPTION_HOOK (IDS_AFTER_WARM_RESET, EarlyParams, &EarlyParams->StdHeader);
  IDS_OPTION_HOOK (IDS_INIT_EARLY_AFTER, EarlyParams, &EarlyParams->StdHeader);
  IDS_PERF_TIME_MEASURE (&EarlyParams->StdHeader);
  AGESA_TESTPOINT (TpIfAmdInitEarlyExit, &EarlyParams->StdHeader);
  return  EarlyInitStatus;
}
