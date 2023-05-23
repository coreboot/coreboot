/** @file

Copyright (c) 2019-2023, Intel Corporation. All rights reserved.<BR>

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

#ifndef _IIO_PCIE_CONFIG_UPD_H_
#define _IIO_PCIE_CONFIG_UPD_H_

#ifndef MAX_SOCKET
#define MAX_SOCKET                  4
#endif

#ifndef MAX_LOGIC_IIO_STACK
#define MAX_LOGIC_IIO_STACK        14
#endif

#ifndef MAX_IIO_PORTS_PER_SOCKET
#define MAX_IIO_PORTS_PER_SOCKET   57
#endif

#ifndef MAX_IOU_PER_SOCKET
#define MAX_IOU_PER_SOCKET         7
#endif

#define MAX_VMD_STACKS_PER_SOCKET          8  // Max number of stacks per socket supported by VMD

#pragma pack(1)

typedef struct {
  UINT8    SLOTEIP[MAX_IIO_PORTS_PER_SOCKET];          // Electromechanical Interlock Present - Slot Capabilities (D0-10 / F0 / R0xA4 / B17)
  UINT8    SLOTHPCAP[MAX_IIO_PORTS_PER_SOCKET];        // Slot Hot Plug capable - Slot Capabilities (D0-10 / F0 / R0xA4 / B6)
  UINT8    SLOTHPSUP[MAX_IIO_PORTS_PER_SOCKET];        // Hot Plug surprise supported - Slot Capabilities (D0-10 / F0 / R0xA4 / B5)
  UINT8    SLOTPIP[MAX_IIO_PORTS_PER_SOCKET];          // Power Indicator Present - Slot Capabilities (D0-10 / F0 / R0xA4 / B4)
  UINT8    SLOTAIP[MAX_IIO_PORTS_PER_SOCKET];          // Attention Inductor Present - Slot Capabilities (D0-10 / F0 / R0xA4 / B3)
  UINT8    SLOTMRLSP[MAX_IIO_PORTS_PER_SOCKET];        // MRL Sensor Present - Slot Capabilities (D0-10 / F0 / R0xA4 / B2)
  UINT8    SLOTPCP[MAX_IIO_PORTS_PER_SOCKET];          // Power Controller Present - Slot Capabilities (D0-10 / F0 / R0xA4 /B1)
  UINT8    SLOTABP[MAX_IIO_PORTS_PER_SOCKET];          // Attention Button Present - Slot Capabilities (D0-10 / F0 / R0xA4 / B0)
  UINT8    SLOTIMP[MAX_IIO_PORTS_PER_SOCKET];
  UINT8    SLOTSPLS[MAX_IIO_PORTS_PER_SOCKET];
  UINT8    SLOTSPLV[MAX_IIO_PORTS_PER_SOCKET];
  UINT16   SLOTPSP[MAX_IIO_PORTS_PER_SOCKET];
  BOOLEAN  VppEnabled[MAX_IIO_PORTS_PER_SOCKET];        // 00 -- Disable, 01 -- Enable  //no setup option defined- aj
  UINT8    VppPort[MAX_IIO_PORTS_PER_SOCKET];           // 00 -- Port 0, 01 -- Port 1   //no setup option defined- aj
  UINT8    VppAddress[MAX_IIO_PORTS_PER_SOCKET];        // 01-07 for SMBUS address of Vpp   //no setup option defined- aj
  UINT8    MuxAddress[MAX_IIO_PORTS_PER_SOCKET];        // SMBUS address of MUX              //no setup option defined
  UINT8    ChannelID[MAX_IIO_PORTS_PER_SOCKET];         // 00 -- channel 0, 01 -- channel 1  //no setup option defined

  UINT8    PciePortEnable[MAX_IIO_PORTS_PER_SOCKET];
  UINT8    PEXPHIDE[MAX_IIO_PORTS_PER_SOCKET];          // Hide any of the DMI or PCIE devices - SKT 0,1,2,3; Device 0-10 PRD
  UINT8    HidePEXPMenu[MAX_IIO_PORTS_PER_SOCKET];      // to suppress /display the PCIe port menu
  UINT8    PciePortOwnership[MAX_IIO_PORTS_PER_SOCKET];
  UINT8    RetimerConnectCount[MAX_IIO_PORTS_PER_SOCKET];
  UINT8    ConfigIOU[MAX_IOU_PER_SOCKET];               // 00-x4x4x4x4, 01-x4x4x8NA, 02-x8NAx4x4, 03-x8NAx8NA, 04-x16 (P1p2p3p4)
  UINT8    PcieHotPlugOnPort[MAX_IIO_PORTS_PER_SOCKET]; // Manual override of hotplug for port
  UINT8    VMDEnabled[MAX_VMD_STACKS_PER_SOCKET];
  UINT8    VMDPortEnable[MAX_IIO_PORTS_PER_SOCKET];
  UINT8    VMDHotPlugEnable[MAX_VMD_STACKS_PER_SOCKET];
  UINT8    PcieMaxPayload[MAX_IIO_PORTS_PER_SOCKET];
  UINT8    PciePortLinkSpeed[MAX_IIO_PORTS_PER_SOCKET]; // auto - 0(default); gen1 -1; gen2 -2; ... gen5 -5.
  UINT8    DfxDnTxPresetGen3[MAX_IIO_PORTS_PER_SOCKET]; //auto - 0xFF(default); p0 - 0; p1 -1; ... p9 - 9.
  UINT8    PcieGlobalAspm;
  UINT8    PcieMaxReadRequestSize;
} UPD_IIO_PCIE_PORT_CONFIG;

typedef struct {
  struct {
    UINT8  Segment;                                  ///< Remember segment, if it changes reset everything
    UINT8  StackPciBusPoolSize[MAX_LOGIC_IIO_STACK]; ///< Number of bus numbers needed for IIO stack
  } Socket[MAX_SOCKET];
} SYSTEM_PCI_BUS_CONFIGURATION;

typedef struct {
  UINT64 Base;    ///< Base (starting) address of a range (I/O, 32 and 64-bit mmio regions)
  UINT64 Limit;   ///< Limit (last valid) address of a range
} PCIE_BASE_LIMIT;

typedef struct {
  UINT32 MmioLSize;
  UINT64 MmioHSize;
} CXL11_LIMITS;

typedef struct {
  PCIE_BASE_LIMIT Io;        ///< Base and limit of I/O range assigned to entity
  PCIE_BASE_LIMIT LowMmio;   ///< Base and limit of low MMIO region for entity
  PCIE_BASE_LIMIT HighMmio;  ///< Base and limit of high (64-bit) MMIO region for entity
} PCI_BASE_LIMITS;

typedef struct {
  PCI_BASE_LIMITS SocketLimits;                     ///< Base and Limit of all PCIe resources for the socket
  PCI_BASE_LIMITS StackLimits[MAX_LOGIC_IIO_STACK]; ///< Base and Limit of all PCIe resources for each stack of the socket
  CXL11_LIMITS    CxlStackReq[MAX_LOGIC_IIO_STACK]; ///< Base and Limit of CXL11 resources for each stack of the socket
} SOCKET_PCI_BASE_LIMITS;

typedef struct {
  //
  // Save basic system configuration parameters along with the resource map to detect a change.
  // Remember low and high I/O memory range when saving recource configuration. It is used to verify
  // whether system memory map changed. Remember also stacks configured when creating the map.
  // If anything changed reset the system PCI resource configuration.
  //
  UINT64                 MmioHBase;
  UINT64                 MmioHGranularity;
  UINT32                 MmioLBase;
  UINT32                 MmioLLimit;
  UINT32                 MmioLGranularity;
  UINT16                 IoBase;
  UINT16                 IoLimit;
  UINT16                 IoGranularity;
  UINT32                 StackPresentBitmap[MAX_SOCKET];
  //
  // Used by the PciHostBridge DXE driver, these variables don't need to be exposed through setup options
  // The variables are used as a communication vehicle from the PciHostBridge DXE driver to an OEM hook
  // which updates the KTI resource map.
  //
  SOCKET_PCI_BASE_LIMITS Socket[MAX_SOCKET];        ///< Base and limit of all PCIe resources for each socket
} SYSTEM_PCI_BASE_LIMITS;

#pragma pack()

#endif // _IIO_PCIE_CONFIG_UPD_H_
