/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to access PCI config space registers
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "amdlib.h"
#include  "Gnb.h"
#include  "GnbLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbCommonLib.h"
#include  "GnbUraServices.h"
#include  "Filecode.h"

#define FILECODE PROC_GNB_MODULES_GNBURALIBV1_GNBURALIBV1_FILECODE

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

CONST UINT8 RegisterDomainMap[] = {
  0x0,          ///< Stub
  0x0,          ///< Stub
  0x64,         ///< 0x2
  0x98,         ///< 0x3
  0xB8,         ///< TYPE_D0F0xB8
  0xFD        ///< End
};

/*----------------------------------------------------------------------------------------*/
/**
 * Convert URA Token to register address and field location
 *
 * @param[in]        UraToken            Register of bit field identifier
 * @param[in]        UraTable            Pointer to register token table
 * @param[in, out]   TokenInfo           Pointer to URA_TOKEN_INFO structure
 *
 * @retval     UraToken   Token is defined
 * @retval     _UNUSED    Token is undefined
 *
 */
URA_TOKEN
STATIC
UraTranslateToken (
  IN       URA_TOKEN         UraToken,
  IN       URA_ENTRY         *UraTable,
  IN OUT   URA_TOKEN_INFO    *TokenInfo
  )
{
  URA_TOKEN_STRUCT  Token;
  URA_FIELD_16B_ENTRY *Field16;
  URA_REGISTER_32B_ENTRY  *Reg32;
  URA_REGISTER_64B_ENTRY  *Reg64;

  Token.Encode = UraToken;
  Reg32 = NULL;
  Reg64 = NULL;
  Field16 = NULL;

  switch (Token.Parser.Type) {
  //
  // Parse 16 bit field encoding
  //
  case URA_TYPE_FIELD_16:
    Field16 = (URA_FIELD_16B_ENTRY *) &(UraTable[Token.Parser.Index]);
    TokenInfo->BfWidth = (UINT8) Field16->BfWidth;
    TokenInfo->BfOffset = (UINT8) Field16->BfOffset;
    while (Field16->Offset == 0) {
      Token.Parser.Index -= 15;
      Field16 = (URA_FIELD_16B_ENTRY *) &UraTable[Token.Parser.Index];
    }

    ASSERT (Field16->Offset != 0);

    if (Token.Parser.ParentType == URA_TOKEN_PARENT_TYPE_32) {
      Token.Parser.Index -= (Field16->Offset - 1) * 2 + 2;
      Reg32 = (URA_REGISTER_32B_ENTRY *) &UraTable[Token.Parser.Index];
    } else if (Token.Parser.ParentType == URA_TOKEN_PARENT_TYPE_64) {
      Token.Parser.Index -= Field16->Offset + 3;
      Reg64 = (URA_REGISTER_64B_ENTRY *) &(UraTable[Token.Parser.Index]);
    } else {
      ASSERT (FALSE);
      return _UNUSED;
    }

    TokenInfo->WholeRegAccess = FALSE;
    break;

  //
  // Parse 32 bit register encoding
  //
  case URA_TYPE_REGISTER_32:
    Reg32 = (URA_REGISTER_32B_ENTRY *) &UraTable[Token.Parser.Index];
    if (Reg32->Address == _UNUSED) {
      return _UNUSED;
    }
    TokenInfo->WholeRegAccess = TRUE;
    break;

  //
  // Parse 64 bit register encoding
  //
  case URA_TYPE_REGISTER_64:
    Reg64 = (URA_REGISTER_64B_ENTRY *) &(UraTable[Token.Parser.Index]);
    if (Reg64->DomainAddress == _UNUSED) {
      return _UNUSED;
    }
    TokenInfo->WholeRegAccess = TRUE;
    break;

  default:
    ASSERT (FALSE);
  }

  // Get register address
  if ((Token.Parser.ParentType == URA_TOKEN_PARENT_TYPE_32) && (Reg32 != NULL)) {
    TokenInfo->RegAddress = Reg32->Address;
    TokenInfo->RegDomainType = 0;
  } else if ((Token.Parser.ParentType == URA_TOKEN_PARENT_TYPE_64) && (Reg64 != NULL)) {
    TokenInfo->RegAddress = (UINT32)Reg64->DomainAddress;
    TokenInfo->RegDomainType = (UINT8)Reg64->DomainType;
  } else {
    ASSERT (FALSE);
    return _UNUSED;
  }

  TokenInfo->MethodType = Token.Parser.Selector;

  // Patch DomainType field
  if (TokenInfo->MethodType == TYPE_GNB_INDIRECT_ACCESS) {
    TokenInfo->RegDomainType = RegisterDomainMap[TokenInfo->RegDomainType];
  }

  TokenInfo->Flags = Token.Parser.S3Save;
  TokenInfo->StreamSet = Token.Parser.StreamSet;

  return Token.Encode;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Gnb Unified Register Access method
 *
 *
 * @param[in]       Device       Standard configuration header
 * @param[in]       UraToken     Context length
 * @param[in, out]  Value        Pointer to Context
 */
VOID
GnbUraGet (
  IN       DEV_OBJECT    *Device,
  IN       URA_TOKEN     UraToken,
  IN OUT   VOID          *Value
  )
{
  AGESA_STATUS              Status;
  GNB_URA_SERVICE           *GnbUraProtocol;
  URA_TOKEN_INFO            TokenInfo;
  URA_ENTRY                 *UraTable;
  UINT32                    UraTableAddress;

  UraTable = NULL;
  Status = GnbLibLocateService (GnbUraService, GnbGetSocketId (Device->GnbHandle), (CONST VOID **)&GnbUraProtocol, Device->StdHeader);
  ASSERT (Status == AGESA_SUCCESS);

  GnbUraProtocol->GnbUraLocateRegTbl (Device, &UraTableAddress);
  UraTable = (URA_ENTRY *) ((UINTN)UraTableAddress);
  UraToken = UraTranslateToken (UraToken, UraTable, &TokenInfo);
  if (UraToken == _UNUSED) {
    return;
  }

  GnbUraProtocol->GnbUraGet (Device, &TokenInfo, Value);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Gnb Unified Register Access method
 *
 *
 * @param[in]  Device       Standard configuration header
 * @param[in]  UraToken     Ura token
 * @param[in]  Value        Pointer to Context
 */
VOID
GnbUraSet (
  IN       DEV_OBJECT          *Device,
  IN       URA_TOKEN           UraToken,
  IN       VOID                *Value
  )
{
  AGESA_STATUS              Status;
  GNB_URA_SERVICE           *GnbUraProtocol;
  URA_TOKEN_INFO            TokenInfo;
  URA_ENTRY                 *UraTable;
  UINT32                    UraTableAddress;

  Status = GnbLibLocateService (GnbUraService, GnbGetSocketId (Device->GnbHandle), (CONST VOID **)&GnbUraProtocol, Device->StdHeader);
  ASSERT (Status == AGESA_SUCCESS);

  GnbUraProtocol->GnbUraLocateRegTbl (Device, &UraTableAddress);
  UraTable = (URA_ENTRY *) ((UINTN)UraTableAddress);
  UraToken = UraTranslateToken (UraToken, UraTable, &TokenInfo);
  if (UraToken == _UNUSED) {
    return;
  }

  GnbUraProtocol->GnbUraSet (Device, &TokenInfo, Value);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Gnb Unified Register Access method
 *
 *
 * @param[in]       Device             Standard configuration header
 * @param[in, out]  UraTokenRegister   Register token
 * @param[in]       UraTuple           Pointer to Context
 * @param[in, out]  CombinedCount      Token count
 */
VOID
GnbUraCombinedGet (
  IN       DEV_OBJECT        *Device,
  IN       URA_TOKEN         UraTokenRegister,
  IN OUT   URA_TUPLE         *UraTuple,
  IN       UINT32            CombinedCount
  )
{
  AGESA_STATUS            Status;
  GNB_URA_SERVICE         *GnbUraProtocol;
  URA_TOKEN_INFO          TokenInfo;
  URA_ENTRY               *UraTable;
  UINT32                  UraTableAddress;
  UINT32                  RegValue;
  UINT32                  TempValue;
  UINT32                  FieldMask;
  URA_TOKEN               UraToken;
  UINT32                  Index;

  Status = GnbLibLocateService (GnbUraService, GnbGetSocketId (Device->GnbHandle), (CONST VOID **)&GnbUraProtocol, Device->StdHeader);
  ASSERT (Status == AGESA_SUCCESS);

  GnbUraProtocol->GnbUraLocateRegTbl (Device, &UraTableAddress);
  UraTable = (URA_ENTRY *) ((UINTN)UraTableAddress);
  UraTokenRegister = UraTranslateToken (UraTokenRegister, UraTable, &TokenInfo);
  if (UraTokenRegister == _UNUSED) {
    return;
  }

  GnbUraProtocol->GnbUraGet (Device, &TokenInfo, &RegValue);

  Index = 0;
  do {
    UraToken = UraTuple[Index].Token;
    UraToken = UraTranslateToken (UraToken, UraTable, &TokenInfo);
    if (UraToken == _UNUSED) {
      return;
    }

    TempValue = RegValue;
    TempValue = TempValue >> TokenInfo.BfOffset;
    FieldMask = (((UINT32)1 << TokenInfo.BfWidth) - 1);
    UraTuple[Index].Value = TempValue & FieldMask;
    Index++;
  } while (Index < CombinedCount);

}

/*----------------------------------------------------------------------------------------*/
/**
 * Gnb Unified Register Access method
 *
 *
 * @param[in]       Device             Standard configuration header
 * @param[in, out]  UraTokenRegister   Register token
 * @param[in]       UraTuple           Pointer to Context
 * @param[in, out]  CombinedCount      Token count
 */
VOID
GnbUraCombinedSet (
  IN       DEV_OBJECT     *Device,
  IN       URA_TOKEN      UraTokenRegister,
  IN OUT   URA_TUPLE      *UraTuple,
  IN       UINT32         CombinedCount
  )
{
  AGESA_STATUS            Status;
  GNB_URA_SERVICE         *GnbUraProtocol;
  URA_TOKEN_INFO          TokenInfo;
  URA_ENTRY               *UraTable;
  UINT32                  UraTableAddress;
  UINT32                  RegValue;
  UINT32                  TempValue;
  UINT32                  FieldMask;
  URA_TOKEN               UraToken;
  UINT32                  Index;

  Status = GnbLibLocateService (GnbUraService, GnbGetSocketId (Device->GnbHandle), (CONST VOID **)&GnbUraProtocol, Device->StdHeader);
  ASSERT (Status == AGESA_SUCCESS);

  GnbUraProtocol->GnbUraLocateRegTbl (Device, &UraTableAddress);
  UraTable = (URA_ENTRY *) ((UINTN)UraTableAddress);
  UraTokenRegister = UraTranslateToken (UraTokenRegister, UraTable, &TokenInfo);
  if (UraTokenRegister == _UNUSED) {
    return;
  }

  if (TokenInfo.StreamSet == 1) {
    GnbUraProtocol->GnbUraStreamSet (Device, &TokenInfo, UraTuple, CombinedCount);
  } else {
    GnbUraProtocol->GnbUraGet (Device, &TokenInfo, &RegValue);

    Index = 0;
    do {
      UraToken = UraTuple[Index].Token;
      UraToken = UraTranslateToken (UraToken, UraTable, &TokenInfo);
      if (UraToken == _UNUSED) {
        return;
      }
      FieldMask = (((UINT32)1 << TokenInfo.BfWidth) - 1);
      TempValue = UraTuple[Index].Value & FieldMask;
      RegValue &= ~(FieldMask << TokenInfo.BfOffset);
      RegValue |= TempValue << TokenInfo.BfOffset;
      Index++;
    } while (Index < CombinedCount);

    UraTokenRegister = UraTranslateToken (UraTokenRegister, UraTable, &TokenInfo);
    GnbUraProtocol->GnbUraSet (Device, &TokenInfo, &RegValue);
  }
}
