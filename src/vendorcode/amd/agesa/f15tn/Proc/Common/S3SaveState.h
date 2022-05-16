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
  CONST IN      VOID                  *Array,
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
  CONST IN      VOID                *Buffer
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
