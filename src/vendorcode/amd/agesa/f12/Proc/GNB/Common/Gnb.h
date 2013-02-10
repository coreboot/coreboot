/* $NoKeywords:$ */
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
 * @e \$Revision: 47656 $   @e \$Date: 2011-02-25 02:39:38 +0800 (Fri, 25 Feb 2011) $
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
#ifndef _GNB_H_
#define _GNB_H_

#pragma pack (push, 1)

#define GNB_DEADLOOP() \
{ \
  VOLATILE BOOLEAN k; \
  k = TRUE; \
  while (k) { \
  } \
}
#ifdef IDSOPT_TRACING_ENABLED
  #if (IDSOPT_TRACING_ENABLED == TRUE)
    #define GNB_TRACE_ENABLE
  #endif
#endif


#ifndef GNB_DEBUG_CODE
  #ifdef GNB_TRACE_ENABLE
    #define  GNB_DEBUG_CODE(Code) Code
  #else
    #define  GNB_DEBUG_CODE(Code)
  #endif
#endif

#ifndef MIN
  #define MIN(x, y) (((x) > (y))? (y):(x))
#endif

#ifndef MAX
  #define MAX(x, y) (((x) > (y))? (x):(y))
#endif

#define OFF 0

#define PVOID UINT64

#define GnbLibGetHeader(x)  ((AMD_CONFIG_PARAMS*) (intptr_t) (x)->StdHeader)

#define AGESA_STATUS_UPDATE(Current, Aggregated) \
if (Current > Aggregated) { \
  Aggregated = Current; \
}

#ifndef offsetof
  #define offsetof(s, m) (UINTN)&(((s *)0)->m)
#endif


//Table properties

#define TABLE_PROPERTY_DEAFULT                     0x00000000
#define TABLE_PROPERTY_IGFX_DISABLED               0x00000001
#define TABLE_PROPERTY_IOMMU_DISABLED              0x00000002
#define TABLE_PROPERTY_LCLK_DEEP_SLEEP             0x00000004
#define TABLE_PROPERTY_ORB_CLK_GATING              0x00000008
#define TABLE_PROPERTY_IOC_LCLK_CLOCK_GATING       0x00000010
#define TABLE_PROPERTY_IOC_SCLK_CLOCK_GATING       0x00000020

#define TABLE_PROPERTY_IOMMU_L1_CLOCK_GATING 0x00000040
#define TABLE_PROPERTY_IOMMU_L2_CLOCK_GATING 0x00000080

//Register access flags Flags
#define GNB_REG_ACC_FLAG_S3SAVE             0x00000001

/// LCLK DPM enable control
typedef enum {
  LclkDpmDisabled,          ///<LCLK DPM disabled
  LclkDpmRcActivity,        ///<LCLK DPM enabled and use Root Complex Activity monitor method
} LCLK_DPM_MODE;

/// IVRS block
typedef enum {
  IvrsIvhdBlock               = 0x10,       ///< IVHD block
  IvrsIvmdBlock               = 0x20,       ///< IVMD block
  IvrsIvmdrBlock              = 0x30,       ///< IVMDR block
  IvrsIvhdrBlock              = 0x40        ///< IVHDR block
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

#define IVINFO_HTATSRESV_MASK       0x00400000
#define IVINFO_VASIZE_MASK          0x003F8000
#define IVINFO_PASIZE_MASK          0x00007F00
#define IVINFO_GASIZE_MASK          0x000000E0

#define IVHD_INFO_MSINUM_OFFSET     0
#define IVHD_INFO_UNITID_OFFSET     8

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

/// Power gaiter data setting (do not change this structure definition)
typedef struct {
  UINT16  MothPsoPwrup;         ///< Mother Timer Powerup
  UINT16  MothPsoPwrdn;         ///< Mother Timer Powerdown
  UINT16  DaugPsoPwrup;         ///< Daughter Timer Powerup
  UINT16  DaugPsoPwrdn;         ///< Daughter Timer Powerdown
  UINT16  ResetTimer;           ///< Reset Timer
  UINT16  IsoTimer;             ///< Isolation Timer
} POWER_GATE_DATA;


/// Topology information
typedef struct {
  BOOLEAN   PhantomFunction;    ///< PCIe topology have device with phantom function
  BOOLEAN   PcieToPciexBridge;  ///< PCIe topology have device with Pcieto Pcix bridge
} GNB_TOPOLOGY_INFO;


#define GNB_STRINGIZE(x)                  #x
#define GNB_SERVICE_DEFINITIONS(x)        GNB_STRINGIZE (Services/x/x.h)
#define GNB_MODULE_DEFINITIONS(x)         GNB_STRINGIZE (Modules/x/x.h)
#define GNB_MODULE_INSTALL(x)             GNB_STRINGIZE (Modules/x/x##Install.h)

/// GNB installable services
typedef enum {
  GnbPcieFamConfigService,                ///< PCIe config service
  GnbPcieFamInitService,                  ///< PCIe Init service
  GnbPcieFamDebugService,                 ///< PCIe Debug service
} GNB_SERVICE_ID;

/// GNB service entry
typedef struct _GNB_SERVICE {
  GNB_SERVICE_ID        ServiceId;        ///< Service ID
  UINT64                Family;           ///< CPU family
  VOID                  *ServiceProtocol; ///< Service protocol
  struct  _GNB_SERVICE  *NextService;     ///< Pointer to next service
} GNB_SERVICE;


#pragma pack (pop)

#endif
