/* $NoKeywords:$ */
/**
 * @file
 *
 * Various PCI service routines.
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

#ifndef _S3SAVESTATE_H_
#define _S3SAVESTATE_H_

#pragma pack (push, 1)

#ifndef S3_SCRIPT_DEBUG_CODE
  #define S3_SCRIPT_DEBUG_CODE(Code) Code
#endif

/// Dispatch function ID repository
typedef enum {
  NbSmuIndirectWriteS3Script_ID = 1,           ///< GNB SMU service request function ID.
  NbSmuServiceRequestS3Script_ID,              ///< GNB PCIe late restore function ID.
  PcieLateRestoreS3Script_ID,                  ///< GNB SMU indirect write.
  GnbSmuServiceRequestV4S3Script_ID,           ///< SMU service request
  GnbLibStallS3Script_ID,                      ///< Stall request
  PcieLateRestoreTNS3Script_ID,                ///< GNB PCIe late restore TN
  PcieLateRestoreKMS3Script_ID,                ///< GNB PCIe late restore KM
  PcieLateRestoreTHS3Script_ID,                ///< GNB PCIe late restore KM
  GfxRequestSclkTNS3Script_ID                  ///< SCLk setting
} S3_DISPATCH_FUNCTION_ID;

#define SAVE_STATE_IO_WRITE_OPCODE              0x00
#define SAVE_STATE_IO_READ_WRITE_OPCODE         0x01
#define SAVE_STATE_MEM_WRITE_OPCODE             0x02
#define SAVE_STATE_MEM_READ_WRITE_OPCODE        0x03
#define SAVE_STATE_PCI_CONFIG_WRITE_OPCODE      0x04
#define SAVE_STATE_PCI_CONFIG_READ_WRITE_OPCODE 0x05
#define SAVE_STATE_STALL_OPCODE                 0x07
#define SAVE_STATE_INFORMATION_OPCODE           0x0A
#define SAVE_STATE_IO_POLL_OPCODE               0x0D
#define SAVE_STATE_MEM_POLL_OPCODE              0x0E
#define SAVE_STATE_PCI_CONFIG_POLL_OPCODE       0x0F
#define SAVE_STATE_DISPATCH_OPCODE              0x20
#define SAVE_STATE_BREAKPOINT_OPCODE            0x21


#define S3_TABLE_LENGTH             8 * 1024
#define S3_TABLE_LENGTH_INCREMENT   1 * 1024

/// S3 Save Table
typedef struct {
  UINT16                      TableLength;        ///< Table Length
  UINT32                      SaveOffset;         ///< Save Location
  BOOLEAN                     Locked;             ///< Locked
} S3_SAVE_TABLE_HEADER;

/// S3 write operation header
typedef struct {
  UINT16                      OpCode;             ///< Opcode
  ACCESS_WIDTH                Width;              ///< Data width (byte, word, dword)
  UINT64                      Address;            ///< Register address
  UINT32                      Count;              ///< Write count
} S3_WRITE_OP_HEADER;

/// S3 Read and Write Operation header
typedef struct {
  UINT16                      OpCode;             ///< Opcode
  ACCESS_WIDTH                Width;              ///< Data width (byte, word, dword)
  UINT64                      Address;            ///< Register Address
} S3_READ_WRITE_OP_HEADER;

/// S3 Poll operation header
typedef struct {
  UINT16                      OpCode;             ///< Opcode
  ACCESS_WIDTH                Width;              ///< Data width (byte, word, dword)
  UINT64                      Address;            ///< Register address
  UINT64                      Delay;              ///< Time delay
} S3_POLL_OP_HEADER;

/// Information operation header
typedef struct {
  UINT16                      OpCode;             ///< Opcode
  UINT32                      Length;             ///< Length of info
} S3_INFO_OP_HEADER;

/// Dispatch operation header
typedef struct {
  UINT16                      OpCode;             ///< Opcode
  UINT16                      FunctionId;         ///< Function ID
  UINT16                      Length;             ///< Length in bytes of the context
} S3_DISPATCH_OP_HEADER;


typedef VOID S3_DISPATCH_FUNCTION (
  IN      AMD_CONFIG_PARAMS     *StdHeader,
  IN      UINT16                ContextLength,
  IN      VOID                  *Context
  );

/// Dispatch function table entry
typedef struct {
  UINT16                        FunctionId;         ///<Function ID
  S3_DISPATCH_FUNCTION          *Function;          ///<Function pointer
} S3_DISPATCH_FUNCTION_ENTRY;

typedef AGESA_STATUS (*S3_SCRIPT_INIT) (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  );

typedef AGESA_STATUS (*S3_SCRIPT_RESTORE) (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  );

/// S3 Script Configuration
typedef struct {
  S3_SCRIPT_INIT              Init;               ///< Script initialization
  S3_SCRIPT_RESTORE           Restore;            ///< Script restore
} S3_SCRIPT_CONFIGURATION;

AGESA_STATUS
S3ScriptInit (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  );

AGESA_STATUS
S3ScriptInitState (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  );

AGESA_STATUS
S3ScriptInitStateStub (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  );

AGESA_STATUS
S3ScriptRestore (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  );

AGESA_STATUS
S3ScriptRestoreState (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  );

AGESA_STATUS
S3ScriptRestoreStateStub (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  );

AGESA_STATUS
S3ScriptGetS3SaveTable (
  IN      AMD_CONFIG_PARAMS     *StdHeader,
  OUT     S3_SAVE_TABLE_HEADER  **S3SaveTable
  );

VOID
S3SaveDebugPrintHexArray (
  IN      AMD_CONFIG_PARAMS     *StdHeader,
  IN      VOID                  *Array,
  IN      UINT32                Count,
  IN      ACCESS_WIDTH          Width
  );

CHAR8*
S3SaveDebugOpcodeString (
  IN      AMD_CONFIG_PARAMS     *StdHeader,
  IN      UINT16                Op
  );

AGESA_STATUS
S3SaveStateSaveWriteOp (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      UINT16              OpCode,
  IN      ACCESS_WIDTH        Width,
  IN      UINT64              Address,
  IN      UINT32              Count,
  IN      VOID                *Buffer
  );

AGESA_STATUS
S3SaveStateSaveReadWriteOp (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      UINT16              OpCode,
  IN      ACCESS_WIDTH        Width,
  IN      UINT64              Address,
  IN      VOID                *Data,
  IN      VOID                *DataMask
  );

AGESA_STATUS
S3SaveStateSavePollOp (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      UINT16              OpCode,
  IN      ACCESS_WIDTH        Width,
  IN      UINT64              Address,
  IN      VOID                *Data,
  IN      VOID                *DataMask,
  IN      UINT64              Delay
  );

AGESA_STATUS
S3SaveStateSaveInfoOp (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      UINT16              OpCode,
  IN      UINT32              InformationLength,
  IN      VOID                *Information
  );

AGESA_STATUS
S3SaveStateSaveDispatchOp (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      UINT16              OpCode,
  IN      UINT16              FunctionId,
  IN      UINT16              ContextLength,
  IN      VOID                *Context
  );

//PCI write
#define S3_SAVE_PCI_WRITE(StdHeader, Address, Width, DataPtr) \
  S3SaveStateSaveWriteOp (  \
    StdHeader,  \
    SAVE_STATE_PCI_CONFIG_WRITE_OPCODE,  \
    Width,  \
    Address.AddressValue, \
    1,  \
    DataPtr  \
    )

//PCI read modify write
#define S3_SAVE_PCI_RMW (StdHeader, Address, Width, DataPtr, DataMaskPtr) \
  S3SaveStateSaveWriteOp (  \
    StdHeader,  \
    SAVE_STATE_PCI_CONFIG_READ_WRITE_OPCODE,  \
    Width,  \
    Address.AddressValue, \
    DataPtr,  \
    DataMask  \
    )

//PCI read modify write
#define S3_SAVE_PCI_POLL(StdHeader, Address, Width, DataPtr, DataMaskPtr, Delay) \
  S3SaveStateSavePollOp (  \
    StdHeader,  \
    SAVE_STATE_PCI_CONFIG_POLL_OPCODE,  \
    Width,  \
    Address.AddressValue, \
    DataPtr,  \
    DataMask,  \
    Delay  \
    )

//Memory/MMIO write
#define S3_SAVE_MEM_WRITE(StdHeader, Address, Width, DataPtr) \
  S3SaveStateSaveWriteOp (  \
    StdHeader,  \
    SAVE_STATE_MEM_WRITE_OPCODE,  \
    Width,  \
    Address, \
    1,  \
    DataPtr  \
    )

//Memory/MMIO read modify write
#define S3_SAVE_MEM_RMW(StdHeader, Address, Width, DataPtr, DataMaskPtr) \
  S3SaveStateSaveWriteOp (  \
    StdHeader,  \
    SAVE_STATE_MEM_READ_WRITE_OPCODE,  \
    Width,  \
    Address, \
    DataPtr,  \
    DataMask  \
    )

//Memory/MMIO read modify write
#define S3_SAVE_MEM_POLL(StdHeader, Address, Width, DataPtr, DataMaskPtr, Delay) \
  S3SaveStateSavePollOp (  \
    StdHeader,  \
    SAVE_STATE_MEM_POLL_OPCODE,  \
    Width,  \
    Address, \
    DataPtr,  \
    DataMask,  \
    Delay \
    )

// I/O write
#define S3_SAVE_IO_WRITE(StdHeader, Address, Width, DataPtr) \
  S3SaveStateSaveWriteOp (  \
    StdHeader,  \
    SAVE_STATE_IO_WRITE_OPCODE,  \
    Width,  \
    Address, \
    1,  \
    DataPtr  \
    )

// Save information
#define S3_SAVE_INFORMATION(StdHeader, InformationLength, Information) \
  S3SaveStateSaveInfoOp (  \
    StdHeader,  \
    SAVE_STATE_INFORMATION_OPCODE,  \
    InformationLength,  \
    Information \
    )

// Save information string S3_SAVE_INFORMATION_STRING (StdHeader, "Message")
#define S3_SAVE_INFORMATION_STRING(StdHeader, Information) \
  S3SaveStateSaveInfoOp (  \
    StdHeader,  \
    SAVE_STATE_INFORMATION_OPCODE,  \
    sizeof (Information),  \
    Information \
    )

// Save dispatch function
#define S3_SAVE_DISPATCH(StdHeader, FunctionId, ContextLength, Context) \
  S3SaveStateSaveDispatchOp (  \
    StdHeader,  \
    SAVE_STATE_DISPATCH_OPCODE,  \
    FunctionId, \
    ContextLength,  \
    Context \
    )

#pragma pack (pop)
#endif
