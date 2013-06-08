/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe late post initialization.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 64730 $   @e \$Date: 2012-01-30 02:05:39 -0600 (Mon, 30 Jan 2012) $
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
#include  "amdlib.h"
#include  "Ids.h"
#include  "heapManager.h"
#include  "GeneralServices.h"
#include  "Gnb.h"
#include  "GnbFuseTable.h"
#include  "GnbPcie.h"
#include  "GnbGfx.h"
#include  "GnbSbLib.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbGfxConfig.h"
#include  "GnbGfxInitLibV1.h"
#include  "GnbGfxFamServices.h"
#include  "GnbRegistersTN.h"
#include  "GnbRegisterAccTN.h"
#include  "GnbNbInitLibV1.h"
#include  "GfxConfigLib.h"
#include  "GfxLibTN.h"
#include  "OptionGnb.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_GFXINTEGRATEDINFOTABLETN_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


#define GFX_REFCLK                        100     // (in MHz) Reference clock is 100 MHz
#define GFX_NCLK_MIN                      700     // (in MHz) Minimum value for NCLK is 700 MHz
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
extern GNB_BUILD_OPTIONS      GnbBuildOptions;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
GfxIntInfoTableInterfaceTN (
  IN      AMD_CONFIG_PARAMS       *StdHeader
  );

CONST UINT8  DdiLaneConfigArrayTN [][4] = {
  {31, 24, 1, 0},
  {24, 31, 0, 1},
  {24, 27, 0, 0},
  {27, 24, 0, 0},
  {28, 31, 1, 1},
  {31, 28, 1, 1},
  {32, 38, 2, 2},
  {32, 35, 2, 2},
  {35, 32, 2, 2},
  {8 , 15, 3, 3},
  {15, 8 , 3, 3},
  {12, 15, 3, 3},
  {15, 12, 3, 3},
  {16, 19, 4, 4},
  {19, 16, 4, 4},
  {16, 23, 4, 5},
  {23, 16, 5, 4},
  {20, 23, 5, 5},
  {23, 20, 5, 5},
};


/*----------------------------------------------------------------------------------------*/
/**
 * Init TN Support for eDP to Lvds translators
 *
 *
 * @param[in]     Engine  Engine configuration info
 * @param[in,out] Buffer  Buffer pointer
 * @param[in]     Pcie    PCIe configuration info
 */
VOID
STATIC
GfxIntegrateducEDPToLVDSRxIdCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8    *uceDPToLVDSRxId;
  uceDPToLVDSRxId = (UINT8*) Buffer;
  // APU output DP signal to a 3rd party DP translator chip (Analogix, Parade etc),
  // the chip is handled by the 3rd party DP Rx firmware and it does not require the AMD SW to have a special
  // initialize/enable/disable sequence to control this chip, the AMD SW just follows the eDP spec
  // to enable the LVDS panel through this chip.

  if (Engine->Type.Ddi.DdiData.ConnectorType == ConnectorTypeEDPToLvds) {
    *uceDPToLVDSRxId = eDP_TO_LVDS_COMMON_ID;
    IDS_HDT_CONSOLE (GNB_TRACE, "Found 3rd party common EDPToLvds Connector\n");
  }
  // APU output DP signal to a 3rd party DP translator chip which requires a AMD SW one time initialization
  // to the chip based on the LVDS panel parameters ( such as power sequence time and panel SS parameter etc ).
  // After that, the AMD SW does not need any specific enable/disable sequences to control this chip and just
  // follows the eDP spec. to control the panel.
  if (Engine->Type.Ddi.DdiData.ConnectorType == ConnectorTypeEDPToRealtecLvds) {
    *uceDPToLVDSRxId = eDP_TO_LVDS_REALTEK_ID;
    IDS_HDT_CONSOLE (GNB_TRACE, "Found Realtec EDPToLvds Connector\n");
  }

}

/*----------------------------------------------------------------------------------------*/
/**
 *Init TN Nb p-State MemclkFreq
 *
 *
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 */

STATIC VOID
GfxFillNbPstateMemclkFreqTN (
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V1_7 *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  D18F2x94_dct0_STRUCT            D18F2x94;
  D18F2x2E0_dct0_STRUCT           D18F2x2E0;
  D18F5x160_STRUCT                NbPstate;
  UINT8                           i;
  UINT8                           Channel;
  ULONG                           memps0_freq;
  ULONG                           memps1_freq;

  if ((Gfx->UmaInfo.UmaAttributes & UMA_ATTRIBUTE_ON_DCT0) != 0) {
    Channel = 0;
  } else {
    Channel = 1;
  }

  GnbRegisterReadTN (
    ((Channel == 0) ? D18F2x94_dct0_TYPE : D18F2x94_dct1_TYPE),
    ((Channel == 0) ? D18F2x94_dct0_ADDRESS : D18F2x94_dct1_ADDRESS),
    &D18F2x94.Value,
    0,
    GnbLibGetHeader (Gfx)
    );

  GnbRegisterReadTN (
    ((Channel == 0) ? D18F2x2E0_dct0_TYPE : D18F2x2E0_dct1_TYPE),
    ((Channel == 0) ? D18F2x2E0_dct0_ADDRESS : D18F2x2E0_dct1_ADDRESS),
    &D18F2x2E0.Value,
    0,
    GnbLibGetHeader (Gfx)
    );

  memps0_freq = 100 * GfxLibExtractDramFrequency ((UINT8) D18F2x94.Field.MemClkFreq, GnbLibGetHeader (Gfx));
  memps1_freq = 100 * GfxLibExtractDramFrequency ((UINT8) D18F2x2E0.Field.M1MemClkFreq, GnbLibGetHeader (Gfx));

  for (i = 0; i < 4; i++) {
    NbPstate.Value = 0;
    GnbRegisterReadTN (
      TYPE_D18F5,
      (D18F5x160_ADDRESS + (i * 4)),
      &NbPstate.Value,
      0,
      GnbLibGetHeader (Gfx)
      );
    if (NbPstate.Field.NbPstateEn == 1) {
      IntegratedInfoTable->ulNbpStateMemclkFreq[i] = (NbPstate.Field.MemPstate == 0) ? memps0_freq : memps1_freq;
    }
  }

}

/*----------------------------------------------------------------------------------------*/
/**
 *Init TN HTC Data
 *
 *
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 */

STATIC VOID
GfxFillHtcDataTN (
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V1_7 *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  D18F3x64_STRUCT  D18F3x64;

  GnbRegisterReadTN (
    D18F3x64_TYPE,
    D18F3x64_ADDRESS,
    &D18F3x64.Value,
    0,
    GnbLibGetHeader (Gfx)
    );

  if (D18F3x64.Field.HtcEn == 1) {
    IntegratedInfoTable->ucHtcTmpLmt = (UCHAR) (D18F3x64.Field.HtcTmpLmt / 2 + 52);
    IntegratedInfoTable->ucHtcHystLmt = (UCHAR) (D18F3x64.Field.HtcHystLmt / 2);
  } else {
    IntegratedInfoTable->ucHtcTmpLmt = 0;
    IntegratedInfoTable->ucHtcHystLmt = 0;
  }
}
/*----------------------------------------------------------------------------------------*/
/**
 * Get TN CSR phy self refresh power down mode.
 *
 *
 * @param[in] Channel    DCT controller index
 * @param[in] StdHeader  Standard configuration header
 * @retval    CsrPhySrPllPdMode
 */
STATIC UINT32
GfxLibGetMemPhyPllPdModeTN (
  IN       UINT8       Channel,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  D18F2xA8_dct0_STRUCT     D18F2xA8;

  GnbRegisterReadTN (
    ((Channel == 0) ? D18F2xA8_dct0_TYPE : D18F2xA8_dct1_TYPE),
    ((Channel == 0) ? D18F2xA8_dct0_ADDRESS : D18F2xA8_dct1_ADDRESS),
    &D18F2xA8.Value,
    0,
    StdHeader
  );

  IDS_HDT_CONSOLE (GNB_TRACE, "MemPhyPllPdMode : %x\n", D18F2xA8.Field.MemPhyPllPdMode);
  return  D18F2xA8.Field.MemPhyPllPdMode;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get TN disable DLL shutdown in self-refresh mode.
 *
 *
 * @param[in] Channel    DCT controller index
 * @param[in] StdHeader  Standard configuration header
 * @retval    DisDllShutdownSR
 */
STATIC UINT32
GfxLibGetDisDllShutdownSRTN (
  IN       UINT8       Channel,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  D18F2x90_dct0_STRUCT D18F2x90;

  GnbRegisterReadTN (
    ((Channel == 0) ? D18F2x90_dct0_TYPE : D18F2x90_dct1_TYPE),
    ((Channel == 0) ? D18F2x90_dct0_ADDRESS : D18F2x90_dct1_ADDRESS),
    &D18F2x90.Value,
    0,
    StdHeader
  );

  IDS_HDT_CONSOLE (GNB_TRACE, "DisDllShutdownSR : %x\n", D18F2x90.Field.DisDllShutdownSR);
  return  D18F2x90.Field.DisDllShutdownSR;
}

/*----------------------------------------------------------------------------------------*/
/**
 *Init TN NbPstateVid
 *
 *
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 */

STATIC VOID
GfxFillNbPStateVidTN (
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V1_7 *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  //TN Register Mapping for D18F5x1[6C:60]
  D18F5x160_STRUCT  NbPstate[4];
  D0F0xBC_x1F428_STRUCT  D0F0xBC_x1F428;
  UINT8             MinNclkIndex;
  UINT8             i;

  MinNclkIndex = 0;
  IntegratedInfoTable->ucNBDPMEnable = 0;


  GnbRegisterReadTN (
    D0F0xBC_x1F428_TYPE,
    D0F0xBC_x1F428_ADDRESS,
    &D0F0xBC_x1F428.Value,
    0,
    GnbLibGetHeader (Gfx)
  );
  // Check if NbPstate enbale
  if (D0F0xBC_x1F428.Field.EnableNbDpm == 1) {
    //1: enable 0: not enable
    IntegratedInfoTable->ucNBDPMEnable = 1;
  }

  for (i = 0; i < 4; i++) {
    GnbRegisterReadTN (
      TYPE_D18F5,
      (D18F5x160_ADDRESS + (i * 4)),
      &NbPstate[i].Value,
      0,
      GnbLibGetHeader (Gfx)
    );
    if (NbPstate[i].Field.NbPstateEn == 1) {
      MinNclkIndex = i;
    }
    IntegratedInfoTable->ulNbpStateNClkFreq[i] = GfxLibGetNclkTN ((UINT8) NbPstate[i].Field.NbFid, (UINT8) NbPstate[i].Field.NbDid);
  }
  IntegratedInfoTable->usNBP0Voltage = (USHORT) ((NbPstate[0].Field.NbVid_7_ << 7) | NbPstate[0].Field.NbVid_6_0_);
  IntegratedInfoTable->usNBP1Voltage = (USHORT) ((NbPstate[1].Field.NbVid_7_ << 7) | NbPstate[1].Field.NbVid_6_0_);
  IntegratedInfoTable->usNBP2Voltage = (USHORT) ((NbPstate[2].Field.NbVid_7_ << 7) | NbPstate[2].Field.NbVid_6_0_);
  IntegratedInfoTable->usNBP3Voltage = (USHORT) ((NbPstate[3].Field.NbVid_7_ << 7) | NbPstate[3].Field.NbVid_6_0_);

  IntegratedInfoTable->ulMinimumNClk = GfxLibGetNclkTN ((UINT8) NbPstate[MinNclkIndex].Field.NbFid, (UINT8) NbPstate[MinNclkIndex].Field.NbDid);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize display path for given engine
 *
 *
 *
 * @param[in]   Engine            Engine configuration info
 * @param[out]  DisplayPathList   Display path list
 * @param[in]   Gfx               Pointer to global GFX configuration
 */

AGESA_STATUS
GfxFmMapEngineToDisplayPath (
  IN       PCIe_ENGINE_CONFIG          *Engine,
     OUT   EXT_DISPLAY_PATH            *DisplayPathList,
  IN       GFX_PLATFORM_CONFIG         *Gfx
  )
{
  AGESA_STATUS      Status;
  UINT8             PrimaryDisplayPathId;
  UINT8             SecondaryDisplayPathId;
  UINTN             DisplayPathIndex;
  PrimaryDisplayPathId = 0xff;
  SecondaryDisplayPathId = 0xff;
  for (DisplayPathIndex = 0; DisplayPathIndex <  (sizeof (DdiLaneConfigArrayTN) / 4); DisplayPathIndex++) {
    if (DdiLaneConfigArrayTN[DisplayPathIndex][0] == Engine->EngineData.StartLane &&
        DdiLaneConfigArrayTN[DisplayPathIndex][1] == Engine->EngineData.EndLane) {
      PrimaryDisplayPathId = DdiLaneConfigArrayTN[DisplayPathIndex][2];
      SecondaryDisplayPathId = DdiLaneConfigArrayTN[DisplayPathIndex][3];
      break;
    }
  }
  if (PrimaryDisplayPathId != 0xff) {
    IDS_HDT_CONSOLE (GFX_MISC, "  Allocate Display Connector at Primary sPath[%d]\n", PrimaryDisplayPathId);
    Engine->InitStatus |= INIT_STATUS_DDI_ACTIVE;
    GfxIntegratedCopyDisplayInfo (
      Engine,
      &DisplayPathList[PrimaryDisplayPathId],
      (PrimaryDisplayPathId != SecondaryDisplayPathId) ? &DisplayPathList[SecondaryDisplayPathId] : NULL,
      Gfx
      );
    Status = AGESA_SUCCESS;
  } else {
    IDS_HDT_CONSOLE (GFX_MISC, "  Error!!! Map DDI lanes %d - %d to display path failed\n",
      Engine->EngineData.StartLane,
      Engine->EngineData.EndLane
      );
    PutEventLog (
      AGESA_ERROR,
      GNB_EVENT_INVALID_DDI_LINK_CONFIGURATION,
      Engine->EngineData.StartLane,
      Engine->EngineData.EndLane,
      0,
      0,
      GnbLibGetHeader (Gfx)
      );
    Status = AGESA_ERROR;
  }
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Copy memory content to FB
 *
 *
 * @param[in] SystemInfoTableV2Ptr    Pointer to integrated info table
 * @param[in] Gfx                     Pointer to global GFX configuration
 *
 */
STATIC VOID
GfxIntInfoTabablePostToFb (
  IN       ATOM_FUSION_SYSTEM_INFO_V2     *SystemInfoTableV2Ptr,
  IN       GFX_PLATFORM_CONFIG            *Gfx
  )
{
  UINT32      Index;
  UINT32      TableOffset;
  UINT32      FbAddress;

  TableOffset = (UINT32) (Gfx->UmaInfo.UmaSize - sizeof (ATOM_FUSION_SYSTEM_INFO_V2)) | 0x80000000;
  for (Index = 0; Index < sizeof (ATOM_FUSION_SYSTEM_INFO_V2); Index = Index + 4 ) {
    FbAddress = TableOffset + Index;
    GnbLibMemWrite (Gfx->GmmBase + GMMx00_ADDRESS, AccessWidth32, &FbAddress, GnbLibGetHeader (Gfx));
    GnbLibMemWrite (Gfx->GmmBase + GMMx04_ADDRESS, AccessWidth32, (UINT8*) SystemInfoTableV2Ptr + Index, GnbLibGetHeader (Gfx));
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 *Init Dispclk <-> VID table
 *
 *
 * @param[in] PpFuseArray         Fuse array pointer
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 */

STATIC VOID
GfxIntInfoTableInitDispclkTable (
  IN       PP_FUSE_ARRAY                    *PpFuseArray,
  IN       ATOM_INTEGRATED_SYSTEM_INFO_V1_7 *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  UINTN   Index;
  for (Index = 0; Index < 4; Index++) {
    if (PpFuseArray->DisplclkDid[Index] != 0) {
      IntegratedInfoTable->sDISPCLK_Voltage[Index].ulMaximumSupportedCLK = GfxFmCalculateClock (
                                                                             PpFuseArray->DisplclkDid[Index],
                                                                             GnbLibGetHeader (Gfx)
                                                                             );
      IntegratedInfoTable->sDISPCLK_Voltage[Index].ulVoltageIndex = (ULONG) Index;
    }
  }
  IntegratedInfoTable->ucDPMState0VclkFid = PpFuseArray->VclkDid[0];
  IntegratedInfoTable->ucDPMState1VclkFid = PpFuseArray->VclkDid[1];
  IntegratedInfoTable->ucDPMState2VclkFid = PpFuseArray->VclkDid[2];
  IntegratedInfoTable->ucDPMState3VclkFid = PpFuseArray->VclkDid[3];
  IntegratedInfoTable->ucDPMState0DclkFid = PpFuseArray->DclkDid[0];
  IntegratedInfoTable->ucDPMState1DclkFid = PpFuseArray->DclkDid[1];
  IntegratedInfoTable->ucDPMState2DclkFid = PpFuseArray->DclkDid[2];
  IntegratedInfoTable->ucDPMState3DclkFid = PpFuseArray->DclkDid[3];
}

/*----------------------------------------------------------------------------------------*/
/**
 *Init Sclk <-> VID table
 *
 *
 * @param[in] PpFuseArray         Fuse array pointer
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 */

STATIC VOID
GfxIntInfoTableInitSclkTable (
  IN       PP_FUSE_ARRAY                    *PpFuseArray,
  IN       ATOM_INTEGRATED_SYSTEM_INFO_V1_7 *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  UINTN                       Index;
  UINTN                       AvailSclkIndex;
  ATOM_AVAILABLE_SCLK_LIST    *AvailSclkList;
  BOOLEAN                     Sorting;
  AvailSclkList = &IntegratedInfoTable->sAvail_SCLK[0];

  AvailSclkIndex = 0;
  for (Index = 0; Index < MAX_NUM_OF_FUSED_DPM_STATES; Index++) {
    if (PpFuseArray->SclkDpmDid[Index] != 0) {
      AvailSclkList[AvailSclkIndex].ulSupportedSCLK = GfxFmCalculateClock (PpFuseArray->SclkDpmDid[Index], GnbLibGetHeader (Gfx));
      AvailSclkList[AvailSclkIndex].usVoltageIndex = PpFuseArray->SclkDpmVid[Index];
      AvailSclkList[AvailSclkIndex].usVoltageID = PpFuseArray->SclkVid[PpFuseArray->SclkDpmVid[Index]];
      AvailSclkIndex++;
    }
  }
  //Sort by VoltageIndex & ulSupportedSCLK
  if (AvailSclkIndex > 1) {
    do {
      Sorting = FALSE;
      for (Index = 0; Index < (AvailSclkIndex - 1); Index++) {
        ATOM_AVAILABLE_SCLK_LIST  Temp;
        BOOLEAN                   Exchange;
        Exchange = FALSE;
        if (AvailSclkList[Index].usVoltageIndex > AvailSclkList[Index + 1].usVoltageIndex) {
          Exchange = TRUE;
        }
        if ((AvailSclkList[Index].usVoltageIndex == AvailSclkList[Index + 1].usVoltageIndex) &&
            (AvailSclkList[Index].ulSupportedSCLK > AvailSclkList[Index + 1].ulSupportedSCLK)) {
          Exchange = TRUE;
        }
        if (Exchange) {
          Sorting = TRUE;
          LibAmdMemCopy (&Temp, &AvailSclkList[Index], sizeof (ATOM_AVAILABLE_SCLK_LIST), GnbLibGetHeader (Gfx));
          LibAmdMemCopy (&AvailSclkList[Index], &AvailSclkList[Index + 1], sizeof (ATOM_AVAILABLE_SCLK_LIST), GnbLibGetHeader (Gfx));
          LibAmdMemCopy (&AvailSclkList[Index + 1], &Temp, sizeof (ATOM_AVAILABLE_SCLK_LIST), GnbLibGetHeader (Gfx));
        }
      }
    } while (Sorting);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 *Read GFX PGFSM register
 *
 *
 * @param[in]  RegisterAddress     Index of PGFSM register
 * @param[out] Value               Pointer to value
 * @param[in]  StdHeader           Standard configuration header
 */

STATIC VOID
GfxPgfsmRegisterReadTN (
  IN       UINT32                 RegisterAddress,
     OUT   UINT32                 *Value,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32      RegisterWriteValue;
  UINT32      RegisterReadValue;
  RegisterWriteValue = (RegisterAddress << D0F0xBC_xE0300000_RegAddr_OFFSET) +
                       (1 << D0F0xBC_xE0300000_ReadOp_OFFSET) +
                       (0 << D0F0xBC_xE0300000_FsmAddr_OFFSET);
  IDS_HDT_CONSOLE (GNB_TRACE, "Read PGFSM Register %d\n", RegisterAddress);
  GnbRegisterWriteTN (
    D0F0xBC_xE0300000_TYPE,
    D0F0xBC_xE0300000_ADDRESS,
    &RegisterWriteValue,
    0,
    StdHeader);
  do {
    GnbRegisterReadTN (
      D0F0xBC_xE0300008_TYPE,
      D0F0xBC_xE0300008_ADDRESS,
      &RegisterReadValue,
      0,
      StdHeader);
  } while ((RegisterReadValue & D0F0xBC_xE0300008_ReadValid_MASK) == 0);
  *Value = RegisterReadValue & D0F0xBC_xE0300008_ReadValue_MASK;
}

/*----------------------------------------------------------------------------------------*/
/**
 *Calculate ulGMCRestoreResetTime
 *
 *
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 * @param[in] PpFuseArray         Fuse array pointer
 * @retval    AGESA_STATUS
 */

STATIC AGESA_STATUS
GfxCalculateRestoreResetTimeTN (
  IN       ATOM_INTEGRATED_SYSTEM_INFO_V1_7 *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx,
  IN       PP_FUSE_ARRAY                    *PpFuseArray
  )
{
  UINT8      MaxDid;
  ULONG      FreqSclk;
  UINTN      Index;
  UINT32     TSclk;
  UINT32     TRefClk;
  UINT32     TNclkHalf;
  UINT32     PgfsmDelayReg0;
  UINT32     PgfsmDelayReg1;
  UINT32     ResetTime;
  UINT32     IsoTime;
  UINT32     MemSd;
  UINT32     MotherPso;
  UINT32     DaughterPso;
  UINT32     THandshake;
  UINT32     TGmcSync;
  UINT32     TPuCmd;
  UINT32     TPgfsmCmdSerialization;
  UINT32     TReset;
  UINT32     TMoPso;
  UINT32     TDaPso;
  UINT32     TMemSd;
  UINT32     TIso;
  UINT32     TRegRestore;
  UINT32     TPgfsmCleanUp;
  UINT32     TGmcPu;
  UINT32     TGmcPd;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxCalculateRestoreResetTimeTN Enter\n");
  // Find FreqSclk = MIN of frequencies SclkDpmDid (0 to 4) and SclkThermDid
  // First find the highest Did
  MaxDid = PpFuseArray->SclkThermDid;
  for (Index = 0; Index < 4; Index++) {
    // Compare with SclkDpmDid[x] - These are stored in:
    // IntegratedInfoTable-> sDISPCLK_Voltage[Index].ulMaximumSupportedCLK
    MaxDid = MAX (MaxDid, PpFuseArray->SclkDpmDid[Index]);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "MaxDid = %d\n", MaxDid);
  FreqSclk = GfxFmCalculateClock (MaxDid, GnbLibGetHeader (Gfx));
  // FreqSclk is in 10KHz units - need calculations in nS
  // For accuracy, do calculations in .01nS, then convert at the end
  TSclk = (100 * (1000000000 / 10000)) / FreqSclk;
  // FreqRefClk frequency of reference clock
  // Caclculate period in .01 nS
  TRefClk = (100 * 1000) / GFX_REFCLK;
  // FreqNclkHalf = half of Minimum NCLK value
  // Calculate period in .01 nS
  TNclkHalf = (100 * 1000) / (GFX_NCLK_MIN / 2);

  // Read delay time values from PGFSM registers
  GfxPgfsmRegisterReadTN (2 , &PgfsmDelayReg0, GnbLibGetHeader (Gfx));
  GfxPgfsmRegisterReadTN (3 , &PgfsmDelayReg1, GnbLibGetHeader (Gfx));
  ResetTime = (PgfsmDelayReg0 & 0x000000FF ) >> 0 ;
  IsoTime = (PgfsmDelayReg0 & 0x0000FF00 ) >> 8 ;
  MemSd = (PgfsmDelayReg0 & 0x00FF0000 ) >> 16 ;
  MotherPso = (PgfsmDelayReg1 & 0x00000FFF ) >> 0 ;
  DaughterPso = (PgfsmDelayReg1 & 0x00FFF000 ) >> 12 ;
  IDS_HDT_CONSOLE (GNB_TRACE, "ResetTime = %d\n", ResetTime);
  IDS_HDT_CONSOLE (GNB_TRACE, "IsoTime = %d\n", IsoTime);
  IDS_HDT_CONSOLE (GNB_TRACE, "MemSd = %d\n", MemSd);
  IDS_HDT_CONSOLE (GNB_TRACE, "MotherPso = %d\n", MotherPso);
  IDS_HDT_CONSOLE (GNB_TRACE, "DaughterPso = %d\n", DaughterPso);

  // Calculate various timing values required for the final calculation
  // THandshake = 10*1/FreqNclkHalf
  THandshake = 10 * TNclkHalf;
  // TGmcSync = 2.5*(1/FreqRefclk+1/FreqSclk)
  TGmcSync = (25 * (TRefClk + TSclk)) / 10;
  // TPuCmd =9*1/FreqSclk
  TPuCmd = 9 * TSclk;
  // TPgfsmCmdSerialization = 82*1/FreqSclk
  TPgfsmCmdSerialization = 82 * TSclk;
  // TReset = (RESET_TIME+3)*1/FreqRefclk+3*1/FreqSclk+TGmcSync
  TReset = ((ResetTime + 3) * TRefClk) + (3 * TSclk) + TGmcSync;
  // TMoPso = (MOTHER_PSO+3)*1/FreqRefclk+3*1/FreqSclk+TgmcSync
  TMoPso = ((MotherPso + 3) * TRefClk) + (3 * TSclk) + TGmcSync;
  // TDaPso = (DAUGHTER_PSO+3)*1/FreqRefclk+3*1/FreqSclk+TgmcSync
  TDaPso = ((DaughterPso + 3) * TRefClk) + (3 * TSclk) + TGmcSync;
  // TMemSD = (MEM_SD+3)*1/FreqRefclk+3*1/FreqSclk+TgmcSync
  TMemSd = ((MemSd + 3) * TRefClk) + (3 * TSclk) + TGmcSync;
  // TIso = (ISO_TIME+3)*1/FreqRefclk+3*1/FreqSclk+TgmcSync
  TIso = ((IsoTime + 3) * TRefClk) + (3 * TSclk) + TGmcSync;
  // TRegRestore = 508*1/FreqSclk
  TRegRestore = 508 * TSclk;
  // TPgfsmCleanUp = 3*1/FreqSclk
  TPgfsmCleanUp = 3 * TSclk;
  // TGmcPu = TPUCmd + TPgfsmCmdSerialization + TReset + TMoPso + TDaPso + TMemSD + TIso + TRegRestore
  TGmcPu = TPuCmd + TPgfsmCmdSerialization + TReset + TMoPso + TDaPso + TMemSd + TIso + TRegRestore;
  // TGmcPd = THandshake + TPgfsmCmdSerialization + Tiso + TmemSD + TMoPso + TDaPso + TpgfsmCleanUp + 3*TReset
  TGmcPd = THandshake + TPgfsmCmdSerialization + TIso + TMemSd + TMoPso + TDaPso + TPgfsmCleanUp + (3 * TReset);
  // ulGMCRestoreResetTime = TGmcPu + TGmcPd
  // All calculated times are in .01nS for accuracy.  We can now correct that.
  // By adding 99 and dividing by 100, value is rounded up to next 1 nS
  IntegratedInfoTable->ulGMCRestoreResetTime = (TGmcPd + TGmcPu + 99) / 100;
  IDS_HDT_CONSOLE (GNB_TRACE, "ulGMCRestoreResetTime = %d\n", IntegratedInfoTable->ulGMCRestoreResetTime);
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxCalculateRestoreResetTimeTN Exit\n");
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Build integrated info table
 *
 *
 *
 * @param[in] Gfx             Gfx configuration info
 * @retval    AGESA_STATUS
 */
AGESA_STATUS
STATIC
GfxIntInfoTableInitTN (
  IN      GFX_PLATFORM_CONFIG     *Gfx
  )
{
  AGESA_STATUS                    Status;
  AGESA_STATUS                    AgesaStatus;
  ATOM_FUSION_SYSTEM_INFO_V2      SystemInfoTableV2;
  PP_FUSE_ARRAY                   *PpFuseArray;
  PCIe_PLATFORM_CONFIG            *Pcie;
  ATOM_PPLIB_POWERPLAYTABLE3      *PpTable;
  UINT8                           Channel;

  AgesaStatus = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntegratedInfoTableInitTN Enter\n");
  LibAmdMemFill (&SystemInfoTableV2, 0x00, sizeof (ATOM_FUSION_SYSTEM_INFO_V2), GnbLibGetHeader (Gfx));
  SystemInfoTableV2.sIntegratedSysInfo.sHeader.usStructureSize = sizeof (ATOM_INTEGRATED_SYSTEM_INFO_V1_7);
  ASSERT (SystemInfoTableV2.sIntegratedSysInfo.sHeader.usStructureSize == 512);
  SystemInfoTableV2.sIntegratedSysInfo.sHeader.ucTableFormatRevision = 1;
  SystemInfoTableV2.sIntegratedSysInfo.sHeader.ucTableContentRevision = 7;
  SystemInfoTableV2.sIntegratedSysInfo.ulDentistVCOFreq = GfxLibGetSytemPllCofTN (GnbLibGetHeader (Gfx)) * 100;
  SystemInfoTableV2.sIntegratedSysInfo.ulBootUpUMAClock = Gfx->UmaInfo.MemClock * 100;
  SystemInfoTableV2.sIntegratedSysInfo.usRequestedPWMFreqInHz = Gfx->LcdBackLightControl;
  SystemInfoTableV2.sIntegratedSysInfo.ucUMAChannelNumber = ((Gfx->UmaInfo.UmaAttributes & UMA_ATTRIBUTE_INTERLEAVE) == 0) ? 1 : 2;
  SystemInfoTableV2.sIntegratedSysInfo.ucMemoryType = 3;     //DDR3
  SystemInfoTableV2.sIntegratedSysInfo.ulBootUpEngineClock = 200 * 100;    //Set default engine clock to 200MhZ
  SystemInfoTableV2.sIntegratedSysInfo.usBootUpNBVoltage = GnbLocateHighestVidIndex (GnbLibGetHeader (Gfx));
  SystemInfoTableV2.sIntegratedSysInfo.ulMinEngineClock = 200 * 100;
  SystemInfoTableV2.sIntegratedSysInfo.usPanelRefreshRateRange = Gfx->DynamicRefreshRate;
  SystemInfoTableV2.sIntegratedSysInfo.usLvdsSSPercentage = Gfx->LvdsSpreadSpectrum;
  //Locate PCIe configuration data to get definitions of display connectors
  SystemInfoTableV2.sIntegratedSysInfo.sExtDispConnInfo.sHeader.usStructureSize = sizeof (ATOM_EXTERNAL_DISPLAY_CONNECTION_INFO);
  SystemInfoTableV2.sIntegratedSysInfo.sExtDispConnInfo.sHeader.ucTableFormatRevision = 1;
  SystemInfoTableV2.sIntegratedSysInfo.sExtDispConnInfo.sHeader.ucTableContentRevision = 1;
  SystemInfoTableV2.sIntegratedSysInfo.sExtDispConnInfo.uc3DStereoPinId = Gfx->Gnb3dStereoPinIndex;
  SystemInfoTableV2.sIntegratedSysInfo.sExtDispConnInfo.ucRemoteDisplayConfig = Gfx->GnbRemoteDisplaySupport;
  SystemInfoTableV2.sIntegratedSysInfo.usExtDispConnInfoOffset = offsetof (ATOM_INTEGRATED_SYSTEM_INFO_V1_7, sExtDispConnInfo);
  SystemInfoTableV2.sIntegratedSysInfo.ulSB_MMIO_Base_Addr = SbGetSbMmioBaseAddress (GnbLibGetHeader (Gfx));

  SystemInfoTableV2.sIntegratedSysInfo.usPCIEClkSSPercentage = Gfx->PcieRefClkSpreadSpectrum;

  SystemInfoTableV2.sIntegratedSysInfo.ucLvdsMisc = Gfx->LvdsMiscControl.Value;
  IDS_HDT_CONSOLE (GNB_TRACE, "Lvds Misc control : %x\n", Gfx->LvdsMiscControl.Value);
  if (Gfx->LvdsMiscControl.Field.TravisLvdsVoltOverwriteEn) {
    SystemInfoTableV2.sIntegratedSysInfo.gnbgfxline429  = Gfx->gfxplmcfg0 ;
    IDS_HDT_CONSOLE (GNB_TRACE, "TravisLVDSVoltAdjust : %x\n", Gfx->gfxplmcfg0 );
  }

  SystemInfoTableV2.sIntegratedSysInfo.ulOtherDisplayMisc = Gfx->DisplayMiscControl.Value;
  IDS_HDT_CONSOLE (GNB_TRACE, "Display Misc control : %x\n", Gfx->DisplayMiscControl.Value);

  // LVDS
  SystemInfoTableV2.sIntegratedSysInfo.ucLVDSPwrOnSeqDIGONtoDE_in4Ms = Gfx->LvdsPowerOnSeqDigonToDe;
  SystemInfoTableV2.sIntegratedSysInfo.ucLVDSPwrOnSeqDEtoVARY_BL_in4Ms = Gfx->LvdsPowerOnSeqDeToVaryBl;
  SystemInfoTableV2.sIntegratedSysInfo.ucLVDSPwrOffSeqVARY_BLtoDE_in4Ms = Gfx->LvdsPowerOnSeqVaryBlToDe;
  SystemInfoTableV2.sIntegratedSysInfo.ucLVDSPwrOffSeqDEtoDIGON_in4Ms = Gfx->LvdsPowerOnSeqDeToDigon;
  SystemInfoTableV2.sIntegratedSysInfo.ucLVDSOffToOnDelay_in4Ms = Gfx->LvdsPowerOnSeqOnToOffDelay;
  SystemInfoTableV2.sIntegratedSysInfo.ucLVDSPwrOnSeqVARY_BLtoBLON_in4Ms = Gfx->LvdsPowerOnSeqVaryBlToBlon;
  SystemInfoTableV2.sIntegratedSysInfo.ucLVDSPwrOffSeqBLONtoVARY_BL_in4Ms = Gfx->LvdsPowerOnSeqBlonToVaryBl;
  SystemInfoTableV2.sIntegratedSysInfo.ulLCDBitDepthControlVal = Gfx->LcdBitDepthControlValue;
  SystemInfoTableV2.sIntegratedSysInfo.usMaxLVDSPclkFreqInSingleLink = Gfx->LvdsMaxPixelClockFreq;
  Status = PcieLocateConfigurationData (GnbLibGetHeader (Gfx), &Pcie);
  ASSERT (Status == AGESA_SUCCESS);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    Status = GfxIntegratedEnumerateAllConnectors (
               &SystemInfoTableV2.sIntegratedSysInfo.sExtDispConnInfo.sPath[0],
               Pcie,
               Gfx
               );
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
  }

  SystemInfoTableV2.sIntegratedSysInfo.sExtDispConnInfo.uceDPToLVDSRxId = eDP_TO_LVDS_RX_DISABLE;
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_VIRTUAL | DESCRIPTOR_DDI_ENGINE,
    GfxIntegrateducEDPToLVDSRxIdCallback,
    &SystemInfoTableV2.sIntegratedSysInfo.sExtDispConnInfo.uceDPToLVDSRxId,
    Pcie
    );

  // Build PP table
  PpTable = (ATOM_PPLIB_POWERPLAYTABLE3*) &SystemInfoTableV2.ulPowerplayTable;
  // Build PP table
  Status = GfxPowerPlayBuildTable (PpTable,  Gfx);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  // Assign usFormatID to 0x000B to represent Trinity
  PpTable->usFormatID = 0xB;
  // Build info from fuses
  PpFuseArray = GnbLocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, GnbLibGetHeader (Gfx));
  ASSERT (PpFuseArray != NULL);
  if (PpFuseArray != NULL) {
    // Build Display clock info
    GfxIntInfoTableInitDispclkTable (PpFuseArray, &SystemInfoTableV2.sIntegratedSysInfo, Gfx);
    // Build Sclk info table
    GfxIntInfoTableInitSclkTable (PpFuseArray, &SystemInfoTableV2.sIntegratedSysInfo, Gfx);
  } else {
    Status = AGESA_ERROR;
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
  }
  //@todo review if thouse parameters needed
  // Fill in Nb P-state MemclkFreq Data
  GfxFillNbPstateMemclkFreqTN (&SystemInfoTableV2.sIntegratedSysInfo, Gfx);
  // Fill in HTC Data
  GfxFillHtcDataTN (&SystemInfoTableV2.sIntegratedSysInfo, Gfx);
  // Fill in NB P states VID
  GfxFillNbPStateVidTN (&SystemInfoTableV2.sIntegratedSysInfo, Gfx);
  // Fill in NCLK info
  //GfxFillNclkInfo (&SystemInfoV1Table.sIntegratedSysInfo, Gfx);
  // Fill in the M3 arbitration control tables
  //GfxFillM3ArbritrationControl (&SystemInfoV1Table.sIntegratedSysInfo, Gfx);
  // Family specific data update

  // Determine ulGMCRestoreResetTime
  Status = GfxCalculateRestoreResetTimeTN (&SystemInfoTableV2.sIntegratedSysInfo, Gfx, PpFuseArray);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);

  //GfxFmIntegratedInfoTableInit (&SystemInfoV1Table.sIntegratedSysInfo, Gfx);
  SystemInfoTableV2.sIntegratedSysInfo.ulDDR_DLL_PowerUpTime = 4940;
  SystemInfoTableV2.sIntegratedSysInfo.ulDDR_PLL_PowerUpTime = 2000;

  if ((Gfx->UmaInfo.UmaAttributes & UMA_ATTRIBUTE_ON_DCT0) != 0) {
    Channel = 0;
  } else {
    Channel = 1;
  }
  if (GfxLibGetMemPhyPllPdModeTN (Channel, GnbLibGetHeader (Gfx)) != 0) {
    SystemInfoTableV2.sIntegratedSysInfo.ulSystemConfig |= BIT2;
  }
  if (GfxLibGetDisDllShutdownSRTN (Channel, GnbLibGetHeader (Gfx)) == 0) {
    SystemInfoTableV2.sIntegratedSysInfo.ulSystemConfig |= BIT1;
  }
  if (GnbBuildOptions.CfgPciePowerGatingFlags != (PCIE_POWERGATING_SKIP_CORE | PCIE_POWERGATING_SKIP_PHY)) {
    SystemInfoTableV2.sIntegratedSysInfo.ulSystemConfig |= BIT0;
  }
  SystemInfoTableV2.sIntegratedSysInfo.ulGPUCapInfo = GPUCAPINFO_TMDS_HDMI_USE_CASCADE_PLL_MODE | GPUCAPINFO_DP_USE_SINGLE_PLL_MODE;

  IDS_HDT_CONSOLE (GNB_TRACE, "ulSystemConfig : %x\n", SystemInfoTableV2.sIntegratedSysInfo.ulSystemConfig);
  IDS_OPTION_CALLOUT (IDS_CALLOUT_GNB_INTEGRATED_TABLE_CONFIG, &SystemInfoTableV2.sIntegratedSysInfo, GnbLibGetHeader (Gfx));
  //Copy integrated info table to Frame Buffer. (Do not use LibAmdMemCopy, routine not guaranteed access to above 4G memory in 32 bit mode.)
  GfxIntInfoTabablePostToFb (&SystemInfoTableV2, Gfx);
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntegratedInfoTableInit Exit [0x%x]\n", Status);
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Build integrated info table
 *  GMC FB access requred
 *
 *
 * @param[in]   StdHeader     Standard configuration header
 * @retval      AGESA_STATUS
 */
AGESA_STATUS
GfxIntInfoTableInterfaceTN (
  IN      AMD_CONFIG_PARAMS       *StdHeader
  )
{
  AGESA_STATUS                    AgesaStatus;
  AGESA_STATUS                    Status;
  GFX_PLATFORM_CONFIG             *Gfx;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntegratedInfoTableInterfaceTN Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  if (GfxLibIsControllerPresent (StdHeader)) {
    Status = GfxLocateConfigData (StdHeader, &Gfx);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    if (Status != AGESA_FATAL) {
      Status = GfxIntInfoTableInitTN (Gfx);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntegratedInfoTableInterfaceTN Exit[0x%x]\n", AgesaStatus);
  return AgesaStatus;
}
