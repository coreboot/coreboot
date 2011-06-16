/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_12 PCI tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x12
 * @e \$Revision: 45812 $   @e \$Date: 2011-01-22 07:45:25 +0800 (Sat, 22 Jan 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
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
#include "cpuRegisters.h"
#include "Table.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FAMILY_0X12_CPUF12PCITABLES_FILECODE

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

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F12PciRegisters[] =
{
// Function 0 - Link Config

// D18F0x68 - Link Transaction Control
// bit[11]     RespPassPW = 1
// bits[19:17] for 8bit APIC config
// bits[22:21] DsNpReqLmt = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x68),  // Address
      0x002E0800ull,                           // regData
      0x006E0800ull,                           // regMask
    }}
  },

// Function 3 - Misc. Control

// D18F3x40 - MCA NB Control
// bit[8]  MstrAbortEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x40),  // Address
      0x00000100ull,                           // regData
      0x00000100ull,                           // regMask
    }}
  },
// D18F3x44 - MCA NB Configuration
// bit[27]     NbMcaToMstCpuEn = 1
// bit[25]     DisPciCfgCpuErrRsp = 1
// bit[21]     SyncOnAnyErrEn = 1
// bit[20]     SyncOnWDTEn = 1
// bits[13:12] WDTBaseSel = 0
// bits[11:9]  WDTCntSel[2:0] = 0
// bit[6]      CpuErrDis = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x44),  // Address
      0x0A300040ull,                           // regData
      0x0A303E40ull,                           // regMask
    }}
  },
// D18F3x6C - Upstream Data Buffer Count
// bits[3:0]   UpLoPreqDBC = 0x0E
// bits[7:4]   UpLoNpreqDBC = 1
// bits[11:8]  UpLoRespDBC = 1
// bits[19:16] UpHiPreqDBC = 0
// bits[23:20] UpHiNpreqDBC = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO(0, 0, 24, FUNC_3, 0x6C),   // Address
      0x0000011Eull,                           // regData
      0x00FF0FFFull,                           // regMask
    }}
  },
// D18F3x74 - Upstream Command Buffer Count
// bits[3:0]   UpLoPreqCBC = 7
// bits[7:4]   UpLoNpreqCBC = 9
// bits[11:8]  UpLoRespCBC = 8
// bits[19:16] UpHiPreqCBC = 0
// bits[23:20] UpHiNpreqCBC = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO(0, 0, 24, FUNC_3, 0x74),   // Address
      0x00000897ull,                           // regData
      0x00FF0FFFull,                           // regMask
    }}
  },
// D18F3x7C - In-Flight Queue Buffer Allocation
// bits[5:0]  CpuBC = 1
// bits[13:8] LoPriPBC = 1
// bits[21:16] LoPriNPBC = 1
// bits[29:24] FreePoolBC = 0x19
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x19010101ull,                           // regData
      0x3F3F3F3Full,                           // regMask
    }}
  },
// D18F3x84 - ACPI Power State Control High
// bit[18] Smaf6DramMemClkTri = 1
// bit[17] Smaf6DramSr = 1
// bit[2]  Smaf4DramMemClkTri = 1
// bit[1]  Smaf4DramSr = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x84),  // Address
      0x00060006ull,                           // regData
      0x00060006ull,                           // regMask
    }}
  },
// D18F3x8C - NB Configuration High
// bit[26] EnConvertToNonIsoc = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x8C),  // Address
      0x04000000ull,                           // regData
      0x04000000ull,                           // regMask
    }}
  },
// D18F3xA0 - Power Control Miscellaneous
// bit[9] SviHighFreqSel = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA0),  // Address
      0x00000200ull,                           // regData
      0x00000200ull,                           // regMask
    }}
  },
// D18F3xA4 - Reported Temperature Control
// bits[12:8] PerStepTimeDn = 0xF
// bit [7]    TmpSlewDnEn   = 1
// bits[6:5]  TmpMaxDiffUp  = 0x3
// bits[4:0]  PerStepTimeUp = 0xF
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA4),  // Address
      0x00000FEFull,                           // regData
      0x00001FFFull,                           // regMask
    }}
  },
// D18F3xD4 - Clock Power Timing Control 0
// bits[11:8]  ClkRampHystSel = 0xF
// bits[15:12] OnionOutHyst = 0x4
// bit[17]     ClockGatingEnDram = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xD4),  // Address
      0x00024F00ull,                           // regData
      0x0002FF00ull,                           // regMask
    }}
  },
// D18F3xD4 - Clock Power Timing Control 0
// bit[7]     ShallowHaltDidAllow = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_LN_Bx                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xD4),  // Address
      0x00000080ull,                           // regData
      0x00000080ull,                           // regMask
    }}
  },
// D18F3xDC - Clock Power Timing Control 2
// bits[29:27] NbClockGateHyst = 3
// bit[30]     NbClockGateEn = 0 - erratum #596
// bit[31]     CnbCifClockGateEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xDC),  // Address
      0x98000000ull,                           // regData
      0xF8000000ull,                           // regMask
    }}
  },
// D18F3x17C - In-Flight Queue Extended Buffer Allocation
// bits[5:0]  HiPriPBC = 0
// bits[13:8] HiPriNPBC = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x17C), // Address
      0x00000000ull,                           // regData
      0x00003F3Full,                           // regMask
    }}
  },
// D18F3x180 - Extended NB MCA Configuration
// bit[2]   WDTCntSel[3] = 0
// bit[5]   DisPciCfgCpuMstAbtRsp = 1
// bit[21]  SyncFloodOnCpuLeakErr = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x180), // Address
      0x00200020ull,                           // regData
      0x00200024ull,                           // regMask
    }}
  },
// D18F3x188 - NB Extended Configuration
// bit[21]      EnCpuSerWrBehindIoRd = 0
// bit[23]      EnCpuSerRdBehindIoRd = 0
// bits[27:24]  FeArbCpuWeightOverLoPrio = 0x0B
// bits[31:28]  FeArbCpuWeightOverHiPrio = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x188), // Address
      0x1B000000ull,                           // regData
      0xFFA00000ull,                           // regMask
    }}
  },

// Function 4 - Extended Misc. Control

// D18F4x104 - TDP Lock Accumulator
// bits[1:0]    TdpLockDivVal = 1
// bits[13:2]   TdpLockDivRate = 0x190
// bits[16:15]  TdpLockDivValCpu = 1
// bits[28:17]  TdpLockDivRateCpu = 0x190
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x104), // Address
      0x03208641ull,                           // regData
      0x1FFFBFFFull,                           // regMask
    }}
  },
// D18F4x118 - C-state Control 1
// bits[10:8] CstAct1 = 0
// bits[2:0]  CstAct0 = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x118), // Address
      0x00000000ull,                           // regData
      0x00000707ull,                           // regMask
    }}
  },
// D18F4x120 - C-state Policy Control 1
// bit[31]      CstateMsgDis = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x120), // Address
      0x80000000ull,                           // regData
      0x80000000ull,                           // regMask
    }}
  },
// D18F4x124 - C-state Monitor Control 1
// bit[15]      TimerTickIntvlScale = 1
// bit[16]      TrackTimerTickInterEn = 1
// bit[17]      IntMonCC6En = 1
// bits[21:18]  IntMonCC6Lmt = 4
// bit[22]      IntMonPkgC6En = 0
// bits[26:23]  IntMonPkgC6Lmt = 0xA
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x124), // Address
      0x05138000ull,                           // regData
      0x07FF8000ull,                           // regMask
    }}
  },
// D18F4x134 - C-state Monitor Control 3
// bits[3:0]    IntRatePkgC6MaxDepth = 0
// bits[7:4]    IntRatePkgC6Threshold = 0
// bits[10:8]   IntRatePkgC6BurstLen = 1
// bits[15:11]  IntRatePkgC6DecrRate = 0x0A
// bits[19:16]  IntRateCC6MaxDepth = 5
// bits[23:20]  IntRateCC6Threshold = 4
// bits[26:24]  IntRateCC6BurstLen = 5
// bits[31:27]  IntRateCC6DecrRate = 0x18
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x134), // Address
      0xC5455100ull,                           // regData
      0xFFFFFFFFull,                           // regMask
    }}
  },
// D18F4x13C - SMAF Code DID 1
// bits[4:0]    Smaf4Did = 0x0F
// bits[20:16]  Smaf6Did = 0x0F
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x13C), // Address
      0x000F000Full,                           // regData
      0x001F001Full,                           // regMask
    }}
  },
// D18F4x14C - LPMV Scalar 2
// bits[25:24]  ApmCstExtPol = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x14C), // Address
      0x01000000ull,                           // regData
      0x03000000ull,                           // regMask
    }}
  },
// D18F4x14C - LPMV Scalar 2
// bit[26] CstatePowerSel = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_LN_Bx                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x14C), // Address
      0x04000000ull,                           // regData
      0x04000000ull,                           // regMask
    }}
  },
// D18F4x15C - Core Performance Boost Control
// bits[1:0]    BoostSrc = 0
// bit[29]      BoostEnAllCores = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x15C), // Address
      0x20000000ull,                           // regData
      0x20000003ull,                           // regMask
    }}
  },
// D18F4x15C - Core Performance Boost Control
// bit[28] IgnoreBoostThresh = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_LN_Bx                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x15C), // Address
      0x10000000ull,                           // regData
      0x10000000ull,                           // regMask
    }}
  },
// D18F4x1A4 - C-state Monitor Mask
// bits[7:0]    IntRateMonMask = 0xFC
// bits[15:8]   TimerTickMonMask = 0xFF
// bits[23:16]  NonC0MonMask = 0xFF
// bits[31:24]  C0MonMask = 0xFF
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x1A4), // Address
      0xFFFFFFFCull,                           // regData
      0xFFFFFFFFull,                           // regMask
    }}
  },
// D18F4x1A8 - CPU State Power Management Dynamic Control 0
// bits[4:0]    SingleHaltCpuDid = 8
// bits[9:5]    AllHaltCpuDid = 0x0F
// bit[15]      CpuProbEn = 0
// bits[22:20]  PServiceTmr = 1
// bit[23]      PServiceTmrEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x1A8), // Address
      0x009001E8ull,                           // regData
      0x00F083FFull,                           // regMask
    }}
  },
// D18F4x1AC - CPU State Power Management Dynamic Control 1
// bits[9:5]  C6Did = 0x0F
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x1AC), // Address
      0x000001E0ull,                           // regData
      0x000003E0ull,                           // regMask
    }}
  },
// D18F6x50 - Configuration Register Access Control
// bit[1]  CfgAccAddrMode = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x50),  // Address
      0x00000000ull,                           // regData
      0x00000002ull,                           // regMask
    }}
  },
// D18F6x54 - Dram Arbitration Control FEQ Collision
// bits[7:0]    FeqLoPrio = 0x20
// bits[15:8]   FeqMedPrio = 0x10
// bits[23:16]  FeqHiPrio = 8
// bit[31]      PpMode = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x54),  // Address
      0x00081020ull,                           // regData
      0x80FFFFFFull,                           // regMask
    }}
  },
// D18F6x154 - Dram Arbitration Control FEQ Collision
// bits[7:0]    FeqLoPrio = 0x20
// bits[15:8]   FeqMedPrio = 0x10
// bits[23:16]  FeqHiPrio = 8
// bit[31]      PpMode = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x154), // Address
      0x00081020ull,                           // regData
      0x80FFFFFFull,                           // regMask
    }}
  },
// D18F6x58 - Dram Arbitration Control Display Collision
// bits[7:0]    DispLoPrio = 0x40
// bits[15:8]   DispMedPrio = 0x20
// bits[23:16]  DispHiPrio = 0x10
// bits[31:24]  DispUrgPrio = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x58),  // Address
      0x00102040ull,                           // regData
      0xFFFFFFFFull,                           // regMask
    }}
  },
// D18F6x158 - Dram Arbitration Control Display Collision
// bits[7:0]    DispLoPrio = 0x40
// bits[15:8]   DispMedPrio = 0x20
// bits[23:16]  DispHiPrio = 0x10
// bits[31:24]  DispUrgPrio = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x158), // Address
      0x00102040ull,                           // regData
      0xFFFFFFFFull,                           // regMask
    }}
  },
// D18F6x5C - Dram Arbitration Control FEQ Write Protect
// bits[7:0]    FeqLoPrio = 0x20
// bits[15:8]   FeqMedPrio = 0x10
// bits[23:16]  FeqHiPrio = 8
// bit[31]      PpMode = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x5C),  // Address
      0x00081020ull,                           // regData
      0x80FFFFFFull,                           // regMask
    }}
  },
// D18F6x15C - Dram Arbitration Control FEQ Write Protect
// bits[7:0]    FeqLoPrio = 0x20
// bits[15:8]   FeqMedPrio = 0x10
// bits[23:16]  FeqHiPrio = 8
// bit[31]      PpMode = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x15C), // Address
      0x00081020ull,                           // regData
      0x80FFFFFFull,                           // regMask
    }}
  },
// D18F6x60 - Dram Arbitration Control Diplay Write Protect
// bits[7:0]    DispLoPri = 0x20
// bits[15:8]   DispMedPrio = 0x10
// bits[23:16]  DispHiPrio = 8
// bits[31:24]  DispUrgPrio = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x60),  // Address
      0x00081020ull,                           // regData
      0xFFFFFFFFull,                           // regMask
    }}
  },
// D18F6x160 - Dram Arbitration Control Diplay Write Protect
// bits[7:0]    DispLoPri = 0x20
// bits[15:8]   DispMedPrio = 0x10
// bits[23:16]  DispHiPrio = 8
// bits[31:24]  DispUrgPrio = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x160), // Address
      0x00081020ull,                           // regData
      0xFFFFFFFFull,                           // regMask
    }}
  },
// D18F6x64 - Dram Arbitration Control FEQ Read Protect
// bits[7:0]    FeqLoPrio = 0x10
// bits[15:8]   FeqMedPrio = 8
// bits[23:16]  FeqHiPrio = 4
// bit[31]      PpMode = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x64),  // Address
      0x00040810ull,                           // regData
      0x80FFFFFFull,                           // regMask
    }}
  },
// D18F6x164 - Dram Arbitration Control FEQ Read Protect
// bits[7:0]    FeqLoPrio = 0x10
// bits[15:8]   FeqMedPrio = 8
// bits[23:16]  FeqHiPrio = 4
// bit[31]      PpMode = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x164), // Address
      0x00040810ull,                           // regData
      0x80FFFFFFull,                           // regMask
    }}
  },
// D18F6x68 - Dram Arbitration Control Display Read Protect
// bits[7:0]    DispLoPrio = 0x10
// bits[15:8]   DispMedPrio = 8
// bits[23:16]  DispHiPrio = 4
// bits[31:24]  DispUrgPrio = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x68),  // Address
      0x00040810ull,                           // regData
      0xFFFFFFFFull,                           // regMask
    }}
  },
// D18F6x168 - Dram Arbitration Control Display Read Protect
// bits[7:0]    DispLoPrio = 0x10
// bits[15:8]   DispMedPrio = 8
// bits[23:16]  DispHiPrio = 4
// bits[31:24]  DispUrgPrio = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x168), // Address
      0x00040810ull,                           // regData
      0xFFFFFFFFull,                           // regMask
    }}
  },
// D18F6x6C - Dram Arbitration Control FEQ Fairness Timer
// bits[7:0]    FeqLoPrio = 0x80
// bits[15:8]   FeqMedPrio = 0x40
// bits[23:16]  FeqHiPrio = 0x20
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x6C),  // Address
      0x00204080ull,                           // regData
      0x00FFFFFFull,                           // regMask
    }}
  },
// D18F6x16C - Dram Arbitration Control FEQ Fairness Timer
// bits[7:0]    FeqLoPrio = 0x80
// bits[15:8]   FeqMedPrio = 0x40
// bits[23:16]  FeqHiPrio = 0x20
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x16C), // Address
      0x00204080ull,                           // regData
      0x00FFFFFFull,                           // regMask
    }}
  },
// D18F6x70 - Dram Arbitration Control Display Fairness Timer
// bits[7:0]    DispLoPrio = 0x80
// bits[15:8]   DispMedPrio = 0x40
// bits[23:16]  DispHiPrio = 0x20
// bits[31:24]  DispUrPrio = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x70),  // Address
      0x00204080ull,                           // regData
      0xFFFFFFFFull,                           // regMask
    }}
  },
// D18F6x170 - Dram Arbitration Control Display Fairness Timer
// bits[7:0]    DispLoPrio = 0x80
// bits[15:8]   DispMedPrio = 0x40
// bits[23:16]  DispHiPrio = 0x20
// bits[31:24]  DispUrPrio = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x170), // Address
      0x00204080ull,                           // regData
      0xFFFFFFFFull,                           // regMask
    }}
  },
// D18F6x78 - Dram Prioritization and Arbitration Control
// bits[1:0]    DispDbePrioEn = 3
// bit[2]       FeqDbePrioEn = 1
// bit[3]       DispArbCtrl = 0
// bits[5:4]    GlcEosDet = 3
// bit[6]       GlcEosDetDis = 0
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x78),  // Address
      0x00000037ull,                           // regData
      0x0000007Full,                           // regMask
    }}
  },
// D18F6x90 - NB P-state Config Low
// bit[30]      NbPsCtrlDis = 1
// bit[29]      NbPsForceSel = 0
// bit[28]      NbPsForceReq = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x90),  // Address
      0x50000000ull,                           // regData
      0x70000000ull,                           // regMask
    }}
  },
// D18F6x94 - NB P-state Config High
// bit[4]       NbPs1NoTransOnDma = 0
// bits[25:23]  NbPsC0Timer = 4
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x94),  // Address
      0x02000000ull,                           // regData
      0x03800010ull,                           // regMask
    }}
  },
// D18F6x9C - NCLK Reduction Control
// bits[6:0]   NclkRedDiv = 0x78
// bit[7]      NclkRedSelfRefrAlways = 1
  {
    PciRegister,
    {
      AMD_FAMILY_12,                      // CpuFamily
      AMD_F12_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x9C),  // Address
      0x000000F8ull,                           // regData
      0x000000FFull,                           // regMask
    }}
  }
};

CONST REGISTER_TABLE ROMDATA F12PciRegisterTable = {
  PrimaryCores,
  (sizeof (F12PciRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F12PciRegisters,
};
