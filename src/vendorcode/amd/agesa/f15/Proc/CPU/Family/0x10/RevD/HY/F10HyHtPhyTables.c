/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 Hydra Ht Phy tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x10
 * @e \$Revision: 56279 $   @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
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
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X10_REVD_HY_F10HYHTPHYTABLES_FILECODE

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
STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F10HyHtPhyRegisters[] =
{
// 0x60:0x68
  {
    HtPhyRangeRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_GT_C0                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL0_ALL,               //
      0x60, 0x68,                           // Address
      0x00000040,                           // regData
      0x00000040,                           // regMask
    }}
  },
// 0x70:0x78
  {
    HtPhyRangeRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_GT_C0                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL1_ALL,               //
      0x70, 0x78,                           // Address
      0x00000040,                           // regData
      0x00000040,                           // regMask
    }}
  },
// 0xC0
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL0_ALL,               //
      0xC0,                                 // Address
      0x40040000,                           // regData
      0xe01F0000,                           // regMask
    }}
  },
// 0xD0
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                      // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL1_ALL,               //
      0xD0,                                 // Address
      0x40040000,                           // regData
      0xe01F0000,                           // regMask
    }}
  },
// 0xCF
// Default for HT3, unless overridden below.
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xCF,                                 // Address
      0x0000002A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// Default for HT3, unless overridden below.
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xDF,                                 // Address
      0x0000002A,                           // regData
      0x000000FF,                           // regMask
    }}
  },

//
// All the entries for XmtRdPtr 6
//

// 0xCF
// For HT frequencies 1200-1600 and NB Freq 1600, 1800
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1200M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK0 | HTPHY_LINKTYPE_SL0_LINK2 | HTPHY_LINKTYPE_SL0_LINK3),
      0xCF,                                 // Address
      0x0000006A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 1200-1600 and NB Freq 1600, 1800
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1200M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK4 | HTPHY_LINKTYPE_SL1_LINK6 | HTPHY_LINKTYPE_SL1_LINK7),
      0xDF,                                 // Address
      0x0000006A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 1800 and NB Freq 1800
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK0 | HTPHY_LINKTYPE_SL0_LINK2 | HTPHY_LINKTYPE_SL0_LINK3),
      0xCF,                                 // Address
      0x0000006A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 1800 and NB Freq 1800
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK4 | HTPHY_LINKTYPE_SL1_LINK6 | HTPHY_LINKTYPE_SL1_LINK7),
      0xDF,                                 // Address
      0x0000006A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 3200 and NB Freq 1600
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_3200M, HT_FREQUENCY_3200M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK0 | HTPHY_LINKTYPE_SL0_LINK2 | HTPHY_LINKTYPE_SL0_LINK3),
      0xCF,                                 // Address
      0x0000006A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 3200 and NB Freq 1600
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_3200M, HT_FREQUENCY_3200M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK4 | HTPHY_LINKTYPE_SL1_LINK6 | HTPHY_LINKTYPE_SL1_LINK7),
      0xDF,                                 // Address
      0x0000006A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 1200 and 1600 and NB Freq 1600 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1200M, HT_FREQUENCY_1200M) | FREQ_RANGE_1 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK1),
      0xCF,                                 // Address
      0x0000006A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 1200 and 1600 and NB Freq 1600 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1200M, HT_FREQUENCY_1200M) | FREQ_RANGE_1 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK5),
      0xDF,                                 // Address
      0x0000006A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 1200 and 1800 and NB Freq 1800 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1200M, HT_FREQUENCY_1200M) | FREQ_RANGE_1 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK1),
      0xCF,                                 // Address
      0x0000006A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 1200 and 1800 and NB Freq 1800 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1200M, HT_FREQUENCY_1200M) | FREQ_RANGE_1 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK5),
      0xDF,                                 // Address
      0x0000006A,                           // regData
      0x000000FF,                           // regMask
    }}
  },

//
// Entries for XmtRdPtr 5
//

// 0xCF
// For HT frequencies 1800-2600 and NB Freq 1600
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_2600M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK0 | HTPHY_LINKTYPE_SL0_LINK2 | HTPHY_LINKTYPE_SL0_LINK3),
      0xCF,                                 // Address
      0x0000005A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 1800-2600 and NB Freq 1600
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_2600M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK4 | HTPHY_LINKTYPE_SL1_LINK6 | HTPHY_LINKTYPE_SL1_LINK7),
      0xDF,                                 // Address
      0x0000005A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 2000 - 2800 and NB Freq 1800
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2000M, HT_FREQUENCY_2800M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK0 | HTPHY_LINKTYPE_SL0_LINK2 | HTPHY_LINKTYPE_SL0_LINK3),
      0xCF,                                 // Address
      0x0000005A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 2000 - 2800 and NB Freq 1800
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2000M, HT_FREQUENCY_2800M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK4 | HTPHY_LINKTYPE_SL1_LINK6 | HTPHY_LINKTYPE_SL1_LINK7),
      0xDF,                                 // Address
      0x0000005A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 1400 and 1800 and NB Freq 1600 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1400M, HT_FREQUENCY_1400M) | FREQ_RANGE_1 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK1),
      0xCF,                                 // Address
      0x0000005A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 1400 and 1800 and NB Freq 1600 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1400M, HT_FREQUENCY_1400M) | FREQ_RANGE_1 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK5),
      0xDF,                                 // Address
      0x0000005A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 1400 and 1600 and NB Freq 1800 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1400M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK1),
      0xCF,                                 // Address
      0x0000005A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 1400 and 1600 and NB Freq 1800 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1400M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK5),
      0xDF,                                 // Address
      0x0000005A,                           // regData
      0x000000FF,                           // regMask
    }}
  },

//
// Entries for XmtRdPtr 4
//

// 0xCF
// For HT frequencies 2800-3000 and NB Freq 1600
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2800M, HT_FREQUENCY_3000M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK0 | HTPHY_LINKTYPE_SL0_LINK2 | HTPHY_LINKTYPE_SL0_LINK3),
      0xCF,                                 // Address
      0x0000004A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 2800-3000 and NB Freq 1600
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2800M, HT_FREQUENCY_3000M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK4 | HTPHY_LINKTYPE_SL1_LINK6 | HTPHY_LINKTYPE_SL1_LINK7),
      0xDF,                                 // Address
      0x0000004A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 3000 - 3200 and NB Freq 1800
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_3000M, HT_FREQUENCY_3200M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK0 | HTPHY_LINKTYPE_SL0_LINK2 | HTPHY_LINKTYPE_SL0_LINK3),
      0xCF,                                 // Address
      0x0000004A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 3000 - 3200 and NB Freq 1800
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_3000M, HT_FREQUENCY_3200M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK4 | HTPHY_LINKTYPE_SL1_LINK6 | HTPHY_LINKTYPE_SL1_LINK7),
      0xDF,                                 // Address
      0x0000004A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 2000 - 2400 and 3200 and NB Freq 1600 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2000M, HT_FREQUENCY_2400M) | FREQ_RANGE_1 (HT_FREQUENCY_3200M, HT_FREQUENCY_3200M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK1),
      0xCF,                                 // Address
      0x0000004A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 2000 - 2400 and 3200 and NB Freq 1600 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2000M, HT_FREQUENCY_2400M) | FREQ_RANGE_1 (HT_FREQUENCY_3200M, HT_FREQUENCY_3200M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK5),
      0xDF,                                 // Address
      0x0000004A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 2000 - 2400 and NB Freq 1800 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2000M, HT_FREQUENCY_2400M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK1),
      0xCF,                                 // Address
      0x0000004A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 2000 - 2400 and NB Freq 1800 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2000M, HT_FREQUENCY_2400M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK5),
      0xDF,                                 // Address
      0x0000004A,                           // regData
      0x000000FF,                           // regMask
    }}
  },

//
// Entries for XmtRdPtr 3
//

// 0xCF
// For HT frequencies 2600-3000 and NB Freq 1600 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2600M, HT_FREQUENCY_3000M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK1),
      0xCF,                                 // Address
      0x0000003A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 2600-3000 and NB Freq 1600 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2600M, HT_FREQUENCY_3000M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK5),
      0xDF,                                 // Address
      0x0000003A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 2600 - 3200 and NB Freq 1800 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2600M, HT_FREQUENCY_3200M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK1),
      0xCF,                                 // Address
      0x0000003A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 2600 - 3200 and NB Freq 1800 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2600M, HT_FREQUENCY_3200M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK5),
      0xDF,                                 // Address
      0x0000003A,                           // regData
      0x000000FF,                           // regMask
    }}
  },

//
// Rev D0 fixups for Erratum 398.
//

// 0xCF
// For HT frequencies 1800, 2200 and NB Freq 1400
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | FREQ_RANGE_1 (HT_FREQUENCY_2200M, HT_FREQUENCY_2200M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1400M, HT_FREQUENCY_1400M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK0 | HTPHY_LINKTYPE_SL0_LINK1 | HTPHY_LINKTYPE_SL0_LINK2 | HTPHY_LINKTYPE_SL0_LINK3),
      0xCF,                                 // Address
      0x0000000A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 1800, 2200 and NB Freq 1400
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | FREQ_RANGE_1 (HT_FREQUENCY_2200M, HT_FREQUENCY_2200M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1400M, HT_FREQUENCY_1400M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK4 | HTPHY_LINKTYPE_SL1_LINK5 | HTPHY_LINKTYPE_SL1_LINK6 | HTPHY_LINKTYPE_SL1_LINK7),
      0xDF,                                 // Address
      0x0000000A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 2600, 3000 and NB Freq 1400
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2600M, HT_FREQUENCY_2600M) | FREQ_RANGE_1 (HT_FREQUENCY_3000M, HT_FREQUENCY_3000M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1400M, HT_FREQUENCY_1400M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK0 | HTPHY_LINKTYPE_SL0_LINK1 | HTPHY_LINKTYPE_SL0_LINK2 | HTPHY_LINKTYPE_SL0_LINK3),
      0xCF,                                 // Address
      0x0000000A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 2600, 3000 and NB Freq 1400
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2600M, HT_FREQUENCY_2600M) | FREQ_RANGE_1 (HT_FREQUENCY_3000M, HT_FREQUENCY_3000M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1400M, HT_FREQUENCY_1400M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK4 | HTPHY_LINKTYPE_SL1_LINK5 | HTPHY_LINKTYPE_SL1_LINK6 | HTPHY_LINKTYPE_SL1_LINK7),
      0xDF,                                 // Address
      0x0000000A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 2200, 2600 and NB Freq 1600
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2200M, HT_FREQUENCY_2200M) | FREQ_RANGE_1 (HT_FREQUENCY_2600M, HT_FREQUENCY_2600M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK0 | HTPHY_LINKTYPE_SL0_LINK2 | HTPHY_LINKTYPE_SL0_LINK3),
      0xCF,                                 // Address
      0x0000003A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 2200, 2600 and NB Freq 1600
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2200M, HT_FREQUENCY_2200M) | FREQ_RANGE_1 (HT_FREQUENCY_2600M, HT_FREQUENCY_2600M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK4 | HTPHY_LINKTYPE_SL1_LINK6 | HTPHY_LINKTYPE_SL1_LINK7),
      0xDF,                                 // Address
      0x0000003A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 3000 and NB Freq 1600
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_3000M, HT_FREQUENCY_3000M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK0 | HTPHY_LINKTYPE_SL0_LINK2 | HTPHY_LINKTYPE_SL0_LINK3),
      0xCF,                                 // Address
      0x0000002A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 3000 and NB Freq 1600
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_3000M, HT_FREQUENCY_3000M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK4 | HTPHY_LINKTYPE_SL1_LINK6 | HTPHY_LINKTYPE_SL1_LINK7),
      0xDF,                                 // Address
      0x0000002A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 2200, 2600 and NB Freq 1800
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2200M, HT_FREQUENCY_2200M) | FREQ_RANGE_1 (HT_FREQUENCY_2600M, HT_FREQUENCY_2600M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK0 | HTPHY_LINKTYPE_SL0_LINK2 | HTPHY_LINKTYPE_SL0_LINK3),
      0xCF,                                 // Address
      0x0000003A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 2200, 2600 and NB Freq 1800
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2200M, HT_FREQUENCY_2200M) | FREQ_RANGE_1 (HT_FREQUENCY_2600M, HT_FREQUENCY_2600M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK4 | HTPHY_LINKTYPE_SL1_LINK6 | HTPHY_LINKTYPE_SL1_LINK7),
      0xDF,                                 // Address
      0x0000003A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 3000 and NB Freq 1800
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_3000M, HT_FREQUENCY_3000M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK0 | HTPHY_LINKTYPE_SL0_LINK2 | HTPHY_LINKTYPE_SL0_LINK3),
      0xCF,                                 // Address
      0x0000002A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 3000 and NB Freq 1800
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_3000M, HT_FREQUENCY_3000M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK4 | HTPHY_LINKTYPE_SL1_LINK6 | HTPHY_LINKTYPE_SL1_LINK7),
      0xDF,                                 // Address
      0x0000002A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 1800 and NB Freq 1600 for all links
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK0 | HTPHY_LINKTYPE_SL0_LINK1 | HTPHY_LINKTYPE_SL0_LINK2 | HTPHY_LINKTYPE_SL0_LINK3),
      0xCF,                                 // Address
      0x0000003A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 1800 and NB Freq 1600 for all links
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_1800M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1600M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK4 | HTPHY_LINKTYPE_SL1_LINK5 | HTPHY_LINKTYPE_SL1_LINK6 | HTPHY_LINKTYPE_SL1_LINK7),
      0xDF,                                 // Address
      0x0000003A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 2200 and NB Freq 1600, 1800 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2200M, HT_FREQUENCY_2200M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK1),
      0xCF,                                 // Address
      0x0000002A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 2200 and NB Freq 1600, 1800 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2200M, HT_FREQUENCY_2200M) | COUNT_RANGE_NONE),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK5),
      0xDF,                                 // Address
      0x0000002A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xCF
// For HT frequencies 2600, 3000 and NB Freq 1600, 1800 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2600M, HT_FREQUENCY_2600M) | FREQ_RANGE_1 (HT_FREQUENCY_3000M, HT_FREQUENCY_3000M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL0_LINK1),
      0xCF,                                 // Address
      0x0000001A,                           // regData
      0x000000FF,                           // regMask
    }}
  },
// 0xDF
// For HT frequencies 2600, 3000 and NB Freq 1600, 1800 only for link 1
  {
    HtPhyFreqRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_D0                          // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      (FREQ_RANGE_0 (HT_FREQUENCY_2600M, HT_FREQUENCY_2600M) | FREQ_RANGE_1 (HT_FREQUENCY_3000M, HT_FREQUENCY_3000M)),
      (FREQ_RANGE_0 (HT_FREQUENCY_1600M, HT_FREQUENCY_1800M) | COUNT_RANGE_NONE),
      (HTPHY_LINKTYPE_SL1_LINK5),
      0xDF,                                 // Address
      0x0000001A,                           // regData
      0x000000FF,                           // regMask
    }}
  },

//
// Deemphasis Settings for D1 processors.
//

// For D1, also set [7]TxLs23ClkGateEn.
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
      AMD_FAMILY_10_HY,                      // CpuFamily
      AMD_F10_D1                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL_NONE,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC5,                                 // Address
      0x00000080,                           // regData
      0xE01F1FDF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10_HY,                      // CpuFamily
      AMD_F10_D1                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL_NONE,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD5,                                 // Address
      0x00000080,                           // regData
      0xE01F1FDF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10_HY,                      // CpuFamily
      AMD_F10_D1                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__3,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC5,                                 // Address
      0x80120080,                           // regData
      0xE01F1FDF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10_HY,                      // CpuFamily
      AMD_F10_D1                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__3,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD5,                                 // Address
      0x80120080,                           // regData
      0xE01F1FDF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10_HY,                      // CpuFamily
      AMD_F10_D1                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__6,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC5,                                 // Address
      0x801F0080,                           // regData
      0xE01F1FDF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10_HY,                      // CpuFamily
      AMD_F10_D1                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__6,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD5,                                 // Address
      0x801F0080,                           // regData
      0xE01F1FDF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10_HY,                      // CpuFamily
      AMD_F10_D1                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__8,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC5,                                 // Address
      0xC01F06C0,                           // regData
      0xE01F1FDF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10_HY,                      // CpuFamily
      AMD_F10_D1                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__8,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD5,                                 // Address
      0xC01F06C0,                           // regData
      0xE01F1FDF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10_HY,                      // CpuFamily
      AMD_F10_D1                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__11,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC5,                                 // Address
      0xC01F0DC0,                           // regData
      0xE01F1FDF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10_HY,                      // CpuFamily
      AMD_F10_D1                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__11,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD5,                                 // Address
      0xC01F0DC0,                           // regData
      0xE01F1FDF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10_HY,                      // CpuFamily
      AMD_F10_D1                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__11_8,
      HTPHY_LINKTYPE_SL0_HT3,               //
      0xC5,                                 // Address
      0xE01F06C7,                           // regData
      0xE01F1FDF,                           // regMask
    }}
  },
  {
    DeemphasisRegister,
    {
      AMD_FAMILY_10_HY,                      // CpuFamily
      AMD_F10_D1                         // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      DEEMPHASIS_LEVEL__11_8,
      HTPHY_LINKTYPE_SL1_HT3,               //
      0xD5,                                 // Address
      0xE01F06C7,                           // regData
      0xE01F1FDF,                           // regMask
    }}
  },

// 0x520A
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                       // CpuRevision
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
  {
    HtPhyRegister,
    {
      AMD_FAMILY_10,                      // CpuFamily
      AMD_F10_HY_ALL                       // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      HTPHY_LINKTYPE_SL1_ALL,                   //
      0x530A,                               // Address
      0x00004000,                           // regData
      0x00006000,                           // regMask
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F10HyHtPhyRegisterTable = {
  PrimaryCores,
  (sizeof (F10HyHtPhyRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F10HyHtPhyRegisters,
};
