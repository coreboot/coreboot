/* $NoKeywords:$ */
/**
 * @file
 *
 * Gnb f1 table
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 84159 $   @e \$Date: 2011-12-21 14:49:48 -0600 (Wed, 21 Dec 2011) $
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include  "AGESA.h"
#include  "Ids.h"
#include  "amdlib.h"
#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbGfxFamServices.h"
#include  "GnbCommonLib.h"
#include  "GnbF1Table.h"
#include  "GnbF1TableKB.h"
#include  "GnbRegistersKB.h"
#include  "GnbRegisterAccKB.h"
#include  "GnbHandleLib.h"
#include  "OptionGnb.h"
#include  "Filecode.h"
//#define FILECODE PROC_GNB_MODULES_GNBINITKB_GNBFUSETABLEKB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_BUILD_OPTIONS GnbBuildOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */



/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */



PP_F1_ARRAY_V2  DefaultPpF1ArrayKB = {
  0,
  {0x40, 0,  0,  0, 0},
  {0x40, 0,  0,  0, 0},
  8,
  {0,    0,  0,  0,  0,  0},
  {0x40, 0x40, 0x40, 0, 0},
  {0,    0,  0,  0,  0},
  3,
  0x10,
  0,
  0,
  0,
  0,
  TRUE,
  {0x3, 0xC, 0x30, 0xC0},
  0,
  {0, 0, 0, 0},
  {0x40, 0x40, 0x40, 0x40, 0},
  0,
  0,
  0,
  0,
  { 1, 1, 1, 1},
  { 0, 0, 0, 0},
  { 2, 2, 2, 2},
  { 0, 0, 0, 0},
  { 0, 0, 0, 0},
  { 0, 0, 0, 0},
  { 0, 0, 0, 0},
  { 0, 0, 0, 0},
  { 0, 0, 0, 0},
  { 1, 1, 1, 1},
  { 0x24, 0x24, 0x24, 0x24, 0x24 },
  { 0x17, 0x18, 0x20, 0x22, 0x24 },
  { 0x17, 0x18, 0x20, 0x22, 0x24 },
  { 0x1, 0x2, 0x3, 0x4, 0x5 },
  0x1E,
  0x3
};


F1_REGISTER_ENTRY_KB D0F0xBC_xC0104007_TABLE [] = {
  {
    5,
    8,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld32[0])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC0104008_TABLE [] = {
  {
    5,
    8,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld32[1])
  },
  {
    13,
    8,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld32[2])
  },
  {
    21,
    8,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld32[3])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC010400C_TABLE [] = {
  {
    25,
    6,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld21)
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC010407C_TABLE [] = {
  {
    20,
    8,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld32[4])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC0104080_TABLE [] = {
  {
    21,
    8,
    (UINT16) offsetof (PP_F1_ARRAY_V2, VceFlags[0])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC0104083_TABLE [] = {
  {
    5,
    8,
    (UINT16) offsetof (PP_F1_ARRAY_V2, VceFlags[1])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC0104084_TABLE [] = {
  {
    5,
    8,
    (UINT16) offsetof (PP_F1_ARRAY_V2, VceFlags[2])
  },
  {
    13,
    8,
    (UINT16) offsetof (PP_F1_ARRAY_V2, VceFlags[3])
  },
  {
    29,
    3,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld16[0])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC0104088_TABLE [] = {
  {
    0,
    3,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld16[1])
  },
  {
    3,
    3,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld16[2])
  },
  {
    6,
    3,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld16[3])
  },
  {
    12,
    5,
    (UINT16) offsetof (PP_F1_ARRAY_V2, VceMclk)
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC01040A8_TABLE [] = {
  {
    16,
    8,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld35[0])
  },
  {
    24,
    8,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld35[1])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC01040AC_TABLE [] = {
  {
    0,
    8,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld35[2])
  },
  {
    8,
    8,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld35[3])
  },
  {
    16,
    8,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld35[4])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC0107044_TABLE [] = {
  {
    16,
    3,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld36)
  },
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC0107064_TABLE [] = {
  {
    0,
    4,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PPlayTableRev)
  },
  {
    4,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld3)
  },
  {
    11,
    3,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PcieGen2Vid)
  },
  {
    17,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld33[0])
  },
  {
    24,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld33[1])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC0107067_TABLE [] = {
  {
    7,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld33[2])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC0107068_TABLE [] = {
  {
    6,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld33[3])
  },
  {
    13,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld33[4])
  },
  {
    20,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, excel841_fld6[0])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC010706B_TABLE [] = {
  {
    3,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, excel841_fld6[1])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC010706C_TABLE [] = {
  {
    2,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, excel841_fld6[2])
  },
  {
    9,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, excel841_fld6[3])
  },
  {
    16,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, excel841_fld6[4])
  },
  {
    23,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld5[0])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC010706F_TABLE [] = {
  {
    6,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld5[1])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC0107070_TABLE [] = {
  {
    5,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld5[2])
  },
  {
    12,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld5[3])
  },
  {
    19,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld5[4])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC0107073_TABLE [] = {
  {
    2,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld2[0])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC0107074_TABLE [] = {
  {
    1,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld2[1])
  },
  {
    8,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld2[2])
  },
  {
    15,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld2[3])
  },
  {
    22,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld2[4])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC0107077_TABLE [] = {
  {
    5,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld1[0])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC0107078_TABLE [] = {
  {
    4,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld1[1])
  },
  {
    11,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld1[2])
  },
  {
    18,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld1[3])
  },
  {
    25,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld1[4])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC010707C_TABLE [] = {
  {
    0,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, EclkDid[0])
  },
  {
    7,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, EclkDid[1])
  },
  {
    14,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, EclkDid[2])
  },
  {
    21,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, EclkDid[3])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC010707F_TABLE [] = {
  {
    4,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, EclkDid[4])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC0107080_TABLE [] = {
  {
    3,
    5,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld37)
  },
  {
    8,
    5,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld38)
  },
  {
    13,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld34[0])
  },
  {
    20,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld34[1])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC0107083_TABLE [] = {
  {
    3,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld34[2])
  }
};

F1_REGISTER_ENTRY_KB D0F0xBC_xC0107084_TABLE [] = {
  {
    2,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld34[3])
  },
  {
    9,
    7,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld34[4])
  }
};

F1_REGISTER_ENTRY_KB D18F3x64_TABLE [] = {
  {
    D18F3x64_HtcEn_OFFSET,
    D18F3x64_HtcEn_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, HtcEn)
  },
  {
    D18F3x64_HtcTmpLmt_OFFSET,
    D18F3x64_HtcTmpLmt_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, HtcTmpLmt)
  },
  {
    D18F3x64_HtcHystLmt_OFFSET,
    D18F3x64_HtcHystLmt_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld20)
  }
};

F1_REGISTER_ENTRY_KB GnbFuseTableKB565_TABLE [] = {
  {
    1,
    6,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld21)
  }
};

F1_REGISTER_ENTRY_KB D18F2x90_dct0_TABLE [] = {
  {
    D18F2x90_dct0_DisDllShutdownSR_OFFSET,
    D18F2x90_dct0_DisDllShutdownSR_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, DisDllShutdownSR[0])
  }
};

F1_REGISTER_ENTRY_KB D18F2x94_dct0_TABLE [] = {
  {
    D18F2x94_dct0_MemClkFreq_OFFSET,
    D18F2x94_dct0_MemClkFreq_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, MemClkFreq[0])
  }
};

F1_REGISTER_ENTRY_KB D18F2xA8_dct0_TABLE [] = {
  {
    D18F2xA8_dct0_MemPhyPllPdMode_OFFSET,
    D18F2xA8_dct0_MemPhyPllPdMode_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, MemPhyPllPdMode[0])
  }
};

F1_REGISTER_ENTRY_KB D18F2x2E0_dct0_TABLE [] = {
  {
    D18F2x2E0_dct0_M1MemClkFreq_OFFSET,
    D18F2x2E0_dct0_M1MemClkFreq_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, M1MemClkFreq[0])
  }
};

F1_REGISTER_ENTRY_KB D18F5x160_TABLE [] = {
  {
    D18F5x160_NbPstateEn_OFFSET,
    D18F5x160_NbPstateEn_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld26[0])
  },
  {
    D18F5x160_MemPstate_OFFSET,
    D18F5x160_MemPstate_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld29[0])
  },
  {
    D18F5x160_NbFid_OFFSET,
    D18F5x160_NbFid_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld30[0])
  },
  {
    D18F5x160_NbDid_OFFSET,
    D18F5x160_NbDid_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld31[0])
  },
  {
    D18F5x160_NbVid_6_0_OFFSET,
    D18F5x160_NbVid_6_0_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld27[0])
  },
  {
    D18F5x160_NbVid_7_OFFSET,
    D18F5x160_NbVid_7_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld28[0])
  }
};

F1_REGISTER_ENTRY_KB D18F5x164_TABLE [] = {
  {
    D18F5x164_NbPstateEn_OFFSET,
    D18F5x164_NbPstateEn_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld26[1])
  },
  {
    D18F5x164_MemPstate_OFFSET,
    D18F5x164_MemPstate_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld29[1])
  },
  {
    D18F5x164_NbFid_OFFSET,
    D18F5x164_NbFid_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld30[1])
  },
  {
    D18F5x164_NbDid_OFFSET,
    D18F5x164_NbDid_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld31[1])
  },
  {
    D18F5x164_NbVid_6_0_OFFSET,
    D18F5x164_NbVid_6_0_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld27[1])
  },
  {
    D18F5x164_NbVid_7_OFFSET,
    D18F5x164_NbVid_7_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld28[1])
  }
};

F1_REGISTER_ENTRY_KB D18F5x168_TABLE [] = {
  {
    D18F5x168_NbPstateEn_OFFSET,
    D18F5x168_NbPstateEn_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld26[2])
  },
  {
    D18F5x168_MemPstate_OFFSET,
    D18F5x168_MemPstate_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld29[2])
  },
  {
    D18F5x168_NbFid_OFFSET,
    D18F5x168_NbFid_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld30[2])
  },
  {
    D18F5x168_NbDid_OFFSET,
    D18F5x168_NbDid_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld31[2])
  },
  {
    D18F5x168_NbVid_6_0_OFFSET,
    D18F5x168_NbVid_6_0_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld27[2])
  },
  {
    D18F5x168_NbVid_7_OFFSET,
    D18F5x168_NbVid_7_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld28[2])
  }
};

F1_REGISTER_ENTRY_KB D18F5x16C_TABLE [] = {
  {
    D18F5x16C_NbPstateEn_OFFSET,
    D18F5x16C_NbPstateEn_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld26[3])
  },
  {
    D18F5x16C_MemPstate_OFFSET,
    D18F5x16C_MemPstate_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld29[3])
  },
  {
    D18F5x16C_NbFid_OFFSET,
    D18F5x16C_NbFid_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld30[3])
  },
  {
    D18F5x16C_NbDid_OFFSET,
    D18F5x16C_NbDid_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld31[3])
  },
  {
    D18F5x16C_NbVid_6_0_OFFSET,
    D18F5x16C_NbVid_6_0_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld27[3])
  },
  {
    D18F5x16C_NbVid_7_OFFSET,
    D18F5x16C_NbVid_7_WIDTH,
    (UINT16) offsetof (PP_F1_ARRAY_V2, PP_FUSE_ARRAY_V2_fld28[3])
  }
};


F1_TABLE_ENTRY_KB F1RegisterTableKB [] = {

  {
    0x4,
    0xC0104007,
    ARRAY_SIZE(D0F0xBC_xC0104007_TABLE),
    D0F0xBC_xC0104007_TABLE
  },
  {
    0x4,
    0xC0104008,
    ARRAY_SIZE(D0F0xBC_xC0104008_TABLE),
    D0F0xBC_xC0104008_TABLE
  },
  {
    0x4,
    0xC010400c,
    ARRAY_SIZE(D0F0xBC_xC010400C_TABLE),
    D0F0xBC_xC010400C_TABLE
  },
  {
    0x4,
    0xC010407c,
    ARRAY_SIZE(D0F0xBC_xC010407C_TABLE),
    D0F0xBC_xC010407C_TABLE
  },
  {
    0x4,
    0xC0104080,
    ARRAY_SIZE(D0F0xBC_xC0104080_TABLE),
    D0F0xBC_xC0104080_TABLE
  },
  {
    0x4,
    0xC0104083,
    ARRAY_SIZE(D0F0xBC_xC0104083_TABLE),
    D0F0xBC_xC0104083_TABLE
  },
  {
    0x4,
    0xC0104084,
    ARRAY_SIZE(D0F0xBC_xC0104084_TABLE),
    D0F0xBC_xC0104084_TABLE
  },
  {
    0x4,
    0xC0104088,
    ARRAY_SIZE(D0F0xBC_xC0104088_TABLE),
    D0F0xBC_xC0104088_TABLE
  },
  {
    0x4,
    0xC01040a8,
    ARRAY_SIZE(D0F0xBC_xC01040A8_TABLE),
    D0F0xBC_xC01040A8_TABLE
  },
  {
    0x4,
    0xC01040ac,
    ARRAY_SIZE(D0F0xBC_xC01040AC_TABLE),
    D0F0xBC_xC01040AC_TABLE
  },
  {
    0x4,
    0xC0107044,
    ARRAY_SIZE(D0F0xBC_xC0107044_TABLE),
    D0F0xBC_xC0107044_TABLE
  },
  {
    0x4,
    0xC0107064,
    ARRAY_SIZE(D0F0xBC_xC0107064_TABLE),
    D0F0xBC_xC0107064_TABLE
  },
  {
    0x4,
    0xC0107067,
    ARRAY_SIZE(D0F0xBC_xC0107067_TABLE),
    D0F0xBC_xC0107067_TABLE
  },
  {
    0x4,
    0xC0107068,
    ARRAY_SIZE(D0F0xBC_xC0107068_TABLE),
    D0F0xBC_xC0107068_TABLE
  },
  {
    0x4,
    0xC010706b,
    ARRAY_SIZE(D0F0xBC_xC010706B_TABLE),
    D0F0xBC_xC010706B_TABLE
  },
  {
    0x4,
    0xC010706c,
    ARRAY_SIZE(D0F0xBC_xC010706C_TABLE),
    D0F0xBC_xC010706C_TABLE
  },
  {
    0x4,
    0xC010706f,
    ARRAY_SIZE(D0F0xBC_xC010706F_TABLE),
    D0F0xBC_xC010706F_TABLE
  },
  {
    0x4,
    0xC0107070,
    ARRAY_SIZE(D0F0xBC_xC0107070_TABLE),
    D0F0xBC_xC0107070_TABLE
  },
  {
    0x4,
    0xC0107073,
    ARRAY_SIZE(D0F0xBC_xC0107073_TABLE),
    D0F0xBC_xC0107073_TABLE
  },
  {
    0x4,
    0xC0107074,
    ARRAY_SIZE(D0F0xBC_xC0107074_TABLE),
    D0F0xBC_xC0107074_TABLE
  },
  {
    0x4,
    0xC0107077,
    ARRAY_SIZE(D0F0xBC_xC0107077_TABLE),
    D0F0xBC_xC0107077_TABLE
  },
  {
    0x4,
    0xC0107078,
    ARRAY_SIZE(D0F0xBC_xC0107078_TABLE),
    D0F0xBC_xC0107078_TABLE
  },
  {
    0x4,
    0xC010707c,
    ARRAY_SIZE(D0F0xBC_xC010707C_TABLE),
    D0F0xBC_xC010707C_TABLE
  },
  {
    0x4,
    0xC0107080,
    ARRAY_SIZE(D0F0xBC_xC0107080_TABLE),
    D0F0xBC_xC0107080_TABLE
  },
  {
    0x4,
    0xC0107083,
    ARRAY_SIZE(D0F0xBC_xC0107083_TABLE),
    D0F0xBC_xC0107083_TABLE
  },
  {
    0x4,
    0xC0107084,
    ARRAY_SIZE(D0F0xBC_xC0107084_TABLE),
    D0F0xBC_xC0107084_TABLE
  }
};

F1_TABLE_ENTRY_KB  PPRegisterTableKB [] = {
  {
    D18F3x64_TYPE,
    D18F3x64_ADDRESS,
    ARRAY_SIZE(D18F3x64_TABLE),
    D18F3x64_TABLE
  },
  {
    0x4,
    0xC0500000,
    ARRAY_SIZE(GnbFuseTableKB565_TABLE),
    GnbFuseTableKB565_TABLE
  },
  {
    D18F2x90_dct0_TYPE,
    D18F2x90_dct0_ADDRESS,
    ARRAY_SIZE(D18F2x90_dct0_TABLE),
    D18F2x90_dct0_TABLE
  },
  {
    D18F2x94_dct0_TYPE,
    D18F2x94_dct0_ADDRESS,
    ARRAY_SIZE(D18F2x94_dct0_TABLE),
    D18F2x94_dct0_TABLE
  },
  {
    D18F2xA8_dct0_TYPE,
    D18F2xA8_dct0_ADDRESS,
    ARRAY_SIZE(D18F2xA8_dct0_TABLE),
    D18F2xA8_dct0_TABLE
  },
  {
    D18F5x160_TYPE,
    D18F5x160_ADDRESS,
    ARRAY_SIZE(D18F5x160_TABLE),
    D18F5x160_TABLE
  },
  {
    D18F5x164_TYPE,
    D18F5x164_ADDRESS,
    ARRAY_SIZE(D18F5x164_TABLE),
    D18F5x164_TABLE
  },
  {
    D18F5x168_TYPE,
    D18F5x168_ADDRESS,
    ARRAY_SIZE(D18F5x168_TABLE),
    D18F5x168_TABLE
  },
  {
    D18F5x16C_TYPE,
    D18F5x16C_ADDRESS,
    ARRAY_SIZE(D18F5x16C_TABLE),
    D18F5x16C_TABLE
  }
};


F1_TABLE_KB  F1TableKB = {
  ARRAY_SIZE(F1RegisterTableKB),
  F1RegisterTableKB
};

F1_TABLE_KB  PPTableKB = {
  ARRAY_SIZE(PPRegisterTableKB),
  PPRegisterTableKB
};


/*----------------------------------------------------------------------------------------*/
/**
 * Load F1 Table KB
 *
 *
 * param[out] PpF1Array      Pointer to save f1 table
 * param[in]  StdHeader        Pointer to Standard configuration
 * retval     AGESA_STATUS
 */

STATIC VOID
NbF1LoadF1TableKB (
  IN       F1_TABLE_KB       *F1Table,
     OUT   PP_F1_ARRAY_V2    *PpF1Array,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINTN                     RegisterIndex;

  for (RegisterIndex = 0; RegisterIndex < F1Table->F1TableLength; RegisterIndex++ ) {
    UINTN   FieldIndex;
    UINTN   F1RegisterTableLength;
    UINT32  F1Value;
    F1RegisterTableLength = F1Table->F1Table[RegisterIndex].F1RegisterTableLength;

    GnbRegisterReadKB (
      GnbGetHandle (StdHeader),
      F1Table->F1Table[RegisterIndex].RegisterSpaceType,
      F1Table->F1Table[RegisterIndex].Register,
      &F1Value,
      0,
      StdHeader
    );
    for (FieldIndex = 0; FieldIndex < F1RegisterTableLength;  FieldIndex++) {
      F1_REGISTER_ENTRY_KB   RegisterEntry;
      RegisterEntry = F1Table->F1Table[RegisterIndex].F1RegisterTable[FieldIndex];
      *((UINT8 *) PpF1Array + RegisterEntry.F1Offset) = (UINT8) ((F1Value >> RegisterEntry.FieldOffset) &
                                                            ((1 << RegisterEntry.FieldWidth) - 1));
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Gnb load f1 table
 *
 *
 *
 * @param[in] StdHeader        Pointer to Standard configuration
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
GnbLoadF1TableKB (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  PP_F1_ARRAY_V2       *PpF1Array;
  AGESA_STATUS           Status;
  D18F3xA0_STRUCT        D18F3xA0;

  Status = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbLoadF1TableKB Enter\n");

  PpF1Array = (PP_F1_ARRAY_V2 *) GnbAllocateHeapBuffer (AMD_PP_F1_TABLE_HANDLE, sizeof (PP_F1_ARRAY_V2), StdHeader);
  ASSERT (PpF1Array != NULL);
  if (PpF1Array != NULL) {
    //Support for real f1 table
    GnbRegisterReadKB (GnbGetHandle (StdHeader), D18F3xA0_TYPE, D18F3xA0_ADDRESS, &D18F3xA0.Value, 0, StdHeader);

    if ((D18F3xA0.Field.CofVidProg) && (GnbBuildOptions.GnbLoadRealF1Table)) {
      NbF1LoadF1TableKB (&F1TableKB, PpF1Array, StdHeader);
      PpF1Array->PP_FUSE_ARRAY_V2_fld13 = TRUE;
      IDS_HDT_CONSOLE (NB_MISC, "  Processor F1d\n");
    } else {
      LibAmdMemCopy (PpF1Array, &DefaultPpF1ArrayKB, sizeof (PP_F1_ARRAY_V2), StdHeader);
      IDS_HDT_CONSOLE (NB_MISC, "  Processor Unf1d\n");
    }
    NbF1LoadF1TableKB (&PPTableKB, PpF1Array, StdHeader);
    IDS_OPTION_CALLOUT (IDS_CALLOUT_GNB_PPF1_OVERRIDE, PpF1Array, StdHeader);
  } else {
    Status = AGESA_ERROR;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbLoadF1TableKB Exit [0x%x]\n", Status);
  return Status;
}


