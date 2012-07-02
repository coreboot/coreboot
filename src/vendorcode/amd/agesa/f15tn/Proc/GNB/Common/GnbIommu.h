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
