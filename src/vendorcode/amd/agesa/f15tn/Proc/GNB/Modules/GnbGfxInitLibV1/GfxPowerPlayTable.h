/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to initialize Power Play Table
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* ***************************************************************************
*
*/
#ifndef _GFXPOWERPLAYTABLE_H_
#define _GFXPOWERPLAYTABLE_H_

#pragma pack (push, 1)

#define POLICY_LABEL_BATTERY                                0x1
#define POLICY_LABEL_PERFORMANCE                            0x2

#define MAX_NUM_OF_SW_STATES                                10
#define MAX_NUM_OF_DPM_STATES                               10
#define MAX_NUM_OF_VCE_CLK_STATES                           5
#define MAX_NUM_OF_VCE_STATES                               6
#define MAX_NUM_OF_FUSED_DPM_STATES                         5
#define MAX_NUM_OF_FUSED_SW_STATES                          6
/// ATOM_PPLIB_POWERPLAYTABLE::ulPlatformCaps
#define ATOM_PP_PLATFORM_CAP_BACKBIAS                       1
#define ATOM_PP_PLATFORM_CAP_POWERPLAY                      2
#define ATOM_PP_PLATFORM_CAP_SBIOSPOWERSOURCE               4
#define ATOM_PP_PLATFORM_CAP_ASPM_L0s                       8
#define ATOM_PP_PLATFORM_CAP_ASPM_L1                        16
#define ATOM_PP_PLATFORM_CAP_HARDWAREDC                     32
#define ATOM_PP_PLATFORM_CAP_GEMINIPRIMARY                  64
#define ATOM_PP_PLATFORM_CAP_STEPVDDC                       128
#define ATOM_PP_PLATFORM_CAP_VOLTAGECONTROL                 256
#define ATOM_PP_PLATFORM_CAP_SIDEPORTCONTROL                512
#define ATOM_PP_PLATFORM_CAP_TURNOFFPLL_ASPML1              1024
#define ATOM_PP_PLATFORM_CAP_HTLINKCONTROL                  2048
#define ATOM_PP_PLATFORM_CAP_MVDDCONTROL                    4096
#define ATOM_PP_PLATFORM_CAP_GOTO_BOOT_ON_ALERT             0x2000        // Go to boot state on alerts, e.g. on an AC->DC transition.
#define ATOM_PP_PLATFORM_CAP_DONT_WAIT_FOR_VBLANK_ON_ALERT  0x4000        // Do NOT wait for VBLANK during an alert (e.g. AC->DC transition).
#define ATOM_PP_PLATFORM_CAP_VDDCI_CONTROL                  0x8000        // Does
#define ATOM_PP_PLATFORM_CAP_REGULATOR_HOT                  0x00010000ul  // Enable the 'regulator hot' feature.
#define ATOM_PP_PLATFORM_CAP_BACO                           0x00020000ul  // Does the driver supports BACO state.


#define ATOM_PPLIB_CLASSIFICATION_UI_BATTERY                1
#define ATOM_PPLIB_CLASSIFICATION_UI_BALANCED               3
#define ATOM_PPLIB_CLASSIFICATION_UI_PERFORMANCE            5

#define ATOM_PPLIB_CLASSIFICATION_BOOT                      0x0008
#define ATOM_PPLIB_CLASSIFICATION_THERMAL                   0x0010
#define ATOM_PPLIB_CLASSIFICATION_LIMITEDPOWERSOURCE        0x0020
#define ATOM_PPLIB_CLASSIFICATION_REST                      0x0040
#define ATOM_PPLIB_CLASSIFICATION_FORCED                    0x0080
#define ATOM_PPLIB_CLASSIFICATION_3DPERFORMANCE             0x0100
#define ATOM_PPLIB_CLASSIFICATION_OVERDRIVETEMPLATE         0x0200
#define ATOM_PPLIB_CLASSIFICATION_UVDSTATE                  0x0400
#define ATOM_PPLIB_CLASSIFICATION_3DLOW                     0x0800
#define ATOM_PPLIB_CLASSIFICATION_ACPI                      0x1000
#define ATOM_PPLIB_CLASSIFICATION_HD2STATE                  0x2000
#define ATOM_PPLIB_CLASSIFICATION_HDSTATE                   0x4000
#define ATOM_PPLIB_CLASSIFICATION_SDSTATE                   0x8000
#define ATOM_PPLIB_CLASSIFICATION_NONUVDSTATE               0x0000

#define ATOM_PPLIB_CLASSIFICATION2_MVC                      0x0004       //Multi-View

#define ATOM_PPLIB_ENABLE_VARIBRIGHT                        0x00008000ul
#define ATOM_PPLIB_ENABLE_DRR                               0x00080000ul

#define ATOM_PP_FANPARAMETERS_NOFAN                         0x80
#define ATOM_PP_THERMALCONTROLLER_SUMO                      0x0E

/// DPM state info
typedef struct _ATOM_PPLIB_SUMO_CLOCK_INFO {
  USHORT                      usEngineClockLow;             ///< Sclk [15:0] (Sclk in 10khz)
  UCHAR                       ucEngineClockHigh;            ///< Sclk [23:16](Sclk in 10khz)
  UCHAR                       vddcIndex;                    ///< 2-bit VDDC index;
  USHORT                      tdpLimit;                     ///< TDP Limit
  USHORT                      rsv1;                         ///< Reserved
  ULONG                       rsv2[2];                      ///< Reserved
} ATOM_PPLIB_SUMO_CLOCK_INFO;

/// Non clock info
typedef struct _ATOM_PPLIB_NONCLOCK_INFO {
  USHORT                      usClassification;             ///< State classification see ATOM_PPLIB_CLASSIFICATION_*
  UCHAR                       ucMinTemperature;             ///< Reserved
  UCHAR                       ucMaxTemperature;             ///< Reserved
  ULONG                       ulCapsAndSettings;            ///< Capability Setting (ATOM_PPLIB_ENABLE_DRR or ATOM_PPLIB_ENABLE_VARIBRIGHT or 0)
  UCHAR                       ucRequiredPower;              ///< Reserved
  USHORT                      usClassification2;            ///< Reserved
  ULONG                       ulVCLK;                       ///< UVD clocks VCLK unit is in 10KHz
  ULONG                       ulDCLK;                       ///< UVD clocks DCLK unit is in 10KHz
  UCHAR                       ucUnused[5];                  ///< Reserved
} ATOM_PPLIB_NONCLOCK_INFO;

/// Thermal controller info stub
typedef struct _ATOM_PPLIB_THERMALCONTROLLER {
  UCHAR                       ucType;                       ///< Reserved. Should be set 0xE
  UCHAR                       ucI2cLine;                    ///< Reserved. Should be set 0
  UCHAR                       ucI2cAddress;                 ///< Reserved. Should be set 0
  UCHAR                       ucFanParameters;              ///< Reserved. Should be set 0x80
  UCHAR                       ucFanMinRPM;                  ///< Reserved. Should be set 0
  UCHAR                       ucFanMaxRPM;                  ///< Reserved. Should be set 0
  UCHAR                       ucReserved;                   ///< Reserved. Should be set 0
  UCHAR                       ucFlags;                      ///< Reserved. Should be set 0
} ATOM_PPLIB_THERMALCONTROLLER;

/// SW state info
typedef struct _ATOM_PPLIB_STATE_V2 {
  UCHAR                       ucNumDPMLevels;               ///< Number of valid DPM levels in this state
  UCHAR                       nonClockInfoIndex;            ///< Index to the array of NonClockInfos
  UCHAR                       ClockInfoIndex[1];            ///< Array of DPM states. Actual number calculated during state enumeration
} ATOM_PPLIB_STATE_V2;

/// SW state Array
typedef struct {
  UCHAR                       ucNumEntries;                 ///< Number of SW states
  ATOM_PPLIB_STATE_V2         States[1];                    ///< SW state info. Actual number calculated during state enumeration
} STATE_ARRAY;

/// Clock info Array
typedef struct {
  UCHAR                       ucNumEntries;                 ///< Number of ClockInfo entries
  UCHAR                       ucEntrySize;                  ///< size of ATOM_PPLIB_SUMO_CLOCK_INFO
  ATOM_PPLIB_SUMO_CLOCK_INFO  ClockInfo[1];                 ///< Clock info array. Size will be determined dynamically base on fuses
} CLOCK_INFO_ARRAY;

/// Non clock info Array
typedef struct {

  UCHAR                       ucNumEntries;                 ///< Number of Entries;
  UCHAR                       ucEntrySize;                  ///< Size of NonClockInfo
  ATOM_PPLIB_NONCLOCK_INFO    NonClockInfo[1];              ///< Non clock info array
} NON_CLOCK_INFO_ARRAY;

/// VCE clock info
typedef struct {
  USHORT                    usEVClkLow;                   ///< EVCLK low
  UCHAR                     ucEVClkHigh;                  ///< EVCLK high
  USHORT                    usECClkLow;                   ///< ECCLK low
  UCHAR                     ucECClkHigh;                  ///< ECCLK high
} VCECLOCKINFO;

/// VCE clock info array
typedef struct {
  UCHAR                     ucNumEntries;                ///< Number of entries
  VCECLOCKINFO              entries[1];                  ///< VCE clock arrau
} VCECLOCKINFOARRAY;

/// VCE voltage limit record
typedef struct {
  USHORT                    usVoltage;                    ///< Voltage index
  UCHAR                     ucVCEClockInfoIndex;          ///< Index of VCE clock state
} ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_RECORD;

/// VCE voltage limit table
typedef struct {
  UCHAR                                     numEntries;   ///< Number of entries
  ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_RECORD entries[1];   ///< Coltage limit state array
} ATOM_PPLIB_VCE_CLOCK_VOLTAGE_LIMIT_TABLE;

/// VCE state record
typedef struct {
  UCHAR                     ucVCEClockInfoIndex;          ///< Index of VCE clock state
  UCHAR                     ucClockInfoIndex;             ///< Index of SCLK clock state
} ATOM_PPLIB_VCE_STATE_RECORD;

/// VCE state table
typedef struct {
  UCHAR                       numEntries;                 ///< Number of state entries
  ATOM_PPLIB_VCE_STATE_RECORD entries[1];                 ///< State entries
} ATOM_PPLIB_VCE_STATE_TABLE;

/// Extended header
typedef struct {
  USHORT                      usSize;                     ///< size of header
  ULONG                       rsv15;                      ///< reserved
  ULONG                       rsv16;                      ///< reserved
  USHORT                      usVCETableOffset;           ///< offset of ATOM_PPLIB_VCE_TABLE
} ATOM_PPLIB_EXTENDEDHEADER;

/// VCE table
typedef struct {
  UCHAR                       revid;                      ///< revision ID
} ATOM_PPLIB_VCE_TABLE;

/// Power Play table
typedef struct _ATOM_PPLIB_POWERPLAYTABLE3 {
  ATOM_COMMON_TABLE_HEADER      sHeader;                    ///< Common header
  UCHAR                         ucDataRevision;             ///< Revision of PP table
  UCHAR                         Reserved1[4];               ///< Reserved
  USHORT                        usStateArrayOffset;         ///< Offset from start of this table to array of ucNumStates ATOM_PPLIB_STATE structures
  USHORT                        usClockInfoArrayOffset;     ///< Offset from start of the table to ClockInfoArray
  USHORT                        usNonClockInfoArrayOffset;  ///< Offset from Start of the table to NonClockInfoArray
  USHORT                        Reserved2[2];               ///< Reserved
  USHORT                        usTableSize;                ///< the size of this structure, or the extended structure
  ULONG                         ulPlatformCaps;             ///< See ATOM_PPLIB_CAPS_*
  ATOM_PPLIB_THERMALCONTROLLER  sThermalController;         ///< Thermal controller stub.
  USHORT                        Reserved4[2];               ///< Reserved
  UCHAR                         Reserved5;                  ///< Reserved
  USHORT                        Reserved6;                  ///< Reserved
  USHORT                        usFormatID;                 ///< Format ID
  USHORT                        Reserved7[1];               ///< Reserved
  USHORT                        usExtendendedHeaderOffset;  ///< Extended header offset
} ATOM_PPLIB_POWERPLAYTABLE3;

#pragma pack (pop)


AGESA_STATUS
GfxPowerPlayBuildTable (
     OUT  VOID                          *Buffer,
  IN      GFX_PLATFORM_CONFIG           *Gfx
  );


#endif
