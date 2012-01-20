/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 DR PCI tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x10
 * @e \$Revision: 56279 $   @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "Table.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X10_CPUF10PCITABLES_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

//  P C I    T a b l e s
// ----------------------

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F10PciRegisters[] =
{
// Function 0 - HT Config

// F0x68 - Link Transaction Control
// bit[11]   , RespPassPW = 1
// bit[19:17], for 8bit APIC config
// bit[22:21], DsNpReqLmt = 10h
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x68),  // Address
      0x004E0800,                           // regData
      0x006E0800,                           // regMask
    }}
  },
// F0x68 - Link Transaction Control
// For uni-processor systems (that is, single link package processors), single core, and no L3:
// [10, DisFillP] = 1b
// [3, DisWrDwP] = 1b
// [2, DisWrBP] = 1b
// [1, DisRdDwP] = 1b
// [0, DisRdBP] = 1b
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_ALL                           // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_SINGLE_CORE | AMD_PF_SINGLE_LINK) },   // platformFeatures
    {{
      PERFORMANCE_NO_L3_CACHE,
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x68),  // Address
      0x0000040F,                           // regData
      0x0000040F,                           // regMask
    }}
  },
// F0x[E4,C4,A4,84] - Link 0 Control Register
// bit[13] LdtStopTriEn = 1
  {
    HtHostPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HT_HOST_FEATURES_ALL,                 // link feats
      0x04,                                 // Address
      0x00002000,                           // regData
      0x00002000,                           // regMask
    }}
  },
// F0x[E4,C4,A4,84] - Link 0 Control Register
// bit [12] IsocEn = 0 default
  {
    HtHostPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_NFCM | AMD_PF_UMA) },
    {{
      HT_HOST_FEATURES_ALL,                 // link feats
      0x04,                                 // Address
      0x00000000,                           // regData
      0x00001000,                           // regMask
    }}
  },
// F0x[E4,C4,A4,84] - Link 0 Control Register
// bit [12] IsocEn = 1 for Isochronous control flow modes.
  {
    HtHostPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_UMA_IFCM | AMD_PF_IFCM | AMD_PF_IOMMU) },
    {{
      HT_HOST_FEATURES_ALL,                 // link feats
      0x04,                                 // Address
      0x00001000,                           // regData
      0x00001000,                           // regMask
    }}
  },
// F0x[F0,D0,B0,90] - Link Base Channel Buffer Count
// bit[31] LockBc = 1
  {
    HtHostPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HT_HOST_FEATURES_ALL,                 // link feats
      0x10,                                 // Address
      0x80000000,                           // regData
      0x80000000,                           // regMask
    }}
  },
// F0x150 - Link Global Retry Control Register
// bit[18:16] TotalRetryAttempts = 7
// bit[13] HtRetryCrcDatInsDynEn = 1
// bit[12]HtRetryCrcCmdPackDynEn = 1
// bit[11:9] HtRetryCrcDatIns = 4
// bit[8] HtRetryCrcCmdPack = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x150), // Address
      0x00073900,                           // regData
      0x00073F00,                           // regMask
    }}
  },
// F0x16C - Link Global Extended Control Register
// bit[15:13] ForceFullT0 = 0
// bit[5:0] T0Time = 0x14
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x16C), // Address
      0x00000014,                           // regData
      0x0000E03F,                           // regMask
    }}
  },
// F0x16C - Link Global Extended Control Register
// bit[15:13] ForceFullT0 = 6
// bit[5:0] T0Time = 0x26
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_SINGLE_LINK},                   // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x16C), // Address
      0x0000C026,                           // regData
      0x0000E03F,                           // regMask
    }}
  },
// F0x16C - Link Global Extended Control Register
// bit[22:17] FullT0Time = 0x33
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_C3                          // CpuRevision
    },
    {AMD_PF_ALL},                             // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x16C), // Address
      0x00660000,                           // regData
      0x007E0000,                           // regMask
    }}
  },

// Function 1 - Map Init

// Before reading F1x114_x2 or F1x114_x3 software must initialize
// the registers or NB Array MCA errors may occur.  BIOS should
// initialize index 0h of F1x114_x2 and F1x114_x3 to prevent reads
// from F1x114 from generating NB Array MCA errors.
// BKDG Doc #3116 Rev 1.07

// F1x110 - Extended Address Map
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_1, 0x110), // Address
      0x20000000,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
// F1x114 - Extended Address Map
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_1, 0x114), // Address
      0x00000000,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
// F1x110 - Extended Address Map
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_1, 0x110), // Address
      0x30000000,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
// F1x114 - Extended Address Map
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_1, 0x114), // Address
      0x00000000,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },

// F2x1B0 - Extended Memory Controller Configuration Low
// bits[10:8], CohPrefPrbLmt = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_2, 0x1B0),  // Address
      0x00000100,                           // regData
      0x00000700,                           // regMask
    }}
  },

// Function 3 - Misc. Control
// F3x40 - MCA NB Control
//
// bit[8], MstrAbrtEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x40),  // Address
      0x00000100,                           // regData
      0x00000100,                           // regMask
    }}
  },
// F3x44 - MCA NB Configuration
// bit[30]  SyncOnDramAdrParErrEn = 1
// bit[27]  NB MCA to CPU0 Enable = 1
// bit[25]  DisPciCfgCpuErrRsp = 1
// bit[21]  SyncOnErr = 1
// bit[20]  SyncOnWDTEn = 1
// bit[6]   CpuErrDis = 1
// bit[4]   SyncPktPropDis = 1
// bit[3]   SyncPktGenDis = 1
// bit[2]   SyncOnUcEccEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x44),  // Address
      0x4A30005C,                           // regData
      0x4A30005C,                           // regMask
    }}
  },
// F3x80 - ACPI Power State Control
// ACPI FIDVID Change
// bits[0] CpuPrbEn = 0
// bits[1] NbLowPwrEn = 0
// bits[2] NbGateEn = 0
// bits[3] NbCofChg = 0
// bits[4] AltVidEn = 0
// bits[7:5] ClkDivisor = 0
// ACPI State S1
// bits[0] CpuPrbEn = 0
// bits[1] NbLowPwrEn = 1
// bits[2] NbGateEn = 1
// bits[3] NbCofChg = 0
// bits[4] AltVidEn = 0
// bits[7:5] ClkDivisor = 7
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x80),  // Address
      0xE6000000,                           // regData
      0xFFFF0000,                           // regMask
    }}
  },
// F3x80 - ACPI Power State Control
// ACPI FIDVID Change
// bits[0] CpuPrbEn = 1
// bits[1] NbLowPwrEn = 1
// bits[2] NbGateEn = 0
// bits[3] NbCofChg = 1
// bits[4] AltVidEn = 0
// bits[7:5] ClkDivisor = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_C3                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x80),  // Address
      0x000B0000,                           // regData
      0x00FF0000,                           // regMask
    }}
  },
// F3x84 - ACPI Power State Control
// ACPI State S3
// bits[0] CpuPrbEn = 0
// bits[1] NbLowPwrEn = 1
// bits[2] NbGateEn = 1
// bits[3] NbCofChg = 0
// bits[4] AltVidEn = 0
// bits[7:5] ClkDivisor = 7
// ACPI State Throttling
// bits[0] CpuPrbEn = 1
// bits[1] NbLowPwrEn = 0
// bits[2] NbGateEn = 0
// bits[3] NbCofChg = 0
// bits[4] AltVidEn = 0
// bits[7:5] ClkDivisor = 2
// ACPI State S4/S5
// bits[0] CpuPrbEn = 0
// bits[1] NbLowPwrEn = 1
// bits[2] NbGateEn = 1
// bits[3] NbCofChg = 0
// bits[4] AltVidEn = 0
// bits[7:5] ClkDivisor = 7
// ACPI State C1
// bits[0] CpuPrbEn = 0
// bits[1] NbLowPwrEn = 0
// bits[2] NbGateEn = 0
// bits[3] NbCofChg = 0
// bits[4] AltVidEn = 0
// bits[7:5] ClkDivisor = 5
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x84),  // Address
      0x01E641E6,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
// F3x84 - ACPI Power State Control
// ACPI State C1
// bits[0] CpuPrbEn = 0
// bits[1] NbLowPwrEn = 0
// bits[2] NbGateEn = 0
// bits[3] NbCofChg = 0
// bits[4] AltVidEn = 0
// bits[7:5] ClkDivisor = 4
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_C2                          // CpuRevision
    },
    {AMD_PF_SINGLE_CORE},                   // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x84),  // Address
      0x80000000,                           // regData
      0xFF000000,                           // regMask
    }}
  },
// F3x8C - NB Configuration High
// Errata 373, bits[25] DisFastTprWr = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x8C),  // Address
      0x02000000,                           // regData
      0x02000000,                           // regMask
    }}
  },
// F3x8C - NB Configuration High
// Clear errata 373, bits[25] DisFastTprWr = 0
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platform Features
    {{
      PERFORMANCE_L3_CACHE,                 // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x8C),  // Address
      0x00000000,                           // regData
      0x02000000,                           // regMask
    }}
  },
// F3xA0 - Power Control Miscellaneous
// bits[13:11] PllLockTime = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_GT_C0                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA0),  // Address
      0x00000800,                           // regData
      0x00003800,                           // regMask
    }}
  },
// F3xA0 - Power Control Miscellaneous
// bits[9] SviHighFreqSel = 1, if PERFORMANCE_VRM_HIGH_SPEED_ENABLE == TRUE
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_C3                            // CpuRevision
    },
    {AMD_PF_ALL},                             // platformFeatures
    {{
      PERFORMANCE_VRM_HIGH_SPEED_ENABLE,    // PerformanceFeatures
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA0),  // Address
      0x00000200,                           // regData
      0x00000200,                           // regMask
    }}
  },
// F3xA4 - Reported Temperature Control
// bits[12:8] PerStepTimeDn = 15
// bits[7] TmpSlewDnEn = 1
// bits[6:5] TmpMaxDiffUp = 3
// bits[4:0] PerStepTimeUp = 15
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA4),  // Address
      0x00000FEF,                           // regData
      0x00001FFF,                           // regMask
    }}
  },
// F3xD4 - Clock Power Timing Control 0
// bits[11:8] ClkRampHystSel = 1
// bits[30:28] NbClkDiv = 1
// bits[31] NbClkDivApplyAll = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xD4),  // Address
      0xC0010F00,                           // regData
      0xF0030F00,                           // regMask
    }}
  },
// F3xD8 - Clock Power Timing Control 1
// bits[2:0] VSSlamTime = 6
// bits[6:4] VSRampTime = 1
// bits[26:24] ReConDel = 3
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xD8),  // Address
      0x03000016,                           // regData
      0x0F000077,                           // regMask
    }}
  },
// F3xDC - Clock Power Timing Control 2
// bits[14:12] NbsynPtrAdj = 6
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xDC),  // Address
      0x00006000,                           // regData
      0x00007000,                           // regMask
    }}
  },
// F3xDC - Clock Power Timing Control 2
// bits[18:16] CacheFlushOnHaltCtl = 0 to ensure AP cache stability at Early
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_GT_Bx                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xDC),  // Address
      0x00000000,                           // regData
      0x00070000,                           // regMask
    }}
  },
// F3x180 - NB Extended Configuration
// bits[1] SyncFloodOnUsPwDataErr = 1
// bits[5] DisPciCfgCpuMstAbtRsp = 1
// bits[6] SyncFloodOnDatErr = 1
// bits[7] SyncFloodOnTgtAbtErr = 1
// bits[8] SyncOnProtEn = 1
// bits[9] SyncOnUncNbAryEn = 1
// bits[20] SyncFloodOnL3LeakErr = 1
// bits[21] SyncFloodOnCpuLeakErr = 1
// bits[22] SyncFloodOnTblWalkErr = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x180), // Address
      0x007003E2,                           // regData
      0x007003E2,                           // regMask
    }}
  },
// F3x188 - NB Extended Configuration Low Register
// bits[4] EnStpGntOnFlushMaskWakeup = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_C3                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x188), // Address
      0x00000010,                           // regData
      0x00000010,                           // regMask
    }}
  },
// F3x1A0 - L3 Buffer Count Register
// bits[14:12]  L3ToSriReqCBC = 4, 4 or fewer cores with L3 cache is 4.
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_L3_CACHE,
      (CORE_RANGE_0 (COUNT_RANGE_LOW, 4) | COUNT_RANGE_NONE), // 4 or fewer cores.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1A0),     // Address
      0x00004000,                               // regData
      0x00007000,                               // regMask
    }}
  },
// F3x1A0 - L3 Buffer Count Register
// bits[14:12]  L3ToSriReqCBC = 5, 5-core with L3 cache is 5.
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_L3_CACHE,
      (CORE_RANGE_0 (5, 5) | COUNT_RANGE_NONE), // 5 core.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1A0),     // Address
      0x00005000,                               // regData
      0x00007000,                               // regMask
    }}
  },
// F3x1A0 - L3 Buffer Count Register
// bits[14:12]  L3ToSriReqCBC = 6, 6-core with L3 cache is 6.
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_L3_CACHE,
      (CORE_RANGE_0 (6, 6) | COUNT_RANGE_NONE), // 6 core.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1A0),     // Address
      0x00006000,                               // regData
      0x00007000,                               // regMask
    }}
  },
// F3x1B8 - L3 Control
// bits[12] L3PrivReplEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1B8), // Address
      0x00001000,                           // regData
      0x00001000,                           // regMask
    }}
  },
  // F4x1C4 - L3 Power Control Register
  // bits[8] L3PwrSavEn = 1
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_L3_CACHE,                 // Features
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x1C4), // Address
      0x00000100,                           // regData
      0x00000100,                           // regMask
    }}
  },
// F3x1CC - IBS Control
// bits[8] LvtOffsetVal = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_GT_A2                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1CC), // Address
      0x00000100,                           // regData
      0x00000100,                           // regMask
    }}
  }
};

CONST REGISTER_TABLE ROMDATA F10PciRegisterTable = {
  PrimaryCores,
  (sizeof (F10PciRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F10PciRegisters,
};
