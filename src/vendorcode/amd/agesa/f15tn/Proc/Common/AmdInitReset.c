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
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
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
#include "cpuServices.h"
#include "AdvancedApi.h"
#include "GeneralServices.h"
#include "OptionsHt.h"
#include "AmdFch.h"
#include "Filecode.h"
#include "heapManager.h"
#include "CreateStruct.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_COMMON_AMDINITRESET_FILECODE

extern BLDOPT_FCH_FUNCTION  BldoptFchFunction;

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

  IDS_PERF_TIMESTAMP (&ResetParams->StdHeader);
  AgesaStatus = AGESA_SUCCESS;

  // Setup ROM execution cache
  CalledAgesaStatus = AllocateExecutionCache (&ResetParams->StdHeader, &ResetParams->CacheRegion[0]);
  if (CalledAgesaStatus > AgesaStatus) {
    AgesaStatus = CalledAgesaStatus;
  }

  //IDS_EXTENDED_HOOK (IDS_INIT_RESET_BEFORE, NULL, NULL, &ResetParams->StdHeader);

  // Init Debug Print function
  IDS_HDT_CONSOLE_INIT (&ResetParams->StdHeader);

  IDS_HDT_CONSOLE (MAIN_FLOW, "\nAmdInitReset: Start\n\n");

  IDS_HDT_CONSOLE (MAIN_FLOW, "\n*** %s ***\n\n", &UserOptions.VersionString);

  AGESA_TESTPOINT (TpIfAmdInitResetEntry, &ResetParams->StdHeader);
  ASSERT (ResetParams != NULL);

  PrevRequestBit = FALSE;
  PrevStateBits = WR_STATE_COLD;

  IDS_PERF_TIMESTAMP (&ResetParams->StdHeader);
  if (IsBsp (&ResetParams->StdHeader, &AgesaStatus)) {
    CalledAgesaStatus = BldoptFchFunction.InitReset (ResetParams);
    AgesaStatus = (CalledAgesaStatus > AgesaStatus) ? CalledAgesaStatus : AgesaStatus;
  }

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

  // Initialize the PCI MMIO access mechanism
  InitializePciMmio (&ResetParams->StdHeader);

  IDS_PERF_TIMESTAMP (&ResetParams->StdHeader);
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

  IDS_PERF_TIMESTAMP (&ResetParams->StdHeader);

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
  BldoptFchFunction.InitResetConstructor (AmdResetParams);

  return AGESA_SUCCESS;
}

