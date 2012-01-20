/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Orochi PCI tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/OR
 * @e \$Revision: 59564 $   @e \$Date: 2011-09-26 12:33:51 -0600 (Mon, 26 Sep 2011) $
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
#include "cpuRegisters.h"
#include "Table.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_OR_F15ORPCITABLES_FILECODE


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

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F15OrPciRegisters[] =
{
// F0x68 - Link Transaction Control
// bit[11]   , RespPassPW = 1
// bits[14:13], BufRelPri = 1
// bit[19:17], for 8bit APIC config
// bit[22:21], DsNpReqLmt = 10b
// bit [25] CHtExtAddrEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x68),  // Address
      0x024E2800,                           // regData
      0x026E6800,                           // regMask
    }}
  },
// F0x6C - Link Initialization Control
// bit[23] TxSSBusPwrSaveEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x6C),  // Address
      0x00800000,                           // regData
      0x00800000,                           // regMask
    }}
  },
// F0x[E4,A4,C4,84] Link Control Register
// bit [15] Addr64bitEn = 1
  {
    HtHostPciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
   },
    {AMD_PF_ALL},
    {{
      HT_HOST_FEAT_NONCOHERENT,
      0x4,
      0x00008000,
      0x00008000,
    }}
  },
// F0x[E4,C4,A4,84] - Link 0 Control Register
// bit[13] LdtStopTriEn = 1
  {
    HtHostPciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
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
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
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
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
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
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
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
// bit[11:9] HtRetryCrcDatIns = leave default reset value (erratum #600)
// bit[8] HtRetryCrcCmdPack = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x150), // Address
      0x00073100,                           // regData
      0x00073100,                           // regMask
    }}
  },
// F0x16C - Link Global Extended Control Register
// bit[22:17] FullT0Time = 0x33
// bit[15:13] ForceFullT0 = 7
// bit[7:6] InLnSt = 01b (PHY_OFF)
// bit[5:0] T0Time = 0x26
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x16C), // Address
      0x0066E066,                           // regData
      0x007EE0FF,                           // regMask
    }}
  },
// F0x[18C:170] - Link Extended Control Register - All connected links.
// bit[8] LS2En = 1
  {
    HtLinkPciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platform Features
    {{
      HT_HOST_FEATURES_ALL,
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x170), // Address
      0x00000100,                           // regData
      0x00000100,                           // regMask
    }}
  },
// F2x1B0 - Extended Memory Controller Configuration Low
// bits[10:8], CohPrefPrbLmt = 0
  {
    ProfileFixup,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                             // platformFeatures
    {{
      PERFORMANCE_PROBEFILTER,              // Features
      MAKE_SBDFO (0, 0, 24, FUNC_2, 0x1B0), // Address
      0x00000000,                           // regData
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
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
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
// bit[27]  NB MCA to Master CPU Enable = 1
// bit[25]  DisPciCfgCpuErrRsp = 1
// bit[21]  SyncFloodOnAnyUcErr = 1
// bit[20]  SyncOnWDTEn = 1
// bit[6]   CpuErrDis = 1
// bit[4]   SyncPktPropDis = 0
// bit[3]   SyncPktGenDis = 0
// bit[2]   SyncOnUcEccEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x44),  // Address
      0x4A300044,                           // regData
      0x4A30005C,                           // regMask
    }}
  },
// F3x70 - SRI_to_XBAR Command Buffer Count
// bits[30:28] IsocRspCBC = 1
// bits[26:24] IsocPreqCBC = 0
// bits[22:20] IsocReqCBC = 1
// bits[18:16] UpRspCBC = 7
// bits[14:12] DnPreqCBC = 1
// bits[10:8]  UpPreqCBC = 1
// bits[7:6]   DnRspCBC = 1
// bits[5:4]   DnReqCBC = 1
// bits[2:0]   UpReqCBC = 5
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x70),  // Address
      0x10171155,                           // regData
      0x777777F7,                           // regMask
    }}
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[31:28] DRReqCBC = 0
// bits[26:24] IsocPreqCBC = 0
// bits[23:20] IsocReqCBC = 1
// bits[19:16] ProbeCBC = 7
// bits[14:12] DnPreqCBC = 2
// bits[10:8]  UpPreqCBC = 1
// bits[6:4]   DnReqCBC = 1
// bits[2:0]   UpReqCBC = 1
  {
    ProfileFixup,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_L3_CACHE,                 // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0x00172111,                           // regData
      0xF7FF7777,                           // regMask
    }}
  },
// F3x78 - MCT to XBAR Buffer Count
// bits[12:8]  ProbeCBC = 0Eh
// bits[4:0]   RspCBC = 12h
  {
    ProfileFixup,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,              // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x78),  // Address
      0x00000E12,                           // regData
      0x00001F1F,                           // regMask
    }}
  },
// F3x78 - MCT to XBAR Buffer Count
// bits[12:8]  ProbeCBC = 0Ch
// bits[4:0]   RspCBC = 14h
  {
    ProfileFixup,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_PROBEFILTER,              // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x78),  // Address
      0x00000C14,                           // regData
      0x00001F1F,                           // regMask
    }}
  },
// F3x7C - Free List Buffer Count
// bits[26:23] SrqExtFreeListBC = 8
// bits[22:20] Sri2XbarFreeRspDBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 0xD
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[11:8]  Sri2XbarFreeXreqCBC = 0xF
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x040D0F00,                           // regData
      0x07FFFF00,                           // regMask
    }}
  },
// F3x7C - Free List Buffer Count
// bits[4:0]   Xbar2SriFreeListCBC = 0x16
  {
    ProfileFixup,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_L3_CACHE,                 // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),               // Address
      0x00000016,                           // regData
      0x0000001F,                           // regMask
    }}
  },
// F3x80 - ACPI Power State Control
// ACPI State C2
// bit[0] CpuPrbEn = 1
// bit[1] NbLowPwrEn = 0
// bit[2] NbGateEn = 0
// bits[7:5] ClkDivisor = 4
// ACPI State C3, C1E or Link init
// bit[0] CpuPrbEn = 0
// bit[1] NbLowPwrEn = 1
// bit[2] NbGateEn = 0
// bit[3] NbCofChg = 0
// bit[4] Reserved = 0
// bits[7:5] ClkDivisor = 7
// NB P-state changes
// bit[0] CpuPrbEn = 1
// bit[1] NbLowPwrEn = 1
// bit[2] NbGateEn = 0
// bit[3] NbCofChg = 1
// bit[4] Reserved = 0
// bits[7:5] ClkDivisor = 0
// S1
// bit[0] CpuPrbEn = 0
// bit[1] NbLowPwrEn = 1
// bit[2] NbGateEn = 0
// bit[3] NbCofChg = 0
// bit[4] Reserved = 0
// bits[7:5] ClkDivisor = 7
  {
    PciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                             // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x80),  // Address
      0xE20BE281,                           // regData
      0xFFFFFFE7,                           // regMask
    }}
  },
// F3x84 - ACPI Power State Control
// ACPI State S3
// bit[0] CpuPrbEn = 0
// bit[1] NbLowPwrEn = 1
// bit[2] NbGateEn = 0
// bit[3] NbCofChg = 0
// bit[4] Reserved = 0
// bits[7:5] ClkDivisor = 7
// ACPI State S4/S5
// bit[0] CpuPrbEn = 0
// bit[1] NbLowPwrEn = 1
// bit[2] NbGateEn = 0
// bit[3] NbCofChg = 0
// bit[4] Reserved = 0
// bits[7:5] ClkDivisor = 7
// ACPI State C1
// bit[0] CpuPrbEn = 0
// bit[1] NbLowPwrEn = 0
// bit[2] NbGateEn = 0
// bit[3] NbCofChg = 0
// bit[4] Reserved = 0
// bits[7:5] ClkDivisor = 7
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x84),  // Address
      0xE0E200E2,                           // regData
      0xEFFF00FF,                           // regMask
    }}
  },
// F3x84 - ACPI Power State Control
// ACPI State C1
// bits[0] CpuPrbEn = 0
// bits[1] NbLowPwrEn = 0
// bits[2] NbGateEn = 0
// bits[7:5] ClkDivisor = 4
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_SINGLE_CORE},                   // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x84),  // Address
      0x80000000,                           // regData
      0xE7000000,                           // regMask
    }}
  },
// F3x90 - GART Aperture Control
// bit[6] = DisGartTblWlkPrb, Erratum 540
  {
    PciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                             // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x90),  // Address
      0x00000040,                           // regData
      0x00000040,                           // regMask
    }}
  },
// F3xA0 - Power Control Miscellaneous
// bit[9] SviHighFreqSel = 1, if PERFORMANCE_VRM_HIGH_SPEED_ENABLE == TRUE
  {
    ProfileFixup,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                             // platformFeatures
    {{
      PERFORMANCE_VRM_HIGH_SPEED_ENABLE,    // PerformanceFeatures
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA0),  // Address
      0x00000200,                           // regData
      0x00000200,                           // regMask
    }}
  },
// F3xD4 - Clock Power Timing Control 0
// bits[11:8] ClkRampHystSel = 0xF
// bits[15] StutterScrubEn = 0
// bits[14] CacheFlushImmOnAllHalt = 0
// bits[13] MTC1eEn = 0
// bits[17:16] LnkPllLock = 1
// bits[30:28] NbClkDiv = 4
// bits[31] NbClkDivApplyAll = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xD4),  // Address
      0xC0010F00,                           // regData
      0xF003EF00,                           // regMask
    }}
  },
// F3xD8 - Clock Power Timing Control 1
// bits[6:4]   VSRampSlamTime = 1
// bits[27:24] ReConDel = 3
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xD8),  // Address
      0x03000010,                           // regData
      0x0F000070,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[1:0]   UpReqTok = 1
// bits[3:2]   DnReqTok = 1
// bits[5:4]   UpPreqTok = 1
// bits[7:6]   DnPreqTok = 1
// bits[11:10] DnRspTok = 1
// bits[13:12] IsocReqTok = 1
// bits[15:14] IsocPreqTok = 0
// bits[17:16] IsocRspTok = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platform Features
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00011455,                           // regData
      0x0003FCFF,                           // regMask
    }}
  },
// F3x144 - MCT to XCS Token Count
// bits[3:0] RspTok = 5
// bits[7:4] ProbeTok = 5
  {
    ProfileFixup,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x144), // Address
      0x00000055,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// F3x144 - MCT to XCS Token Count
// bits[3:0] RspTok = 8
// bits[7:4] ProbeTok = 2
  {
    ProfileFixup,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_PROBEFILTER,              // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x144), // Address
      0x00000028,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// F3x160 - NB Machine Check Misc 0
// bits[23:20] LvtOffset = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x160), // Address
      0x00100000,                           // regData
      0x00F00000,                           // regMask
    }}
  },
// F3x168 - NB Machine Check Misc 1
// bits[23:20] LvtOffset = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x168), // Address
      0x00100000,                           // regData
      0x00F00000,                           // regMask
    }}
  },
// F3x170 - NB Machine Check Misc 2
// bits[23:20] LvtOffset = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x170), // Address
      0x00100000,                           // regData
      0x00F00000,                           // regMask
    }}
  },
// F3x180 - NB Extended Configuration
// bit[1]  SyncFloodOnUsPwDatErr = 1
// bit[5]  DisPciCfgCpuMstAbtRsp = 1
// bit[6]  SyncFloodOnDatErr = 1
// bit[7]  SyncFloodOnTgtAbtErr = 1
// bit[8]  SyncFloodOnHtProtEn = 1
// bit[9]  SyncOnUCNbAryEn = 1
// bit[20] SyncFloodOnL3LeakErr = 1
// bit[21] SyncFloodOnCpuLeakErr = 1
// bit[22] SyncFloodOnTblWalkErr = 1
// bit[24] McaLogErrAddrWdtErr = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x180), // Address
      0x017003E2,                           // regData
      0x017003E2,                           // regMask
    }}
  },
// F3x188 - NB Configuration 2 Register
// bit[9]  DisL3HiPriFreeListAlloc = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                             // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x188), // Address
      0x00000200,                           // regData
      0x00000200,                           // regMask
    }}
  },
// F3x1A0 - L3 Buffer Count
// bits[17:16] CpuToNbFreeBufCnt = 3
  {
    ProfileFixup,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_L3_CACHE,                 // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1A0), // Address
      0x00030000,                           // regData
      0x00030000,                           // regMask
    }}
  },
// F3x1B8 - L3 Control 1
// bit[12] L3PrivReplEn = 1
// bit[18] Reserved = 1, Erratum #504
  {
    ProfileFixup,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_L3_CACHE,                 // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1B8), // Address
      0x00041000,                           // regData
      0x00041000,                           // regMask
    }}
  },
// F3x1E4 - SBI Control
// bits[11:8] LvtOffset = 3
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1E4), // Address
      0x00000300,                           // regData
      0x00000F00,                           // regMask
    }}
  },
// F4x104 - TDP Accumulator Divisor Control
// bits[1:0] TdpAccDivVal = 1
// bits[13:2] TdpAccDivRate = 0x0C8
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x104), // Address
      0x00000321,                           // regData
      0x00003FFF,                           // regMask
    }}
  },
// F4x110 - Sample and Residency Timer
// bits[11:0] CSampleTimer = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x110), // Address
      0x00000001,                           // regData
      0x00000FFF,                           // regMask
    }}
  },
// F4x118 - C-state Control 1
// bit [0]     CpuPrbEnCstAct0 = 0
// bit [1]     CacheFlushEnCstAct0 = 0
// bits[3:2]   CacheFlushTmrSelCstAct0 = 0
// bits[7:5]   ClkDivisorCstAct0 = 0
// bit [8]     PwrGateEnCstAct0 = 0
// bit [16]    CpuPrbEnCstAct1 = 0
// bit [17]    CacheFlushEnCstAct1 = 0
// bits[19:18] CacheFlushTmrSelCstAct1 = 0
// bits[23:21] ClkDivisorCstAct1 = 0
// bit [24]    PwrGateEnCstAct1 = 0
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x118), // Address
      0x00000000,                           // regData
      0x01EF01EF,                           // regMask
    }}
  },
// F4x11C - C-state Control 2
// bit [0]     CpuPrbEnCstAct2 = 0
// bit [1]     CacheFlushEnCstAct2 = 0
// bits[3:2]   CacheFlushTmrSelCstAct2 = 0
// bits[7:5]   ClkDivisorCstAct2 = 0
// bit [8]     PwrGateEnCstAct2 = 0
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x11C), // Address
      0x00000000,                           // regData
      0x000001EF,                           // regMask
    }}
  },
// F4x128 - C-state Policy Control 1
// bits[20:18] CacheFlushSucMonThreshold = 4
// bits[11:5]  CacheFlushTmr = 0x28
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x128), // Address
      0x00100500,                           // regData
      0x001C0FE0,                           // regMask
    }}
  },
// F4x16C - APM TDP Control
// bit[4] ApmTdpLimitIntEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x16C), // Address
      0x00000010,                           // regData
      0x00000010,                           // regMask
    }}
  },
// F4x1C4 - L3 Power Control Register
// bits[8] L3PwrSavEn = 1
  {
    ProfileFixup,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_L3_CACHE,                 // Features
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x1C4), // Address
      0x00000100,                           // regData
      0x00000100,                           // regMask
    }}
  },
// F4x1CC - L3 Control 2
// bit[4]    ImplRdAnySubUnavail = 1
// bits[8:6] ImplRdProjDelayThresh = 2
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x1CC), // Address
      0x00000090,                           // regData
      0x000001D0,                           // regMask
    }}
  },
// F5x88 - Northbridge Configuration 4
// bit[5] Reserved, BIOS must set
  {
    ProfileFixup,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      0x04,                                 // Features
      MAKE_SBDFO (0, 0, 24, FUNC_5, 0x88),  // Address
      0x00000020,                           // regData
      0x00000020,                           // regMask
    }}
  },
// F5x88 - Northbridge Configuration 4
// bit[14] Reserved, BIOS must set
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_Bx                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_5, 0x88),  // Address
      0x00004000,                           // regData
      0x00004000,                           // regMask
    }}
  },
// F5xE0 - Processor TDP Running Average
// bits[3:0] RunAvgRange = 0xE
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_5, 0xE0), // Address
      0x0000000E,                           // regData
      0x0000000F,                           // regMask
    }}
  }
};

CONST REGISTER_TABLE ROMDATA F15OrPciRegisterTable = {
  PrimaryCores,
  (sizeof (F15OrPciRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F15OrPciRegisters,
};

