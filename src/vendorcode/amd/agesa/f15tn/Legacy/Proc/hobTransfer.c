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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
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
#include "cpuRegisters.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "cpuCacheInit.h"
#include "cpuFamilyTranslation.h"
#include "heapManager.h"
#include "cpuLateInit.h"
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
    GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
    FamilySpecificServices->GetCacheInfo (FamilySpecificServices, (CONST VOID **) &CacheInfoPtr, &Ignored, StdHeader);

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
    BaseAddressInCache = (UINT8 *) (UINT32)StdHeader->HeapBasePtr;
    HeapManagerInCache = (HEAP_MANAGER *) BaseAddressInCache;
    HeapInCacheOffset = HeapManagerInCache->FirstActiveBufferOffset;
    HeapInCache = (BUFFER_NODE *) (BaseAddressInCache + HeapInCacheOffset);

    BaseAddressInTempMem = (UINT8 *) (UINTN) UserOptions.CfgHeapDramAddress;
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
    BaseAddressInTempMem = (UINT8 *)(UINT32) StdHeader->HeapBasePtr;
    HeapManagerInTempMem = (HEAP_MANAGER *)(UINT32) StdHeader->HeapBasePtr;
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
  if ( (UINTN) StdHeader->HeapBasePtr >= 0x100000) {
    // Find out which variable MTRR was used in CopyHeapToTempRamAtPost.
    GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
    FamilySpecificServices->GetCacheInfo (FamilySpecificServices, (CONST VOID **) &CacheInfoPtr, &Ignored, StdHeader);
    for (HeapRamVariableMtrr = AMD_MTRR_VARIABLE_HEAP_BASE;
         HeapRamVariableMtrr >= AMD_MTRR_VARIABLE_BASE0;
         HeapRamVariableMtrr--) {
      LibAmdMsrRead (HeapRamVariableMtrr, &VariableMtrrBase, StdHeader);
      LibAmdMsrRead ((HeapRamVariableMtrr + 1), &VariableMtrrMask, StdHeader);
      if ((VariableMtrrBase == (UINT64) (UINTN) (StdHeader->HeapBasePtr & CacheInfoPtr->HeapBaseMask)) &&
          (VariableMtrrMask == (UINT64) (CacheInfoPtr->VariableMtrrHeapMask & AMD_HEAP_MTRR_MASK))) {
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
