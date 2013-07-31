/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_16 KB PCI tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x16/KB
 * @e \$Revision: 86705 $   @e \$Date: 2013-01-24 17:34:21 -0600 (Thu, 24 Jan 2013) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
#include "amdlib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "F16KbPowerMgmt.h"
#include "Table.h"
#include "Filecode.h"
CODE_GROUP  (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X16_KB_F16KBPCITABLES_FILECODE

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
SetDisCstateBoostBlockPstateUp (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
ScalingApmParamBaseOnCSampleTimer (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
SetTdpLimitDis (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );
/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

//   P C I   T a b l e s
// ----------------------

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F16KbPciRegistersAfterApLaunch[] =
{
// D18F0x6C - Link Initialization Control
// bits[0]       RouteTblDis         = 0x0
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x6C),  // Address
      0x00000000,                           // RegData
      0x00000001,                           // RegMask
    }}
  },
// D18F0x84 - Link Control
// bits[12]       IsocEn             = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x84),  // Address
      0x00001000,                           // RegData
      0x00001000,                           // RegMask
    }}
  },
// D18F0x90 - Link Base Channel Buffer Count
// bits[30]      PReq[3]             = 0x0
// bits[29:28]   NpReqData[3:2]      = 0x0
// bits[27:25]   FreeData            = 0x0
// bits[24:20]   FreeCmd             = 0x0
// bits[19:18]   RspData             = 0x1
// bits[17:16]   NpReqData[1:0]      = 0x1
// bits[15:12]   ProbeCmd            = 0x0
// bits[11:8]    RspCmd              = 0x1
// bits[7:5]     PReq[2:0]           = 0x5
// bits[4:0]     NpReqCmd            = 0x5
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x90),  // Address
      0x000501A5,                           // RegData
      0x7FFFFFFF,                           // RegMask
    }}
  },
// D18F0x94 - Link Isochronous Channel Buffer Count
// bits[28:27]   IsocRspData         = 0x0
// bits[26:25]   IsocNpReqData       = 0x1
// bits[24:22]   IsocRspCmd          = 0x0
// bits[21:19]   IsocPReq            = 0x0
// bits[18:16]   IsocNpReqCmd        = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x94),  // Address
      0x02010000,                           // RegData
      0x1FFF0000,
    }}
  },
// D18F0x90 - Link Base Channel Buffer Count
// bit [31]      LockBc              = 0x1
//
// NOTE: The entry is intended to be programmed after other bits of D18F0x[90, 94] is programmed.
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x90),  // Address
      0x80000000,                           // regData
      0x80000000,                           // regMask
    }}
  },
// D18F0x110 - Link Clumping Enable
// bits [31:1]   ClumpEn             = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x110), // Address
      0x00000002,                           // regData
      0xFFFFFFFE,                           // regMask
    }}
  },
// D18F0x168 - Extended Link Transaction Control
// bits[20]                          = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x168), // Address
      0x00100000,                           // RegData
      0x00100000,                           // RegMask
    }}
  },

// D18F2x118 - Memory Controller Configuration Low
// bits[31:28]   MctVarPriCntLmt     = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_2, 0x118), // Address
      0x10000000,                           // RegData
      0xF0000000,                           // RegMask
    }}
  },
// D18F3x44 - MCA NB Configuration
// bits[30]      SyncFloodOnDramAdrParErr = 0x1
// bits[27]      NbMcaToMstCpuEn     = 0x1
// bits[21]      SyncFloodOnAnyUcErr = 0x1
// bits[20]      SyncFloodOnWDT      = 0x1
// bits[6]       CpuErrDis           = 0x1
// bits[4]       SyncPktPropDis      = 0x0
// bits[3]       SyncPktGenDis       = 0x0
// bits[2]       SyncFloodOnDramUcEcc = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x44),  // Address
      0x48300044,                           // RegData
      0x4830005C,                           // RegMask
    }}
  },
// D18F3x6C - Data Buffer Count
// bits[30:28]   IsocRspDBC          = 0x1
// bits[18:16]   UpRspDBC            = 0x1
// bits[7:6]     DnRspDBC            = 0x1
// bits[5:4]     DnReqDBC            = 0x1
// bits[2:0]     UpReqDBC            = 0x2
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x6C),  // Address
      0x10010052,                           // RegData
      0x700700F7,                           // RegMask
    }}
  },
// D18F3x70 - SRI to XBAR Command Buffer Count
// bits[30:28]   IsocRspCBC          = 0x1
// bits[26:24]   IsocPreqCBC         = 0x0
// bits[22:20]   IsocReqCBC          = 0x1
// bits[18:16]   UpRspCBC            = 0x3
// bits[14:12]   DnPreqCBC           = 0x1
// bits[10:8]    UpPreqCBC           = 0x1
// bits[7:6]     DnRspCBC            = 0x1
// bits[5:4]     DnReqCBC            = 0x1
// bits[2:0]     UpReqCBC            = 0x3
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x70),  // Address
      0x10131153,                           // RegData
      0x777777F7,                           // RegMask
    }}
  },
// D18F3x74 - XBAR to SRI Command Buffer Count
// bits[31:28]   DRReqCBC            = 0x0
// bits[26:24]   IsocPreqCBC         = 0x1
// bits[23:20]   IsocReqCBC          = 0x1
// bits[19:16]   ProbeCBC            = 0xC
// bits[14:12]   DnPreqCBC           = 0x0
// bits[10:8]    UpPreqCBC           = 0x1
// bits[6:4]     DnReqCBC            = 0x0
// bits[2:0]     UpReqCBC            = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0x011C0101,                           // RegData
      0xF7FF7777,                           // RegMask
    }}
  },
// D18F3x78 - MCT to XBAR Buffer Count
// bits[12:8]    ProbeCBC            = 0x8
// bits[5:0]     RspCBC              = 0x10
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x78),  // Address
      0x00000810,                           // RegData
      0x00001F3F,                           // RegMask
    }}
  },

// D18F3x7C - Free List Buffer Count
// bits[22:20]   Sri2XbarFreeRspDBC  = 0x0
// bits[19:16]   Sri2XbarFreeXreqDBC = 0x5
// bits[15:12]   Sri2XbarFreeRspCBC  = 0x0
// bits[11:8]    Sri2XbarFreeXreqCBC = 0x6
// bits[5:0]     Xbar2SriFreeListCBC = 0x1B
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x0005061B,                           // RegData
      0x007FFF3F,                           // RegMask
    }}
  },
// D18F3x84 - ACPI Power State Control High
// bits[1]       NbLowPwrEnSmafAct4  = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x84),  // Address
      0x00000002,                           // RegData
      0x00000002,                           // RegMask
    }}
  },
// D18F3xA0 - Power Control Miscellaneous
// bits[13:11]   PllLockTime         = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA0),  // Address
      0x00000800,                           // RegData
      0x00003800,                           // RegMask
    }}
  },
// D18F3xA0 - Power Control Miscellaneous
// bits[14]      Svi2HighFreqSel     = 0x1, if PERFORMANCE_VRM_HIGH_SPEED_ENABLE == TRUE
  {
    ProfileFixup,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      PERFORMANCE_VRM_HIGH_SPEED_ENABLE,    // PerformanceFeatures
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA0),  // Address
      0x00004000,                           // regData
      0x00004000,                           // regMask
    }}
  },
// D18F3xA4 - Reported Temperature Control
// bits[12:8]    PerStepTimeDn       = 0xF
// bits[7]       TmpSlewDnEn         = 0x1
// bits[6:5]     TmpMaxDiffUp        = 0x3
// bits[4:0]     PerStepTimeUp       = 0xF
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA4),  // Address
      0x00000FEF,                           // RegData
      0x00001FFF,                           // RegMask
    }}
  },
// D18F3xD4 - Clock Power/Timing Control 0
// bits[31]      NbClkDivApplyAll    = 0x1
// bits[30:28]   NbClkDiv            = 0x1
// bits[27:24]   PowerStepUp         = 0x8
// bits[23:20]   PowerStepDown       = 0x8
// bits[11:8]    ClkRampHystSel      = 0xF
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xD4),  // Address
      0x98800F00,                           // RegData
      0xFFF00F00,                           // RegMask
    }}
  },
// D18F3xD8 - Clock Power/Timing Control 1
// bits[6:4]     VSRampSlamTime      = 0x4
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xD8),  // Address
      0x00000040,                           // RegData
      0x00000070,                           // RegMask
    }}
  },
// D18F3xDC - Clock Power/Timing Control 2
// bits[29:27]   NbsynPtrAdjLo       = 0x5
// bits[26]      IgnCpuPrbEn         = 0x0
// bits[25:19]   CacheFlushOnHaltTmr = 0xF
// bits[18:16]   CacheFlushOnHaltCtl = 0x0
// bits[14:12]   NbsynPtrAdj         = 0x6
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xDC),  // Address
      0x28786000,                           // RegData
      0x3FFF7000,                           // RegMask
    }}
  },
// D18F3x140 - SRI to XCS Token Count
// bits[24:20]   FreeTok             = 0xF
// bits[17:16]   IsocRspTok          = 0x1
// bits[15:14]   IsocPreqTok         = 0x0
// bits[13:12]   IsocReqTok          = 0x1
// bits[11:10]   DnRspTok            = 0x1
// bits[9:8]     UpRspTok            = 0x1
// bits[7:6]     DnPreqTok           = 0x1
// bits[5:4]     UpPreqTok           = 0x2
// bits[3:2]     DnReqTok            = 0x1
// bits[1:0]     UpReqTok            = 0x2
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00F11566,                           // RegData
      0x01F3FFFF,                           // RegMask
    }}
  },
// D18F3x144 - MCT to XCS Token Count
// bits[7:4]     ProbeTok            = 0x4
// bits[3:0]     RspTok              = 0x7
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x144), // Address
      0x00000047,                           // RegData
      0x000000FF,                           // RegMask
    }}
  },
// D18F3x148 - Link to XCS Token Count - Link 0
// bits[31:30]   FreeTok[3:2]        = 0x0
// bits[28]      IsocRspTok1         = 0x0
// bits[26]      IsocPreqTok1        = 0x0
// bits[24]      IsocReqTok1         = 0x0
// bits[23:22]   ProbeTok1           = 0x0
// bits[21:20]   RspTok1             = 0x0
// bits[19:18]   PReqTok1            = 0x0
// bits[17:16]   ReqTok1             = 0x0
// bits[15:14]   FreeTok[1:0]        = 0x0
// bits[13:12]   IsocRspTok0         = 0x0
// bits[11:10]   IsocPreqTok0        = 0x1
// bits[9:8]     IsocReqTok0         = 0x1
// bits[7:6]     ProbeTok0           = 0x0
// bits[5:4]     RspTok0             = 0x2
// bits[3:2]     PReqTok0            = 0x2
// bits[1:0]     ReqTok0             = 0x2
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x0000052A,                           // RegData
      0xD5FFFFFF,                           // RegMask
    }}
  },
// D18F3x160 - NB Machine Check Misc (DRAM Thresholding) 0 (MC4_MISC0)
// bits[23:20]   LvtOffset           = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x160), // Address
      0x00100000,                           // RegData
      0x00F00000,                           // RegMask
    }}
  },
// D18F3x168 - NB Machine Check Misc (Link Thresholding) 1 (MC4_MISC1)
// bits[23:20]   LvtOffset           = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x168), // Address
      0x00100000,                           // RegData
      0x00F00000,                           // RegMask
    }}
  },
// D18F3x17C - Extended Freelist Buffer Count
// bits[3:0]     SPQPrbFreeCBC       = 0x8
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x17C), // Address
      0x00000008,                           // RegData
      0x0000000F,                           // RegMask
    }}
  },
// D18F3x180 - Extended NB MCA Configuration
// bits[28]      SyncFloodOnCC6DramUcErr = 0x1
// bits[24]      McaLogErrAddrWdtErr = 0x1
// bits[21]      SyncFloodOnCpuLeakErr = 0x1
// bits[17]      SyncFloodOnDeferErrToIO = 0x1
// bits[9]       SyncFloodOnUCNbAry  = 0x1
// bits[8]       SyncFloodOnProtErr  = 0x1
// bits[7]       SyncFloodOnTgtAbortErr = 0x1
// bits[6]       SyncFloodOnDatErr   = 0x1
// bits[5]       DisPciCfgCpuMstAbortRsp = 0x1
// bits[1]       SyncFloodOnUsPwDatErr = 0x1
// bits[0]       McaLogUsPwDatErrEn  = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x180), // Address
      0x112203E3,                           // RegData
      0x112203E3,                           // RegMask
    }}
  },
// bits[9]                           = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x188), // Address
      0x00000200,                           // RegData
      0x00000200,                           // RegMask
    }}
  },
// D18F3x1A0 - Core Interface Buffer Count
// bits[18:16]   CpuToNbFreeBufCnt   = 0x3
// bits[2:0]     CpuCmdBufCnt        = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1A0), // Address
      0x00030001,                           // RegData
      0x00070007,                           // RegMask
    }}
  },
// D18F3x1CC - IBS Control
// bits[8]       LvtOffsetVal        = 0x1
// bits[3:0]     LvtOffset           = 0x0
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1CC), // Address
      0x00000100,                           // RegData
      0x0000010F,                           // RegMask
    }}
  },
// D18F3x1E4 - SBI Control
// bits[11:8]    LvtOffset           = 0x3
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1E4), // Address
      0x00000300,                           // RegData
      0x00000F00,                           // RegMask
    }}
  },
// D18F0x6C - Link Initialization Control
// bit [30]      RlsLnkFullTokCntImm = 0x1
// bit [28]      RlsIntFullTokCntImm = 0x1
//
// NOTE: The entry is intended to be after D18F0x[90, 94] and D18F3x[6C, 70, 74, 78, 7C, 140, 144, 148, 14C, 17C, 1A0] are programmed.
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x6C),  // Address
      0x50000000,                           // RegData
      0x50000000,                           // RegMask
    }}
  },
// D18F0x6C - Link Initialization Control
// bits[27]      ApplyIsocModeEnNow  = 0x1
//
// NOTE: The entry is intended to be after RlsLnkFullTokCntImm and RlsIntFullTokCntImm have been set.
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x6C),  // Address
      0x08000000,                           // RegData
      0x08000000,                           // RegMask
    }}
  },
// D18F4x118 - C-state Control 1
// bits[23:21]   ClkDivisorCstAct1   = 0x0
// bits[19:18]   CacheFlushTmrSelCstAct1 = 0x2
// bits[16]      CpuPrbEnCstAct1     = 0x1
// bits[7:5]     ClkDivisorCstAct0   = 0x0
// bits[3:2]     CacheFlushTmrSelCstAct0 = 0x2
// bits[0]       CpuPrbEnCstAct0     = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x118), // Address
      0x00090009,                           // RegData
      0x00ED00ED,                           // RegMask
    }}
  },
// D18F4x128 - C-state Policy Control 1
// bits[24:23]   CacheFlushSucMonMispredictAct = 0x1
// bits[22:21]   CacheFlushSucMonTmrSel = 0x1
// bits[20:18]   CacheFlushSucMonThreshold = 0x4
// bits[11:5]    CacheFlushTmr       = 0x7F
// bits[4:2]     HaltCstateIndex     = 0x0
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x128), // Address
      0x00B00FE0,                           // RegData
      0x01FC0FFC,                           // RegMask
    }}
  },
// D18F4x15C - Core Performance Boost Control
// bits[8]       CstatePowerEn       = 0x1
// bits[1:0]     BoostSrc            = 0x0
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x15C), // Address
      0x00000100,                           // RegData
      0x00000103,                           // RegMask
    }}
  },
// D18F4x16C - APM TDP Control
// bits[14]      CacUpC1             = 0x0
// bits[4]       ApmTdpLimitIntEn    = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x16C), // Address
      0x00000010,                           // RegData
      0x00004010,                           // RegMask
    }}
  },
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x210), // Address
      0x0000B41F,                           // RegData
      0x0000F7FF,                           // RegMask
    }}
  },
// D18F5x88 - NB Configuration 4 (NB_CFG4)
// bits[24]      DisHbNpReqBusLock   = 0x1
// bits[0]       CC6PstateWakeUpDis  = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_5, 0x88),  // Address
      0x01000001,                           // RegData
      0x01000001,                           // RegMask
    }}
  },
// D18F5x8C
// bits[15]      EnSrqAllocGt31      = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_5, 0x8C),  // Address
      0x00008000,                           // RegData
      0x00008000,                           // RegMask
    }}
  },
// D18F5xE0 - Processor TDP Running Average
// bits[3:0]     RunAvgRange         = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_5, 0xE0),  // Address
      0x00000001,                           // RegData
      0x0000000F,                           // RegMask
    }}
  },
// D18F5x128 - Clock Power/Timing Control 3
// bits[13:12]   PwrGateTmr          = 0x0
// bits[9]       FastSlamTimeDown    = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_5, 0x128), // Address
      0x00000200,                           // RegData
      0x00003200,                           // RegMask
    }}
  },
// D18F5x12C - Clock Power/Timing Control 4
// bits[5]       CorePsi1En          = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_5, 0x12C), // Address
      0x00000020,                           // RegData
      0x00000020,                           // RegMask
    }}
  },
// D18F5x170 - Northbridge P-state Control
// bits[12:9]    NbPstateThreshold   = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_5, 0x170), // Address
      0x00000200,                           // RegData
      0x00001E00,                           // RegMask
    }}
  },
// D18F5x178 - Northbridge Fusion Configuration
// bits[18]      CstateFusionHsDis   = 0x1
// bits[17]                          = 0x1
// bits[16]      ProcHotToGnbEn      = 0x1
// bits[11]                          = 0x1
// bits[10]                          = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_5, 0x178), // Address
      0x00070C00,                           // RegData
      0x00070C00,                           // RegMask
    }}
  },
};

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F16KbPciRegistersBeforeApLaunch[] =
{
// D18F0x68 - Link Transaction Control
// bits[22:21]   DsNpReqLmt          = 0x2
// bits[19]      ApicExtSpur         = 0x1
// bits[18]      ApicExtId           = 0x1
// bits[17]      ApicExtBrdCst       = 0x1
// bits[15]      LimitCldtCfg        = 0x1
// bits[11]      RespPassPW          = 0x1
// bits[10]      DisFillP            = 0x0
// bits[4]       DisMTS              = 0x1
// bits[3]       DisWrDwP            = 0x1
// bits[2]       DisWrBP             = 0x1
// bits[1]       DisRdDwP            = 0x1
// bits[0]       DisRdBP             = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x68),  // Address
      0x004E881F,                           // RegData
      0x006E8C1F,                           // RegMask
    }}
  },
// D18F3x8C - NB Configuration 1 High (NB_CFG1_HI)
// bits[30]      MSRC001_001F[DisStpClkAbortFlush] = 0x1
// bits[23]      MSRC001_001F[EnaDiv1CpuLowPwr] = 0x1
// bits[22]      MSRC001_001F[InitApicIdCpuIdLo] = 0x1
// bits[19]      MSRC001_001F[DisDatFwdVic] = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x8C),  // Address
      0x40C80000,                           // RegData
      0x40C80000,                           // RegMask
    }}
  },
};

//   PCI with Special Programming Requirements Table
// ----------------------

STATIC CONST FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_INITIALIZER ROMDATA F16KbPciWorkarounds[] =
{
// D18F3x88
  {
    FamSpecificWorkaround,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      SetDisCstateBoostBlockPstateUp,       // function call
      0x00000000,                           // data
    }}
  },
// D18F4x110
  {
    FamSpecificWorkaround,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      ScalingApmParamBaseOnCSampleTimer,    // function call
      0x00000000,                           // data
    }}
  },
// D18F4x16C
  {
    FamSpecificWorkaround,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_KB_ALL                        // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      SetTdpLimitDis,                       // function call
      0x00000000,                           // data
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F16KbPciRegisterTableAfterApLaunch = {
  PrimaryCores,
  PERFORM_TP_AFTER_AP_LAUNCH,
  (sizeof (F16KbPciRegistersAfterApLaunch) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) F16KbPciRegistersAfterApLaunch,
};

CONST REGISTER_TABLE ROMDATA F16KbPciRegisterTableBeforeApLaunch = {
  PrimaryCores,
  PERFORM_TP_BEFORE_AP_LAUNCH,
  (sizeof (F16KbPciRegistersBeforeApLaunch) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) F16KbPciRegistersBeforeApLaunch,
};

CONST REGISTER_TABLE ROMDATA F16KbPciWorkaroundTable = {
  PrimaryCores,
  PERFORM_TP_AFTER_AP_LAUNCH,
  (sizeof (F16KbPciWorkarounds) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *) F16KbPciWorkarounds,
};

/*---------------------------------------------------------------------------------------*/
/**
 *  Workaround for Kabini processors.
 *
 *
 * @param[in]   Data         The table data value, for example to indicate which CPU and Platform types matched.
 * @param[in]   StdHeader    Config handle for library and services.
 *
 */
VOID
SetDisCstateBoostBlockPstateUp (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  PCI_ADDR PciAddress;
  NB_CFG1_LOW_REG_REGISTER NbCfg1Low;
  NB_CFG_4_REGISTER NbCfg4;

  PciAddress.AddressValue = NB_CFG_REG4_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, (VOID *)&NbCfg4, StdHeader);

  PciAddress.AddressValue = NB_CFG1_LOW_REG_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, (VOID *)&NbCfg1Low, StdHeader);

  NbCfg1Low.DisCstateBoostBlockPstateUp = NbCfg4.EnCstateBoostBlockCC6Exit;
  LibAmdPciWrite (AccessWidth32, PciAddress, (VOID *)&NbCfg1Low, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Workaround for Kabini processors.
 *
 *  AGESA should program F4x110[11:0][CSampleTimer] to 2, and update below registers as:
 *  The following should be ROUNDED-NEAREST:
 *
 *  @param[in]   Data         The table data value, for example to indicate which CPU and Platform types matched.
 *  @param[in]   StdHeader    Config handle for library and services.
 *
 */
VOID
ScalingApmParamBaseOnCSampleTimer (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  PCI_ADDR PciAddress;
  pmgmt_str0 var0;
  pmgmt_str1 var1;
  SAMPLE_RESIDENCY_TIMER_REGISTER SampleResidencyTimer;

  PciAddress.AddressValue = SAMPLE_RESIDENCY_TIMER_PCI_ADDR;
  LibAmdPciRead (AccessWidth32, PciAddress, (VOID *)&SampleResidencyTimer, StdHeader);

  SampleResidencyTimer.CSampleTimer = 20;
  IDS_OPTION_HOOK (IDS_CSAMPLE_TIMER, (VOID *) (&SampleResidencyTimer), StdHeader);
  ASSERT (SampleResidencyTimer.CSampleTimer != 0);
  LibAmdPciWrite (AccessWidth32, PciAddress, (VOID *)&SampleResidencyTimer, StdHeader);

  // The following should be ROUNDED-NEAREST:
  // F5xB8[19:0] = F5xB8[19:0] * 2 / F4x110[CSampleTimer]
  // F5xBC[19:0] = F5xBC[19:0] * 2 / F4x110[CSampleTimer]
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, FUNC_5, 0xB8);
  LibAmdPciRead (AccessWidth32, PciAddress, (VOID *)&var0, StdHeader);
  var0.Bits_19_0 = ((var0.Bits_19_0 * 2) + (SampleResidencyTimer.CSampleTimer / 2)) / SampleResidencyTimer.CSampleTimer;
  LibAmdPciWrite (AccessWidth32, PciAddress, (VOID *)&var0, StdHeader);

  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, FUNC_5, 0xBC);
  LibAmdPciRead (AccessWidth32, PciAddress, (VOID *)&var1, StdHeader);
  var1.Bits_19_0 = ((var1.Bits_19_0 * 2) + (SampleResidencyTimer.CSampleTimer / 2)) / SampleResidencyTimer.CSampleTimer;
  LibAmdPciWrite (AccessWidth32, PciAddress, (VOID *)&var1, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Workaround for Kabini processors.
 *
 *  AGESA should program F4x16C[TdpLimitDis] in some case
 *
 * @param[in]   Data         The table data value, for example to indicate which CPU and Platform types matched.
 * @param[in]   StdHeader    Config handle for library and services.
 *
 */
VOID
SetTdpLimitDis (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 PciData;
  PCI_ADDR PciAddress;

  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, FUNC_0, 0xB8);
  PciData = 0xC010408C;
  LibAmdPciWrite (AccessWidth32, PciAddress, (VOID *)&PciData, StdHeader);
  PciAddress.Address.Register = 0xBC;
  LibAmdPciRead (AccessWidth32, PciAddress, (VOID *)&PciData, StdHeader);

  if ((PciData & BIT2) == BIT2) {
    PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, FUNC_4, 0x16C);
    LibAmdPciRead (AccessWidth32, PciAddress, (VOID *)&PciData, StdHeader);
    PciData |= BIT3;
    LibAmdPciWrite (AccessWidth32, PciAddress, (VOID *)&PciData, StdHeader);
  }
}

