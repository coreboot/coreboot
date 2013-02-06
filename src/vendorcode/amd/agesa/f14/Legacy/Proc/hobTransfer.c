/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Hob Transfer functions.
 *
 * Contains code that copy Heap to temp memory or main memory.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
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
#include "cpuRegisters.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "cpuCacheInit.h"
#include "cpuFamilyTranslation.h"
#include "heapManager.h"
//#include "cpuLateInit.h"
#include "cpuEnvInit.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LEGACY_PROC_HOBTRANSFER_FILECODE
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

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CopyHeapToTempRamAtPost
 *
 *     This function copies BSP heap content to RAM
 *
 *    @param[in,out]   StdHeader   - Pointer to AMD_CONFIG_PARAMS struct.
 *
 *    @retval          AGESA_STATUS
 *
 */
AGESA_STATUS
CopyHeapToTempRamAtPost (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 *BaseAddressInCache;
  UINT8 *BaseAddressInTempMem;
  UINT8 *Source;
  UINT8 *Destination;
  UINT8  AlignTo16ByteInCache;
  UINT8  AlignTo16ByteInTempMem;
  UINT8  Ignored;
  UINT32 SizeOfNodeData;
  UINT32 TotalSize;
  UINT32 HeapRamFixMtrr;
  UINT32 HeapRamVariableMtrr;
  UINT32 HeapInCacheOffset;
  UINT64 MsrData;
  UINT64 VariableMtrrBase;
  UINT64 VariableMtrrMask;
  UINTN  AmdHeapRamAddress;
  AGESA_STATUS IgnoredStatus;
  BUFFER_NODE *HeapInCache;
  BUFFER_NODE *HeapInTempMem;
  HEAP_MANAGER *HeapManagerInCache;
  HEAP_MANAGER *HeapManagerInTempMem;
  CACHE_INFO *CacheInfoPtr;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  AmdHeapRamAddress = (UINTN) UserOptions.CfgHeapDramAddress;
  //
  //If the user define address above 1M, Mem Init has already set
  //whole available memory as WB cacheable.
  //
  if (AmdHeapRamAddress < 0x100000) {
    // Region below 1MB
    // Fixed MTTR region
    // turn on modification bit
    LibAmdMsrRead (MSR_SYS_CFG, &MsrData, StdHeader);
    MsrData |= 0x80000;
    LibAmdMsrWrite (MSR_SYS_CFG, &MsrData, StdHeader);

    if (AmdHeapRamAddress >= 0xC0000) {
      //
      // 0xC0000 ~ 0xFFFFF
      //
      HeapRamFixMtrr = (UINT32) (AMD_MTRR_FIX4k_C0000 + (((AmdHeapRamAddress >> 16) & 0x3) * 2));
      MsrData = AMD_MTRR_FIX4K_UC_DRAM;
      LibAmdMsrWrite (HeapRamFixMtrr, &MsrData, StdHeader);
      LibAmdMsrWrite ((HeapRamFixMtrr + 1), &MsrData, StdHeader);
    } else if (AmdHeapRamAddress >= 0x80000) {
      //
      // 0x80000~0xBFFFF
      //
      HeapRamFixMtrr = (UINT32) (AMD_MTRR_FIX16k_80000 + ((AmdHeapRamAddress >> 17) & 0x1));
      MsrData = AMD_MTRR_FIX16K_UC_DRAM;
      LibAmdMsrWrite (HeapRamFixMtrr, &MsrData, StdHeader);
    } else {
      //
      // 0x0 ~ 0x7FFFF
      //
      LibAmdMsrRead (AMD_MTRR_FIX64k_00000, &MsrData, StdHeader);
      MsrData = MsrData & (~(0xFF << (8 * ((AmdHeapRamAddress >> 16) & 0x7))));
      MsrData = MsrData | (AMD_MTRR_FIX64K_UC_DRAM << (8 * ((AmdHeapRamAddress >> 16) & 0x7)));
      LibAmdMsrWrite (AMD_MTRR_FIX64k_00000, &MsrData, StdHeader);
    }

    // Turn on MTTR enable bit and turn off modification bit
    LibAmdMsrRead (MSR_SYS_CFG, &MsrData, StdHeader);
    MsrData |= 0x40000;
    MsrData &= 0xFFFFFFFFFFF7FFFF;
    LibAmdMsrWrite (MSR_SYS_CFG, &MsrData, StdHeader);
  } else {
    // Region above 1MB
    // Variable MTTR region
    // Get family specific cache Info
    GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
    FamilySpecificServices->GetCacheInfo (FamilySpecificServices, (const VOID **)&CacheInfoPtr, &Ignored, StdHeader);

    // Find an empty MTRRphysBase/MTRRphysMask
    for (HeapRamVariableMtrr = AMD_MTRR_VARIABLE_HEAP_BASE;
         HeapRamVariableMtrr >= AMD_MTRR_VARIABLE_BASE0;
         HeapRamVariableMtrr--) {
      LibAmdMsrRead (HeapRamVariableMtrr, &VariableMtrrBase, StdHeader);
      LibAmdMsrRead ((HeapRamVariableMtrr + 1), &VariableMtrrMask, StdHeader);
      if ((VariableMtrrBase == 0) && (VariableMtrrMask == 0)) {
        break;
      }
    }
    if (HeapRamVariableMtrr < AMD_MTRR_VARIABLE_BASE0) {
      // All variable MTRR is used.
      ASSERT (FALSE);
    }

    // Set variable MTRR base and mask
    // If the address ranges of two or more MTRRs overlap
    // and if at least one of the memory types is UC, the UC memory type is used.
    VariableMtrrBase = (UINT64) (AmdHeapRamAddress & CacheInfoPtr->HeapBaseMask);
    VariableMtrrMask = CacheInfoPtr->VariableMtrrHeapMask & AMD_HEAP_MTRR_MASK;
    LibAmdMsrWrite (HeapRamVariableMtrr, &VariableMtrrBase, StdHeader);
    LibAmdMsrWrite ((HeapRamVariableMtrr + 1), &VariableMtrrMask, StdHeader);
  }
  // Copying Heap content
  if (IsBsp (StdHeader, &IgnoredStatus)) {
    TotalSize = sizeof (HEAP_MANAGER);
    SizeOfNodeData = 0;
    AlignTo16ByteInTempMem = 0;
    BaseAddressInCache = (UINT8 *) StdHeader->HeapBasePtr;
    HeapManagerInCache = (HEAP_MANAGER *) BaseAddressInCache;
    HeapInCacheOffset = HeapManagerInCache->FirstActiveBufferOffset;
    HeapInCache = (BUFFER_NODE *) (BaseAddressInCache + HeapInCacheOffset);

    BaseAddressInTempMem = (UINT8 *) (intptr_t) (UserOptions.CfgHeapDramAddress);
    HeapManagerInTempMem = (HEAP_MANAGER *) BaseAddressInTempMem;
    HeapInTempMem = (BUFFER_NODE *) (BaseAddressInTempMem + TotalSize);

    // copy heap from cache to temp memory.
    // only heap with persist great than HEAP_LOCAL_CACHE will be copied.
    // Note: Only copy heap with persist greater than HEAP_LOCAL_CACHE.
    while (HeapInCacheOffset != AMD_HEAP_INVALID_HEAP_OFFSET) {
      if (HeapInCache->Persist > HEAP_LOCAL_CACHE) {
        AlignTo16ByteInCache = HeapInCache->PadSize;
        AlignTo16ByteInTempMem = (UINT8) ((0x10 - (((UINTN) (VOID *) HeapInTempMem + sizeof (BUFFER_NODE) + SIZE_OF_SENTINEL) & 0xF)) & 0xF);
        SizeOfNodeData = HeapInCache->BufferSize - AlignTo16ByteInCache;
        TotalSize = (UINT32) (TotalSize  + sizeof (BUFFER_NODE) + SizeOfNodeData + AlignTo16ByteInTempMem);
        Source = (UINT8 *) HeapInCache + sizeof (BUFFER_NODE) + AlignTo16ByteInCache;
        Destination = (UINT8 *) HeapInTempMem + sizeof (BUFFER_NODE) + AlignTo16ByteInTempMem;
        LibAmdMemCopy  (HeapInTempMem, HeapInCache, sizeof (BUFFER_NODE), StdHeader);
        LibAmdMemCopy  (Destination, Source, SizeOfNodeData, StdHeader);
        HeapInTempMem->OffsetOfNextNode = TotalSize;
        HeapInTempMem->BufferSize = SizeOfNodeData + AlignTo16ByteInTempMem;
        HeapInTempMem->PadSize = AlignTo16ByteInTempMem;
        HeapInTempMem = (BUFFER_NODE *) (BaseAddressInTempMem + TotalSize);
      }
      HeapInCacheOffset = HeapInCache->OffsetOfNextNode;
      HeapInCache = (BUFFER_NODE *) (BaseAddressInCache + HeapInCacheOffset);
    }
    // initialize heap manager
    if (TotalSize == sizeof (HEAP_MANAGER)) {
      // heap is empty
      HeapManagerInTempMem->UsedSize = sizeof (HEAP_MANAGER);
      HeapManagerInTempMem->FirstActiveBufferOffset = AMD_HEAP_INVALID_HEAP_OFFSET;
      HeapManagerInTempMem->FirstFreeSpaceOffset = sizeof (HEAP_MANAGER);
    } else {
      // heap is NOT empty
      HeapManagerInTempMem->UsedSize = TotalSize;
      HeapManagerInTempMem->FirstActiveBufferOffset = sizeof (HEAP_MANAGER);
      HeapManagerInTempMem->FirstFreeSpaceOffset = TotalSize;
      HeapInTempMem = (BUFFER_NODE *) (BaseAddressInTempMem + TotalSize - SizeOfNodeData - AlignTo16ByteInTempMem - sizeof (BUFFER_NODE));
      HeapInTempMem->OffsetOfNextNode = AMD_HEAP_INVALID_HEAP_OFFSET;
      HeapInTempMem = (BUFFER_NODE *) (BaseAddressInTempMem + TotalSize);
    }
    // heap signature
    HeapManagerInCache->Signature = 0x00000000;
    HeapManagerInTempMem->Signature = HEAP_SIGNATURE_VALID;
    // Free space node
    HeapInTempMem->BufferSize = (UINT32) (AMD_HEAP_SIZE_PER_CORE - TotalSize);
    HeapInTempMem->OffsetOfNextNode = AMD_HEAP_INVALID_HEAP_OFFSET;
  }
  return AGESA_SUCCESS;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *  CopyHeapToMainRamAtPost
 *
 *    This function copies Temp Ram heap content to Main Ram
 *
 *    @param[in,out]   StdHeader   - Pointer to AMD_CONFIG_PARAMS struct.
 *
 *    @retval          AGESA_STATUS
 *
 */
AGESA_STATUS
CopyHeapToMainRamAtPost (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 *BaseAddressInTempMem;
  UINT8 *BaseAddressInMainMem;
  UINT8 *Source;
  UINT8 *Destination;
  UINT8  AlignTo16ByteInTempMem;
  UINT8  AlignTo16ByteInMainMem;
  UINT8  Ignored;
  UINT32 SizeOfNodeData;
  UINT32 TotalSize;
  UINT32 HeapInTempMemOffset;
  UINT32 HeapRamVariableMtrr;
  UINT64 VariableMtrrBase;
  UINT64 VariableMtrrMask;
  AGESA_STATUS IgnoredStatus;
  BUFFER_NODE *HeapInTempMem;
  BUFFER_NODE *HeapInMainMem;
  HEAP_MANAGER *HeapManagerInTempMem;
  HEAP_MANAGER *HeapManagerInMainMem;
  AGESA_BUFFER_PARAMS AgesaBuffer;
  CACHE_INFO *CacheInfoPtr;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  if (IsBsp (StdHeader, &IgnoredStatus)) {
    TotalSize = sizeof (HEAP_MANAGER);
    SizeOfNodeData = 0;
    AlignTo16ByteInMainMem = 0;
    BaseAddressInTempMem = (UINT8 *) StdHeader->HeapBasePtr;
    HeapManagerInTempMem = (HEAP_MANAGER *) StdHeader->HeapBasePtr;
    HeapInTempMemOffset = HeapManagerInTempMem->FirstActiveBufferOffset;
    HeapInTempMem = (BUFFER_NODE *) (BaseAddressInTempMem + HeapInTempMemOffset);

    AgesaBuffer.StdHeader = *StdHeader;
    AgesaBuffer.BufferHandle = AMD_HEAP_IN_MAIN_MEMORY_HANDLE;
    AgesaBuffer.BufferLength = AMD_HEAP_SIZE_PER_CORE;
    if (AgesaAllocateBuffer (0, &AgesaBuffer) != AGESA_SUCCESS) {
      return AGESA_ERROR;
    }
    BaseAddressInMainMem = (UINT8 *) AgesaBuffer.BufferPointer;
    HeapManagerInMainMem = (HEAP_MANAGER *) BaseAddressInMainMem;
    HeapInMainMem = (BUFFER_NODE *) (BaseAddressInMainMem + TotalSize);
    LibAmdMemFill (BaseAddressInMainMem, 0x00, AMD_HEAP_SIZE_PER_CORE, StdHeader);
    // copy heap from temp memory to main memory.
    // only heap with persist great than HEAP_TEMP_MEM will be copied.
    // Note: Only copy heap buffers with persist greater than HEAP_TEMP_MEM.
    while (HeapInTempMemOffset != AMD_HEAP_INVALID_HEAP_OFFSET) {
      if (HeapInTempMem->Persist > HEAP_TEMP_MEM) {
        AlignTo16ByteInTempMem = HeapInTempMem->PadSize;
        AlignTo16ByteInMainMem = (UINT8) ((0x10 - (((UINTN) (VOID *) HeapInMainMem  + sizeof (BUFFER_NODE) + SIZE_OF_SENTINEL) & 0xF)) & 0xF);
        SizeOfNodeData = HeapInTempMem->BufferSize - AlignTo16ByteInTempMem;
        TotalSize = (UINT32) (TotalSize  + sizeof (BUFFER_NODE) + SizeOfNodeData + AlignTo16ByteInMainMem);
        Source = (UINT8 *) HeapInTempMem + sizeof (BUFFER_NODE) + AlignTo16ByteInTempMem;
        Destination = (UINT8 *) HeapInMainMem + sizeof (BUFFER_NODE) + AlignTo16ByteInMainMem;
        LibAmdMemCopy  (HeapInMainMem, HeapInTempMem, sizeof (BUFFER_NODE), StdHeader);
        LibAmdMemCopy  (Destination, Source, SizeOfNodeData, StdHeader);
        HeapInMainMem->OffsetOfNextNode = TotalSize;
        HeapInMainMem->BufferSize = SizeOfNodeData + AlignTo16ByteInMainMem;
        HeapInMainMem->PadSize = AlignTo16ByteInMainMem;
        HeapInMainMem = (BUFFER_NODE *) (BaseAddressInMainMem + TotalSize);
      }
      HeapInTempMemOffset = HeapInTempMem->OffsetOfNextNode;
      HeapInTempMem = (BUFFER_NODE *) (BaseAddressInTempMem + HeapInTempMemOffset);
    }
    // initialize heap manager
    if (TotalSize == sizeof (HEAP_MANAGER)) {
      // heap is empty
      HeapManagerInMainMem->UsedSize = sizeof (HEAP_MANAGER);
      HeapManagerInMainMem->FirstActiveBufferOffset = AMD_HEAP_INVALID_HEAP_OFFSET;
      HeapManagerInMainMem->FirstFreeSpaceOffset = sizeof (HEAP_MANAGER);
    } else {
      // heap is NOT empty
      HeapManagerInMainMem->UsedSize = TotalSize;
      HeapManagerInMainMem->FirstActiveBufferOffset = sizeof (HEAP_MANAGER);
      HeapManagerInMainMem->FirstFreeSpaceOffset = TotalSize;
      HeapInMainMem = (BUFFER_NODE *) (BaseAddressInMainMem + TotalSize - SizeOfNodeData - AlignTo16ByteInMainMem - sizeof (BUFFER_NODE));
      HeapInMainMem->OffsetOfNextNode = AMD_HEAP_INVALID_HEAP_OFFSET;
      HeapInMainMem = (BUFFER_NODE *) (BaseAddressInMainMem + TotalSize);
    }
    // heap signature
    HeapManagerInTempMem->Signature = 0x00000000;
    HeapManagerInMainMem->Signature = HEAP_SIGNATURE_VALID;
    // Free space node
    HeapInMainMem->BufferSize = AMD_HEAP_SIZE_PER_CORE - TotalSize;
    HeapInMainMem->OffsetOfNextNode = AMD_HEAP_INVALID_HEAP_OFFSET;
  }
  // if address of heap in temp memory is above 1M, then we must used one variable MTRR.
  if (StdHeader->HeapBasePtr >= (void *) 0x100000) {
    // Find out which variable MTRR was used in CopyHeapToTempRamAtPost.
    GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
    FamilySpecificServices->GetCacheInfo (FamilySpecificServices, (const VOID **)&CacheInfoPtr, &Ignored, StdHeader);
    for (HeapRamVariableMtrr = AMD_MTRR_VARIABLE_HEAP_BASE;
         HeapRamVariableMtrr >= AMD_MTRR_VARIABLE_BASE0;
         HeapRamVariableMtrr--) {
      LibAmdMsrRead (HeapRamVariableMtrr, &VariableMtrrBase, StdHeader);
      LibAmdMsrRead ((HeapRamVariableMtrr + 1), &VariableMtrrMask, StdHeader);
      if ((VariableMtrrBase == ((UINT64) (intptr_t) (StdHeader->HeapBasePtr) & CacheInfoPtr->HeapBaseMask)) &&
          (VariableMtrrMask == (CacheInfoPtr->VariableMtrrHeapMask & AMD_HEAP_MTRR_MASK))) {
        break;
      }
    }
    if (HeapRamVariableMtrr >= AMD_MTRR_VARIABLE_BASE0) {
      // Clear variable MTRR which set in CopyHeapToTempRamAtPost.
      VariableMtrrBase = 0;
      VariableMtrrMask = 0;
      LibAmdMsrWrite (HeapRamVariableMtrr, &VariableMtrrBase, StdHeader);
      LibAmdMsrWrite ((HeapRamVariableMtrr + 1), &VariableMtrrMask, StdHeader);
    }
  }
  return AGESA_SUCCESS;
}
