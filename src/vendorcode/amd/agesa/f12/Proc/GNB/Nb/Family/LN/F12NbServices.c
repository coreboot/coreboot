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
 * @e \$Revision: 48452 $   @e \$Date: 2011-03-09 12:50:44 +0800 (Wed, 09 Mar 2011) $
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
#include  "GnbCommonLib.h"
#include  "GnbGfxInitLibV1.h"
#include  "GnbNbInitLibV1.h"
#include  "GnbPcieConfig.h"
#include  "NbConfigData.h"
#include  "OptionGnb.h"
#include  "NbLclkDpm.h"
#include  "NbFamilyServices.h"
#include  "NbPowerMgmt.h"
#include  "GnbRegistersLN.h"
#include  "cpuFamilyTranslation.h"
#include  "GfxLib.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_NB_FAMILY_LN_F12NBSERVICES_FILECODE
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
  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * UnitID Clumping
 *
 *
 * @param[in] NbPciAddress
 * @param[in] StdHeader   Standard Configuration Header
 * @retval    AGESA_STATUS
 */

VOID
NbFmClumpUnitID (
  IN      PCI_ADDR            NbPciAddress,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  GnbClumpUnitID (NbPciAddress, StdHeader);
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
  UINT8         LclkDpmMode;
  UINT8         SwSatateIndex;
  UINT8         MaxSclkIndex;
  UINT8         DpmStateIndex;
  UINT8         CurrentSclkDpmDid;
  CPU_LOGICAL_ID  LogicalId;
  D18F3x15C_STRUCT      D18F3x15C;

  LclkDpmMode = GnbBuildOptions.LclkDpmEn ? LclkDpmRcActivity : LclkDpmDisabled;
  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  if ((LogicalId.Revision & (AMD_F12_LN_A0 | AMD_F12_LN_A1)) != 0) {
    LclkDpmMode = LclkDpmDisabled;
  }
  IDS_OPTION_HOOK (IDS_GNB_LCLK_DPM_EN, &LclkDpmMode, StdHeader);

  // Read Sclk VID
  GnbLibPciRead (
    MAKE_SBDFO ( 0, 0, 0x18, 3, D18F3x15C_ADDRESS),
    AccessWidth32,
    &D18F3x15C.Value,
    StdHeader
    );
  PpFuseArray->SclkVid[0] = (UINT8) (D18F3x15C.Field.SclkVidLevel0);
  PpFuseArray->SclkVid[1] = (UINT8) (D18F3x15C.Field.SclkVidLevel1);
  PpFuseArray->SclkVid[2] = (UINT8) (D18F3x15C.Field.SclkVidLevel2);
  PpFuseArray->SclkVid[3] = (UINT8) (D18F3x15C.Field.SclkVidLevel3);

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
    IDS_HDT_CONSOLE (NB_MISC, "  F12 LCLK DPM Mode Disable -- use DPM0 fusing\n");

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

  //Patch SclkThermDid to 200Mhz if not fused
  if (PpFuseArray->SclkThermDid == 0) {
    PpFuseArray->SclkThermDid = GfxLibCalculateDid (200 * 100, GfxLibGetMainPllFreq (StdHeader) * 100);
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

FUSE_REGISTER_ENTRY FCRxFE00_6022_TABLE [] = {
  {
    FCRxFE00_6022_DclkVclkSel0_OFFSET,
    FCRxFE00_6022_DclkVclkSel0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDclkSel[0])
  },
  {
    FCRxFE00_6022_DclkVclkSel1_OFFSET,
    FCRxFE00_6022_DclkVclkSel1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDclkSel[1])
  },
  {
    FCRxFE00_6022_DclkVclkSel2_OFFSET,
    FCRxFE00_6022_DclkVclkSel2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDclkSel[2])
  },
  {
    FCRxFE00_6022_DclkVclkSel3_OFFSET,
    FCRxFE00_6022_DclkVclkSel3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDclkSel[3])
  },
  {
    FCRxFE00_6022_DclkVclkSel4_OFFSET,
    FCRxFE00_6022_DclkVclkSel4_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDclkSel[4])
  },
  {
    FCRxFE00_6022_DclkVclkSel5_OFFSET,
    FCRxFE00_6022_DclkVclkSel5_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDclkSel[5])
  },
};

FUSE_REGISTER_ENTRY FCRxFE00_7103_TABLE [] = {
  {
    FCRxFE00_7103_SclkDpmVid0_OFFSET,
    FCRxFE00_7103_SclkDpmVid0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmVid[0])
  },
  {
    FCRxFE00_7103_SclkDpmVid1_OFFSET,
    FCRxFE00_7103_SclkDpmVid1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmVid[1])
  },
  {
    FCRxFE00_7103_SclkDpmVid2_OFFSET,
    FCRxFE00_7103_SclkDpmVid2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmVid[2])
  },
  {
    FCRxFE00_7103_SclkDpmVid3_OFFSET,
    FCRxFE00_7103_SclkDpmVid3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmVid[3])
  },
  {
    FCRxFE00_7103_SclkDpmVid4_OFFSET,
    FCRxFE00_7103_SclkDpmVid4_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmVid[4])
  },
};

FUSE_REGISTER_ENTRY FCRxFE00_7104_TABLE [] = {
  {
    FCRxFE00_7104_SclkDpmDid0_OFFSET,
    FCRxFE00_7104_SclkDpmDid0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmDid[0])
  },
  {
    FCRxFE00_7104_SclkDpmDid1_OFFSET,
    FCRxFE00_7104_SclkDpmDid1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmDid[1])
  },
  {
    FCRxFE00_7104_SclkDpmDid2_OFFSET,
    FCRxFE00_7104_SclkDpmDid2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmDid[2])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_7107_TABLE [] = {
  {
    FCRxFE00_7107_SclkDpmDid3_OFFSET,
    FCRxFE00_7107_SclkDpmDid3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmDid[3])
  },
  {
    FCRxFE00_7107_SclkDpmDid4_OFFSET,
    FCRxFE00_7107_SclkDpmDid4_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmDid[4])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_7109_TABLE [] = {
  {
    FCRxFE00_7109_SclkDpmCacBase_OFFSET,
    FCRxFE00_7109_SclkDpmCacBase_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmCac[4])
  }
};


FUSE_REGISTER_ENTRY FCRxFE00_710D_TABLE [] = {
  {
    FCRxFE00_710D_DispclkDid0_OFFSET,
    FCRxFE00_710D_DispclkDid0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, DisplclkDid[0])
  },
  {
    FCRxFE00_710D_DispclkDid1_OFFSET,
    FCRxFE00_710D_DispclkDid1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, DisplclkDid[1])
  },
  {
    FCRxFE00_710D_DispclkDid2_OFFSET,
    FCRxFE00_710D_DispclkDid2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, DisplclkDid[2])
  },
  {
    FCRxFE00_710D_DispclkDid3_OFFSET,
    FCRxFE00_710D_DispclkDid3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, DisplclkDid[3])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_7110_TABLE [] = {
  {
    FCRxFE00_7110_LclkDpmDid0_OFFSET,
    FCRxFE00_7110_LclkDpmDid0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, LclkDpmDid[0])
  },
  {
    FCRxFE00_7110_LclkDpmDid1_OFFSET,
    FCRxFE00_7110_LclkDpmDid1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, LclkDpmDid[1])
  },
  {
    FCRxFE00_7110_LclkDpmDid2_OFFSET,
    FCRxFE00_7110_LclkDpmDid2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, LclkDpmDid[2])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_7113_TABLE [] = {
  {
    FCRxFE00_7113_LclkDpmDid3_OFFSET,
    FCRxFE00_7113_LclkDpmDid3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, LclkDpmDid[3])
  },
  {
    FCRxFE00_7113_LclkDpmValid0_OFFSET,
    FCRxFE00_7113_LclkDpmValid0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, LclkDpmValid[0])
  },
  {
    FCRxFE00_7113_LclkDpmValid1_OFFSET,
    FCRxFE00_7113_LclkDpmValid1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, LclkDpmValid[1])
  },
  {
    FCRxFE00_7113_LclkDpmValid2_OFFSET,
    FCRxFE00_7113_LclkDpmValid2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, LclkDpmValid[2])
  },
  {
    FCRxFE00_7113_LclkDpmValid3_OFFSET,
    FCRxFE00_7113_LclkDpmValid3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, LclkDpmValid[3])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_7114_TABLE [] = {
  {
    FCRxFE00_7114_DclkDid0_OFFSET,
    FCRxFE00_7114_DclkDid0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, DclkDid[0])
  },
  {
    FCRxFE00_7114_DclkDid1_OFFSET,
    FCRxFE00_7114_DclkDid1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, DclkDid[1])
  },
  {
    FCRxFE00_7114_DclkDid2_OFFSET,
    FCRxFE00_7114_DclkDid2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, DclkDid[2])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_7117_TABLE [] = {
  {
    FCRxFE00_7117_DclkDid3_OFFSET,
    FCRxFE00_7117_DclkDid3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, DclkDid[3])
  },
  {
    FCRxFE00_7117_VclkDid3_OFFSET,
    FCRxFE00_7117_VclkDid3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDid[3])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_7119_TABLE [] = {
  {
    FCRxFE00_7119_SclkDpmValid0_OFFSET,
    FCRxFE00_7119_SclkDpmValid0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmValid[0])
  },
  {
    FCRxFE00_7119_SclkDpmValid1_OFFSET,
    FCRxFE00_7119_SclkDpmValid1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmValid[1])
  },
  {
    FCRxFE00_7119_SclkDpmValid2_OFFSET,
    FCRxFE00_7119_SclkDpmValid2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmValid[2])
  },
  {
    FCRxFE00_7119_SclkDpmValid3_OFFSET,
    FCRxFE00_7119_SclkDpmValid3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmValid[3])
  },
  {
    FCRxFE00_7119_SclkDpmValid4_OFFSET,
    FCRxFE00_7119_SclkDpmValid4_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmValid[4])
  },
  {
    FCRxFE00_7119_SclkDpmValid5_OFFSET,
    FCRxFE00_7119_SclkDpmValid5_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkDpmValid[5])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_711C_TABLE [] = {
  {
    FCRxFE00_711C_PolicyLabel0_OFFSET,
    FCRxFE00_711C_PolicyLabel0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyLabel[0])
  },
  {
    FCRxFE00_711C_PolicyLabel1_OFFSET,
    FCRxFE00_711C_PolicyLabel1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyLabel[1])
  },
  {
    FCRxFE00_711C_PolicyLabel2_OFFSET,
    FCRxFE00_711C_PolicyLabel2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyLabel[2])
  },
  {
    FCRxFE00_711C_PolicyLabel3_OFFSET,
    FCRxFE00_711C_PolicyLabel3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyLabel[3])
  },
  {
    FCRxFE00_711C_PolicyLabel4_OFFSET,
    FCRxFE00_711C_PolicyLabel4_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyLabel[4])
  },
  {
    FCRxFE00_711C_PolicyLabel5_OFFSET,
    FCRxFE00_711C_PolicyLabel5_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyLabel[5])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_711E_TABLE [] = {
  {
    FCRxFE00_711E_PolicyFlags0_OFFSET,
    FCRxFE00_711E_PolicyFlags0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyFlags[0])
  },
  {
    FCRxFE00_711E_PolicyFlags1_OFFSET,
    FCRxFE00_711E_PolicyFlags1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyFlags[1])
  },
  {
    FCRxFE00_711E_PolicyFlags2_OFFSET,
    FCRxFE00_711E_PolicyFlags2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyFlags[2])
  },
  {
    FCRxFE00_711E_PolicyFlags3_OFFSET,
    FCRxFE00_711E_PolicyFlags3_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyFlags[3])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_7121_TABLE [] = {
  {
    FCRxFE00_7121_PolicyFlags4_OFFSET,
    FCRxFE00_7121_PolicyFlags4_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyFlags[4])
  },
  {
    FCRxFE00_7121_PolicyFlags5_OFFSET,
    FCRxFE00_7121_PolicyFlags5_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PolicyFlags[5])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_4036_TABLE [] = {
  {
    FCRxFE00_4036_PPlayTableRev_OFFSET,
    FCRxFE00_4036_PPlayTableRev_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PPlayTableRev)
  },
  {
    FCRxFE00_4036_SclkThermDid_OFFSET,
    FCRxFE00_4036_SclkThermDid_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, SclkThermDid)
  },
  {
    FCRxFE00_4036_PcieGen2Vid_OFFSET,
    FCRxFE00_4036_PcieGen2Vid_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, PcieGen2Vid)
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_4003_TABLE [] = {
  {
    FCRxFE00_4003_VclkDid0_OFFSET,
    FCRxFE00_4003_VclkDid0_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDid[0])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_4008_TABLE [] = {
  {
    FCRxFE00_4008_VclkDid1_OFFSET,
    FCRxFE00_4008_VclkDid1_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDid[1])
  }
};

FUSE_REGISTER_ENTRY FCRxFE00_4028_TABLE [] = {
  {
    FCRxFE00_4028_VclkDid2_OFFSET,
    FCRxFE00_4028_VclkDid2_WIDTH,
    (UINT8) offsetof (PP_FUSE_ARRAY, VclkDid[2])
  }
};

FUSE_TABLE_ENTRY  FuseRegisterTable [] = {
  {
    FCRxFE00_4003_ADDRESS,
    sizeof (FCRxFE00_4003_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_4003_TABLE
  },
  {
    FCRxFE00_4008_ADDRESS,
    sizeof (FCRxFE00_4008_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_4008_TABLE
  },
  {
    FCRxFE00_4028_ADDRESS,
    sizeof (FCRxFE00_4028_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_4028_TABLE
  },
  {
    FCRxFE00_4036_ADDRESS,
    sizeof (FCRxFE00_4036_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_4036_TABLE
  },
  {
    FCRxFE00_600E_ADDRESS,
    sizeof (FCRxFE00_600E_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_600E_TABLE
  },
  {
    FCRxFE00_6022_ADDRESS,
    sizeof (FCRxFE00_6022_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_6022_TABLE
  },
  {
    FCRxFE00_7103_ADDRESS,
    sizeof (FCRxFE00_7103_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_7103_TABLE
  },
  {
    FCRxFE00_7104_ADDRESS,
    sizeof (FCRxFE00_7104_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_7104_TABLE
  },
  {
    FCRxFE00_7107_ADDRESS,
    sizeof (FCRxFE00_7107_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_7107_TABLE
  },
  {
    FCRxFE00_7109_ADDRESS,
    sizeof (FCRxFE00_7109_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_7109_TABLE
  },
  {
    FCRxFE00_710D_ADDRESS,
    sizeof (FCRxFE00_710D_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_710D_TABLE
  },
  {
    FCRxFE00_7110_ADDRESS,
    sizeof (FCRxFE00_7110_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_7110_TABLE
  },
  {
    FCRxFE00_7113_ADDRESS,
    sizeof (FCRxFE00_7113_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_7113_TABLE
  },
  {
    FCRxFE00_7114_ADDRESS,
    sizeof (FCRxFE00_7114_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_7114_TABLE
  },
  {
    FCRxFE00_7117_ADDRESS,
    sizeof (FCRxFE00_7117_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_7117_TABLE
  },
  {
    FCRxFE00_7119_ADDRESS,
    sizeof (FCRxFE00_7119_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_7119_TABLE
  },
  {
    FCRxFE00_711C_ADDRESS,
    sizeof (FCRxFE00_711C_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_711C_TABLE
  },
  {
    FCRxFE00_711E_ADDRESS,
    sizeof (FCRxFE00_711E_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_711E_TABLE
  },
  {
    FCRxFE00_7121_ADDRESS,
    sizeof (FCRxFE00_7121_TABLE) / sizeof (FUSE_REGISTER_ENTRY),
    FCRxFE00_7121_TABLE
  }
};

FUSE_TABLE  FuseTable = {
  sizeof (FuseRegisterTable) / sizeof (FUSE_TABLE_ENTRY),
  FuseRegisterTable
};
