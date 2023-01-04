/** @file

  @copyright
  INTEL CONFIDENTIAL
  Copyright 2007 -2021  Intel Corporation. <BR>

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary    and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.
**/

#ifndef _PREV_BOOT_ERR_SRC_HOB_H_
#define _PREV_BOOT_ERR_SRC_HOB_H_

#define FSP_PREV_BOOT_ERR_SRC_HOB_GUID \
{ 0xc5, 0xb5, 0x38, 0x51, 0x69, 0x93, 0xec, 0x48, 0x5b, 0x97, 0x38, 0xa2, 0xf7, 0x09, 0x66, 0x75 }

#define PREV_BOOT_ERR_SRC_HOB_SIZE  1000

#pragma pack(1)

typedef struct {
UINT16  Length;    // Actual size of the error sources  used in the HOB
UINT8  HobData[PREV_BOOT_ERR_SRC_HOB_SIZE -2];  // List of Error source structures of format //MCBANK_ERR_INFO  or CSR_ERR_INFO
}PREV_BOOT_ERR_SRC_HOB;

typedef struct{
UINT8  Type;    // McBankType = 1;
UINT8  Segment;
UINT8  Socket;
UINT16  ApicId;    // ApicId  is Needed only if it a core McBank.
UINT16  McBankNum;
UINT64  McBankStatus;
UINT64  McbankAddr;
UINT64  McBankMisc;
} MCBANK_ERR_INFO;

typedef struct {
UINT8  Type;    // PciExType =2 ;
UINT8  Segment;
UINT8  Bus;
UINT8  Device;
UINT8  Function;
UINT32  AerUncErrSts;
UINT8  AerHdrLogData[16];
} PCI_EX_ERR_INFO;

typedef struct {
UINT8  Type;    // Other Csr error type =3 ;
UINT8  Segment;
UINT8  Bus;
UINT8  Device;
UINT8  Function;
UINT16  offset;
UINT32  Value;
} CSR_ERR_INFO;

typedef enum {
  McBankType = 1,
  PciExType,
  CsrOtherType
} ERROR_ACCESS_TYPE;

#pragma pack()

#endif //_PREV_BOOT_ERR_SRC_HOB_H_