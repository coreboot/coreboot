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
#include "cpuCacheInit.h"
#include "cpuServices.h"
#include "AdvancedApi.h"
#include "GeneralServices.h"
#include "OptionsHt.h"
#include "Filecode.h"
#define FILECODE PROC_COMMON_AMDINITRESET_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern CONST OPTION_HT_INIT_RESET HtOptionInitReset;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
EXECUTION_CACHE_REGION InitResetExeCacheMap[] =
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

/*------------------------------------------------------------------------------------*/
/**
 * Initializer routine that will be invoked by the wrapper to initialize the input
 * structure for the AllocateExecutionCache.
 *
 *  Parameters:
 * @param[in]   StdHeader         Opaque handle to standard config header
 * @param[in]   AmdExeAddrMapPtr  Our Service interface struct
 *
 * @retval      AGESA_SUCCESS     Always Succeeds.
 *
 */
AGESA_STATUS
AmdInitResetExecutionCacheAllocateInitializer (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       EXECUTION_CACHE_REGION *AmdExeAddrMapPtr
  )
{
  UINT8 i;
  ASSERT (AmdExeAddrMapPtr != NULL);

  for (i = 0; i < MAX_CACHE_REGIONS; ++i) {
    AmdExeAddrMapPtr[i].ExeCacheStartAddr = InitResetExeCacheMap[i].ExeCacheStartAddr;
    AmdExeAddrMapPtr[i].ExeCacheSize = InitResetExeCacheMap[i].ExeCacheSize;
  }

  return AGESA_SUCCESS;
}
/*---------------------------------------------------------------------------------------*/
/**
 * Main entry point for the AMD_INIT_RESET function.
 *
 * This entry point is responsible for establishing the HT links to the program
 * ROM and for performing basic processor initialization.
 *
 * @param[in,out] ResetParams    Required input parameters for the AMD_INIT_RESET
 *                                  entry point.
 *
 * @return        Aggregated status across all internal AMD reset calls invoked.
 *
 */
AGESA_STATUS
AmdInitReset (
  IN OUT   AMD_RESET_PARAMS *ResetParams
  )
{
  AGESA_STATUS AgesaStatus;
  AGESA_STATUS CalledAgesaStatus;
  WARM_RESET_REQUEST Request;

  AGESA_TESTPOINT (TpIfAmdInitResetEntry, &ResetParams->StdHeader);
  ASSERT (ResetParams != NULL);

  AgesaStatus = AGESA_SUCCESS;

  GetWarmResetFlag (&ResetParams->StdHeader, &Request);
  Request.RequestBit = FALSE;
  SetWarmResetFlag (&ResetParams->StdHeader, &Request);

  // Setup ROM execution cache
  CalledAgesaStatus = AllocateExecutionCache (&ResetParams->StdHeader, &ResetParams->CacheRegion[0]);
  if (CalledAgesaStatus > AgesaStatus) {
    AgesaStatus = CalledAgesaStatus;
  }

  // Initialize the PCI MMIO access mechanism
  InitializePciMmio (&ResetParams->StdHeader);

  // Initialize Hyper Transport Registers
  if (HtOptionInitReset.HtInitReset != NULL) {
    CalledAgesaStatus = HtOptionInitReset.HtInitReset (&ResetParams->StdHeader, &ResetParams->HtConfig);
    if (CalledAgesaStatus > AgesaStatus) {
      AgesaStatus = CalledAgesaStatus;
    }
  }

  // Warm Rest, should be at the end of AmdInitReset
  GetWarmResetFlag (&ResetParams->StdHeader, &Request);
  if (Request.RequestBit == TRUE) {
    Request.RequestBit = FALSE;
    Request.StateBits = WR_STATE_RESET;
    SetWarmResetFlag (&ResetParams->StdHeader, &Request);
    AgesaDoReset (WARM_RESET_WHENEVER, &ResetParams->StdHeader);
  } else {
    if (Request.StateBits < WR_STATE_RESET) {
      Request.StateBits = WR_STATE_RESET;
      SetWarmResetFlag (&ResetParams->StdHeader, &Request);
    }
  }
  // Check for Cache As Ram Corruption
  IDS_CAR_CORRUPTION_CHECK (&ResetParams->StdHeader);

  AGESA_TESTPOINT (TpIfAmdInitResetExit, &ResetParams->StdHeader);
  return  AgesaStatus;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Initialize defaults and options for Amd Init Reset.
 *
 * @param[in]  StdHeader        Header
 * @param[in]  AmdResetParams   The Reset Init interface to initialize.
 *
 * @retval     AGESA_SUCCESS    Always Succeeds.
 */
AGESA_STATUS
AmdInitResetConstructor (
  IN       AMD_CONFIG_PARAMS    *StdHeader,
  IN       AMD_RESET_PARAMS     *AmdResetParams
  )
{
  ASSERT (AmdResetParams != NULL);

  AmdResetParams->StdHeader = *StdHeader;

  AmdInitResetExecutionCacheAllocateInitializer (&AmdResetParams->StdHeader, &AmdResetParams->CacheRegion[0]);
  AmdHtResetConstructor (&AmdResetParams->StdHeader, &AmdResetParams->HtConfig);

  return AGESA_SUCCESS;
}

