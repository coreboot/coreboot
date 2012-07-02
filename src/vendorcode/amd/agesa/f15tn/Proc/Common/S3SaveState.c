/* $NoKeywords:$ */
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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Porting.h"
#include "AMD.h"
#include "AGESA.h"
#include "Ids.h"
#include "amdlib.h"
#include "heapManager.h"
#include "S3SaveState.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

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
AGESA_STATUS
S3SaveStateExtendTableLenth (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
  IN OUT   S3_SAVE_TABLE_HEADER  **S3SaveTable
  );

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
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  Status = HeapAllocateBuffer (&AllocHeapParams, StdHeader);
  if (Status == AGESA_SUCCESS) {
    ((S3_SAVE_TABLE_HEADER *) AllocHeapParams.BufferPtr)->TableLength = S3_TABLE_LENGTH;
    ((S3_SAVE_TABLE_HEADER *) AllocHeapParams.BufferPtr)->SaveOffset = sizeof (S3_SAVE_TABLE_HEADER);
    ((S3_SAVE_TABLE_HEADER *) AllocHeapParams.BufferPtr)->Locked = FALSE;
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
AGESA_STATUS
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
  if (S3SaveTablePtr->Locked) {
    return  AGESA_UNSUPPORTED;
  }
  WidthLength = LibAmdAccessWidth (Width);
  OpCodeLength = sizeof (S3_WRITE_OP_HEADER) + WidthLength * Count;
  if ((S3SaveTablePtr->SaveOffset + OpCodeLength) > S3SaveTablePtr->TableLength) {
    Status = S3SaveStateExtendTableLenth (StdHeader, &S3SaveTablePtr);
    if (Status != AGESA_SUCCESS) {
      return Status;
    }
  }
  S3_SCRIPT_DEBUG_CODE (
    IDS_HDT_CONSOLE (S3_TRACE, "  S3 Save: %s Address: 0x%08x Data: ", S3SaveDebugOpcodeString (StdHeader, OpCode), Address);
    S3SaveDebugPrintHexArray (StdHeader, Buffer, Count, Width);
    IDS_HDT_CONSOLE (S3_TRACE, "\n");
    );
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
  if (S3SaveTablePtr->Locked) {
    return  AGESA_UNSUPPORTED;
  }
  WidthLength = LibAmdAccessWidth (Width);
  OpCodeLength = sizeof (S3_READ_WRITE_OP_HEADER) + WidthLength * 2;
  if ((S3SaveTablePtr->SaveOffset + OpCodeLength) > S3SaveTablePtr->TableLength) {
    Status = S3SaveStateExtendTableLenth (StdHeader, &S3SaveTablePtr);
    if (Status != AGESA_SUCCESS) {
      return Status;
    }
  }
  S3_SCRIPT_DEBUG_CODE (
    IDS_HDT_CONSOLE (S3_TRACE, "  S3 Save: %s Address: 0x%08x Data: ", S3SaveDebugOpcodeString (StdHeader, OpCode), Address);
    S3SaveDebugPrintHexArray (StdHeader, Data, 1, Width);
    IDS_HDT_CONSOLE (S3_TRACE, " Mask: ");
    S3SaveDebugPrintHexArray (StdHeader, DataMask, 1, Width);
    IDS_HDT_CONSOLE (S3_TRACE, "\n");
    );
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
  if (S3SaveTablePtr->Locked) {
    return  AGESA_UNSUPPORTED;
  }
  WidthLength = LibAmdAccessWidth (Width);
  OpCodeLength = sizeof (S3_POLL_OP_HEADER) + WidthLength * 2;
  if ((S3SaveTablePtr->SaveOffset + OpCodeLength) > S3SaveTablePtr->TableLength) {
    Status = S3SaveStateExtendTableLenth (StdHeader, &S3SaveTablePtr);
    if (Status != AGESA_SUCCESS) {
      return Status;
    }
  }
  S3_SCRIPT_DEBUG_CODE (
    IDS_HDT_CONSOLE (S3_TRACE, "  S3 Save: %s Address: 0x%08x Data: ", S3SaveDebugOpcodeString (StdHeader, OpCode), Address);
    S3SaveDebugPrintHexArray (StdHeader, Data, 1, Width);
    IDS_HDT_CONSOLE (S3_TRACE, " Mask: ");
    S3SaveDebugPrintHexArray (StdHeader, DataMask, 1, Width);
    IDS_HDT_CONSOLE (S3_TRACE, "\n");
    );
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
 * Save S3 info opcode
 *
 *
 *
 * @param[in] StdHeader               Pointer to standard header
 * @param[in] OpCode                  Operation opcode
 * @param[in] InformationLength       Info length
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
  if (S3SaveTablePtr->Locked) {
    return  AGESA_UNSUPPORTED;
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
  S3_SCRIPT_DEBUG_CODE (
    IDS_HDT_CONSOLE (S3_TRACE, "  S3 Save: Info: %s \n", Information);
    );
  LibAmdMemCopy (
    (UINT8 *) SaveOffsetPtr + sizeof (S3_INFO_OP_HEADER),
    Information,
    InformationLength,
    StdHeader
    );
  S3SaveTablePtr->SaveOffset += OpCodeLength;
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Save S3 dispatch opcode
 *
 *
 *
 * @param[in] StdHeader               Pointer to standard header
 * @param[in] OpCode                  Operation opcode
 * @param[in] FunctionId              Function ID
 * @param[in] ContextLength           Context length
 * @param[in] Context                 Pointer to Context
 */
AGESA_STATUS
S3SaveStateSaveDispatchOp (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      UINT16              OpCode,
  IN      UINT16              FunctionId,
  IN      UINT16              ContextLength,
  IN      VOID                *Context
  )
{

  S3_SAVE_TABLE_HEADER    *S3SaveTablePtr;
  S3_DISPATCH_OP_HEADER   *SaveOffsetPtr;
  UINT32                  OpCodeLength;
  AGESA_STATUS            Status;

  Status = S3ScriptGetS3SaveTable (StdHeader, &S3SaveTablePtr);
  if (Status != AGESA_SUCCESS) {
    return Status;
  }
  if (S3SaveTablePtr->Locked) {
    return  AGESA_UNSUPPORTED;
  }
  OpCodeLength = sizeof (S3_DISPATCH_OP_HEADER) + ContextLength;
  if ((S3SaveTablePtr->SaveOffset + OpCodeLength) > S3SaveTablePtr->TableLength) {
    Status = S3SaveStateExtendTableLenth (StdHeader, &S3SaveTablePtr);
    if (Status != AGESA_SUCCESS) {
      return Status;
    }
  }
  S3_SCRIPT_DEBUG_CODE (
    IDS_HDT_CONSOLE (S3_TRACE, "  S3 Save: %s Function Id: 0x%02x, Context: ",  S3SaveDebugOpcodeString (StdHeader, OpCode), FunctionId);
    S3SaveDebugPrintHexArray (StdHeader, Context, ContextLength, AccessWidth8);
    IDS_HDT_CONSOLE (S3_TRACE, "\n");
    );
  SaveOffsetPtr = (S3_DISPATCH_OP_HEADER *) ((UINT8 *) S3SaveTablePtr + S3SaveTablePtr->SaveOffset);
  SaveOffsetPtr->OpCode = OpCode;
  SaveOffsetPtr->Length = ContextLength;
  SaveOffsetPtr->FunctionId = FunctionId;
  LibAmdMemCopy (
    (UINT8 *) SaveOffsetPtr + sizeof (S3_DISPATCH_OP_HEADER),
    Context,
    ContextLength,
    StdHeader
    );

  S3SaveTablePtr->SaveOffset += OpCodeLength;
  return AGESA_SUCCESS;
}




/*----------------------------------------------------------------------------------------*/
/**
 * Save S3 debug support
 *
 *
 *
 * @param[in] StdHeader               Pointer to standard header
 * @param[in] Op                      Opcode
 */
CHAR8*
S3SaveDebugOpcodeString (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      UINT16              Op
  )
{
  switch (Op) {
  case SAVE_STATE_IO_WRITE_OPCODE:
    return (CHAR8 *)"IO WR";
  case SAVE_STATE_IO_READ_WRITE_OPCODE:
    return (CHAR8 *)"IO RD/WR";
  case SAVE_STATE_IO_POLL_OPCODE:
    return (CHAR8 *)"IO POLL";
  case SAVE_STATE_MEM_WRITE_OPCODE:
    return (CHAR8 *)"MEM WR";
  case SAVE_STATE_MEM_READ_WRITE_OPCODE:
    return (CHAR8 *)"MEM RD/WR";
  case SAVE_STATE_MEM_POLL_OPCODE:
    return (CHAR8 *)"MEM POLL";
  case SAVE_STATE_PCI_CONFIG_WRITE_OPCODE:
    return (CHAR8 *)"PCI WR";
  case SAVE_STATE_PCI_CONFIG_READ_WRITE_OPCODE:
    return (CHAR8 *)"PCI RD/WR";
  case SAVE_STATE_PCI_CONFIG_POLL_OPCODE:
    return (CHAR8 *)"PCI POLL";
  case SAVE_STATE_STALL_OPCODE:
    return (CHAR8 *)"STALL";
  case SAVE_STATE_DISPATCH_OPCODE:
    return (CHAR8 *)"DISPATCH";
  default:
    IDS_ERROR_TRAP;
  }
  return (CHAR8 *)"!!! Unrecognize opcode !!!";
}


/*----------------------------------------------------------------------------------------*/
/**
 * Save S3 debug support
 *
 *
 *
 * @param[in] StdHeader               Pointer to standard header
 * @param[in] Array                   Array
 * @param[in] Count                   Count of element in array
 * @param[in] Width                   Array Element width
 */
VOID
S3SaveDebugPrintHexArray (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      VOID                *Array,
  IN      UINT32              Count,
  IN      ACCESS_WIDTH        Width
  )
{
  UINTN  Index;

  for (Index = 0; Index < Count; Index++) {
    switch (Width) {
    case AccessWidth8:
    case AccessS3SaveWidth8:
      IDS_HDT_CONSOLE (S3_TRACE, "0x%02x", *((UINT8*)Array + Index));
      break;
    case AccessWidth16:
    case AccessS3SaveWidth16:
      IDS_HDT_CONSOLE (S3_TRACE, "0x%04x", *((UINT16*)Array + Index));
      break;
    case AccessWidth32:
    case AccessS3SaveWidth32:
      IDS_HDT_CONSOLE (S3_TRACE, "0x%08x", *((UINT32*)Array + Index));
      break;
    case AccessWidth64:
    case AccessS3SaveWidth64:
      //IDS_HDT_CONSOLE (S3_TRACE, "0x%08x%08x", ((UINT32*) ((UINT64*)Array + Index)[1], ((UINT32*) ((UINT64*)Array + Index))[0]));
      break;
    default:
      IDS_ERROR_TRAP;
    }
    if (Index < (Count - 1)) {
      IDS_HDT_CONSOLE (S3_TRACE, ", ");
    }
  }
}
