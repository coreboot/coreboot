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

/* Bit definitions for RasModes */
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

#define MAX_FPGA_REMOTE_SAD_RULES         2     // Maximum FPGA sockets exists on ICX platform

#define MAX_SAD_RULES                     24
#define MAX_DRAM_CLUSTERS                 1
#define MAX_IMC_PER_SOCKET                2
#define MAX_SRAT_MEM_ENTRIES_PER_IMC      8
#define MAX_ACPI_MEMORY_AFFINITY_COUNT ( \
	MAX_SOCKET * MAX_IMC_PER_SOCKET * MAX_SRAT_MEM_ENTRIES_PER_IMC \
	)

/* ACPI SRAT Memory Flags */
#define SRAT_ACPI_MEMORY_ENABLED               (1 << 0)
#define SRAT_ACPI_MEMORY_HOT_REMOVE_SUPPORTED  (1 << 1)
#define SRAT_ACPI_MEMORY_NONVOLATILE           (1 << 2)

#define MEM_TYPE_RESERVED (1 << 8)
#define MEM_ADDR_64MB_SHIFT_BITS 26

#define NGN_MAX_SERIALNUMBER_STRLEN 4
#define NGN_MAX_PARTNUMBER_STRLEN 20
#define NGN_FW_VER_LEN 4
//
//  System Memory Map HOB information
//

#pragma pack(1)

typedef struct SystemMemoryMapElement {
	// Type of this memory element; Bit0: 1LM  Bit1: 2LM  Bit2: PMEM  Bit3: PMEM-cache
	// Bit4: BLK Window  Bit5: CSR/Mailbox/Ctrl region
	UINT16   Type;
	// Node ID of the HA Owning the memory
	UINT8    NodeId;
	// Socket Id of socket that has his memory - ONLY IN NUMA
	UINT8    SocketId;
	// Socket interleave bitmap, if more that on socket then ImcInterBitmap and ChInterBitmap are identical in all sockets
	UINT8    SktInterBitmap;
	// IMC interleave bitmap for this memory
	UINT8    ImcInterBitmap;
	// Bit map to denote which channels are interleaved per IMC eg: 111b - Ch 2,1 & 0 are interleaved;
	// 011b denotes Ch1 & 0 are interleaved
	UINT8    ChInterBitmap[MAX_IMC];
	// Base Address of the element in 64MB chunks
	UINT32   BaseAddress;
	// Size of this memory element in 64MB chunks
	UINT32   ElementSize;
} SYSTEM_MEMORY_MAP_ELEMENT;

typedef struct DimmDevice {
	UINT8    Present;
	UINT8    reserved1[1];
	UINT8    DcpmmPresent;
	UINT8    reserved2[1];
	UINT8    NumRanks;
	UINT8    reserved3[1];
	UINT8    actKeyByte2;
	UINT8    reserved4[4];
	UINT16   nonVolCap;
	UINT16   DimmSize;
	UINT8    reserved5[4];
	UINT16   SPDMMfgId;				    // Module Mfg Id from SPD
	UINT16   VendorID;
	UINT16   DeviceID;
	UINT8    reserved6[22];
	UINT8    serialNumber[NGN_MAX_SERIALNUMBER_STRLEN]; // Serial Number
	UINT8    PartNumber[NGN_MAX_PARTNUMBER_STRLEN];     // Part Number
	UINT8    FirmwareVersionStr[NGN_FW_VER_LEN];        // Used to update the SMBIOS TYPE 17
	UINT8    reserved7[23];
	UINT16   SubsystemVendorID;
	UINT16   SubsystemDeviceID;
	UINT8    reserved8[4];
	UINT8    DimmSku;				    // Dimm SKU info
	UINT8    reserved9[3];
	INT32    commonTck;
	UINT8    EnergyType;
	UINT8    reserved10[1];
	UINT16   SPDRegVen;				    // Register Vendor ID in SPD
} MEMMAP_DIMM_DEVICE_INFO_STRUCT;

struct ChannelDevice {
	UINT8    reserved1[15];
	MEMMAP_DIMM_DEVICE_INFO_STRUCT    DimmInfo[MAX_IMC];
};

typedef struct socket {
	UINT8    reserved1[1114];
	struct   ChannelDevice ChannelInfo[MAX_CH];
} MEMMAP_SOCKET;

/* NOTE - Reserved sizes need to be calibrated if any of the above #define values change */
typedef struct SystemMemoryMapHob {
  UINT8    reserved1[61];

  UINT32   lowMemBase;                            // Mem base in 64MB units for below 4GB mem.
  UINT32   lowMemSize;                            // Mem size in 64MB units for below 4GB mem.
  UINT32   highMemBase;                           // Mem base in 64MB units for above 4GB mem.
  UINT32   highMemSize;                           // Mem size in 64MB units for above 4GB mem.
  UINT32   memSize;                               // Total physical memory size
  UINT16   memFreq;                               // Mem Frequency

  UINT8    reserved2[22];

  UINT8    DdrVoltage;
  UINT8    reserved3[33];
  UINT8    RasModesEnabled;                       // RAS modes that are enabled
  UINT8    reserved4[4];
  UINT8    NumChPerMC;
  UINT8    numberEntries;                         // Number of Memory Map Elements
  SYSTEM_MEMORY_MAP_ELEMENT Element[(MAX_SOCKET * MAX_DRAM_CLUSTERS * MAX_SAD_RULES) + MAX_FPGA_REMOTE_SAD_RULES];
  UINT8    reserved5[2216];
  MEMMAP_SOCKET Socket[MAX_SOCKET];
  UINT8    reserved6[1603];

  UINT16  BiosFisVersion;                              // Firmware Interface Specification version currently supported by BIOS

  UINT8    reserved7[24];

  UINT32   MmiohBase;                                   // MMIOH base in 64MB granularity

  UINT8    reserved8[5];

} SYSTEM_MEMORY_MAP_HOB;

#pragma pack()

#endif
