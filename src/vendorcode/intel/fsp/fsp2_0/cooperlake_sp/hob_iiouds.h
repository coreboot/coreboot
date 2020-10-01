/**
Copyright (c) 2019-2020, Intel Corporation. All rights reserved.<BR>

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.

**/


#ifndef _HOB_IIOUDS_H_
#define _HOB_IIOUDS_H_

#include <fsp/util.h>

#define FSP_HOB_IIO_UNIVERSAL_DATA_GUID { \
	0xa1, 0x96, 0xf3, 0x7f, 0x7d, 0xee, 0x1e, 0x43, \
	0xba, 0x53, 0x8f, 0xCa, 0x12, 0x7c, 0x44, 0xc0  \
}

#define NUMBER_PORTS_PER_SOCKET   21
#define MAX_SOCKET                8 // CONFIG_MAX_SOCKET
#define MaxIIO                    MAX_SOCKET
#define MAX_IIO_STACK             6
#define MAX_IMC                   2
#define MAX_CH                    6
#define MC_MAX_NODE               (MAX_SOCKET * MAX_IMC)
#define MAX_CHA_MAP               4

// Maximum KTI PORTS to be used in structure definition
#if (MAX_SOCKET == 1)
  #define MAX_FW_KTI_PORTS     	  3
#else
  #define MAX_FW_KTI_PORTS        6
#endif //(MAX_SOCKET == 1)

#define MAX_LOGIC_IIO_STACK       (MAX_IIO_STACK+2)

#pragma pack(1)

//--------------------------------------------------------------------------------------//
// Structure definitions for Universal Data Store (UDS)
//--------------------------------------------------------------------------------------//

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

///
/// RC version number structure.
///
typedef struct {
	uint8_t   Major;
	uint8_t   Minor;
	uint8_t   Revision;
	uint16_t  BuildNumber;
} RC_VERSION;

/**
 IIO PCIe Ports
 **/
typedef enum {
	// IOU0, CSTACK
	PORT_0 = 0,
	// IOU1, PSTACK0
	PORT_1A,
	PORT_1B,
	PORT_1C,
	PORT_1D,
	// IOU2, PSTACK1
	PORT_2A,
	PORT_2B,
	PORT_2C,
	PORT_2D,
	// IOU3, PSTACK2
	PORT_3A,
	PORT_3B,
	PORT_3C,
	PORT_3D,
	MAX_PORTS
} PCIE_PORTS;

/**
 * IIO Stacks
 *
 * Ports    Stack	Stack(HOB)	IioConfigIou
 * =================================================
 * 0        CSTACK	stack 0		IOU0
 * 1A..1D   PSTACK0	stack 1		IOU1
 * 2A..2D   PSTACK1	stack 2		IOU2
 * 3A..3D   PSTACK2	stack 4		IOU3
 */
typedef enum {
	CSTACK = 0,
	PSTACK0,
	PSTACK1,
	PSTACK2 = 4,
	MAX_STACKS
} IIO_STACKS;

typedef struct uint64_t_struct {
	uint32_t  lo;
	uint32_t  hi;
} UINT64_STRUCT;

typedef struct {
	uint8_t       Device;
	uint8_t       Function;
} IIO_PORT_INFO;

typedef struct {
  uint8_t                     Valid;         // TRUE, if the link is valid (i.e reached normal operation)
  uint8_t                     PeerSocId;     // Socket ID
  uint8_t                     PeerSocType;   // Socket Type (0 - CPU; 1 - IIO)
  uint8_t                     PeerPort;      // Port of the peer socket
} QPI_PEER_DATA;

typedef struct {
  uint8_t                     Valid;
  uint32_t                    MmioBar[TYPE_MAX_MMIO_BAR];
  uint8_t                     PcieSegment;
  UINT64_STRUCT               SegMmcfgBase;
  uint16_t                    stackPresentBitmap;
  uint16_t                    M2PciePresentBitmap;
  uint8_t                     TotM3Kti;
  uint8_t                     TotCha;
  uint32_t                    ChaList[MAX_CHA_MAP];
  uint32_t                    SocId;
  QPI_PEER_DATA               PeerInfo[MAX_FW_KTI_PORTS];    // QPI LEP info
} QPI_CPU_DATA;

typedef struct {
  uint8_t                     Valid;
  uint8_t                     SocId;
  QPI_PEER_DATA             PeerInfo[MAX_SOCKET];    // QPI LEP info
} QPI_IIO_DATA;

typedef struct {
    IIO_PORT_INFO           PortInfo[NUMBER_PORTS_PER_SOCKET];
} IIO_DMI_PCIE_INFO;

typedef enum {
	TYPE_UBOX = 0,
	TYPE_UBOX_IIO,
	TYPE_MCP,
	TYPE_FPGA,
	TYPE_HFI,
	TYPE_NAC,
	TYPE_GRAPHICS,
	TYPE_DINO,
	TYPE_RESERVED,
	TYPE_DISABLED,              // This item must be prior to stack specific disable types
	TYPE_UBOX_IIO_DIS,
	TYPE_MCP_DIS,
	TYPE_FPGA_DIS,
	TYPE_HFI_DIS,
	TYPE_NAC_DIS,
	TYPE_GRAPHICS_DIS,
	TYPE_DINO_DIS,
	TYPE_RESERVED_DIS,
	TYPE_NONE
} STACK_TYPE;

typedef struct _STACK_RES {
  uint8_t                   Personality; // see STACK_TYPE for details
  uint8_t                   BusBase;
  uint8_t                   BusLimit;
  uint16_t                  PciResourceIoBase;
  uint16_t                  PciResourceIoLimit;
  uint32_t                  IoApicBase; // Base of IO configured for this stack
  uint32_t                  IoApicLimit; // Limit of IO configured for this stack
  uint32_t                  Mmio32Base;
  uint32_t                  Mmio32Limit;
  uint64_t                  Mmio64Base;
  uint64_t                  Mmio64Limit;
  uint32_t                  PciResourceMem32Base;
  uint32_t                  PciResourceMem32Limit;
  uint64_t                  PciResourceMem64Base;
  uint64_t                  PciResourceMem64Limit;
  uint32_t                  VtdBarAddress;
  uint32_t                  Mmio32MinSize;         // Minimum required size of MMIO32 resource needed for this stack
} STACK_RES;

typedef struct {
    uint8_t                   Valid;
    uint8_t                   SocketID;            // Socket ID of the IIO (0..3)
    uint8_t                   BusBase;
    uint8_t                   BusLimit;
    uint16_t                  PciResourceIoBase;
    uint16_t                  PciResourceIoLimit;
    uint32_t                  IoApicBase;
    uint32_t                  IoApicLimit;
    uint32_t                  Mmio32Base;
    uint32_t                  Mmio32Limit;
    uint64_t                  Mmio64Base;
    uint64_t                  Mmio64Limit;
    STACK_RES                 StackRes[MAX_LOGIC_IIO_STACK];
    uint32_t                  RcBaseAddress;
    IIO_DMI_PCIE_INFO         PcieInfo;
    uint8_t                   DmaDeviceCount;
} IIO_RESOURCE_INSTANCE;

typedef struct {
    uint16_t                  PlatGlobalIoBase;       // Global IO Base
    uint16_t                  PlatGlobalIoLimit;      // Global IO Limit
    uint32_t                  PlatGlobalMmio32Base;    // Global Mmiol base
    uint32_t                  PlatGlobalMmio32Limit;   // Global Mmiol limit
    uint64_t                  PlatGlobalMmio64Base;    // Global Mmioh Base [43:0]
    uint64_t                  PlatGlobalMmio64Limit;   // Global Mmioh Limit [43:0]
    QPI_CPU_DATA              CpuQpiInfo[MAX_SOCKET]; // QPI related info per CPU
    QPI_IIO_DATA              IioQpiInfo[MAX_SOCKET]; // QPI related info per IIO
    uint32_t                  MemTsegSize;
    uint32_t                  MemIedSize;
    uint64_t                  PciExpressBase;
    uint32_t                  PciExpressSize;
    uint32_t                  MemTolm;
    IIO_RESOURCE_INSTANCE     IIO_resource[MAX_SOCKET];
    uint8_t                   numofIIO;
    uint8_t                   MaxBusNumber;
    uint32_t                  packageBspApicID[MAX_SOCKET]; // This data array is valid only for SBSP, not for non-SBSP CPUs.
    uint8_t                   EVMode;
    uint8_t                   Pci64BitResourceAllocation;
    uint8_t                   SkuPersonality[MAX_SOCKET];
    uint8_t                   VMDStackEnable[MaxIIO][MAX_IIO_STACK];
    uint16_t                  IoGranularity;
    uint32_t                  MmiolGranularity;
    UINT64_STRUCT             MmiohGranularity;
    uint8_t                   RemoteRequestThreshold;  //5370389
    uint32_t                  UboxMmioSize;
    uint32_t                  MaxAddressBits;
} PLATFORM_DATA;

typedef struct {
	uint8_t                   CurrentUpiiLinkSpeed; // Current programmed UPI Link speed (Slow/Full speed mode)
	uint8_t                   CurrentUpiLinkFrequency; // Current requested UPI Link frequency (in GT)
	uint8_t                   OutKtiCpuSktHotPlugEn;   // 0 - Disabled, 1 - Enabled for PM X2APIC
	uint32_t                  OutKtiPerLinkL1En[MAX_SOCKET]; // output kti link enabled status for PM
	uint8_t                   IsocEnable;
	uint32_t                  meRequestedSize; // Size of the memory range requested by ME FW, in MB
	uint32_t                  ieRequestedSize; // Size of the memory range requested by IE FW, in MB
	uint8_t                   DmiVc1;
	uint8_t                   DmiVcm;
	uint32_t                  CpuPCPSInfo;
	uint8_t                   cpuSubType;
	uint8_t                   SystemRasType;
	uint8_t                   numCpus;  // 1,..4. Total number of CPU packages installed and detected (1..4)by QPI RC
	uint16_t                  tolmLimit;
	uint32_t                  tohmLimit;
	RC_VERSION                RcVersion;
	BOOLEAN                   MsrTraceEnable;
	uint8_t                   DdrXoverMode; // DDR 2.2 Mode
	// For RAS
	uint8_t                   bootMode;
	uint8_t                   OutClusterOnDieEn; // Whether RC enabled COD support
	uint8_t                   OutSncEn;
	uint8_t                   OutNumOfCluster;
	uint8_t                   imcEnabled[MAX_SOCKET][MAX_IMC];
	uint16_t                  LlcSizeReg;
	uint8_t                   chEnabled[MAX_SOCKET][MAX_CH];
	uint8_t                   memNode[MC_MAX_NODE];
	uint8_t                   IoDcMode;
	uint8_t                   DfxRstCplBitsEn;
} SYSTEM_STATUS;

typedef struct {
	PLATFORM_DATA	  PlatformData;
	SYSTEM_STATUS	  SystemStatus;
	uint32_t          OemValue;
} IIO_UDS;
#pragma pack()

#endif
