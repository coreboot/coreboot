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
 * @e \$Revision: 7735 $   @e \$Date: 2008-08-27 14:49:19 -0500 (Wed, 27 Aug 2008) $
 *
 */
/*****************************************************************************
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
#include "CommonInits.h"
#include "Filecode.h"
#include "heapManager.h"
#include "CreateStruct.h"
#include "Topology.h"
#define FILECODE PROC_COMMON_AMDS3SAVE_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
CONST UINT32 ROMDATA S3LateHeapTable[] =
{
  EVENT_LOG_BUFFER_HANDLE,
  SOCKET_DIE_MAP_HANDLE,
  NODE_ID_MAP_HANDLE,
  LOCAL_AP_MAIL_BOX_CACHE_HANDLE,
  IDS_CONTROL_HANDLE,
  AMD_S3_SCRIPT_SAVE_TABLE_HANDLE
};

#define S3LATE_TABLE_SIZE (sizeof (S3LateHeapTable) / sizeof (UINT32)) //(sizeof (S3LateHeapTable) / sizeof (S3LATE_HEAP_ELEMENT))


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
AmdS3SavePlatformConfigInit (
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
 * Main entry point for the AMD_S3_SAVE function.
 *
 * This entry point is responsible for saving silicon component registers to the
 * SMM save area in preparation of entering system suspend-to-RAM mode.
 *
 * @param[in,out] AmdS3SaveParams   Required input parameters for the AMD_S3_SAVE
 *                                  entry point.
 *
 * @return        Aggregated status across all internal AMD S3 save calls invoked.
 *
 */
AGESA_STATUS
AmdS3Save (
  IN OUT   AMD_S3SAVE_PARAMS *AmdS3SaveParams
  )
{
  UINTN  i;
  UINT32 EarlyBufferSize;
  UINT32 LateBufferSize;
  UINT32 LateContextSize;
  UINT32 HeapSize;
  UINT8  *BufferPointer;
  UINT8  HeapStatus;
  ALLOCATE_HEAP_PARAMS HeapParams;
  AGESA_BUFFER_PARAMS AllocParams;
  DEVICE_BLOCK_HEADER *MemoryRelatedDeviceList;
  DEVICE_BLOCK_HEADER *NonMemoryRelatedDeviceList;
  AGESA_STATUS ReturnStatus;
  VOID   *HeapPtrs[S3LATE_TABLE_SIZE];
  UINT32 HeapSizes[S3LATE_TABLE_SIZE];
  UINT32 HeapBuffersPresent;
  HEAP_MANAGER *HeapPtr;

  AGESA_TESTPOINT (TpIfAmdS3SaveEntry, &AmdS3SaveParams->StdHeader);

  ASSERT (AmdS3SaveParams != NULL);

  HeapBuffersPresent = 0;
  EarlyBufferSize = 0;
  LateBufferSize = 0;
  LateContextSize = 0;
  HeapSize = 0;
  NonMemoryRelatedDeviceList = NULL;
  ReturnStatus = AGESA_SUCCESS;

  IDS_SKIP_HOOK (IDS_BEFORE_S3_SAVE, AmdS3SaveParams, &(AmdS3SaveParams->StdHeader)) {

    // Get memory device list
    MemFS3GetDeviceList (&MemoryRelatedDeviceList, &AmdS3SaveParams->StdHeader);
    if (MemoryRelatedDeviceList != NULL) {
      // Determine size needed
      EarlyBufferSize = GetWorstCaseContextSize (MemoryRelatedDeviceList, INIT_RESUME, &AmdS3SaveParams->StdHeader);
    }

    if (UserOptions.CfgS3LateRestore) {
      AllocParams.StdHeader = AmdS3SaveParams->StdHeader;
      for (i = 0; i < S3LATE_TABLE_SIZE; i++) {
        AllocParams.BufferHandle = S3LateHeapTable[i];
        AllocParams.BufferLength = 0;
        if (AgesaLocateBuffer (0, &AllocParams) == AGESA_SUCCESS) {
          ASSERT (AllocParams.BufferLength != 0);
          HeapBuffersPresent++;
          HeapSize += AllocParams.BufferLength; //S3LateHeapTable[i].BufferLength;
          HeapPtrs[i] = AllocParams.BufferPointer;
          HeapSizes[i] = AllocParams.BufferLength;
        } else {
          HeapPtrs[i] = NULL;
          HeapSizes[i] = 0;
        }
      }

      // Determine heap data size requirements
      if (HeapBuffersPresent != 0) {
        HeapSize += (((sizeof (HEAP_MANAGER)) + (HeapBuffersPresent * (sizeof (BUFFER_NODE)))) + (HeapBuffersPresent * (2 * SIZE_OF_SENTINEL)));
      }

      // Get non memory device list
      GetNonMemoryRelatedDeviceList (&NonMemoryRelatedDeviceList, &AmdS3SaveParams->StdHeader);

      if (NonMemoryRelatedDeviceList != NULL) {
        // Determine size needed
        LateContextSize = GetWorstCaseContextSize (NonMemoryRelatedDeviceList, S3_LATE_RESTORE, &AmdS3SaveParams->StdHeader);
      }
      LateBufferSize = HeapSize + LateContextSize;
      if (LateBufferSize != 0) {
        LateBufferSize += sizeof (S3_VOLATILE_STORAGE_HEADER);
      }
    }

    if ((EarlyBufferSize != 0) || (LateBufferSize != 0)) {
      //
      // Allocate a buffer by callback function
      //
      AllocParams.StdHeader = AmdS3SaveParams->StdHeader;
      AllocParams.BufferLength = EarlyBufferSize + LateBufferSize;
      AllocParams.BufferHandle = AMD_S3_INFO_BUFFER_HANDLE;

      AGESA_TESTPOINT (TpIfBeforeAllocateS3SaveBuffer, &AmdS3SaveParams->StdHeader);
      if (AgesaAllocateBuffer (0, &AllocParams) != AGESA_SUCCESS) {
        if (AGESA_ERROR > ReturnStatus) {
          ReturnStatus = AGESA_ERROR;
        }
      }
      AGESA_TESTPOINT (TpIfAfterAllocateS3SaveBuffer, &AmdS3SaveParams->StdHeader);

      if (EarlyBufferSize != 0) {
        AmdS3SaveParams->S3DataBlock.NvStorage = AllocParams.BufferPointer;
        SaveDeviceListContext (MemoryRelatedDeviceList,
                           AmdS3SaveParams->S3DataBlock.NvStorage,
                           INIT_RESUME,
                           &EarlyBufferSize,
                           &AmdS3SaveParams->StdHeader);

        AmdS3SaveParams->S3DataBlock.NvStorageSize = EarlyBufferSize;
      }

      if (LateBufferSize != 0) {
        BufferPointer = AllocParams.BufferPointer;
        AmdS3SaveParams->S3DataBlock.VolatileStorage = &(BufferPointer[EarlyBufferSize]);

        ((S3_VOLATILE_STORAGE_HEADER *) AmdS3SaveParams->S3DataBlock.VolatileStorage)->HeapOffset = 0;
        ((S3_VOLATILE_STORAGE_HEADER *) AmdS3SaveParams->S3DataBlock.VolatileStorage)->HeapSize = HeapSize;
        ((S3_VOLATILE_STORAGE_HEADER *) AmdS3SaveParams->S3DataBlock.VolatileStorage)->RegisterDataOffset = 0;
        ((S3_VOLATILE_STORAGE_HEADER *) AmdS3SaveParams->S3DataBlock.VolatileStorage)->RegisterDataSize = LateContextSize;

        if (HeapSize != 0) {
          // Transfer heap contents
          ((S3_VOLATILE_STORAGE_HEADER *) AmdS3SaveParams->S3DataBlock.VolatileStorage)->HeapOffset = sizeof (S3_VOLATILE_STORAGE_HEADER);
          HeapPtr = (HEAP_MANAGER *) &BufferPointer[EarlyBufferSize + sizeof (S3_VOLATILE_STORAGE_HEADER)];
          HeapPtr->AvailableSize = (UINT16) (HeapSize - sizeof (HEAP_MANAGER));

          HeapStatus = AmdS3SaveParams->StdHeader.HeapStatus;
          AmdS3SaveParams->StdHeader.HeapStatus = HEAP_S3_RESUME;
          AmdS3SaveParams->StdHeader.HeapBasePtr = (UINT32) HeapPtr;

          for (i = 0; i < S3LATE_TABLE_SIZE; i++) {
            if (HeapPtrs[i] != NULL) {
              HeapParams.RequestedBufferSize = (UINT16) HeapSizes[i]; // S3LateHeapTable[i].BufferLength;
              HeapParams.BufferHandle = S3LateHeapTable[i];
              HeapParams.Persist = HEAP_S3_RESUME;
              if (HeapAllocateBuffer (&HeapParams, &AmdS3SaveParams->StdHeader) == AGESA_SUCCESS) {
                LibAmdMemCopy ((VOID *) HeapParams.BufferPtr, HeapPtrs[i], HeapSizes[i], &AmdS3SaveParams->StdHeader);
              }
            }
          }

          AmdS3SaveParams->StdHeader.HeapStatus = HeapStatus;
        }


        if (LateContextSize != 0) {

          ((S3_VOLATILE_STORAGE_HEADER *) AmdS3SaveParams->S3DataBlock.VolatileStorage)->RegisterDataOffset = HeapSize + sizeof (S3_VOLATILE_STORAGE_HEADER);

          SaveDeviceListContext (NonMemoryRelatedDeviceList,
                                &(BufferPointer[EarlyBufferSize + HeapSize + sizeof (S3_VOLATILE_STORAGE_HEADER)]),
                                S3_LATE_RESTORE,
                                &LateContextSize,
                                &AmdS3SaveParams->StdHeader);
        }

        AmdS3SaveParams->S3DataBlock.VolatileStorageSize = HeapSize + LateContextSize + sizeof (S3_VOLATILE_STORAGE_HEADER);
      }
    }
  }
  IDS_OPTION_HOOK (IDS_AFTER_S3_SAVE, AmdS3SaveParams, &AmdS3SaveParams->StdHeader);
  AGESA_TESTPOINT (TpIfAmdS3SaveExit, &AmdS3SaveParams->StdHeader);
  return  ReturnStatus;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Constructor for the AMD_S3_SAVE function.
 *
 * This routine is responsible for setting default values for the
 * input parameters needed by the AMD_S3_SAVE entry point.
 *
 * @param[in]    StdHeader       The standard header.
 * @param[in,out] S3SaveParams   Required input parameters for the AMD_S3_SAVE
 *                               entry point.
 *
 * @retval        AGESA_SUCCESS  Always Succeeds.
 *
 */
AGESA_STATUS
AmdS3SaveInitializer (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_S3SAVE_PARAMS *S3SaveParams
  )
{
  ASSERT (StdHeader != NULL);
  ASSERT (S3SaveParams != NULL);

  S3SaveParams->StdHeader = *StdHeader;

  AmdS3ParamsInitializer (&S3SaveParams->S3DataBlock);

  AmdS3SavePlatformConfigInit (&S3SaveParams->PlatformConfig, &S3SaveParams->StdHeader);

  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Destructor for the AMD_S3_SAVE function.
 *
 * This routine is responsible for deallocation of heap space allocated during
 * AMD_S3_SAVE entry point.
 *
 * @param[in]     StdHeader       The standard header.
 * @param[in,out] S3SaveParams   Required input parameters for the AMD_INIT_RESUME
 *                               entry point.
 *
 * @retval        AGESA_STATUS
 *
 */
AGESA_STATUS
AmdS3SaveDestructor (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_S3SAVE_PARAMS *S3SaveParams
  )
{
  AGESA_STATUS ReturnStatus;
  AGESA_STATUS RetVal;

  ASSERT (S3SaveParams != NULL);

  ReturnStatus = AGESA_SUCCESS;

  // Deallocate heap space allocated during memory S3 save
  RetVal = MemS3Deallocate (&S3SaveParams->StdHeader);
  if (RetVal > ReturnStatus) {
    ReturnStatus = RetVal;
  }

  RetVal = HeapDeallocateBuffer (AMD_S3_NB_INFO_BUFFER_HANDLE, StdHeader);
  if (RetVal > ReturnStatus) {
    ReturnStatus = RetVal;
  }

  RetVal = HeapDeallocateBuffer (AMD_S3_INFO_BUFFER_HANDLE, StdHeader);
  if (RetVal > ReturnStatus) {
    ReturnStatus = RetVal;
  }

  return ReturnStatus;
}

/*------------------------------------------------------------------------------------*/
/**
 * Initialize AmdS3Save stage platform profile and user option input.
 *
 * @param[in,out]   PlatformConfig   Platform profile/build option config structure
 * @param[in,out]   StdHeader        AMD standard header config param
 *
 * @retval          AGESA_SUCCESS    Always Succeeds.
 *
 */
AGESA_STATUS
AmdS3SavePlatformConfigInit (
  IN OUT   PLATFORM_CONFIGURATION    *PlatformConfig,
  IN OUT   AMD_CONFIG_PARAMS         *StdHeader
  )
{
  CommonPlatformConfigInit (PlatformConfig, StdHeader);

  return AGESA_SUCCESS;
}
