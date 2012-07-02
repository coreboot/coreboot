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

