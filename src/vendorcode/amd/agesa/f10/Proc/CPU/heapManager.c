/**
 * @file
 *
 * AMD Heap Manager and Heap Allocation APIs, and related functions.
 *
 * Contains code that initialize, maintain, and allocate the heap space.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*******************************************************************************
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
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "heapManager.h"
#include "cpuCacheInit.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_HEAPMANAGER_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          P U B L I C     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern BUILD_OPT_CFG UserOptions;

/*---------------------------------------------------------------------------------------*/
/**
 *  This function initializes the heap for each CPU core.
 *
 *  Check for already initialized.  If not, determine offset of local heap in CAS and
 *  setup initial heap markers and bookkeeping status.  Initialize a couple heap items
 *  all cores need, for convenience.  Currently these are caching the AP mailbox info and
 *  an initial event log.
 *
 *  @param[in]  StdHeader          Handle of Header for calling lib functions and services.
 *
 *  @retval     AGESA_SUCCESS      This core's heap is initialized (always succeeds)
 *
 */
AGESA_STATUS
HeapManagerInit (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  // First Time Initialization
  // Note: First 16 bytes of buffer is reserved for Heap Manager use
  UINT16                HeapAlreadyInitSizeDword;
  UINT8                 *HeapBufferPtr;
  UINT32                *HeapDataPtr;
  UINT64                MsrData;
  UINT8                 Ignored;
  CACHE_INFO            *CacheInfoPtr;
  AGESA_STATUS          IgnoredSts;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  GetCpuServicesOfCurrentCore (&FamilySpecificServices, StdHeader);
  FamilySpecificServices->GetCacheInfo (FamilySpecificServices, (CONST VOID **)&CacheInfoPtr, &Ignored, StdHeader);

  if (!IsBsp (StdHeader, &IgnoredSts)) {
    // APs must transfer their system core number from the mailbox to
    // a local register while it is still valid.
    FamilySpecificServices->TransferApCoreNumber (FamilySpecificServices, StdHeader);
  }

  HeapBufferPtr = (UINT8 *) HeapGetCurrentBase (StdHeader);
  // check whether the heap manager is already initialized
  LibAmdMsrRead (AMD_MTRR_VARIABLE_HEAP_MASK, &MsrData, StdHeader);
  if (MsrData == (CacheInfoPtr->VariableMtrrMask & AMD_HEAP_MTRR_MASK)) {
    LibAmdMsrRead (AMD_MTRR_VARIABLE_HEAP_BASE, &MsrData, StdHeader);
    if ((MsrData & 0xFFFFFFFFFFFFFF00ull) == (UINT32) HeapBufferPtr) {
      // This is not a bug, there are multiple premem basic entry points,
      // and each will call heap init to make sure create struct will succeed.
      // If that is later deemed a problem, there needs to be a reasonable test
      // for the calling code to make to determine if it needs to init heap or not.
      // In the mean time, add this to the event log
      PutEventLog (AGESA_SUCCESS,
                   CPU_ERROR_HEAP_IS_ALREADY_INITIALIZED,
                   0, 0, 0, 0, StdHeader);
      return AGESA_SUCCESS;
    }
  }

  // set variable MTRR base and mask
  MsrData = (UINT32) HeapBufferPtr;
  MsrData |= 0x06;
  LibAmdMsrWrite (AMD_MTRR_VARIABLE_HEAP_BASE, &MsrData, StdHeader);
  MsrData = CacheInfoPtr->VariableMtrrMask & AMD_HEAP_MTRR_MASK;
  LibAmdMsrWrite (AMD_MTRR_VARIABLE_HEAP_MASK, &MsrData, StdHeader);

  // set top of memory to a temp value
  MsrData = (UINT64) (AMD_TEMP_TOM);
  LibAmdMsrWrite (TOP_MEM, &MsrData, StdHeader);

  // Enable variable MTTRs
  LibAmdMsrRead (SYS_CFG, &MsrData, StdHeader);
  MsrData |= AMD_VAR_MTRR_ENABLE_BIT;
  LibAmdMsrWrite (SYS_CFG, &MsrData, StdHeader);

  // Initialize Heap Space
  HeapDataPtr = (UINT32 *) HeapBufferPtr;
  for (HeapAlreadyInitSizeDword = 0; HeapAlreadyInitSizeDword < AMD_HEAP_SIZE_DWORD_PER_CORE; HeapAlreadyInitSizeDword++) {
    *HeapDataPtr = 0;
    HeapDataPtr++;
  }

  // Save size of heap (which is contiguous right now) for later use
  // Note: We are reserving the first 16 bytes for Heap Manager use
  ((HEAP_MANAGER*) HeapBufferPtr)->AvailableSize = (AMD_HEAP_SIZE_PER_CORE - sizeof (HEAP_MANAGER));

  StdHeader->HeapStatus = HEAP_LOCAL_CACHE;
  if (!IsBsp (StdHeader, &IgnoredSts)) {
    // The BSP's hardware mailbox has not been initialized, so only APs
    // can do this at this point.
    CacheApMailbox (StdHeader);
  }
  EventLogInitialization (StdHeader);
  return AGESA_SUCCESS;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Allocates space for a new buffer in the heap
 *
 * This function determines if whether or not there is enough space for the
 * new structure.  If so, it will zero out the buffer, and return a pointer
 * to the region.
 *
 * @param[in,out]  AllocateHeapParamsPtr Structure containing the size of the
 *                                   desired new region, its handle, and the
 *                                   return pointer.
 * @param[in,out]  StdHeader         Config handle for library and services.
 *
 * @retval         AGESA_SUCCESS     No error
 * @retval         AGESA_BOUNDS_CHK  Handle already exists, or not enough
 *                                   free space
 *
 */
AGESA_STATUS
HeapAllocateBuffer (
  IN OUT   ALLOCATE_HEAP_PARAMS *AllocateHeapParamsPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8       *HeapBufferPtr;
  UINT8       *StartOfBufferPtr;
  HEAP_MANAGER *HeapManagerPtr;
  BUFFER_NODE *HeadNodePtr;
  BUFFER_NODE *CurrentNodePtr;
  BUFFER_NODE *NextNodePtr;
  AGESA_BUFFER_PARAMS  AgesaBuffer;
  UINT32      AmdHeapRamAddress;

  AmdHeapRamAddress = (UINT32) UserOptions.CfgHeapDramAddress;

  // Buffer pointer is NULL unless we return a buffer.
  AllocateHeapParamsPtr->BufferPtr = NULL;

  // check to see where the heap is
  if (StdHeader->HeapStatus == HEAP_LOCAL_CACHE) {
    HeapBufferPtr = (UINT8 *) HeapGetCurrentBase (StdHeader);
  } else if (StdHeader->HeapStatus == HEAP_TEMP_MEM) {
    HeapBufferPtr = (UINT8 *) AmdHeapRamAddress;
  } else if (StdHeader->HeapStatus == HEAP_SYSTEM_MEM) {
    AgesaBuffer.StdHeader = *StdHeader;
    AgesaBuffer.BufferHandle = AllocateHeapParamsPtr->BufferHandle;
    AgesaBuffer.BufferLength = AllocateHeapParamsPtr->RequestedBufferSize;

    AGESA_TESTPOINT (TpIfBeforeAllocateHeapBuffer, StdHeader);
    if (AgesaAllocateBuffer (0, &AgesaBuffer) != AGESA_SUCCESS) {
      AllocateHeapParamsPtr->BufferPtr = NULL;
      return AGESA_ERROR;
    }
    AGESA_TESTPOINT (TpIfAfterAllocateHeapBuffer, StdHeader);

    AllocateHeapParamsPtr->BufferPtr = (UINT8 *) (AgesaBuffer.BufferPointer);
    return AGESA_SUCCESS;
  } else if (StdHeader->HeapStatus == HEAP_S3_RESUME) {
    HeapBufferPtr = (UINT8 *)(UINT32) StdHeader->HeapBasePtr;
  } else {
    AllocateHeapParamsPtr->BufferPtr = NULL;
    // Heap buffer is not present.
    return AGESA_BOUNDS_CHK;
  }

  HeapManagerPtr = (HEAP_MANAGER *) HeapBufferPtr;
  StartOfBufferPtr = HeapBufferPtr + sizeof (HEAP_MANAGER);
  HeadNodePtr = (BUFFER_NODE *) StartOfBufferPtr;
  CurrentNodePtr = HeadNodePtr;

  // Check if we can allocate space
  if (AllocateHeapParamsPtr->RequestedBufferSize > (HeapManagerPtr->AvailableSize - sizeof (BUFFER_NODE) - 2 * SIZE_OF_SENTINEL)) {
    PutEventLog (AGESA_BOUNDS_CHK,
                 CPU_ERROR_HEAP_IS_FULL,
                 AllocateHeapParamsPtr->BufferHandle, 0, 0, 0, StdHeader);
    return AGESA_BOUNDS_CHK;
  }

  if (HeadNodePtr->BufferSize == 0) {
    NextNodePtr = CurrentNodePtr;
  } else {
    // locate the last heap, but if there already has been a heap with the incoming BufferHandle, we return AGESA_BOUNDS_CHK.
    while (CurrentNodePtr->NextNodePtr != NULL) {
      if (CurrentNodePtr->BufferHandle == AllocateHeapParamsPtr->BufferHandle) {
        PutEventLog (AGESA_BOUNDS_CHK,
                     CPU_ERROR_HEAP_BUFFER_HANDLE_IS_ALREADY_USED,
                     AllocateHeapParamsPtr->BufferHandle, 0, 0, 0, StdHeader);
        return AGESA_BOUNDS_CHK;
      }
      if (StdHeader->HeapStatus == HEAP_S3_RESUME) {
        HeapBufferPtr = (UINT8 *) CurrentNodePtr;
        CurrentNodePtr = (BUFFER_NODE *) &HeapBufferPtr[CurrentNodePtr->BufferSize + sizeof (BUFFER_NODE) + (2 * SIZE_OF_SENTINEL)];
      } else {
        CurrentNodePtr = CurrentNodePtr->NextNodePtr;
      }
    }
    if (CurrentNodePtr->BufferHandle == AllocateHeapParamsPtr->BufferHandle) {
      PutEventLog (AGESA_BOUNDS_CHK,
                   CPU_ERROR_HEAP_BUFFER_HANDLE_IS_ALREADY_USED,
                   AllocateHeapParamsPtr->BufferHandle, 0, 0, 0, StdHeader);
      return AGESA_BOUNDS_CHK;
    }

    // Create a new node and link it with previous node
    CurrentNodePtr->NextNodePtr = (BUFFER_NODE *)
                                  (((UINT8 *) CurrentNodePtr) +
                                   sizeof (BUFFER_NODE) +
                                   CurrentNodePtr->BufferSize +
                                   2 * SIZE_OF_SENTINEL);
    NextNodePtr = CurrentNodePtr->NextNodePtr;
  }
  NextNodePtr->BufferSize = AllocateHeapParamsPtr->RequestedBufferSize;
  NextNodePtr->NextNodePtr = NULL;
  NextNodePtr->BufferHandle = AllocateHeapParamsPtr->BufferHandle;

  // Debug feature
  SET_SENTINEL_BEFORE (NextNodePtr);
  SET_SENTINEL_AFTER (NextNodePtr);

 // Persist
  if ((AllocateHeapParamsPtr->Persist == HEAP_TEMP_MEM) || (AllocateHeapParamsPtr->Persist == HEAP_SYSTEM_MEM)) {
    NextNodePtr->Persist = AllocateHeapParamsPtr->Persist;
  } else {
    NextNodePtr->Persist = HEAP_LOCAL_CACHE;
  }

  // Update global variables
  HeapManagerPtr->AvailableSize -= NextNodePtr->BufferSize + sizeof (BUFFER_NODE) + 2 * SIZE_OF_SENTINEL;

  // Now fill in the incoming structure
  AllocateHeapParamsPtr->BufferPtr = (UINT8 *) ((UINT8 *) NextNodePtr + sizeof (BUFFER_NODE) + SIZE_OF_SENTINEL);
  Heap_Check (StdHeader);

  return AGESA_SUCCESS;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Deallocates a previously allocated buffer in the heap
 *
 * This function finds the buffer to deallocate, defragments the remaining
 * heap, clears the new remainder, and updates the size of the heap.
 *
 * @param[in]      BufferHandle      Handle of the buffer to free.
 * @param[in]      StdHeader         Config handle for library and services.
 *
 * @retval         AGESA_SUCCESS     No error
 * @retval         AGESA_BOUNDS_CHK  Handle does not exist on the heap
 *
 */
AGESA_STATUS
HeapDeallocateBuffer (
  IN       UINT32 BufferHandle,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT16      AboveDeallocationHeapSize;
  UINT16      DeallocationHeapSize;
  UINT16      BelowDeallocationHeapSize;
  UINT16      i;
  UINT8       *HeapBufferPtr;
  UINT8       *HeapDataPtr1;
  UINT8       *HeapDataPtr2;
  HEAP_MANAGER *HeapManagerPtr;
  BUFFER_NODE *HeadNodePtr;
  BUFFER_NODE *PreviousNodePtr;
  BUFFER_NODE *CurrentNodePtr;
  BUFFER_NODE *NextNodePtr;
  AGESA_BUFFER_PARAMS  AgesaBuffer;
  UINT32      AmdHeapRamAddress;

  AmdHeapRamAddress = (UINT32) UserOptions.CfgHeapDramAddress;

  AboveDeallocationHeapSize = 0;

  // Step 1: locate the heap which is expected to be deallocated.
  if (StdHeader->HeapStatus == HEAP_LOCAL_CACHE) {
    HeapBufferPtr = (UINT8 *) HeapGetCurrentBase (StdHeader);
  } else if (StdHeader->HeapStatus == HEAP_TEMP_MEM) {
    HeapBufferPtr = (UINT8 *) AmdHeapRamAddress;
  } else if (StdHeader->HeapStatus == HEAP_SYSTEM_MEM) {
    AgesaBuffer.StdHeader = *StdHeader;
    AgesaBuffer.BufferHandle = BufferHandle;

    AGESA_TESTPOINT (TpIfBeforeDeallocateHeapBuffer, StdHeader);
    if (AgesaDeallocateBuffer (0, &AgesaBuffer) != AGESA_SUCCESS) {
      return AGESA_ERROR;
    }
    AGESA_TESTPOINT (TpIfAfterDeallocateHeapBuffer, StdHeader);

    return AGESA_SUCCESS;
  } else {
    // Heap buffer is not present.
    IDS_ERROR_TRAP;
    return AGESA_BOUNDS_CHK;
  }

  HeapManagerPtr = (HEAP_MANAGER *) HeapBufferPtr;
  HeadNodePtr = (BUFFER_NODE *) ((HeapBufferPtr + sizeof (HEAP_MANAGER)));
  CurrentNodePtr = HeadNodePtr;
  PreviousNodePtr = CurrentNodePtr;

  while (CurrentNodePtr->BufferHandle != BufferHandle) {
    AboveDeallocationHeapSize += (CurrentNodePtr->BufferSize) + sizeof (BUFFER_NODE) + 2 * SIZE_OF_SENTINEL;
    PreviousNodePtr = CurrentNodePtr;
    if (CurrentNodePtr->NextNodePtr == NULL) {
      // If we are at the end of the heap and still unable to locate the buffer handle, return AGESA_BOUNDS_CHK
      PutEventLog (AGESA_BOUNDS_CHK,
                   CPU_ERROR_HEAP_BUFFER_HANDLE_IS_NOT_PRESENT,
                   BufferHandle, 0, 0, 0, StdHeader);
      return AGESA_BOUNDS_CHK;
    } else {
      CurrentNodePtr = CurrentNodePtr->NextNodePtr;
    }
  }

  // Step 2: defragment

  // Before:
  //  ------------------------ <- UINT8 * HeapBufferPtr
  //  | Heap Manager 16 Bytes|
  //  ------------------------  ---
  //  | Heap 1               |  AboveDeallocationHeapSize
  //  |                      |
  //  ------------------------  ---
  //  | Heap 2               | <- Deallocate
  //  |                      |  DeallocationHeapSize
  //  ------------------------  ---
  //  | Heap 3               |  BelowDeallocationHeapSize
  //  |                      |
  //  ------------------------  ---
  //  | Unused               |  UnusedSize = HeapManagerPtr->AvailableSize
  //  ------------------------  ---
  //  After deallocating: shift all subsequent heap buffers up
  //  ------------------------
  //  | Heap Manager 16 Bytes|
  //  ------------------------
  //  | Heap 1               |
  //  |                      |
  //  ------------------------
  //  | Heap 3               |
  //  |                      |
  //  ------------------------
  //  |                      |
  //  |                      |
  //  | Unused               |
  //  ------------------------
  DeallocationHeapSize = (CurrentNodePtr->BufferSize) + sizeof (BUFFER_NODE) + 2 * SIZE_OF_SENTINEL;
  BelowDeallocationHeapSize = AMD_HEAP_SIZE_PER_CORE - sizeof (HEAP_MANAGER)
                              - HeapManagerPtr->AvailableSize
                              - AboveDeallocationHeapSize
                              - DeallocationHeapSize;
  HeapDataPtr1 = (UINT8 *) CurrentNodePtr;

  // if this is the last Heap, then PreviousNodePtr->NextNodePtr = NULL and return
  if (CurrentNodePtr->NextNodePtr == NULL) {
    PreviousNodePtr->NextNodePtr = NULL;
  } else {
    HeapDataPtr2 = (UINT8 *) (CurrentNodePtr->NextNodePtr);
    // subtract DeallocationHeapSize from all subsequent NextNodePtr
    CurrentNodePtr = CurrentNodePtr->NextNodePtr;
    while (CurrentNodePtr->NextNodePtr != NULL) {
      NextNodePtr = CurrentNodePtr->NextNodePtr;
      CurrentNodePtr->NextNodePtr = (BUFFER_NODE *) ((UINT8 *) (CurrentNodePtr->NextNodePtr) - DeallocationHeapSize);
      CurrentNodePtr = NextNodePtr;
    }
    // shift subsequent heap buffers up
    for (i = 0; i < BelowDeallocationHeapSize; i++) {
      *HeapDataPtr1 = *HeapDataPtr2;
      HeapDataPtr1++;
      HeapDataPtr2++;
    }
  }

  // Step 3: clear remainder useless data
  LibAmdMemFill (HeapDataPtr1, 0, DeallocationHeapSize, StdHeader);

  // Step 4: update unused size
  HeapManagerPtr->AvailableSize = HeapManagerPtr->AvailableSize + DeallocationHeapSize;
  return AGESA_SUCCESS;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Locates a previously allocated buffer on the heap.
 *
 * This function searches the heap for a buffer with the desired handle, and
 * returns a pointer to the buffer.
 *
 * @param[in,out]  LocateHeapPtr     Structure containing the buffer's handle,
 *                                   and the return pointer.
 * @param[in]      StdHeader         Config handle for library and services.
 *
 * @retval         AGESA_SUCCESS     No error
 * @retval         AGESA_BOUNDS_CHK  Handle does not exist on the heap
 *
 */
AGESA_STATUS
HeapLocateBuffer (
  IN OUT   LOCATE_HEAP_PTR *LocateHeapPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8       *HeapBufferPtr;
  UINT8       *StartOfBufferPtr;
  HEAP_MANAGER *HeapManagerPtr;
  BUFFER_NODE *HeadNodePtr;
  BUFFER_NODE *CurrentNodePtr;
  BOOLEAN     BufferHandleFlag;
  AGESA_BUFFER_PARAMS  AgesaBuffer;
  UINT32  AmdHeapRamAddress;

  AmdHeapRamAddress = (UINT32) UserOptions.CfgHeapDramAddress;

  ASSERT (StdHeader != NULL);

  BufferHandleFlag = TRUE;

  if (StdHeader->HeapStatus == HEAP_LOCAL_CACHE) {
    HeapBufferPtr = (UINT8 *) HeapGetCurrentBase (StdHeader);
  } else if (StdHeader->HeapStatus == HEAP_TEMP_MEM) {
    HeapBufferPtr = (UINT8 *) AmdHeapRamAddress;
  } else if (StdHeader->HeapStatus == HEAP_SYSTEM_MEM) {
    AgesaBuffer.StdHeader = *StdHeader;
    AgesaBuffer.BufferHandle = LocateHeapPtr->BufferHandle;

    AGESA_TESTPOINT (TpIfBeforeLocateHeapBuffer, StdHeader);
    if (AgesaLocateBuffer (0, &AgesaBuffer) != AGESA_SUCCESS) {
      LocateHeapPtr->BufferPtr = NULL;
      return AGESA_ERROR;
    }
    AGESA_TESTPOINT (TpIfAfterLocateHeapBuffer, StdHeader);

    LocateHeapPtr->BufferPtr = (UINT8 *) (AgesaBuffer.BufferPointer);
    return AGESA_SUCCESS;
  } else if (StdHeader->HeapStatus == HEAP_S3_RESUME) {
    HeapBufferPtr = (UINT8 *) (UINT32) StdHeader->HeapBasePtr;
  } else {
    return AGESA_BOUNDS_CHK;
  }

  HeapManagerPtr = (HEAP_MANAGER *) HeapBufferPtr;
  StartOfBufferPtr = HeapBufferPtr + sizeof (HEAP_MANAGER);
  HeadNodePtr = (BUFFER_NODE *) StartOfBufferPtr;
  CurrentNodePtr = HeadNodePtr;

  while (CurrentNodePtr->BufferHandle != LocateHeapPtr->BufferHandle) {
    if (CurrentNodePtr->NextNodePtr == NULL) {
      BufferHandleFlag = FALSE;
      break;
    } else {
      if (StdHeader->HeapStatus == HEAP_S3_RESUME) {
        HeapBufferPtr = (UINT8 *) CurrentNodePtr;
        CurrentNodePtr = (BUFFER_NODE *) &HeapBufferPtr[CurrentNodePtr->BufferSize + sizeof (BUFFER_NODE) + (2 * SIZE_OF_SENTINEL)];
      } else {
        CurrentNodePtr = CurrentNodePtr->NextNodePtr;
      }
    }
  }

  if (BufferHandleFlag) {
    LocateHeapPtr->BufferPtr = (UINT8 *) ((UINT8 *) CurrentNodePtr + sizeof (BUFFER_NODE) + SIZE_OF_SENTINEL);
    return AGESA_SUCCESS;
  } else {
    LocateHeapPtr->BufferPtr = NULL;
    PutEventLog (AGESA_BOUNDS_CHK,
                 CPU_ERROR_HEAP_BUFFER_HANDLE_IS_NOT_PRESENT,
                 LocateHeapPtr->BufferHandle, 0, 0, 0, StdHeader);
    return AGESA_BOUNDS_CHK;
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 * Determines the base address of the executing core's heap.
 *
 * This function uses the executing core's socket/core numbers to determine
 * where it's heap should be located.
 *
 * @param[in]      StdHeader      Config handle for library and services.
 *
 * @return         A pointer to the executing core's heap.
 *
 */
VOID *
HeapGetCurrentBase (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32      SystemCoreNumber;
  UINT64      ReturnPtr;
  AGESA_STATUS          IgnoredStatus;
  CPU_SPECIFIC_SERVICES *FamilyServices;

  if (IsBsp (StdHeader, &IgnoredStatus)) {
    ReturnPtr = AMD_HEAP_START_ADDRESS;
  } else {
    GetCpuServicesOfCurrentCore (&FamilyServices, StdHeader);
    ASSERT (FamilyServices != NULL);

    SystemCoreNumber = FamilyServices->GetApCoreNumber (FamilyServices, StdHeader);
    ASSERT (SystemCoreNumber != 0);
    ASSERT (SystemCoreNumber < 64);
    ReturnPtr = ((SystemCoreNumber * AMD_HEAP_SIZE_PER_CORE) + AMD_HEAP_START_ADDRESS);
  }
  ASSERT (ReturnPtr <= ((AMD_HEAP_REGION_END_ADDRESS + 1) - AMD_HEAP_SIZE_PER_CORE));
  return ((VOID *) (UINT32) ReturnPtr);
}
