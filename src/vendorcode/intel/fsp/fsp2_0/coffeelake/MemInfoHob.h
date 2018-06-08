/** @file
  This file contains definitions required for creation of
  Memory S3 Save data, Memory Info data and Memory Platform
  data hobs.


 Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>

 This program and the accompanying materials are licensed and made available under
 the terms and conditions of the BSD License which accompanies this distribution.
 The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _MEM_INFO_HOB_H_
#define _MEM_INFO_HOB_H_

#include <Uefi/UefiMultiPhase.h>
#include <Pi/PiBootMode.h>
#include <Pi/PiHob.h>

#pragma pack (push, 1)

extern EFI_GUID gSiMemoryS3DataGuid;
extern EFI_GUID gSiMemoryInfoDataGuid;
extern EFI_GUID gSiMemoryPlatformDataGuid;

#define MAX_NODE        1
#define MAX_CH          2
#define MAX_DIMM        2

///
/// Host reset states from MRC.
///
#define  WARM_BOOT        2

#define R_MC_CHNL_RANK_PRESENT  0x7C
#define   B_RANK0_PRS           BIT0
#define   B_RANK1_PRS           BIT1
#define   B_RANK2_PRS           BIT4
#define   B_RANK3_PRS           BIT5

///
/// Defines taken from MRC so avoid having to include MrcInterface.h
///

//
// Matches MAX_SPD_SAVE define in MRC
//
#ifndef MAX_SPD_SAVE
#define MAX_SPD_SAVE 29
#endif

//
// MRC version description.
//
typedef struct {
  UINT8  Major;     ///< Major version number
  UINT8  Minor;     ///< Minor version number
  UINT8  Rev;       ///< Revision number
  UINT8  Build;     ///< Build number
} SiMrcVersion;

//
// Matches MrcChannelSts enum in MRC
//
#ifndef CHANNEL_NOT_PRESENT
#define CHANNEL_NOT_PRESENT     0  // There is no channel present on the controller.
#endif
#ifndef CHANNEL_DISABLED
#define CHANNEL_DISABLED     1  // There is a channel present but it is disabled.
#endif
#ifndef CHANNEL_PRESENT
#define CHANNEL_PRESENT     2  // There is a channel present and it is enabled.
#endif

//
// Matches MrcDimmSts enum in MRC
//
#ifndef DIMM_ENABLED
#define DIMM_ENABLED     0  // DIMM/rank Pair is enabled, presence will be detected.
#endif
#ifndef DIMM_DISABLED
#define DIMM_DISABLED    1  // DIMM/rank Pair is disabled, regardless of presence.
#endif
#ifndef DIMM_PRESENT
#define DIMM_PRESENT     2  // There is a DIMM present in the slot/rank pair and it will be used.
#endif
#ifndef DIMM_NOT_PRESENT
#define DIMM_NOT_PRESENT 3  // There is no DIMM present in the slot/rank pair.
#endif

//
// Matches MrcBootMode enum in MRC
//
#ifndef bmCold
#define bmCold 0            // Cold boot
#endif
#ifndef bmWarm
#define bmWarm 1            // Warm boot
#endif
#ifndef bmS3
#define bmS3   2            // S3 resume
#endif
#ifndef bmFast
#define bmFast 3            // Fast boot
#endif

//
// Matches MrcDdrType enum in MRC
//
#ifndef MRC_DDR_TYPE_DDR4
#define MRC_DDR_TYPE_DDR4     0
#endif
#ifndef MRC_DDR_TYPE_DDR3
#define MRC_DDR_TYPE_DDR3     1
#endif
#ifndef MRC_DDR_TYPE_LPDDR3
#define MRC_DDR_TYPE_LPDDR3   2
#endif
#ifndef CPU_CFL//CNL
#ifndef MRC_DDR_TYPE_LPDDR4
#define MRC_DDR_TYPE_LPDDR4   3
#endif
#else//CFL
#ifndef MRC_DDR_TYPE_UNKNOWN
#define MRC_DDR_TYPE_UNKNOWN  3
#endif
#endif//CPU_CFL-endif

#define MAX_PROFILE_NUM     4 // number of memory profiles supported
#define MAX_XMP_PROFILE_NUM 2 // number of XMP profiles supported

//
// DIMM timings
//
typedef struct {
  UINT32 tCK;       ///< Memory cycle time, in femtoseconds.
  UINT16 NMode;     ///< Number of tCK cycles for the channel DIMM's command rate mode.
  UINT16 tCL;       ///< Number of tCK cycles for the channel DIMM's CAS latency.
  UINT16 tCWL;      ///< Number of tCK cycles for the channel DIMM's minimum CAS write latency time.
  UINT16 tFAW;      ///< Number of tCK cycles for the channel DIMM's minimum four activate window delay time.
  UINT16 tRAS;      ///< Number of tCK cycles for the channel DIMM's minimum active to precharge delay time.
  UINT16 tRCDtRP;   ///< Number of tCK cycles for the channel DIMM's minimum RAS# to CAS# delay time and Row Precharge delay time.
  UINT16 tREFI;     ///< Number of tCK cycles for the channel DIMM's minimum Average Periodic Refresh Interval.
  UINT16 tRFC;      ///< Number of tCK cycles for the channel DIMM's minimum refresh recovery delay time.
  UINT16 tRFCpb;    ///< Number of tCK cycles for the channel DIMM's minimum per bank refresh recovery delay time.
  UINT16 tRFC2;     ///< Number of tCK cycles for the channel DIMM's minimum refresh recovery delay time.
  UINT16 tRFC4;     ///< Number of tCK cycles for the channel DIMM's minimum refresh recovery delay time.
  UINT16 tRPab;     ///< Number of tCK cycles for the channel DIMM's minimum row precharge delay time for all banks.
  UINT16 tRRD;      ///< Number of tCK cycles for the channel DIMM's minimum row active to row active delay time.
  UINT16 tRRD_L;    ///< Number of tCK cycles for the channel DIMM's minimum row active to row active delay time for same bank groups.
  UINT16 tRRD_S;    ///< Number of tCK cycles for the channel DIMM's minimum row active to row active delay time for different bank groups.
  UINT16 tRTP;      ///< Number of tCK cycles for the channel DIMM's minimum internal read to precharge command delay time.
  UINT16 tWR;       ///< Number of tCK cycles for the channel DIMM's minimum write recovery time.
  UINT16 tWTR;      ///< Number of tCK cycles for the channel DIMM's minimum internal write to read command delay time.
  UINT16 tWTR_L;    ///< Number of tCK cycles for the channel DIMM's minimum internal write to read command delay time for same bank groups.
  UINT16 tWTR_S;    ///< Number of tCK cycles for the channel DIMM's minimum internal write to read command delay time for different bank groups.
  UINT16 tCCD_L;  ///< Number of tCK cycles for the channel DIMM's minimum CAS-to-CAS delay for same bank group.
} MRC_CH_TIMING;

typedef struct {
  UINT8 SG;         ///< Number of tCK cycles between transactions in the same bank group.
  UINT8 DG;         ///< Number of tCK cycles between transactions when switching bank groups.
  UINT8 DR;         ///< Number of tCK cycles between transactions when switching between Ranks (in the same DIMM).
  UINT8 DD;         ///< Number of tCK cycles between transactions when switching between DIMMs.
} MRC_TA_TIMING;

///
/// Memory SMBIOS & OC Memory Data Hob
///
typedef struct {
  UINT8            Status;                  ///< See MrcDimmStatus for the definition of this field.
  UINT8            DimmId;
  UINT32           DimmCapacity;            ///< DIMM size in MBytes.
  UINT16           MfgId;
  UINT8            ModulePartNum[20];       ///< Module part number for DDR3 is 18 bytes however for DRR4 20 bytes as per JEDEC Spec, so reserving 20 bytes
  UINT8            RankInDimm;              ///< The number of ranks in this DIMM.
  UINT8            SpdDramDeviceType;       ///< Save SPD DramDeviceType information needed for SMBIOS structure creation.
  UINT8            SpdModuleType;           ///< Save SPD ModuleType information needed for SMBIOS structure creation.
  UINT8            SpdModuleMemoryBusWidth; ///< Save SPD ModuleMemoryBusWidth information needed for SMBIOS structure creation.
  UINT8            SpdSave[MAX_SPD_SAVE];   ///< Save SPD Manufacturing information needed for SMBIOS structure creation.
} DIMM_INFO;

typedef struct {
  UINT8            Status;                  ///< Indicates whether this channel should be used.
  UINT8            ChannelId;
  UINT8            DimmCount;               ///< Number of valid DIMMs that exist in the channel.
  MRC_CH_TIMING    Timing[MAX_PROFILE_NUM]; ///< The channel timing values.
  DIMM_INFO        DimmInfo[MAX_DIMM];      ///< Save the DIMM output characteristics.
} CHANNEL_INFO;

typedef struct {
  UINT8             Status;                  ///< Indicates whether this controller should be used.
  UINT16            DeviceId;                ///< The PCI device id of this memory controller.
  UINT8             RevisionId;              ///< The PCI revision id of this memory controller.
  UINT8             ChannelCount;            ///< Number of valid channels that exist on the controller.
  CHANNEL_INFO      ChannelInfo[MAX_CH];     ///< The following are channel level definitions.
  MRC_TA_TIMING     tRd2Rd;                  ///< Read-to-Read   Turn Around Timings
  MRC_TA_TIMING     tRd2Wr;                  ///< Read-to-Write  Turn Around Timings
  MRC_TA_TIMING     tWr2Rd;                  ///< Write-to-Read  Turn Around Timings
  MRC_TA_TIMING     tWr2Wr;                  ///< Write-to-Write Turn Around Timings
} CONTROLLER_INFO;

typedef struct {
  UINT8             Revision;
  UINT16            DataWidth;              ///< Data width, in bits, of this memory device
  /** As defined in SMBIOS 3.0 spec
    Section 7.18.2 and Table 75
  **/
  UINT8             MemoryType;             ///< DDR type: DDR3, DDR4, or LPDDR3
  UINT16            MaximumMemoryClockSpeed;///< The maximum capable speed of the device, in megahertz (MHz)
  UINT16            ConfiguredMemoryClockSpeed; ///< The configured clock speed to the memory device, in megahertz (MHz)
  /** As defined in SMBIOS 3.0 spec
    Section 7.17.3 and Table 72
  **/
  UINT8             ErrorCorrectionType;

  SiMrcVersion      Version;
  BOOLEAN           EccSupport;
  UINT8             MemoryProfile;
  UINT32            TotalPhysicalMemorySize;
  UINT32            DefaultXmptCK[MAX_XMP_PROFILE_NUM];///< Stores the tCK value read from SPD XMP profiles if they exist.
  UINT8             XmpProfileEnable;                  ///< If XMP capable DIMMs are detected, this will indicate which XMP Profiles are common among all DIMMs.
  UINT8             Ratio;
  UINT8             RefClk;
  UINT32            VddVoltage[MAX_PROFILE_NUM];
  CONTROLLER_INFO   Controller[MAX_NODE];
} MEMORY_INFO_DATA_HOB;

/**
  Memory Platform Data Hob

  <b>Revision 1:</b>
  - Initial version.
  <b>Revision 2:</b>
  - Added TsegBase, PrmrrSize, PrmrrBase, Gttbase, MmioSize, PciEBaseAddress fields
**/
typedef struct {
  UINT8             Revision;
  UINT8             Reserved[3];
  UINT32            BootMode;
  UINT32            TsegSize;
  UINT32            TsegBase;
  UINT32            PrmrrSize;
  UINT32            PrmrrBase;
  UINT32            GttBase;
  UINT32            MmioSize;
  UINT32            PciEBaseAddress;
#ifdef CPU_CFL
  UINT32            GdxcIotBase;
  UINT32            GdxcIotSize;
  UINT32            GdxcMotBase;
  UINT32            GdxcMotSize;
#endif //CPU_CFL
} MEMORY_PLATFORM_DATA;

typedef struct {
  EFI_HOB_GUID_TYPE    EfiHobGuidType;
  MEMORY_PLATFORM_DATA Data;
  UINT8                *Buffer;
} MEMORY_PLATFORM_DATA_HOB;

#pragma pack (pop)

#endif // _MEM_INFO_HOB_H_
