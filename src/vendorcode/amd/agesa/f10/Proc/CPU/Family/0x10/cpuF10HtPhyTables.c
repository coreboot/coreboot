/**
 * @file
 *
 * AMD Family_10 DR PCI tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x10
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
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
#define FILECODE PROC_CPU_FAMILY_0X10_CPUF10HTPHYTABLES_FILECODE

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

//  HT   P C I   T a b l e s
// -------------------------
STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F10HtPhyRegisters[] =
{
// 0xCF
// HT_PHY_HT1_FIFO_PTR_OPT_VALUE
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL0_HT1,               //
      0xCF,                                 // Address
      0x0000006D,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// HT_PHY_HT1_FIFO_PTR_OPT_VALUE
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL1_HT1,               //
      0xDF,                                 // Address
      0x0000006D,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// Default for HT3, unless overridden below.
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xCF,                                 // Address
      0x0000005A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// Default for HT3, unless overridden below.
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xDF,                                 // Address
      0x0000005A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xD1
// [29:22] LfcMax = 20h, [21:14] LfcMin = 10h
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
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
// [29:22] LfcMax = 20h, [21:14] LfcMin = 10h
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
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
// [29:22] LfcMax = 10h, [21:14] LfcMin = 08h
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL1_HT1,               //
      0xD1,                                 // Address
      0x04020000,                           // regData
      0x3FFFC000,                           // regMask
    }}
  },
// 0xC1
// [29:22] LfcMax = 10h, [21:14] LfcMin = 08h
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL0_HT1,               //
      0xC1,                                 // Address
      0x04020000,                           // regData
      0x3FFFC000,                           // regMask
    }}
  },
//
// Deemphasis Settings
//

// HT1: clear any warm reset deemphasis settings.
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL0_HT1,               //
      0xC5,                                 // Address
      0x00000000,                           // regData
      0xE01F1FDF,                           // regMask
    }}
  },
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL1_HT1,               //
      0xD5,                                 // Address
      0x00000000,                           // regData
      0xE01F1FDF,                           // regMask
    }}
  },
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL0_HT1,               //
      0xC4,                                 // Address
      0x00000000,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL1_HT1,               //
      0xD4,                                 // Address
      0x00000000,                           // regData
      0x0000FC00,                           // regMask
    }}
  },

//deemphasis level        DL1[20:16], DL2[12:8], DP1[4:0] PostCur1En[31] PostCur2En[30] PreCur1En[29] MapPostCur2En[6]
// No deemphasis            00h        00h          00h     0                0             0             0
// -3dB postcursor          12h        00h          00h     1                0             0             0
// -6dB postcursor          1Fh        00h          00h     1                0             0             0
// -8dB postcursor          1Fh        06h          00h     1                1             0             1
// -11dB postcursor         1Fh        0Dh          00h     1                1             0             1
// -11dB postcursor with
// -8dB precursor           1Fh        06h          07h     1                1             1             1

  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL_NONE,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC5,                                 // Address
      0x00000000,                           // regData
      0xE01F1F5F,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL_NONE,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD5,                                 // Address
      0x00000000,                           // regData
      0xE01F1F5F,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__3,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC5,                                 // Address
      0x80120000,                           // regData
      0xE01F1F5F,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__3,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD5,                                 // Address
      0x80120000,                           // regData
      0xE01F1F5F,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__6,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC5,                                 // Address
      0x801F0000,                           // regData
      0xE01F1F5F,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__6,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD5,                                 // Address
      0x801F0000,                           // regData
      0xE01F1F5F,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__8,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC5,                                 // Address
      0xC01F0640,                           // regData
      0xE01F1F5F,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__8,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD5,                                 // Address
      0xC01F0640,                           // regData
      0xE01F1F5F,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__11,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC5,                                 // Address
      0xC01F0D40,                           // regData
      0xE01F1F5F,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__11,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD5,                                 // Address
      0xC01F0D40,                           // regData
      0xE01F1F5F,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__11_8,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC5,                                 // Address
      0xE01F0647,                           // regData
      0xE01F1F5F,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__11_8,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD5,                                 // Address
      0xE01F0647,                           // regData
      0xE01F1F5F,                           // regMask
    }}
  },

// Far-device deemphasis setting DCV[15:10]
// No deemphasis                 20h
// -2dB postcursor               19h
// -3dB postcursor               17h
// -5dB postcursor               11h
// -6dB postcursor               10h
// -7dB postcursor               0Eh
// -8dB postcursor               0Dh
// -9dB postcursor               0Bh
// -11dB postcursor              09h

  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL_NONE,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x00008000,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL_NONE,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x00008000,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__2,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x00006400,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__2,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x00006400,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__3,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x00005C00,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__3,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x00005C00,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__5,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x00004400,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__5,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x00004400,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__6,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x00004000,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__6,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x00004000,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__7,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x00003800,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__7,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x00003800,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__8,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x00003400,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__8,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x00003400,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__9,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x00002C00,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__9,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x00002C00,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__11,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC4,                                 // Address
      0x00002400,                           // regData
      0x0000FC00,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DCV_LEVEL__11,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD4,                                 // Address
      0x00002400,                           // regData
      0x0000FC00,                           // regMask
    }}
  },

};

CONST REGISTER_TABLE ROMDATA F10HtPhyRegisterTable = {
  PrimaryCores,
  (sizeof (F10HtPhyRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F10HtPhyRegisters,
};

