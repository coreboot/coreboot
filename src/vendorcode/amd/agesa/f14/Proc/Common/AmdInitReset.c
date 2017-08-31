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
 * @e \$Revision: 36908 $   @e \$Date: 2010-08-27 09:19:54 +0800 (Fri, 27 Aug 2010) $
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
#include "cpuCacheInit.h"
#include "cpuServices.h"
#include "AdvancedApi.h"
#include "GeneralServices.h"
#include "OptionsHt.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_COMMON_AMDINITRESET_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern CONST OPTION_HT_INIT_RESET HtOptionInitReset;
extern BUILD_OPT_CFG UserOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

AGESA_STATUS
AmdInitResetExecutionCacheAllocateInitializer (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       EXECUTION_CACHE_REGION *AmdExeAddrMapPtr
  );

AGESA_STATUS
AmdInitResetConstructor (
  IN       AMD_CONFIG_PARAMS    *StdHeader,
  IN       AMD_RESET_PARAMS     *AmdResetParams
  );

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
  ASSERT (AmdExeAddrMapPtr != NULL);

  LibAmdMemFill (AmdExeAddrMapPtr, 0, sizeof (EXECUTION_CACHE_REGION) * MAX_CACHE_REGIONS, StdHeader);

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
  UINT8 PrevRequestBit;
  UINT8 PrevStateBits;

  // Init Debug Print function
  IDS_HDT_CONSOLE_INIT (&ResetParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "\n*** %s ***\n\n", &UserOptions.VersionString);

  IDS_HDT_CONSOLE_DEBUG_CODE (
    {
      extern CHAR8 *BldOptDebugOutput[];

      UINT8 i;
      for (i = 0; BldOptDebugOutput[i] != NULL; i++) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "\t%s\n", BldOptDebugOutput[i]);
      }
    }
  )

  IDS_HDT_CONSOLE (MAIN_FLOW, "\nAmdInitReset: Start\n\n");

  AGESA_TESTPOINT (TpIfAmdInitResetEntry, &ResetParams->StdHeader);
  ASSERT (ResetParams != NULL);

  AgesaStatus = AGESA_SUCCESS;
  PrevRequestBit = FALSE;
  PrevStateBits = WR_STATE_COLD;

  // If a previously requested warm reset cannot be triggered in the
  // current stage, store the previous state of request and reset the
  // request struct to the current post stage
  GetWarmResetFlag (&ResetParams->StdHeader, &Request);
  if (Request.RequestBit == TRUE) {
    if (Request.StateBits >= Request.PostStage) {
      PrevRequestBit = Request.RequestBit;
      PrevStateBits = Request.StateBits;
      Request.RequestBit = FALSE;
      Request.StateBits = Request.PostStage - 1;
      SetWarmResetFlag (&ResetParams->StdHeader, &Request);
    }
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "AllocateExecutionCache: Start\n");
  // Setup ROM execution cache
  CalledAgesaStatus = AllocateExecutionCache (&ResetParams->StdHeader, &ResetParams->CacheRegion[0]);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AllocateExecutionCache: End\n");
  if (CalledAgesaStatus > AgesaStatus) {
    AgesaStatus = CalledAgesaStatus;
  }

  // Initialize the PCI MMIO access mechanism
  InitializePciMmio (&ResetParams->StdHeader);

  // Initialize Hyper Transport Registers
  if (HtOptionInitReset.HtInitReset != NULL) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "HtInitReset: Start\n");
    CalledAgesaStatus = HtOptionInitReset.HtInitReset (&ResetParams->StdHeader, &ResetParams->HtConfig);
    IDS_HDT_CONSOLE (MAIN_FLOW, "HtInitReset: End\n");
    if (CalledAgesaStatus > AgesaStatus) {
      AgesaStatus = CalledAgesaStatus;
    }
  }

  // Warm Reset, should be at the end of AmdInitReset
  GetWarmResetFlag (&ResetParams->StdHeader, &Request);
  // If a warm reset is requested in the current post stage, trigger the
  // warm reset and ignore the previous request
  if (Request.RequestBit == TRUE) {
    if (Request.StateBits < Request.PostStage) {
      AgesaDoReset (WARM_RESET_WHENEVER, &ResetParams->StdHeader);
    }
  } else {
    // Otherwise, if there's a previous request, restore it
    // so that the subsequent post stage can trigger the warm reset
    if (PrevRequestBit == TRUE) {
      Request.RequestBit = PrevRequestBit;
      Request.StateBits = PrevStateBits;
      SetWarmResetFlag (&ResetParams->StdHeader, &Request);
    }
  }
  // Check for Cache As Ram Corruption
  IDS_CAR_CORRUPTION_CHECK (&ResetParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "\nAmdInitReset: End\n\n");

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
  // Initialize Hyper Transport input structure
  if (HtOptionInitReset.HtResetConstructor != NULL) {
    HtOptionInitReset.HtResetConstructor (&AmdResetParams->StdHeader, &AmdResetParams->HtConfig);
  }

  return AGESA_SUCCESS;
}

