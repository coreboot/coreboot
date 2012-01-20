/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Orochi PCI tables from Multi-Link BKDG paragraph recommended settings.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/OR
 * @e \$Revision: 41897 $   @e \$Date: 2010-11-12 12:39:18 +0800 (Fri, 12 Nov 2010) $
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
#include "F15PackageType.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_OR_F15ORMULTILINKPCITABLES_FILECODE


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

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F15OrMultiLinkPciRegisters[] =
{
  // Function 0

// F0x68 - Link Transaction Control
// bit[14:13], BufRelPri = 01h
  {
    PciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL,                          // CpuRevision  rev C or less.
    },
    {AMD_PF_MULTI_LINK},                      // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x68),  // Address
      0x00002000,                           // regData
      0x00006000,                           // regMask
    }}
  },
  // F0x[F0,D0,B0,90] Link Base Buffer Count Register
  // 27:25 FreeData: 0
  // 24:20 FreeCmd: 8
  // 19:18 RspData: 3
  // 17:16 NpReqData: 3
  // 15:12 ProbeCmd: 8
  // 11:8 RspCmd: 9
  // 7:5 PReq: 2
  // 4:0 NpReqCmd: 4
{
    HtHostPciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_MULTI_LINK},  // platformFeatures
    {{
      HT_HOST_FEAT_COHERENT, // link features
      0x10,                               // address
      0x008F8944,                         // data
      0x0FFFFFFF                          // mask
    }}
  },
  // F0x[F0,D0,B0,90] Link Base Buffer Count Register
  // 27:25 FreeData: 0
  // 24:20 FreeCmd: 8
  // 19:18 RspData: 1
  // 17:16 NpReqData: 0
  // 15:12 ProbeCmd: 0
  // 11:8 RspCmd: 2
  // 7:5 PReq: 7
  // 4:0 NpReqCmd: 14
  {
    HtHostPciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_MULTI_LINK},  // platformFeatures
    {{
      (HT_HOST_AND | HT_HOST_FEAT_NONCOHERENT | HT_HOST_FEAT_GANGED), // link features
      0x10,                               // address
      0x008402EE,                         // data
      0x0FFFFFFF                          // mask
    }}
  },
  // F0x[F0,D0,B0,90] Link Base Buffer Count Register
  // 27:25 FreeData: 0
  // 24:20 FreeCmd: 8
  // 19:18 RspData: 3
  // 17:16 NpReqData: 3
  // 15:12 ProbeCmd: 4
  // 11:8 RspCmd: 9
  // 7:5 PReq: 2
  // 4:0 NpReqCmd: 8
  {
    HtHostPerfPciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_MULTI_LINK},                    // platformFeatures
    {{
      PERFORMANCE_PROBEFILTER,
      HT_HOST_FEAT_COHERENT,              // link features
      0x10,                               // address
      0x008F4948,                         // data
      0x0FFFFFFF                          // mask
    }}
  },
  // F0x[F4,D4,B4,94] Link Base Buffer Count Register
  // 28:27 IsocRspData: 0
  // 26:25 IsocNpReqData: 0
  // 24:22 IsocRspCmd: 0
  // 21:19 IsocPReq: 0
  // 18:16 IsocNpReqCmd: 1
  {
    HtHostPciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_MULTI_LINK},  // platformFeatures
    {{
      HT_HOST_FEAT_COHERENT,              // link features
      0x14,                               // address
      0x00010000,                         // data
      0x1FFF0000                          // mask
    }}
  },
  // F0x[F4,D4,B4,94] Link Base Buffer Count Register
  // 28:27 IsocRspData: 0
  // 26:25 IsocNpReqData: 0
  // 24:22 IsocRspCmd: 0
  // 21:19 IsocPReq: 0
  // 18:16 IsocNpReqCmd: 1
  {
    HtHostPciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                           // CpuRevision
    },
    {AMD_PF_MULTI_LINK},
    {{
      (HT_HOST_AND | HT_HOST_FEAT_NONCOHERENT | HT_HOST_FEAT_GANGED), // Link Features
      0x14,                                 // Address
      0x00010000,                           // Data
      0x1FFF0000                            // Mask
    }},
  },

// Function 3 - Misc. Control

// NOTE: Order is important. Do not re-order
// the entries for F3x140.

// F3x140 - SRI_to_XCS Token Count
// bits[9:8] UpRspTok = 3
// bits[23:20] FreeTok = 10
  {
    TokenPciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                           // CpuRevision
    },
    {AMD_PF_MULTI_LINK},                      // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      (DEGREE_RANGE_0 (COUNT_RANGE_LOW, 1) | COUNT_RANGE_NONE),     // SCM
      PACKAGE_TYPE_SCM,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00A00300,                           // regData
      0x00F00300,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[9:8] UpRspTok = 3
// bits[23:20] FreeTok = 10
  {
    TokenPciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                           // CpuRevision
    },
    {AMD_PF_MULTI_LINK},                      // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      (DEGREE_RANGE_0 (COUNT_RANGE_LOW, 1) | COUNT_RANGE_NONE),     // MCM1 or MCM2h
      PACKAGE_TYPE_MCM,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00A00300,                           // regData
      0x00F00300,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[9:8] UpRspTok = 3
// bits[23:20] FreeTok = 9
  {
    TokenPciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                           // CpuRevision
    },
    {AMD_PF_MULTI_LINK},                      // platformFeatures
    {{
      PERFORMANCE_PROBEFILTER,
      (DEGREE_RANGE_0 (COUNT_RANGE_LOW, 1) | COUNT_RANGE_NONE),     // MCM1 or MCM2h
      PACKAGE_TYPE_MCM,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00900300,                           // regData
      0x00F00300,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[9:8] UpRspTok = 1
// bits[23:20] FreeTok = 11
  {
    TokenPciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                           // CpuRevision
    },
    {AMD_PF_MULTI_LINK},  // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      (DEGREE_RANGE_0 (3, 3) | COUNT_RANGE_NONE),     // MCM2
      PACKAGE_TYPE_MCM,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00B00100,                             // regData
      0x00F00300,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[9:8] UpRspTok = 3
// bits[23:20] FreeTok = 10
  {
    TokenPciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                           // CpuRevision
    },
    {AMD_PF_MULTI_LINK},                      // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      (DEGREE_RANGE_0 (2, 2) | COUNT_RANGE_NONE),     // MCM4h
      PACKAGE_TYPE_MCM,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00A00300,                           // regData
      0x00F00300,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[9:8] UpRspTok = 1
// bits[23:20] FreeTok = 9
  {
    TokenPciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                           // CpuRevision
    },
    {AMD_PF_MULTI_LINK},  // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      (DEGREE_RANGE_0 (4, COUNT_RANGE_HIGH) | COUNT_RANGE_NONE),     // MCM4
      PACKAGE_TYPE_MCM,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00900100,                           // regData
      0x00F00300,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 2
// bits[3:2] PReqTok0 = 2
// bits[5:4] RspTok0 = 2
// bits[7:6] ProbeTok0 = 2
// bits[9:8] IsocReqTok0 = 1
// bits[11:10] IsocPreqTok0 = 0
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 3
// bits[17:16] ReqTok1 = 0
// bits[19:18] PReqTok1 = 0
// bits[21:20] RspTok1 = 0
// bits[23:22] ProbeTok1= 0
// bits[24] IsocReqTok1 = 0
// bits[26] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                           // CpuRevision
    },
    {AMD_PF_MULTI_LINK},  // platformFeatures
    {{
      (COUNT_RANGE_ALL | COUNT_RANGE_NONE), // SCM
      PERFORMANCE_PROFILE_ALL,
      (HT_HOST_AND | HT_HOST_FEAT_COHERENT | HT_HOST_FEAT_GANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x0000C1AA,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 2
// bits[3:2] PReqTok0 = 2
// bits[5:4] RspTok0 = 2
// bits[7:6] ProbeTok0 = 2
// bits[9:8] IsocReqTok0 = 1
// bits[11:10] IsocPreqTok0 = 0
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 0
// bits[17:16] ReqTok1 = 0
// bits[19:18] PReqTok1 = 0
// bits[21:20] RspTok1 = 0
// bits[23:22] ProbeTok1= 0
// bits[24] IsocReqTok1 = 0
// bits[26] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                           // CpuRevision
    },
    {AMD_PF_MULTI_LINK},  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (COUNT_RANGE_LOW, 1) | COUNT_RANGE_NONE), // MCM1 or MCM2h.
      PERFORMANCE_PROFILE_ALL,
      (HT_HOST_AND | HT_HOST_FEAT_COHERENT | HT_HOST_FEAT_GANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x000001AA,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 2
// bits[3:2] PReqTok0 = 2
// bits[5:4] RspTok0 = 2
// bits[7:6] ProbeTok0 = 1
// bits[9:8] IsocReqTok0 = 1
// bits[11:10] IsocPreqTok0 = 0
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 0
// bits[17:16] ReqTok1 = 0
// bits[19:18] PReqTok1 = 0
// bits[21:20] RspTok1 = 0
// bits[23:22] ProbeTok1= 0
// bits[24] IsocReqTok1 = 0
// bits[26] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                           // CpuRevision
    },
    {AMD_PF_MULTI_LINK},  // platformFeatures
    {{
      (IGNORE_PROCESSOR_0 | DEGREE_RANGE_1 (2, 3)),  // MCM2 or MCM4h
      PERFORMANCE_PROFILE_ALL,
      (HT_HOST_AND | HT_HOST_FEAT_COHERENT | HT_HOST_FEAT_GANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x0000016A,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 2
// bits[3:2] PReqTok0 = 1
// bits[5:4] RspTok0 = 1
// bits[7:6] ProbeTok0 = 2
// bits[9:8] IsocReqTok0 = 1
// bits[11:10] IsocPreqTok0 = 0
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 0
// bits[17:16] ReqTok1 = 0
// bits[19:18] PReqTok1 = 0
// bits[21:20] RspTok1 = 0
// bits[23:22] ProbeTok1= 0
// bits[24] IsocReqTok1 = 0
// bits[26] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                           // CpuRevision
    },
    {AMD_PF_MULTI_LINK},  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (3, COUNT_RANGE_HIGH) | COUNT_RANGE_NONE),  // MCM4
      PERFORMANCE_PROFILE_ALL,
      (HT_HOST_AND | HT_HOST_FEAT_COHERENT | HT_HOST_FEAT_GANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x00000196,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 1
// bits[3:2] PReqTok0 = 1
// bits[5:4] RspTok0 = 1
// bits[7:6] ProbeTok0 = 1
// bits[9:8] IsocReqTok0 = 1
// bits[11:10] IsocPreqTok0 = 0
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 0
// bits[17:16] ReqTok1 = 1
// bits[19:18] PReqTok1 = 1
// bits[21:20] RspTok1 = 1
// bits[23:22] ProbeTok1= 1
// bits[24] IsocReqTok1 = 0
// bits[26] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                           // CpuRevision
    },
    {AMD_PF_MULTI_LINK},  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (COUNT_RANGE_LOW, 1) | COUNT_RANGE_NONE), // MCM1 or MCM2h.
      PERFORMANCE_PROFILE_ALL,
      (HT_HOST_AND | HT_HOST_FEAT_COHERENT | HT_HOST_FEAT_UNGANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x00550155,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
  // F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 1
// bits[3:2] PReqTok0 = 1
// bits[5:4] RspTok0 = 2
// bits[7:6] ProbeTok0 = 1
// bits[9:8] IsocReqTok0 = 1
// bits[11:10] IsocPreqTok0 = 0
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 0
// bits[17:16] ReqTok1 = 1
// bits[19:18] PReqTok1 = 1
// bits[21:20] RspTok1 = 1
// bits[23:22] ProbeTok1= 1
// bits[24] IsocReqTok1 = 0
// bits[26] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                           // CpuRevision
    },
    {AMD_PF_MULTI_LINK},  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (COUNT_RANGE_LOW, 1) | COUNT_RANGE_NONE), // MCM1 or MCM2h.
      PERFORMANCE_PROBEFILTER,
      (HT_HOST_AND | HT_HOST_FEAT_COHERENT | HT_HOST_FEAT_UNGANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x00550165,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 1
// bits[3:2] PReqTok0 = 1
// bits[5:4] RspTok0 = 1
// bits[7:6] ProbeTok0 = 1
// bits[9:8] IsocReqTok0 = 1
// bits[11:10] IsocPreqTok0 = 0
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 2
// bits[17:16] ReqTok1 = 1
// bits[19:18] PReqTok1 = 1
// bits[21:20] RspTok1 = 1
// bits[23:22] ProbeTok1= 1
// bits[24] IsocReqTok1 = 1
// bits[26] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                           // CpuRevision
    },
    {AMD_PF_MULTI_LINK},  // platformFeatures
    {{
      (IGNORE_PROCESSOR_0 | DEGREE_RANGE_1 (3, 3)),     // MCM2
      PERFORMANCE_PROFILE_ALL,
      (HT_HOST_AND | HT_HOST_FEAT_COHERENT | HT_HOST_FEAT_UNGANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x01558155,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 1
// bits[3:2] PReqTok0 = 1
// bits[5:4] RspTok0 = 1
// bits[7:6] ProbeTok0 = 1
// bits[9:8] IsocReqTok0 = 1
// bits[11:10] IsocPreqTok0 = 0
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 0
// bits[17:16] ReqTok1 = 1
// bits[19:18] PReqTok1 = 1
// bits[21:20] RspTok1 = 1
// bits[23:22] ProbeTok1= 1
// bits[24] IsocReqTok1 = 1
// bits[26] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 1
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                           // CpuRevision
    },
    {AMD_PF_MULTI_LINK},  // platformFeatures
    {{
      (IGNORE_PROCESSOR_0 | DEGREE_RANGE_1 (2, 2)),     // MCM4h
      PERFORMANCE_PROFILE_ALL,
      (HT_HOST_AND | HT_HOST_FEAT_COHERENT | HT_HOST_FEAT_UNGANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x41550155,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 1
// bits[3:2] PReqTok0 = 1
// bits[5:4] RspTok0 = 1
// bits[7:6] ProbeTok0 = 1
// bits[9:8] IsocReqTok0 = 1
// bits[11:10] IsocPreqTok0 = 0
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 0
// bits[17:16] ReqTok1 = 1
// bits[19:18] PReqTok1 = 1
// bits[21:20] RspTok1 = 1
// bits[23:22] ProbeTok1= 1
// bits[24] IsocReqTok1 = 1
// bits[26] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_15,                        // CpuFamily
      AMD_F15_OR_ALL                           // CpuRevision
    },
    {AMD_PF_MULTI_LINK},  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (3, COUNT_RANGE_HIGH) | COUNT_RANGE_NONE),     // MCM4
      PERFORMANCE_PROFILE_ALL,
      (HT_HOST_AND | HT_HOST_FEAT_COHERENT | HT_HOST_FEAT_UNGANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x01550155,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 2
// bits[3:2] PReqTok0 = 2
// bits[5:4] RspTok0 = 2
// bits[7:6] ProbeTok0 = 0
// bits[9:8] IsocReqTok0 = 1
// bits[11:10] IsocPreqTok0 = 0
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 3
// bits[17:16] ReqTok1 = 0
// bits[19:18] PReqTok1 = 0
// bits[21:20] RspTok1 = 0
// bits[23:22] ProbeTok1= 0
// bits[24] IsocReqTok1 = 0
// bits[26] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_MULTI_LINK},  // platformFeatures
    {{
      (COUNT_RANGE_ALL | COUNT_RANGE_NONE),    //SCM
      PERFORMANCE_PROFILE_ALL,
      (HT_HOST_AND | HT_HOST_FEAT_NONCOHERENT  | HT_HOST_FEAT_GANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x0000C12A,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 2
// bits[3:2] PReqTok0 = 2
// bits[5:4] RspTok0 = 2
// bits[7:6] ProbeTok0 = 0
// bits[9:8] IsocReqTok0 = 1
// bits[11:10] IsocPreqTok0 = 0
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 0
// bits[17:16] ReqTok1 = 0
// bits[19:18] PReqTok1 = 0
// bits[21:20] RspTok1 = 0
// bits[23:22] ProbeTok1= 0
// bits[24] IsocReqTok1 = 0
// bits[26] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_MULTI_LINK},  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (COUNT_RANGE_LOW, 2) | COUNT_RANGE_NONE), // MCM1 or MCM2h or MCM2 or MCM4h
      PERFORMANCE_PROFILE_ALL,
      (HT_HOST_AND | HT_HOST_FEAT_NONCOHERENT  | HT_HOST_FEAT_GANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x0000012A,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 2
// bits[3:2] PReqTok0 = 2
// bits[5:4] RspTok0 = 2
// bits[7:6] ProbeTok0 = 2
// bits[9:8] IsocReqTok0 = 2
// bits[11:10] IsocPreqTok0 = 0
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 0
// bits[17:16] ReqTok1 = 0
// bits[19:18] PReqTok1 = 0
// bits[21:20] RspTok1 = 0
// bits[23:22] ProbeTok1= 0
// bits[24] IsocReqTok1 = 0
// bits[26] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_MULTI_LINK},  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (3, COUNT_RANGE_HIGH) | COUNT_RANGE_NONE),     // MCM4
      PERFORMANCE_PROFILE_ALL,
      (HT_HOST_AND | HT_HOST_FEAT_NONCOHERENT  | HT_HOST_FEAT_GANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x000002AA,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
  // F3x158 - Link to XCS Token Count Registers
  // bits [3:0]LnkToXcsDRToken = 0
  {
    PciRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_MULTI_LINK},
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x158), // Address
      0x00000000,
      0x0000000F
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F15OrMultiLinkPciRegisterTable = {
  PrimaryCores,
  (sizeof (F15OrMultiLinkPciRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F15OrMultiLinkPciRegisters,
};
