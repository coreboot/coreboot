/* $NoKeywords:$ */
/**
 * @file
 *
 * SMU access routine
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 39275 $   @e \$Date: 2010-10-09 08:22:05 +0800 (Sat, 09 Oct 2010) $
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
#include  "AGESA.h"
#include  "amdlib.h"
#include  "Ids.h"
#include  "Gnb.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  "OptionGnb.h"
#include  "NbSmuLib.h"
#include  "GnbRegistersON.h"
#include  "S3SaveState.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_NB_NBSMULIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/// Efuse write packet
typedef struct {
  SMUx0B_x8600_STRUCT SMUx0B_x8600;  ///< Reg  SMUx0B_x8600
  SMUx0B_x8604_STRUCT SMUx0B_x8604;  ///< Reg  SMUx0B_x8604
  SMUx0B_x8608_STRUCT SMUx0B_x8608;  ///< Reg  SMUx0B_x8605
} MBUS;


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * SMU indirect register read
 *
 *
 *
 * @param[in]   Address           Register Address
 * @param[in]   Width             Data width for read
 * @param[out]  Value             Pointer read value
 * @param[in]   StdHeader          Pointer to standard configuration
 */


VOID
NbSmuIndirectRead (
  IN       UINT8               Address,
  IN       ACCESS_WIDTH        Width,
     OUT   VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{

  D0F0x64_x4D_STRUCT  D0F0x64_x4D;
  UINT32              Data;
  GnbLibPciIndirectRead (
    MAKE_SBDFO (0, 0, 0, 0, D0F0x60_ADDRESS),
    D0F0x64_x4D_ADDRESS | IOC_WRITE_ENABLE,
    AccessWidth32,
    &D0F0x64_x4D.Value,
    StdHeader
    );

  D0F0x64_x4D.Field.ReqType = 0;
  D0F0x64_x4D.Field.SmuAddr = Address;
  if (Width == AccessS3SaveWidth32 || Width == AccessWidth32) {
    D0F0x64_x4D.Field.SmuAddr += 1;
  }

  D0F0x64_x4D.Field.ReqToggle = !D0F0x64_x4D.Field.ReqToggle;

  GnbLibPciIndirectWrite (
    MAKE_SBDFO (0, 0, 0, 0, D0F0x60_ADDRESS),
    D0F0x64_x4D_ADDRESS | IOC_WRITE_ENABLE,
    (Width >= AccessS3SaveWidth8) ? AccessS3SaveWidth32 : AccessWidth32,
    &D0F0x64_x4D.Value,
    StdHeader
    );

  GnbLibPciIndirectRead (
    MAKE_SBDFO (0, 0, 0, 0, D0F0x60_ADDRESS),
    D0F0x64_x4E_ADDRESS | IOC_WRITE_ENABLE,
    (Width >= AccessS3SaveWidth8) ? AccessS3SaveWidth32 : AccessWidth32,
    &Data,
    StdHeader
    );

  switch (Width) {
  case AccessWidth16:
    //no break; intended to fall through
  case AccessS3SaveWidth16:
    *(UINT16 *) Value = (UINT16) Data;
    break;
  case AccessWidth32:
    //no break; intended to fall through
  case AccessS3SaveWidth32:
    *(UINT32 *) Value = Data;
    break;
  default:
    ASSERT (FALSE);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * SMU indirect register read
 *
 *
 *
 * @param[in]   Address       Register Address
 * @param[in]   Width         Access width
 * @param[in]   Mask          Data mask for compare
 * @param[in]   CompateData   Compare data
 * @param[in]   StdHeader     Pointer to standard configuration
 */


VOID
NbSmuIndirectPoll (
  IN      UINT8               Address,
  IN      ACCESS_WIDTH        Width,
  IN      UINT32              Mask,
  IN      UINT32              CompateData,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32  Value;

  do {
    NbSmuIndirectRead (
      Address,
      Width,
      &Value,
      StdHeader
      );
  } while ((Value & Mask) != CompateData);
}


/*----------------------------------------------------------------------------------------*/
/**
 * SMU indirect register write
 *
 *
 *
 * @param[in]   Address           Register Address
 * @param[in]   Width             Data width for write
 * @param[in]   Value             Pointer to write value
 * @param[in]   StdHeader         Pointer to standard configuration
 */


VOID
NbSmuIndirectWriteEx (
  IN      UINT8               Address,
  IN      ACCESS_WIDTH        Width,
  CONST IN      VOID                *Value,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  D0F0x64_x4D_STRUCT  D0F0x64_x4D;
  ASSERT (Width != AccessWidth8);
  ASSERT (Width != AccessS3SaveWidth8);

  GnbLibPciIndirectRead (
    MAKE_SBDFO (0, 0, 0, 0, D0F0x60_ADDRESS),
    D0F0x64_x4D_ADDRESS | IOC_WRITE_ENABLE,
    AccessWidth32,
    &D0F0x64_x4D.Value,
    StdHeader
    );

  D0F0x64_x4D.Field.ReqType = 0x1;
  D0F0x64_x4D.Field.SmuAddr = Address;
  D0F0x64_x4D.Field.ReqToggle = (!D0F0x64_x4D.Field.ReqToggle);

  D0F0x64_x4D.Field.WriteData = ((UINT16 *) Value) [0];

  GnbLibPciIndirectWrite (
    MAKE_SBDFO (0, 0, 0, 0, D0F0x60_ADDRESS),
    D0F0x64_x4D_ADDRESS | IOC_WRITE_ENABLE,
    (Width >= AccessS3SaveWidth8) ? AccessS3SaveWidth32 : AccessWidth32,
    &D0F0x64_x4D.Value,
    StdHeader
    );
  if (LibAmdAccessWidth (Width) <= 2) {
    return;
  }
  D0F0x64_x4D.Field.ReqType = 0x1;
  D0F0x64_x4D.Field.SmuAddr = Address + 1;
  D0F0x64_x4D.Field.ReqToggle = (!D0F0x64_x4D.Field.ReqToggle);
  D0F0x64_x4D.Field.WriteData = ((UINT16 *) Value)[1];

  GnbLibPciIndirectWrite (
    MAKE_SBDFO (0, 0, 0, 0, D0F0x60_ADDRESS),
    D0F0x64_x4D_ADDRESS | IOC_WRITE_ENABLE,
    (Width >= AccessS3SaveWidth8) ? AccessS3SaveWidth32 : AccessWidth32,
    &D0F0x64_x4D.Value,
    StdHeader
    );
}


/*----------------------------------------------------------------------------------------*/
/**
 * SMU indirect register write
 *
 *
 *
 * @param[in]   Address           Register Address
 * @param[in]   Width             Data width for write
 * @param[in]   Value             Pointer to write value
 * @param[in]   StdHeader         Pointer to standard configuration
 */


VOID
NbSmuIndirectWrite (
  IN      UINT8               Address,
  IN      ACCESS_WIDTH        Width,
  CONST IN      VOID                *Value,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  if (Width >= AccessS3SaveWidth8) {
    SMU_INDIRECT_WRITE_DATA   Data;
    Data.Address = Address;
    Data.Width = Width;
    Data.Value = *((UINT32*) Value);
    S3_SAVE_DISPATCH (StdHeader, S3DispatchGnbSmuIndirectWrite, sizeof (SMU_INDIRECT_WRITE_DATA), &Data);
    Width = Width - (AccessS3SaveWidth8 - AccessWidth8);
  }
  NbSmuIndirectWriteEx (Address, Width, Value, StdHeader);
}


/*----------------------------------------------------------------------------------------*/
/**
 * SMU Service request for S3 script
 *
 *
 * @param[in] StdHeader       Standard configuration header
 * @param[in] ContextLength   Not used
 * @param[in] Context         Pointer to service request ID
 */

VOID
NbSmuIndirectWriteS3Script (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      UINT16              ContextLength,
  IN      VOID*               Context
  )
{
  SMU_INDIRECT_WRITE_DATA   *Data;
  Data = (SMU_INDIRECT_WRITE_DATA*) Context;
  NbSmuIndirectWriteEx (Data->Address, Data->Width, &Data->Value, StdHeader);
}

/*----------------------------------------------------------------------------------------*/
/**
 * SMU RAM mapped register write
 *
 *
 *
 * @param[in]   Address         Register Address
 * @param[in]   Value           Data pointer for write
 * @param[in]   Count           Number of registers to write
 * @param[in]   S3Save          Save for S3 (True/False)
 * @param[in]   StdHeader       Standard configuration header
 */

VOID
NbSmuRcuRegisterWrite (
  IN      UINT16              Address,
  CONST IN      UINT32              *Value,
  IN      UINT32              Count,
  IN      BOOLEAN             S3Save,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32 CurrentAddress;
  CurrentAddress = Address;
  NbSmuIndirectWrite (
    SMUx0B_ADDRESS,
    S3Save ? AccessS3SaveWidth16 : AccessWidth16,
    &Address,
    StdHeader
    );
  while (Count-- > 0) {
    IDS_HDT_CONSOLE (NB_SMUREG_TRACE, "    *WR SMUx0B:0x%x = 0x%x\n", CurrentAddress, *Value);
    NbSmuIndirectWrite (
      SMUx05_ADDRESS,
      S3Save ? AccessS3SaveWidth32 : AccessWidth32,
      Value++,
      StdHeader
    );
    CurrentAddress += 4;
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * SMU RAM mapped register read
 *
 *
 *
 * @param[in]   Address           Register Address
 * @param[out]  Value             Pointer read value
 * @param[in]   Count             Number of registers to read
 * @param[in]   StdHeader         Pointer to standard configuration
 */

VOID
NbSmuRcuRegisterRead (
  IN       UINT16              Address,
     OUT   UINT32              *Value,
  IN       UINT32              Count,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  NbSmuIndirectWrite (SMUx0B_ADDRESS, AccessWidth16, &Address, StdHeader);
  while (Count-- > 0) {
    NbSmuIndirectRead (SMUx05_ADDRESS, AccessWidth32, Value++, StdHeader);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * SMU Service request Ext
 *
 *
 * @param[in] RequestId       request ID
 * @param[in] Flags           Flags
 * @param[in] StdHeader       Standard configuration header
 */

VOID
NbSmuServiceRequestEx (
  IN      UINT8               RequestId,
  IN      UINT8               Flags,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  SMUx03_STRUCT SMUx03;
  SMUx03.Value = 0;
  SMUx03.Field.IntReq = 1;
  SMUx03.Field.ServiceIndex = RequestId;
  NbSmuIndirectWrite (SMUx03_ADDRESS, AccessWidth32, &SMUx03.Value, StdHeader);
  if ((Flags & SMU_EXT_SERVICE_FLAGS_POLL_ACK) != 0) {
    NbSmuIndirectPoll (SMUx03_ADDRESS, AccessWidth32, BIT1, BIT1, StdHeader);            // Wait till IntAck
  }
  if ((Flags & SMU_EXT_SERVICE_FLAGS_POLL_DONE) != 0) {
    NbSmuIndirectPoll (SMUx03_ADDRESS, AccessWidth32, BIT2, BIT2, StdHeader);            // Wait till IntDone
  }
  SMUx03.Value = 0;                                                      // Clear IRQ register
  NbSmuIndirectWrite (SMUx03_ADDRESS, AccessWidth32, &SMUx03.Value, StdHeader);
}

/*----------------------------------------------------------------------------------------*/
/**
 * SMU Service request
 *
 *
 * @param[in] RequestId       request ID
 * @param[in] S3Save          Save for S3 (True/False)
 * @param[in] StdHeader       Standard configuration header
 */

VOID
NbSmuServiceRequest (
  IN      UINT8               RequestId,
  IN      BOOLEAN             S3Save,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "NbSmuServiceRequest Enter [0x%02x]\n", RequestId);
  if (S3Save) {
    S3_SAVE_DISPATCH (StdHeader, S3DispatchGnbSmuServiceRequest, sizeof (RequestId), &RequestId);
  }
  NbSmuServiceRequestEx (
    RequestId,
    SMU_EXT_SERVICE_FLAGS_POLL_ACK | SMU_EXT_SERVICE_FLAGS_POLL_DONE,
    StdHeader
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "NbSmuServiceRequest Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * SMU Service request for S3 script
 *
 *
 * @param[in] StdHeader       Standard configuration header
 * @param[in] ContextLength   Not used
 * @param[in] Context         Pointer to service request ID
 */

VOID
NbSmuServiceRequestS3Script (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      UINT16              ContextLength,
  IN      VOID*               Context
  )
{
  NbSmuServiceRequest (*((UINT8*) Context), FALSE, StdHeader);
}

/*----------------------------------------------------------------------------------------*/
/**
 * SMU Read FCR register
 *
 *
 * @param[in] Address          FCR Address
 * @param[in] StdHeader        Standard configuration header
 */

UINT32
NbSmuReadEfuse (
  IN      UINT32              Address,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32  Value;

  NbSmuSrbmRegisterRead (Address, &Value, StdHeader);
  Value = (Value >> 24) | (Value << 24) | ((Value >> 8) & 0xFF00) | ((Value << 8) & 0xFF0000);
  return Value;
}

/*----------------------------------------------------------------------------------------*/
/**
 * SMU Read arbitrary fuse field
 *
 *
 * @param[in] Chain            Address
 * @param[in] Offset           Offcet
 * @param[in] Length           Length
 * @param[in] StdHeader        Standard configuration header
 */

UINT32
NbSmuReadEfuseField (
  IN      UINT8               Chain,
  IN      UINT16              Offset,
  IN      UINT8               Length,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32  Value;
  UINT32  Result;
  UINT32  Address;
  UINT16  Shift;
  ASSERT (Length <= 32);
  Shift = (Offset - (Offset & ~0x7));
  Address = 0xFE000000 | (Chain << 12) | (Offset >> 3);
  Value = NbSmuReadEfuse (Address, StdHeader);
  Result = Value >> Shift;
  if ((Shift + Length) > 32) {
    Value = NbSmuReadEfuse (Address + 1, StdHeader);
    Result |= (Value << (32 - Shift));
  }
  Result &= ((1 << Length) - 1);
  return Value;
}

/*----------------------------------------------------------------------------------------*/
/**
 * SMU SRBM (GMM) register read
 *
 *
 *
 * @param[in]   Address           Register Address
 * @param[out]  Value             Pointer read value
 * @param[in]   StdHeader         Pointer to standard configuration
 */

VOID
NbSmuSrbmRegisterRead (
  IN       UINT32              Address,
     OUT   UINT32              *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  MBUS    Mbus;
  Mbus.SMUx0B_x8600.Value = (0x8650 << SMUx0B_x8600_MemAddr_7_0__OFFSET) |
                            (1 << SMUx0B_x8600_TransactionCount_OFFSET);
  Mbus.SMUx0B_x8604.Value = (4 << SMUx0B_x8604_Txn1TransferLength_7_0__OFFSET);
  Mbus.SMUx0B_x8608.Value = (UINT32) (3 << SMUx0B_x8608_Txn1Tsize_OFFSET);
  Mbus.SMUx0B_x8600.Field.Txn1MBusAddr_7_0_ = Address & 0xff;
  Mbus.SMUx0B_x8604.Field.Txn1MBusAddr_15_8_ = (Address >> 8) & 0xff;
  Mbus.SMUx0B_x8604.Field.Txn1MBusAddr_23_16_ = (Address >> 16) & 0xff;
  Mbus.SMUx0B_x8604.Field.Txn1MBusAddr_31_24_ = (Address >> 24) & 0xff;
  NbSmuRcuRegisterWrite (SMUx0B_x8600_ADDRESS, (UINT32*) &Mbus, 3, FALSE, StdHeader);
  NbSmuServiceRequest (0x0B, FALSE, StdHeader);
  NbSmuRcuRegisterRead (SMUx0B_x8650_ADDRESS, Value, 1, StdHeader);
}

/*----------------------------------------------------------------------------------------*/
/**
 * SMU SRBM (GMM) register write
 *
 *
 *
 * @param[in]   Address         Register Address
 * @param[in]   Value           Data pointer for write
 * @param[in]   S3Save          Save for S3 (True/False)
 * @param[in]   StdHeader       Standard configuration header
 */

VOID
NbSmuSrbmRegisterWrite (
  IN      UINT32              Address,
  IN      UINT32              *Value,
  IN      BOOLEAN             S3Save,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  MBUS    Mbus;
  IDS_HDT_CONSOLE (NB_SMUREG_TRACE, "    *WR SRBM (GMM):0x%x = 0x%x\n", Address, *Value);
  Mbus.SMUx0B_x8600.Value = (0x8650 << SMUx0B_x8600_MemAddr_7_0__OFFSET) |
                            (1 << SMUx0B_x8600_TransactionCount_OFFSET);
  Mbus.SMUx0B_x8604.Value = (4 << SMUx0B_x8604_Txn1TransferLength_7_0__OFFSET);
  Mbus.SMUx0B_x8608.Value = (UINT32) (3 << SMUx0B_x8608_Txn1Tsize_OFFSET);
  Mbus.SMUx0B_x8608.Field.Txn1Mode = 0x1;
  Mbus.SMUx0B_x8600.Field.Txn1MBusAddr_7_0_ = Address & 0xff;
  Mbus.SMUx0B_x8604.Field.Txn1MBusAddr_15_8_ = (Address >> 8) & 0xff;
  Mbus.SMUx0B_x8604.Field.Txn1MBusAddr_23_16_ = (Address >> 16) & 0xff;
  Mbus.SMUx0B_x8604.Field.Txn1MBusAddr_31_24_ = (Address >> 24) & 0xff;
  NbSmuRcuRegisterWrite (SMUx0B_x8600_ADDRESS, (UINT32*) &Mbus, 3, S3Save, StdHeader);
  NbSmuRcuRegisterWrite (SMUx0B_x8650_ADDRESS, Value, 1, S3Save, StdHeader);
  NbSmuServiceRequest (0x0B, S3Save, StdHeader);
}

/*----------------------------------------------------------------------------------------*/
/**
 * SMU firmware download
 *
 *
 *
 * @param[in] StdHeader        Pointer to Standard configuration
 * @param[in] Firmware         Pointer to SMU firmware header
 * @retval    AGESA_STATUS
 */

VOID
NbSmuFirmwareDownload (
  CONST      SMU_FIRMWARE_HEADER *Firmware,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINTN          Index;
  SMUx01_STRUCT  SMUx01;
  NbSmuServiceRequestEx (0x10, SMU_EXT_SERVICE_FLAGS_POLL_ACK , StdHeader);
  SMUx01.Value = (1 << SMUx01_RamSwitch_OFFSET) | (1 << SMUx01_VectorOverride_OFFSET);
  NbSmuIndirectWrite (SMUx01_ADDRESS, AccessWidth32, &SMUx01.Value, StdHeader);
  for (Index = 0; Index < Firmware->NumberOfBlock; Index++) {
    NbSmuRcuRegisterWrite (
      (Firmware->BlockArray)[Index].Address,
      (Firmware->BlockArray)[Index].Data,
      (Firmware->BlockArray)[Index].Length,
      FALSE,
      StdHeader
      );
  }
  SMUx01.Value = (1 << SMUx01_Reset_OFFSET) | (1 << SMUx01_VectorOverride_OFFSET);
  NbSmuIndirectWrite (SMUx01_ADDRESS, AccessWidth32, &SMUx01.Value, StdHeader);
}

/*----------------------------------------------------------------------------------------*/
/**
 * SMU firmware revision
 *
 *
 *
 * @param[in] StdHeader        Pointer to Standard configuration
 * @retval                     Firmware revision info
 */

SMU_FIRMWARE_REV
NbSmuFirmwareRevision (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  SMU_FIRMWARE_REV  Revision;
  UINT32            FmRev;
  NbSmuRcuRegisterRead (
    0x830C,
    &FmRev,
    1,
    StdHeader
    );
  Revision.MajorRev = ((UINT16*)&FmRev) [1];
  Revision.MinorRev = ((UINT16*)&FmRev) [0];
  return  Revision;
}
