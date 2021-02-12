/* $NoKeywords:$ */
/**
 * @file
 *
 * LCLK DPM initialization
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 39007 $   @e \$Date: 2010-10-05 00:32:54 +0800 (Tue, 05 Oct 2010) $
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
#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbFuseTable.h"
#include  "GnbPcie.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  GNB_MODULE_DEFINITIONS (GnbPcieConfig)
#include  "GnbRegistersON.h"
#include  "OptionGnb.h"
#include  "GfxLib.h"
#include  "NbConfigData.h"
#include  "NbSmuLib.h"
#include  "NbLclkDpm.h"
#include  "NbFamilyServices.h"
#include  "Filecode.h"

#define FILECODE PROC_GNB_NB_FAMILY_0X14_F14NBLCLKDPM_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_BUILD_OPTIONS  GnbBuildOptions;
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

UINT32  LclkDpmCacTable [] = {
  0x0,
  0x0,
  0x0,
  0x0
};

UINT32  LclkDpmActivityThresholdTable [] = {
  0x100,
  0x40FFFF,
  0x40FFFF,
  0x0
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Init NB LCLK DPM in Root Complex Activity mode
 *
 *
 *
 * @param[in] StdHeader        Pointer to Standard configuration
 * @retval                    Initialization status
 */

AGESA_STATUS
NbFmInitLclkDpmRcActivity (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AGESA_STATUS    Status;
  PP_FUSE_ARRAY   *PpFuseArray;
  INT8            Index;
  UINTN           LclkState;
  Status = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "NbFmInitLclkDpmRcActivity F14 Enter\n");
  PpFuseArray = GnbLocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, StdHeader);
  if (PpFuseArray != NULL) {
    UINT32  ActivityThreshold [8];
    UINT16  SamplingPeriod [10];
    UINT8   LclkScalingDid [4];
    UINT8   LclkScalingVid [4];
    UINT32  LclkDpmValid;
    UINT32  MainPllVcoKHz;
    LibAmdMemFill (&ActivityThreshold[0], 0, sizeof (ActivityThreshold), StdHeader);
    LibAmdMemFill (&SamplingPeriod[0], 0, sizeof (SamplingPeriod), StdHeader);
    MainPllVcoKHz = GfxLibGetMainPllFreq (StdHeader) * 100;
    LclkDpmValid = 0;
    LclkState = 7;
    for (Index = 3; Index >= 0; Index--) {
      if (PpFuseArray->LclkDpmValid [Index] != 0) {
        // Set valid DPM state
        LclkDpmValid |= (1 << (LclkState));
        // Set LCLK scaling DID
        LclkScalingDid [7 - LclkState] = PpFuseArray->LclkDpmDid [Index];
        // Set LCLK scaling VID
        LclkScalingVid [7 - LclkState] = PpFuseArray->LclkDpmVid [Index];
        // Set sampling period
        SamplingPeriod [LclkState] = 0xC350;
        //  Changed from 0xC350 to 0x1388 for DPM 0
        if (Index == 0) {
          SamplingPeriod [LclkState] = 0x1388;
        }
        // Set activity threshold from BKDG:
        // Raising -- ActivityThreshold [LclkState]  = ((102 * (GfxLibCalculateClk (LclkScalingDid [7 - LclkState], MainPllVcoKHz) / 100)) - 10) / 10;
        // Lowering -- ActivityThreshold [LclkState] |= (((407 * (GfxLibCalculateClk (LclkScalingDid [7 - LclkState], MainPllVcoKHz) / 100)) + 99) / 10) << 16;
        // For ON specific enable LCLK DPM :
        ActivityThreshold [LclkState]  = LclkDpmActivityThresholdTable [Index];

        IDS_HDT_CONSOLE (GNB_TRACE, "Fused State Index:%d  LCLK DPM State [%d]: LclkScalingDid - 0x%x, ActivityThreshold - 0x%x,  SamplingPeriod - 0x%x\n",
          Index, LclkState, LclkScalingDid [7 - LclkState], ActivityThreshold [LclkState], SamplingPeriod [LclkState]
          );
        LclkState--;
      }
    }
    if (LclkState != 7) {
      SMUx33_STRUCT         SMUx33;
      SMUx0B_x8434_STRUCT   SMUx0B_x8434;
      FCRxFF30_01E4_STRUCT  FCRxFF30_01E4;
      UINT8                 CurrentUnit;
      UINT16                FinalUnit;
      UINT16                FinalPeriod;
      UINT32                Freq;
      UINT32                FreqDelta;
      UINT32                Value;
      ASSERT (LclkScalingDid [0] != 0);
      FreqDelta = 0xffffffff;
      FinalPeriod = 0;
      FinalUnit = 0;
      Freq = (65535 * 100 * 100) / GfxLibCalculateClk (LclkScalingDid [0], MainPllVcoKHz);
      for (CurrentUnit = 0; CurrentUnit < 16; CurrentUnit++) {
        UINT32  CurrentFreqDelta;
        UINT32  CurrentPeriod;
        UINT32  Temp;
        Temp = GnbLibPowerOf (4, CurrentUnit);
        CurrentPeriod = Freq / Temp;
        if (CurrentPeriod <= 0xFFFF) {
          CurrentFreqDelta = Freq - Temp * CurrentPeriod;
          if (FreqDelta > CurrentFreqDelta) {
            FinalUnit = CurrentUnit;
            FinalPeriod = (UINT16) CurrentPeriod;
            FreqDelta = CurrentFreqDelta;
          }
        }
      }
      //Process to enablement LCLK DPM States
      NbSmuIndirectRead (SMUx33_ADDRESS, AccessWidth32, &SMUx33.Value, StdHeader);
      SMUx33.Field.BusyCntSel = 0x3;
      SMUx33.Field.LclkActMonUnt = FinalUnit;
      SMUx33.Field.LclkActMonPrd = FinalPeriod;
      NbSmuIndirectWrite (SMUx33_ADDRESS, AccessS3SaveWidth32, &SMUx33.Value, StdHeader);
      SMUx0B_x8434.Value = 0;
      SMUx0B_x8434.Field.LclkDpmType = 0x1;
      SMUx0B_x8434.Field.LclkDpmEn = 0x1;
      SMUx0B_x8434.Field.LclkTimerPeriod = 0x0C350;
      SMUx0B_x8434.Field.LclkTimerPrescalar = 0x1;
      NbSmuRcuRegisterWrite (
        SMUx0B_x8434_ADDRESS,
        &SMUx0B_x8434.Value,
        1,
        TRUE,
        StdHeader
        );
      NbSmuRcuRegisterWrite (
        0x84AC,
        &LclkDpmCacTable[0],
        ARRAY_SIZE(LclkDpmCacTable),
        TRUE,
        StdHeader
        );
      // Program activity threshold
     IDS_HDT_CONSOLE (GNB_TRACE, "ActivityThreshold[4] - 0x%lx ActivityThreshold[5] - 0x%lx ActivityThreshold[6] - 0x%lx ActivityThreshold[7] - 0x%lx\n",
       ActivityThreshold[4], ActivityThreshold[5], ActivityThreshold[6], ActivityThreshold [7]
       );
      NbSmuRcuRegisterWrite (
        SMUx0B_x8470_ADDRESS,
        &ActivityThreshold[4],
        4,
        TRUE,
        StdHeader
        );
      // Program sampling period
      for (Index = 0; Index < ARRAY_SIZE(SamplingPeriod); Index = Index + 2) {
        UINT16 Temp;
        Temp = SamplingPeriod[Index];
        SamplingPeriod[Index] = SamplingPeriod[Index + 1];
        SamplingPeriod[Index + 1] = Temp;
      }
      IDS_HDT_CONSOLE (GNB_TRACE, "SamplingPeriod[4] - 0x%x SamplingPeriod[5] - 0x%x SamplingPeriod[6] - 0x%x SamplingPeriod[7] - 0x%x  \n",
        SamplingPeriod[4], SamplingPeriod[5], SamplingPeriod[6], SamplingPeriod[7]
        );
      NbSmuRcuRegisterWrite (
        SMUx0B_x8440_ADDRESS,
        (UINT32*) &SamplingPeriod[4],
        2,
        TRUE,
        StdHeader
        );
     // Program LCK scaling DID
      NbSmuRcuRegisterWrite (
        SMUx0B_x848C_ADDRESS,
        (UINT32*) &LclkScalingDid[0],
        1,
        TRUE,
        StdHeader
        );
      // Program LCK scaling VID
      NbSmuRcuRegisterWrite (
        SMUx0B_x8498_ADDRESS,
        (UINT32*) &LclkScalingVid[0],
        1,
        TRUE,
        StdHeader
        );
      // Program valid LCLK DPM states
      LclkDpmValid = NbFmDpmStateBootupInit (LclkDpmValid, StdHeader);
      NbSmuRcuRegisterWrite (
        SMUx0B_x8490_ADDRESS,
        &LclkDpmValid,
        1,
        TRUE,
        StdHeader
        );
      //Setup Activity Monitor  Coefficients
      Value = (0x24 << SMUx35_DownTrendCoef_OFFSET) | (0x24 << SMUx35_UpTrendCoef_OFFSET);
      NbSmuIndirectWrite (SMUx35_ADDRESS, AccessS3SaveWidth32, &Value, StdHeader);
      Value = (0x22 << SMUx35_DownTrendCoef_OFFSET) | (0x22 << SMUx35_UpTrendCoef_OFFSET);
      for (Index = SMUx37_ADDRESS; Index <= SMUx51_ADDRESS; Index = Index + 2) {
        NbSmuIndirectWrite (Index, AccessS3SaveWidth32, &Value, StdHeader);
      }
      // Enable LCLK DPM as voltage client
      NbSmuSrbmRegisterRead (FCRxFF30_01E4_ADDRESS, &FCRxFF30_01E4.Value, StdHeader);
      FCRxFF30_01E4.Field.VoltageChangeEn = 0x1;
      NbSmuSrbmRegisterWrite (FCRxFF30_01E4_ADDRESS, &FCRxFF30_01E4.Value, TRUE, StdHeader);
      // Start LCLK service
      NbSmuServiceRequest (0x8, TRUE, StdHeader);
    }
  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "  ERROR! Cannot locate fuse table\n");
    Status = AGESA_ERROR;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "NbFmInitLclkDpmRcActivity F14 Exit [0x%x]\n", Status);
  return Status;
}



/*----------------------------------------------------------------------------------------*/
/**
 * Family specific check PsppPolicy to initially enable appropriate DPM states
 *
 *
 * @param[in] LclkDpmValid            UINT32 Lclk Dpm Valid
 * @param[in] StdHeader               Pointer to AMD_CONFIG_PARAMS
 */
UINT32
NbFmDpmStateBootupInit (
  IN       UINT32              LclkDpmValid,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  PCIe_PLATFORM_CONFIG    *Pcie;
  UINT32                  LclkDpmValidState;
  UINT8                   Dpm0ValidOffset;

  if ((LclkDpmValid & 0xFF) == 0) {
    IDS_HDT_CONSOLE (NB_MISC, "  No valid DPM State Bootup Init\n");
    return 0;
  }

  // For ON, from DPM0(the most right non-zero bit) to highest DPM(bit 7)
  Dpm0ValidOffset = LibAmdBitScanForward (LclkDpmValid & 0xFF);
  // Enable DPM0
  LclkDpmValidState = 1 << Dpm0ValidOffset;

  if (PcieLocateConfigurationData (StdHeader, &Pcie) == AGESA_SUCCESS) {
    switch (Pcie->PsppPolicy) {
    case PsppDisabled:
    case PsppPerformance:
    case PsppBalanceHigh:
      if ((Dpm0ValidOffset + 2) <= 7) {
        // Enable DPM0 + DPM2
        LclkDpmValidState = LclkDpmValidState + (1 << (Dpm0ValidOffset + 2));
      }
      break;
    case PsppBalanceLow:
      if ((Dpm0ValidOffset + 1) <= 7) {
        // Enable DPM0 + DPM1
        LclkDpmValidState = LclkDpmValidState + (1 << (Dpm0ValidOffset + 1));
      }
      break;
    case PsppPowerSaving:
      // Enable DPM0
      break;
    default:
      ASSERT (FALSE);
    }
  } else {
    IDS_HDT_CONSOLE (NB_MISC, "  DPM State Bootup Init Pcie Locate ConfigurationData Fail!! -- Enable DPM0 only\n");
  }
  return LclkDpmValidState;
}

