/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 PCI tables from Multi-Link BKDG paragraph recommended settings.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x10
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
#include "Ids.h"
#include "cpuRegisters.h"
#include "Table.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X10_F10MULTILINKPCITABLES_FILECODE

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

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F10MultiLinkPciRegisters[] =
{
  // Function 0

// F0x68 - Link Transaction Control
// bit[14:13], BufPriRel = 02h
  {
    PciRegister,
    {
      AMD_FAMILY_10,                        // CpuFamily
      (AMD_F10_ALL & ~AMD_F10_Dx),          // CpuRevision  rev C or less.
    },
    {AMD_PF_MULTI_LINK},                     // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x68),  // Address
      0x00004000,                           // regData
      0x00006000,                           // regMask
    }}
  },
  // F0x[F0,D0,B0,90] Link Base Buffer Count Register
  // 27:25 FreeData: 2
  // 24:20 FreeCmd: 8
  // 19:18 RspData: 2
  // 17:16 NpReqData: 2
  // 15:12 ProbeCmd: 9
  // 11:8 RspCmd: 9
  // 7:5 PReq: 2
  // 4:0 NpReqCmd: 4
{
    HtHostPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      HT_HOST_FEAT_COHERENT,              // link features
      0x10,                               // address
      0x048A9944,                         // data
      0x0FFFFFFF                          // mask
    }}
  },
  // F0x[F0,D0,B0,90] Link Base Buffer Count Register
  // 27:25 FreeData: 2
  // 24:20 FreeCmd: 8
  // 19:18 RspData: 1
  // 17:16 NpReqData: 1
  // 15:12 ProbeCmd: 0
  // 11:8 RspCmd: 2
  // 7:5 PReq: 4
  // 4:0 NpReqCmd: 18
  {
    HtHostPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Cx                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      HT_HOST_FEAT_NONCOHERENT,           // link features
      0x10,                               // address
      0x04850292,                         // data
      0x0FFFFFFF                          // mask
    }}
  },
  // F0x[F0,D0,B0,90] Link Base Buffer Count Register
  // 27:25 FreeData: 0
  // 24:20 FreeCmd: 8
  // 19:18 RspData: 1
  // 17:16 NpReqData: 1
  // 15:12 ProbeCmd: 0
  // 11:8 RspCmd: 2
  // 7:5 PReq: 6
  // 4:0 NpReqCmd: 16
  {
    HtHostPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Dx                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      HT_HOST_FEAT_NONCOHERENT,           // link features
      0x10,                               // address
      0x008502D0,                         // data
      0x0FFFFFFF                          // mask
    }}
  },
  // F0x[F0,D0,B0,90] Link Base Buffer Count Register
  // 27:25 FreeData: 0
  // 24:20 FreeCmd: 8
  // 19:18 RspData: 3
  // 17:16 NpReqData: 2
  // 15:12 ProbeCmd: 8
  // 11:8 RspCmd: 9
  // 7:5 PReq: 2
  // 4:0 NpReqCmd: 4
  {
    HtHostPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      HT_HOST_FEAT_COHERENT,              // link features
      0x10,                               // address
      0x008E8944,                         // data
      0x0FFFFFFF                          // mask
    }}
  },
  // F0x[F0,D0,B0,90] Link Base Buffer Count Register
  // 27:25 FreeData: 0
  // 24:20 FreeCmd: 8
  // 19:18 RspData: 1
  // 17:16 NpReqData: 1
  // 15:12 ProbeCmd: 0
  // 11:8 RspCmd: 2
  // 7:5 PReq: 6
  // 4:0 NpReqCmd: 15
  {
    HtHostPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      HT_HOST_FEAT_NONCOHERENT,           // link features
      0x10,                               // address
      0x008502CF,                         // data
      0x0FFFFFFF                          // mask
    }}
  },
  // F0x[F4,D4,B4,94] Link Base Buffer Count Register
  // 28:27 IsocRspData: 0
  // 26:25 IsocNpReqData: 0
  // 24:22 IsocRspCmd: 0
  // 21:19 IsocPReq: 0
  // 18:16 IsocNpReqCmd: 0
{
    HtHostPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      HT_HOST_FEAT_COHERENT,              // link features
      0x14,                               // address
      0x00000000,                         // data
      0x1FFF0000                          // mask
    }}
  },
  // F0x[F4,D4,B4,94] Link Base Buffer Count Register
  // 28:27 IsocRspData: 0
  // 26:25 IsocNpReqData: 0
  // 24:22 IsocRspCmd: 0
  // 21:19 IsocPReq: 0
  // 18:16 IsocNpReqCmd: 0
  {
    HtHostPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      HT_HOST_FEAT_NONCOHERENT,           // link features
      0x14,                               // address
      0x00000000,                         // data
      0x1FFF0000                          // mask
    }}
  },
  // F0x[F4,D4,B4,94] Link Base Buffer Count Register
  // 28:27 IsocRspData: 0
  // 26:25 IsocNpReqData: 1
  // 24:22 IsocRspCmd: 0
  // 21:19 IsocPReq: 0
  // 18:16 IsocNpReqCmd: 1
  {
    HtHostPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      HT_HOST_FEAT_COHERENT,              // link features
      0x14,                               // address
      0x02010000,                         // data
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
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      HT_HOST_FEAT_NONCOHERENT,           // link features
      0x14,                               // address
      0x00010000,                         // data
      0x1FFF0000                          // mask
    }}
  },

// Function 3 - Misc. Control

// F3x6C - Data Buffer Control
// XBAR buffer settings
// bits[2:0]   UpReqDBC = 2
// bits[5:4]   DnReqDBC = 1
// bits[7:6]   DnRspDBC = 1
// bit[15]     DatBuf24 = 1
// bits[18:16] UpRspDBC = 1
// bits[30:28] IsocRspDBC = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Cx                          // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      MAKE_SBDFO(0, 0, 24, FUNC_3, 0x6C),   // Address
      0x00018052,                           // regData
      0x700780F7,                           // regMask
    }}
  },
// F3x6C - Data Buffer Control
// XBAR buffer settings
// bits[2:0]   UpReqDBC = 2
// bits[5:4]   DnReqDBC = 1
// bits[7:6]   DnRspDBC = 1
// bit[15]     DatBuf24 = 1
// bits[18:16] UpRspDBC = 2
// bits[30:28] IsocRspDBC = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Dx                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      MAKE_SBDFO(0, 0, 24, FUNC_3, 0x6C),   // Address
      0x00028052,                           // regData
      0x700780F7,                           // regMask
    }}
  },
// F3x6C - Data Buffer Control
// bits[2:0]   UpReqDBC = 2
// bits[5:4]   DnReqDBC = 1
// bits[7:6]   DnRspDBC = 1
// bit[15]     DatBuf24 = 1
// bits[18:16] UpRspDBC = 1
// bits[30:28] IsocRspDBC = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x6C),  // Address
      0x10018052,                           // regData
      0x700780F7,                           // regMask
    }}
  },
// F3x70 - SRI_to_XBAR Command Buffer Count
// bits[2:0]   UpReqCBC = 3
// bits[5:4]   DnReqCBC = 1
// bits[7:6]   DnRspCBC = 1
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 1
// bits[18:16] UpRspCBC = 4
// bits[22:20] IsocReqCBC = 0
// bits[26:24] IsocPreqCBC = 0
// bits[30:28] IsocRspCBC = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Cx                          // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x70),  // Address
      0x00041153,                           // regData
      0x777777F7,                           // regMask
    }}
  },
// F3x70 - SRI_to_XBAR Command Buffer Count
// bits[2:0]   UpReqCBC = 3
// bits[5:4]   DnReqCBC = 1
// bits[7:6]   DnRspCBC = 1
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 1
// bits[18:16] UpRspCBC = 5
// bits[22:20] IsocReqCBC = 0
// bits[26:24] IsocPreqCBC = 0
// bits[30:28] IsocRspCBC = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Dx                          // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x70),  // Address
      0x00051153,                           // regData
      0x777777F7,                           // regMask
    }}
  },
// F3x70 - SRI_to_XBAR Command Buffer Count
// bits[2:0]   UpReqCBC = 3
// bits[5:4]   DnReqCBC = 1
// bits[7:6]   DnRspCBC = 1
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 1
// bits[18:16] UpRspCBC = 5
// bits[22:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 0
// bits[30:28] IsocRspCBC = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x70),  // Address
      0x10151153,                           // regData
      0x777777F7,                           // regMask
    }}
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 1
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 1
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 0
// bits[26:24] IsocPreqCBC = 0
// bits[30:28] DRReqCBC = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0x00081111,                           // regData
      0x00FF7777,                           // regMask
    }}
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 1
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 1
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 0
// bits[30:28] DRReqCBC = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0x00181111,                           // regData
      0x00FF7777,                           // regMask
    }}
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 20
// bits[11:8] Sri2XbarFreeXreqCBC = 9
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 9
// bits[22:20] Sri2XbarFreeRspDBC = 0
// bits[30:28] Xbar2SriFreeListCBInc = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Cx                          // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x00090914,                           // regData
      0x707FFF1F,                           // regMask
    }}
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 24
// bits[11:8] Sri2XbarFreeXreqCBC = 9
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 9
// bits[22:20] Sri2XbarFreeRspDBC = 0
// bits[30:28] Xbar2SriFreeListCBInc = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Dx                          // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      (CORE_RANGE_0 (COUNT_RANGE_LOW, 4) | COUNT_RANGE_NONE),   // 4 or fewer cores.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x00090A18,                           // regData
      0x007FFF1F,                           // regMask
    }}
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 22
// bits[11:8] Sri2XbarFreeXreqCBC = 9
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 9
// bits[22:20] Sri2XbarFreeRspDBC = 0
// bits[30:28] Xbar2SriFreeListCBInc = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Dx                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      (CORE_RANGE_0 (5, COUNT_RANGE_HIGH) | COUNT_RANGE_NONE),  // greater than 4, ex. 6.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x00090A16,                           // regData
      0x707FFF1F,                           // regMask
    }}
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 23
// bits[11:8] Sri2XbarFreeXreqCBC = 9
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 9
// bits[22:20] Sri2XbarFreeRspDBC = 0
// bits[30:28] Xbar2SriFreeListCBInc = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Cx                          // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x00090917,                           // regData
      0x707FFF1F,                           // regMask
    }}
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 23
// bits[11:8] Sri2XbarFreeXreqCBC = 9
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 9
// bits[22:20] Sri2XbarFreeRspDBC = 0
// bits[30:28] Xbar2SriFreeListCBInc = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Dx                          // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      (CORE_RANGE_0 (COUNT_RANGE_LOW, 4) | COUNT_RANGE_NONE),   // 4 or fewer cores.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x00090917,                           // regData
      0x007FFF1F,                           // regMask
    }}
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 21
// bits[11:8] Sri2XbarFreeXreqCBC = 9
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 9
// bits[22:20] Sri2XbarFreeRspDBC = 0
// bits[30:28] Xbar2SriFreeListCBInc = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_Dx                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      (CORE_RANGE_0 (5, COUNT_RANGE_HIGH) | COUNT_RANGE_NONE),  // greater than 4, ex. 6.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x00090915,                           // regData
      0x707FFF1F,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[1:0] UpReqTok = 1
// bits[3:2] DnReqTok = 1
// bits[5:4] UpPreqTokC = 1
// bits[7:6] DnPreqTok = 1
// bits[9:8] UpRspTok = 3
// bits[11:10] DnRspTok = 1
// bits[13:12] IsocReqTok = 0
// bits[15:14] IsocPreqTok = 0
// bits[17:16] IsocRspTok = 0
// bits[23:20] FreeTok = A
  {
    ProcCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      (PROCESSOR_RANGE_0 (COUNT_RANGE_LOW, 1) | PROCESSOR_RANGE_1 (3, COUNT_RANGE_HIGH)),  // anything but two.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00A00755,                           // regData
      0x00F3FFFF,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[1:0] UpReqTok = 1
// bits[3:2] DnReqTok = 1
// bits[5:4] UpPreqTokC = 1
// bits[7:6] DnPreqTok = 1
// bits[9:8] UpRspTok = 3
// bits[11:10] DnRspTok = 1
// bits[13:12] IsocReqTok = 0
// bits[15:14] IsocPreqTok = 0
// bits[17:16] IsocRspTok = 0
// bits[23:20] FreeTok = 8
  {
    ProcCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      (PROCESSOR_RANGE_0 (2, 2) | COUNT_RANGE_NONE),   // exactly two.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00800755,                           // regData
      0x00F3FFFF,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[1:0] UpReqTok = 1
// bits[3:2] DnReqTok = 1
// bits[5:4] UpPreqTokC = 1
// bits[7:6] DnPreqTok = 1
// bits[9:8] UpRspTok = 3
// bits[11:10] DnRspTok = 1
// bits[13:12] IsocReqTok = 1
// bits[15:14] IsocPreqTok = 0
// bits[17:16] IsocRspTok = 1
// bits[23:20] FreeTok = 10
  {
    TokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      (DEGREE_RANGE_0 (COUNT_RANGE_LOW, 1) | COUNT_RANGE_NONE),     // 2 Socket, half populated.
      PACKAGE_TYPE_ALL,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00A11755,                           // regData
      0x00F3FFFF,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[1:0] UpReqTok = 1
// bits[3:2] DnReqTok = 1
// bits[5:4] UpPreqTokC = 1
// bits[7:6] DnPreqTok = 1
// bits[9:8] UpRspTok = 3
// bits[11:10] DnRspTok = 1
// bits[13:12] IsocReqTok = 1
// bits[15:14] IsocPreqTok = 0
// bits[17:16] IsocRspTok = 1
// bits[23:20] FreeTok = 9
  {
    TokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      PERFORMANCE_PROBEFILTER,
      (DEGREE_RANGE_0 (COUNT_RANGE_LOW, 1) | COUNT_RANGE_NONE),     // 2 Socket, half populated.
      PACKAGE_TYPE_ALL,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00911755,                           // regData
      0x00F3FFFF,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[1:0] UpReqTok = 1
// bits[3:2] DnReqTok = 1
// bits[5:4] UpPreqTokC = 1
// bits[7:6] DnPreqTok = 1
// bits[9:8] UpRspTok = 3
// bits[11:10] DnRspTok = 1
// bits[13:12] IsocReqTok = 1
// bits[15:14] IsocPreqTok = 0
// bits[17:16] IsocRspTok = 1
// bits[23:20] FreeTok = 5
  {
    TokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      (DEGREE_RANGE_0 (3, 3) | COUNT_RANGE_NONE),     // 2 Socket, fully populated.
      PACKAGE_TYPE_ALL,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00511755,                           // regData
      0x00F3FFFF,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[1:0] UpReqTok = 1
// bits[3:2] DnReqTok = 1
// bits[5:4] UpPreqTokC = 1
// bits[7:6] DnPreqTok = 1
// bits[9:8] UpRspTok = 1
// bits[11:10] DnRspTok = 1
// bits[13:12] IsocReqTok = 1
// bits[15:14] IsocPreqTok = 0
// bits[17:16] IsocRspTok = 1
// bits[23:20] FreeTok = 7
  {
    TokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      PERFORMANCE_PROBEFILTER,
      (DEGREE_RANGE_0 (3, 3) | COUNT_RANGE_NONE),     // 2 Socket, fully populated.
      PACKAGE_TYPE_ALL,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00711555,                           // regData
      0x00F3FFFF,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[1:0] UpReqTok = 1
// bits[3:2] DnReqTok = 1
// bits[5:4] UpPreqTokC = 1
// bits[7:6] DnPreqTok = 1
// bits[9:8] UpRspTok = 3
// bits[11:10] DnRspTok = 1
// bits[13:12] IsocReqTok = 1
// bits[15:14] IsocPreqTok = ]
// bits[17:16] IsocRspTok = 1
// bits[23:20] FreeTok = 8
  {
    TokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      (DEGREE_RANGE_0 (2, 2) | COUNT_RANGE_NONE),     // 4 Socket, half populated.
      PACKAGE_TYPE_ALL,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00811755,                           // regData
      0x00F3FFFF,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[1:0] UpReqTok = 1
// bits[3:2] DnReqTok = 1
// bits[5:4] UpPreqTokC = 1
// bits[7:6] DnPreqTok = 1
// bits[9:8] UpRspTok = 3
// bits[11:10] DnRspTok = 1
// bits[13:12] IsocReqTok = 1
// bits[15:14] IsocPreqTok = 0
// bits[17:16] IsocRspTok = 2
// bits[23:20] FreeTok = 2
  {
    TokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      (DEGREE_RANGE_0 (4, COUNT_RANGE_HIGH) | COUNT_RANGE_NONE),     // 4 Socket, fully populated.
      PACKAGE_TYPE_ALL,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00211755,                           // regData
      0x00F3FFFF,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[1:0] UpReqTok = 1
// bits[3:2] DnReqTok = 1
// bits[5:4] UpPreqTokC = 1
// bits[7:6] DnPreqTok = 1
// bits[9:8] UpRspTok = 1
// bits[11:10] DnRspTok = 1
// bits[13:12] IsocReqTok = 1
// bits[15:14] IsocPreqTok = 0
// bits[17:16] IsocRspTok = 1
// bits[23:20] FreeTok = 6
  {
    TokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      PERFORMANCE_PROBEFILTER,
      (DEGREE_RANGE_0 (4, COUNT_RANGE_HIGH) | COUNT_RANGE_NONE),     // 4 Socket, fully populated.
      PACKAGE_TYPE_ALL,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00611555,                           // regData
      0x00F3FFFF,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[1:0] UpReqTok = 2
// bits[3:2] DnReqTok = 1
// bits[5:4] UpPreqTokC = 1
// bits[7:6] DnPreqTok = 1
// bits[9:8] UpRspTok = 3
// bits[11:10] DnRspTok = 1
// bits[13:12] IsocReqTok = 0
// bits[15:14] IsocPreqTok = 0
// bits[17:16] IsocRspTok = 0
// bits[23:20] FreeTok = 8
  {
    TokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_C32_ALL                     // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      (COUNT_RANGE_ALL | COUNT_RANGE_NONE),
      PACKAGE_TYPE_ALL,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00800756,                           // regData
      0x00F3FFFF,                           // regMask
    }}
  },
// F3x140 - SRI_to_XCS Token Count
// bits[1:0] UpReqTok = 2
// bits[3:2] DnReqTok = 1
// bits[5:4] UpPreqTokC = 1
// bits[7:6] DnPreqTok = 1
// bits[9:8] UpRspTok = 3
// bits[11:10] DnRspTok = 1
// bits[13:12] IsocReqTok = 1
// bits[15:14] IsocPreqTok = 0
// bits[17:16] IsocRspTok = 1
// bits[23:20] FreeTok = 8
  {
    TokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_C32_ALL                     // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      (COUNT_RANGE_ALL | COUNT_RANGE_NONE),
      PACKAGE_TYPE_ALL,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00811756,                           // regData
      0x00F3FFFF,                           // regMask
    }}
  },
// F3x144 - MCT to XCS Token Count
// bits[3:0] RspTok = 3
// bits[7:4] ProbeTok = 3
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x144), // Address
      0x00000033,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// F3x144 - MCT to XCS Token Count
// bits[3:0] RspTok = 5
// bits[7:4] ProbeTok = 1
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      PERFORMANCE_PROBEFILTER,              // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x144), // Address
      0x00000015,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// F3x144 - MCT to XCS Token Count
// All non probe filter configs
// bits[3:0] RspTok = 3
// bits[7:4] ProbeTok = 3
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      PERFORMANCE_PROFILE_ALL,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x144), // Address
      0x00000033,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// F3x144 - MCT to XCS Token Count
// bits[3:0] RspTok = 4
// bits[7:4] ProbeTok = 1
  {
    TokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      PERFORMANCE_PROBEFILTER,              // Features
      (DEGREE_RANGE_0 (COUNT_RANGE_LOW, 1) | DEGREE_RANGE_1 (4, COUNT_RANGE_HIGH)),     // 2 Socket, half populated, or 4 Socket, fully populated.
      PACKAGE_TYPE_ALL,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x144), // Address
      0x00000014,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// F3x144 - MCT to XCS Token Count
// bits[3:0] RspTok = 5
// bits[7:4] ProbeTok = 1
  {
    TokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      PERFORMANCE_PROBEFILTER,              // Features
      (DEGREE_RANGE_0 (2, 2) | DEGREE_RANGE_1 (3, 3)),     // 2 Socket, fully populated, or 4 Socket, half populated.
      PACKAGE_TYPE_ALL,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x144), // Address
      0x00000015,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// F3x144 - MCT to XCS Token Count
// bits[3:0] RspTok = 5
// bits[7:4] ProbeTok = 1
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_C32_ALL                     // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      PERFORMANCE_PROBEFILTER,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x144), // Address
      0x00000015,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 2
// bits[3:2] PReqTok0 = 2
// bits[5:4] RspTok0 = 2
// bits[7:6] ProbeTok0 = 2
// bits[9:8] IsocReqTok0 = 0
// bits[11:10] IsocPreqTok0 = 0
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 0
// bits[17:16] ReqTok1 = 0
// bits[19:18] PReqTok1 = 0
// bits[21:20] RspTok1 = 0
// bits[23:22] ProbeTok1= 0
// bits[24] IsocReqTok1 = 0
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      (COUNT_RANGE_ALL | COUNT_RANGE_NONE),
      PERFORMANCE_PROFILE_ALL,
      HT_HOST_FEAT_GANGED,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x000000AA,                           // regData
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
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (COUNT_RANGE_LOW, 1) | COUNT_RANGE_NONE),
      PERFORMANCE_PROFILE_ALL,
      HT_HOST_FEAT_UNGANGED,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x00550055,                           // regData
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
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (3, COUNT_RANGE_HIGH) | COUNT_RANGE_NONE),
      PERFORMANCE_PROFILE_ALL,
      HT_HOST_FEAT_UNGANGED,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x00550055,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 1
// bits[3:2] PReqTok0 = 1
// bits[5:4] RspTok0 = 1
// bits[7:6] ProbeTok0 = 1
// bits[9:8] IsocReqTok0 = 0
// bits[11:10] IsocPreqTok0 = 0
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 1
// bits[17:16] ReqTok1 = 1
// bits[19:18] PReqTok1 = 1
// bits[21:20] RspTok1 = 1
// bits[23:22] ProbeTok1= 1
// bits[24] IsocReqTok1 = 0
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (2, 2) | COUNT_RANGE_NONE),
      PERFORMANCE_PROFILE_ALL,
      HT_HOST_FEAT_UNGANGED,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x00554055,                           // regData
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
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (COUNT_RANGE_LOW, 2) | COUNT_RANGE_NONE),
      PERFORMANCE_PROFILE_ALL,
      HT_HOST_FEAT_NONCOHERENT,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x0000012A,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 2
// bits[3:2] PReqTok0 = 1
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
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (COUNT_RANGE_LOW, 2) | COUNT_RANGE_NONE),
      PERFORMANCE_PROFILE_ALL,
      (HT_HOST_AND | HT_HOST_FEAT_COHERENT | HT_HOST_FEAT_GANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x000001A6,                           // regData
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
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (COUNT_RANGE_LOW, 2) | COUNT_RANGE_NONE),
      PERFORMANCE_PROBEFILTER,
      (HT_HOST_AND | HT_HOST_FEAT_COHERENT | HT_HOST_FEAT_GANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x0000016A,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 1
// bits[3:2] PReqTok0 = 1
// bits[5:4] RspTok0 = 1
// bits[7:6] ProbeTok0 = 1
// bits[9:8] IsocReqTok0 = 1
// bits[11:10] IsocPreqTok0 = 1
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 0
// bits[17:16] ReqTok1 = 1
// bits[19:18] PReqTok1 = 1
// bits[21:20] RspTok1 = 1
// bits[23:22] ProbeTok1= 1
// bits[24] IsocReqTok1 = 1
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (COUNT_RANGE_LOW, 1) | COUNT_RANGE_NONE),
      PERFORMANCE_PROFILE_ALL,
      (HT_HOST_AND | HT_HOST_FEAT_COHERENT | HT_HOST_FEAT_UNGANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x01550155,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 1
// bits[3:2] PReqTok0 = 1
// bits[5:4] RspTok0 = 1
// bits[7:6] ProbeTok0 = 1
// bits[9:8] IsocReqTok0 = 1
// bits[11:10] IsocPreqTok0 = 1
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 0
// bits[17:16] ReqTok1 = 1
// bits[19:18] PReqTok1 = 1
// bits[21:20] RspTok1 = 1
// bits[23:22] ProbeTok1= 1
// bits[24] IsocReqTok1 = 1
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (3, COUNT_RANGE_HIGH) | COUNT_RANGE_NONE),
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
// bits[9:8] IsocReqTok0 = 2
// bits[11:10] IsocPreqTok0 = 0
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 0
// bits[17:16] ReqTok1 = 0
// bits[19:18] PReqTok1 = 0
// bits[21:20] RspTok1 = 0
// bits[23:22] ProbeTok1= 0
// bits[24] IsocReqTok1 = 0
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (3, COUNT_RANGE_HIGH) | COUNT_RANGE_NONE),
      PERFORMANCE_PROFILE_ALL,
      HT_HOST_FEAT_NONCOHERENT,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x0000022A,                           // regData
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
// bits[15:14] FreeTok[1:0] = 1
// bits[17:16] ReqTok1 = 1
// bits[19:18] PReqTok1 = 1
// bits[21:20] RspTok1 = 1
// bits[23:22] ProbeTok1= 1
// bits[24] IsocReqTok1 = 1
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (2, 2) | COUNT_RANGE_NONE),
      PERFORMANCE_PROFILE_ALL,
      (HT_HOST_AND | HT_HOST_FEAT_COHERENT | HT_HOST_FEAT_UNGANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x01554155,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 2
// bits[3:2] PReqTok0 = 1
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
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (3, COUNT_RANGE_HIGH) | COUNT_RANGE_NONE),
      PERFORMANCE_PROFILE_ALL,
      (HT_HOST_AND | HT_HOST_FEAT_COHERENT | HT_HOST_FEAT_GANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x000001A6,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 2
// bits[3:2] PReqTok0 = 1
// bits[5:4] RspTok0 =1
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
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      (PROCESSOR_RANGE_0 (3, COUNT_RANGE_HIGH) | COUNT_RANGE_NONE),
      PERFORMANCE_PROBEFILTER,
      (HT_HOST_AND | HT_HOST_FEAT_COHERENT | HT_HOST_FEAT_GANGED),
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x00000196,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 2
// bits[3:2] PReqTok0 = 2
// bits[5:4] RspTok0 = 2
// bits[7:6] ProbeTok0 = 2
// bits[9:8] IsocReqTok0 = 0
// bits[11:10] IsocPreqTok0 = 0
// bits[13:12] IsocRspTok0 = 0
// bits[15:14] FreeTok[1:0] = 3
// bits[17:16] ReqTok1 = 0
// bits[19:18] PReqTok1 = 0
// bits[21:20] RspTok1 = 0
// bits[23:22] ProbeTok1= 0
// bits[24] IsocReqTok1 = 0
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_C32_ALL                     // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_NFCM) },  // platformFeatures
    {{
      (COUNT_RANGE_ALL | COUNT_RANGE_NONE),
      PERFORMANCE_PROFILE_ALL,
      HT_HOST_FEATURES_ALL,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x0000C0AA,                           // regData
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
// bits[15:14] FreeTok[1:0] = 2
// bits[17:16] ReqTok1 = 0
// bits[19:18] PReqTok1 = 0
// bits[21:20] RspTok1 = 0
// bits[23:22] ProbeTok1= 0
// bits[24] IsocReqTok1 = 0
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_C32_ALL                     // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      (COUNT_RANGE_ALL | COUNT_RANGE_NONE),
      PERFORMANCE_PROFILE_ALL,
      HT_HOST_FEAT_NONCOHERENT,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x0000812A,                           // regData
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
// bits[15:14] FreeTok[1:0] = 2
// bits[17:16] ReqTok1 = 0
// bits[19:18] PReqTok1 = 0
// bits[21:20] RspTok1 = 0
// bits[23:22] ProbeTok1= 0
// bits[24] IsocReqTok1 = 0
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 0
  {
    HtTokenPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_C32_ALL                     // CpuRevision
    },
    { (AMD_PF_AND | AMD_PF_MULTI_LINK | AMD_PF_IOMMU) },  // platformFeatures
    {{
      (COUNT_RANGE_ALL | COUNT_RANGE_NONE),
      PERFORMANCE_PROFILE_ALL,
      HT_HOST_FEAT_COHERENT,
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x000081AA,                           // regData
      0xD5FFFFFF,                           // regMask
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F10MultiLinkPciRegisterTable = {
  PrimaryCores,
  (sizeof (F10MultiLinkPciRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F10MultiLinkPciRegisters,
};
