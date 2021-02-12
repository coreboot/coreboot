/* $NoKeywords:$ */
/**
 * @file
 *
 * Gnb fuse table
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#include  "GnbFuseTable.h"
#include  "GnbFuseTableTN.h"
#include  "GnbRegistersTN.h"
#include  "GnbRegisterAccTN.h"
#include  "OptionGnb.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_GNBFUSETABLETN_FILECODE
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

VOID
GnbFuseTableDebugDumpTN (
  IN       PP_FUSE_ARRAY       *PpFuseArray,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );



PP_FUSE_ARRAY  ex907  = {
  0,                              // PP table revision
  {1,    0,  0,  0,  0, 0},       // Valid DPM states
  {0x40, 0,  0,  0,  0, 0},       // Sclk DPM DID
  {0,    0,  0,  0,  0, 0},       // Sclk DPM VID
  {0,    0,  0,  0,  0},          // Sclk DPM Cac
  {1,    0,  0,  0,  0,  0},      // State policy flags
  {2,    0,  0,  0,  0,  0},      // State policy label
  {0x40, 0,  0,  0},              // VCLK DID
  {0x40, 0,  0,  0},              // DCLK DID
  8,                              // Thermal SCLK
  {0,    0,  0,  0,  0,  0},      // Vclk/Dclk selector
  {0,    0,  0,  0},              // Valid Lclk DPM states
  {0x40, 0x40, 0x40, 0},          // Lclk DPM DID
  {0x40, 0x40, 0x40, 0},          // Lclk DPM VID
  {0,    0,  0,  0},              // Displclk DID
  3,                              // Pcie Gen 2 VID
  0x10,                           // Main PLL id for 3200 VCO
  0,                              // WRCK SMU clock Divisor
  {0x24, 0x24, 0x24, 0x24},       // SCLK VID
  0,                              // GPU boost cap
  {0, 0, 0, 0, 0, 0},             // Sclk DPM TDP limit
  0,                              // TDP limit PG
  0,                              // Boost margin
  0,                              // Throttle margin
  TRUE,                           // Support VCE in PP table
  {0x3, 0xC, 0x30, 0xC0},         // VCE Flags
  {0, 1, 0, 1},                   // MCLK for VCE
  {0, 0, 0, 0},                   // SCLK selector for VCE
  {0x40, 0x40, 0x40, 0x40}        // Eclk DID
};


FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0104158_TABLE [] = {
  {
    D0F0xBC_xE0104158_EClkDid0_OFFSET,
    D0F0xBC_xE0104158_EClkDid0_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, EclkDid[0])
  },
  {
    D0F0xBC_xE0104158_EClkDid1_OFFSET,
    D0F0xBC_xE0104158_EClkDid1_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, EclkDid[1])
  },
  {
    D0F0xBC_xE0104158_EClkDid2_OFFSET,
    D0F0xBC_xE0104158_EClkDid2_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, EclkDid[2])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE010415B_TABLE [] = {
  {
    D0F0xBC_xE010415B_EClkDid3_OFFSET,
    D0F0xBC_xE010415B_EClkDid3_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, EclkDid[3])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0104184_TABLE [] = {
  {
    D0F0xBC_xE0104184_VCEFlag0_OFFSET,
    D0F0xBC_xE0104184_VCEFlag0_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VceFlags[0])
  },
  {
    D0F0xBC_xE0104184_VCEFlag1_OFFSET,
    D0F0xBC_xE0104184_VCEFlag1_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VceFlags[1])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0104187_TABLE [] = {
  {
    D0F0xBC_xE0104187_VCEFlag2_OFFSET,
    D0F0xBC_xE0104187_VCEFlag2_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VceFlags[2])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0104188_TABLE [] = {
  {
    D0F0xBC_xE0104188_VCEFlag3_OFFSET,
    D0F0xBC_xE0104188_VCEFlag3_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VceFlags[3])
  },
  {
    D0F0xBC_xE0104188_ReqSclkSel0_OFFSET,
    D0F0xBC_xE0104188_ReqSclkSel0_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VceReqSclkSel[0])
  },
  {
    D0F0xBC_xE0104188_ReqSclkSel1_OFFSET,
    D0F0xBC_xE0104188_ReqSclkSel1_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VceReqSclkSel[1])
  },
  {
    D0F0xBC_xE0104188_ReqSclkSel2_OFFSET,
    D0F0xBC_xE0104188_ReqSclkSel2_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VceReqSclkSel[2])
  },
  {
    D0F0xBC_xE0104188_ReqSclkSel3_OFFSET,
    D0F0xBC_xE0104188_ReqSclkSel3_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VceReqSclkSel[3])
  },
  {
    D0F0xBC_xE0104188_VCEMclk_OFFSET + 0,
    1,
    (UINT16) offsetof (PP_FUSE_ARRAY, VceMclk[0])
  },
  {
    D0F0xBC_xE0104188_VCEMclk_OFFSET + 1,
    1,
    (UINT16) offsetof (PP_FUSE_ARRAY, VceMclk[1])
  },
  {
    D0F0xBC_xE0104188_VCEMclk_OFFSET + 2,
    1,
    (UINT16) offsetof (PP_FUSE_ARRAY, VceMclk[2])
  },
  {
    D0F0xBC_xE0104188_VCEMclk_OFFSET + 3,
    1,
    (UINT16) offsetof (PP_FUSE_ARRAY, VceMclk[3])
  },
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0106020_TABLE [] = {
  {
    D0F0xBC_xE0106020_PowerplayDClkVClkSel0_OFFSET,
    D0F0xBC_xE0106020_PowerplayDClkVClkSel0_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VclkDclkSel[0])
  },
  {
    D0F0xBC_xE0106020_PowerplayDClkVClkSel1_OFFSET,
    D0F0xBC_xE0106020_PowerplayDClkVClkSel1_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VclkDclkSel[1])
  },
  {
    D0F0xBC_xE0106020_PowerplayDClkVClkSel2_OFFSET,
    D0F0xBC_xE0106020_PowerplayDClkVClkSel2_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VclkDclkSel[2])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0106023_TABLE [] = {
  {
    D0F0xBC_xE0106023_PowerplayDClkVClkSel3_OFFSET,
    D0F0xBC_xE0106023_PowerplayDClkVClkSel3_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VclkDclkSel[3])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0106024_TABLE [] = {
  {
    D0F0xBC_xE0106024_PowerplayDClkVClkSel4_OFFSET,
    D0F0xBC_xE0106024_PowerplayDClkVClkSel4_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VclkDclkSel[4])
  },
  {
    D0F0xBC_xE0106024_PowerplayDClkVClkSel5_OFFSET,
    D0F0xBC_xE0106024_PowerplayDClkVClkSel5_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VclkDclkSel[5])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE010705C_TABLE [] = {
  {
    D0F0xBC_xE010705C_PowerplayTableRev_OFFSET,
    D0F0xBC_xE010705C_PowerplayTableRev_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, PPlayTableRev)
  },
  {
    D0F0xBC_xE010705C_SClkThermDid_OFFSET,
    D0F0xBC_xE010705C_SClkThermDid_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkThermDid)
  },
  {
    D0F0xBC_xE010705C_PcieGen2Vid_OFFSET,
    D0F0xBC_xE010705C_PcieGen2Vid_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, PcieGen2Vid)
  }
};
FUSE_REGISTER_ENTRY_TN D0F0xBC_xE010705F_TABLE [] = {
  {
    D0F0xBC_xE010705F_SClkDpmVid0_OFFSET,
    D0F0xBC_xE010705F_SClkDpmVid0_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkDpmVid[0])
  },
  {
    D0F0xBC_xE010705F_SClkDpmVid0_OFFSET,
    D0F0xBC_xE010705F_SClkDpmVid0_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, LclkDpmVid[0])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0107060_TABLE [] = {
  {
    D0F0xBC_xE0107060_SClkDpmVid1_OFFSET,
    D0F0xBC_xE0107060_SClkDpmVid1_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkDpmVid[1])
  },
  {
    D0F0xBC_xE0107060_SClkDpmVid1_OFFSET,
    D0F0xBC_xE0107060_SClkDpmVid1_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, LclkDpmVid[1])
  },
  {
    D0F0xBC_xE0107060_SClkDpmVid2_OFFSET,
    D0F0xBC_xE0107060_SClkDpmVid2_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkDpmVid[2])
  },
  {
    D0F0xBC_xE0107060_SClkDpmVid2_OFFSET,
    D0F0xBC_xE0107060_SClkDpmVid2_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, LclkDpmVid[2])
  },
  {
    D0F0xBC_xE0107060_SClkDpmVid3_OFFSET,
    D0F0xBC_xE0107060_SClkDpmVid3_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkDpmVid[3])
  },
  {
    D0F0xBC_xE0107060_SClkDpmVid4_OFFSET,
    D0F0xBC_xE0107060_SClkDpmVid4_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkDpmVid[4])
  },
  {
    D0F0xBC_xE0107060_SClkDpmDid0_OFFSET,
    D0F0xBC_xE0107060_SClkDpmDid0_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkDpmDid[0])
  },
  {
    D0F0xBC_xE0107060_SClkDpmDid1_OFFSET,
    D0F0xBC_xE0107060_SClkDpmDid1_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkDpmDid[1])
  },
  {
    D0F0xBC_xE0107060_SClkDpmDid2_OFFSET,
    D0F0xBC_xE0107060_SClkDpmDid2_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkDpmDid[2])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0107063_TABLE [] = {
  {
    D0F0xBC_xE0107063_SClkDpmDid3_OFFSET,
    D0F0xBC_xE0107063_SClkDpmDid3_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkDpmDid[3])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0107064_TABLE [] = {
  {
    D0F0xBC_xE0107064_SClkDpmDid4_OFFSET,
    D0F0xBC_xE0107064_SClkDpmDid4_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkDpmDid[4])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0107067_TABLE [] = {
  {
    D0F0xBC_xE0107067_DispClkDid0_OFFSET,
    D0F0xBC_xE0107067_DispClkDid0_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, DisplclkDid[0])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0107068_TABLE [] = {
  {
    D0F0xBC_xE0107068_DispClkDid1_OFFSET,
    D0F0xBC_xE0107068_DispClkDid1_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, DisplclkDid[1])
  },
  {
    D0F0xBC_xE0107068_DispClkDid2_OFFSET,
    D0F0xBC_xE0107068_DispClkDid2_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, DisplclkDid[2])
  },
  {
    D0F0xBC_xE0107068_DispClkDid3_OFFSET,
    D0F0xBC_xE0107068_DispClkDid3_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, DisplclkDid[3])
  },
  {
    D0F0xBC_xE0107068_LClkDpmDid0_OFFSET,
    D0F0xBC_xE0107068_LClkDpmDid0_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, LclkDpmDid[0])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE010706B_TABLE [] = {
  {
    D0F0xBC_xE010706B_LClkDpmDid1_OFFSET,
    D0F0xBC_xE010706B_LClkDpmDid1_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, LclkDpmDid[1])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE010706C_TABLE [] = {
  {
    D0F0xBC_xE010706C_LClkDpmDid2_OFFSET,
    D0F0xBC_xE010706C_LClkDpmDid2_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, LclkDpmDid[2])
  },
  {
    D0F0xBC_xE010706C_LClkDpmDid3_OFFSET,
    D0F0xBC_xE010706C_LClkDpmDid3_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, LclkDpmDid[3])
  },
  {
    D0F0xBC_xE010706C_LClkDpmValid_OFFSET + 0,
    1,
    (UINT16) offsetof (PP_FUSE_ARRAY, LclkDpmValid[0])
  },
  {
    D0F0xBC_xE010706C_LClkDpmValid_OFFSET + 1,
    1,
    (UINT16) offsetof (PP_FUSE_ARRAY, LclkDpmValid[1])
  },
  {
    D0F0xBC_xE010706C_LClkDpmValid_OFFSET + 2,
    1,
    (UINT16) offsetof (PP_FUSE_ARRAY, LclkDpmValid[2])
  },
  {
    D0F0xBC_xE010706C_LClkDpmValid_OFFSET + 3,
    1,
    (UINT16) offsetof (PP_FUSE_ARRAY, LclkDpmValid[3])
  },
  {
    D0F0xBC_xE010706C_DClkDid0_OFFSET,
    D0F0xBC_xE010706C_DClkDid0_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, DclkDid[0])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE010706F_TABLE [] = {
  {
    D0F0xBC_xE010706F_DClkDid1_OFFSET,
    D0F0xBC_xE010706F_DClkDid1_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, DclkDid[1])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0107070_TABLE [] = {
  {
    D0F0xBC_xE0107070_DClkDid2_OFFSET,
    D0F0xBC_xE0107070_DClkDid2_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, DclkDid[2])
  },
  {
    D0F0xBC_xE0107070_DClkDid3_OFFSET,
    D0F0xBC_xE0107070_DClkDid3_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, DclkDid[3])
  },
  {
    D0F0xBC_xE0107070_VClkDid0_OFFSET,
    D0F0xBC_xE0107070_VClkDid0_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VclkDid[0])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0107073_TABLE [] = {
  {
    D0F0xBC_xE0107073_VClkDid1_OFFSET,
    D0F0xBC_xE0107073_VClkDid1_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VclkDid[1])
  },
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0107074_TABLE [] = {
  {
    D0F0xBC_xE0107074_VClkDid2_OFFSET,
    D0F0xBC_xE0107074_VClkDid2_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VclkDid[2])
  },
  {
    D0F0xBC_xE0107074_VClkDid3_OFFSET,
    D0F0xBC_xE0107074_VClkDid3_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, VclkDid[3])
  },
  {
    D0F0xBC_xE0107074_PowerplaySclkDpmValid0_OFFSET,
    D0F0xBC_xE0107074_PowerplaySclkDpmValid0_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkDpmValid[0])
  },
  {
    D0F0xBC_xE0107074_PowerplaySclkDpmValid1_OFFSET,
    D0F0xBC_xE0107074_PowerplaySclkDpmValid1_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkDpmValid[1])
  },
  {
    D0F0xBC_xE0107074_PowerplaySclkDpmValid2_OFFSET,
    D0F0xBC_xE0107074_PowerplaySclkDpmValid2_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkDpmValid[2])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0107077_TABLE [] = {
  {
    D0F0xBC_xE0107077_PowerplaySclkDpmValid3_OFFSET,
    D0F0xBC_xE0107077_PowerplaySclkDpmValid3_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkDpmValid[3])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0107078_TABLE [] = {
  {
    D0F0xBC_xE0107078_PowerplaySclkDpmValid4_OFFSET,
    D0F0xBC_xE0107078_PowerplaySclkDpmValid4_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkDpmValid[4])
  },
  {
    D0F0xBC_xE0107078_PowerplaySclkDpmValid5_OFFSET,
    D0F0xBC_xE0107078_PowerplaySclkDpmValid5_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkDpmValid[5])
  },
  {
    D0F0xBC_xE0107078_PowerplayPolicyLabel0_OFFSET,
    D0F0xBC_xE0107078_PowerplayPolicyLabel0_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, PolicyLabel[0])
  },
  {
    D0F0xBC_xE0107078_PowerplayPolicyLabel1_OFFSET,
    D0F0xBC_xE0107078_PowerplayPolicyLabel1_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, PolicyLabel[1])
  },
  {
    D0F0xBC_xE0107078_PowerplayPolicyLabel2_OFFSET,
    D0F0xBC_xE0107078_PowerplayPolicyLabel2_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, PolicyLabel[2])
  },
  {
    D0F0xBC_xE0107078_PowerplayPolicyLabel3_OFFSET,
    D0F0xBC_xE0107078_PowerplayPolicyLabel3_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, PolicyLabel[3])
  },
  {
    D0F0xBC_xE0107078_PowerplayPolicyLabel4_OFFSET,
    D0F0xBC_xE0107078_PowerplayPolicyLabel4_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, PolicyLabel[4])
  },
  {
    D0F0xBC_xE0107078_PowerplayPolicyLabel5_OFFSET,
    D0F0xBC_xE0107078_PowerplayPolicyLabel5_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, PolicyLabel[5])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE010707B_TABLE [] = {
  {
    D0F0xBC_xE010707B_PowerplayStateFlag0_OFFSET,
    D0F0xBC_xE010707B_PowerplayStateFlag0_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, PolicyFlags[0])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE010707C_TABLE [] = {
  {
    D0F0xBC_xE010707C_PowerplayStateFlag1_OFFSET,
    D0F0xBC_xE010707C_PowerplayStateFlag1_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, PolicyFlags[1])
  },
  {
    D0F0xBC_xE010707C_PowerplayStateFlag2_OFFSET,
    D0F0xBC_xE010707C_PowerplayStateFlag2_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, PolicyFlags[2])
  },
  {
    D0F0xBC_xE010707C_PowerplayStateFlag3_OFFSET,
    D0F0xBC_xE010707C_PowerplayStateFlag3_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, PolicyFlags[3])
  },
  {
    D0F0xBC_xE010707C_PowerplayStateFlag4_OFFSET,
    D0F0xBC_xE010707C_PowerplayStateFlag4_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, PolicyFlags[4])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE010707F_TABLE [] = {
  {
    D0F0xBC_xE010707F_PowerplayStateFlag5_OFFSET,
    D0F0xBC_xE010707F_PowerplayStateFlag5_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, PolicyFlags[5])
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xFF000000_TABLE [] = {
  {
    D0F0xBC_xFF000000_MainPllOpFreqIdStartup_OFFSET,
    D0F0xBC_xFF000000_MainPllOpFreqIdStartup_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, MainPllId)
  }
};

FUSE_REGISTER_ENTRY_TN D0F0xBC_xE0001008_TABLE [] = {
  {
    D0F0xBC_xE0001008_SClkVid0_OFFSET,
    D0F0xBC_xE0001008_SClkVid0_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkVid[0])
  },
  {
    D0F0xBC_xE0001008_SClkVid1_OFFSET,
    D0F0xBC_xE0001008_SClkVid1_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkVid[1])
  },
  {
    D0F0xBC_xE0001008_SClkVid2_OFFSET,
    D0F0xBC_xE0001008_SClkVid2_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkVid[2])
  },
  {
    D0F0xBC_xE0001008_SClkVid3_OFFSET,
    D0F0xBC_xE0001008_SClkVid3_WIDTH,
    (UINT16) offsetof (PP_FUSE_ARRAY, SclkVid[3])
  }
};


FUSE_TABLE_ENTRY_TN  FuseRegisterTableTN [] = {
  {
    D0F0xBC_xE0104158_TYPE,
    D0F0xBC_xE0104158_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0104158_TABLE),
    D0F0xBC_xE0104158_TABLE
  },
  {
    D0F0xBC_xE010415B_TYPE,
    D0F0xBC_xE010415B_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE010415B_TABLE),
    D0F0xBC_xE010415B_TABLE
  },
  {
    D0F0xBC_xE0104184_TYPE,
    D0F0xBC_xE0104184_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0104184_TABLE),
    D0F0xBC_xE0104184_TABLE
  },
  {
    D0F0xBC_xE0104187_TYPE,
    D0F0xBC_xE0104187_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0104187_TABLE),
    D0F0xBC_xE0104187_TABLE
  },
  {
    D0F0xBC_xE0104188_TYPE,
    D0F0xBC_xE0104188_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0104188_TABLE),
    D0F0xBC_xE0104188_TABLE
  },
  {
    D0F0xBC_xE0106020_TYPE,
    D0F0xBC_xE0106020_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0106020_TABLE),
    D0F0xBC_xE0106020_TABLE
  },
  {
    D0F0xBC_xE0106023_TYPE,
    D0F0xBC_xE0106023_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0106023_TABLE),
    D0F0xBC_xE0106023_TABLE
  },
  {
    D0F0xBC_xE0106024_TYPE,
    D0F0xBC_xE0106024_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0106024_TABLE),
    D0F0xBC_xE0106024_TABLE
  },
  {
    D0F0xBC_xE010705C_TYPE,
    D0F0xBC_xE010705C_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE010705C_TABLE),
    D0F0xBC_xE010705C_TABLE
  },
  {
    D0F0xBC_xE010705F_TYPE,
    D0F0xBC_xE010705F_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE010705F_TABLE),
    D0F0xBC_xE010705F_TABLE
  },
  {
    D0F0xBC_xE0107060_TYPE,
    D0F0xBC_xE0107060_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0107060_TABLE),
    D0F0xBC_xE0107060_TABLE
  },
  {
    D0F0xBC_xE0107063_TYPE,
    D0F0xBC_xE0107063_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0107063_TABLE),
    D0F0xBC_xE0107063_TABLE
  },
  {
    D0F0xBC_xE0107064_TYPE,
    D0F0xBC_xE0107064_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0107064_TABLE),
    D0F0xBC_xE0107064_TABLE
  },
  {
    D0F0xBC_xE0107067_TYPE,
    D0F0xBC_xE0107067_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0107067_TABLE),
    D0F0xBC_xE0107067_TABLE
  },
  {
    D0F0xBC_xE0107068_TYPE,
    D0F0xBC_xE0107068_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0107068_TABLE),
    D0F0xBC_xE0107068_TABLE
  },
  {
    D0F0xBC_xE010706B_TYPE,
    D0F0xBC_xE010706B_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE010706B_TABLE),
    D0F0xBC_xE010706B_TABLE
  },
  {
    D0F0xBC_xE010706C_TYPE,
    D0F0xBC_xE010706C_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE010706C_TABLE),
    D0F0xBC_xE010706C_TABLE
  },
  {
    D0F0xBC_xE010706F_TYPE,
    D0F0xBC_xE010706F_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE010706F_TABLE),
    D0F0xBC_xE010706F_TABLE
  },
  {
    D0F0xBC_xE0107070_TYPE,
    D0F0xBC_xE0107070_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0107070_TABLE),
    D0F0xBC_xE0107070_TABLE
  },
  {
    D0F0xBC_xE0107073_TYPE,
    D0F0xBC_xE0107073_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0107073_TABLE),
    D0F0xBC_xE0107073_TABLE
  },
  {
    D0F0xBC_xE0107074_TYPE,
    D0F0xBC_xE0107074_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0107074_TABLE),
    D0F0xBC_xE0107074_TABLE
  },
  {
    D0F0xBC_xE0107077_TYPE,
    D0F0xBC_xE0107077_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0107077_TABLE),
    D0F0xBC_xE0107077_TABLE
  },
  {
    D0F0xBC_xE0107078_TYPE,
    D0F0xBC_xE0107078_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0107078_TABLE),
    D0F0xBC_xE0107078_TABLE
  },
  {
    D0F0xBC_xE010707B_TYPE,
    D0F0xBC_xE010707B_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE010707B_TABLE),
    D0F0xBC_xE010707B_TABLE
  },
  {
    D0F0xBC_xE010707C_TYPE,
    D0F0xBC_xE010707C_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE010707C_TABLE),
    D0F0xBC_xE010707C_TABLE
  },
  {
    D0F0xBC_xE010707F_TYPE,
    D0F0xBC_xE010707F_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE010707F_TABLE),
    D0F0xBC_xE010707F_TABLE
  },
  {
    D0F0xBC_xFF000000_TYPE,
    D0F0xBC_xFF000000_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xFF000000_TABLE),
    D0F0xBC_xFF000000_TABLE
  },
  {
    D0F0xBC_xE0001008_TYPE,
    D0F0xBC_xE0001008_ADDRESS,
    ARRAY_SIZE(D0F0xBC_xE0001008_TABLE),
    D0F0xBC_xE0001008_TABLE
  }
};

FUSE_TABLE_TN  FuseTableTN = {
  ARRAY_SIZE(FuseRegisterTableTN),
  FuseRegisterTableTN
};

/*----------------------------------------------------------------------------------------*/
/**
 * Load Fuse Table TN
 *
 *
 * @param[out] PpFuseArray      Pointer to save fuse table
 * @param[in]  StdHeader        Pointer to Standard configuration
 * @retval     AGESA_STATUS
 */

STATIC VOID
NbFuseLoadFuseTableTN (
     OUT   PP_FUSE_ARRAY       *PpFuseArray,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  FUSE_TABLE_TN             *FuseTable;
  UINTN                     RegisterIndex;
  FuseTable = &FuseTableTN;
  for (RegisterIndex = 0; RegisterIndex < FuseTable->FuseTableLength; RegisterIndex++ ) {
    UINTN   FieldIndex;
    UINTN   FuseRegisterTableLength;
    UINT32  FuseValue;
    FuseRegisterTableLength = FuseTable->FuseTable[RegisterIndex].FuseRegisterTableLength;

    GnbRegisterReadTN (
      FuseTable->FuseTable[RegisterIndex].RegisterSpaceType,
      FuseTable->FuseTable[RegisterIndex].Register,
      &FuseValue,
      0,
      StdHeader
    );
    for (FieldIndex = 0; FieldIndex < FuseRegisterTableLength;  FieldIndex++) {
      FUSE_REGISTER_ENTRY_TN   RegisterEntry;
      RegisterEntry = FuseTable->FuseTable[RegisterIndex].FuseRegisterTable[FieldIndex];
      *((UINT8 *) PpFuseArray + RegisterEntry.FuseOffset) = (UINT8) ((FuseValue >> RegisterEntry.FieldOffset) &
                                                            ((1 << RegisterEntry.FieldWidth) - 1));
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Gnb load fuse table
 *
 *
 *
 * @param[in] StdHeader        Pointer to Standard configuration
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
GnbLoadFuseTableTN (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  PP_FUSE_ARRAY          *PpFuseArray;
  AGESA_STATUS           Status;
  D18F3xA0_STRUCT        D18F3xA0;

  Status = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbLoadFuseTableTN Enter\n");

  PpFuseArray = (PP_FUSE_ARRAY *) GnbAllocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, sizeof (PP_FUSE_ARRAY), StdHeader);
  ASSERT (PpFuseArray != NULL);
  if (PpFuseArray != NULL) {
    //Support for real fuste table
    GnbRegisterReadTN (D18F3xA0_TYPE, D18F3xA0_ADDRESS, &D18F3xA0.Value, 0, StdHeader);
    if ((D18F3xA0.Field.CofVidProg) && (GnbBuildOptions.GnbLoadRealFuseTable)) {
      NbFuseLoadFuseTableTN (PpFuseArray, StdHeader);
      PpFuseArray->VceSateTableSupport = TRUE;
      IDS_HDT_CONSOLE (NB_MISC, "  Processor Fused\n");
    } else {
      LibAmdMemCopy (PpFuseArray, &ex907 , sizeof (PP_FUSE_ARRAY), StdHeader);
      IDS_HDT_CONSOLE (NB_MISC, "  Processor Unfuse\n");
    }
  } else {
    Status = AGESA_ERROR;
  }
  IDS_OPTION_CALLOUT (IDS_CALLOUT_GNB_PPFUSE_OVERRIDE, PpFuseArray, StdHeader);
  GnbFuseTableDebugDumpTN (PpFuseArray, StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbLoadFuseTableTN Exit [0x%x]\n", Status);
  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Debug dump fuse table
 *
 *
 * @param[out] PpFuseArray      Pointer to save fuse table
 * @param[in]  StdHeader        Pointer to Standard configuration
 */

VOID
GnbFuseTableDebugDumpTN (
  IN       PP_FUSE_ARRAY       *PpFuseArray,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINTN   Index;

  IDS_HDT_CONSOLE (NB_MISC, "<------------ GNB FUSE TABLE------------>\n");
  for (Index = 0; Index < 4; Index++) {
    if (PpFuseArray->LclkDpmValid[Index]  != 0) {
      IDS_HDT_CONSOLE (
        NB_MISC,
        "  LCLK DID[%d] - 0x%02x (%dMHz)\n",
        Index,
        PpFuseArray->LclkDpmDid[Index],
        (PpFuseArray->LclkDpmDid[Index] != 0) ? (GfxFmCalculateClock (PpFuseArray->LclkDpmDid[Index], StdHeader) / 100) : 0
        );
      IDS_HDT_CONSOLE (NB_MISC, "  LCLK VID[%d] - 0x02%x\n", Index, PpFuseArray->LclkDpmVid[Index]);
    }
  }
  for (Index = 0; Index < 4; Index++) {
    IDS_HDT_CONSOLE (
      NB_MISC,
      "  VCLK DID[%d] - 0x%02x (%dMHz)\n",
      Index,
      PpFuseArray->VclkDid[Index],
      (PpFuseArray->VclkDid[Index] != 0) ? (GfxFmCalculateClock (PpFuseArray->VclkDid[Index], StdHeader) / 100) : 0
      );
    IDS_HDT_CONSOLE (
      NB_MISC,
      "  DCLK DID[%d] - 0x%02x (%dMHz)\n",
      Index,
      PpFuseArray->DclkDid[Index],
      (PpFuseArray->DclkDid[Index] != 0) ? (GfxFmCalculateClock (PpFuseArray->DclkDid[Index], StdHeader) / 100) : 0
    );
  }
  for (Index = 0; Index < 4; Index++) {
    IDS_HDT_CONSOLE (
      NB_MISC,
      "  DISPCLK DID[%d] - 0x%02x (%dMHz)\n",
      Index,
      PpFuseArray->DisplclkDid[Index],
      (PpFuseArray->DisplclkDid[Index] != 0) ? (GfxFmCalculateClock (PpFuseArray->DisplclkDid[Index], StdHeader) / 100) : 0
      );
  }
  for (Index = 0; Index < 4; Index++) {
    IDS_HDT_CONSOLE (
      NB_MISC,
      "  ECLK DID[%d] - 0x%02x (%dMHz)\n",
      Index,
      PpFuseArray->EclkDid[Index],
      (PpFuseArray->EclkDid[Index] != 0) ? (GfxFmCalculateClock (PpFuseArray->EclkDid[Index], StdHeader) / 100) : 0
      );
    IDS_HDT_CONSOLE (
      NB_MISC,
      "  VCE SCLK DID[%d] - 0x%02x (%dMHz)\n",
      Index,
      PpFuseArray->SclkDpmDid[PpFuseArray->VceReqSclkSel[Index]],
      (PpFuseArray->SclkDpmDid[PpFuseArray->VceReqSclkSel[Index]] != 0) ? (GfxFmCalculateClock (PpFuseArray->SclkDpmDid[PpFuseArray->VceReqSclkSel[Index]], StdHeader) / 100) : 0
      );
    IDS_HDT_CONSOLE (
      NB_MISC,
      "  VCE Flags[ % d] - 0x % 02x\n",
      Index,
      PpFuseArray->VceFlags[Index]
      );
  }
  for (Index = 0; Index < 6; Index++) {
    IDS_HDT_CONSOLE (
      NB_MISC,
      "  SCLK DID[%d] - 0x%02x (%dMHz)\n",
      Index,
      PpFuseArray->SclkDpmDid[Index],
      (PpFuseArray->SclkDpmDid[Index] != 0) ? (GfxFmCalculateClock (PpFuseArray->SclkDpmDid[Index], StdHeader) / 100) : 0
    );
    IDS_HDT_CONSOLE (
      NB_MISC,
      "  SCLK TDP[%d] - 0x%x \n",
      Index,
      PpFuseArray->SclkDpmTdpLimit[Index]
    );
    IDS_HDT_CONSOLE (NB_MISC, "  SCLK VID[%d] - 0x%02x\n", Index, PpFuseArray->SclkDpmVid[Index]);
  }
  for (Index = 0; Index < 6; Index++) {
    IDS_HDT_CONSOLE (NB_MISC, "  State #%d\n", Index);
  }
  IDS_HDT_CONSOLE (NB_MISC, "<------------ GNB FUSE END-------------->\n");
}
