/** @file
  Data format for IIO Universal Data HOB Structure

  @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2021 Intel Corporation. <BR>

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

#ifndef _IIO_UNIVERSAL_DATA_HOB_H_
#define _IIO_UNIVERSAL_DATA_HOB_H_

#define IIO_UNIVERSAL_DATA_GUID { \
  0xa1, 0x96, 0xf3, 0x7f, 0x7d, 0xee, 0x1e, 0x43, \
  0xba, 0x53, 0x8f, 0xCa, 0x12, 0x7c, 0x44, 0xc0  \
}

#ifndef MAX_SOCKET
#define MAX_SOCKET                4
#endif

#if (MAX_SOCKET == 1)
  #define MAX_FW_KTI_PORTS        4    // Maximum KTI PORTS to be used in structure definition.
#else
  #define MAX_FW_KTI_PORTS        6    // Maximum KTI PORTS to be used in structure definition
#endif //(MAX_SOCKET == 1)

#ifndef MAX_IMC
#define MAX_IMC                   4    // Maximum memory controllers per socket
#endif

#ifndef MAX_MC_CH
#define MAX_MC_CH                 2    // Max number of channels per MC (3 for EP)
#endif

#ifndef MAX_CH
#define MAX_CH                    ((MAX_IMC)*(MAX_MC_CH))     // Max channels per socket (worst case EP * EX combination = 16)
#endif

#define MC_MAX_NODE               (MAX_SOCKET * MAX_IMC)  // Max number of memory nodes

#ifndef MAX_IIO_PORTS_PER_SOCKET
#define MAX_IIO_PORTS_PER_SOCKET  57
#endif

#ifndef MAX_IIO_STACK
#define MAX_IIO_STACK             12
#endif

#define MAX_LOGIC_IIO_STACK       14

#define MAX_COMPUTE_DIE           1
#define MAX_CHA_MAP               (2 * MAX_COMPUTE_DIE)  //for GNR & SRF only, each compute die has its own CAPID6 & CAPID7 (i.e. 2 CAPID registers)

#pragma pack(1)

typedef struct _UINT64_STRUCT {
  UINT32  lo;
  UINT32  hi;
} UINT64_STRUCT, *PUINT64_STRUCT;

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

typedef struct {
    UINT8       Device;
    UINT8       Function;
} IIO_PORT_INFO;

typedef struct {
  UINT8   Major;
  UINT8   Minor;
  UINT8   Revision;
  UINT16  BuildNumber;
} RC_VERSION;

//--------------------------------------------------------------------------------------//
// Structure definitions for Universal Data Store (UDS)
//--------------------------------------------------------------------------------------//
typedef struct {
  UINT8                     Valid;         // TRUE, if the link is valid (i.e reached normal operation)
  UINT8                     PeerSocId;     // Socket ID
  UINT8                     PeerSocType;   // Socket Type (0 - CPU; 1 - IIO)
  UINT8                     PeerPort;      // Port of the peer socket
} QPI_PEER_DATA;

typedef struct {
  UINT8                     Valid;
  UINT32                    MmioBar[TYPE_MAX_MMIO_BAR];
  UINT8                     PcieSegment;
  UINT64_STRUCT             SegMmcfgBase;
  UINT32                    StackPresentBitmap;
  UINT16                    CxlPresentBitmap;
  UINT16                    Cxl20CapableBitmap;
  UINT8                     TotM3Kti;
  UINT8                     TotCha;
  UINT32                    ChaList[MAX_CHA_MAP];
  UINT32                    SocId;
  QPI_PEER_DATA             PeerInfo[MAX_FW_KTI_PORTS];    // QPI LEP info
} QPI_CPU_DATA;

typedef struct {
  UINT8                     Valid;
  UINT8                     SocId;
  QPI_PEER_DATA             PeerInfo[MAX_SOCKET];    // QPI LEP info
} QPI_IIO_DATA;

typedef struct {
    IIO_PORT_INFO           PortInfo[MAX_IIO_PORTS_PER_SOCKET];
} IIO_DMI_PCIE_INFO;

typedef struct _STACK_RES {
  UINT8                   Personality;
  UINT8                   BusBase;               // Base of Bus configured for this stack
  UINT8                   BusLimit;              // Limit of Bus configured for this stack
  UINT16                  IoBase;                // Base of IO configured for this stack
  UINT16                  IoLimit;               // Limit of IO configured for this stack
  UINT32                  IoApicBase;
  UINT32                  IoApicLimit;
  UINT32                  Mmio32Base;            // Base of low MMIO configured for this stack in memory map
  UINT32                  Mmio32Limit;           // Limit of low MMIO configured for this stack in memory map
  UINT64                  Mmio64Base;            // Base of high MMIO configured for this stack in memory map
  UINT64                  Mmio64Limit;           // Limit of high MMIO configured for this stack in memory map
  UINT8                   PciResourceBusBase;    // Base of Bus resource available for PCI devices
  UINT8                   PciResourceBusLimit;   // Limit of Bus resource available for PCI devices
  UINT16                  PciResourceIoBase;     // Base of IO resource available for PCI devices
  UINT16                  PciResourceIoLimit;    // Limit of IO resource available for PCI devices
  UINT32                  PciResourceMem32Base;  // Base of low MMIO resource available for PCI devices
  UINT32                  PciResourceMem32Limit; // Limit of low MMIO resource available for PCI devices
  UINT64                  PciResourceMem64Base;  // Base of high MMIO resource available for PCI devices
  UINT64                  PciResourceMem64Limit; // Limit of high MMIO resource available for PCI devices
  UINT32                  VtdBarAddress;         // NOTE: Obsolete, not used in next gen platforms
} STACK_RES;

typedef struct {
    UINT8                   Valid;
    UINT8                   SocketID;            // Socket ID of the IIO (0..3)
    UINT8                   BusBase;
    UINT8                   BusLimit;
    UINT16                  PciResourceIoBase;
    UINT16                  PciResourceIoLimit;
    UINT32                  IoApicBase;
    UINT32                  IoApicLimit;
    UINT32                  Mmio32Base;          // Base of low MMIO configured for this socket in memory map
    UINT32                  Mmio32Limit;         // Limit of low MMIO configured for this socket in memory map
    UINT64                  Mmio64Base;          // Base of high MMIO configured for this socket in memory map
    UINT64                  Mmio64Limit;         // Limit of high MMIO configured for this socket in memory map
    STACK_RES               StackRes[MAX_LOGIC_IIO_STACK];
    IIO_DMI_PCIE_INFO       PcieInfo;            // NOTE: Obsolete, not used in next gen platforms
} IIO_RESOURCE_INSTANCE;

typedef struct {
    UINT16                  PlatGlobalIoBase;       // Global IO Base
    UINT16                  PlatGlobalIoLimit;      // Global IO Limit
    UINT32                  PlatGlobalMmio32Base;   // Global Mmiol base
    UINT32                  PlatGlobalMmio32Limit;  // Global Mmiol limit
    UINT64                  PlatGlobalMmio64Base;   // Global Mmioh Base [43:0]
    UINT64                  PlatGlobalMmio64Limit;  // Global Mmioh Limit [43:0]
    QPI_CPU_DATA            CpuQpiInfo[MAX_SOCKET]; // QPI related info per CPU
    QPI_IIO_DATA            IioQpiInfo[MAX_SOCKET]; // QPI related info per IIO
    UINT32                  MemTsegSize;
    UINT64                  PciExpressBase;
    UINT32                  PciExpressSize;
    UINT32                  MemTolm;
    IIO_RESOURCE_INSTANCE   IIO_resource[MAX_SOCKET];
    UINT8                   numofIIO;
    UINT8                   MaxBusNumber;
    UINT32                  packageBspApicID[MAX_SOCKET]; // This data array is valid only for SBSP, not for non-SBSP CPUs. <AS> for CpuSv
    UINT8                   EVMode;
    UINT8                   SkuPersonality[MAX_SOCKET];
    UINT16                  IoGranularity;
    UINT32                  MmiolGranularity;
    UINT64_STRUCT           MmiohGranularity;
    UINT8                   RemoteRequestThreshold;  //5370389
    UINT32                  UboxMmioSize;
    UINT32                  MaxAddressBits;
} PLATFORM_DATA;

typedef struct {
    UINT8                   CurrentUpiiLinkSpeed;// Current programmed UPI Link speed (Slow/Full speed mode)
    UINT8                   CurrentUpiLinkFrequency; // Current requested UPI Link frequency (in GT)
    UINT8                   OutKtiCpuSktHotPlugEn;            // 0 - Disabled, 1 - Enabled for PM X2APIC
    UINT32                  OutKtiPerLinkL1En[MAX_SOCKET];    // output kti link enabled status for PM
    UINT8                   IsocEnable;
    UINT32                  meRequestedSize; // Size of the memory range requested by ME FW, in MB
    UINT8                   DmiVc1;
    UINT8                   DmiVcm;
    UINT32                  CpuPCPSInfo;
    UINT8                   cpuSubType;
    UINT8                   SystemRasType;
    UINT8                   numCpus;                                        // 1,..4. Total number of CPU packages installed and detected (1..4)by QPI RC
    UINT16                  tolmLimit;
    RC_VERSION              RcVersion;
    BOOLEAN                 MsrTraceEnable;
    UINT8                   DdrXoverMode;           // DDR 2.2 Mode
    // For RAS
    UINT8                   bootMode;
    UINT8                   OutClusterOnDieEn; // Whether RC enabled COD support
    UINT8                   OutSncEn;
    UINT8                   OutNumOfCluster;
    UINT8                   imcEnabled[MAX_SOCKET][MAX_IMC];
    UINT16                  LlcSizeReg;
    UINT8                   chEnabled[MAX_SOCKET][MAX_CH];
    UINT8                   memNode[MC_MAX_NODE];
    UINT8                   IoDcMode;
    UINT8                   DfxRstCplBitsEn;
    UINT8                   BitsUsed;    //For 5 Level Paging
} SYSTEM_STATUS;

typedef struct {
    PLATFORM_DATA           PlatformData;
    SYSTEM_STATUS           SystemStatus;
    UINT32                  OemValue;
} IIO_UDS;
#pragma pack()

#endif  //#ifndef _IIO_UNIVERSAL_DATA_HOB_H_
