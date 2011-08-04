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
#include "mm.h"
#include "mn.h"
#include "S3.h"
#include "mfs3.h"
#include "Filecode.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuPostInit.h"
#include "CreateStruct.h"
#include "CommonInits.h"
#include "cpuFeatures.h"
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

  AGESA_TESTPOINT (TpIfAmdInitResumeEntry, &ResumeParams->StdHeader);

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

      IDS_HDT_CONSOLE (CPU_TRACE, "  Dispatch CPU features after S3 AP MTRR sync\n");
      ReturnStatus = DispatchCpuFeatures (CPU_FEAT_AFTER_RESUME_MTRR_SYNC, &ResumeParams->PlatformConfig, &ResumeParams->StdHeader);
      if (ReturnStatus > AmdInitResumeStatus) {
        AmdInitResumeStatus = ReturnStatus;
      }
    }
  }

  // Set TscFreqSel at the rate specified by the core P0
  SetCoresTscFreqSel (&ResumeParams->StdHeader);
  // HDT out of All Aps
  IDS_HDT_CONSOLE_S3_AP_EXIT (&ResumeParams->StdHeader);
  // Relinquish control of all APs to IBV
  RelinquishControlOfAllAPs (&ResumeParams->StdHeader);

  AGESA_TESTPOINT (TpIfAmdInitResumeExit, &ResumeParams->StdHeader);
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
