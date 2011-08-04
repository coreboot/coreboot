/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_14 PCI tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x14
 * @e \$Revision: 38376 $   @e \$Date: 2010-09-23 11:39:55 +0800 (Thu, 23 Sep 2010) $
 *
 */
/*
 *****************************************************************************
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
 * 
 * ***************************************************************************
 *
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "cpuRegisters.h"
#include "Table.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FAMILY_0X14_CPUF14PCITABLES_FILECODE

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

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F14PciRegisters[] =
{
// Function 0 - Link Config

// D18F0x68 - Link Transaction Control
// bit[11]     RespPassPW = 1
// bits[19:17] for 8bit APIC config
// bits[22:21] DsNpReqLmt = 1
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x68),  // Address
      0x002E0800,                           // regData
      0x006E0800,                           // regMask
    }}
  },

// Function 2 - DRAM Controller

// D18F2xB8
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_2, 0xB8),  // Address
      0x00000000,                           // regData
      0xF000F000,                           // regMask
    }}
  },
// D18F2xBC
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_2, 0xBC),  // Address
      0x00000000,                           // regData
      0xC0000000,                           // regMask
    }}
  },
// D18F2x118 - Memory Controller Configuration Low
// bits[7:6], MctPriHiWr = 10b
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_2, 0x118),  // Address
      0x00000080,                           // regData
      0x000000C0,                           // regMask
    }}
  },
// D18F2x11C - Memory Controller Configuration High
// bits[24:22], PrefConf = 1
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_2, 0x11C),  // Address
      0x00400000,                           // regData
      0x01C00000,                           // regMask
    }}
  },

// Function 3 - Misc. Control

// D18F3x40 - MCA NB Control
// bit[8]  MstrAbortEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x40),  // Address
      0x00000100,                           // regData
      0x00000100,                           // regMask
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
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x44),  // Address
      0x0A300040,                           // regData
      0x0A303E40,                           // regMask
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
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x84),  // Address
      0x00060006,                           // regData
      0x00060006,                           // regMask
    }}
  },
// D18F3x8C - NB Configuration High
// bit[26] EnConvertToNonIsoc = 1
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x8C),  // Address
      0x04000000,                           // regData
      0x04000000,                           // regMask
    }}
  },
// D18F3xA0 - Power Control Miscellaneous
// bit[9] SviHighFreqSel = 1
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA0),  // Address
      0x00000200,                           // regData
      0x00000200,                           // regMask
    }}
  },
// D18F3xA4 - Reported Temperature Control
// bits[12:8] PerStepTimeDn = 0xF
// bit[7]     TmpSlewDnEn = 1
// bits[6:5]  TmpMaxDiffUp = 0x3
// bits[4:0]  PerStepTimeUp = 0xF

  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA4),  // Address
      0x00000FEF,                           // regData
      0x00001FFF,                           // regMask
    }}
  },
// D18F3xD4 - Clock Power Timing Control 0
// bits[11:8]  ClkRampHystSel = 0xF
// bits[15:12] OnionOutHyst = 0x4
// bit[17]     ClockGatingEnDram = 1
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xD4),  // Address
      0x00024F00,                           // regData
      0x0002FF00,                           // regMask
    }}
  },
// D18F3xDC - Clock Power Timing Control 2
// bits[29:27] NbClockGateHyst = 3
// bit[30]     NbClockGateEn = 0 - erratum #596
// bit[31]     CnbCifClockGateEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xDC),  // Address
      0x98000000,                           // regData
      0xF8000000,                           // regMask
    }}
  },
// D18F3x180 - Extended NB MCA Configuration
// bit[2]   WDTCntSel[3] = 0
// bit[5]   DisPciCfgCpuMstAbtRsp = 1
// bit[21]  SyncFloodOnCpuLeakErr = 1
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x180), // Address
      0x00200020,                           // regData
      0x00200024,                           // regMask
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
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x188), // Address
      0x1B000000,                           // regData
      0xFFA00000,                           // regMask
    }}
  },

// Function 4 - Extended Misc. Control

// D18F4x118 - C-state Control 1
// bits[2:0]  CstAct0 = 0
// bits[10:8] CstAct1 = 0
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x118), // Address
      0x00000000,                           // regData
      0x00000707,                           // regMask
    }}
  },
// D18F4x124 - C-state Monitor Control 1
// bit[15]      TimerTickIntvlScale = 1
// bit[16]      TrackTimerTickInterEn = 1
// bit[17]      IntMonCC6En = 1
// bits[21:18]  IntMonCC6Lmt = 4
// bit[22]      IntMonPkgC6En = 0
// bits[26:23]  IntMonPkgC6Lmt = 0x0A
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x124), // Address
      0x05138000,                           // regData
      0x07FF8000,                           // regMask
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
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x134), // Address
      0x05455100,                           // regData
      0x07FFFFFF,                           // regMask
    }}
  },
// D18F4x13C - SMAF Code DID 1
// bits[4:0]    Smaf4Did = 0x0F
// bits[20:16]  Smaf6Did = 0x0F
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x13C), // Address
      0x000F000F,                           // regData
      0x001F001F,                           // regMask
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
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x1A4), // Address
      0xFFFFFFFC,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
// D18F4x1A8 - CPU State Power Management Dynamic Control 0
// bits[4:0]   SingleHaltCpuDid = 0x1E
// bits[9:5]   AllHaltCpuDid = 0x1F
// bit[15]     CpuProbEn = 0
// bits[22:20] PServiceTmr = 1
// bit[23]     PServiceTmrEn = 1
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x1A8), // Address
      0x009003FE,                           // regData
      0x00F083FF,                           // regMask
    }}
  },
// D18F4x1AC - CPU State Power Management Dynamic Control 1
// bits[9:5]  C6Did = 0x1F
// bits[28]   CoreC6Dis = 1
// bits[29]   PkgC6Dis = 1
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x1AC), // Address
      0x300003E0,                           // regData
      0x300003E0,                           // regMask
    }}
  },
// D18F6x50 - Configuration Register Access Control
// bit[1]  CfgAccAddrMode = 0
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x50),  // Address
      0x00000000,                           // regData
      0x00000002,                           // regMask
    }}
  },
// D18F6x54 - DRAM Arbitration Control FEQ Collision
// bits[7:0]    FeqLoPrio = 0x20
// bits[15:8]   FeqMedPrio = 0x10
// bits[23:16]  FeqHiPrio = 8
// bit[31]      PpMode = 0
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x54),  // Address
      0x00081020,                           // regData
      0x80FFFFFF,                           // regMask
    }}
  },
// D18F6x58 - DRAM Arbitration Control Display Collision
// bits[7:0]    DispLoPrio = 0x40
// bits[15:8]   DispMedPrio = 0x20
// bits[23:16]  DispHiPrio = 0x10
// bits[31:24]  DispUrgPrio = 0
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x58),  // Address
      0x00102040,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
// D18F6x5C - DRAM Arbitration Control FEQ Write Protect
// bits[7:0]    FeqLoPrio = 0x20
// bits[15:8]   FeqMedPrio = 0x10
// bits[23:16]  FeqHiPrio = 0x08
// bit[31]      PpMode = 0
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x5C),  // Address
      0x00081020,                           // regData
      0x80FFFFFF,                           // regMask
    }}
  },
// D18F6x60 - DRAM Arbitration Control Display Write Protect
// bits[7:0]    DispLoPri = 0x20
// bits[15:8]   DispMedPrio = 0x10
// bits[23:16]  DispHiPrio = 0x08
// bits[31:24]  DispUrgPrio = 0
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x60),  // Address
      0x00081020,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
// D18F6x64 - DRAM Arbitration Control FEQ Read Protect
// bits[7:0]    FeqLoPrio = 0x10
// bits[15:8]   FeqMedPrio = 8
// bits[23:16]  FeqHiPrio = 4
// bit[31]      PpMode = 0
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x64),  // Address
      0x00040810,                           // regData
      0x80FFFFFF,                           // regMask
    }}
  },
// D18F6x68 - DRAM Arbitration Control Display  Read Protect
// bits[7:0]    DispLoPrio = 0x10
// bits[15:8]   DispMedPrio = 8
// bits[23:16]  DispHiPrio = 4
// bits[31:24]  DispUrgPrio = 0
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x68),  // Address
      0x00040810,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
// D18F6x6C - DRAM Arbitration Control FEQ Fairness Timer
// bits[7:0]    FeqLoPrio = 0x80
// bits[15:8]   FeqMedPrio = 0x40
// bits[23:16]  FeqHiPrio = 0x20
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x6C),  // Address
      0x00204080,                           // regData
      0x00FFFFFF,                           // regMask
    }}
  },
// D18F6x70 - DRAM Arbitration Control Display Fairness Timer
// bits[7:0]    DispLoPrio = 0x80
// bits[15:8]   DispMedPrio = 0x40
// bits[23:16]  DispHiPrio = 0x20
// bits[31:24]  DispUrPrio = 0
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x70),  // Address
      0x00204080,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
// D18F6x74 - Dram Idle Page Close Limit
// bits[40]    IdleLimit = 0x1E
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x74),  // Address
      0x0000001E,                           // regData
      0x0000001F,                           // regMask
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
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x78),  // Address
      0x00000037,                           // regData
      0x0000007F,                           // regMask
    }}
  },
// D18F6x90 - NB P-state Config Low
// As part of BIOS Requirements for NB P-state Initialization
// bit[30]      NbPsCtrlDis = 1
// bit[29]      NbPsForceSel = 0
// bit[28]      NbPsForceReq = 1
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x90),  // Address
      0x50000000,                           // regData
      0x70000000,                           // regMask
    }}
  },
// D18F6x94 - NB P-state Config High
// bits[25:23]  NbPsC0Timer = 4
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x94),  // Address
      0x02000000,                           // regData
      0x03800000,                           // regMask
    }}
  },
// D18F6x9C - NCLK Reduction Control
// bits[6:0]   NclkRedDiv = 0x60
// bit[7]      NclkRedSelfRefrAlways = 1
// bit[8]      NclkRampWithDllRelock = 1
  {
    PciRegister,
    {
      AMD_FAMILY_14,                      // CpuFamily
      AMD_F14_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_6, 0x9C),  // Address
      0x000001E0,                           // regData
      0x000001FF,                           // regMask
    }}
  }
};

CONST REGISTER_TABLE ROMDATA F14PciRegisterTable = {
  PrimaryCores,
  (sizeof (F14PciRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F14PciRegisters,
};
