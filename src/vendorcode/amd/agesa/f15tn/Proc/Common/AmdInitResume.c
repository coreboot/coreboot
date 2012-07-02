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
#include "mm.h"
#include "mn.h"
#include "S3.h"
#include "mfs3.h"
#include "Filecode.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuPostInit.h"
#include "CommonInits.h"
#include "cpuFeatures.h"
#include "heapManager.h"
#include "CreateStruct.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_COMMON_AMDINITRESUME_FILECODE
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

/*---------------------------------------------------------------------------------------*/
/**
 * Main entry point for the AMD_INIT_RESUME function.
 *
 * This entry point is responsible for performing silicon device and memory
 * re-initialization for the resume boot path.
 *
 * @param[in]     ResumeParams   Required input parameters for the AMD_INIT_RESUME
 *                                  entry point.
 *
 * @return        Aggregated status across all internal AMD resume calls invoked.
 *
 */
AGESA_STATUS
AmdInitResume (
  IN       AMD_RESUME_PARAMS *ResumeParams
  )
{
  VOID   *OrMaskPtr;
  AGESA_STATUS ReturnStatus;
  AGESA_STATUS AmdInitResumeStatus;
  BSC_AP_MSR_SYNC ApMsrSync[4];

  IDS_PERF_TIMESTAMP (&ResumeParams->StdHeader);
  AGESA_TESTPOINT (TpIfAmdInitResumeEntry, &ResumeParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdInitResume Start\n");

  AmdInitResumeStatus = AGESA_SUCCESS;

  ASSERT (ResumeParams != NULL);

  if (ResumeParams->S3DataBlock.NvStorage != NULL) {

    MemS3ResumeInitNB (&ResumeParams->StdHeader);

    // Restore registers before exiting self refresh
    RestorePreESRContext (&OrMaskPtr,
                          ResumeParams->S3DataBlock.NvStorage,
                          INIT_RESUME,
                          &ResumeParams->StdHeader);
    // Exit self refresh
    ReturnStatus = AmdMemS3Resume (&ResumeParams->StdHeader);
    IDS_PERF_TIMESTAMP (&ResumeParams->StdHeader);
    if (ReturnStatus > AmdInitResumeStatus) {
      AmdInitResumeStatus = ReturnStatus;
    }
    if (ReturnStatus == AGESA_SUCCESS) {

      // Restore registers after exiting self refresh
      RestorePostESRContext (OrMaskPtr,
                             ResumeParams->S3DataBlock.NvStorage,
                             INIT_RESUME,
                             &ResumeParams->StdHeader);

      ApMsrSync[0].RegisterAddress = SYS_CFG;
      ApMsrSync[1].RegisterAddress = TOP_MEM;
      ApMsrSync[2].RegisterAddress = TOP_MEM2;
      ApMsrSync[3].RegisterAddress = 0;
      SyncApMsrsToBsc (ApMsrSync, &ResumeParams->StdHeader);
      IDS_PERF_TIMESTAMP (&ResumeParams->StdHeader);

      IDS_HDT_CONSOLE (CPU_TRACE, "  Dispatch CPU features after S3 AP MTRR sync\n");
      ReturnStatus = DispatchCpuFeatures (CPU_FEAT_AFTER_RESUME_MTRR_SYNC, &ResumeParams->PlatformConfig, &ResumeParams->StdHeader);
      IDS_PERF_TIMESTAMP (&ResumeParams->StdHeader);
      if (ReturnStatus > AmdInitResumeStatus) {
        AmdInitResumeStatus = ReturnStatus;
      }
    }
  }

  // Set TscFreqSel at the rate specified by the core P0
  SetCoresTscFreqSel (&ResumeParams->StdHeader);

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdInitResume End\n");
  // HDT out of All Aps
  IDS_HDT_CONSOLE_FLUSH_BUFFER (&ResumeParams->StdHeader);
  // Relinquish control of all APs to IBV
  RelinquishControlOfAllAPs (&ResumeParams->StdHeader);

  // Restore IDT
  IDS_EXCEPTION_TRAP (IDS_IDT_RESTORE_IDTR_FOR_BSC, NULL, &ResumeParams->StdHeader);
  IDS_OPTION_HOOK (IDS_AFTER_S3_RESUME, NULL, &ResumeParams->StdHeader);
  AGESA_TESTPOINT (TpIfAmdInitResumeExit, &ResumeParams->StdHeader);
  IDS_PERF_TIMESTAMP (&ResumeParams->StdHeader);
  IDS_PERF_ANALYSE (&ResumeParams->StdHeader);

  return (AmdInitResumeStatus);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Constructor for the AMD_INIT_RESUME function.
 *
 * This routine is responsible for setting default values for the
 * input parameters needed by the AMD_INIT_RESUME entry point.
 *
 * @param[in]     StdHeader       The standard header.
 * @param[in,out] ResumeParams   Required input parameters for the AMD_INIT_RESUME
 *                               entry point.
 *
 * @retval        AGESA_SUCCESS  Always Succeeds.
 *
 */
AGESA_STATUS
AmdInitResumeInitializer (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_RESUME_PARAMS *ResumeParams
  )
{
  ASSERT (StdHeader != NULL);
  ASSERT (ResumeParams != NULL);

  ResumeParams->StdHeader = *StdHeader;

  AmdS3ParamsInitializer (&ResumeParams->S3DataBlock);
  CommonPlatformConfigInit (&ResumeParams->PlatformConfig, &ResumeParams->StdHeader);

  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Destructor for the AMD_INIT_RESUME function.
 *
 * This routine is responsible for deallocation of heap space allocated during
 * AMD_INIT_RESUME entry point.
 *
 * @param[in]    StdHeader       The standard header.
 * @param[in,out] ResumeParams   Required input parameters for the AMD_INIT_RESUME
 *                               entry point.
 *
 * @retval        AGESA_STATUS
 *
 */
AGESA_STATUS
AmdInitResumeDestructor (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_RESUME_PARAMS *ResumeParams
  )
{
  AGESA_STATUS ReturnStatus;
  AGESA_STATUS RetVal;

  ASSERT (ResumeParams != NULL);

  ReturnStatus = AGESA_SUCCESS;

  // Deallocate heap space allocated during memory S3 resume
  RetVal = MemS3Deallocate (&ResumeParams->StdHeader);
  if (RetVal > ReturnStatus) {
    ReturnStatus = RetVal;
  }

  return ReturnStatus;
}
