/* $NoKeywords:$ */
/**
 * @file
 *
 * Graphics Controller family specific service procedure
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 48498 $   @e \$Date: 2011-03-09 12:44:53 -0700 (Wed, 09 Mar 2011) $
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
#include  "AGESA.h"
#include  "amdlib.h"
#include  "Ids.h"
#include  "Gnb.h"
#include  "GnbFuseTable.h"
#include  "GnbPcie.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  GNB_MODULE_DEFINITIONS (GnbGfxInitLibV1)
#include  GNB_MODULE_DEFINITIONS (GnbPcieConfig)
#include  "NbConfigData.h"
#include  "OptionGnb.h"
#include  "NbLclkDpm.h"
#include  "NbFamilyServices.h"
#include  "NbPowerMgmt.h"
#include  "GfxLib.h"
#include  "GnbRegistersON.h"
#include  "cpuFamilyTranslation.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_NB_FAMILY_0X14_F14NBSERVICES_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

extern GNB_BUILD_OPTIONS  GnbBuildOptions;
FUSE_TABLE  FuseTable;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * NB family specific clock gating
 *
 *
 * @param[in, out] NbClkGatingCtrl    Pointer to NB_CLK_GATING_CTRL
 * @param[in] StdHeader               Pointer to AMD_CONFIG_PARAMS
 */
VOID
NbFmNbClockGating (
  IN OUT   VOID  *NbClkGatingCtrl,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  NB_CLK_GATING_CTRL  *NbClkGatingCtrlPtr;
  CPU_LOGICAL_ID  LogicalId;

  NbClkGatingCtrlPtr = (NB_CLK_GATING_CTRL *)NbClkGatingCtrl;
  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  if ((LogicalId.Revision & AMD_F14_ON_Cx) != 0) {
    NbClkGatingCtrlPtr->Smu_Sclk_Gating = FALSE;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * UnitID Clumping
 *
 *
 * @param[in] NbPciAddress
 * @param[in] StdHeader
 * @retval    AGESA_STATUS
 */

VOID
NbFmClumpUnitID (
  IN      PCI_ADDR            NbPciAddress,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get Fuse translation table
 *
 *
 * @retval    pointer to fuse translation table
 */

FUSE_TABLE*
NbFmGetFuseTranslationTable (
  VOID
  )
{
  return  &FuseTable;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Family specific fuse table patch
 * Is's correct behavior if we would have 4 states, it would be
 *   PP_FUSE_ARRAY->LclkDpmDid[0] - Goes to State 5
 *   PP_FUSE_ARRAY->LclkDpmDid[1] - Goes to State 6
 *   PP_FUSE_ARRAY->LclkDpmDid[2] - Goes to State 7
 * If we would have 4 states it would be
 *   PP_FUSE_ARRAY->LclkDpmDid[0] - Goes to State 4
 *   PP_FUSE_ARRAY->LclkDpmDid[1] - Goes to State 5
 *   PP_FUSE_ARRAY->LclkDpmDid[2] - Goes to State 6
 *   PP_FUSE_ARRAY->LclkDpmDid[3] - Goes to State 7
 *
 * @param[in] PpFuseArray             Pointer to PP_FUSE_ARRAY
 * @param[in] StdHeader               Pointer to AMD_CONFIG_PARAMS
 */
VOID
NbFmFuseAdjustFuseTablePatch (
  IN OUT   PP_FUSE_ARRAY       *PpFuseArray,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8           LclkDpmMode;
  UINT8           SwSatateIndex;
  UINT8           MaxSclkIndex;
  UINT8           DpmStateIndex;
  UINT8           CurrentSclkDpmDid;
  CPU_LOGICAL_ID  LogicalId;

  LclkDpmMode = GnbBuildOptions.LclkDpmEn ? LclkDpmRcActivity : LclkDpmDisabled;
  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  if ((LogicalId.Revision & (AMD_F14_ON_A0 | AMD_F14_ON_A1)) != 0) {
    LclkDpmMode = LclkDpmDisabled;
  }
  IDS_OPTION_HOOK (IDS_GNB_LCLK_DPM_EN, &LclkDpmMode, StdHeader);

  //For all CPU rev  LclkDpmValid[3] = 0
  PpFuseArray->LclkDpmValid[3] = 0;
  PpFuseArray->LclkDpmVid[3] = 0;
  PpFuseArray->LclkDpmDid[3] = 0;

  // For LCLKDPM set LclkDpmVid[0] = 0, no matter if LCLK DMP enable or disable.
  PpFuseArray->LclkDpmVid[0] = 0;

  if (LclkDpmMode != LclkDpmRcActivity) {
    //If LCLK DPM disable (LclkDpmMode != LclkDpmRcActivity)
    // -  LclkDpmDid[1,2] = LclkDpmDid [0],  LclkDpmVid[1,2] = LclkDpmVid[0]
    // -  Execute LCLK DPM init

    PpFuseArray->LclkDpmVid[1] = PpFuseArray->LclkDpmVid[0];
    PpFuseArray->LclkDpmVid[2] = PpFuseArray->LclkDpmVid[0];
    PpFuseArray->LclkDpmDid[1] = PpFuseArray->LclkDpmDid[0];
    PpFuseArray->LclkDpmDid[2] = PpFuseArray->LclkDpmDid[0];
    IDS_HDT_CONSOLE (NB_MISC, "  F14 LCLK DPM Mode Disable -- use DPM0 fusing\n");

  } else {
    // If LCLK DPM enabled
    // - use fused values for LclkDpmDid[0,1,2] and appropriate voltage
    // - Execute LCLK DPM init
    PpFuseArray->LclkDpmVid[2] = PpFuseArray->PcieGen2Vid;
    if (GfxLibIsControllerPresent (StdHeader)) {
      //VID index = VID index associated with highest SCLK DPM state in the Powerplay state where Label_Performance=1 // This would ignore the UVD case (where Label_Performance would be 0).
      for (SwSatateIndex = 0 ; SwSatateIndex < PP_FUSE_MAX_NUM_SW_STATE; SwSatateIndex++) {
        if (PpFuseArray->PolicyLabel[SwSatateIndex] == POLICY_LABEL_PERFORMANCE) {
          break;
        }
      }
      MaxSclkIndex = 0;
      CurrentSclkDpmDid = 0xff;
      ASSERT (PpFuseArray->SclkDpmValid[SwSatateIndex] != 0);
      for (DpmStateIndex = 0; DpmStateIndex < PP_FUSE_MAX_NUM_DPM_STATE; DpmStateIndex++) {
        if ((PpFuseArray->SclkDpmValid[SwSatateIndex] & (1 << DpmStateIndex)) != 0) {
          if (PpFuseArray->SclkDpmDid[DpmStateIndex] < CurrentSclkDpmDid) {
            CurrentSclkDpmDid = PpFuseArray->SclkDpmDid[DpmStateIndex];
            MaxSclkIndex = DpmStateIndex;
          }
        }
      }
      PpFuseArray->LclkDpmVid[1] = PpFuseArray->SclkDpmVid[MaxSclkIndex];
    } else {
      PpFuseArray->LclkDpmVid[1] = PpFuseArray->LclkDpmVid[0];
      PpFuseArray->LclkDpmDid[1] = PpFuseArray->LclkDpmDid[0];
    }
    // - use fused values for LclkDpmDid[0,1,2] and appropriate voltage
    //Keep using actual fusing
    IDS_HDT_CONSOLE (NB_MISC, "  LCLK DPM use actual fusing.\n");
  }

  //Patch SclkThermDid to 175Mhz if not fused
  if (PpFuseArray->SclkThermDid == 0) {
    PpFuseArray->SclkThermDid = GfxLibCalculateDid (175 * 100, GfxLibGetMainPllFreq (StdHeader) * 100);
  }
}


/*----------------------------------------------------------------------------------------
 *           FUSE translation table
 *----------------------------------------------------------------------------------------
 */

FUSE_REGISTER_ENTRY FCRxFE00_600E_TABLE [] = {
  {
    FCRxFE00_600E_MainPllOpFreqIdStartup_OFFSET,
    FCRxFE00_600E_MainPllOpFreqIdStartup_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, MainPllId)
  },
  {
    FCRxFE00_600E_WrCkDid_OFFSET,
    FCRxFE00_600E_WrCkDid_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, WrCkDid)
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70A2_TABLE [] = {
  {
    FCRxFE00_70A2_PPlayTableRev_OFFSET,
    FCRxFE00_70A2_PPlayTableRev_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PPlayTableRev)
  },
  {
    FCRxFE00_70A2_SclkThermDid_OFFSET,
    FCRxFE00_70A2_SclkThermDid_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkThermDid)
  },
  {
    FCRxFE00_70A2_PcieGen2Vid_OFFSET,
    FCRxFE00_70A2_PcieGen2Vid_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PcieGen2Vid)
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70A4_TABLE [] = {
  {
    FCRxFE00_70A4_SclkDpmVid0_OFFSET,
    FCRxFE00_70A4_SclkDpmVid0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmVid[0])
  },
  {
    FCRxFE00_70A4_SclkDpmVid1_OFFSET,
    FCRxFE00_70A4_SclkDpmVid1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmVid[1])
  },
  {
    FCRxFE00_70A4_SclkDpmVid2_OFFSET,
    FCRxFE00_70A4_SclkDpmVid2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmVid[2])
  },
  {
    FCRxFE00_70A4_SclkDpmVid3_OFFSET,
    FCRxFE00_70A4_SclkDpmVid3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmVid[3])
  },
  {
    FCRxFE00_70A4_SclkDpmVid4_OFFSET,
    FCRxFE00_70A4_SclkDpmVid4_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmVid[4])
  },
};

FUSE_REGISTER_ENTRY FCRxFE00_70A5_TABLE [] = {
  {
    FCRxFE00_70A5_SclkDpmDid0_OFFSET,
    FCRxFE00_70A5_SclkDpmDid0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmDid[0])
  },
  {
    FCRxFE00_70A5_SclkDpmDid1_OFFSET,
    FCRxFE00_70A5_SclkDpmDid1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmDid[1])
  },
  {
    FCRxFE00_70A5_SclkDpmDid2_OFFSET,
    FCRxFE00_70A5_SclkDpmDid2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmDid[2])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70A8_TABLE [] = {
  {
    FCRxFE00_70A8_SclkDpmDid3_OFFSET,
    FCRxFE00_70A8_SclkDpmDid3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmDid[3])
  },
  {
    FCRxFE00_70A8_SclkDpmDid4_OFFSET,
    FCRxFE00_70A8_SclkDpmDid4_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmDid[4])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70AA_TABLE [] = {
  {
    FCRxFE00_70AA_SclkDpmCacBase_OFFSET,
    FCRxFE00_70AA_SclkDpmCacBase_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmCac[4])
  }
};


FUSE_REGISTER_ENTRY FCRxFE00_70AE_TABLE [] = {
  {
    FCRxFE00_70AE_DispClkDid0_OFFSET,
    FCRxFE00_70AE_DispClkDid0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, DisplclkDid[0])
  },
  {
    FCRxFE00_70AE_DispClkDid1_OFFSET,
    FCRxFE00_70AE_DispClkDid1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, DisplclkDid[1])
  },
  {
    FCRxFE00_70AE_DispClkDid2_OFFSET,
    FCRxFE00_70AE_DispClkDid2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, DisplclkDid[2])
  },
  {
    FCRxFE00_70AE_DispClkDid3_OFFSET,
    FCRxFE00_70AE_DispClkDid3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, DisplclkDid[3])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70B1_TABLE [] = {
  {
    FCRxFE00_70B1_LclkDpmDid0_OFFSET,
    FCRxFE00_70B1_LclkDpmDid0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, LclkDpmDid[0])
  },
  {
    FCRxFE00_70B1_LclkDpmDid1_OFFSET,
    FCRxFE00_70B1_LclkDpmDid1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, LclkDpmDid[1])
  },
  {
    FCRxFE00_70B1_LclkDpmDid2_OFFSET,
    FCRxFE00_70B1_LclkDpmDid2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, LclkDpmDid[2])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70B4_TABLE [] = {
  {
    FCRxFE00_70B4_LclkDpmDid3_OFFSET,
    FCRxFE00_70B4_LclkDpmDid3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, LclkDpmDid[3])
  },
  {
    FCRxFE00_70B4_LclkDpmValid0_OFFSET,
    FCRxFE00_70B4_LclkDpmValid0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, LclkDpmValid[0])
  },
  {
    FCRxFE00_70B4_LclkDpmValid1_OFFSET,
    FCRxFE00_70B4_LclkDpmValid1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, LclkDpmValid[1])
  },
  {
    FCRxFE00_70B4_LclkDpmValid2_OFFSET,
    FCRxFE00_70B4_LclkDpmValid2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, LclkDpmValid[2])
  },
  {
    FCRxFE00_70B4_LclkDpmValid3_OFFSET,
    FCRxFE00_70B4_LclkDpmValid3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, LclkDpmValid[3])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70B5_TABLE [] = {
  {
    FCRxFE00_70B5_DclkDid0_OFFSET,
    FCRxFE00_70B5_DclkDid0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, DclkDid[0])
  },
  {
    FCRxFE00_70B5_DclkDid1_OFFSET,
    FCRxFE00_70B5_DclkDid1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, DclkDid[1])
  },
  {
    FCRxFE00_70B5_DclkDid2_OFFSET,
    FCRxFE00_70B5_DclkDid2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, DclkDid[2])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70B8_TABLE [] = {
  {
    FCRxFE00_70B8_DclkDid3_OFFSET,
    FCRxFE00_70B8_DclkDid3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, DclkDid[3])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70B9_TABLE [] = {
  {
    FCRxFE00_70B9_VclkDid0_OFFSET,
    FCRxFE00_70B9_VclkDid0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDid[0])
  },
  {
    FCRxFE00_70B9_VclkDid1_OFFSET,
    FCRxFE00_70B9_VclkDid1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDid[1])
  },
  {
    FCRxFE00_70B9_VclkDid2_OFFSET,
    FCRxFE00_70B9_VclkDid2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDid[2])
  },
  {
    FCRxFE00_70B9_VclkDid3_OFFSET,
    FCRxFE00_70B9_VclkDid3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDid[3])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70BC_TABLE [] = {
  {
    FCRxFE00_70BC_SclkDpmValid0_OFFSET,
    FCRxFE00_70BC_SclkDpmValid0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmValid[0])
  },
  {
    FCRxFE00_70BC_SclkDpmValid1_OFFSET,
    FCRxFE00_70BC_SclkDpmValid1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmValid[1])
  },
  {
    FCRxFE00_70BC_SclkDpmValid2_OFFSET,
    FCRxFE00_70BC_SclkDpmValid2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmValid[2])
  },
  {
    FCRxFE00_70BC_SclkDpmValid3_OFFSET,
    FCRxFE00_70BC_SclkDpmValid3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmValid[3])
  },
  {
    FCRxFE00_70BC_SclkDpmValid4_OFFSET,
    FCRxFE00_70BC_SclkDpmValid4_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmValid[4])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70BF_TABLE [] = {
  {
    FCRxFE00_70BF_SclkDpmValid5_OFFSET,
    FCRxFE00_70BF_SclkDpmValid5_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmValid[5])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70C0_TABLE [] = {
  {
    FCRxFE00_70C0_PolicyLabel0_OFFSET,
    FCRxFE00_70C0_PolicyLabel0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyLabel[0])
  },
  {
    FCRxFE00_70C0_PolicyLabel1_OFFSET,
    FCRxFE00_70C0_PolicyLabel1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyLabel[1])
  },
  {
    FCRxFE00_70C0_PolicyLabel2_OFFSET,
    FCRxFE00_70C0_PolicyLabel2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyLabel[2])
  },
  {
    FCRxFE00_70C0_PolicyLabel3_OFFSET,
    FCRxFE00_70C0_PolicyLabel3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyLabel[3])
  },
  {
    FCRxFE00_70C0_PolicyLabel4_OFFSET,
    FCRxFE00_70C0_PolicyLabel4_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyLabel[4])
  },
  {
    FCRxFE00_70C0_PolicyLabel5_OFFSET,
    FCRxFE00_70C0_PolicyLabel5_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyLabel[5])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70C1_TABLE [] = {
  {
    FCRxFE00_70C1_PolicyFlags0_OFFSET,
    FCRxFE00_70C1_PolicyFlags0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyFlags[0])
  },
  {
    FCRxFE00_70C1_PolicyFlags1_OFFSET,
    FCRxFE00_70C1_PolicyFlags1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyFlags[1])
  },
  {
    FCRxFE00_70C1_PolicyFlags2_OFFSET,
    FCRxFE00_70C1_PolicyFlags2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyFlags[2])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70C4_TABLE [] = {
    {
    FCRxFE00_70C4_PolicyFlags3_OFFSET,
    FCRxFE00_70C4_PolicyFlags3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyFlags[3])
  },
  {
    FCRxFE00_70C4_PolicyFlags4_OFFSET,
    FCRxFE00_70C4_PolicyFlags4_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyFlags[4])
  },
  {
    FCRxFE00_70C4_PolicyFlags5_OFFSET,
    FCRxFE00_70C4_PolicyFlags5_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyFlags[5])
  }
};


FUSE_REGISTER_ENTRY FCRxFE00_70C7_TABLE [] = {
  {
    FCRxFE00_70C7_DclkVclkSel0_OFFSET,
    FCRxFE00_70C7_DclkVclkSel0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDclkSel[0])
  },
  {
    FCRxFE00_70C7_DclkVclkSel1_OFFSET,
    FCRxFE00_70C7_DclkVclkSel1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDclkSel[1])
  },
  {
    FCRxFE00_70C7_DclkVclkSel2_OFFSET,
    FCRxFE00_70C7_DclkVclkSel2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDclkSel[2])
  },
  {
    FCRxFE00_70C7_DclkVclkSel3_OFFSET,
    FCRxFE00_70C7_DclkVclkSel3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDclkSel[3])
  },

  {
    FCRxFE00_70C7_DclkVclkSel4_OFFSET,
    FCRxFE00_70C7_DclkVclkSel4_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDclkSel[4])
  },
  {
    FCRxFE00_70C7_DclkVclkSel5_OFFSET,
    FCRxFE00_70C7_DclkVclkSel5_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDclkSel[5])
  },
};

FUSE_REGISTER_ENTRY FCRxFE00_70C8_TABLE [] = {
  {
    FCRxFE00_70C8_GpuBoostCap_OFFSET,
    FCRxFE00_70C8_GpuBoostCap_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, GpuBoostCap)
  },
  {
    FCRxFE00_70C8_SclkDpmVid5_OFFSET,
    FCRxFE00_70C8_SclkDpmVid5_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmVid[5])
  },
  {
    FCRxFE00_70C8_SclkDpmDid5_OFFSET,
    FCRxFE00_70C8_SclkDpmDid5_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmDid[5])
  },
};

FUSE_REGISTER_ENTRY FCRxFE00_70C9_TABLE [] = {
  {
    FCRxFE00_70C9_SclkDpmTdpLimit0_OFFSET,
    FCRxFE00_70C9_SclkDpmTdpLimit0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmTdpLimit[0])
  },
  {
    FCRxFE00_70C9_SclkDpmTdpLimit1_OFFSET,
    FCRxFE00_70C9_SclkDpmTdpLimit1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmTdpLimit[1])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70CC_TABLE [] = {
  {
    FCRxFE00_70CC_SclkDpmTdpLimit2_OFFSET,
    FCRxFE00_70CC_SclkDpmTdpLimit2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmTdpLimit[2])
  },
  {
    FCRxFE00_70CC_SclkDpmTdpLimit3_OFFSET,
    FCRxFE00_70CC_SclkDpmTdpLimit3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmTdpLimit[3])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70CF_TABLE [] = {
  {
    FCRxFE00_70CF_SclkDpmTdpLimit4_OFFSET,
    FCRxFE00_70CF_SclkDpmTdpLimit4_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmTdpLimit[4])
  },
  {
    FCRxFE00_70CF_SclkDpmTdpLimit5_OFFSET,
    FCRxFE00_70CF_SclkDpmTdpLimit5_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmTdpLimit[5])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70D2_TABLE [] = {
  {
    FCRxFE00_70D2_SclkDpmTdpLimitPG_OFFSET,
    FCRxFE00_70D2_SclkDpmTdpLimitPG_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmTdpLimitPG)
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70D4_TABLE [] = {
  {
    FCRxFE00_70D4_SclkDpmBoostMargin_OFFSET,
    FCRxFE00_70D4_SclkDpmBoostMargin_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmBoostMargin)
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_70D7_TABLE [] = {
  {
    FCRxFE00_70D7_SclkDpmThrottleMargin_OFFSET,
    FCRxFE00_70D7_SclkDpmThrottleMargin_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmThrottleMargin)
  }
};

FUSE_TABLE_ENTRY  FuseRegisterTable [] = {
  {
    FCRxFE00_70A2_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70A2_TABLE),
    FCRxFE00_70A2_TABLE
  },
  {
    FCRxFE00_70A4_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70A4_TABLE),
    FCRxFE00_70A4_TABLE
  },
  {
    FCRxFE00_70A5_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70A5_TABLE),
    FCRxFE00_70A5_TABLE
  },
  {
    FCRxFE00_70A8_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70A8_TABLE),
    FCRxFE00_70A8_TABLE
  },
  {
    FCRxFE00_600E_ADDRESS,
    ARRAY_SIZE(FCRxFE00_600E_TABLE),
    FCRxFE00_600E_TABLE
  },
  {
    FCRxFE00_70AA_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70AA_TABLE),
    FCRxFE00_70AA_TABLE
  },
  {
    FCRxFE00_70AE_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70AE_TABLE),
    FCRxFE00_70AE_TABLE
  },
  {
    FCRxFE00_70B1_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70B1_TABLE),
    FCRxFE00_70B1_TABLE
  },
  {
    FCRxFE00_70B4_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70B4_TABLE),
    FCRxFE00_70B4_TABLE
  },
  {
    FCRxFE00_70B5_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70B5_TABLE),
    FCRxFE00_70B5_TABLE
  },
  {
    FCRxFE00_70B8_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70B8_TABLE),
    FCRxFE00_70B8_TABLE
  },
  {
    FCRxFE00_70B9_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70B9_TABLE),
    FCRxFE00_70B9_TABLE
  },
  {
    FCRxFE00_70BC_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70BC_TABLE),
    FCRxFE00_70BC_TABLE
  },
  {
    FCRxFE00_70BF_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70BF_TABLE),
    FCRxFE00_70BF_TABLE
  },
  {
    FCRxFE00_70C0_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70C0_TABLE),
    FCRxFE00_70C0_TABLE
  },
  {
    FCRxFE00_70C1_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70C1_TABLE),
    FCRxFE00_70C1_TABLE
  },
  {
    FCRxFE00_70C4_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70C4_TABLE),
    FCRxFE00_70C4_TABLE
  },
  {
    FCRxFE00_70C7_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70C7_TABLE),
    FCRxFE00_70C7_TABLE
  },
  {
    FCRxFE00_70C8_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70C8_TABLE),
    FCRxFE00_70C8_TABLE
  },
  {
    FCRxFE00_70C9_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70C9_TABLE),
    FCRxFE00_70C9_TABLE
  },
  {
    FCRxFE00_70CC_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70CC_TABLE),
    FCRxFE00_70CC_TABLE
  },
  {
    FCRxFE00_70CF_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70CF_TABLE),
    FCRxFE00_70CF_TABLE
  },
  {
    FCRxFE00_70D2_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70D2_TABLE),
    FCRxFE00_70D2_TABLE
  },
  {
    FCRxFE00_70D4_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70D4_TABLE),
    FCRxFE00_70D4_TABLE
  },
  {
    FCRxFE00_70D7_ADDRESS,
    ARRAY_SIZE(FCRxFE00_70D7_TABLE),
    FCRxFE00_70D7_TABLE
  },
};

FUSE_TABLE  FuseTable = {
  ARRAY_SIZE(FuseRegisterTable),
  FuseRegisterTable
};
