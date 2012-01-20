/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Orochi Ht Phy tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/OR
 * @e \$Revision: 53592 $   @e \$Date: 2011-05-23 00:27:15 -0600 (Mon, 23 May 2011) $
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

#define FILECODE PROC_CPU_FAMILY_0X15_REVD_HY_F15HYHTPHYTABLES_FILECODE


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

//  HT   Phy   T a b l e s
// -------------------------
STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F15OrHtPhyRegisters[] =
{
//
// All the entries for XmtRdPtr
//
// 0xCF
// HT_PHY_HT1_FIFO_PTR_OPT_VALUE
  {
    HtPhyRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL0_HT1,               //
      0xCF,                                 // Address
      0x00000D4D,                           // regData
      0x0000FFFF,                           // regMask
    }}
  },
// 0xDF
// HT_PHY_HT1_FIFO_PTR_OPT_VALUE
  {
    HtPhyRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL1_HT1,               //
      0xDF,                                 // Address
      0x00000D4D,                           // regData
      0x0000FFFF,                           // regMask
    }}
  },
// 0xCF
// Default for HT3, unless overridden below.
  {
    HtPhyRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xCF,                                 // Address
      0x00000A2A,                           // regData
      0x0000FFFF,                           // regMask
    }}
  },
// 0xDF
// Default for HT3, unless overridden below.
  {
    HtPhyRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xDF,                                 // Address
      0x00000A2A,                           // regData
      0x0000FFFF,                           // regMask
    }}
  },
// 0xC1
// [29:22] LfcMax = 20h, [21:14] LfcMin = 10h
  {
    HtPhyRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC1,                                 // Address
      0x08040000,                           // regData
      0x3FFFC000,                           // regMask
    }}
  },
// 0xD1
// [29:22] LfcMax = 20h, [21:14] LfcMin = 10h
  {
    HtPhyRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD1,                                 // Address
      0x08040000,                           // regData
      0x3FFFC000,                           // regMask
    }}
  },
// 0xC1
// [29:22] LfcMax = 10h, [21:14] LfcMin = 08h
  {
    HtPhyRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL0_HT1,               //
      0xC1,                                 // Address
      0x04020000,                           // regData
      0x3FFFC000,                           // regMask
    }}
  },
// 0xD1
// [29:22] LfcMax = 10h, [21:14] LfcMin = 08h
  {
    HtPhyRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL1_HT1,               //
      0xD1,                                 // Address
      0x04020000,                           // regData
      0x3FFFC000,                           // regMask
    }}
  },
// 0xC5
// [7] TxLs23ClkGateEn = 1
  {
    HtPhyRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_ALL,               //
      0xC5,                                 // Address
      0x00000080,                           // regData
      0x00000080,                           // regMask
    }}
  },
// 0xD5
// [7] TxLs23ClkGateEn = 1
  {
    HtPhyRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_ALL,               //
      0xD5,                                 // Address
      0x00000080,                           // regData
      0x00000080,                           // regMask
    }}
  },

//
// Deemphasis Settings
//
// HT1: clear any warm reset deemphasis settings.

  {
    HtPhyRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL0_HT1,               //
      0xC4,                                 // Address
      0x00000000,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    HtPhyRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL1_HT1,               //
      0xD4,                                 // Address
      0x00000000,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    HtPhyRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL0_HT1,               //
      0x720C,                                 // Address
      0x00000000,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
  {
    HtPhyRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL1_HT1,               //
      0x730C,                                 // Address
      0x00000000,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },

//deemphasis level        Post2[31, 24]    Post1[23, 16]    Pre1[15, 8]    Margin[7, 0]
// No deemphasis               00h              00h             00h            00h
// -3dB postcursor             00h              26h             00h            00h
// -6dB postcursor             00h              40h             00h            00h
// -8dB postcursor             00h              4Dh             00h            00h
// -11dB postcursor            00h              5Ch             00h            00h
//                             00h              4Dh             0Fh            00h
// -11dB postcursor with -8dB precursor

  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL_NONE,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0x720C,                                 // Address
      0x00000000,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL_NONE,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0x730C,                                 // Address
      0x00000000,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__3,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0x720C,                                 // Address
      0x00260000,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__3,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0x730C,                                 // Address
      0x00260000,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__6,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0x720C,                                 // Address
      0x00400000,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__6,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0x730C,                                 // Address
      0x00400000,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__8,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0x720C,                                 // Address
      0x004D0000,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__8,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0x730C,                                 // Address
      0x004D0000,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__11,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0x720C,                                 // Address
      0x005C0000,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__11,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0x730C,                                 // Address
      0x005C0000,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__11_8,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0x720C,                                 // Address
      0x004D0F00,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__11_8,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0x730C,                                 // Address
      0x004D0F00,                           // regData
      0xFFFFFFFF,                           // regMask
    }}
  },

// Far-device deemphasis setting DCV[15:10]
// No deemphasis                 4Dh
// -2dB postcursor               3Dh
// -3dB postcursor               36h
// -5dB postcursor               2Bh
// -6dB postcursor               27h
// -7dB postcursor               22h
// -8dB postcursor               1Fh
// -9dB postcursor               1Bh
// -11dB postcursor              16h

  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL_NONE,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x00013400,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL_NONE,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x00013400,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__2,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x0000F400,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__2,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x0000F400,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__3,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x0000D800,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__3,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x0000D800,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__5,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x0000AC00,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__5,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x0000AC00,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__6,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x00009C00,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__6,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x00009C00,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__7,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x00008800,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__7,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x00008800,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__8,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x00007C00,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__8,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x00007C00,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__9,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x00006C00,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__9,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x00006C00,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__11,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x00005800,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__11,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x00005800,                           // regData
      0x0003FC00,                           // regMask
    }}
  },
// 0x520A
// [14:13] AnalogWaitTime = 10b
  {
    HtPhyRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL0_ALL,                   //
      0x520A,                               // Address
      0x00004000,                           // regData
      0x00006000,                           // regMask
    }}
  },
// 0x530A
// [14:13] AnalogWaitTime = 10b
  {
    HtPhyRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL1_ALL,                   //
      0x530A,                               // Address
      0x00004000,                           // regData
      0x00006000,                           // regMask
    }}
  },
// 0xE3
// [7] RoCalEn = 1b
  {
    HtPhyRegister,
    {
      AMD_FAMILY_15,                      // CpuFamily
      AMD_F15_OR_ALL                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_ALL,                   //
      0xE3,                               // Address
      0x00000080,                           // regData
      0x00000080,                           // regMask
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F15OrHtPhyRegisterTable = {
  PrimaryCores,
  (sizeof (F15OrHtPhyRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F15OrHtPhyRegisters,
};
