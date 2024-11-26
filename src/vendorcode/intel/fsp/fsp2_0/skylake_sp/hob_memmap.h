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


#ifndef _MEMORY_MAP_GUID_H_
#define _MEMORY_MAP_GUID_H_

#define FSP_SYSTEM_MEMORYMAP_HOB_GUID { \
	0x15, 0x00, 0x87, 0xf8, 0x94, 0x69, 0x98, 0x4b, 0x95, 0xa2, \
	0xbd, 0x56, 0xda, 0x91, 0xc0, 0x7f \
	}

#define CH_INDEPENDENT		0
#define FULL_MIRROR_1LM		BIT0
#define FULL_MIRROR_2LM		BIT1
#define CH_LOCKSTEP		BIT2
#define RK_SPARE		BIT3
#define PARTIAL_MIRROR_1LM	BIT5
#define PARTIAL_MIRROR_2LM	BIT6
#define STAT_VIRT_LOCKSTEP	BIT7

#define MEMTYPE_1LM_MASK       (1 << 0)
#define MEMTYPE_2LM_MASK       (1 << 1)
#define MEMTYPE_VOLATILE_MASK  (MEMTYPE_1LM_MASK | MEMTYPE_2LM_MASK)

#define MAX_IMC_PER_SOCKET                2

#define MEM_TYPE_RESERVED (1 << 8)

//
//  System Memory Map HOB information
//

#pragma pack(1)

typedef struct DimmDevice {
	UINT8    Present;
	UINT8    reserved1[3];
	UINT8    NumRanks;
	UINT8    reserved2[1];
	UINT8    actKeyByte2;
	UINT8    reserved3[9];
	UINT16   DimmSize;
	UINT8    reserved4[8];
	UINT16   VendorID;
	UINT16   DeviceID;
	UINT8    reserved5[24];
	UINT8    serialNumber[4];
	UINT8    PartNumber[4];
	UINT8    reserved6[50];
	INT32    commonTck;
	UINT8    reserved7[24];
} MEMMAP_DIMM_DEVICE_INFO_STRUCT;

struct ChannelDevice {
	UINT8    reserved1[191];
	MEMMAP_DIMM_DEVICE_INFO_STRUCT    DimmInfo[MAX_IMC];
};
typedef struct socket {
	UINT8    reserved[2075];
	struct   ChannelDevice ChannelInfo[MAX_CH];
} MEMMAP_SOCKET;

struct SystemMemoryMapElement {
	UINT8    NodeId;         // Node ID of the HA Owning the memory
	UINT8    SocketId;       // Socket Id of socket that has his memory - ONLY IN NUMA
	UINT8    ImcInterBitmap; // IMC interleave bitmap for this DRAM rule - ONLY IN NUMA
	UINT32   BaseAddress;    // Base Address of the element in 64MB chunks
	UINT32   ElementSize;    // Size of this memory element in 64MB chunks
	// Type of this memory element; Bit0: 1LM  Bit1: 2LM  Bit2: PMEM
	// Bit3: PMEM-cache  Bit4: BLK Window  Bit5: CSR/Mailbox/Ctrl region
	UINT16   Type;
};

struct SystemMemoryMapHob {
	UINT32  lowMemBase;		 // Mem base in 64MB units for below 4GB mem.
	UINT32  lowMemSize;		 // Mem size in 64MB units for below 4GB mem.
	UINT32  highMemBase;		// Mem base in 64MB units for above 4GB mem.
	UINT32  highMemSize;		// Mem size in 64MB units for above 4GB mem.
	UINT32  asilLoMemBase;	// Mem base in 64MB units for below 4GB mem.
	UINT32  asilHiMemBase;	// Mem base in 64MB units for above 4GB mem.
	UINT32  asilLoMemSize;	// Mem size in 64MB units for below 4GB mem.
	UINT32  asilHiMemSize;	// Mem size in 64MB units for above 4GB mem.

	UINT32  memSize;			// Total physical memory size
	UINT16  memFreq;			// Mem Frequency
	UINT8	memMode;			// 0 - Independent, 1 - Lockstep
	UINT8	volMemMode;	 // 0 - 1LM, 1 - 2LM
	UINT8	DimmType;
	UINT16  DramType;
	UINT8	DdrVoltage;
	// If at least one Aep Dimm Present (used by Nfit), then this should get set
	UINT8	AepDimmPresent;
	UINT8	SADNum;
	UINT8	XMPProfilesSup;
	UINT8	cpuType;
	UINT8	cpuStepping;
	UINT8	SystemRasType;
	UINT8	RasModesEnabled; // RAS modes that are enabled
	UINT8	ExRasModesEnabled; // Extended RAS modes that are enabled
	//RAS modes that are supported by current memory population.
	UINT8	RasModesSupported;
	// 0 - SNC disabled for this configuration, 1 - SNC enabled for this configuration
	UINT8	sncEnabled;
	UINT8	NumOfCluster;
	UINT8	NumChPerMC;
	UINT8	numberEntries;	 // Number of Memory Map Elements
	UINT8	maxIMC;
	UINT8	maxCh;
	struct  SystemMemoryMapElement Element[MAX_SOCKET * SAD_RULES];
	UINT8	reserved1[982];
	MEMMAP_SOCKET Socket[MAX_SOCKET];
	UINT8	reserved3[707];
};

#pragma pack()

void soc_display_memmap_hob(void);

#endif
