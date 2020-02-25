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
#define MAX_SOCKET                CONFIG_MAX_SOCKET
#define MAX_IIO                   MAX_SOCKET
#define MAX_IIO_STACK             6
#define MAX_KTI_PORTS             3
#define MAX_IMC                   2
#define MAX_CH                    6
#define MC_MAX_NODE               (MAX_SOCKET * MAX_IMC)
#define SAD_RULES                 24
#define TDP_MAX_LEVEL             5

#pragma pack(1)

//--------------------------------------------------------------------------------------//
// Structure definitions for Universal Data Store (UDS)
//--------------------------------------------------------------------------------------//
typedef struct uint64_t_struct {
	uint32_t  lo;
	uint32_t  hi;
} UINT64_STRUCT;

typedef struct {
	uint8_t       Device;
	uint8_t       Function;
} IIO_PORT_INFO;

typedef struct {
	// TRUE, if the link is valid (i.e reached normal operation)
	uint8_t                     Valid;
	uint8_t                     PeerSocId;     // Socket ID
	uint8_t                     PeerSocType;   // Socket Type (0 - CPU; 1 - IIO)
	uint8_t                     PeerPort;      // Port of the peer socket
} QPI_PEER_DATA;

typedef struct {
	uint8_t            Valid;
	uint8_t            SocketFirstBus;
	uint8_t            SocketLastBus;
	uint8_t            segmentSocket;
	uint8_t            PcieSegment;
	UINT64_STRUCT      SegMmcfgBase;
	uint8_t            stackPresentBitmap;
	uint8_t            StackBus[MAX_IIO_STACK];
	uint8_t            M2PciePresentBitmap;
	uint8_t            TotM3Kti;
	uint8_t            TotCha;
	uint32_t           ChaList;
	uint32_t           SocId;
	QPI_PEER_DATA      PeerInfo[MAX_KTI_PORTS];		// QPI LEP info
} QPI_CPU_DATA;

typedef struct {
	uint8_t            Valid;
	uint8_t            SocId;
	QPI_PEER_DATA      PeerInfo[MAX_SOCKET];		// QPI LEP info
} QPI_IIO_DATA;

typedef struct {
	IIO_PORT_INFO    PortInfo[NUMBER_PORTS_PER_SOCKET];
} IIO_DMI_PCIE_INFO;

typedef struct _STACK_RES {
	uint8_t    Personality;
	uint8_t    BusBase;
	uint8_t    BusLimit;
	uint16_t   PciResourceIoBase;
	uint16_t   PciResourceIoLimit;
	uint32_t   IoApicBase;
	uint32_t   IoApicLimit;
	uint32_t   PciResourceMem32Base;
	uint32_t   PciResourceMem32Limit;
	uint64_t   PciResourceMem64Base;
	uint64_t   PciResourceMem64Limit;
	uint32_t   VtdBarAddress;
} STACK_RES;

typedef struct {
	uint8_t                  Valid;
	int8_t                   SocketID;            // Socket ID of the IIO (0..3)
	uint8_t                  BusBase;
	uint8_t                  BusLimit;
	uint16_t                 PciResourceIoBase;
	uint16_t                 PciResourceIoLimit;
	uint32_t                 IoApicBase;
	uint32_t                 IoApicLimit;
	uint32_t                 PciResourceMem32Base;
	uint32_t                 PciResourceMem32Limit;
	uint64_t                 PciResourceMem64Base;
	uint64_t                 PciResourceMem64Limit;
	STACK_RES                StackRes[MAX_IIO_STACK];
	uint32_t                 RcBaseAddress;
	IIO_DMI_PCIE_INFO        PcieInfo;
	uint8_t                  DmaDeviceCount;
} IIO_RESOURCE_INSTANCE;

typedef struct {
	uint16_t               PlatGlobalIoBase;       // Global IO Base
	uint16_t               PlatGlobalIoLimit;	  // Global IO Limit
	uint32_t               PlatGlobalMmiolBase;	// Global Mmiol base
	uint32_t               PlatGlobalMmiolLimit;   // Global Mmiol limit
	uint64_t               PlatGlobalMmiohBase;	// Global Mmioh Base [43:0]
	uint64_t               PlatGlobalMmiohLimit;   // Global Mmioh Limit [43:0]
	QPI_CPU_DATA           CpuQpiInfo[MAX_SOCKET]; // QPI related info per CPU
	QPI_IIO_DATA           IioQpiInfo[MAX_SOCKET]; // QPI related info per IIO
	uint32_t               MemTsegSize;
	uint32_t               MemIedSize;
	uint64_t               PciExpressBase;
	uint32_t               PciExpressSize;
	uint32_t               MemTolm;
	IIO_RESOURCE_INSTANCE  IIO_resource[MAX_SOCKET];
	uint8_t                numofIIO;
	uint8_t                MaxBusNumber;
	// This data array is valid only for SBSP, not for non-SBSP CPUs. <AS> for CpuSv
	uint32_t               packageBspApicID[MAX_SOCKET];
	uint8_t                EVMode;
	uint8_t                Pci64BitResourceAllocation;
	uint8_t                SkuPersonality[MAX_SOCKET];
	uint8_t                VMDStackEnable[MAX_IIO][MAX_IIO_STACK];
	uint16_t               IoGranularity;
	uint32_t               MmiolGranularity;
	UINT64_STRUCT          MmiohGranularity;
	uint8_t                RemoteRequestThreshold;
	// bitmap of Softsku sockets with CPUs present detected
	uint64_t               softskuSocketPresentBitMap;
	BOOLEAN                Simics; // TRUE - Simics Environtment; FALSE - H\w
} PLATFORM_DATA;

typedef struct {
	uint32_t         FILLER_BUG;
	// Current programmed CSI (or UPI) Link speed (Slow/Full speed mode)
	uint8_t          CurrentCsiLinkSpeed;
	// Current requested CSI (or UPI) Link frequency (in GT)
	uint8_t          CurrentCsiLinkFrequency;
	// output kti link enabled status for PM
	uint32_t         OutKtiPerLinkL1En[MAX_SOCKET];
	uint8_t          IsocEnable;
	// Size of the memory range requested by ME FW, in MB
	uint32_t         meRequestedSize;
	uint8_t          DmiVc1;
	uint8_t          DmiVcm;
	uint32_t         CpuPCPSInfo;
	uint8_t          MinimumCpuStepping;
	uint8_t          LtsxEnable;
	uint8_t          MctpEn;
	uint8_t          cpuType;
	uint8_t          cpuSubType;
	uint8_t          SystemRasType;
	// 1,..4. Total number of CPU packages installed and detected (1..4)by QPI RC
	uint8_t          numCpus;
	// Fused Core Mask in the package
	uint32_t         FusedCores[MAX_SOCKET];
	// Current activated core Mask in the package
	uint32_t         ActiveCores[MAX_SOCKET];
	// Package Max Non-turbo Ratio (per socket).
	uint8_t          MaxCoreToBusRatio[MAX_SOCKET];
	// Package Maximum Efficiency Ratio (per socket).
	uint8_t          MinCoreToBusRatio[MAX_SOCKET];
	uint8_t          CurrentCoreToBusRatio; // Current system Core to Bus Ratio
	// ISS Capable (system level) Bit[7:0] and current Config TDP Level Bit[15:8]
	uint32_t         IntelSpeedSelectCapable;
	uint32_t         IssConfigTdpLevelInfo; // get B2P CONFIG_TDP_GET_LEVELS_INFO
	// get B2P CONFIG_TDP_GET_TDP_INFO
	uint32_t         IssConfigTdpTdpInfo[TDP_MAX_LEVEL];
	// get B2P CONFIG_TDP_GET_POWER_INFO
	uint32_t         IssConfigTdpPowerInfo[TDP_MAX_LEVEL];
	// get B2P CONFIG_TDP_GET_CORE_COUNT
	uint8_t          IssConfigTdpCoreCount[TDP_MAX_LEVEL];
	// bitmap of sockets with CPUs present detected by QPI RC
	uint32_t         socketPresentBitMap;
	// bitmap of NID w/ fpga  present detected by QPI RC
	uint32_t         FpgaPresentBitMap;
	uint16_t         tolmLimit;
	uint32_t         tohmLimit;
	uint32_t         mmCfgBase;
	uint32_t         RcVersion;
	uint8_t          DdrXoverMode;		   // DDR 2.2 Mode
	uint8_t          bootMode;
	uint8_t          OutClusterOnDieEn; // Whether RC enabled COD support
	uint8_t          OutSncEn;
	uint8_t          OutNumOfCluster;
	uint8_t          imcEnabled[MAX_SOCKET][MAX_IMC];
	uint8_t          numChPerMC;
	uint8_t          maxCh;
	uint8_t          maxIMC;
	uint16_t         LlcSizeReg;
	uint8_t          chEnabled[MAX_SOCKET][MAX_CH];
	uint8_t          mcId[MAX_SOCKET][MAX_CH];
	uint8_t          memNode[MC_MAX_NODE];
	uint8_t          IoDcMode;
	uint8_t          CpuAccSupport;
	uint8_t          SmbusErrorRecovery;
	uint8_t          AepDimmPresent;
} SYSTEM_STATUS;

typedef struct {
	PLATFORM_DATA	  PlatformData;
	SYSTEM_STATUS	  SystemStatus;
	uint32_t          OemValue;
} IIO_UDS;
#pragma pack()

void soc_display_iio_universal_data_hob(void);

#endif
