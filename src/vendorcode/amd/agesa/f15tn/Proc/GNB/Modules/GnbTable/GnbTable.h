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
#ifndef _GNBTABLE_H_
#define _GNBTABLE_H_

#include "GnbPcie.h"

#pragma pack (push, 1)

#define GNB_TABLE_FLAGS_FORCE_S3_SAVE  0x00000001ul

typedef UINT8 GNB_TABLE;

#define __DATA(x) x

#define _DATA32(Data) (__DATA(Data)) & 0xFF, ((__DATA(Data)) >> 8) & 0xFF, ((__DATA(Data)) >> 16) & 0xFF, ((__DATA(Data)) >> 24) & 0xFF
#define _DATA64(Data) _DATA32(Data & 0xfffffffful) , _DATA32(Data >> 32)

/// Entry type
typedef enum {
  GnbEntryWr,                     ///< Write register
  GnbEntryPropertyWr,             ///< Write register check property
  GnbEntryFullWr,                 ///< Write Rgister check revision and property
  GnbEntryRmw,                    ///< Read Modify Write register
  GnbEntryPropertyRmw,            ///< Read Modify Write register check property
  GnbEntryRevRmw,                 ///< Read Modify Write register check revision
  GnbEntryFullRmw,                ///< Read Modify Write register check revision and property
  GnbEntryPoll,                   ///< Poll register
  GnbEntryPropertyPoll,           ///< Poll register check property
  GnbEntryFullPoll,               ///< Poll register check property
  GnbEntryCopy,                   ///< Copy field from one register to another
  GnbEntryStall,                  ///< Copy field from one register to another
  GnbEntryTerminate = 0xFF        ///< Terminate table
} GNB_TABLE_ENTRY_TYPE;

#define GNB_ENTRY_WR(RegisterSpaceType, Address, Value) \
  GnbEntryWr, RegisterSpaceType, _DATA32 (Address), _DATA32 (Value)

/// Write register entry
typedef struct {
  UINT8   EntryType;              ///< Entry type
  UINT8   RegisterSpaceType;      ///< Register space
  UINT32  Address;                ///< Register address
  UINT32  Value;                  ///< Value
} GNB_TABLE_ENTRY_WR;

#define GNB_ENTRY_PROPERTY_WR(Property, RegisterSpaceType, Address, Value) \
  GnbEntryPropertyWr, _DATA32 (Property), RegisterSpaceType, _DATA32 (Address), _DATA32 (Value)

/// Write register entry
typedef struct {
  UINT8   EntryType;              ///< Entry type
  UINT32  Property;               ///< Property
  UINT8   RegisterSpaceType;      ///< Register space
  UINT32  Address;                ///< Register address
  UINT32  Value;                  ///< Value
} GNB_TABLE_ENTRY_PROPERTY_WR;


#define GNB_ENTRY_RMW(RegisterSpaceType, Address, AndMask, OrMask) \
  GnbEntryRmw, RegisterSpaceType, _DATA32 (Address), _DATA32 (AndMask), _DATA32 (OrMask)

///Read Modify Write data Block
typedef struct {
  UINT8   RegisterSpaceType;      ///< Register space
  UINT32  Address;                ///< Register address
  UINT32  AndMask;                ///< And Mask
  UINT32  OrMask;                 ///< Or Mask
} GNB_RMW_BLOCK;

/// Read Modify Write register entry
typedef struct {
  UINT8           EntryType;      ///< Entry type
  GNB_RMW_BLOCK   Data;           ///< Data
} GNB_TABLE_ENTRY_RMW;

#define GNB_ENTRY_FULL_WR(Property, Revision, RegisterSpaceType, Address, Value) \
   GnbEntryFullWr, _DATA32 (Property), _DATA64 (Revision), RegisterSpaceType, _DATA32 (Address), _DATA32 (Value)

/// Write register entry
typedef struct {
  UINT8   EntryType;              ///< Entry type
  UINT32  Property;               ///< Property
  UINT64  Revision;               ///< Revision
  UINT8   RegisterSpaceType;      ///< Register space
  UINT32  Address;                ///< Register address
  UINT32  Value;                  ///< Value
} GNB_TABLE_ENTRY_FULL_WR;


#define GNB_ENTRY_PROPERTY_RMW(Property,  RegisterSpaceType, Address, AndMask, OrMask) \
  GnbEntryPropertyRmw, _DATA32 (Property), RegisterSpaceType, _DATA32 (Address), _DATA32 (AndMask), _DATA32 (OrMask)

/// Read Modify Write register entry
typedef struct {
  UINT8           EntryType;      ///< Entry type
  UINT32          Property;       ///< Property
  GNB_RMW_BLOCK   Data;           ///< Data
} GNB_TABLE_ENTRY_PROPERTY_RMW;

#define GNB_ENTRY_REV_RMW(Rev,  RegisterSpaceType, Address, AndMask, OrMask) \
  GnbEntryRevRmw, _DATA64 (Rev), RegisterSpaceType, _DATA32 (Address), _DATA32 (AndMask), _DATA32 (OrMask)

/// Read Modify Write register entry
typedef struct {
  UINT8           EntryType;      ///< Entry type
  UINT64          Revision;       ///< revision
  GNB_RMW_BLOCK   Data;           ///< Data
} GNB_TABLE_ENTRY_REV_RMW;

#define GNB_ENTRY_FULL_RMW(Property, Revision, RegisterSpaceType, Address, AndMask, OrMask) \
  GnbEntryFullRmw, _DATA32 (Property), _DATA64 (Revision), RegisterSpaceType, _DATA32 (Address), _DATA32 (AndMask), _DATA32 (OrMask)

/// Read Modify Write register entry
typedef struct {
  UINT8           EntryType;      ///< Entry type
  UINT32          Property;       ///< Property
  UINT64          Revision;       ///< Revision
  GNB_RMW_BLOCK   Data;           ///< Data
} GNB_TABLE_ENTRY_FULL_RMW;

#define GNB_ENTRY_POLL(RegisterSpaceType, Address, AndMask, CompareValue) \
  GnbEntryPoll, RegisterSpaceType, _DATA32 (Address), _DATA32 (AndMask), _DATA32 (CompareValue)
/// Poll register entry
typedef struct {
  UINT8   EntryType;              ///< Entry type
  UINT8   RegisterSpaceType;      ///< Register space
  UINT32  Address;                ///< Register address
  UINT32  AndMask;                ///< End mask
  UINT32  CompareValue;           ///< Compare value
} GNB_TABLE_ENTRY_POLL;

#define GNB_ENTRY_PROPERTY_POLL(Property, RegisterSpaceType, Address, AndMask, CompareValue) \
  GnbEntryPropertyPoll, _DATA32 (Property), RegisterSpaceType, _DATA32 (Address), _DATA32 (AndMask), _DATA32 (CompareValue)
/// Poll register entry
typedef struct {
  UINT8   EntryType;              ///< Entry type
  UINT32  Property;               ///< Property
  UINT8   RegisterSpaceType;      ///< Register space
  UINT32  Address;                ///< Register address
  UINT32  AndMask;                ///< End mask
  UINT32  CompareValue;           ///< Compare value
} GNB_TABLE_ENTRY_PROPERTY_POLL;

#define GNB_ENTRY_FULL_POLL(Property, Revision, RegisterSpaceType, Address, AndMask, CompareValue) \
  GnbEntryFullPoll, _DATA32 (Property), _DATA64 (Revision), RegisterSpaceType, _DATA32 (Address), _DATA32 (AndMask), _DATA32 (CompareValue)
/// Poll register entry
typedef struct {
  UINT8   EntryType;              ///< Entry type
  UINT32  Property;               ///< Property
  UINT64  Revision;               ///< Revision
  UINT8   RegisterSpaceType;      ///< Register space
  UINT32  Address;                ///< Register address
  UINT32  AndMask;                ///< End mask
  UINT32  CompareValue;           ///< Compare value
} GNB_TABLE_ENTRY_FULL_POLL;

#define GNB_ENTRY_COPY(DestRegSpaceType, DestAddress, DestFieldOffset, DestFieldWidth, SrcRegisterSpaceType, SrcAddress, SrcFieldOffset, SrcFieldWidth) \
  GnbEntryCopy, DestRegSpaceType, _DATA32 (DestAddress), DestFieldOffset, DestFieldWidth, SrcRegisterSpaceType, _DATA32 (SrcAddress), SrcFieldOffset, SrcFieldWidth

/// Copy regster entry
typedef struct {
  UINT8   EntryType;              ///< Entry type
  UINT8   DestRegisterSpaceType;  ///< Register space
  UINT32  DestAddress;            ///< Register address
  UINT8   DestFieldOffset;        ///< Field Offset
  UINT8   DestFieldWidth;         ///< Field Width
  UINT8   SrcRegisterSpaceType;   ///< Register space
  UINT32  SrcAddress;             ///< Register address
  UINT8   SrcFieldOffset;         ///< Field Offset
  UINT8   SrcFieldWidth;          ///< Field Width
} GNB_TABLE_ENTRY_COPY;

#define GNB_ENTRY_STALL(Microsecond) \
  GnbEntryStall, _DATA32 (Microsecond)

/// Write register entry
typedef struct {
  UINT8   EntryType;               ///< Entry type
  UINT32  Microsecond;             ///< Value
} GNB_TABLE_ENTRY_STALL;

#define GNB_ENTRY_TERMINATE GnbEntryTerminate

AGESA_STATUS
GnbProcessTable (
  IN      GNB_HANDLE            *GnbHandle,
  IN      GNB_TABLE             *Table,
  IN      UINT32                Property,
  IN      UINT32                Flags,
  IN      AMD_CONFIG_PARAMS     *StdHeader
  );

#pragma pack (pop)

#endif
