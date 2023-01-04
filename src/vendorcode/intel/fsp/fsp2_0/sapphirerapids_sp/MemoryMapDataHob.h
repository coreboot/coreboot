/** @file
  GUID used for Memory Map Data entries in the HOB list.

  @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2022 Intel Corporation. <BR>

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

#ifndef _MEMORY_MAP_DATA_HOB_H_
#define _MEMORY_MAP_DATA_HOB_H_

#define MEMORY_MAP_HOB_GUID \
  { \
    0xf8870015,0x6994,0x4b98,0x95,0xa2,0xbd,0x56,0xda,0x91,0xc0,0x7f \
  }

#ifndef MAX_SOCKET
#define MAX_SOCKET                       4
#endif

#ifndef MAX_IMC
#define MAX_IMC                          4                     // Maximum memory controllers per socket
#endif

#ifndef MAX_MC_CH
#define MAX_MC_CH                        2                     // Max number of channels per MC (3 for EP)
#endif

#ifndef MAX_CH
#define MAX_CH                           ((MAX_IMC)*(MAX_MC_CH))     // Max channels per socket (worst case EP * EX combination = 16)
#endif

#define MAX_CXL_AMT                       0

#ifndef MAX_UNIQUE_NGN_DIMM_INTERLEAVE
#define MAX_UNIQUE_NGN_DIMM_INTERLEAVE    2                         // Max number of unique interleaves for NGN DIMM
#endif

#ifndef MAX_SPARE_RANK
#define MAX_SPARE_RANK                    2             // Max number of spare ranks in a channel
#endif

#ifndef MAX_HBM_IO
#define MAX_HBM_IO                        4
#endif

#ifndef MAX_DIMM
#define MAX_DIMM                          2                       // Max DIMM per channel
#endif

#ifndef MAX_RANK_DIMM
#define MAX_RANK_DIMM                     2
#endif

#ifndef MAX_DRAM_CLUSTERS
#define MAX_DRAM_CLUSTERS                 4
#endif

#ifndef MAX_SAD_RULES
#define MAX_SAD_RULES                     16
#endif

#ifndef MAX_FPGA_REMOTE_SAD_RULES
#define MAX_FPGA_REMOTE_SAD_RULES         2     // Maximum FPGA sockets exists on ICX platform
#endif

#define MAX_SI_SOCKET                     8   // Maximum silicon supported socket number

#define MAX_AMT                           (MAX_IMC+MAX_CXL_AMT)      // Max abstract memory target per socket
#define AMT_MAX_NODE                      ((MAX_AMT)*(MAX_SOCKET))   // Max abstract memory target for all sockets

// NGN
#define NGN_MAX_SERIALNUMBER_STRLEN       4
#define NGN_MAX_PARTNUMBER_STRLEN         30
#define NGN_FW_VER_LEN                    4
#define NGN_LOG_TYPE_NUM                  2
#define NGN_LOG_LEVEL_NUM                 2

/**
 * Memory channel index conversion macros.
 *
 * We got two types of memory channel indexes:
 * - socket channel - indexes 0..MAX_CH, used in [socket][channel] indexing
 * - IMC channel    - indexes 0..MAX_MC_CH, used in [socket][IMC][channel] indexing
 * The below defined macros convert one channel index to the other one.
 */
#define MEM_SKTCH_TO_IMC(SktCh)     ((SktCh) / MAX_MC_CH)
#define MEM_SKTCH_TO_IMCCH(SktCh)   ((SktCh) % MAX_MC_CH)
#define MEM_IMCCH_TO_SKTCH(Imc, Ch) ((Imc) * MAX_MC_CH + (Ch))

#define MEM_64MB_TO_BYTES(Size64M)  ((UINT64)(Size64M) << 26)
#define MEM_64MB_TO_KBYTES(Size64M) ((UINT64)(Size64M) << 16)
#define MEM_64MB_TO_MBYTES(Size64M) ((UINT64)(Size64M) << 6)
#define MEM_64MB_TO_GBYTES(Size64M) ((Size64M) >> 4)
#define MEM_BYTES_TO_64MB(SizeB)    ((SizeB) >> 26)
#define MEM_KBYTES_TO_64MB(SizeKB)  ((SizeKB) >> 16)
#define MEM_MBYTES_TO_64MB(SizeMB)  ((SizeMB) >> 6)

typedef UINT8               CXL_INTLV_SET_KEY;

#define FSP_RESERVED1_LEN                 77
#define FSP_RESERVED2_LEN                 2174
#define FSP_RESERVED3_LEN                 21
#define FSP_RESERVED4_LEN                 130
#define FSP_RESERVED5_LEN                 10
#define FSP_RESERVED6_LEN                 800
#pragma pack(1)

typedef enum {
  DIMM_RANK_MAP_OUT_UNKNOWN = 0,
  DIMM_RANK_MAP_OUT_MEM_DECODE,
  DIMM_RANK_MAP_OUT_POP_POR_VIOLATION,
  DIMM_RANK_MAP_OUT_RANK_DISABLED,
  DIMM_RANK_MAP_OUT_ADVMEMTEST_FAILURE,
  DIMM_RANK_MAP_OUT_MAX
} DIMM_RANK_MAP_OUT_REASON;

struct RankDevice {
  UINT8   enabled;                // 0 = disabled,  1 = enabled
  UINT8   logicalRank;            // Logical Rank number (0 - 7)
  UINT16  rankSize;               // Units of 64 MB
};

struct PersisentDpaMap
{
    UINT32 perRegionDPAOffset;
    UINT32 SPALimit;
};

typedef struct firmwareRev {
  UINT8  majorVersion;
  UINT8  minorVersion;
  UINT8  hotfixVersion;
  UINT16 buildVersion;
} FIRMWARE_REV;

typedef struct DimmDevice {
  UINT8                     Present;
  BOOLEAN                   Enabled;
  UINT8                     DcpmmPresent;                                 // 1 - This is a DCPMM
  UINT8                     X4Present;
  UINT8                     DramIoWidth;                                  // Actual DRAM IO Width (4, 8, 16)
  UINT8                     NumRanks;
  UINT8                     keyByte;
  UINT8                     actKeyByte2;                                  // Actual module type reported by SPD
  UINT8                     actSPDModuleOrg;                              // Actual number of DRAM ranks and device width
  UINT8                     dimmTs;                                       // Thermal sensor data.
  UINT16                    VolCap;                                       // Volatile capacity (AEP DIMM only)
  UINT16                    nonVolCap;                                    // Non-volatile capacity (AEP DIMM only)
  UINT16                    DimmSize;
  UINT32                    NVmemSize;
  UINT16                    SPDMMfgId;                                    // Module Mfg Id from SPD
  UINT16                    VendorID;
  UINT16                    DeviceID;
  UINT16                    RevisionID;
  UINT32                    perRegionDPA;                                 // DPA of PMEM that Nfit needs
  struct  PersisentDpaMap   perDPAMap[MAX_UNIQUE_NGN_DIMM_INTERLEAVE];    // DPA map
  UINT8                     serialNumber[NGN_MAX_SERIALNUMBER_STRLEN];    // Serial Number
  UINT8                     PartNumber[NGN_MAX_PARTNUMBER_STRLEN];        // Part Number
  UINT8                     FirmwareVersionStr[NGN_FW_VER_LEN];           // Used to update the SMBIOS TYPE 17
  struct  firmwareRev       FirmwareVersion;                              // Firmware revision
  struct  RankDevice        rankList[MAX_RANK_DIMM];
  UINT16                    InterfaceFormatCode;
  UINT16                    SubsystemVendorID;
  UINT16                    SubsystemDeviceID;
  UINT16                    SubsystemRevisionID;
  UINT16                    FisVersion;                                   // Firmware Interface Specification version
  UINT8                     DimmSku;                                      // Dimm SKU info
  UINT8                     manufacturingLocation;                        // Manufacturing location for the NVDIMM
  UINT16                    manufacturingDate;                            // Date the NVDIMM was manufactured
  INT32                     commonTck;
  UINT8                     EnergyType;                                   // 0: 12V aux power; 1: dedicated backup energy source; 2: no backup energy source
  BOOLEAN                   NvDimmNPresent;                               // JEDEC NVDIMM-N Type Memory Present
  UINT8                     CidBitMap;                                    // SubRankPer CS for DIMM device
  UINT16                    SPDRegVen;                                    // Register Vendor ID in SPD
  DIMM_RANK_MAP_OUT_REASON  MapOutReason;
} MEMMAP_DIMM_DEVICE_INFO_STRUCT;

struct ChannelDevice {
  UINT8                               Enabled;                            // 0 = channel disabled, 1 = channel enabled
  UINT8                               Features;                           // Bit mask of features to enable or disable
  UINT8                               MaxDimm;                            // Number of DIMM
  UINT8                               NumRanks;                           // Number of ranks on this channel
  UINT8                               chFailed;
  UINT8                               ngnChFailed;
  UINT8                               Is9x4DimmPresent;                   // 9x4 dimm present indicator
  UINT8                               SpareLogicalRank[MAX_SPARE_RANK];   // Logical rank, selected as Spare
  UINT8                               SparePhysicalRank[MAX_SPARE_RANK];  // Physical rank, selected as spare
  UINT16                              SpareRankSize[MAX_SPARE_RANK];      // spare rank size
  UINT8                               EnabledLogicalRanks;                // Bitmap of Logical ranks that are enabled
  UINT8                               DdrPopulationMap;                   // Bitmap to indicate location of DDR DIMMs within the channel memory slots (BIT0: Ch.D0, BIT1: CH.D1)
  UINT8                               PmemPopulationMap;                  // Bitmap to indicate location of PMem modules within the channel memory slots (BIT0: Ch.D0, BIT1: CH.D1)
  MEMMAP_DIMM_DEVICE_INFO_STRUCT      DimmInfo[MAX_DIMM];
};

struct memcontroller {
  UINT32    MemSize;
};

typedef enum {
  MemTypeNone = 0,
  MemType1lmDdr,
  MemType1lmAppDirect,
  MemType1lmAppDirectReserved,
  MemType1lmCtrl,
  MemType1lmHbm,
  MemTypeNxm,
  MemType2lmDdrCacheMemoryMode,
  MemType2lmDdrWbCacheAppDirect,
  MemType2lmHbmCacheDdr,
  MemType2lmHbmCacheMemoryMode,
  MemTypeCxlAccVolatileMem,
  MemTypeCxlAccPersistentMem,
  MemTypeFpga,
  MemTypeCxlExpVolatileMem,
  MemTypeCxlExpPersistentMem,
  MemTypeCxl2lmDdrCacheMem,
  MemTypeCxlHetero,
  MemTypeMax
} MEM_TYPE;

typedef struct SADTable {
  UINT8    Enable;         // Rule enable
  MEM_TYPE type;           // Bit map of memory region types, See defines 'MEM_TYPE_???' above for bit definitions of the ranges.
  UINT8    granularity;    // Interleave granularities for current SAD entry.  Possible interleave granularity options depend on the SAD entry type.  Note that SAD entry type BLK Window and CSR/Mailbox/Ctrl region do not support any granularity options
  UINT32   Base;           // Base of the current SAD entry
  UINT32   Limit;          // Limit of the current SAD entry
  UINT8    ways;           // Interleave ways for SAD
  UINT8    channelInterBitmap[MAX_AMT];   //Bit map to denote which DDR4/NM channels are interleaved per IMC eg: 111b - Ch 2,1 & 0 are interleaved; 011b denotes Ch1 & 0 are interleaved
  UINT8    FMchannelInterBitmap[MAX_AMT];   //Bit map to denote which FM channels are interleaved per IMC eg: 111b - Ch 2,1 & 0 are interleaved; 011b denotes Ch1 & 0 are interleaved
  UINT8    NmChWays;         // Channel Interleave ways for SAD. Represents channelInterBitmap ways for DDR4/NM.
  UINT8    FmChWays;         // Channel Interleave ways for SAD. Represents FMchannelInterBitmap ways for DDRT.
  UINT16   imcInterBitmap;   // Bit map to denote which IMCs are interleaved from this socket.
  UINT16   NmImcInterBitmap; // Bit map to denote which IMCs are interleaved from this socket as NM.
  BOOLEAN  local;          // 0 - Remote 1- Local
  UINT8    IotEnabled;     // To indicate if IOT is enabled
  UINT8    mirrored;       // To Indicate the SAD is mirrored while enabling partial mirroring
  UINT8    Attr;
  UINT8    tgtGranularity; // Interleave mode for target list
  UINT8    Cluster;        // SNC cluster, hemisphere, or quadrant index.
  UINT8    HostBridgeGran; // Host bridge interleaving granularity.
  UINT32   HotPlugPadSize; // Memory size padded for CXL hot plug. 0 if it is not a CXL hot plug SAD.
  CXL_INTLV_SET_KEY   CxlIntlvSetKey;   // The unique key of CXL interleave set. (7nm)
} SAD_TABLE;

typedef struct socket {
  UINT8      SocketEnabled;
  UINT32     IioStackBitmap;
  BOOLEAN    HbmIoEn[MAX_HBM_IO];                    // Flag indicates if HBM IO is enabled. TRUE: HBM IO is Enabled, FALSE: HBM IO is disabled.
  UINT8      imcEnabled[MAX_IMC];
  UINT8      SadIntList[MAX_DRAM_CLUSTERS * MAX_SAD_RULES][AMT_MAX_NODE]; // SAD interleave list
  UINT32     SktTotMemMapSPA;                        // Total memory mapped to SPA
  UINT32     SktMemSize2LM;                          // Total memory excluded from Limit
  SAD_TABLE  SAD[MAX_DRAM_CLUSTERS * MAX_SAD_RULES];                     // SAD table
  struct     memcontroller imc[MAX_IMC];
  struct     ChannelDevice ChannelInfo[MAX_CH];
} MEMMAP_SOCKET;

typedef struct {
  UINT8  Enabled;           // Indicates if this EDC is enabled
  UINT32 MemAsCache;        // Memory used as Cache, in 64MB units
  UINT32 MemAsFlat;         // Memory used as Flat memory, in 64MB units
} EDC_INFO;

typedef struct SystemMemoryMapElement {
  UINT16   Type;           // Type of this memory element; Bit0: 1LM  Bit1: 2LM  Bit2: PMEM  Bit3: PMEM-cache  Bit4: BLK Window  Bit5: CSR/Mailbox/Ctrl region
  UINT8    NodeId;         // Node ID of the HA Owning the memory
  UINT8    SocketId;       // Socket Id of socket that has his memory - ONLY IN NUMA
  UINT8    SktInterBitmap; // Socket interleave bitmap, if more that on socket then ImcInterBitmap and ChInterBitmap are identical in all sockets
  UINT16   ImcInterBitmap; // IMC interleave bitmap for this memory
  UINT8    ChInterBitmap[MAX_IMC];//Bit map to denote which channels are interleaved per IMC eg: 111b - Ch 2,1 & 0 are interleaved; 011b denotes Ch1 & 0 are interleaved
  UINT32   BaseAddress;    // Base Address of the element in 64MB chunks
  UINT32   ElementSize;    // Size of this memory element in 64MB chunks
} SYSTEM_MEMORY_MAP_ELEMENT;

typedef struct SystemMemoryMapHob {
  //
  // Total Clusters. In SNC2 mode there are 2 clusters and SNC4 mode has 4 clusters.
  // All2All/Quad/Hemi modes can be considered as having only one cluster (i.e SNC1).
  //
  UINT8    TotalClusters;

  UINT8    reserved1[FSP_RESERVED1_LEN];          // MEMORY_MAP_BLOCK_DECODER_DATA BlockDecoderData; // block decoder data structure
  UINT32   lowMemBase;                            // Mem base in 64MB units for below 4GB mem.
  UINT32   lowMemSize;                            // Mem size in 64MB units for below 4GB mem.
  UINT32   highMemBase;                           // Mem base in 64MB units for above 4GB mem.
  UINT32   highMemSize;                           // Mem size in 64MB units for above 4GB mem.
  UINT32   memSize;                               // Total physical memory size
  UINT16   memFreq;                               // Mem Frequency
  UINT16   HbmFreq;                               // HBM Frequency in MHz.
  UINT8    memMode;                               // 0 - Independent, 1 - Lockstep
  UINT8    volMemMode;                            // 0 - 1LM, 1 - 2LM
  UINT8    CacheMemType;                          // 0 - DDR$DDRT, 1 - HBM$DDR. Only valid if volMemMode is 2LM
  UINT16   DramType;
  UINT8    DdrVoltage;
  BOOLEAN  SupportedPMemPresent;                  // TRUE if at least one PMem is present and supported by BIOS
  BOOLEAN  EkvPresent;                            // Set if EKV controller on system
  BOOLEAN  BwvPresent;                            // Set if BWV controller on system
  BOOLEAN  CwvPresent;                            // Set if CWV controller on system
  UINT8    XMPProfilesSup;
  UINT8    XMPCapability;
  //
  // HBM Specific Info.
  //
  UINT16   HbmInstances[MAX_SOCKET];              // Total HBM instances
  UINT32   HbmFlatMemSize;                        // Total HBM memory size in 64MB units, if HBM used as Flat memory
  UINT32   HbmCacheMemSize;                       // Total HBM memory used as cache, if HBM used as Cache memory
  UINT16   HbmSpeed;                              // Configured HBM (i.e OPIO channel) speed in MT/s.
  UINT16   MaxHbmSpeed;                           // Max HBM (i.e OPIO channel) speed in MT/s.
  UINT8    SystemRasType;
  UINT8    RasModesEnabled;                       // RAS modes that are enabled
  UINT16   ExRasModesEnabled;                     // Extended RAS modes that are enabled
  UINT8    sncEnabled;                            // 0 - SNC disabled for this configuration, 1 - SNC enabled for this configuration
  UINT8    NumOfCluster;
  UINT8    NumChPerMC;
  UINT8    numberEntries;                         // Number of Memory Map Elements
  SYSTEM_MEMORY_MAP_ELEMENT Element[(MAX_SOCKET * MAX_DRAM_CLUSTERS * MAX_SAD_RULES) + MAX_FPGA_REMOTE_SAD_RULES];
  UINT8   reserved2[FSP_RESERVED2_LEN];                // struct   memSetup MemSetup;
  UINT8   reserved3[FSP_RESERVED3_LEN];                // MEM_DFXMEMVARS    DfxMemVar;
  MEMMAP_SOCKET Socket[MAX_SOCKET];
  UINT8   reserved4[FSP_RESERVED4_LEN];                // struct   memTiming  profileMemTime[XMP_MAX_PROFILES];

  UINT8   reserved5[FSP_RESERVED5_LEN];                // RASMEMORYINFO  RasMeminfo;
  UINT8   LatchSystemShutdownState;
  BOOLEAN IsWpqFlushSupported;
  UINT8   EadrSupport;
  UINT8   EadrCacheFlushMode;
  UINT8   SetSecureEraseSktChHob[MAX_SOCKET][MAX_CH];  //MAX_CH * MAX_SOCKET * MAX_DCPMM_CH
  UINT8   reserved6[FSP_RESERVED6_LEN];                // HOST_DDRT_DIMM_DEVICE_INFO_STRUCT HostDdrtDimmInfo[MAX_SOCKET][MAX_CH];
  UINT32  DdrCacheSize[MAX_SOCKET][MAX_CH];            // Size of DDR memory reserved for 2LM cache (64MB granularity)
  BOOLEAN AdrStateForPmemModule[MAX_SOCKET][MAX_CH];   // ADR state for Intel PMEM Modules
  UINT16  BiosFisVersion;                              // Firmware Interface Specification version currently supported by BIOS
  UINT16  MaxAveragePowerLimit;                        // Max Power limit in mW used for averaged power ( Valid range ends at 15000mW)
  UINT16  MinAveragePowerLimit;                        // Min Power limit in mW used for averaged power ( Valid range starts from 10000mW)
  UINT16  CurrAveragePowerLimit;                       // Current Power limit in mW used for average power
  UINT16  MaxMbbPowerLimit;                            // Max MBB power limit ( Valid range ends at 18000mW).
  UINT16  MinMbbPowerLimit;                            // Min MBB power limit ( Valid range starts from 15000mW).
  UINT16  CurrMbbPowerLimit;                           // Current Power limit in mW used for MBB power
  UINT32  MaxMbbAveragePowerTimeConstant;              // Max MBB Average Power Time Constant
  UINT32  MinMbbAveragePowerTimeConstant;              // Min MBB Average Power Time Constant
  UINT32  CurrMbbAveragePowerTimeConstant;             // Current MBB Average Power Time Constant
  UINT32  MmiohBase;                                   // MMIOH base in 64MB granularity
  UINT8   MaxSadRules;                                 // Maximum SAD entries supported by silicon (24 for 14nm silicon, 16 for 10nm silicon)
  UINT8   NumberofChaDramClusters;                     // Number of CHA DRAM decoder clusters
  BOOLEAN VirtualNumaEnable;                           // Enable or Disable Virtual NUMA
  UINT8   VirtualNumOfCluster;                         // Number of Virtual NUMA nodes in each physical NUMA node (Socket or SNC cluster)
} SYSTEM_MEMORY_MAP_HOB;

#pragma pack()

#endif // _MEMORY_MAP_DATA_H_
