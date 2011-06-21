/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 PCI tables in Recommended Settings for Single Link Processors.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x10
 * @e \$Revision: 36159 $   @e \$Date: 2010-08-12 11:19:16 +0800 (Thu, 12 Aug 2010) $
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
#include "Ids.h"
#include "cpuRegisters.h"
#include "Table.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FAMILY_0X10_F10SINGLELINKPCITABLES_FILECODE

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

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F10SingleLinkPciRegisters[] =
{
// F0x68 - Link Transaction Control
// bit[14:13], BufPriRel = 01b
  {
    PciRegister,
    {
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_ALL                           // CpuRevision
    },
    AMD_PF_SINGLE_LINK,                     // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x68),  // Address
      0x00002000,                           // regData
      0x00006000,                           // regMask
    }
  },
// F0x68 - Link Transaction Control
// bit[24], DispRefModeEn = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_ALL                           // CpuRevision
    },
    AMD_PF_ALL,                             // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x68),  // Address
      0x00000000,                           // regData
      0x01000000,                           // regMask
    }
  },
// F0x68 - Link Transaction Control
// bit[24], DispRefModeEn = 1 for UMA, but can only set it on the warm reset.
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_ALL                           // CpuRevision
    },
    AMD_PF_UMA,                     // platform Features
    {
      PERFORMANCE_IS_WARM_RESET,
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x68),  // Address
      0x01000000,                           // regData
      0x01000000,                           // regMask
    }
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
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_ALL                           // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_NFCM),
    {
      HT_HOST_FEATURES_ALL,                 // Link Features
      0x10,                                 // Address
      0x04850292,                           // Data
      0x0FFFFFFF                            // Mask
    },
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
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_ALL                           // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),
    {
      HT_HOST_FEATURES_ALL,                 // Link Features
      0x10,                                 // Address
      0x04850292,                           // Data
      0x0FFFFFFF                            // Mask
    },
  },
  // F0x[F0,D0,B0,90] Link Base Buffer Count Register
  // 27:25 FreeData: 2
  // 24:20 FreeCmd: 8
  // 19:18 RspData: 1
  // 17:16 NpReqData: 1
  // 15:12 ProbeCmd: 0
  // 11:8 RspCmd: 2
  // 7:5 PReq: 3
  // 4:0 NpReqCmd: 11
  {
    HtHostPciRegister,
    {
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_ALL                           // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM),
    {
      HT_HOST_FEATURES_ALL,                 // Link Features
      0x10,                                 // Address
      0x0485026B,                           // Data
      0x0FFFFFFF                            // Mask
    },
  },
  // F0x[F0,D0,B0,90] Link Base Buffer Count Register
  // 27:25 FreeData: 2
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
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_ALL                           // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_IFCM),
    {
      HT_HOST_FEATURES_ALL,                 // Link Features
      0x10,                                 // Address
      0x008502CF,                           // Data
      0x0FFFFFFF                            // Mask
    },
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
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_ALL                           // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_NFCM),
    {
      HT_HOST_FEATURES_ALL,                 // Link Features
      0x14,                                 // Address
      0x00000000,                           // Data
      0x1FFF0000                            // Mask
    },
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
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_ALL                           // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),
    {
      HT_HOST_FEATURES_ALL,                 // Link Features
      0x14,                                 // Address
      0x00000000,                           // Data
      0x1FFF0000                            // Mask
    },
  },
  // F0x[F4,D4,B4,94] Link Base Buffer Count Register
  // 28:27 IsocRspData: 0
  // 26:25 IsocNpReqData: 0
  // 24:22 IsocRspCmd: 0
  // 21:19 IsocPReq: 1
  // 18:16 IsocNpReqCmd: 7
  {
    HtHostPciRegister,
    {
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_ALL                           // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM),
    {
      HT_HOST_FEATURES_ALL,                 // Link Features
      0x14,                                 // Address
      0x000F0000,                           // Data
      0x1FFF0000                            // Mask
    },
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
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_ALL                           // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_IFCM),
    {
      HT_HOST_FEATURES_ALL,                 // Link Features
      0x14,                                 // Address
      0x00010000,                           // Data
      0x1FFF0000                            // Mask
    },
  },
// F0x170 - Link Extended Control Register - Link 0, sublink 0
// bit[8] LS2En = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_ALL                           // CpuRevision
    },
    AMD_PF_SINGLE_LINK,                     // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_0, 0x170), // Address
      0x00000100,                           // regData
      0x00000100,                           // regMask
    }
  },
// F2x118 - Memory Controller Configuration Low Register
// bits[13:12] MctPriIsoc = 10b
// bits[31:28] MctVarPriCntLmt = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_UMA | AMD_PF_UMA_IFCM),        // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_2, 0x118),  // Address
      0x00002000,                           // regData
      0xF0003000,                           // regMask
    }
  },
// F2x118 - Memory Controller Configuration Low Register
// bits[13:12] MctPriIsoc = 11b
// bits[31:28] MctVarPriCntLmt = 1
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_UMA | AMD_PF_UMA_IFCM),       // platform Features
    {
      PERFORMANCE_MCT_ISOC_VARIABLE,        // Features
      MAKE_SBDFO (0, 0, 24, FUNC_2, 0x118),  // Address
      0x10003000,                           // regData
      0xF0003000,                           // regMask
    }
  },
// F2x[1,0]90 - DRAM Configuration Low Register
// bits [10] BurstLength32  0
// It is okay to write both channels, if one is disabled, this bit has no effect on that channel.
// If the channels are ganged for 128 bit operation, the memory init code will resolve any conflict with this setting.
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_UMA | AMD_PF_UMA_IFCM),        // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_2, 0x90),  // Address
      0x00000000,                           // regData
      0x00000400,                           // regMask
    }
  },
// F2x[1,0]90 - DRAM Configuration Low Register
// bits [10] BurstLength32 = 0
// It is okay to write both channels, if one is disabled, this bit has no effect on that channel.
// If the channels are ganged for 128 bit operation, the memory init code will resolve any conflict with this setting.
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_UMA | AMD_PF_UMA_IFCM),        // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_2, 0x190),  // Address
      0x00000000,                           // regData
      0x00000400,                           // regMask
    }
  },
// F2x[1,0]90 - DRAM Configuration Low Register
// bits [10] BurstLength32 = 1
// It is okay to write both channels, if one is disabled, this bit has no effect on that channel.
// If the channels are ganged for 128 bit operation, the memory init code will resolve any conflict with this setting.
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_UMA | AMD_PF_UMA_IFCM),        // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B,
      MAKE_SBDFO (0, 0, 24, FUNC_2, 0x90),  // Address
      0x00000400,                           // regData
      0x00000400,                           // regMask
    }
  },
// F2x[1,0]90 - DRAM Configuration Low Register
// bits [10] BurstLength32 = 1
// It is okay to write both channels, if one is disabled, this bit has no effect on that channel.
// If the channels are ganged for 128 bit operation, the memory init code will resolve any conflict with this setting.
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_UMA | AMD_PF_UMA_IFCM),        // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B,
      MAKE_SBDFO (0, 0, 24, FUNC_2, 0x190),  // Address
      0x00000400,                           // regData
      0x00000400,                           // regMask
    }
  },
// F3x6C - Data Buffer Control
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
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_NFCM), // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x6C),  // Address
      0x00018052,                           // regData
      0x700780F7,                           // regMask
    }
  },
// F3x6C - Data Buffer Control
// bits[2:0]   UpReqDBC = 1
// bits[5:4]   DnReqDBC = 1
// bits[7:6]   DnRspDBC = 1
// bit[15]     DatBuf24 = 1
// bits[18:16] UpRspDBC = 1
// bits[30:28] IsocRspDBC = 6
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA), // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x6C),  // Address
      0x60018051,                           // regData
      0x700780F7,                           // regMask
    }
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
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_IFCM), // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x6C),  // Address
      0x10018052,                           // regData
      0x700780F7,                           // regMask
    }
  },
// F3x6C - Data Buffer Control
// bits[2:0]   UpReqDBC = 1
// bits[5:4]   DnReqDBC = 1
// bits[7:6]   DnRspDBC = 1
// bit[15]     DatBuf24 = 1
// bits[18:16] UpRspDBC = 1
// bits[30:28] IsocRspDBC = 6
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM), // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x6C),  // Address
      0x60018051,                           // regData
      0x700780F7,                           // regMask
    }
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
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_NFCM), // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x70),  // Address
      0x00041153,                           // regData
      0x777777F7,                           // regMask
    }
  },
// F3x70 - SRI_to_XBAR Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[5:4]   DnReqCBC = 1
// bits[7:6]   DnRspCBC = 1
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 1
// bits[18:16] UpRspCBC = 2
// bits[22:20] IsocReqCBC = 2
// bits[26:24] IsocPreqCBC = 1
// bits[30:28] IsocRspCBC = 6
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA), // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x70),  // Address
      0x61221151,                           // regData
      0x777777F7,                           // regMask
    }
  },
// F3x70 - SRI_to_XBAR Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[5:4]   DnReqCBC = 1
// bits[7:6]   DnRspCBC = 1
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 1
// bits[18:16] UpRspCBC = 2
// bits[22:20] IsocReqCBC = 2
// bits[26:24] IsocPreqCBC = 1
// bits[30:28] IsocRspCBC = 6
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM), // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x70),  // Address
      0x61221151,                           // regData
      0x777777F7,                           // regMask
    }
  },
// F3x70 - SRI_to_XBAR Command Buffer Count
// bits[2:0]   UpReqCBC = 3
// bits[5:4]   DnReqCBC = 1
// bits[7:6]   DnRspCBC = 1
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 1
// bits[18:16] UpRspCBC = 4
// bits[22:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[30:28] IsocRspCBC = 1
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_IFCM), // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x70),  // Address
      0x11141153,                           // regData
      0x777777F7,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 1
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 1
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 0
// bits[26:24] IsocPreqCBC = 0
// bits[31:28] DRReqCBC = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_NFCM), // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0x00081111,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// No Mct Variable Priority or 32 byte requests.
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = 9
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0x91180101,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// No Mct Variable Priority or 32 byte requests.
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = 9
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM), // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0x91180101,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// No Mct Variable Priority or 32 byte requests.
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = 8
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      PERFORMANCE_PROFILE_ALL,  // Features
      (CORE_RANGE_0 (5, 5) | COUNT_RANGE_NONE),  // 5-cores
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0x81180101,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// No Mct Variable Priority or 32 byte requests.
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = 8
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM), // platform Features
    {
      PERFORMANCE_PROFILE_ALL,  // Features
      (CORE_RANGE_0 (5, 5) | COUNT_RANGE_NONE),  // 5-cores
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0x81180101,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// No Mct Variable Priority or 32 byte requests.
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = 7
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      PERFORMANCE_PROFILE_ALL,  // Features
      (CORE_RANGE_0 (6, 6) | COUNT_RANGE_NONE),  // 6-cores
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0x71180101,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// No Mct Variable Priority or 32 byte requests.
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = 7
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM), // platform Features
    {
      PERFORMANCE_PROFILE_ALL,  // Features
      (CORE_RANGE_0 (6, 6) | COUNT_RANGE_NONE),  // 6-cores
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0x71180101,                           // regData
      0xF7FF7777,                           // regMask
    }
  },

// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = C
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      PERFORMANCE_MCT_ISOC_VARIABLE,        // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0xC1180101,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = C
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM), // platform Features
    {
      PERFORMANCE_MCT_ISOC_VARIABLE,        // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0xC1181111,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = F
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B,        // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0xF1180101,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = F
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM), // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B,        // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0xF1181111,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = B
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B,        // Features
      (CORE_RANGE_0 (5, 5) | COUNT_RANGE_NONE),  // 5-cores
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0xB1180101,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = B
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM), // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B,        // Features
      (CORE_RANGE_0 (5, 5) | COUNT_RANGE_NONE),  // 5-cores
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0xB1180101,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = A
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B,        // Features
      (CORE_RANGE_0 (6, 6) | COUNT_RANGE_NONE),  // 6-cores
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0xA1180101,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = A
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM), // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B,        // Features
      (CORE_RANGE_0 (6, 6) | COUNT_RANGE_NONE),  // 6-cores
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0xA1180101,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = E
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B,        // Features
      (CORE_RANGE_0 (5, 5) | COUNT_RANGE_NONE),  // 5-cores
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0xE1180101,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = E
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM), // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B,        // Features
      (CORE_RANGE_0 (5, 5) | COUNT_RANGE_NONE),  // 5-cores
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0xE1180101,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = D
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B,        // Features
      (CORE_RANGE_0 (6, 6) | COUNT_RANGE_NONE),  // 6-cores
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0xD1180101,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 1
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = D
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM), // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B,        // Features
      (CORE_RANGE_0 (6, 6) | COUNT_RANGE_NONE),  // 6-cores
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0xD1180101,                           // regData
      0xF7FF7777,                           // regMask
    }
  },

// F3x74 - XBAR_to_SRI Command Buffer Count
// bits[2:0]   UpReqCBC = 1
// bits[6:4]   DnReqCBC = 0
// bits[10:8]  UpPreqCBC = 1
// bits[14:12] DnPreqCBC = 0
// bits[19:16] ProbeCBC = 8
// bits[23:20] IsocReqCBC = 8
// bits[26:24] IsocPreqCBC = 1
// bits[31:28] DRReqCBC = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_IFCM), // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x74),  // Address
      0x01880101,                           // regData
      0xF7FF7777,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 20
// bits[11:8] Sri2XbarFreeXreqCBC = 9
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 9
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_NFCM),  // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x00090914,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// No Mct Variable Priority or 32 byte requests.
// bits[4:0]  Xbar2SriFreeListCBC = 15
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x0007080F,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// No Mct Variable Priority or 32 byte requests.
// bits[4:0]  Xbar2SriFreeListCBC = 15
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM),  // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x0007080F,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 12
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      PERFORMANCE_MCT_ISOC_VARIABLE,        // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x0007080C,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 12
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM),  // platform Features
    {
      PERFORMANCE_MCT_ISOC_VARIABLE,        // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x0007080C,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 9
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B,              // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x00070809,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 9
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    ProfileFixup,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM),  // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B ,              // Features
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x00070809,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 17
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_IFCM),  // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x00070811,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// No Mct Variable Priority or 32 byte requests.
// bits[4:0]  Xbar2SriFreeListCBC = 14
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      PERFORMANCE_PROFILE_ALL,
      (CORE_RANGE_0 (5, 5) | COUNT_RANGE_NONE),  // 5 cores.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x0007080E,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// No Mct Variable Priority or 32 byte requests.
// bits[4:0]  Xbar2SriFreeListCBC = 14
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM),  // platform Features
    {
      PERFORMANCE_PROFILE_ALL,
      (CORE_RANGE_0 (5, 5) | COUNT_RANGE_NONE),  // 5 cores.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x0007080E,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// No Mct Variable Priority or 32 byte requests.
// bits[4:0]  Xbar2SriFreeListCBC = 13
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      PERFORMANCE_PROFILE_ALL,
      (CORE_RANGE_0 (6, 6) | COUNT_RANGE_NONE),  // 6 cores.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x0007080D,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// No Mct Variable Priority or 32 byte requests.
// bits[4:0]  Xbar2SriFreeListCBC = 13
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM),  // platform Features
    {
      PERFORMANCE_PROFILE_ALL,
      (CORE_RANGE_0 (6, 6) | COUNT_RANGE_NONE),  // 6 cores.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x0007080D,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 11
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      PERFORMANCE_MCT_ISOC_VARIABLE,        // Features
      (CORE_RANGE_0 (5, 5) | COUNT_RANGE_NONE),  // 5 cores.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x0007080B,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 11
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM),  // platform Features
    {
      PERFORMANCE_MCT_ISOC_VARIABLE,        // Features
      (CORE_RANGE_0 (5, 5) | COUNT_RANGE_NONE),  // 5 cores.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x0007080B,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 10
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      PERFORMANCE_MCT_ISOC_VARIABLE,        // Features
      (CORE_RANGE_0 (6, 6) | COUNT_RANGE_NONE),  // 6 cores.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x0007080A,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 10
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM),  // platform Features
    {
      PERFORMANCE_MCT_ISOC_VARIABLE,        // Features
      (CORE_RANGE_0 (6, 6) | COUNT_RANGE_NONE),  // 6 cores.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x0007080A,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 8
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B,              // Features
      (CORE_RANGE_0 (5, 5) | COUNT_RANGE_NONE),  // 5 cores.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x00070808,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 8
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM),  // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B ,              // Features
      (CORE_RANGE_0 (5, 5) | COUNT_RANGE_NONE),  // 5 cores.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x00070808,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 7
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B,              // Features
      (CORE_RANGE_0 (6, 6) | COUNT_RANGE_NONE),  // 6 cores.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x00070807,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 7
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM),  // platform Features
    {
      PERFORMANCE_REFRESH_REQUEST_32B ,              // Features
      (CORE_RANGE_0 (6, 6) | COUNT_RANGE_NONE),  // 6 cores.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x00070807,                           // regData
      0x007FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 16
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_IFCM),  // platform Features
    {
      PERFORMANCE_PROFILE_ALL,
      (CORE_RANGE_0 (5, 5) | COUNT_RANGE_NONE),  // 5 cores.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x00070810,                           // regData
      0x707FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 15
// bits[11:8] Sri2XbarFreeXreqCBC = 8
// bits[15:12] Sri2XbarFreeRspCBC = 0
// bits[19:16] Sri2XbarFreeXreqDBC = 7
// bits[22:20] Sri2XbarFreeRspDBC = 0
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_IFCM),  // platform Features
    {
      PERFORMANCE_PROFILE_ALL,
      (CORE_RANGE_0 (6, 6) | COUNT_RANGE_NONE),  // 6 cores.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),  // Address
      0x0007080F,                           // regData
      0x707FFF1F,                           // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 22, 1-core without L3 cache is 22
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    AMD_PF_SINGLE_LINK,                   // platformFeatures
    {
      PERFORMANCE_NO_L3_CACHE,
      (CORE_RANGE_0 (1, 1) | COUNT_RANGE_NONE), // 1 core.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),      // Address
      0x00000016,                               // regData
      0x0000001F,                               // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 20, 2-core is 20
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    AMD_PF_SINGLE_LINK,                   // platformFeatures
    {
      PERFORMANCE_NO_L3_CACHE,
      (CORE_RANGE_0 (2, 2) | COUNT_RANGE_NONE), // 2 core.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),      // Address
      0x00000014,                               // regData
      0x0000001F,                               // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 18, 3-core without L3 cache is 18.
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    AMD_PF_SINGLE_LINK,                   // platformFeatures
    {
      PERFORMANCE_NO_L3_CACHE,
      (CORE_RANGE_0 (3, 3) | COUNT_RANGE_NONE), // 3 core.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),      // Address
      0x00000012,                               // regData
      0x0000001F,                               // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 14, 4-core without L3 cache is 16.
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    AMD_PF_SINGLE_LINK,                   // platformFeatures
    {
      PERFORMANCE_NO_L3_CACHE,
      (CORE_RANGE_0 (4, 4) | COUNT_RANGE_NONE), // 4 core.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),      // Address
      0x00000010,                               // regData
      0x0000001F,                               // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 14, 5-core without L3 cache is 14.
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    AMD_PF_SINGLE_LINK,                   // platformFeatures
    {
      PERFORMANCE_NO_L3_CACHE,
      (CORE_RANGE_0 (5, 5) | COUNT_RANGE_NONE), // 5 core.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),      // Address
      0x0000000E,                               // regData
      0x0000001F,                               // regMask
    }
  },
// F3x7C - Free List Buffer Count
// bits[4:0]  Xbar2SriFreeListCBC = 12, 6-core without L3 cache is 12.
  {
    CoreCountsPciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    AMD_PF_SINGLE_LINK,                   // platformFeatures
    {
      PERFORMANCE_NO_L3_CACHE,
      (CORE_RANGE_0 (6, 6) | COUNT_RANGE_NONE), // 6 core.
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x7C),      // Address
      0x0000000C,                               // regData
      0x0000001F,                               // regMask
    }
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
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_NFCM),  // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00800756,                           // regData
      0x00F3FFFF,                           // regMask
    }
  },
// F3x140 - SRI_to_XCS Token Count
// bits[1:0] UpReqTok = 2
// bits[3:2] DnReqTok = 1
// bits[5:4] UpPreqTokC = 1
// bits[7:6] DnPreqTok = 1
// bits[9:8] UpRspTok = 3
// bits[11:10] DnRspTok = 1
// bits[13:12] IsocReqTok = 3
// bits[15:14] IsocPreqTok = 1
// bits[17:16] IsocRspTok = 3
// bits[23:20] FreeTok = 12
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00C37756,                           // regData
      0x00F3FFFF,                           // regMask
    }
  },
// F3x140 - SRI_to_XCS Token Count
// bits[1:0] UpReqTok = 2
// bits[3:2] DnReqTok = 1
// bits[5:4] UpPreqTokC = 1
// bits[7:6] DnPreqTok = 1
// bits[9:8] UpRspTok = 3
// bits[11:10] DnRspTok = 1
// bits[13:12] IsocReqTok = 3
// bits[15:14] IsocPreqTok = 1
// bits[17:16] IsocRspTok = 3
// bits[23:20] FreeTok = 12
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM),  // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00C37756,                           // regData
      0x00F3FFFF,                           // regMask
    }
  },
// F3x140 - SRI_to_XCS Token Count
// bits[1:0] UpReqTok = 2
// bits[3:2] DnReqTok = 1
// bits[5:4] UpPreqTokC = 1
// bits[7:6] DnPreqTok = 1
// bits[9:8] UpRspTok = 2
// bits[11:10] DnRspTok = 1
// bits[13:12] IsocReqTok = 3
// bits[15:14] IsocPreqTok = 1
// bits[17:16] IsocRspTok = 3
// bits[23:20] FreeTok = 12
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_IFCM),  // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x140), // Address
      0x00C37656,                           // regData
      0x00F3FFFF,                           // regMask
    }
  },
// F3x144 - MCT to XCS Token Count
// bits[3:0] RspTok = 3
// bits[7:4] ProbeTok = 3
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_NFCM),  // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x144), // Address
      0x00000033,                           // regData
      0x000000FF,                           // regMask
    }
  },
// F3x144 - MCT to XCS Token Count
// bits[3:0] RspTok = 6
// bits[7:4] ProbeTok = 3
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x144), // Address
      0x00000036,                           // regData
      0x000000FF,                           // regMask
    }
  },
// F3x144 - MCT to XCS Token Count
// bits[3:0] RspTok = 6
// bits[7:4] ProbeTok = 3
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM),  // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x144), // Address
      0x00000036,                           // regData
      0x000000FF,                           // regMask
    }
  },
// F3x144 - MCT to XCS Token Count
// bits[3:0] RspTok = 6
// bits[7:4] ProbeTok = 3
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_IFCM),  // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x144), // Address
      0x00000036,                           // regData
      0x000000FF,                           // regMask
    }
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
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_NFCM),  // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x0000C0AA,                           // regData
      0xD5FFFFFF,                           // regMask
    }
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 2
// bits[3:2] PReqTok0 = 2
// bits[5:4] RspTok0 = 2
// bits[7:6] ProbeTok0 = 0
// bits[9:8] IsocReqTok0 = 1
// bits[11:10] IsocPreqTok0 = 1
// bits[13:12] IsocRspTok0 = 1
// bits[15:14] FreeTok[1:0] = 0
// bits[17:16] ReqTok1 = 0
// bits[19:18] PReqTok1 = 0
// bits[21:20] RspTok1 = 0
// bits[23:22] ProbeTok1= 0
// bits[24] IsocReqTok1 = 0
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 2
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA),  // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x8000152A,                           // regData
      0xD5FFFFFF,                           // regMask
    }
  },
// F3x148 - Link to XCS Token Count
// bits[1:0] ReqTok0 = 2
// bits[3:2] PReqTok0 = 2
// bits[5:4] RspTok0 = 2
// bits[7:6] ProbeTok0 = 0
// bits[9:8] IsocReqTok0 = 1
// bits[11:10] IsocPreqTok0 = 1
// bits[13:12] IsocRspTok0 = 1
// bits[15:14] FreeTok[1:0] = 0
// bits[17:16] ReqTok1 = 0
// bits[19:18] PReqTok1 = 0
// bits[21:20] RspTok1 = 0
// bits[23:22] ProbeTok1= 0
// bits[24] IsocReqTok1 = 0
// bits[25] IsocPreqTok1 = 0
// bits[28] IsocRspTok1 = 0
// bits[31:30] FreeTok[3:2] = 2
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_UMA_IFCM),  // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x8000152A,                           // regData
      0xD5FFFFFF,                           // regMask
    }
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
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_AND | AMD_PF_SINGLE_LINK | AMD_PF_IFCM),  // platform Features
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x148), // Address
      0x0000C0AA,                           // regData
      0xD5FFFFFF,                           // regMask
    }
  },
  // F3x158 - Link to XCS Token Count Registers
  // bits [3:0]LnkToXcsDRToken = 0
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_NFCM | AMD_PF_IFCM | AMD_PF_IOMMU),
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x158), // Address
      0x00000000,
      0x0000000F
    }
  },
  // F3x158 - Link to XCS Token Count Registers
  // bits [3:0]LnkToXcsDRToken = 3
  {
    PciRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    (AMD_PF_UMA_IFCM | AMD_PF_UMA),
    {
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x158), // Address
      0x00000003,
      0x0000000F
    }
  },
};

CONST REGISTER_TABLE ROMDATA F10SingleLinkPciRegisterTable = {
  PrimaryCores,
  (sizeof (F10SingleLinkPciRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F10SingleLinkPciRegisters,
};
