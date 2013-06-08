/**
 * @file
 *
 * Misc common definition
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
#ifndef _GNBIOMMU_H_
#define _GNBIOMMU_H_

#pragma pack (push, 1)


/// IVRS block
typedef enum {
  IvrsIvhdBlock               = 0x10,       ///< I/O Virtualization Hardware Definition Block
  IvrsIvmdBlock               = 0x20,       ///< I/O Virtualization Memory Definition Block for all peripherals
  IvrsIvmdBlockSingle         = 0x21,       ///< IVMD block for specified peripheral
  IvrsIvmdBlockRange          = 0x22,       ///< IVMD block for peripheral range
  IvrsIvhdrBlock              = 0x40,       ///< IVHDR (Relative) block
  IvrsIvmdrBlock              = 0x50,       ///< IVMDR (Relative) block for all peripherals
  IvrsIvmdrBlockSingle        = 0x51        ///< IVMDR block for last IVHDR
} IVRS_BLOCK_TYPE;

#define DEVICE_ID(PciAddress) (UINT16) (((PciAddress).Address.Bus << 8) | ((PciAddress).Address.Device << 3) | (PciAddress).Address.Function)

/// IVHD entry types
typedef enum {
  IvhdEntryPadding            =  0,         ///< Table padding
  IvhdEntrySelect             =  2,         ///< Select
  IvhdEntryStartRange         =  3,         ///< Start Range
  IvhdEntryEndRange           =  4,         ///< End Range
  IvhdEntryAliasSelect        =  66,        ///< Alias select
  IvhdEntryAliasStartRange    =  67,        ///< Alias start range
  IvhdEntryExtendedSelect     =  70,        ///< Extended select
  IvhdEntryExtendedStartRange =  71,        ///< Extended Start range
  IvhdEntrySpecialDevice      =  72         ///< Special device
} IVHD_ENTRY_TYPE;

/// Special device variety
typedef enum {
  IvhdSpecialDeviceIoapic     = 0x1,        ///< IOAPIC
  IvhdSpecialDeviceHpet       = 0x2         ///< HPET
} IVHD_SPECIAL_DEVICE;


#define IVHD_FLAG_COHERENT          BIT5
#define IVHD_FLAG_IOTLBSUP          BIT4
#define IVHD_FLAG_ISOC              BIT3
#define IVHD_FLAG_RESPASSPW         BIT2
#define IVHD_FLAG_PASSPW            BIT1
#define IVHD_FLAG_PPRSUB            BIT7
#define IVHD_FLAG_PREFSUP           BIT6

#define IVHD_EFR_XTSUP_OFFSET       0
#define IVHD_EFR_NXSUP_OFFSET       1
#define IVHD_EFR_GTSUP_OFFSET       2
#define IVHD_EFR_GLXSUP_OFFSET      3
#define IVHD_EFR_IASUP_OFFSET       5
#define IVHD_EFR_GASUP_OFFSET       6
#define IVHD_EFR_HESUP_OFFSET       7
#define IVHD_EFR_PASMAX_OFFSET      8
#define IVHD_EFR_PNCOUNTERS_OFFSET  13
#define IVHD_EFR_PNBANKS_OFFSET     17
#define IVHD_EFR_MSINUMPPR_OFFSET   23
#define IVHD_EFR_GATS_OFFSET        28
#define IVHD_EFR_HATS_OFFSET        30

#define IVINFO_HTATSRESV_MASK       0x00400000ul
#define IVINFO_VASIZE_MASK          0x003F8000ul
#define IVINFO_PASIZE_MASK          0x00007F00ul
#define IVINFO_GASIZE_MASK          0x000000E0ul

#define IVHD_INFO_MSINUM_OFFSET     0
#define IVHD_INFO_UNITID_OFFSET     8

#define IVMD_FLAG_EXCLUSION_RANGE   BIT3
#define IVMD_FLAG_IW                BIT2
#define IVMD_FLAG_IR                BIT1
#define IVMD_FLAG_UNITY             BIT0

/// IVRS header
typedef struct {
  UINT8   Sign[4];           ///< Signature
  UINT32  TableLength;       ///< Table Length
  UINT8   Revision;          ///< Revision
  UINT8   Checksum;          ///< Checksum
  UINT8   OemId[6];          ///< OEM ID
  UINT8   OemTableId[8];     ///< OEM Tabled ID
  UINT32  OemRev;            ///< OEM Revision
  UINT8   CreatorId[4];      ///< Creator ID
  UINT32  CreatorRev;        ///< Creator Revision
  UINT32  IvInfo;            ///< IvInfo
  UINT64  Reserved;          ///< Reserved
} IOMMU_IVRS_HEADER;

/// IVRS IVHD Entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT8   Flags;              ///< Flags
  UINT16  Length;             ///< Length
  UINT16  DeviceId;           ///< DeviceId
  UINT16  CapabilityOffset;   ///< CapabilityOffset
  UINT64  BaseAddress;        ///< BaseAddress
  UINT16  PciSegment;         ///< Pci segment
  UINT16  IommuInfo;          ///< IOMMU info
  UINT32  IommuEfr;           ///< reserved
} IVRS_IVHD_ENTRY;

/// IVHD generic entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT16  DeviceId;           ///< Device id
  UINT8   DataSetting;        ///< Data settings
} IVHD_GENERIC_ENTRY;

///IVHD alias entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT16  DeviceId;           ///< Device id
  UINT8   DataSetting;        ///< Data settings
  UINT8   Reserved;           ///< Reserved
  UINT16  AliasDeviceId;      ///< Alias device id
  UINT8   Reserved2;          ///< Reserved
} IVHD_ALIAS_ENTRY;

///IVHD extended entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT16  DeviceId;           ///< Device id
  UINT8   DataSetting;        ///< Data settings
  UINT32  ExtSetting;         ///< Extended settings
} IVHD_EXT_ENTRY;

/// IVHD special entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT16  Reserved;           ///< Reserved
  UINT8   DataSetting;        ///< Data settings
  UINT8   Handle;             ///< Handle
  UINT16  AliasDeviceId;      ///< Alis device id
  UINT8   Variety;            ///< Variety
} IVHD_SPECIAL_ENTRY;

/// IVRS IVMD Entry
typedef struct {
  UINT8   Type;               ///< Type
  UINT8   Flags;              ///< Flags
  UINT16  Length;             ///< Length
  UINT16  DeviceId;           ///< DeviceId
  UINT16  AuxiliaryData;      ///< Auxiliary data
  UINT64  Reserved;           ///< Reserved (0000_0000_0000_0000)
  UINT64  BlockStart;         ///< IVMD start address
  UINT64  BlockLength;        ///< IVMD memory block length
} IVRS_IVMD_ENTRY;

#pragma pack (pop)

#endif
