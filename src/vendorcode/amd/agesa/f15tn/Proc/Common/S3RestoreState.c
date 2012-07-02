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

