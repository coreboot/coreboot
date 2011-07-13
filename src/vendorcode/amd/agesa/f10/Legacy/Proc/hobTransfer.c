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
 * @e \$Revision: 7532 $   @e \$Date: 2008-08-22 04:50:27 +0800 (Fri, 22 Aug 2008) $
 *
 */
/*
 ******************************************************************************
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
#include "GeneralServices.h"
#include "heapManager.h"
#include "cpuPostInit.h"
#include "cpuEnvInit.h"
#include "Filecode.h"
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
  UINT32  i;
  UINT32  *HeapBufferPtr;
  UINT32  *HeapRamPtr;
  UINT64  MsrData;
  HEAP_MANAGER *HeapManagerPtr;
  BUFFER_NODE *CurrentNodePtr;
  AGESA_STATUS IgnoredSts;
  UINT32  AmdHeapRamAddress;
  UINT32  HeapRamFixMtrr;

  AmdHeapRamAddress = (UINT32) UserOptions.CfgHeapDramAddress;

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
      MsrData = AMD_MTRR_FIX4K_WB_DRAM;
      LibAmdMsrWrite (HeapRamFixMtrr, &MsrData, StdHeader);
      LibAmdMsrWrite ((HeapRamFixMtrr + 1), &MsrData, StdHeader);
    } else if (AmdHeapRamAddress >= 0x80000) {
      //
      // 0x80000~0xBFFFF
      //
      HeapRamFixMtrr = (UINT32) (AMD_MTRR_FIX16k_80000 + ((AmdHeapRamAddress >> 17) & 0x1));
      MsrData = AMD_MTRR_FIX16K_WB_DRAM;
      LibAmdMsrWrite (HeapRamFixMtrr, &MsrData, StdHeader);
    } else {
      //
      // 0x0 ~ 0x7FFFF
      //
      LibAmdMsrRead (AMD_MTRR_FIX64k_00000, &MsrData, StdHeader);
      MsrData = MsrData & (~(0xFF << (8 * ((AmdHeapRamAddress >> 16) & 0x7))));
      MsrData = MsrData | (AMD_MTRR_FIX64K_WB_DRAM << (8 * ((AmdHeapRamAddress >> 16) & 0x7)));
      LibAmdMsrWrite (AMD_MTRR_FIX64k_00000, &MsrData, StdHeader);
    }

    // Turn on MTTR enable bit and turn off modification bit
    LibAmdMsrRead (MSR_SYS_CFG, &MsrData, StdHeader);
    MsrData &= 0xFFFFFFFFFFF7FFFFull;
    LibAmdMsrWrite (MSR_SYS_CFG, &MsrData, StdHeader);
  }

  // Copying Heap content
  if (IsBsp (StdHeader, &IgnoredSts)) {
    HeapBufferPtr  = (UINT32 *)AMD_HEAP_START_ADDRESS;
    HeapRamPtr  = (UINT32 *)AmdHeapRamAddress;
    for (i = 0; i < AMD_HEAP_COPY_SIZE_DWORD; i++) {
      *HeapRamPtr = *HeapBufferPtr;
      HeapBufferPtr++;
      HeapRamPtr++;
    }
    // re-calculate NextNodePtr of every Heap
    HeapManagerPtr = (HEAP_MANAGER *) AmdHeapRamAddress;
    if (HeapManagerPtr->AvailableSize != AMD_HEAP_SIZE_PER_CORE - sizeof (HEAP_MANAGER)) {
      CurrentNodePtr = (BUFFER_NODE *) (AmdHeapRamAddress + sizeof (HEAP_MANAGER));
      while (CurrentNodePtr != NULL) {
        if (CurrentNodePtr->NextNodePtr != NULL) {
          CurrentNodePtr->NextNodePtr = (BUFFER_NODE *) ((UINT8 *) CurrentNodePtr->NextNodePtr - AMD_HEAP_START_ADDRESS + AmdHeapRamAddress);
        }
        CurrentNodePtr = CurrentNodePtr->NextNodePtr;
      }
    }
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
  UINT8 * HeapTempRamPtr;
  UINT8 * HeapMainRamPtr;
  UINT32 i;
  HEAP_MANAGER *HeapManagerPtr;
  BUFFER_NODE  *CurrentNodePtr;
  AGESA_BUFFER_PARAMS agesaAllocateBuffer;
  AGESA_STATUS AgesaStatus;
  UINT32  AmdHeapRamAddress;

  AmdHeapRamAddress = (UINT32) UserOptions.CfgHeapDramAddress;
  // initialize StdHeader
  AgesaStatus = AGESA_SUCCESS;
  agesaAllocateBuffer.StdHeader = *StdHeader;

  HeapManagerPtr = (HEAP_MANAGER *) AmdHeapRamAddress;
  CurrentNodePtr = (BUFFER_NODE *) (AmdHeapRamAddress + sizeof (HEAP_MANAGER));

  if (HeapManagerPtr->AvailableSize != AMD_HEAP_SIZE_PER_CORE - sizeof (HEAP_MANAGER)) {
    while (CurrentNodePtr != NULL) {
      // allocate buffer
      agesaAllocateBuffer.BufferHandle = CurrentNodePtr->BufferHandle;
      agesaAllocateBuffer.BufferLength = CurrentNodePtr->BufferSize;
      AgesaStatus = AgesaAllocateBuffer (0, &agesaAllocateBuffer);

      if (AgesaStatus == AGESA_SUCCESS) {
        // copy heap from temp ram to real ram
        HeapMainRamPtr = (UINT8 *) agesaAllocateBuffer.BufferPointer;
        HeapTempRamPtr = (UINT8 *) ((UINT8 *) CurrentNodePtr + sizeof (BUFFER_NODE));
        for (i = 0; i < CurrentNodePtr->BufferSize; i++) {
          *HeapMainRamPtr = *HeapTempRamPtr;
          HeapMainRamPtr++;
          HeapTempRamPtr++;
        }
      } else {
        return AgesaStatus;
      }
      CurrentNodePtr = CurrentNodePtr->NextNodePtr;
    }
  }
  return AGESA_SUCCESS;
}
