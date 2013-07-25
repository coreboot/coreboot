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
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*****************************************************************************
 *
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
#include "Ids.h"
#include "Options.h"
#include "Filecode.h"
#include "heapManager.h"
#include "CreateStruct.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_COMMON_AMDLATERUNAPTASK_FILECODE
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
extern CONST DISPATCH_TABLE ApDispatchTable[];

/*---------------------------------------------------------------------------------------*/
/**
 * Application Processor perform a function as directed by the BSC.
 *
 * This is needed for an AP task that must run after AGESA has relinquished control
 * of the APs to the IBV.
 *
 * @param[in]     AmdApExeParams  The interface struct for any required routine.
 *
 * @return        The most severe AGESA_STATUS returned by any called service.  Note
 *                that this will be the return value passed back to the BSC as the
 *                return value for the call out.
 *
 */
AGESA_STATUS
AmdLateRunApTask (
  IN       AP_EXE_PARAMS  *AmdApExeParams
  )
{
  AGESA_STATUS        CalledAgesaStatus;
  AGESA_STATUS        ApLateTaskStatus;
  DISPATCH_TABLE      *Entry;

  AGESA_TESTPOINT (TpIfAmdLateRunApTaskEntry, &AmdApExeParams->StdHeader);

  ASSERT (AmdApExeParams != NULL);
  ApLateTaskStatus = AGESA_SUCCESS;
  CalledAgesaStatus = AGESA_UNSUPPORTED;

  // Dispatch, if valid
  Entry = (DISPATCH_TABLE *) ApDispatchTable;
  while (Entry->FunctionId != 0) {
    if (AmdApExeParams->FunctionNumber == Entry->FunctionId) {
      CalledAgesaStatus = Entry->EntryPoint (AmdApExeParams);
      break;
    }
    Entry++;
  }

  if (CalledAgesaStatus > ApLateTaskStatus) {
    ApLateTaskStatus = CalledAgesaStatus;
  }

  AGESA_TESTPOINT (TpIfAmdLateRunApTaskExit, &AmdApExeParams->StdHeader);
  return  ApLateTaskStatus;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Constructor for the AMD_LATE_RUN_AP_TASK function.
 *
 * This routine is responsible for setting default values for the
 * input parameters needed by the AMD_S3_SAVE entry point.
 *
 * @param[in]     StdHeader      The standard header.
 * @param[in,out] AmdApExeParams Required input parameters for the AMD_LATE_RUN_AP_TASK
 *                               entry point.
 *
 * @retval        AGESA_SUCCESS  Always Succeeds.
 *
 */
AGESA_STATUS
AmdLateRunApTaskInitializer (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AP_EXE_PARAMS     *AmdApExeParams
  )
{
  ASSERT (StdHeader != NULL);
  ASSERT (AmdApExeParams != NULL);

  AmdApExeParams->StdHeader = *StdHeader;
  AmdApExeParams->FunctionNumber = 0;
  AmdApExeParams->RelatedDataBlock = NULL;
  AmdApExeParams->RelatedBlockLength = 0;
  return AGESA_SUCCESS;
}

