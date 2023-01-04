/** @file
  System Information HOB Hearder File

  @copyright
  INTEL CONFIDENTIAL
  Copyright 2017 - 2021 Intel Corporation. <BR>

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

#ifndef _SYSTEM_INFO_HOB_H_
#define _SYSTEM_INFO_HOB_H_

#define SYSTEM_INFO_HOB_GUID          { 0x7650A0F2, 0x0D91, 0x4B0C, { 0x92, 0x3B, 0xBD, 0xCF, 0x22, 0xD1, 0x64, 0x35 }}

#ifndef MAX_SOCKET
#define MAX_SOCKET                    4
#endif

#ifndef MAX_IIO_STACK
#define MAX_IIO_STACK                 12
#endif

#define MAX_LOGIC_IIO_STACK           14

#ifndef MAX_IMC
#define MAX_IMC                       4                     // Maximum memory controllers per socket
#endif

#ifndef MAX_MC_CH
#define MAX_MC_CH                     2                     // Max number of channels per MC (3 for EP)
#endif

#ifndef MAX_CH
#define MAX_CH                        ((MAX_IMC)*(MAX_MC_CH))     // Max channels per socket (worst case EP * EX combination = 16)
#endif

#define MAX_HPM_PFS_ENTRY_NUM         15      // Number of entries in PFS structure
#define HPM_PFS_ENTRY_SIZE            2       // Entry size of PFS structure in DWORD

#pragma pack(1)

#ifndef MMIO_BARS_ENUM
#define MMIO_BARS_ENUM
typedef enum {
  TYPE_SCF_BAR = 0,
  TYPE_PCU_BAR,
  TYPE_MEM_BAR0,
  TYPE_MEM_BAR1,
  TYPE_MEM_BAR2,
  TYPE_MEM_BAR3,
  TYPE_MEM_BAR4,
  TYPE_MEM_BAR5,
  TYPE_MEM_BAR6,
  TYPE_MEM_BAR7,
  TYPE_SBREG_BAR,
  TYPE_MAX_MMIO_BAR
} MMIO_BARS;
#endif

typedef enum {
  ReservedMemSs,
  Ddr4MemSs = 1,
  Ddr5MemSs = 2,
  LpDdr4MemSs = 3,
  LpDdr5MemSs = 4,
  Hbm2MemSs = 5,
  MrcMstMax,
  MrcMstDelim = MAX_INT32
} MRC_MST;

typedef enum BootMode {
  NormalBoot  = 0,  // Normal path through RC with full init, mem detection, init, training, etc.
                    //    Some of these MRC specific init routines can be skipped based on MRC input params
                    //    in addition to the sub-boot type (WarmBoot, WarmBootFast, etc).
  S3Resume    = 1   // S3 flow through RC. Should do the bare minimum required for S3
                    //    init and be optimized for speed.
} BootMode;

//
// This is used to determine what type of die is connected to a UPI link
//
typedef enum {
  UpiConnectionTypeCpu,
  UpiConnectionTypePcieGen4,
  UpiConnectionTypeFpga,
  UpiConnectionTypeMax
} UPI_CONNECTION_TYPE;

//
// VSEC info for HPM
// The HPM_INFO struct contains info collected from the VSEC structure for the HPM.
// VSEC Structure is defined as below:
//
// ------------------------VSEC Structure------------------------
// | Next Cap Offset(12b)  | Cap Version(4b) | PCIe Cap ID(16b) |
// --------------------------------------------------------------
// |     VSEC_LEN(12b)     |   VSEC_REV(4b)  |   VSEC_ID(16b)   |
// --------------------------------------------------------------
// | EntrySize(8b)     |     NumEntries(8b)  |    Rsvd(16b)     |
// --------------------------------------------------------------
// |                 Address(29b)                    | tBIR(3b) |
// --------------------------------------------------------------
//
//
typedef struct {
  UINT8        NumEntries;  // The number of PM feature interface instances in this VSEC space
  UINT8        EntrySize;   // The entry size for each PM interface instance in 32-bit DWORDs
  UINT32       Address;     // The offset from the BAR register which is used for HPM to point to the base of the discovery entry. i.e. the PFS (PM Feature Structure)
  UINT8        tBIR;        // Which one of he function's BAR is used for the PFS offset
} HPM_INFO;

//
//-------------------------------- - PFS Entry--------------------------------
//|  EntrySize(16b)                     |  NumEntries(8b)  |  VSEC_SubID(8b)  |
//----------------------------------------------------------------------------
//| RSVD(14b)                | Attr(2b) |       CapOffset(16b)                |
//----------------------------------------------------------------------------
//
typedef struct {
  UINT32                TpmiId : 8;
  UINT32                NumEntries : 8;
  UINT32                EntrySize : 16;
  UINT32                CapOffset : 16;
  UINT32                Attribute : 2;
  UINT32                EntryBitMap : 8;
  UINT32                Rsvd : 6;
} PFS_ENTRY;

//
// HPM (Hierarchical Power Management) VSEC resource allocation info
//
typedef struct {
  BOOLEAN             HpmEnabled;                   // HPM enabled or not in BIOS knob
  BOOLEAN             HpmInfraReady;                // All the HPM infrastructure data is buffered
  UINT32              OobMsmHpmBarBase;               // The base of OOBMSM BAR1 for HPM PFS and LUT tables
  UINT32              OobMsmHpmBarLimit;              // The limit of OOBMSM BAR1 from where the required size will be allocated
  UINT32              OobMsmHpmBarMmcfgAddress;       // The OOBMSM BAR1 MMCFG address
  UINT32              OobMsmHpmBarRequiredSize;       // The required size of OOBMSM BAR1

  HPM_INFO            HpmVsecInfo;                  // For now, single VSEC is assumed
  PFS_ENTRY           Pfs[MAX_HPM_PFS_ENTRY_NUM];   // The buffered PFS structure

} HPM_VSEC_RESOURCE;

//
// Stack id swap information, which includes stack swap flag and the new stack id swap array.
//
typedef struct {
  BOOLEAN StackSwapFlag;
  UINT8   StackIdSwapArray[MAX_IIO_STACK];
} STACKID_SWAP_INFO;

typedef struct {
  UINT64  Cxl1p1RcrbBase;
  BOOLEAN Cxl1p1RcrbValid;
} CXL_1P1_RCRB;

typedef struct {
  UINT32 StackPresentBitmap[MAX_SOCKET];           ///< bitmap of present stacks per socket
  UINT8  StackBus[MAX_SOCKET][MAX_LOGIC_IIO_STACK];///< Bus of each stack
  UINT32 StackMmiol[MAX_SOCKET][MAX_IIO_STACK];    ///< mmiol of each IIO stack, if it works as CXL, the mmiol base is RCRBBAR
  UINT8  SocketFirstBus[MAX_SOCKET];
  UINT8  Socket10nmUboxBus0[MAX_SOCKET];           //10nm CPU use only
  UINT8  TotIoDie[MAX_SOCKET];                     //GNR & SRF CPU use only
  UINT8  TotCmpDie[MAX_SOCKET];                    //GNR & SRF CPU use only
  UINT8  SocketLastBus[MAX_SOCKET];
  UINT8  segmentSocket[MAX_SOCKET];
  UINT8  KtiPortCnt;
  UINT32 socketPresentBitMap;
  UINT32 SecondaryNodeBitMap;
  UINT32 FpgaPresentBitMap;
  UINT32 mmCfgBase;
  UINT64 SocketMmCfgBase[MAX_SOCKET];
  UINT8  DdrMaxCh;
  UINT8  DdrMaxImc;                               ///< Logical number of IMC count, ignoring logical holes
  UINT8  DdrPhysicalMaxImc;                       ///< Physical number of IMC count from Capid
  UINT8  DdrNumChPerMc;
  UINT8  DdrNumPseudoChPerCh;
  UINT8  imcEnabled[MAX_SOCKET][MAX_IMC];
  UINT8  mcId[MAX_SOCKET][MAX_CH];
  MRC_MST MemSsType[MAX_SOCKET];                    ///< MemSsType global identifier for DDR vs. HBM
  UINT32  MmioBar[MAX_SOCKET][TYPE_MAX_MMIO_BAR];
  UINT8   HbmMaxCh;
  UINT8   HbmMaxIoInst;
  UINT8   HbmNumChPerMc;
  UINT8   HbmNumChPerIo;
  UINT32  LastCsrAddress[2];
  UINT32  LastCsrMmioAddr;
  UINT8   CsrCachingEnable;
  UINT32  LastCsrMcAddress[2];
  UINT32  LastCsrMcMmioPhyAddr;
  UINT8   CsrPciBarCachingEnable;
  UINT32  LastCsrPciBarAddr[2];
  UINT64  LastCsrPciBarPhyAddr;
  UINT32  LastSBPortId[MAX_SOCKET];
  UPI_CONNECTION_TYPE         UpiConnectionType[MAX_SOCKET];
  BOOLEAN PostedCsrAccessAllowed;                   // SW is allowed to use posted CSR writes method when TRUE
  BOOLEAN PostedWritesEnabled;                      // All CSR writes use posted method when TRUE, non-posted when FALSE
  BOOLEAN DataPopulated;                            // CPU_CSR_ACCESS_VAR is unavailable when FALSE
  HPM_VSEC_RESOURCE SocketHpmVsecRes[MAX_SOCKET];   // HPM VSEC info for all sockets
  BOOLEAN HbmSku;
  UINT8   HcxType[MAX_SOCKET];
  STACKID_SWAP_INFO StackIdSwapInfo[MAX_SOCKET];    //StackID sync after do StackId swap,including Stack swap table and whether do stack swap
  CXL_1P1_RCRB  Cxl1p1Rcrb[MAX_SOCKET][MAX_IIO_STACK];    // CXL 1.1 RCRB, one per PI5 stack
  UINT32  DmiRcrb[MAX_SOCKET];                      // DMI RCRB region, one per socket
  UINT8   FabricType;                               //Compute die 10x6, 10x5, and 6x5 type is stored
  UINT8   ChopType;                                 //Compute Die Chop Type
  UINT8   MdfInstCount;
} CPU_CSR_ACCESS_VAR;

typedef struct {
  UINT32 MeRequestedSizeNv;
  UINT32 MeRequestedAlignmentNv;
  UINT8  SbspSocketIdNv;
} SYS_INFO_VAR_NVRAM;

typedef struct _CPUID_REGISTER_INFO {
  UINT32 Eax;
  UINT32 Ebx;
  UINT32 Ecx;
  UINT32 Edx;
} CPUID_REGISTER_INFO;

typedef struct _PROCESSOR_COMMON_INFO {
  UINT32  capid0;
  UINT32  capid1;
  UINT32  capid2;
  UINT32  capid3;
  UINT32  capid4;
  UINT32  capid5;
  UINT32  capid6lo;
  UINT32  capid6hi;
  CPUID_REGISTER_INFO  ExtCpuid7;
  CPUID_REGISTER_INFO  ExtCpuid1B;
} PROCESSOR_COMMON_INFO;

typedef struct {
  UINT32 MeRequestedSize;
  UINT32 MeRequestedAlignment;
  UINT32 CheckPoint;
  UINT8  ResetRequired;
  UINT8  Emulation;
  BootMode SysBootMode;
  CPU_CSR_ACCESS_VAR        CpuCsrAccessVarHost;    // Common resource for CsrAccessRoutines
  UINT64   CpuFreq;
  UINT8    SocketId;
  SYS_INFO_VAR_NVRAM SysInfoVarNvram;
  BOOLEAN  UsraTraceControl;
  UINT16   UsraTraceConfiguration;                 // Bit               7       6        5           4                     3:0
                                                   //               Write, Modify, GetAddr, Long/Short, Trace 0x5 as signature
                                                   // Bit               15                              14:11   10     9     8
                                                   // DumpCpuCsrAccessVar,                                TBD, CSR, PCIE, Read
  BOOLEAN  CpuCsrAccessVarInfoDumped;
  PROCESSOR_COMMON_INFO          ProcessorCommonInfo[MAX_SOCKET];
  EFI_PHYSICAL_ADDRESS  SocAddrMapData;
  UINTN    UsraPpiPtr;
} SYSTEM_INFO_VAR;


#pragma pack ()

#endif //#ifndef _SYSTEM_INFO_HOB_H_
