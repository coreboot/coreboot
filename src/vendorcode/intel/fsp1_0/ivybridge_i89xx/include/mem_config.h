/** @file

Copyright (C) 2013, Intel Corporation

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

//
// mem_config.h
//

#ifndef _MEM_CONFIG_H_
#define _MEM_CONFIG_H_

typedef enum {
  fi1067_IVB=0,
  fi1333_IVB,
  fi1400_IVB,
  fi1600_IVB,
  fi1800_IVB,
  fi1867_IVB,
  fi2000_IVB,
  fi2133_IVB,
  fi2200_IVB,
  fi2400_IVB,
  fi2600_IVB,
  fi2667_IVB,
  fi2800_IVB,
  fiUnsupport_IVB,
}TFrequencyIndex_IVB;

#define NUM_IVB_MEM_CLK_FREQUENCIES 13

// DDR3 memory SPD data
//
// NOTE: This only includes the SPD bytes that are relevant to the MRC
typedef struct {                             // BYTE
    uint8_t  SPDGeneral;                     // 0   Number of Serial PD Bytes Written / SPD Device Size / CRC Coverage 1, 2
    uint8_t  SPDRevision;                    // 1   SPD Revision
    uint8_t  DRAMDeviceType;                 // 2   DRAM Device Type
    uint8_t  ModuleType;                     // 3   Module Type
    uint8_t  SDRAMDensityAndBanks;           // 4   SDRAM Density and Banks
    uint8_t  SDRAMAddressing;                // 5   SDRAM Addressing
    uint8_t  VDD;                            // 6   Module Nominal Voltage
    uint8_t  ModuleOrganization;             // 7   Module Organization
    uint8_t  ModuleMemoryBusWidth;           // 8   Module Memory Bus Width
    uint8_t  FineTimebase;                   // 9   Fine Timebase (FTB) Dividend / Divisor
    uint8_t  TimebaseDividend;               // 10  Medium Timebase (MTB) Dividend
    uint8_t  TimebaseDivisor;                // 11  Medium Timebase (MTB) Divisor
    uint8_t  SDRAMMinimumCycleTime;          // 12  SDRAM Minimum Cycle Time (tCKmin)
    uint8_t  Reserved0;                      // 13  Reserved0
    uint8_t  CASLatenciesLSB;                // 14  CAS Latencies Supported, Least Significant Byte
    uint8_t  CASLatenciesMSB;                // 15  CAS Latencies Supported, Most Significant Byte
    uint8_t  MinimumCASLatencyTime;          // 16  Minimum CAS Latency Time (tAAmin)
    uint8_t  MinimumWriteRecoveryTime;       // 17  Minimum Write Recovery Time (tWRmin)
    uint8_t  MinimumRASToCASDelayTime;       // 18  Minimum RAS# to CAS# Delay Time (tRCDmin)
    uint8_t  MinimumRowToRowDelayTime;       // 19  Minimum Row Active to Row Active Delay Time (tRRDmin)
    uint8_t  MinimumRowPrechargeDelayTime;   // 20  Minimum Row Precharge Delay Time (tRPmin)
    uint8_t  UpperNibblesFortRASAndtRC;      // 21  Upper Nibbles for tRAS and tRC
    uint8_t  tRASmin;                        // 22  Minimum Active to Precharge Delay Time (tRASmin), Least Significant Byte
    uint8_t  tRCmin;                         // 23  Minimum Active to Active/Refresh Delay Time (tRCmin), Least Significant Byte
    uint8_t  tRFCminLeastSignificantByte;    // 24  Minimum Refresh Recovery Delay Time (tRFCmin), Least Significant Byte
    uint8_t  tRFCminMostSignificantByte;     // 25  Minimum Refresh Recovery Delay Time (tRFCmin), Most Significant Byte
    uint8_t  tWTRmin;                        // 26  Minimum Internal Write to Read Command Delay Time (tWTRmin)
    uint8_t  tRTPmin;                        // 27  Minimum Internal Read to Precharge Command Delay Time (tRTPmin)
    uint8_t  UpperNibbleFortFAW;             // 28  Upper Nibble for tFAW
    uint8_t  tFAWmin;                        // 29  Minimum Four Activate Window Delay Time (tFAWmin)
    uint8_t  SDRAMOptionalFeatures;          // 30  SDRAM Optional Features
    uint8_t  SDRAMThermalAndRefreshOptions;  // 31  SDRAMThermalAndRefreshOptions
    uint8_t  ModuleThermalSensor;            // 32  ModuleThermalSensor
    uint8_t  SDRAMDeviceType;                // 33  SDRAM Device Type
    int8_t   tCKminFine;                     // 34  Fine Offset for SDRAM Minimum Cycle Time (tCKmin)
    int8_t   tAAminFine;                     // 35  Fine Offset for Minimum CAS Latency Time (tAAmin)
    int8_t   tRCDminFine;                    // 36  Fine Offset for Minimum RAS# to CAS# Delay Time (tRCDmin)
    int8_t   tRPminFine;                     // 37  Fine Offset for Minimum Row Precharge Delay Time (tRPmin)
    int8_t   tRCminFine;                     // 38  Fine Offset for Minimum Active to Active/Refresh Delay Time (tRCmin)
    uint8_t  ReferenceRawCardUsed;           // 62  Reference Raw Card Used
    uint8_t  AddressMappingEdgeConnector;    // 63  Address Mapping from Edge Connector to DRAM
    uint8_t  ThermalHeatSpreaderSolution;    // 64  ThermalHeatSpreaderSolution
    uint8_t  ModuleManufacturerIdCodeLsb;    // 117 Module Manufacturer ID Code, Least Significant Byte
    uint8_t  ModuleManufacturerIdCodeMsb;    // 118 Module Manufacturer ID Code, Most Significant Byte
    uint8_t  ModuleManufacturingLocation;    // 119 Module Manufacturing Location
    uint8_t  ModuleManufacturingDateYear;    // 120 Module Manufacturing Date Year
    uint8_t  ModuleManufacturingDateWW;      // 121 Module Manufacturing Date creation work week
    uint8_t  ModuleSerialNumberA;            // 122 Module Serial Number A
    uint8_t  ModuleSerialNumberB;            // 123 Module Serial Number B
    uint8_t  ModuleSerialNumberC;            // 124 Module Serial Number C
    uint8_t  ModuleSerialNumberD;            // 125 Module Serial Number D
    uint8_t  CRCA;                           // 126 CRC A
    uint8_t  CRCB;                           // 127 CRC B
} DDR3_SPD;

// Configuration for each memory channel/bank
typedef struct {
    uint32_t  Exists;
    DDR3_SPD  SpdData;
    uint8_t   InitClkPiValue[NUM_IVB_MEM_CLK_FREQUENCIES];
} MEM_BANK_CONFIG;

// Memory configuration
typedef struct {
    MEM_BANK_CONFIG  ChannelABank0;
    MEM_BANK_CONFIG  ChannelABank1;
    MEM_BANK_CONFIG  ChannelBBank0;
    MEM_BANK_CONFIG  ChannelBBank1;
} MEM_CONFIG;

#endif
