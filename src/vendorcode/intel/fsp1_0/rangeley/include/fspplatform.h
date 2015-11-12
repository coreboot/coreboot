/**

Copyright (C) 2013 - 2015, Intel Corporation

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

#ifndef _FSP_PLATFORM_H_
#define _FSP_PLATFORM_H_

#include "fsptypes.h"
#include "fspapi.h"

//
// Maximum number of SDRAM channels supported by each CPU
//
#define MAX_CHANNELS                 2
//
// Maximum number of DIMM sockets supported by each channel
//
#define MAX_DIMMS                    2

#pragma pack(1)
//
// SPD DDR3 structure
//
typedef struct {
  UINT8  DRAMDeviceType;                 // 2   DRAM Device Type
  UINT8  ModuleType;                     // 3   Module Type
  UINT8  SDRAMDensityAndBanks;           // 4   SDRAM Density and Banks
  UINT8  SDRAMAddressing;                // 5   SDRAM Addressing
  UINT8  VDD;                            // 6   Module Nominal Voltage
  UINT8  ModuleOrganization;             // 7   Module Organization
  UINT8  ModuleMemoryBusWidth;           // 8   Module Memory Bus Width
  UINT8  TimebaseDividend;               // 10  Medium Timebase (MTB) Dividend
  UINT8  TimebaseDivisor;                // 11  Medium Timebase (MTB) Divisor
  UINT8  SDRAMMinimumCycleTime;          // 12  SDRAM Minimum Cycle Time (tCKmin)
  UINT8  CASLatenciesLSB;                // 14  CAS Latencies Supported, Least Significant Byte
  UINT8  CASLatenciesMSB;                // 15  CAS Latencies Supported, Most Significant Byte
  UINT8  MinimumCASLatencyTime;          // 16  Minimum CAS Latency Time (tAAmin)
  UINT8  MinimumWriteRecoveryTime;       // 17  Minimum Write Recovery Time (tWRmin)
  UINT8  MinimumRASToCASDelayTime;       // 18  Minimum RAS# to CAS# Delay Time (tRCDmin)
  UINT8  MinimumRowToRowDelayTime;       // 19  Minimum Row Active to Row Active Delay Time (tRRDmin)
  UINT8  MinimumRowPrechargeDelayTime;   // 20  Minimum Row Precharge Delay Time (tRPmin)
  UINT8  UpperNibblesFortRASAndtRC;      // 21  Upper Nibbles for tRAS and tRC
  UINT8  tRASmin;                        // 22  Minimum Active to Precharge Delay Time (tRASmin), Least Significant Byte
  UINT8  tRCmin;                         // 23  Minimum Active to Active/Refresh Delay Time (tRCmin), Least Significant Byte
  UINT8  tRFCminLeastSignificantByte;    // 24  Minimum Refresh Recovery Delay Time (tRFCmin), Least Significant Byte
  UINT8  tRFCminMostSignificantByte;     // 25  Minimum Refresh Recovery Delay Time (tRFCmin), Most Significant Byte
  UINT8  tWTRmin;                        // 26  Minimum Internal Write to Read Command Delay Time (tWTRmin)
  UINT8  tRTPmin;                        // 27  Minimum Internal Read to Precharge Command Delay Time (tRTPmin)
  UINT8  UpperNibbleFortFAW;             // 28  Upper Nibble for tFAW
  UINT8  tFAWmin;                        // 29  Minimum Four Activate Window Delay Time (tFAWmin)
  UINT8  SdramThermalRefreshOption;      // 31  SdramThermalRefreshOption
  UINT8  ModuleThermalSensor;            // 32  ModuleThermalSensor
  UINT8  SDRAMDeviceType;                // 33  SDRAM Device Type
  UINT8  tCKminFine;                     // 34  Fine Offset for SDRAM Minimum Cycle Time (tCKmin)
  UINT8  tAAminFine;                     // 35  Fine Offset for Minimum CAS Latency Time (tAAmin)
  UINT8  MACCount;                       // 41  Maximum Activate Count
  UINT8  ReferenceRawCardUsed;           // 62  Reference Raw Card Used
  UINT8  AddressMappingEdgeConnector;    // 63  Address Mapping from Edge Connector to DRAM
  UINT8  ModuleManufacturerIdCodeLsb;    // 117 Module Manufacturer ID Code, Least Significant Byte
  UINT8  ModuleManufacturerIdCodeMsb;    // 118 Module Manufacturer ID Code, Most Significant Byte
  UINT8  ModuleManufacturingLocation;    // 119 Module Manufacturing Location
  UINT8  ModuleManufacturingDateYear;    // 120 Module Manufacturing Date Year
  UINT8  ModuleManufacturingDateWW;      // 121 Module Manufacturing Date creation work week
  UINT8  ModuleSerialNumberA;            // 122 Module Serial Number A
  UINT8  ModuleSerialNumberB;            // 123 Module Serial Number B
  UINT8  ModuleSerialNumberC;            // 124 Module Serial Number C
  UINT8  ModuleSerialNumberD;            // 125 Module Serial Number D
  UINT8  DramManufacturerIdLsb;          // 148 DRAM Manufacturer ID Code, LSB
  UINT8  DramManufacturerIdMsb;          // 149 DRAM Manufacturer ID Code, MSB
} MEM_DOWN_DIMM_SPD_DATA;

typedef struct {
  UINT32   MemoryDownDimmPopulation;  // 0 - Empty, 1 - DIMM populated
  MEM_DOWN_DIMM_SPD_DATA   MemoryDownDimmSpdData;
} MEM_DOWN_DIMM_CONFIG;

typedef struct {
  CONST MEM_DOWN_DIMM_CONFIG  *MemDownDimmConfig[MAX_CHANNELS][MAX_DIMMS];
} FSP_INIT_RT_PLATFORM_BUFFER;

typedef struct {
  FSP_INIT_RT_COMMON_BUFFER  Common;
  FSP_INIT_RT_PLATFORM_BUFFER Platform;
} FSP_INIT_RT_BUFFER;

#pragma pack()

#endif