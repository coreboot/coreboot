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
 * @e \$Revision: 85947 $   @e \$Date: 2013-01-14 17:25:21 -0600 (Mon, 14 Jan 2013) $
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
#ifndef _GNBURA_H_
#define _GNBURA_H_

#include "GnbPcie.h"

/// Generic device object
typedef struct {
  PCI_ADDR              DevPciAddress;  ///< DevPciAddress
  GNB_HANDLE            *GnbHandle;     ///< GnbHandle
  AMD_CONFIG_PARAMS     *StdHeader;     ///<
} DEV_OBJECT;

/// Register address 32 bit encoding
typedef struct {
  UINT32  Address: 31;         ///< Register address
  UINT32  MultInst: 1;         ///< Must equals to 0
} URA_REGISTER_32B_ENTRY;

/// Register address encoding that represents a group of registers that have same fields' definitions
typedef struct {
  UINT32  Addr: 24;         ///< Register address
  UINT32  InstOffset: 7;    ///< The address offset between register instances
  UINT32  MultInst: 1;      ///< Must equals to 1
} URA_MULT_REG_ENTRY;

/// 16 bit field encoding
typedef struct {
  UINT16  BfOffset: 6;       ///< Offset of the bit field
  UINT16  BfWidth: 6;        ///< Width of the bit field
  UINT16  Offset: 4;         ///< Number of field entries from its register address entry
                             ///< The first field entry of a register must be place right after its register entry and has offset 1.
                             ///< The second field is placed right after the first field and has offset 2 and so on.
                             ///< The offset field is maxed out at 15. So any entries after the 15th entries will take offset 0.
} URA_FIELD_16B_ENTRY;

/// 32 bit field encoding
typedef struct {
  UINT32  BfOffset: 6;        ///< Offset of the bit field
  UINT32  BfWidth: 6;         ///< Width of the bit field
  UINT32  RegIndex: 13;     ///< Direct index to its register address entry
  UINT32  Rev: 6;           ///< Contains ID of the family/rev that this bit field is valid
  UINT32  Reserved: 1;      ///< Reserved
} URA_FIELD_32B_ENTRY;

/// Register address 64 bit encoding
typedef struct {
  UINT64  DomainAddress:32;   ///< Register domain address
  UINT64  DomainType: 8;         ///< Register domain type
  UINT64  Reserved: 24;      ///< Reserved
} URA_REGISTER_64B_ENTRY;

/// Register / Field table entry
#define URA_ENTRY UINT16

/// Token
typedef union {
  UINT32      Encode;       ///< Token value

  struct {                  ///< Sub structure used to parse Token
    UINT32    Index: 13;    ///< Index into a specific register or field entry in register/field table
    UINT32    Type: 3;      ///< Entry type:
                            ///<   000b: 16-bit field location encoding
                            ///<   001b: Register address encoding
                            ///<   010b: 32-bit field location encoding
                            ///<   011b: 64-bit field location encoding
                            ///<   Others would reserved
    UINT32    Selector: 6;  ///< Used by unified register access to select the access method
    UINT32    InstSel: 6;   ///< When register/field has multiple instances, this field is used to obtain the register address of a register group.
    UINT32    S3Save:1;     ///< Indicate whether S3Save is needed.
    UINT32    StreamSet:1;  ///< Stream Set
    UINT32    ParentType:1; ///< Parent token type
    UINT32    Reserved: 1;  ///< Reserved for device specific usage
  } Parser;
} URA_TOKEN_STRUCT;

#define URA_TOKEN UINT32

#define URA_TYPE_FIELD_16  0
#define URA_TYPE_FIELD_32  1
#define URA_TYPE_REGISTER_32  2
#define URA_TYPE_REGISTER_64  3

#define URA_TOKEN_PARENT_TYPE_32     0x20000000ul
#define URA_TOKEN_PARENT_TYPE_64     0x00000000ul

/**
  TOKEN_DEF(Index, Type, Selector)
  Defines a register of field token

  @param[in]    Index
  @param[in]    Type
  @param[in]    Selector
  @param[in]    ParentType

  @return   URA_TOKEN   Encrypted URA_TOKEN format
--*/
#define TOKEN_DEF(Index, Type, Selector, ParentType) ( \
    (((UINT32) (Index)) | (((UINT32) (Type)) << 13) | (((UINT32) (Selector)) << 16) | ((UINT32) (ParentType))) \
  )

#define _RESERVED                         0xFFFFFFFFul
#define _UNUSED                           0xFEFEFEFEul
#define GNB_URA_FLAG_S3SAVE               0x00000001ul
#define GNB_URA_STREAM_SET                0x20000000ul

/// Structure used to pass token info to access methods
typedef struct {
  UINT32        RegAddress;        ///< Register address
  UINT8         BfOffset;          ///< Offset of the bit field
  UINT8         BfWidth;           ///< Width of the bit field
  BOOLEAN       WholeRegAccess;    ///< Whole register access
  UINT32        MethodType;        ///< Index into the access method table to select access method
  UINT32        Flags;             ///< S3
  UINT32        StreamSet;         ///< StreamSet
  UINT8         RegDomainType;     ///< Register Domain type
} URA_TOKEN_INFO;

#define TYPE_GNB_INDIRECT_ACCESS        0
#define TYPE_GNB_PROTOCOL_ACCESS        1

/// URA_TUPLE
typedef struct {
  URA_TOKEN     Token;       ///< Token
  UINT32        Value;       ///< Token value
  UINT32        StepLength;  ///< Byte length to next address for stream set usage.
} URA_TUPLE;

typedef VOID F_GNBURASERVICELOCATEREGTBL (
  IN       DEV_OBJECT          *Device,
  IN       UINT32              *UraTableAddress
  );

typedef VOID F_GNBURASERVICEGET (
  IN       DEV_OBJECT       *Device,
  IN       URA_TOKEN_INFO   *UraTokenInfo,
  IN OUT   VOID             *Value
  );

typedef VOID F_GNBURASERVICESET (
  IN       DEV_OBJECT       *Device,
  IN       URA_TOKEN_INFO   *UraTokenInfo,
  IN OUT   VOID             *Value
  );

typedef VOID F_GNBURASERVICESTREAMSET (
  IN       DEV_OBJECT        *Device,
  IN       URA_TOKEN_INFO    *UraTokenInfo,
  IN OUT   URA_TUPLE         *UraTuple,
  IN       UINT32            CombinedCount
  );

/// Register Read/Write protocol
typedef struct {
  F_GNBURASERVICELOCATEREGTBL *GnbUraLocateRegTbl;   ///<
  F_GNBURASERVICEGET *GnbUraGet;                     ///<
  F_GNBURASERVICESET *GnbUraSet;                     ///<
  F_GNBURASERVICESTREAMSET *GnbUraStreamSet;         ///<
} GNB_URA_SERVICE;

#endif
