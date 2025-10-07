/** @file
  This file contains definitions required for creation of
  Memory S3 Save data, Memory Info data and Memory Platform
  data hobs.

  @copyright
  Copyright (c) 1999 - 2024, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials are licensed and made available under
  the terms and conditions of the BSD License that accompanies this distribution.
  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.
  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,

  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

@par Specification Reference:
**/
#ifndef _MEM_INFO_HOB_H_
#define _MEM_INFO_HOB_H_


#pragma pack (push, 1)

extern EFI_GUID gSiMemoryS3DataGuid;
extern EFI_GUID gSiMemoryS3Data2Guid;
extern EFI_GUID gSiMemoryInfoDataGuid;
extern EFI_GUID gSiMemoryPlatformDataGuid;

#define MAX_NODE        2
#define MAX_CH          4
#define MAX_DDR5_CH     2
#define MAX_DIMM        2

// Must be same or higher than the corresponding definitions in MrcGlobalDefinitions.h
#define _MAX_RANK_IN_CHANNEL       (4)       ///< The maximum number of ranks per channel.
#define _MAX_SDRAM_IN_DIMM         (5)       ///< The maximum number of SDRAMs per DIMM.

// Must match the corresponding definition in CMrcExtTypes.h
#define PPR_REQUEST_MAX           (2)

// Must match definitions in
// Intel\OneSiliconPkg\IpBlock\MemoryInit\Mtl\Include\MrcInterface.h
#define HOB_MAX_SAGV_POINTS 4

///
/// Host reset states from MRC.
///
#define  WARM_BOOT        2

#define R_MC_CHNL_RANK_PRESENT  0x7C
#define   B_RANK0_PRS           BIT0
#define   B_RANK1_PRS           BIT1
#define   B_RANK2_PRS           BIT4
#define   B_RANK3_PRS           BIT5

// @todo remove and use the MdePkg\Include\Pi\PiHob.h
#if !defined(_PEI_HOB_H_) && !defined(__PI_HOB_H__)
#ifndef __HOB__H__
typedef struct _EFI_HOB_GENERIC_HEADER {
  UINT16  HobType;
  UINT16  HobLength;
  UINT32  Reserved;
} EFI_HOB_GENERIC_HEADER;

typedef struct _EFI_HOB_GUID_TYPE {
  EFI_HOB_GENERIC_HEADER  Header;
  EFI_GUID                Name;
  ///
  /// Guid specific data goes here
  ///
} EFI_HOB_GUID_TYPE;
#endif
#endif

///
/// Defines taken from MRC so avoid having to include MrcInterface.h
///

//
// Matches MAX_SPD_SAVE define in MRC
//
#ifndef MAX_SPD_SAVE
#define MAX_SPD_SAVE 39
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
#define CHANNEL_DISABLED        1  // There is a channel present but it is disabled.
#endif
#ifndef CHANNEL_PRESENT
#define CHANNEL_PRESENT         2  // There is a channel present and it is enabled.
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
#ifndef __MRC_BOOT_MODE__
#define __MRC_BOOT_MODE__                 //The below values are originated from MrcCommonTypes.h
  #ifndef INT32_MAX
  #define INT32_MAX                       (0x7FFFFFFF)
  #endif  //INT32_MAX
typedef enum {
  bmCold,                                 ///< Cold boot
  bmWarm,                                 ///< Warm boot
  bmS3,                                   ///< S3 resume
  bmFast,                                 ///< Fast boot
  MrcBootModeMax,                         ///< MRC_BOOT_MODE enumeration maximum value.
  MrcBootModeDelim = INT32_MAX            ///< This value ensures the enum size is consistent on both sides of the PPI.
} MRC_BOOT_MODE;
#endif  //__MRC_BOOT_MODE__

//
// Matches MrcDdrType enum in MRC
//
#ifndef MRC_DDR_TYPE_LPDDR5
#define MRC_DDR_TYPE_LPDDR5   0
#endif
#ifndef MRC_DDR_TYPE_DDR5
#define MRC_DDR_TYPE_DDR5     1
#endif
#ifndef MRC_DDR_TYPE_UNKNOWN
#define MRC_DDR_TYPE_UNKNOWN  2
#endif

#define MAX_PROFILE_NUM       7 // number of memory profiles supported
#define MAX_XMP_PROFILE_NUM   5 // number of XMP profiles supported

#ifndef MAX_RCOMP_TARGETS
#define MAX_RCOMP_TARGETS     5
#endif

#ifndef MAX_ODT_ENTRIES
#define MAX_ODT_ENTRIES       11
#endif

#ifndef MAX_COPY_DIMM_DFE_TAPS
#define MAX_COPY_DIMM_DFE_TAPS      2
#endif

#define MAX_TRACE_REGION              5
#define MAX_TRACE_CACHE_TYPE          2

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
  UINT32 tREFI;     ///< Number of tCK cycles for the channel DIMM's minimum Average Periodic Refresh Interval.
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
  UINT16 tCCD_L;    ///< Number of tCK cycles for the channel DIMM's minimum CAS-to-CAS delay for same bank group.
  UINT16 tCCD_L_WR; ///< Number of tCK cycles for the channel DIMM's minimum Write-to-Write delay for same bank group.
  UINT8  Resv[2];   ///< Reserved.
} MRC_CH_TIMING;

typedef struct {
  UINT16 tRDPRE;     ///< Read CAS to Precharge cmd delay
} MRC_IP_TIMING;

typedef union {
  struct {
    UINT16 ContinuationCount                   :  7; ///< Bits 6:0
    UINT16 ContinuationParity                  :  1; ///< Bits 7:7
    UINT16 LastNonZeroByte                     :  8; ///< Bits 15:8
  } Bits;
  UINT16 Data;
  UINT8  Data8[2];
} HOB_MANUFACTURER_ID_CODE;

///
/// Memory SMBIOS & OC Memory Data Hob
///
typedef struct {
  UINT8                     Status;                  ///< See MrcDimmStatus for the definition of this field.
  UINT8                     DimmId;
  UINT32                    DimmCapacity;            ///< DIMM size in MBytes.
  HOB_MANUFACTURER_ID_CODE  MfgId;                   ///< Dram module manufacturer ID
  HOB_MANUFACTURER_ID_CODE  CkdMfgID;                ///< Clock Driver (CKD) Manufacturer ID
  UINT8                     CkdDeviceRev;            ///< Clock Driver (CKD) device revision
  HOB_MANUFACTURER_ID_CODE  DramMfgID;               ///< Manufacturer ID code for DRAM chip on the module
  UINT8                     ModulePartNum[30];       ///< Module part number in ASCII
  UINT8                     RankInDimm;              ///< The number of ranks in this DIMM.
  UINT8                     SpdDramDeviceType;       ///< Save SPD DramDeviceType information needed for SMBIOS structure creation.
  UINT8                     SpdModuleType;           ///< Save SPD ModuleType information needed for SMBIOS structure creation.
  UINT8                     SpdSave[MAX_SPD_SAVE];   ///< Save SPD Manufacturing information needed for SMBIOS structure creation.
  UINT16                    Speed;                   ///< The maximum capable speed of the device, in MHz
  UINT8                     MdSocket;                ///< MdSocket: 0 = Memory Down, 1 = Socketed. Needed for SMBIOS structure creation.
  UINT8                     Banks;                   ///< Number of banks the DIMM contains.
  UINT8                     BankGroups;              ///< Number of bank groups the DIMM contains.
  UINT8                     DeviceDensity;           ///< Device Density in Gb
  UINT32                    SerialNumber;            ///< DIMM Serial Number
  UINT8                     TotalWidth;              ///< Total Data width in bits
  UINT8                     DataWidth;               ///< Primary bus width in bits
} DIMM_INFO;

typedef struct {
  UINT8            Status;                  ///< Indicates whether this channel should be used.
  UINT8            ChannelId;
  UINT8            DimmCount;               ///< Number of valid DIMMs that exist in the channel.
  MRC_CH_TIMING    Timing[MAX_PROFILE_NUM]; ///< The channel timing values.
  DIMM_INFO        DimmInfo[MAX_DIMM];      ///< Save the DIMM output characteristics.
} CHANNEL_INFO;

typedef struct {
  UINT8            Status;                  ///< Indicates whether this controller should be used.
  UINT16           DeviceId;                ///< The PCI device id of this memory controller.
  UINT8            RevisionId;              ///< The PCI revision id of this memory controller.
  UINT8            ChannelCount;            ///< Number of valid channels that exist on the controller.
  CHANNEL_INFO     ChannelInfo[MAX_CH];     ///< The following are channel level definitions.
} CONTROLLER_INFO;

//
// Each DIMM Slot Mechanical present bit map
//
typedef struct {
  UINT8 MrcSlotMap[MAX_NODE][MAX_CH];
} MRC_SLOTMAP;

typedef struct {
  UINT64   BaseAddress;   ///< Trace Base Address
  UINT64   TotalSize;     ///< Total Trace Region of Same Cache type
  UINT8    CacheType;     ///< Trace Cache Type
  UINT8    ErrorCode;     ///< Trace Region Allocation Fail Error code
  UINT8    Rsvd[2];
} PSMI_MEM_INFO;

/// This data structure contains per-SaGv timing values that are considered output by the MRC.
typedef struct {
  UINT32        DataRate;    ///< The memory rate for the current SaGv Point in units of MT/s
  MRC_CH_TIMING JedecTiming; ///< Timings used for this entry's corresponding SaGv Point - derived from JEDEC SPD spec
  MRC_IP_TIMING IpTiming;    ///< Timings used for this entry's corresponding SaGv Point - IP specific
  UINT16        MaxMemoryBandwidth; ///< Maximum theoretical bandwidth in GB/s supported by GV
} HOB_SAGV_TIMING_OUT;

/// This data structure contains SAGV config values that are considered output by the MRC.
typedef struct {
  UINT32              NumSaGvPointsEnabled; ///< Count of the total number of SAGV Points enabled.
  UINT32              SaGvPointMask;        ///< Bit mask where each bit indicates an enabled SAGV point.
  HOB_SAGV_TIMING_OUT SaGvTiming[HOB_MAX_SAGV_POINTS];
} HOB_SAGV_INFO;

typedef struct _PPR_RESULT_COLUMNS_HOB {
  UINT8  PprRowRepairsSuccessful;
  UINT8  Controller;
  UINT8  Channel;
  UINT8  Rank;
  UINT8  BankGroup;
  UINT8  Bank;
  UINT32 Row;
  UINT8  Device;
} PPR_RESULT_COLUMNS_HOB;

/**
  Memory Info Data Hob

  <b>Revision 1:</b>
  - Initial version. (from MTL)
  <b>Revision 2:</b>
  - Added MopPackages, MopDensity, MopRanks, MopVendor fields
  <b>Revision 3:</b>
  - Added MaxRankCapacity
  - Removed DataWidth
  - DIMM_INFO: increased ModulePartNum from 20 to 30 chars
  - DIMM_INFO: Added SerialNumber, TotalWidth and DataWidth
  - DIMM_INFO: Removed SpdModuleMemoryBusWidth
  - MFG ID fields: use HOB_MANUFACTURER_ID_CODE instead of UINT16 for easier parsing

**/
typedef struct {
  UINT8             Revision;
  /** As defined in SMBIOS 3.0 spec
    Section 7.18.2 and Table 75
  **/
  UINT8             MemoryType;                 ///< DDR type: DDR5 or LPDDR5, uses SMBIOS MEMORY_DEVICE_TYPE encoding
  UINT16            MaximumMemoryClockSpeed;    ///< The maximum capable speed of the device, in megahertz (MHz)
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
  UINT8             XmpConfigWarning;                  ///< If XMP capable DIMMs config support only 1DPC, but 2DPC is installed
  BOOLEAN           DynamicMemoryBoostTrainingFailed;  ///< TRUE if Dynamic Memory Boost failed to train and was force disabled on the last full training boot. FALSE otherwise.
  UINT16            Ratio;                             ///< DDR Frequency Ratio, used for programs that require ratios higher then 255
  UINT8             RefClk;
  UINT32            VddVoltage[MAX_PROFILE_NUM];
  UINT32            VddqVoltage[MAX_PROFILE_NUM];
  UINT32            VppVoltage[MAX_PROFILE_NUM];
  UINT16            RcompTarget[MAX_PROFILE_NUM][MAX_RCOMP_TARGETS];
  UINT16            DimmOdt[MAX_PROFILE_NUM][MAX_DIMM][MAX_ODT_ENTRIES];
  INT8              DimmDFE[MAX_PROFILE_NUM][MAX_DDR5_CH][MAX_DIMM][MAX_COPY_DIMM_DFE_TAPS];
  CONTROLLER_INFO   Controller[MAX_NODE];
  UINT32            NumPopulatedChannels;              ///< Total number of memory channels populated
  HOB_SAGV_INFO     SagvConfigInfo;                    ///< This data structure contains SAGV config values that are considered output by the MRC.
  BOOLEAN           IsIbeccEnabled;
  UINT16            TotalMemWidth;                     ///< Total Memory Width in bits from all populated channels
  UINT8             MopPackages;                       ///< Mop DRAM package population
  UINT8             MopDensity;                        ///< Mop DRAM die density
  UINT8             MopRanks;                          ///< Mop Number of ranks
  UINT8             MopVendor;                         ///< Mop DRAM vendor ID
  UINT8             PprRanInLastBoot;                  ///< Whether PPR ran in the prior boot
  UINT16            PprDetectedErrors;                 ///< PPR: Counts of detected bad rows
  UINT16            PprRepairFails;                    ///< PPR: Counts of repair failure
  UINT16            PprForceRepairStatus;              ///< PPR: Force Repair Status
  UINT16            PprRepairsSuccessful;              ///< PPR: Counts of repair successes
  PPR_RESULT_COLUMNS_HOB PprErrorInfo;                 ///< PPR: Error location
  UINT8             PprAvailableResources[MAX_NODE][MAX_CH][_MAX_RANK_IN_CHANNEL][_MAX_SDRAM_IN_DIMM]; ///< PPR available resources per device
  BOOLEAN           MixedEccDimms;                     ///< TRUE if both ECC and nonECC Dimms were detected in the system
  UINT8             MaxRankCapacity;                   ///< Maximum possible rank capacity in [GB]
  UINT16            PprFailingChannelBitMask;          ///< PPR failing channel mask
  BOOLEAN           PprTargetedStatus[PPR_REQUEST_MAX]; ///< PPR status of each Targeted PPR request (0 = Targeted PPR was successful, 1 = PPR failed)
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
  UINT64            PrmrrBase;
  UINT32            GttBase;
  UINT32            MmioSize;
  UINT32            PciEBaseAddress;
  PSMI_MEM_INFO     PsmiInfo[MAX_TRACE_CACHE_TYPE];
  PSMI_MEM_INFO     PsmiRegionInfo[MAX_TRACE_REGION];
  BOOLEAN           MrcBasicMemoryTestPass;
} MEMORY_PLATFORM_DATA;

typedef struct {
  EFI_HOB_GUID_TYPE    EfiHobGuidType;
  MEMORY_PLATFORM_DATA Data;
  UINT8                *Buffer;
} MEMORY_PLATFORM_DATA_HOB;

#define EFI_RMT_OS_VARIABLE_NAME        L"Rmt"
#define RMT_ENABLE  1
#define RMT_DISABLE 0

extern EFI_GUID gRmtVariableGuid;

//Structure of RMT UEFI variable which should be R/W by OS
//EnDsRmt - To enable Memory margining support

typedef struct {
  UINT8                EnDsRmt;
} RMT_VAR;

#pragma pack (pop)

#endif // _MEM_INFO_HOB_H_
