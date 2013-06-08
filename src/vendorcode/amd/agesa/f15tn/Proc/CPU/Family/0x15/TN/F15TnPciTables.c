/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Trinity PCI tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/TN
 * @e \$Revision: 64462 $   @e \$Date: 2012-01-21 10:59:15 -0600 (Sat, 21 Jan 2012) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuRegisters.h"
#include "cpuF15TnPowerMgmt.h"
#include "Table.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_TN_F15TNPCITABLES_FILECODE


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

VOID
STATIC
SetEnCstateBoostBlockCC6Exit (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
Erratum687Workaround (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

//  P C I    T a b l e s
// ----------------------

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F15TnPciRegisters[] =
{
// F0x68 - Link Transaction Control
// bits[22:21] DsNpReqLmt = 01b
// bit [19]    ApicExtSpur = 1
// bit [18]    ApicExtId = 1
// bit [17]    ApicExtBrdCst = 1
// bit [15]    LimitCldtCfg = 1
// bit [10]    DisFillP = 0
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x68),  // Address
      0x002E8000,                           // regData
      0x006E8400,                           // regMask
    }}
  },
// F0x6C - Link Initialization Control
// bit[0] RouteTblDis = 0
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x6C),  // Address
      0x00000000,                           // regData
      0x00000001,                           // regMask
    }}
  },
// F0x84 - Link Control
// bit [12] IsocEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x84),  // Address
      0x00001000,                           // regData
      0x00001000,                           // regMask
    }}
  },
// F0x90 - Upstream Base Channel Buffer Count
// bits[27:25] FreeData = 0
// bits[24:20] FreeCmd = 0
// bits[19:18] RspData = 1
// bits[17:16] NpReqData = 1
// bits[15:12] ProbeCmd = 0
// bits[11:8]  RspCmd = 2
// bits[7:5]   PReq = 5
// bits[4:0]   NpReqCmd = 8
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x90),  // Address
      0x000502A8,                           // regData
      0x0FFFFFFF,                           // regMask
    }}
  },
// F0x94 - Link Isochronous Channel Buffer Count
// bits[28:27] IsocRspData = 0
// bits[26:25] IsocNpReqData = 1
// bits[24:22] IsocRspCmd = 0
// bits[21:19] IsocpReq = 0
// bits[18:16] IsocNpReqCmd = 1
// bits[15:8]  SecBusNum = 0 (F1XE0 [BaseBusNum])
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x94),  // Address
      0x02010000,                           // regData
      0x1FFFFF00,                           // regMask
    }}
  },
// F1xE0 - Configuration Map
// bits[31:24] BusNumLimit = F8
// bits[23:16] BaseBusNum = 0
// bit [1]     WE = 1
// bit [0]     RE = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
        MAKE_SBDFO (0, 0, 24, FUNC_1, 0xE0),// Address
        0xF8000003,                         // regData
        0xFFFF0003,                         // regMask
    }}
  },
// F3x44 - MCA NB Configuration
//
// bit[30] SyncFloodOnDramAdrParErr = 1
// bit[27] NbMcaToMstCpuEn = 1
// bit[21] SyncFloodOnAnyUcErr = 1
// bit[20] SyncFloodOnWDT = 1

  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x44),  // Address
      0x48300000,                           // regData
      0x48300000,                           // regMask
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
// bits[2:0]   UpReqCBC = 7
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x70),  // Address
      0x10171157,                           // regData
      0x777777F7,                           // regMask
    }}
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[31:28] DRReqCBC = 0
// bits[26:24] IsocPreqCBC = 1
// bits[23:20] IsocReqCBC = 1
// bits[19:16] ProbeCBC = 8
// bits[14:12] DnPreqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[2:0]   UpReqCBC = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0x01180101,                           // regData
      0xF7FF7777,                           // regMask
    }}
  },
// F3x7C - Free List Buffer Count
// bits[26:23] ExtSrqFreeList = 8
// bits[22:20] Sri2XbarFreeRspDBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 5
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[11:8]  Sri2XbarFreeXreqCBC = 0xE
// bits[4:0]   Xbar2SriFreeListCBC = 18h
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x04050E18,                           // regData
      0x07FFFF1F,                           // regMask
    }}
  },
// F3x84 - ACPI Power State Control High
// ACPI State S3
// bit[1] NbLowPwrEnSmafAct4 = 1
// bit[7:5] ClkDivisorSmafAct4 = 7
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x84),  // Address
      0x000000E2,                           // regData
      0x000000E2,                           // regMask
    }}
  },
// F3xA0 - Power Control Miscellaneous
// bit[14] Svi2HighFreqSel = 1, if PERFORMANCE_VRM_HIGH_SPEED_ENABLE == TRUE
  {
    ProfileFixup,
    {
      AMD_FAMILY_15_TN,                     // CpuFamily
      AMD_F15_TN_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_VRM_HIGH_SPEED_ENABLE,    // PerformanceFeatures
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA0),  // Address
      0x00004000,                           // regData
      0x00004000,                           // regMask
    }}
  },
// F3xD4 - Clock Power Timing Control 0
// bit [31]    NbClkDivApplyAll = 1
// bits[30:28] NbClkDiv = 4
// bits[27:24] PowerStepUp = 8
// bits[23:20] PowerStepDown = 8
// bit [14]    CacheFlushImmOnAllHalt = 0
// bit [12]    ClkRampHystCtl = 0
// bits[11:8]  ClkRampHystSel = 0xF
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xD4),  // Address
      0xC8800F00,                           // regData
      0xFFF05F00,                           // regMask
    }}
  },
// F3xD8 - Clock Power Timing Control 1
// bits[6:4]   VSRampSlamTime = 100b
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xD8),  // Address
      0x00000040,                           // regData
      0x00000070,                           // regMask
    }}
  },
// F3xDC - Clock Power Timing Control 2
// bits[14:12] NbsynPtrAdj = 5
  {
    PciRegister,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                      // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xDC),  // Address
      0x00005000,                           // regData
      0x00007000,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[23:20] FreeTok = 0xA
// bits[17:16] IsocRspTok = 1
// bits[15:14] IsocPreqTok = 0
// bits[13:12] IsocReqTok = 1
// bits[11:10] DnRspTok = 1
// bits[9:8]   UpRspTok = 1
// bits[7:6]   DnPreqTok = 1
// bits[5:4]   UpPreqTok = 1
// bits[3:2]   DnReqTok = 1
// bits[1:0]   UpReqTok = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platform Features
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00A11555,                           // regData
      0x00F3FFFF,                           // regMask
    }}
  },
// F3x144 - MCT_to_XCS Token Count
// bits[7:4] ProbeTok = 7
// bits[3:0] RspTok = 7
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platform Features
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x144), // Address
      0x00000077,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// F3x148 - Link_to_XCS Token Count
// bits[31:30] FreeTok[3:2] = FreeTok[1:0] = 0
// bit [28]    IsocRspTok1 = 0
// bit [26]    IsocPreqTok1 = 0
// bit [24]    IsocReqTok1 = 0
// bits[23:22] ProbeTok1 = 0
// bits[21:20] RspTok1 = 0
// bits[19:18] PReqTok1 = 0
// bits[17:16] ReqTok1 = 0
// bits[15:14] FreeTok[1:0] = 0
// bits[13:12] IsocRspTok0 = 0
// bits[11:10] IsocPreqTok0 = 1
// bits[9:8]   IsocReqTok0 = 1
// bits[7:6]   ProbeTok0 = 0
// bits[5:4]   RspTok0 = 2
// bits[3:2]   PReqTok0 = 2
// bits[1:0]   ReqTok0 = 2
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platform Features
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x0000052A,                           // regData
      0xD5FFFFFF                            // regMask
    }}
  },
// F3x17C - Extended Freelist Buffer Count
// bits[3:0] SPQPrbFreeCBC = 4
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platform Features
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x17C), // Address
      0x00000004,                           // regData
      0x0000000F                            // regMask
    }}
  },
// F3x180 - NB Extended Configuration
// bit[24] McaLogErrAddrWdtErr = 1
// bit[22] SyncFloodOnTblWalkErr = 1
// bit[21] SyncFloodOnCpuLeakErr = 1
// bit[20] SyncFloodOnL3LeakErr = 1
// bit[9]  SyncFloodOnUCNbAry = 1
// bit[8]  SyncFloodOnHtProt = 1
// bit[7]  SyncFloodOnTgtAbortErr = 1
// bit[6]  SyncFloodOnDatErr = 1
// bit[5]  DisPciCfgCpuMstAbortRsp = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x180), // Address
      0x017003E0,                           // regData
      0x017003E0,                           // regMask
    }}
  },
// F3x1A0 - Core to NB Buffer Count
// bit[17:16] CpuToNbFreeBufCnt = 3
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1A0), // Address
      0x00030000,                           // regData
      0x00030000,                           // regMask
    }}
  },
// F4x110 - Sample and Residency Timer
// bits[11:0] CSampleTimer = 2
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x110), // Address
      0x00000002,                           // regData
      0x00000FFF,                           // regMask
    }}
  },
// F4x124 - C-state Interrupt Control
// bits[26:23] IntMonPC6Limit = 0
// bit [22]    IntMonPC6En = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x124), // Address
      0x00400000,                           // regData
      0x07C00000,                           // regMask
    }}
  },
// F4x16C - Erratum #667
// bit [1] = 1
// bit [4] = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x16C), // Address
      0x00000012,                           // regData
      0x00000012,                           // regMask
    }}
  },
// F5xAC - Erratum #667
// bit [3] = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_5, 0xAC),  // Address
      0x00000008,                           // regData
      0x00000008,                           // regMask
    }}
  },
// F5x88 - Northbridge Configuration 4
// bit[24] DisHbNpReqBusLock   = 1
// bit[2]  IntStpClkHaltExitEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_5, 0x88),  // Address
      0x01000004,                           // regData
      0x01000004,                           // regMask
    }}
  },
// F5xE0 - Processor TDP Running Average
// bits[3:0] RunAvgRange = 0x2
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_5, 0xE0),  // Address
      0x00000002,                           // regData
      0x0000000F,                           // regMask
    }}
  },
// F5x128 - Clock Power/Timing Control 3
// bits[13:12] PwrGateTmr = 1
// bits[11:10] PllVddOutUpTime = 3
// bit [9]     FastSlamTimeDown = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_5, 0x128), // Address
      0x00001E00,                           // regData
      0x00003E00,                           // regMask
    }}
  },
// F5x12C - Clock Power/Timing Control 4
// bit [5] CorePsi1En = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_5, 0x12C), // Address
      0x00000020,                           // regData
      0x00000020,                           // regMask
    }}
  },
// F5x178 - Northbridge Fusion Configuration
// bit [18] CstateFusionHsDis = 1
// bit [17] Dis2ndGnbAllowPsWait = 1
// bit [11] AllowSelfRefrS3Dis = 1
// bit [10] InbWakeS3Dis = 1
// bit [2]  CstateFusionDis = 1
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_5, 0x178), // Address
      0x00060C04,                           // regData
      0x00060C04,                           // regMask
    }}
  },
// F0x90 - Upstream Base Channel Buffer Count
// bit [31] LockBc = 1
//
// NOTE: The entry is intended to be programmed after other bits of D18F0x[90, 94] is programmed and before D18F0x6C[30] is programmed.
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x90),  // Address
      0x80000000,                           // regData
      0x80000000,                           // regMask
    }}
  },
// F0x6C - Link Initialization Control
// bit [30] RlsLnkFullTokCntImm = 1
// bit [28] RlsIntFullTokCntImm = 1
//
// NOTE: The entry is intended to be after D18F0x[90, 94] and D18F0x[70, 74, 78, 7C, 140, 144, 148, 17C, 1A0] are programmed.
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x6C),  // Address
      0x50000000,                           // regData
      0x50000000,                           // regMask
    }}
  },
// F0x6C - Link Initialization Control
// bit [27] ApplyIsocModeEnNow = 1
//
// NOTE: The entry is intended to be after D18F0x6C[30, 28] are programmed.
  {
    PciRegister,
    {
      AMD_FAMILY_15_TN,                   // CpuFamily
      AMD_F15_TN_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x6C),  // Address
      0x08000000,                           // regData
      0x08000000,                           // regMask
    }}
  },
};


// PCI with Special Programming Requirements Table

STATIC CONST FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_INITIALIZER ROMDATA F15TnPciWorkarounds[] =
{
// D18F5x88
  {
    FamSpecificWorkaround,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                        // CpuFamily
      AMD_F15_TN_GT_A0                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      SetEnCstateBoostBlockCC6Exit,         // function call
      0x00000000,                           // data
    }}
  },
// D18F5x88 and D18F2x408
  {
    FamSpecificWorkaround,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                        // CpuFamily
      AMD_F15_TN_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      Erratum687Workaround,                 // function call
      0x00000000,                           // data
    }}
  },
};


CONST REGISTER_TABLE ROMDATA F15TnPciRegisterTable = {
  PrimaryCores,
  (sizeof (F15TnPciRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F15TnPciRegisters,
};


CONST REGISTER_TABLE ROMDATA F15TnPciWorkaroundTable = {
  PrimaryCores,
  (sizeof (F15TnPciWorkarounds) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) F15TnPciWorkarounds,
};


/*---------------------------------------------------------------------------------------*/
/**
 *  Workaround for Non-A0 TN processors.
 *
 *  AGESA should program F5x88[18] with the fused value from F3x1FC[20] for non-RevA0 parts.
 *
 * @param[in]   Data         The table data value, for example to indicate which CPU and Platform types matched.
 * @param[in]   StdHeader    Config handle for library and services.
 *
 */
VOID
STATIC
SetEnCstateBoostBlockCC6Exit (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  PCI_ADDR PciAddress;
  PRODUCT_INFO_REGISTER ProductInfo;
  NB_CFG_4_REGISTER NbCfg4;

  PciAddress.AddressValue = PRCT_INFO_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, (VOID *)&ProductInfo, StdHeader);

  PciAddress.AddressValue = NB_CFG_REG4_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, (VOID *)&NbCfg4, StdHeader);

  NbCfg4.EnCstateBoostBlockCC6Exit = ProductInfo.EnCstateBoostBlockCC6Exit;
  LibAmdPciWrite (AccessWidth32, PciAddress, (VOID *)&NbCfg4, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Workaround for Erratum #687 for TN processors.
 *
 *  AGESA should program F5x88[14] with the fused value from F3x1FC[29] and
 *  program F2x408[CpuElevPrioDis] with inversed fuse value from F3x1FC[29]  for all TN parts.
 *
 * @param[in]   Data         The table data value, for example to indicate which CPU and Platform types matched.
 * @param[in]   StdHeader    Config handle for library and services.
 *
 */
VOID
STATIC
Erratum687Workaround (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  PCI_ADDR                  PciAddress;
  PRODUCT_INFO_REGISTER     ProductInfo;
  NB_CFG_4_REGISTER         NbCfg4;
  GMC_TO_DCT_CTL_2_REGISTER GmcToDctCtrl2;
  UINT32                    DctSelCnt;
  DCT_CFG_SEL_REGISTER      DctCfgSel;

  PciAddress.AddressValue = PRCT_INFO_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, (VOID *)&ProductInfo, StdHeader);

  PciAddress.AddressValue = NB_CFG_REG4_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, (VOID *)&NbCfg4, StdHeader);
  NbCfg4.Bit14 = ProductInfo.EnDcqChgPriToHigh;
  LibAmdPciWrite (AccessWidth32, PciAddress, (VOID *)&NbCfg4, StdHeader);

  for (DctSelCnt = 0; DctSelCnt <= 1; DctSelCnt++) {
    PciAddress.AddressValue = GMC_TO_DCT_CTL_2_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, (VOID *)&GmcToDctCtrl2, StdHeader);
    GmcToDctCtrl2.CpuElevPrioDis = ~ProductInfo.EnDcqChgPriToHigh;
    LibAmdPciWrite (AccessWidth32, PciAddress, (VOID *)&GmcToDctCtrl2, StdHeader);

    PciAddress.AddressValue = DCT_CFG_SEL_REG_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, (VOID *)&DctCfgSel, StdHeader);
    DctCfgSel.DctCfgSel = ~DctCfgSel.DctCfgSel;
    LibAmdPciWrite (AccessWidth32, PciAddress, (VOID *)&DctCfgSel, StdHeader);
  }
}

