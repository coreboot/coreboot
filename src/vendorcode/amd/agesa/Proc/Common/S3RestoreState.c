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
#include "Porting.h"
#include "AMD.h"
#include "AGESA.h"
#include "Ids.h"
#include "amdlib.h"
#include "heapManager.h"
#include "S3SaveState.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_COMMON_S3RESTORESTATE_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

extern S3_SCRIPT_CONFIGURATION    OptionS3ScriptConfiguration;
extern S3_DISPATCH_FUNCTION_ENTRY S3DispatchFunctionTable[];
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


AGESA_STATUS
STATIC
S3RestoreStateFromTable (
  IN      S3_SAVE_TABLE_HEADER  *S3SaveTablePtr,
  IN      AMD_CONFIG_PARAMS     *StdHeader
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
S3ScriptRestore (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  return OptionS3ScriptConfiguration.Restore (StdHeader);
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
S3ScriptRestoreStateStub (
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
S3ScriptRestoreState (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AGESA_STATUS          Status;
  S3_SAVE_TABLE_HEADER  *S3SaveTablePtr;
  Status = S3ScriptGetS3SaveTable (StdHeader, &S3SaveTablePtr);
  if (Status != AGESA_SUCCESS) {
    IDS_ERROR_TRAP;
    return  AGESA_FATAL;
  }
  S3SaveTablePtr->Locked = TRUE;
  Status = S3RestoreStateFromTable (S3SaveTablePtr, StdHeader);
  return  Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Initialize S3 Script framework
 *
 *
 *
 * @param[in] S3SaveTablePtr     Pointer to S3 Save Table
 * @param[in] StdHeader          Pointer to standard header
 */
AGESA_STATUS
STATIC
S3RestoreStateFromTable (
  IN      S3_SAVE_TABLE_HEADER  *S3SaveTablePtr,
  IN      AMD_CONFIG_PARAMS     *StdHeader
  )
{
  VOID                  *S3SaveTableRecordPtr;
  PCI_ADDR              PciAddress;
  UINTN                 Index;
  S3SaveTableRecordPtr = (UINT8 *) S3SaveTablePtr + sizeof (S3_SAVE_TABLE_HEADER);
  IDS_HDT_CONSOLE (S3_TRACE, "Start S3 restore\n", ((S3_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Address);
  while ((UINT8 *) S3SaveTableRecordPtr < ((UINT8 *) S3SaveTablePtr + S3SaveTablePtr->SaveOffset)) {
    switch  (*(UINT16 *) S3SaveTableRecordPtr) {
    case  SAVE_STATE_IO_WRITE_OPCODE:
      S3_SCRIPT_DEBUG_CODE (
        IDS_HDT_CONSOLE (S3_TRACE, "  S3 Restore: %s Address: 0x%08x Data: ", S3SaveDebugOpcodeString (StdHeader, *(UINT16 *) S3SaveTableRecordPtr), (UINT16) ((S3_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Address);
        S3SaveDebugPrintHexArray (StdHeader, (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_WRITE_OP_HEADER), 1, ((S3_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width);
        IDS_HDT_CONSOLE (S3_TRACE, "\n");
        );
      LibAmdIoWrite (
        ((S3_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width,
        (UINT16) ((S3_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Address,
        (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_WRITE_OP_HEADER),
        StdHeader
        );
      S3SaveTableRecordPtr = (UINT8 *) S3SaveTableRecordPtr +
                              sizeof (S3_WRITE_OP_HEADER) +
                              LibAmdAccessWidth (((S3_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width);
      break;
    case  SAVE_STATE_IO_READ_WRITE_OPCODE:
      S3_SCRIPT_DEBUG_CODE (
        IDS_HDT_CONSOLE (S3_TRACE, "  S3 Restore: %s Address: 0x%08x Data: ", S3SaveDebugOpcodeString (StdHeader, *(UINT16 *) S3SaveTableRecordPtr), (UINT16) ((S3_READ_WRITE_OP_HEADER*) S3SaveTableRecordPtr)->Address);
        S3SaveDebugPrintHexArray (
          StdHeader,
          (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_READ_WRITE_OP_HEADER),
          1,
          ((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width
          );
        IDS_HDT_CONSOLE (S3_TRACE, " Mask: ");
        S3SaveDebugPrintHexArray (
          StdHeader,
          (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_READ_WRITE_OP_HEADER) + LibAmdAccessWidth (((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width),
          1,
          ((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width
          );
        IDS_HDT_CONSOLE (S3_TRACE, "\n");
        );
      LibAmdIoRMW (
        ((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width,
        (UINT16) ((S3_READ_WRITE_OP_HEADER*) S3SaveTableRecordPtr)->Address,
        (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_READ_WRITE_OP_HEADER),
        (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_READ_WRITE_OP_HEADER) + LibAmdAccessWidth (((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width),
        StdHeader
        );
      S3SaveTableRecordPtr = (UINT8 *) S3SaveTableRecordPtr +
                              sizeof (S3_READ_WRITE_OP_HEADER) +
                              2 * LibAmdAccessWidth (((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width);
      break;
    case  SAVE_STATE_MEM_WRITE_OPCODE:
      S3_SCRIPT_DEBUG_CODE (
        IDS_HDT_CONSOLE (S3_TRACE, "  S3 Restore: %s Address: 0x%08x Data: ", S3SaveDebugOpcodeString (StdHeader, *(UINT16 *) S3SaveTableRecordPtr), (UINT16) ((S3_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Address);
        S3SaveDebugPrintHexArray (StdHeader, (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_WRITE_OP_HEADER), 1, ((S3_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width);
        IDS_HDT_CONSOLE (S3_TRACE, "\n");
        );
     LibAmdMemWrite (
        ((S3_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width,
        ((S3_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Address,
        (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_WRITE_OP_HEADER),
        StdHeader
        );
      S3SaveTableRecordPtr = (UINT8 *) S3SaveTableRecordPtr +
                              sizeof (S3_WRITE_OP_HEADER) +
                              LibAmdAccessWidth (((S3_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width);
      break;
    case  SAVE_STATE_MEM_READ_WRITE_OPCODE:
      S3_SCRIPT_DEBUG_CODE (
        IDS_HDT_CONSOLE (S3_TRACE, "  S3 Restore: %s Address: 0x%08x Data: ", S3SaveDebugOpcodeString (StdHeader, *(UINT16 *) S3SaveTableRecordPtr), (UINT16) ((S3_READ_WRITE_OP_HEADER*) S3SaveTableRecordPtr)->Address);
        S3SaveDebugPrintHexArray (
          StdHeader,
          (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_READ_WRITE_OP_HEADER),
          1,
          ((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width
          );
        IDS_HDT_CONSOLE (S3_TRACE, " Mask: ");
        S3SaveDebugPrintHexArray (
          StdHeader,
          (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_READ_WRITE_OP_HEADER) + LibAmdAccessWidth (((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width),
          1,
          ((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width
          );
        IDS_HDT_CONSOLE (S3_TRACE, "\n");
        );
      LibAmdMemRMW (
        ((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width,
        ((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Address,
        (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_READ_WRITE_OP_HEADER),
        (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_READ_WRITE_OP_HEADER) + LibAmdAccessWidth (((S3_READ_WRITE_OP_HEADER*) S3SaveTableRecordPtr)->Width),
        StdHeader
        );
      S3SaveTableRecordPtr = (UINT8 *) S3SaveTableRecordPtr +
                              sizeof (S3_READ_WRITE_OP_HEADER) +
                              2 * LibAmdAccessWidth (((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width);
      break;
    case  SAVE_STATE_PCI_CONFIG_WRITE_OPCODE:
      PciAddress.AddressValue = (UINT32) ((S3_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Address;
      S3_SCRIPT_DEBUG_CODE (
        IDS_HDT_CONSOLE (S3_TRACE, "  S3 Restore: %s Address: 0x%08x Data: ", S3SaveDebugOpcodeString (StdHeader, *(UINT16 *) S3SaveTableRecordPtr), (UINT16) ((S3_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Address);
        S3SaveDebugPrintHexArray (StdHeader, (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_WRITE_OP_HEADER), 1, ((S3_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width);
        IDS_HDT_CONSOLE (S3_TRACE, "\n");
        );
      LibAmdPciWrite (
        ((S3_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width,
        PciAddress,
        (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_WRITE_OP_HEADER),
        StdHeader
        );
      S3SaveTableRecordPtr = (UINT8 *) S3SaveTableRecordPtr +
                              sizeof (S3_WRITE_OP_HEADER) +
                              LibAmdAccessWidth (((S3_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width);
      break;
    case  SAVE_STATE_PCI_CONFIG_READ_WRITE_OPCODE:
      PciAddress.AddressValue = (UINT32) ((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Address;
      S3_SCRIPT_DEBUG_CODE (
        IDS_HDT_CONSOLE (S3_TRACE, "  S3 Restore: %s Address: 0x%08x Data: ", S3SaveDebugOpcodeString (StdHeader, *(UINT16 *) S3SaveTableRecordPtr), (UINT16) ((S3_READ_WRITE_OP_HEADER*) S3SaveTableRecordPtr)->Address);
        S3SaveDebugPrintHexArray (
          StdHeader,
          (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_READ_WRITE_OP_HEADER),
          1,
          ((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width
          );
        IDS_HDT_CONSOLE (S3_TRACE, " Mask: ");
        S3SaveDebugPrintHexArray (
          StdHeader,
          (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_READ_WRITE_OP_HEADER) + LibAmdAccessWidth (((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width),
          1,
          ((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width
          );
        IDS_HDT_CONSOLE (S3_TRACE, "\n");
        );
      LibAmdPciRMW (
        ((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width,
        PciAddress,
        (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_READ_WRITE_OP_HEADER),
        (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_READ_WRITE_OP_HEADER) + LibAmdAccessWidth (((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width),
        StdHeader
        );
      S3SaveTableRecordPtr = (UINT8 *) S3SaveTableRecordPtr +
                              sizeof (S3_READ_WRITE_OP_HEADER) +
                              2 * LibAmdAccessWidth (((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width);
      break;
    case  SAVE_STATE_STALL_OPCODE:
      break;
    case  SAVE_STATE_INFORMATION_OPCODE:
      IDS_HDT_CONSOLE (S3_TRACE, "  S3 Restore: Info: [%s]\n", (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_INFO_OP_HEADER));
      S3SaveTableRecordPtr = (UINT8 *) S3SaveTableRecordPtr +
                             sizeof (S3_INFO_OP_HEADER) +
                             ((S3_INFO_OP_HEADER*) S3SaveTableRecordPtr)->Length;
      break;
    case  SAVE_STATE_DISPATCH_OPCODE:
      S3_SCRIPT_DEBUG_CODE (
        IDS_HDT_CONSOLE (S3_TRACE, "  S3 Restore: %s Function Id: 0x%02x, Context: ",  S3SaveDebugOpcodeString (StdHeader, *(UINT16 *) S3SaveTableRecordPtr), ((S3_DISPATCH_OP_HEADER*) S3SaveTableRecordPtr)->FunctionId);
        S3SaveDebugPrintHexArray (
          StdHeader,
          (VOID*)((UINT8*) S3SaveTableRecordPtr + sizeof (S3_DISPATCH_OP_HEADER)),
          ((S3_DISPATCH_OP_HEADER*) S3SaveTableRecordPtr)->Length,
          AccessWidth8);
        IDS_HDT_CONSOLE (S3_TRACE, "\n");
        );
      Index = 0;
      while (S3DispatchFunctionTable[Index].FunctionId != 0) {
        if (S3DispatchFunctionTable[Index].FunctionId == ((S3_DISPATCH_OP_HEADER*) S3SaveTableRecordPtr)->FunctionId) {
          (S3DispatchFunctionTable[Index].Function) (
            StdHeader,
            ((S3_DISPATCH_OP_HEADER*) S3SaveTableRecordPtr)->Length,
            (VOID*)((UINT8*) S3SaveTableRecordPtr + sizeof (S3_DISPATCH_OP_HEADER))
            );
          break;
        }
        Index++;
      }
      ASSERT (S3DispatchFunctionTable[Index].FunctionId != 0);
      S3SaveTableRecordPtr = (UINT8 *) S3SaveTableRecordPtr +
                             sizeof (S3_DISPATCH_OP_HEADER) +
                             ((S3_DISPATCH_OP_HEADER*) S3SaveTableRecordPtr)->Length;
      break;

    case  SAVE_STATE_IO_POLL_OPCODE:
      S3_SCRIPT_DEBUG_CODE (
        IDS_HDT_CONSOLE (S3_TRACE, "  S3 Restore: %s Address: 0x%04x Data: ", S3SaveDebugOpcodeString (StdHeader, *(UINT16 *) S3SaveTableRecordPtr), (UINT16) ((S3_POLL_OP_HEADER*) S3SaveTableRecordPtr)->Address);
        S3SaveDebugPrintHexArray (
          StdHeader,
          (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_POLL_OP_HEADER),
          1,
          ((S3_READ_WRITE_OP_HEADER *) S3SaveTableRecordPtr)->Width
          );
        IDS_HDT_CONSOLE (S3_TRACE, " Mask: ");
        S3SaveDebugPrintHexArray (
          StdHeader,
          (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_POLL_OP_HEADER) + LibAmdAccessWidth (((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Width),
          1,
          ((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Width
          );
        )
      LibAmdIoPoll (
        ((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Width,
        (UINT16) ((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Address,
        (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_POLL_OP_HEADER),
        (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_POLL_OP_HEADER) + LibAmdAccessWidth (((S3_POLL_OP_HEADER*) S3SaveTableRecordPtr)->Width),
        ((S3_POLL_OP_HEADER*) S3SaveTableRecordPtr)->Delay,
        StdHeader
        );
      S3SaveTableRecordPtr = (UINT8 *) S3SaveTableRecordPtr +
                              sizeof (S3_POLL_OP_HEADER) +
                              2 * LibAmdAccessWidth (((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Width);
      break;
    case  SAVE_STATE_MEM_POLL_OPCODE:
      S3_SCRIPT_DEBUG_CODE (
        IDS_HDT_CONSOLE (S3_TRACE, "  S3 Restore: %s Address: 0x%08x Data: ", S3SaveDebugOpcodeString (StdHeader, *(UINT16 *) S3SaveTableRecordPtr), (UINT32) ((S3_POLL_OP_HEADER*) S3SaveTableRecordPtr)->Address);
        S3SaveDebugPrintHexArray (
          StdHeader,
          (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_POLL_OP_HEADER),
          1,
          ((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Width
          );
        IDS_HDT_CONSOLE (S3_TRACE, " Mask: ");
        S3SaveDebugPrintHexArray (
          StdHeader,
          (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_POLL_OP_HEADER) + LibAmdAccessWidth (((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Width),
          1,
          ((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Width
          );
        )
      LibAmdMemPoll (
        ((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Width,
        ((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Address,
        (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_POLL_OP_HEADER),
        (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_POLL_OP_HEADER) + LibAmdAccessWidth (((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Width),
        ((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Delay,
        StdHeader
        );
      S3SaveTableRecordPtr = (UINT8 *) S3SaveTableRecordPtr +
                              sizeof (S3_POLL_OP_HEADER) +
                              2 * LibAmdAccessWidth (((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Width);
      break;
    case  SAVE_STATE_PCI_CONFIG_POLL_OPCODE:
      PciAddress.AddressValue = (UINT32) ((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Address;
      S3_SCRIPT_DEBUG_CODE (
        IDS_HDT_CONSOLE (S3_TRACE, "  S3 Restore: %s Address: 0x%08x Data: ", S3SaveDebugOpcodeString (StdHeader, *(UINT16 *) S3SaveTableRecordPtr), (UINT32) ((S3_POLL_OP_HEADER*) S3SaveTableRecordPtr)->Address);
        S3SaveDebugPrintHexArray (
          StdHeader,
          (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_POLL_OP_HEADER),
          1,
          ((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Width
          );
        IDS_HDT_CONSOLE (S3_TRACE, " Mask: ");
        S3SaveDebugPrintHexArray (
          StdHeader,
          (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_POLL_OP_HEADER) + LibAmdAccessWidth (((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Width),
          1,
          ((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Width
          );
        )
      LibAmdPciPoll (
        ((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Width,
        PciAddress,
        (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_POLL_OP_HEADER),
        (UINT8 *) S3SaveTableRecordPtr + sizeof (S3_POLL_OP_HEADER) + LibAmdAccessWidth (((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Width),
        ((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Delay,
        StdHeader
        );
      S3SaveTableRecordPtr = (UINT8 *) S3SaveTableRecordPtr +
                              sizeof (S3_POLL_OP_HEADER) +
                              2 * LibAmdAccessWidth (((S3_POLL_OP_HEADER *) S3SaveTableRecordPtr)->Width);
      break;
    default:
      IDS_HDT_CONSOLE (S3_TRACE, " ERROR!!! Invalid S3 restore opcode\n");
      ASSERT (FALSE);
      return AGESA_ERROR;
    }
  }
  IDS_HDT_CONSOLE (S3_TRACE, " End S3 Restore \n");
  return AGESA_SUCCESS;
}

