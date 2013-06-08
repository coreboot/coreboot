/**
 * @file
 *
 * Adding IDT table for debugging exception
 *
 * Contains IDT related function
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 */
/*****************************************************************************
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#include "IdsLib.h"
#include "amdlib.h"
#include "AMD.h"
#include "GeneralServices.h"
#include "cpuApicUtilities.h"
#include "heapManager.h"
#include "cpuRegisters.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_IDS_DEBUG_IDSIDTTABLE_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
 IDS_STATUS
STATIC
IdsReplaceIdtr (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

IDS_STATUS
STATIC
IdsRestoreIdtr (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

IDS_STATUS
STATIC
IdsUpdateExceptionVector (
  IN       VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern UINT64 IdsExceptionHandler;
extern UINT32 SizeIdtDescriptor;
extern UINT32 SizeTotalIdtDescriptors;

/**
 * IDS IDT table.
 *
 * This is the top level interface for IDS IDT table function.
 * Adding a 'jmp $' into every exception handler.
 * So debugger could use HDT to skip 'jmp $' and execute the iret,
 * then they could find which instruction cause the exception.
 *
 * @param[in]     IdsIdtFuncId IDT indicator value, see @ref IDS_IDT_FUNC_ID
 * @param[in]     DataPtr      Points to data that may used by IdsIdtTable routine
 * @param[in,out] StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 **/
IDS_STATUS
IdsExceptionTrap (
  IN       IDS_IDT_FUNC_ID IdsIdtFuncId,
  IN       VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  IDS_STATUS   ReturnFlag;
  AGESA_STATUS Ignored;

  ReturnFlag = IDS_SUCCESS;
  switch (IdsIdtFuncId) {
  case IDS_IDT_REPLACE_IDTR_FOR_BSC:
    if (IsBsp (StdHeader, &Ignored)) {
      ReturnFlag = IdsReplaceIdtr (StdHeader);
    }
    break;
  case IDS_IDT_RESTORE_IDTR_FOR_BSC:
    if (IsBsp (StdHeader, &Ignored)) {
      ReturnFlag = IdsRestoreIdtr (StdHeader);
    }
    break;
  case IDS_IDT_UPDATE_EXCEPTION_VECTOR_FOR_AP:
    ReturnFlag = IdsUpdateExceptionVector (DataPtr, StdHeader);
    break;
  default:
    return IDS_UNSUPPORTED;
    break;
  }
  return ReturnFlag;
}

/*---------------------------------------------------------------------------------------
 *                           L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/**
 * Replace IDTR of BSC.
 *
 * Save IDTR of BSC to heap and replace IDTR.
 *
 * @param[in,out] StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 **/
IDS_STATUS
STATIC
IdsReplaceIdtr (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  ALLOCATE_HEAP_PARAMS AllocParams;
  LOCATE_HEAP_PTR LocateHeap;
  IDT_BASE_LIMIT IdtInfo;

  // Save IDTR to Heap
  AllocParams.BufferHandle = IDS_SAVE_IDTR_HANDLE;
  AllocParams.RequestedBufferSize = sizeof (IDT_BASE_LIMIT);
  AllocParams.Persist = HEAP_TEMP_MEM;
  if (HeapAllocateBuffer (&AllocParams, StdHeader) != AGESA_SUCCESS) {
    return IDS_UNSUPPORTED;
  }
  GetIdtr ((IDT_BASE_LIMIT *) AllocParams.BufferPtr, StdHeader);

  // Check if host env has their own IDT
  if (((IDT_BASE_LIMIT *) AllocParams.BufferPtr)->Base == 0) {
    // Load AGESA's IDT
    LocateHeap.BufferHandle = IDS_BSC_IDT_HANDLE;
    // Check if we already allocated heap for creating IDT
    if (HeapLocateBuffer (&LocateHeap, StdHeader) != AGESA_SUCCESS) {
      // Create IDT for BSC in heap
      AllocParams.BufferHandle = IDS_BSC_IDT_HANDLE;
      AllocParams.RequestedBufferSize = sizeof (IDT_DESCRIPTOR) * 32;
      AllocParams.Persist = HEAP_SYSTEM_MEM;
      if (HeapAllocateBuffer (&AllocParams, StdHeader) != AGESA_SUCCESS) {
        return IDS_UNSUPPORTED;
      }
      IdtInfo.Base = (UINT64) AllocParams.BufferPtr;
    } else {
      IdtInfo.Base = (UINT64) LocateHeap.BufferPtr;
    }
    IdsUpdateExceptionVector ((VOID *) &IdtInfo,  StdHeader);
    SetIdtr (&IdtInfo, StdHeader);
  } else {
    return IDS_UNSUPPORTED;
  }
  return IDS_SUCCESS;
}

/**
 * Restore IDTR of BSC.
 *
 * Restore IDTR of BSC from heap.
 *
 * @param[in,out] StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 **/
IDS_STATUS
STATIC
IdsRestoreIdtr (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LOCATE_HEAP_PTR LocateHeap;
  IDT_BASE_LIMIT IdtInfo;

  IdtInfo.Base = 0;
  IdtInfo.Limit = 0;
  GetIdtr (&IdtInfo, StdHeader);

  LocateHeap.BufferHandle = IDS_BSC_IDT_HANDLE;
  if (HeapLocateBuffer (&LocateHeap, StdHeader) != AGESA_SUCCESS) {
    // If AGESA loaded its own IDT, there must be a buffer with handle IDS_BSC_IDT_HANDLE in heap
    return IDS_UNSUPPORTED;
  }
  // If current IDTR.Base != address of AGESA's own IDT, it is possibile that Host env load their own IDT
  if (IdtInfo.Base != ((UINT64) (LocateHeap.BufferPtr))) {
    return IDS_UNSUPPORTED;
  } else {
    LocateHeap.BufferHandle = IDS_SAVE_IDTR_HANDLE;
    if (HeapLocateBuffer (&LocateHeap, StdHeader) != AGESA_SUCCESS) {
      return IDS_UNSUPPORTED;
    }
    SetIdtr ((IDT_BASE_LIMIT *) LocateHeap.BufferPtr, StdHeader);
    HeapDeallocateBuffer (IDS_SAVE_IDTR_HANDLE, StdHeader);
  }
  return IDS_SUCCESS;
}

/**
 * Update exception vector.
 *
 *  Adding a 'jmp $' into every exception handler.
 *  So debugger could use HDT to skip 'jmp $' and execute the iret,
 *  then they could find which instruction cause the exception.
 *
 *  @param[in,out] StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 **/
IDS_STATUS
STATIC
IdsUpdateExceptionVector (
  IN       VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  IDT_DESCRIPTOR *IdtDesc;
  UINT8   DescSize;
  UINT16  Selector;
  UINT16  i;
  UINT64  HandlerOffset;
  UINT64  EferRegister;
  AGESA_STATUS Ignored;

  LibAmdMsrRead (MSR_EXTENDED_FEATURE_EN, &EferRegister, StdHeader);
  if ((EferRegister & 0x100) != 0) {
    DescSize = 16;
  } else {
    DescSize = 8;
  }

  // Update limit
  ((IDT_BASE_LIMIT *) DataPtr)->Limit = (UINT16) ((DescSize * 32) - 1);
  // Update IDT
  IdtDesc = (IDT_DESCRIPTOR *) (((IDT_BASE_LIMIT *) DataPtr)->Base);
  HandlerOffset = (UINT64) IdsExceptionHandler;
  GetCsSelector (&Selector, StdHeader);

  ASSERT (SizeTotalIdtDescriptors == (SizeIdtDescriptor * 32));
  for (i = 0; i < 32; i++) {
    // Vector - 2 NMI handler for APs is used by AGESA. So we should not replace that handler.
    if ((i == 2) && (!IsBsp (StdHeader, &Ignored)))  {
      IdtDesc = (IDT_DESCRIPTOR *) (((UINT8 *) IdtDesc) + DescSize);
      HandlerOffset += SizeIdtDescriptor;
      continue;
    }
    IdtDesc->OffsetLo = (UINT16) HandlerOffset & 0xFFFF;
    IdtDesc->OffsetHi = (UINT16) (HandlerOffset >> 16);
    IdtDesc->Flags = IDT_DESC_PRESENT | IDT_DESC_TYPE_INT32;
    IdtDesc->Selector = Selector;
    IdtDesc->Rsvd = 0;
    if ((EferRegister & 0x100) != 0) {
      IdtDesc->Offset64 = (UINT32) (HandlerOffset >> 32);
      IdtDesc->Rsvd64 = 0;
    }
    IdtDesc = (IDT_DESCRIPTOR *) (((UINT8 *) IdtDesc) + DescSize);
    HandlerOffset += SizeIdtDescriptor;
  }
  return IDS_SUCCESS;
}

