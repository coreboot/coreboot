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
 * @e \$Revision: 35136 $   @e \$Date: 2010-07-16 11:29:48 +0800 (Fri, 16 Jul 2010) $
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
