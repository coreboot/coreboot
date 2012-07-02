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
#include "Ids.h"
#include "cpuEnvInit.h"
#include "heapManager.h"
#include "GnbInterface.h"
#include "CommonInits.h"
#include "AmdFch.h"
#include "S3SaveState.h"
#include "Filecode.h"
#include "CreateStruct.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_COMMON_AMDINITENV_FILECODE

extern BLDOPT_FCH_FUNCTION  BldoptFchFunction;

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
  BldoptFchFunction.InitEnvConstructor (EnvParamsPtr);
  GnbInitDataStructAtEnvDef (&EnvParamsPtr->GnbEnvConfiguration, EnvParamsPtr);

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


  //Copy Temp Ram heap content to Main Ram
  AgesaStatus = CopyHeapToMainRamAtPost (&(EnvParams->StdHeader));
  if (AgesaStatus > AmdInitEnvStatus) {
    AmdInitEnvStatus = AgesaStatus;
  }
  EnvParams->StdHeader.HeapStatus = HEAP_SYSTEM_MEM;
  EnvParams->StdHeader.HeapBasePtr = HeapGetBaseAddress (&EnvParams->StdHeader);
  // Any heap allocate/deallocat/locate buffer should be used after heap is rebuild from here.
  // After persist heaps are tansfer and rebuild, HeapLocateBuffer can start to be used in IDS hook.

  //Heap have been relocated, so Debug Print need be init again to get new address
  IDS_PERF_TIMESTAMP (&EnvParams->StdHeader);
  IDS_HDT_CONSOLE_INIT (&EnvParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "Heap transfer End\n");
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdInitEnv: Start\n\n");
  IDS_OPTION_HOOK (IDS_PLATFORMCFG_OVERRIDE, &EnvParams->PlatformConfig, &(EnvParams->StdHeader));
  IDS_OPTION_HOOK (IDS_BEFORE_PCI_INIT, EnvParams, &(EnvParams->StdHeader));

  AgesaStatus = S3ScriptInit (&EnvParams->StdHeader);
  if (AgesaStatus > AmdInitEnvStatus) {
    AmdInitEnvStatus = AgesaStatus;
  }

  IDS_PERF_TIMESTAMP (&EnvParams->StdHeader);
  AgesaStatus = BldoptFchFunction.InitEnv (EnvParams);
  AmdInitEnvStatus = (AgesaStatus > AmdInitEnvStatus) ? AgesaStatus : AmdInitEnvStatus;

  IDS_PERF_TIMESTAMP (&EnvParams->StdHeader);
  AgesaStatus = GnbInitAtEnv (EnvParams);
  if (AgesaStatus > AmdInitEnvStatus) {
    AmdInitEnvStatus = AgesaStatus;
  }

  AGESA_TESTPOINT (TpIfAmdInitEnvExit, &EnvParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "\nAmdInitEnv: End\n");
  IDS_PERF_TIMESTAMP (&EnvParams->StdHeader);
  IDS_HDT_CONSOLE_FLUSH_BUFFER (&EnvParams->StdHeader);
  return  AmdInitEnvStatus;
}


