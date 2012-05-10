/**
 * @file
 *
 * S3 save/restore script
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 20539 $   @e \$Date: 2009-10-14 10:48:54 -0700 (Wed, 14 Oct 2009) $
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
#include "Porting.h"
#include "AMD.h"
#include "amdlib.h"
#include "heapManager.h"
#include "S3SaveState.h"
#include "Filecode.h"
#define FILECODE PROC_COMMON_S3SAVESTATE_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
extern S3_SCRIPT_CONFIGURATION OptionS3ScriptConfiguration;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize S3 Script framework
 *
 *
 *
 * @param[in] StdHeader          Pointer to standard header
 */
AGESA_STATUS
S3ScriptInit (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  return OptionS3ScriptConfiguration.Init (StdHeader);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize S3 Script framework
 *
 *
 *
 * @param[in] StdHeader          Pointer to standard header
 */
AGESA_STATUS
S3ScriptInitStateStub (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize S3 Script framework
 *
 *
 *
 * @param[in] StdHeader          Pointer to standard header
 */
AGESA_STATUS
S3ScriptInitState (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AGESA_STATUS          Status;
  ALLOCATE_HEAP_PARAMS  AllocHeapParams;

  AllocHeapParams.RequestedBufferSize = S3_TABLE_LENGTH;
  AllocHeapParams.BufferHandle = AMD_S3_SCRIPT_SAVE_TABLE_HANDLE;
  AllocHeapParams.Persist = StdHeader->HeapStatus;
  Status = HeapAllocateBuffer (&AllocHeapParams, StdHeader);
  if (Status == AGESA_SUCCESS) {
    ((S3_SAVE_TABLE_HEADER *) AllocHeapParams.BufferPtr)->TableLength = S3_TABLE_LENGTH;
    ((S3_SAVE_TABLE_HEADER *) AllocHeapParams.BufferPtr)->SaveOffset = sizeof (S3_SAVE_TABLE_HEADER);
  }
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize S3 Script framework
 *
 *
 *
 * @param[in]     StdHeader          Pointer to standard header
 * @param[in,out] S3SaveTable        S3 save table header
 */
STATIC AGESA_STATUS
S3SaveStateExtendTableLenth (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
  IN OUT   S3_SAVE_TABLE_HEADER  **S3SaveTable
  )
{
  AGESA_STATUS          Status;
  ALLOCATE_HEAP_PARAMS  AllocHeapParams;
  VOID                  *TempBuffer;
  UINT16                NewTableLength;
  UINT16                CurrentTableLength;
  //Allocate temporary buffer
  NewTableLength = (*S3SaveTable)->TableLength + S3_TABLE_LENGTH_INCREMENT;
  AllocHeapParams.RequestedBufferSize = NewTableLength;
  AllocHeapParams.BufferHandle = AMD_S3_SCRIPT_TEMP_BUFFER_HANDLE;
  AllocHeapParams.Persist = StdHeader->HeapStatus;
  Status = HeapAllocateBuffer (&AllocHeapParams, StdHeader);
  if (Status != AGESA_SUCCESS) {
    return Status;
  }
  //Save current table length
  CurrentTableLength = (*S3SaveTable)->TableLength;
  //Update table length
  (*S3SaveTable)->TableLength = NewTableLength;
  //Copy S3 save toable to temporary location
  LibAmdMemCopy (AllocHeapParams.BufferPtr, *S3SaveTable, CurrentTableLength, StdHeader);
  //Save pointer to temp buffer
  TempBuffer = AllocHeapParams.BufferPtr;
  // Free original S3 save buffer
  HeapDeallocateBuffer (AMD_S3_SCRIPT_SAVE_TABLE_HANDLE, StdHeader);

  AllocHeapParams.RequestedBufferSize = NewTableLength;
  AllocHeapParams.BufferHandle = AMD_S3_SCRIPT_SAVE_TABLE_HANDLE;
  AllocHeapParams.Persist = StdHeader->HeapStatus;
  Status = HeapAllocateBuffer (&AllocHeapParams, StdHeader);
  if (Status != AGESA_SUCCESS) {
    return Status;
  }
  LibAmdMemCopy (AllocHeapParams.BufferPtr, TempBuffer, AllocHeapParams.RequestedBufferSize, StdHeader);
  *S3SaveTable = (S3_SAVE_TABLE_HEADER*) AllocHeapParams.BufferPtr;
  HeapDeallocateBuffer (AMD_S3_SCRIPT_TEMP_BUFFER_HANDLE, StdHeader);
  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Initialize S3 Script framework
 *
 *
 *
 * @param[in]  StdHeader          Pointer to standard header
 * @param[out] S3SaveTable        S3 save table header
 */
AGESA_STATUS
S3ScriptGetS3SaveTable (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
     OUT   S3_SAVE_TABLE_HEADER  **S3SaveTable
  )
{
  AGESA_STATUS          Status;
  LOCATE_HEAP_PTR       LocHeapParams;
  LocHeapParams.BufferHandle = AMD_S3_SCRIPT_SAVE_TABLE_HANDLE;
  Status = HeapLocateBuffer (&LocHeapParams, StdHeader);
  if (Status != AGESA_SUCCESS) {
    *S3SaveTable = NULL;
    return Status;
  }
  *S3SaveTable = (S3_SAVE_TABLE_HEADER *) LocHeapParams.BufferPtr;
  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Save S3 write opcode
 *
 *
 *
 * @param[in] StdHeader          Pointer to standard header
 * @param[in] OpCode             Operation opcode
 * @param[in] Width              Width
 * @param[in] Address            Register address
 * @param[in] Count              Number of register writes
 * @param[in] Buffer             Pointer to write buffer
 */
AGESA_STATUS
S3SaveStateSaveWriteOp (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      UINT16              OpCode,
  IN      ACCESS_WIDTH        Width,
  IN      UINT64              Address,
  IN      UINT32              Count,
  IN      VOID                *Buffer
  )
{
  S3_SAVE_TABLE_HEADER    *S3SaveTablePtr;
  S3_WRITE_OP_HEADER      *SaveOffsetPtr;
  UINT32                  OpCodeLength;
  UINT32                  WidthLength;
  AGESA_STATUS            Status;

  Status = S3ScriptGetS3SaveTable (StdHeader, &S3SaveTablePtr);
  if (Status != AGESA_SUCCESS) {
    return Status;
  }
  WidthLength = LibAmdAccessWidth (Width);
  OpCodeLength = sizeof (S3_WRITE_OP_HEADER) + WidthLength * Count;
  if ((S3SaveTablePtr->SaveOffset + OpCodeLength) > S3SaveTablePtr->TableLength) {
    Status = S3SaveStateExtendTableLenth (StdHeader, &S3SaveTablePtr);
    if (Status != AGESA_SUCCESS) {
      return Status;
    }
  }
  SaveOffsetPtr = (S3_WRITE_OP_HEADER *) ((UINT8 *) S3SaveTablePtr + S3SaveTablePtr->SaveOffset);
  SaveOffsetPtr->OpCode = OpCode;
  SaveOffsetPtr->Width = Width;
  SaveOffsetPtr->Count = Count;
  SaveOffsetPtr->Address = Address;
  LibAmdMemCopy (
    (UINT8 *) SaveOffsetPtr + sizeof (S3_WRITE_OP_HEADER),
    Buffer,
    WidthLength * Count,
    StdHeader
    );
  S3SaveTablePtr->SaveOffset += OpCodeLength;
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Save S3 write opcode
 *
 *
 *
 * @param[in] StdHeader          Pointer to standard header
 * @param[in] OpCode             Operation opcode
 * @param[in] Width              Width
 * @param[in] Address            Register address
 * @param[in] Data               Pointer to data
 * @param[in] DataMask           Pointer data mask
 */
AGESA_STATUS
S3SaveStateSaveReadWriteOp (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      UINT16              OpCode,
  IN      ACCESS_WIDTH        Width,
  IN      UINT64              Address,
  IN      VOID                *Data,
  IN      VOID                *DataMask
  )
{

  S3_SAVE_TABLE_HEADER    *S3SaveTablePtr;
  S3_READ_WRITE_OP_HEADER *SaveOffsetPtr;
  UINT32                  OpCodeLength;
  UINT32                  WidthLength;
  AGESA_STATUS            Status;

  Status = S3ScriptGetS3SaveTable (StdHeader, &S3SaveTablePtr);
  if (Status != AGESA_SUCCESS) {
    return Status;
  }
  WidthLength = LibAmdAccessWidth (Width);
  OpCodeLength = sizeof (S3_READ_WRITE_OP_HEADER) + WidthLength * 2;
  if ((S3SaveTablePtr->SaveOffset + OpCodeLength) > S3SaveTablePtr->TableLength) {
    Status = S3SaveStateExtendTableLenth (StdHeader, &S3SaveTablePtr);
    if (Status != AGESA_SUCCESS) {
      return Status;
    }
  }

  SaveOffsetPtr = (S3_READ_WRITE_OP_HEADER *) ((UINT8 *) S3SaveTablePtr + S3SaveTablePtr->SaveOffset);
  SaveOffsetPtr->OpCode = OpCode;
  SaveOffsetPtr->Width = Width;
  SaveOffsetPtr->Address = Address;

  LibAmdMemCopy (
    (UINT8 *) SaveOffsetPtr + sizeof (S3_READ_WRITE_OP_HEADER),
    Data,
    WidthLength,
    StdHeader
    );
  LibAmdMemCopy (
    (UINT8 *) SaveOffsetPtr + sizeof (S3_READ_WRITE_OP_HEADER) + WidthLength,
    DataMask,
    WidthLength,
    StdHeader
    );
  S3SaveTablePtr->SaveOffset += OpCodeLength;
  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Save S3 poll opcode
 *
 *
 *
 * @param[in] StdHeader          Pointer to standard header
 * @param[in] OpCode             Operation opcode
 * @param[in] Width              Width
 * @param[in] Address            Register address
 * @param[in] Data               Pointer to data
 * @param[in] DataMask           Pointer data mask
 * @param[in] Delay              Time delay for poll
 */
AGESA_STATUS
S3SaveStateSavePollOp (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      UINT16              OpCode,
  IN      ACCESS_WIDTH        Width,
  IN      UINT64              Address,
  IN      VOID                *Data,
  IN      VOID                *DataMask,
  IN      UINT64              Delay
  )
{

  S3_SAVE_TABLE_HEADER    *S3SaveTablePtr;
  S3_POLL_OP_HEADER       *SaveOffsetPtr;
  UINT32                  OpCodeLength;
  UINT32                  WidthLength;
  AGESA_STATUS            Status;

  Status = S3ScriptGetS3SaveTable (StdHeader, &S3SaveTablePtr);
  if (Status != AGESA_SUCCESS) {
    return Status;
  }
  WidthLength = LibAmdAccessWidth (Width);
  OpCodeLength = sizeof (S3_POLL_OP_HEADER) + WidthLength * 2;
  if ((S3SaveTablePtr->SaveOffset + OpCodeLength) > S3SaveTablePtr->TableLength) {
    Status = S3SaveStateExtendTableLenth (StdHeader, &S3SaveTablePtr);
    if (Status != AGESA_SUCCESS) {
      return Status;
    }
  }

  SaveOffsetPtr = (S3_POLL_OP_HEADER *) ((UINT8 *) S3SaveTablePtr + S3SaveTablePtr->SaveOffset);
  SaveOffsetPtr->OpCode = OpCode;
  SaveOffsetPtr->Width = Width;
  SaveOffsetPtr->Delay = Delay;
  SaveOffsetPtr->Address = Address;

  LibAmdMemCopy (
    (UINT8 *) SaveOffsetPtr + sizeof (S3_POLL_OP_HEADER),
    Data,
    WidthLength,
    StdHeader
    );
  LibAmdMemCopy (
    (UINT8 *) SaveOffsetPtr + sizeof (S3_POLL_OP_HEADER) + WidthLength,
    DataMask,
    WidthLength,
    StdHeader
    );
  S3SaveTablePtr->SaveOffset += OpCodeLength;
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Save S3 poll opcode
 *
 *
 *
 * @param[in] StdHeader              Pointer to standard header
 * @param[in] OpCode                  Operation opcode
 * @param[in] InformationLength  Info length
 * @param[in] Information             Pointer to information
 */
AGESA_STATUS
S3SaveStateSaveInfoOp (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      UINT16              OpCode,
  IN      UINT32              InformationLength,
  IN      VOID                *Information
  )
{

  S3_SAVE_TABLE_HEADER    *S3SaveTablePtr;
  S3_INFO_OP_HEADER       *SaveOffsetPtr;
  UINT32                  OpCodeLength;

  AGESA_STATUS            Status;

  Status = S3ScriptGetS3SaveTable (StdHeader, &S3SaveTablePtr);
  if (Status != AGESA_SUCCESS) {
    return Status;
  }
  OpCodeLength = sizeof (S3_INFO_OP_HEADER) + InformationLength;
  if ((S3SaveTablePtr->SaveOffset + OpCodeLength) > S3SaveTablePtr->TableLength) {
    Status = S3SaveStateExtendTableLenth (StdHeader, &S3SaveTablePtr);
    if (Status != AGESA_SUCCESS) {
      return Status;
    }
  }

  SaveOffsetPtr = (S3_INFO_OP_HEADER *) ((UINT8 *) S3SaveTablePtr + S3SaveTablePtr->SaveOffset);
  SaveOffsetPtr->OpCode = OpCode;
  SaveOffsetPtr->Length = InformationLength;

  LibAmdMemCopy (
    (UINT8 *) SaveOffsetPtr + sizeof (S3_INFO_OP_HEADER),
    Information,
    InformationLength,
    StdHeader
    );
  S3SaveTablePtr->SaveOffset += OpCodeLength;
  return AGESA_SUCCESS;
}
