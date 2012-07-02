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
#include "S3.h"
#include "cpuFeatures.h"
#include "S3SaveState.h"
#include "CommonInits.h"
#include "Filecode.h"
#include "heapManager.h"
#include "CreateStruct.h"
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

  ReturnStatus = AGESA_SUCCESS;

  ASSERT (S3LateParams != NULL);

  BufferPointer = (UINT8 *) S3LateParams->S3DataBlock.VolatileStorage;
  S3LateParams->StdHeader.HeapBasePtr = (UINT32) &BufferPointer[((S3_VOLATILE_STORAGE_HEADER *) S3LateParams->S3DataBlock.VolatileStorage)->HeapOffset];
  ASSERT (S3LateParams->StdHeader.HeapBasePtr != 0);

  IDS_HDT_CONSOLE_INIT (&S3LateParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdS3LateRestore: Start\n\n");

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
 * Initialize AmdS3LateRestore stage platform profile and user option input.
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
