/* $NoKeywords:$ */
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
 * @e \$Revision: 38448 $   @e \$Date: 2010-09-24 07:13:08 +0800 (Fri, 24 Sep 2010) $
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
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "heapManager.h"
#include "cpuCacheInit.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

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
UINT64
STATIC
HeapGetCurrentBase (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
DeleteFreeSpaceNode (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       UINT32            OffsetOfDeletedNode
  );

VOID
STATIC
InsertFreeSpaceNode (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       UINT32            OffsetOfInsertNode
  );

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
 *  @retval     AGESA_SUCCESS      This core's heap is initialized
 *  @retval     AGESA_FATAL        This core's heap cannot be initialized due to any reasons below:
 *                                 - current processor family cannot be identified.
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
  UINT32                HeapAlreadyRead;
  UINT8                 L2LineSize;
  UINT8                 *HeapBufferPtr;
  UINT8                 *HeapInitPtr;
  UINT32                *HeapDataPtr;
  UINT64                MsrData;
  UINT64                MsrMask;
  UINT8                 Ignored;
  CPUID_DATA            CpuId;
  BUFFER_NODE           *FreeSpaceNode;
  CACHE_INFO            *CacheInfoPtr;
  AGESA_STATUS          IgnoredSts;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  CPU_LOGICAL_ID        CpuFamilyRevision;

  // Check whether this is a known processor family.
  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
  if ((CpuFamilyRevision.Family == 0) && (CpuFamilyRevision.Revision == 0)) {
    IDS_ERROR_TRAP;
    return AGESA_FATAL;
  }

  GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  FamilySpecificServices->GetCacheInfo (FamilySpecificServices, (const VOID **)&CacheInfoPtr, &Ignored, StdHeader);
  HeapBufferPtr = (UINT8 *) StdHeader->HeapBasePtr;

  // Check whether the heap manager is already initialized
  LibAmdMsrRead (AMD_MTRR_VARIABLE_HEAP_MASK, &MsrData, StdHeader);
  if (MsrData == (CacheInfoPtr->VariableMtrrMask & AMD_HEAP_MTRR_MASK)) {
    LibAmdMsrRead (AMD_MTRR_VARIABLE_HEAP_BASE, &MsrData, StdHeader);
    if (((UINT64) (intptr_t) MsrData & CacheInfoPtr->HeapBaseMask) ==
        ((UINT64) (intptr_t) HeapBufferPtr & CacheInfoPtr->HeapBaseMask)) {
      if (((HEAP_MANAGER *) HeapBufferPtr)->Signature == HEAP_SIGNATURE_VALID) {
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
  }

  // Set variable MTRR base and mask
  MsrData = ((UINT64) (intptr_t)HeapBufferPtr & CacheInfoPtr->HeapBaseMask);
  MsrMask = CacheInfoPtr->VariableMtrrHeapMask & AMD_HEAP_MTRR_MASK;

  MsrData |= 0x06;
  LibAmdMsrWrite (AMD_MTRR_VARIABLE_HEAP_BASE, &MsrData, StdHeader);
  LibAmdMsrWrite (AMD_MTRR_VARIABLE_HEAP_MASK, &MsrMask, StdHeader);

  // Set top of memory to a temp value
  MsrData = (UINT64) (AMD_TEMP_TOM);
  LibAmdMsrWrite (TOP_MEM, &MsrData, StdHeader);

  // Enable variable MTTRs
  LibAmdMsrRead (SYS_CFG, &MsrData, StdHeader);
  MsrData |= AMD_VAR_MTRR_ENABLE_BIT;
  LibAmdMsrWrite (SYS_CFG, &MsrData, StdHeader);

  // Initialize Heap Space
  // BIOS may store to a line only after it has been allocated by a load
  LibAmdCpuidRead (AMD_CPUID_L2L3Cache_L2TLB, &CpuId, StdHeader);
  L2LineSize = (UINT8) (CpuId.ECX_Reg);
  HeapInitPtr = HeapBufferPtr ;
  for (HeapAlreadyRead = 0; HeapAlreadyRead < AMD_HEAP_SIZE_PER_CORE;
      (HeapAlreadyRead = HeapAlreadyRead + L2LineSize)) {
    Ignored = *HeapInitPtr;
    HeapInitPtr += L2LineSize;
  }

  HeapDataPtr = (UINT32 *) HeapBufferPtr;
  for (HeapAlreadyInitSizeDword = 0; HeapAlreadyInitSizeDword < AMD_HEAP_SIZE_DWORD_PER_CORE; HeapAlreadyInitSizeDword++) {
    *HeapDataPtr = 0;
    HeapDataPtr++;
  }

  // Note: We are reserving the first 16 bytes for Heap Manager use
  // UsedSize indicates the size of heap spaced is used for HEAP_MANAGER, BUFFER_NODE,
  //   Pad for 16-byte alignment, buffer data, and IDS SENTINEL.
  // FirstActiveBufferOffset is initalized as invalid heap offset, AMD_HEAP_INVALID_HEAP_OFFSET.
  // FirstFreeSpaceOffset is initalized as the byte right after HEAP_MANAGER header.
  // Then we set Signature of HEAP_MANAGER header as valid, HEAP_SIGNATURE_VALID.
  ((HEAP_MANAGER*) HeapBufferPtr)->UsedSize = sizeof (HEAP_MANAGER);
  ((HEAP_MANAGER*) HeapBufferPtr)->FirstActiveBufferOffset = AMD_HEAP_INVALID_HEAP_OFFSET;
  ((HEAP_MANAGER*) HeapBufferPtr)->FirstFreeSpaceOffset = sizeof (HEAP_MANAGER);
  ((HEAP_MANAGER*) HeapBufferPtr)->Signature = HEAP_SIGNATURE_VALID;
  // Create free space link
  FreeSpaceNode = (BUFFER_NODE *) (HeapBufferPtr + sizeof (HEAP_MANAGER));
  FreeSpaceNode->BufferSize = AMD_HEAP_SIZE_PER_CORE - sizeof (HEAP_MANAGER) - sizeof (BUFFER_NODE);
  FreeSpaceNode->OffsetOfNextNode = AMD_HEAP_INVALID_HEAP_OFFSET;

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
 * This function will allocate new buffer either by using internal 'AGESA' heapmanager
 * or by using externa (IBV) heapmanager. This function will also determine if whether or not
 * there is enough space for the new structure. If so, it will zero out the buffer,
 * and return a pointer to the region.
 *
 * @param[in,out]  AllocateHeapParams structure pointer containing the size of the
 *                                   desired new region, its handle, and the
 *                                   return pointer.
 * @param[in,out]  StdHeader         Config handle for library and services.
 *
 * @retval         AGESA_SUCCESS     No error
 * @retval         AGESA_BOUNDS_CHK  Handle already exists, or not enough
 *                                   free space
 * @retval         AGESA_ERROR       Heap is invaild
 *
 */
AGESA_STATUS
HeapAllocateBuffer (
  IN OUT   ALLOCATE_HEAP_PARAMS *AllocateHeapParams,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 *BaseAddress;
  UINT8  AlignTo16Byte;
  UINT32 RemainSize;
  UINT32 OffsetOfSplitNode;
  UINT32 OffsetOfNode;
  HEAP_MANAGER *HeapManager;
  BUFFER_NODE *FreeSpaceNode;
  BUFFER_NODE *SplitFreeSpaceNode;
  BUFFER_NODE *CurrentBufferNode;
  BUFFER_NODE *NewBufferNode;
  AGESA_BUFFER_PARAMS  AgesaBuffer;

  ASSERT (StdHeader != NULL);

  // At this stage we will decide to either use external (IBV) heap manger
  // or internal (AGESA) heap manager.

  // If (HeapStatus == HEAP_SYSTEM_MEM), then use the call function to call
  // external heap manager
  if (StdHeader->HeapStatus == HEAP_SYSTEM_MEM) {
    AgesaBuffer.StdHeader = *StdHeader;
    AgesaBuffer.BufferHandle = AllocateHeapParams->BufferHandle;
    AgesaBuffer.BufferLength = AllocateHeapParams->RequestedBufferSize;

    AGESA_TESTPOINT (TpIfBeforeAllocateHeapBuffer, StdHeader);
    if (AgesaAllocateBuffer (0, &AgesaBuffer) != AGESA_SUCCESS) {
      AllocateHeapParams->BufferPtr = NULL;
      return AGESA_ERROR;
    }
    AGESA_TESTPOINT (TpIfAfterAllocateHeapBuffer, StdHeader);

    AllocateHeapParams->BufferPtr = (UINT8 *) (AgesaBuffer.BufferPointer);
    return AGESA_SUCCESS;
  }

  // If (StdHeader->HeapStatus != HEAP_SYSTEM_MEM), then allocated buffer
  // using following AGESA Heap Manager code.

  // Buffer pointer is NULL unless we return a buffer.
  AlignTo16Byte = 0;
  AllocateHeapParams->BufferPtr = NULL;
  AllocateHeapParams->RequestedBufferSize += NUM_OF_SENTINEL * SIZE_OF_SENTINEL;

  // Get base address
  BaseAddress = (UINT8 *) StdHeader->HeapBasePtr;
  HeapManager = (HEAP_MANAGER *) BaseAddress;

  // Check Heap database is valid
  if ((BaseAddress == NULL) || (HeapManager->Signature != HEAP_SIGNATURE_VALID)) {
    ASSERT (FALSE);
    return AGESA_ERROR;
  }

  // Allocate
  CurrentBufferNode = (BUFFER_NODE *) (BaseAddress + sizeof (HEAP_MANAGER));
  // If there already has been a heap with the incoming BufferHandle, we return AGESA_BOUNDS_CHK.
  if (HeapManager->FirstActiveBufferOffset != AMD_HEAP_INVALID_HEAP_OFFSET) {
    CurrentBufferNode = (BUFFER_NODE *) (BaseAddress + HeapManager->FirstActiveBufferOffset);
    while (CurrentBufferNode->OffsetOfNextNode != AMD_HEAP_INVALID_HEAP_OFFSET) {
      if (CurrentBufferNode->BufferHandle == AllocateHeapParams->BufferHandle) {
      PutEventLog (AGESA_BOUNDS_CHK,
                   CPU_ERROR_HEAP_BUFFER_HANDLE_IS_ALREADY_USED,
                   AllocateHeapParams->BufferHandle, 0, 0, 0, StdHeader);
        return AGESA_BOUNDS_CHK;
      } else {
        CurrentBufferNode = (BUFFER_NODE *) (BaseAddress + CurrentBufferNode->OffsetOfNextNode);
      }
    }
    if (CurrentBufferNode->BufferHandle == AllocateHeapParams->BufferHandle) {
      PutEventLog (AGESA_BOUNDS_CHK,
                   CPU_ERROR_HEAP_BUFFER_HANDLE_IS_ALREADY_USED,
                   AllocateHeapParams->BufferHandle, 0, 0, 0, StdHeader);
      return AGESA_BOUNDS_CHK;
    }
  }

  // Find the buffer size that first matches the requested buffer size (i.e. the first free buffer of greater size).
  OffsetOfNode = HeapManager->FirstFreeSpaceOffset;
  FreeSpaceNode = (BUFFER_NODE *) (BaseAddress + OffsetOfNode);
  while (OffsetOfNode != AMD_HEAP_INVALID_HEAP_OFFSET) {
    AlignTo16Byte = (UINT8) ((0x10 - (((UINTN) (VOID *) FreeSpaceNode + sizeof (BUFFER_NODE) + SIZE_OF_SENTINEL) & 0xF)) & 0xF);
    AllocateHeapParams->RequestedBufferSize = (UINT32) (AllocateHeapParams->RequestedBufferSize + AlignTo16Byte);
    if (FreeSpaceNode->BufferSize >= AllocateHeapParams->RequestedBufferSize) {
      break;
    }
    AllocateHeapParams->RequestedBufferSize = (UINT32) (AllocateHeapParams->RequestedBufferSize - AlignTo16Byte);
    OffsetOfNode = FreeSpaceNode->OffsetOfNextNode;
    FreeSpaceNode = (BUFFER_NODE *) (BaseAddress + OffsetOfNode);
  }
  if (OffsetOfNode == AMD_HEAP_INVALID_HEAP_OFFSET) {
    // We don't find any free space buffer that matches the requested buffer size.
    PutEventLog (AGESA_BOUNDS_CHK,
                 CPU_ERROR_HEAP_IS_FULL,
                 AllocateHeapParams->BufferHandle, 0, 0, 0, StdHeader);
    return AGESA_BOUNDS_CHK;
  } else {
    // We find one matched free space buffer.
    DeleteFreeSpaceNode (StdHeader, OffsetOfNode);
    NewBufferNode = FreeSpaceNode;
    // Add new buffer node to the buffer chain
    if (HeapManager->FirstActiveBufferOffset == AMD_HEAP_INVALID_HEAP_OFFSET) {
      HeapManager->FirstActiveBufferOffset = sizeof (HEAP_MANAGER);
    } else {
      CurrentBufferNode->OffsetOfNextNode = OffsetOfNode;
    }
    // New buffer size
    RemainSize = FreeSpaceNode->BufferSize - AllocateHeapParams->RequestedBufferSize;
    if (RemainSize > sizeof (BUFFER_NODE)) {
      NewBufferNode->BufferSize = AllocateHeapParams->RequestedBufferSize;
      OffsetOfSplitNode = OffsetOfNode + sizeof (BUFFER_NODE) + NewBufferNode->BufferSize;
      SplitFreeSpaceNode = (BUFFER_NODE *) (BaseAddress + OffsetOfSplitNode);
      SplitFreeSpaceNode->BufferSize = RemainSize - sizeof (BUFFER_NODE);
      InsertFreeSpaceNode (StdHeader, OffsetOfSplitNode);
    } else {
      // Remain size is less than BUFFER_NODE, we use whole size instead of requested size.
      NewBufferNode->BufferSize = FreeSpaceNode->BufferSize;
    }
  }

  // Initialize BUFFER_NODE structure of NewBufferNode
  NewBufferNode->BufferHandle = AllocateHeapParams->BufferHandle;
  if ((AllocateHeapParams->Persist == HEAP_TEMP_MEM) || (AllocateHeapParams->Persist == HEAP_SYSTEM_MEM)) {
    NewBufferNode->Persist = AllocateHeapParams->Persist;
  } else {
    NewBufferNode->Persist = HEAP_LOCAL_CACHE;
  }
  NewBufferNode->OffsetOfNextNode = AMD_HEAP_INVALID_HEAP_OFFSET;
  NewBufferNode->PadSize = AlignTo16Byte;

  // Clear to 0x00
  LibAmdMemFill ((VOID *) ((UINT8 *) NewBufferNode + sizeof (BUFFER_NODE)), 0x00, NewBufferNode->BufferSize, StdHeader);

  // Debug feature
  SET_SENTINEL_BEFORE (NewBufferNode, AlignTo16Byte);
  SET_SENTINEL_AFTER (NewBufferNode);

  // Update global variables
  HeapManager->UsedSize += NewBufferNode->BufferSize + sizeof (BUFFER_NODE);

  // Now fill in the incoming structure
  AllocateHeapParams->BufferPtr = (UINT8 *) ((UINT8 *) NewBufferNode + sizeof (BUFFER_NODE) + SIZE_OF_SENTINEL + AlignTo16Byte);
  AllocateHeapParams->RequestedBufferSize -= (NUM_OF_SENTINEL * SIZE_OF_SENTINEL + AlignTo16Byte);

  return AGESA_SUCCESS;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Deallocates a previously allocated buffer in the heap
 *
 * This function will deallocate buffer either by using internal 'AGESA' heapmanager
 * or by using externa (IBV) heapmanager.
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
  UINT8 *BaseAddress;
  UINT32 NodeSize;
  UINT32 OffsetOfFreeSpaceNode;
  UINT32 OffsetOfPreviousNode;
  UINT32 OffsetOfCurrentNode;
  BOOLEAN HeapLocateFlag;
  HEAP_MANAGER *HeapManager;
  BUFFER_NODE *CurrentNode;
  BUFFER_NODE *PreviousNode;
  BUFFER_NODE *FreeSpaceNode;
  AGESA_BUFFER_PARAMS  AgesaBuffer;

  ASSERT (StdHeader != NULL);

  HeapLocateFlag = TRUE;
  BaseAddress = (UINT8 *) StdHeader->HeapBasePtr;
  HeapManager = (HEAP_MANAGER *) BaseAddress;

  // Check Heap database is valid
  if ((BaseAddress == NULL) || (HeapManager->Signature != HEAP_SIGNATURE_VALID)) {
    ASSERT (FALSE);
    return AGESA_ERROR;
  }

  OffsetOfPreviousNode = AMD_HEAP_INVALID_HEAP_OFFSET;
  OffsetOfCurrentNode =  HeapManager->FirstActiveBufferOffset;
  CurrentNode = (BUFFER_NODE *) (BaseAddress + OffsetOfCurrentNode);

  // Locate heap
  if ((BaseAddress != NULL) && (HeapManager->Signature == HEAP_SIGNATURE_VALID)) {
    if (OffsetOfCurrentNode == AMD_HEAP_INVALID_HEAP_OFFSET) {
      HeapLocateFlag = FALSE;
    } else {
      while (CurrentNode->BufferHandle != BufferHandle) {
        if (CurrentNode->OffsetOfNextNode == AMD_HEAP_INVALID_HEAP_OFFSET) {
          HeapLocateFlag = FALSE;
          break;
        } else {
          OffsetOfPreviousNode = OffsetOfCurrentNode;
          OffsetOfCurrentNode = CurrentNode->OffsetOfNextNode;
          CurrentNode = (BUFFER_NODE *) (BaseAddress + OffsetOfCurrentNode);
        }
      }
    }
  } else {
    HeapLocateFlag = FALSE;
  }

  if (HeapLocateFlag == TRUE) {
    // CurrentNode points to the buffer which wanted to be deallocated.
    // Remove deallocated heap from active buffer chain.
    if (OffsetOfPreviousNode == AMD_HEAP_INVALID_HEAP_OFFSET) {
      HeapManager->FirstActiveBufferOffset = CurrentNode->OffsetOfNextNode;
    } else {
      PreviousNode = (BUFFER_NODE *) (BaseAddress + OffsetOfPreviousNode);
      PreviousNode->OffsetOfNextNode = CurrentNode->OffsetOfNextNode;
    }
    // Now, CurrentNode become a free space node.
    HeapManager->UsedSize -= CurrentNode->BufferSize + sizeof (BUFFER_NODE);
    // Loop free space chain to see if any free space node is just before/after CurrentNode, then merge them.
    OffsetOfFreeSpaceNode = HeapManager->FirstFreeSpaceOffset;
    FreeSpaceNode = (BUFFER_NODE *) (BaseAddress + OffsetOfFreeSpaceNode);
    while (OffsetOfFreeSpaceNode != AMD_HEAP_INVALID_HEAP_OFFSET) {
      if ((OffsetOfFreeSpaceNode + sizeof (BUFFER_NODE) + FreeSpaceNode->BufferSize) == OffsetOfCurrentNode) {
        DeleteFreeSpaceNode (StdHeader, OffsetOfFreeSpaceNode);
        NodeSize = FreeSpaceNode->BufferSize + CurrentNode->BufferSize + sizeof (BUFFER_NODE);
        OffsetOfCurrentNode = OffsetOfFreeSpaceNode;
        CurrentNode = FreeSpaceNode;
        CurrentNode->BufferSize = NodeSize;
      } else if (OffsetOfFreeSpaceNode == (OffsetOfCurrentNode + sizeof (BUFFER_NODE) + CurrentNode->BufferSize)) {
        DeleteFreeSpaceNode (StdHeader, OffsetOfFreeSpaceNode);
        NodeSize = FreeSpaceNode->BufferSize + CurrentNode->BufferSize + sizeof (BUFFER_NODE);
        CurrentNode->BufferSize = NodeSize;
      }
      OffsetOfFreeSpaceNode = FreeSpaceNode->OffsetOfNextNode;
      FreeSpaceNode = (BUFFER_NODE *) (BaseAddress + OffsetOfFreeSpaceNode);
    }
    InsertFreeSpaceNode (StdHeader, OffsetOfCurrentNode);
    return AGESA_SUCCESS;
  } else {
    // If HeapStatus == HEAP_SYSTEM_MEM, try callout function
    if (StdHeader->HeapStatus == HEAP_SYSTEM_MEM) {
      AgesaBuffer.StdHeader = *StdHeader;
      AgesaBuffer.BufferHandle = BufferHandle;

      AGESA_TESTPOINT (TpIfBeforeDeallocateHeapBuffer, StdHeader);
      if (AgesaDeallocateBuffer (0, &AgesaBuffer) != AGESA_SUCCESS) {
        return AGESA_ERROR;
      }
      AGESA_TESTPOINT (TpIfAfterDeallocateHeapBuffer, StdHeader);

      return AGESA_SUCCESS;
    }
    // If we are still unable to locate the buffer handle, return AGESA_BOUNDS_CHK
    if ((BaseAddress != NULL) && (HeapManager->Signature == HEAP_SIGNATURE_VALID)) {
      PutEventLog (AGESA_BOUNDS_CHK,
                 CPU_ERROR_HEAP_BUFFER_HANDLE_IS_NOT_PRESENT,
                 BufferHandle, 0, 0, 0, StdHeader);
    } else {
      ASSERT (FALSE);
    }
    return AGESA_BOUNDS_CHK;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Locates a previously allocated buffer on the heap.
 *
 * This function searches the heap for a buffer with the desired handle, and
 * returns a pointer to the buffer.
 *
 * @param[in,out]  LocateHeap     Structure containing the buffer's handle,
 *                                   and the return pointer.
 * @param[in]      StdHeader         Config handle for library and services.
 *
 * @retval         AGESA_SUCCESS     No error
 * @retval         AGESA_BOUNDS_CHK  Handle does not exist on the heap
 *
 */
AGESA_STATUS
HeapLocateBuffer (
  IN OUT   LOCATE_HEAP_PTR *LocateHeap,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 *BaseAddress;
  UINT8  AlignTo16Byte;
  UINT32 OffsetOfCurrentNode;
  BOOLEAN HeapLocateFlag;
  HEAP_MANAGER *HeapManager;
  BUFFER_NODE *CurrentNode;
  AGESA_BUFFER_PARAMS  AgesaBuffer;

  ASSERT (StdHeader != NULL);

  HeapLocateFlag = TRUE;
  BaseAddress = (UINT8 *) StdHeader->HeapBasePtr;
  HeapManager = (HEAP_MANAGER *) BaseAddress;

  // Check Heap database is valid
  if ((BaseAddress == NULL) || (HeapManager->Signature != HEAP_SIGNATURE_VALID)) {
    ASSERT (FALSE);
    return AGESA_ERROR;
  }
  OffsetOfCurrentNode =  HeapManager->FirstActiveBufferOffset;
  CurrentNode = (BUFFER_NODE *) (BaseAddress + OffsetOfCurrentNode);

  // Find buffer using internal heap manager
  // Locate the heap using handle = LocateHeap-> BufferHandle
  // If HeapStatus != HEAP_SYSTEM_ MEM
  if ((BaseAddress != NULL) && (HeapManager->Signature == HEAP_SIGNATURE_VALID)) {
    if (OffsetOfCurrentNode == AMD_HEAP_INVALID_HEAP_OFFSET) {
      HeapLocateFlag = FALSE;
    } else {
      while (CurrentNode->BufferHandle != LocateHeap->BufferHandle) {
        if (CurrentNode->OffsetOfNextNode == AMD_HEAP_INVALID_HEAP_OFFSET) {
          HeapLocateFlag = FALSE;
          break;
        } else {
          OffsetOfCurrentNode = CurrentNode->OffsetOfNextNode;
          CurrentNode = (BUFFER_NODE *) (BaseAddress + OffsetOfCurrentNode);
        }
      }
    }
  } else {
    HeapLocateFlag = FALSE;
  }

  if (HeapLocateFlag) {
    AlignTo16Byte = CurrentNode->PadSize;
    LocateHeap->BufferPtr = (UINT8 *) ((UINT8 *) CurrentNode + sizeof (BUFFER_NODE) + SIZE_OF_SENTINEL + AlignTo16Byte);
    LocateHeap->BufferSize = CurrentNode->BufferSize - NUM_OF_SENTINEL * SIZE_OF_SENTINEL - AlignTo16Byte;
    return AGESA_SUCCESS;
  } else {
    // If HeapStatus == HEAP_SYSTEM_MEM, try callout function
    if (StdHeader->HeapStatus == HEAP_SYSTEM_MEM) {
      AgesaBuffer.StdHeader = *StdHeader;
      AgesaBuffer.BufferHandle = LocateHeap->BufferHandle;

      AGESA_TESTPOINT (TpIfBeforeLocateHeapBuffer, StdHeader);
      if (AgesaLocateBuffer (0, &AgesaBuffer) != AGESA_SUCCESS) {
        LocateHeap->BufferPtr = NULL;
        return AGESA_ERROR;
      }
      LocateHeap->BufferSize = AgesaBuffer.BufferLength;
      AGESA_TESTPOINT (TpIfAfterLocateHeapBuffer, StdHeader);

      LocateHeap->BufferPtr = (UINT8 *) (AgesaBuffer.BufferPointer);
      return AGESA_SUCCESS;
    }

    // If we are still unable to deallocate the buffer handle, return AGESA_BOUNDS_CHK
    LocateHeap->BufferPtr = NULL;
    LocateHeap->BufferSize = 0;
    if ((BaseAddress != NULL) && (HeapManager->Signature == HEAP_SIGNATURE_VALID)) {
      PutEventLog (AGESA_BOUNDS_CHK,
                   CPU_ERROR_HEAP_BUFFER_HANDLE_IS_NOT_PRESENT,
                   LocateHeap->BufferHandle, 0, 0, 0, StdHeader);
    } else {
      ASSERT (FALSE);
    }
    return AGESA_BOUNDS_CHK;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Get the heap base address
 *
 * This function will try to locate heap from cache, temp memory, main memory.
 * The heap signature will be checked for validity on each possible location.
 * Firstly, try if heap base is in cache by calling the function HeapGetCurrentBase.
 * Secondly, try if heap base is temp memory by UserOptoions.CfgHeapDramAddress.
 * Thirdly, try if heap base is in main memory by doing a buffer locate with buffer handle
 * AMD_HEAP_IN_MAIN_MEMORY_HANDLE.
 * If no valid heap signature is found in each possible location above, a NULL pointer is returned.
 *
 * @param[in]      StdHeader      Config handle for library and services.
 *
 * @return         Heap base address of the executing core's heap.
 *
 */
VOID
*HeapGetBaseAddress (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 BaseAddress;
  HEAP_MANAGER *HeapManager;
  AGESA_BUFFER_PARAMS AgesaBuffer;

  // Firstly, we try to see if heap is in cache
  BaseAddress = HeapGetCurrentBase (StdHeader);
  HeapManager = (HEAP_MANAGER *) (intptr_t) BaseAddress;

  if ((HeapManager->Signature != HEAP_SIGNATURE_VALID) &&
      (StdHeader->HeapStatus != HEAP_DO_NOT_EXIST_YET) &&
      (StdHeader->HeapStatus != HEAP_LOCAL_CACHE)) {
    // Secondly, we try to see if heap is in temp memory
    BaseAddress = UserOptions.CfgHeapDramAddress;
    HeapManager = (HEAP_MANAGER *) (intptr_t) BaseAddress;
    if (HeapManager->Signature != HEAP_SIGNATURE_VALID) {
      // Thirdly, we try to see if heap in main memory
      // by locating with external buffer manager (IBV)
      AgesaBuffer.StdHeader = *StdHeader;
      AgesaBuffer.BufferHandle = AMD_HEAP_IN_MAIN_MEMORY_HANDLE;
      if (AgesaLocateBuffer (0, &AgesaBuffer) == AGESA_SUCCESS) {
        BaseAddress = (UINT64) (intptr_t) AgesaBuffer.BufferPointer;
        HeapManager = (HEAP_MANAGER *) (intptr_t) BaseAddress;
        if (HeapManager->Signature != HEAP_SIGNATURE_VALID) {
          // No valid heap signature ever found, return a NULL pointer
          BaseAddress = (UINT64) (intptr_t) NULL;
        }
      } else {
        // No heap buffer is allocated by external manager (IBV), return a NULL pointer
        BaseAddress = (UINT64) (intptr_t) NULL;
      }
    }
  }

  return (void *) (intptr_t) BaseAddress;
}

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *  DeleteFreeSpaceNode
 *
 *  Description:
 *    Delete a free space node from free space chain
 *
 *  Parameters:
 * @param[in]      StdHeader             Config handle for library and services.
 * @param[in]      OffsetOfDeletedNode   Offset of deleted node.
 *
 *  Processing:
 *
 */
VOID
STATIC
DeleteFreeSpaceNode (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       UINT32            OffsetOfDeletedNode
  )
{
  UINT8 *BaseAddress;
  UINT32 OffsetOfPreviousNode;
  UINT32 OffsetOfCurrentNode;
  HEAP_MANAGER *HeapManager;
  BUFFER_NODE *CurrentFreeSpaceNode;
  BUFFER_NODE *PreviousFreeSpaceNode;


  BaseAddress = (UINT8 *) StdHeader->HeapBasePtr;
  HeapManager = (HEAP_MANAGER *) BaseAddress;

  OffsetOfPreviousNode = AMD_HEAP_INVALID_HEAP_OFFSET;
  OffsetOfCurrentNode = HeapManager->FirstFreeSpaceOffset;
  //
  // After AmdInitEnv, there is no free space provided for HeapAllocateBuffer.
  // Hence if the FirstFreeSpaceOffset is AMD_HEAP_INVALID_HEAP_OFFSET, then
  // no need to do more on delete node.
  //
  if (OffsetOfCurrentNode != AMD_HEAP_INVALID_HEAP_OFFSET) {
    CurrentFreeSpaceNode = (BUFFER_NODE *) (BaseAddress + OffsetOfCurrentNode);
    while ((OffsetOfCurrentNode != AMD_HEAP_INVALID_HEAP_OFFSET) && (OffsetOfCurrentNode != OffsetOfDeletedNode)) {
      OffsetOfPreviousNode = OffsetOfCurrentNode;
      OffsetOfCurrentNode = CurrentFreeSpaceNode->OffsetOfNextNode;
      CurrentFreeSpaceNode = (BUFFER_NODE *) (BaseAddress + OffsetOfCurrentNode);
    }
    if (OffsetOfCurrentNode != AMD_HEAP_INVALID_HEAP_OFFSET) {
      if (OffsetOfPreviousNode == AMD_HEAP_INVALID_HEAP_OFFSET) {
        HeapManager->FirstFreeSpaceOffset = CurrentFreeSpaceNode->OffsetOfNextNode;
      } else {
        PreviousFreeSpaceNode = (BUFFER_NODE *) (BaseAddress + OffsetOfPreviousNode);
        PreviousFreeSpaceNode->OffsetOfNextNode = CurrentFreeSpaceNode->OffsetOfNextNode;
      }
    }
  }
  return;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  InsertFreeSpaceNode
 *
 *  Description:
 *    Insert a free space node to free space chain, size order
 *
 *  Parameters:
 * @param[in]      StdHeader             Config handle for library and services.
 * @param[in]      OffsetOfInsertNode    Offset of inserted node.
 *
 *  Processing:
 *
 */
VOID
STATIC
InsertFreeSpaceNode (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       UINT32            OffsetOfInsertNode
  )
{
  UINT8 *BaseAddress;
  UINT32 OffsetOfPreviousNode;
  UINT32 OffsetOfCurrentNode;
  HEAP_MANAGER *HeapManager;
  BUFFER_NODE *CurrentFreeSpaceNode;
  BUFFER_NODE *PreviousFreeSpaceNode;
  BUFFER_NODE *FreeSpaceInsertNode;

  BaseAddress = (UINT8 *) StdHeader->HeapBasePtr;
  HeapManager = (HEAP_MANAGER *) BaseAddress;

  OffsetOfPreviousNode = AMD_HEAP_INVALID_HEAP_OFFSET;
  OffsetOfCurrentNode = HeapManager->FirstFreeSpaceOffset;
  CurrentFreeSpaceNode = (BUFFER_NODE *) (BaseAddress + OffsetOfCurrentNode);
  FreeSpaceInsertNode = (BUFFER_NODE *) (BaseAddress + OffsetOfInsertNode);
  while ((OffsetOfCurrentNode != AMD_HEAP_INVALID_HEAP_OFFSET) &&
         (CurrentFreeSpaceNode->BufferSize < FreeSpaceInsertNode->BufferSize)) {
    OffsetOfPreviousNode = OffsetOfCurrentNode;
    OffsetOfCurrentNode = CurrentFreeSpaceNode->OffsetOfNextNode;
    CurrentFreeSpaceNode = (BUFFER_NODE *) (BaseAddress + OffsetOfCurrentNode);
  }
  FreeSpaceInsertNode->OffsetOfNextNode = OffsetOfCurrentNode;
  if (OffsetOfPreviousNode == AMD_HEAP_INVALID_HEAP_OFFSET) {
    HeapManager->FirstFreeSpaceOffset = OffsetOfInsertNode;
  } else {
    PreviousFreeSpaceNode = (BUFFER_NODE *) (BaseAddress + OffsetOfPreviousNode);
    PreviousFreeSpaceNode->OffsetOfNextNode = OffsetOfInsertNode;
  }
  return;
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
UINT64
STATIC
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
    GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilyServices, StdHeader);
    ASSERT (FamilyServices != NULL);

    SystemCoreNumber = FamilyServices->GetApCoreNumber (FamilyServices, StdHeader);
    ASSERT (SystemCoreNumber != 0);
    ASSERT (SystemCoreNumber < 64);
    ReturnPtr = ((SystemCoreNumber * AMD_HEAP_SIZE_PER_CORE) + AMD_HEAP_START_ADDRESS);
  }
  ASSERT (ReturnPtr <= ((AMD_HEAP_REGION_END_ADDRESS + 1) - AMD_HEAP_SIZE_PER_CORE));
  return ReturnPtr;
}


