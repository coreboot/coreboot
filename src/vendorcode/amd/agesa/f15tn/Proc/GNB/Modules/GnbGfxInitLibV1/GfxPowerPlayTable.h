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
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
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
