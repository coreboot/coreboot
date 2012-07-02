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
 * @e sub-project:  Common
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
#include "AdvancedApi.h"
#include "heapManager.h"
#include "mm.h"
#include "GnbInterface.h"
#include "cpuRecovery.h"
#include "cpuCacheInit.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_COMMON_AMDINITRECOVERY_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Perform initialization services required at the Early Init POST time point.
 *
 * Execution Cache, HyperTransport, C1e, and AP Init advanced services are performed.
 *
 * @param[in, out]     RecoveryParams  The interface struct for Recovery services
 *
 * @return        The most severe AGESA_STATUS returned by any called service.
 *
 */
AGESA_STATUS
AmdInitRecovery (
  IN OUT   AMD_RECOVERY_PARAMS      *RecoveryParams
  )
{
  AGESA_STATUS  AgesaStatus;
  AGESA_STATUS  CalledAgesaStatus;

  AGESA_TESTPOINT (TpIfAmdInitRecoveryEntry, &RecoveryParams->StdHeader);

  ASSERT (RecoveryParams != NULL);

  AgesaStatus = AGESA_SUCCESS;

  // Setup ROM execution cache
  CalledAgesaStatus = AllocateExecutionCache (&RecoveryParams->StdHeader, &RecoveryParams->CacheRegion[0]);
  if (CalledAgesaStatus > AgesaStatus) {
    AgesaStatus = CalledAgesaStatus;
  }

  CalledAgesaStatus = AmdHtInitRecovery (&RecoveryParams->StdHeader);
  if (CalledAgesaStatus > AgesaStatus) {
    AgesaStatus = CalledAgesaStatus;
  }

  CalledAgesaStatus = AmdCpuRecovery ((AMD_CPU_RECOVERY_PARAMS *) &RecoveryParams->StdHeader);
  if (CalledAgesaStatus > AgesaStatus) {
    AgesaStatus = CalledAgesaStatus;
  }

  CalledAgesaStatus = AmdMemRecovery (RecoveryParams->MemConfig.MemData);
  if (CalledAgesaStatus > AgesaStatus) {
    AgesaStatus = CalledAgesaStatus;
  }

  CalledAgesaStatus = AmdGnbRecovery (&RecoveryParams->StdHeader);
  if (CalledAgesaStatus > AgesaStatus) {
    AgesaStatus = CalledAgesaStatus;
  }

  AGESA_TESTPOINT (TpIfAmdInitRecoveryExit, &RecoveryParams->StdHeader);
  return  AgesaStatus;
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * Initialize defaults and options for Amd Init Reset.
 *
 * @param[in]  StdHeader              AMD standard header config param.
 * @param[in]  AmdRecoveryParamsPtr   The Reset Init interface to initialize.
 *
 * @retval     AGESA_SUCCESS    Always Succeeds.
 */
AGESA_STATUS
AmdInitRecoveryInitializer (
  IN       AMD_CONFIG_PARAMS   *StdHeader,
  IN OUT   AMD_RECOVERY_PARAMS *AmdRecoveryParamsPtr
  )
{
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  ASSERT (StdHeader != NULL);
  ASSERT (AmdRecoveryParamsPtr != NULL);

  AmdRecoveryParamsPtr->StdHeader = *StdHeader;

  AllocHeapParams.RequestedBufferSize = sizeof (MEM_DATA_STRUCT);
  AllocHeapParams.BufferHandle = AMD_MEM_DATA_HANDLE;
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, &AmdRecoveryParamsPtr->StdHeader) == AGESA_SUCCESS) {
    AmdRecoveryParamsPtr->MemConfig.MemData = (MEM_DATA_STRUCT *) AllocHeapParams.BufferPtr;
    AmdRecoveryParamsPtr->MemConfig.MemData->ParameterListPtr = &(AmdRecoveryParamsPtr->MemConfig);
    LibAmdMemCopy ((VOID *) AmdRecoveryParamsPtr->MemConfig.MemData,
                   (VOID *) AmdRecoveryParamsPtr,
                   (UINTN) sizeof (AmdRecoveryParamsPtr->StdHeader),
                   &AmdRecoveryParamsPtr->StdHeader
                  );
    AmdMemInitDataStructDefRecovery (AmdRecoveryParamsPtr->MemConfig.MemData);
    return AGESA_SUCCESS;
  } else {
    return AGESA_ERROR;
  }
}
