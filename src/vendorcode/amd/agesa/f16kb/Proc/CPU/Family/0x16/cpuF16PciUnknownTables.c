/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_16 PCI tables for unknown processor
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x16
 * @e \$Revision: 85962 $   @e \$Date: 2013-01-14 20:12:29 -0600 (Mon, 14 Jan 2013) $
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
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X16_CPUF16PCIUNKNOWNTABLES_FILECODE

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

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F16PciUnknownRegisters[] =
{
// D18F0x68 - Link Transaction Control
// bits[22:21]   DsNpReqLmt          = 0x2
// bits[19]      ApicExtSpur         = 0x1
// bits[18]      ApicExtId           = 0x1
// bits[17]      ApicExtBrdCst       = 0x1
// bits[15]      LimitCldtCfg        = 0x1
// bits[11]      RespPassPW          = 0x1
// bits[10]      DisFillP            = 0x0
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_ALL                           // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x68),  // Address
      0x004E8800,                           // RegData
      0x006E8C00,                           // RegMask
    }}
  },
// D18F0x6C - Link Initialization Control
// bits[0]       RouteTblDis         = 0x0
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_ALL                           // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x6C),    // Address
      0x00000000,                           // RegData
      0x00000001,                           // RegMask
    }}
  },
// D18F0x90 - Upstream Base Channel Buffer Count
// bits[27:25]   FreeData            = 0x0
// bits[24:20]   FreeCmd             = 0x0
// bits[19:18]   RspData             = 0x1
// bits[17:16]   NpReqData           = 0x1
// bits[15:12]   ProbeCmd            = 0x0
// bits[11:8]    RspCmd              = 0x2
// bits[7:5]     PReq                = 0x5
// bits[4:0]     NpReqCmd            = 0x8
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_ALL                           // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x90),    // Address
      0x000502A8,                           // RegData
      0x0FFFFFFF,                           // RegMask
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
      AMD_F16_ALL                           // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x94),    // Address
      0x02010000,                           // RegData
      0x1FFF0000,                           // RegMask
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
      AMD_F16_ALL                           // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x6C),    // Address
      0x10010052,                           // RegData
      0x700700F7,                           // RegMask
    }}
  },
// D18F3xA0 - Power Control Miscellaneous
// bits[13:11]   PllLockTime         = 0x1
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_ALL                           // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA0),    // Address
      0x00000800,                           // RegData
      0x00003800,                           // RegMask
    }}
  },
// D18F3xA4 - Reported Temperature Control
// bits[20]      TcenPwrDnCc6En      = 0x1
// bits[12:8]    PerStepTimeDn       = 0xF
// bits[7]       TmpSlewDnEn         = 0x1
// bits[6:5]     TmpMaxDiffUp        = 0x3
// bits[4:0]     PerStepTimeUp       = 0xF
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_ALL                           // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA4),    // Address
      0x00100FEF,                           // RegData
      0x00101FFF,                           // RegMask
    }}
  },
// D18F3x1CC - IBS Control
// bits[8]       LvtOffsetVal        = 0x1
// bits[3:0]     LvtOffset           = 0x0
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_ALL                           // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1CC),   // Address
      0x00000100,                           // RegData
      0x0000010F,                           // RegMask
    }}
  },
// D18F4x15C - Core Performance Boost Control
// bits[1:0]     BoostSrc            = 0
  {
    PciRegister,
    {
      AMD_FAMILY_16,                        // CpuFamily
      AMD_F16_ALL                           // CpuRevision
    },
    {AMD_PF_ALL},                           // PlatformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x15C),   // Address
      0x00000000,                           // RegData
      0x00000003,                           // RegMask
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F16PciUnknownRegisterTable = {
  PrimaryCores,
  PERFORM_TP_BEFORE_AP_LAUNCH,
  (sizeof (F16PciUnknownRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F16PciUnknownRegisters,
};
