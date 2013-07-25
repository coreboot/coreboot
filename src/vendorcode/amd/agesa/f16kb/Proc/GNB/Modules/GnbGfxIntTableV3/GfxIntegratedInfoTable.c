/* $NoKeywords:$ */
/**
 * @file
 *
 * Integrated table info init
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
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
#include  "amdlib.h"
#include  "Ids.h"
#include  "heapManager.h"
#include  "GeneralServices.h"
#include  "Gnb.h"
#include  "GnbF1Table.h"
#include  "GnbPcie.h"
#include  "GnbGfx.h"
#include  "GnbSbLib.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbGfxConfig.h"
#include  "GnbGfxInitLibV1.h"
#include  "GnbGfxFamServices.h"
#include  "GnbNbInitLibV1.h"
#include  "GnbNbInitLibV5.h"
#include  "GfxConfigLib.h"
#include  "GfxIntegratedInfoTable.h"
#include  "GfxPwrPlayTable.h"
#include  "OptionGnb.h"
#include  "GfxLibV3.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBGFXINTTABLEV3_GFXINTEGRATEDINFOTABLE_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern GNB_BUILD_OPTIONS  GnbBuildOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Init V3 Support for eDP to Lvds translators
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
  if (Engine->Type.Ddi.DdiData.ConnectorType == ConnectorTypeEDPToLvdsSwInit) {
    *uceDPToLVDSRxId = eDP_TO_LVDS_SWINIT_ID;
    IDS_HDT_CONSOLE (GNB_TRACE, "Found EDPToLvds Connector requiring SW init\n");
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Calculate V3 NCLK clock
 *
 *
 *
 * @param[in] NbFid         NbFid
 * @param[in] NbDid         NbDid
 * @retval                  Clock in 10KHz
 */

STATIC UINT32
GfxLibGetNclkV3 (
  IN      UINT8                   NbFid,
  IN      UINT8                   NbDid
  )
{
  UINT32  Divider;
  //i.e. NBCOF[0] = (100 * ([NbFid] + 4h) / (2^[NbDid])) Mhz
  if (NbDid == 1) {
    Divider = 2;
  } else if (NbDid == 0) {
    Divider = 1;
  } else {
    Divider = 1;
  }
  ASSERT (NbDid == 0 || NbDid == 1);
  return ((10000 * (NbFid + 4)) / Divider);
}

/*----------------------------------------------------------------------------------------*/
/**
 *Init V3 Nb p-State MemclkFreq
 *
 *
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] PpF1Array           pointer
 * @param[in] Gfx                 Gfx configuration info
 */

STATIC VOID
GfxFillNbPstateMemclkFreqV3 (
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V1_8 *IntegratedInfoTable,
  IN       PP_F1_ARRAY_V2                 *PpF1Array,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  UINT8                           i;
  UINT8                           Channel;
  ULONG                           memps0_freq;
  ULONG                           memps1_freq;

  Channel = 0;
  if ((Gfx->UmaInfo.UmaAttributes & UMA_ATTRIBUTE_ON_DCT1) != 0) {
    Channel = 1;
  } else if ((Gfx->UmaInfo.UmaAttributes & UMA_ATTRIBUTE_ON_DCT2) != 0) {
    Channel = 2;
  } else if ((Gfx->UmaInfo.UmaAttributes & UMA_ATTRIBUTE_ON_DCT3) != 0) {
    Channel = 3;
  }


  memps0_freq = 100 * GfxLibExtractDramFrequencyV3 ((UINT8) PpF1Array->MemClkFreq[Channel], GnbLibGetHeader (Gfx));
  memps1_freq = 100 * GfxLibExtractDramFrequencyV3 ((UINT8) PpF1Array->M1MemClkFreq[Channel], GnbLibGetHeader (Gfx));

  for (i = 0; i < 4; i++) {
    if (PpF1Array->PP_FUSE_ARRAY_V2_fld26[i] == 1) {
      IntegratedInfoTable->ulNbpStateMemclkFreq[i] = (PpF1Array->PP_FUSE_ARRAY_V2_fld29[i] == 0) ? memps0_freq : memps1_freq;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 *Init V3 NbPstateVid
 *
 *
 * @param[in] PpF1Array
 * @param[in] IntegratedInfoTable Integrated info table pointer
 * @param[in] Gfx                 Gfx configuration info
 */

STATIC VOID
GfxFillNbPStateVidV3 (
  IN       PP_F1_ARRAY_V2                 *PpF1Array,
  IN OUT   ATOM_INTEGRATED_SYSTEM_INFO_V1_8 *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  UINT8                   i;

  for (i = 0; i < 4; i++) {
    IntegratedInfoTable->usNBPStateVoltage[i] = (USHORT) ((PpF1Array->PP_FUSE_ARRAY_V2_fld28[i] << 7) | (PpF1Array->PP_FUSE_ARRAY_V2_fld27[i]));
    IntegratedInfoTable->ulNbpStateNClkFreq[i] = GfxLibGetNclkV3 ((UINT8) (PpF1Array->PP_FUSE_ARRAY_V2_fld30[i]), (UINT8) (PpF1Array->PP_FUSE_ARRAY_V2_fld31[i]));
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Copy memory content to FB
 *
 *
 * @param[in] SystemInfoTableV3Ptr    Pointer to integrated info table
 * @param[in] Gfx                     Pointer to global GFX configuration
 *
 */
VOID
GfxIntInfoTablePostToFbV3 (
  IN       ATOM_FUSION_SYSTEM_INFO_V3     *SystemInfoTableV3Ptr,
  IN       GFX_PLATFORM_CONFIG            *Gfx
  )
{
  UINT32      Index;
  UINT32      TableOffset;
  UINT32      FbAddress;
  TableOffset = (UINT32) (Gfx->UmaInfo.UmaSize - sizeof (ATOM_FUSION_SYSTEM_INFO_V3)) | 0x80000000;
  for (Index = 0; Index < sizeof (ATOM_FUSION_SYSTEM_INFO_V3); Index = Index + 4 ) {
    FbAddress = TableOffset + Index;
    GnbLibMemWrite (Gfx->GmmBase + GMMx00_ADDRESS, AccessWidth32, &FbAddress, GnbLibGetHeader (Gfx));
    GnbLibMemWrite (Gfx->GmmBase + GMMx04_ADDRESS, AccessWidth32, (UINT8*) SystemInfoTableV3Ptr + Index, GnbLibGetHeader (Gfx));
  }
}


STATIC VOID
GfxIntegratedInfoTable289_fun (
  IN       PP_F1_ARRAY_V2                 *PpF1Array,
  IN       ATOM_INTEGRATED_SYSTEM_INFO_V1_8 *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  UINTN   Index;
  for (Index = 0; Index < 4; Index++) {
    if (PpF1Array->excel841_fld6[Index] != 0) {
      IntegratedInfoTable->ATOM_INTEGRATED_SYSTEM_INFO_V1_8_fld4[Index].ulMaximumSupportedCLK = GfxFmCalculateClock (
                                                                             PpF1Array->excel841_fld6[Index],
                                                                             GnbLibGetHeader (Gfx)
                                                                             );
      IntegratedInfoTable->ATOM_INTEGRATED_SYSTEM_INFO_V1_8_fld4[Index].ulVoltageIndex = (ULONG) Index;
    }
  }
}

/*----------------------------------------------------------------------------------------*/

STATIC VOID
GfxIntegratedInfoTable318_fun (
  IN       PP_F1_ARRAY_V2                 *PpF1Array,
  IN       ATOM_INTEGRATED_SYSTEM_INFO_V1_8 *IntegratedInfoTable,
  IN       GFX_PLATFORM_CONFIG              *Gfx
  )
{
  UINT8                       Index;
  UINTN                       v1;
  GnbGfx275_STRUCT    *pv2;
  BOOLEAN                     Sorting;
  pv2 = &IntegratedInfoTable->ATOM_INTEGRATED_SYSTEM_INFO_V1_8[0];

  v1 = 0;
  for (Index = 0; Index < 5; Index++) {
    if (PpF1Array->PP_FUSE_ARRAY_V2_fld33[Index] != 0) {
      pv2[v1].GnbGfx275_STRUCT_fld0 = GfxFmCalculateClock (PpF1Array->PP_FUSE_ARRAY_V2_fld33[Index], GnbLibGetHeader (Gfx));
      pv2[v1].GnbGfx275_STRUCT_fld1 = Index;
      pv2[v1].GnbGfx275_STRUCT_fld2 = PpF1Array->PP_FUSE_ARRAY_V2_fld32[Index];
      v1++;
    }
  }
  if (v1 > 1) {
    do {
      Sorting = FALSE;
      for (Index = 0; Index < (v1 - 1); Index++) {
        GnbGfx275_STRUCT  Temp;
        BOOLEAN                   Exchange;
        Exchange = FALSE;
        if (pv2[Index].GnbGfx275_STRUCT_fld1 > pv2[Index + 1].GnbGfx275_STRUCT_fld1) {
          Exchange = TRUE;
        }
        if ((pv2[Index].GnbGfx275_STRUCT_fld1 == pv2[Index + 1].GnbGfx275_STRUCT_fld1) &&
            (pv2[Index].GnbGfx275_STRUCT_fld0 > pv2[Index + 1].GnbGfx275_STRUCT_fld0)) {
          Exchange = TRUE;
        }
        if (Exchange) {
          Sorting = TRUE;
          LibAmdMemCopy (&Temp, &pv2[Index], sizeof (GnbGfx275_STRUCT), GnbLibGetHeader (Gfx));
          LibAmdMemCopy (&pv2[Index], &pv2[Index + 1], sizeof (GnbGfx275_STRUCT), GnbLibGetHeader (Gfx));
          LibAmdMemCopy (&pv2[Index + 1], &Temp, sizeof (GnbGfx275_STRUCT), GnbLibGetHeader (Gfx));
        }
      }
    } while (Sorting);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Build integrated info table
 *
 *
 *
 * @param[in] Gfx                 Gfx configuration info
 * @param[in] SystemInfoTableV3   ATOM_FUSION_SYSTEM_INFO_V3 pointer
 * @param[in] PpF1Array
 * @retval    AGESA_STATUS
 */
AGESA_STATUS
GfxIntInfoTableInitV3 (
  IN      GFX_PLATFORM_CONFIG         *Gfx,
  IN      ATOM_FUSION_SYSTEM_INFO_V3  *SystemInfoTableV3,
  IN      PP_F1_ARRAY_V2             *PpF1Array
  )
{
  AGESA_STATUS                    Status;
  AGESA_STATUS                    AgesaStatus;
  PCIe_PLATFORM_CONFIG            *Pcie;
  ATOM_PPLIB_POWERPLAYTABLE4      *PpTable;
  UINT8                           Channel;

  AgesaStatus = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntegratedInfoTableInitV3 Enter\n");

  if (PpF1Array != NULL) {

    Channel = 0;
    if ((Gfx->UmaInfo.UmaAttributes & UMA_ATTRIBUTE_ON_DCT1) != 0) {
      Channel = 1;
    } else if ((Gfx->UmaInfo.UmaAttributes & UMA_ATTRIBUTE_ON_DCT2) != 0) {
      Channel = 2;
    } else if ((Gfx->UmaInfo.UmaAttributes & UMA_ATTRIBUTE_ON_DCT3) != 0) {
      Channel = 3;
    }
    SystemInfoTableV3->sIntegratedSysInfo.sHeader.usStructureSize = sizeof (ATOM_INTEGRATED_SYSTEM_INFO_V1_8);
    ASSERT (SystemInfoTableV3->sIntegratedSysInfo.sHeader.usStructureSize == 512);
    SystemInfoTableV3->sIntegratedSysInfo.sHeader.ucTableFormatRevision = 1;
    SystemInfoTableV3->sIntegratedSysInfo.sHeader.ucTableContentRevision = 8;
    SystemInfoTableV3->sIntegratedSysInfo.ulBootUpEngineClock = 200 * 100;    //Set default engine clock to 200MhZ
    SystemInfoTableV3->sIntegratedSysInfo.field2 = (PpF1Array->PP_FUSE_ARRAY_V2_fld21 + 0x10) * 10000;
    SystemInfoTableV3->sIntegratedSysInfo.ulBootUpUMAClock = Gfx->UmaInfo.MemClock * 100;

    SystemInfoTableV3->sIntegratedSysInfo.usRequestedPWMFreqInHz = Gfx->LcdBackLightControl;
    SystemInfoTableV3->sIntegratedSysInfo.ucUMAChannelNumber = ((Gfx->UmaInfo.UmaAttributes & UMA_ATTRIBUTE_INTERLEAVE) == 0) ? 1 : 2;
    SystemInfoTableV3->sIntegratedSysInfo.ucMemoryType = Gfx->UmaInfo.MemType;
    SystemInfoTableV3->sIntegratedSysInfo.usBootUpNBVoltage = GnbLocateHighestVidIndexV5 (GnbLibGetHeader (Gfx));
    SystemInfoTableV3->sIntegratedSysInfo.usPanelRefreshRateRange = Gfx->DynamicRefreshRate;
    SystemInfoTableV3->sIntegratedSysInfo.usLvdsSSPercentage = Gfx->LvdsSpreadSpectrum;
    //Locate PCIe configuration data to get definitions of display connectors
    SystemInfoTableV3->sIntegratedSysInfo.sExtDispConnInfo.sHeader.usStructureSize = sizeof (ATOM_EXTERNAL_DISPLAY_CONNECTION_INFO);
    SystemInfoTableV3->sIntegratedSysInfo.sExtDispConnInfo.sHeader.ucTableFormatRevision = 1;
    SystemInfoTableV3->sIntegratedSysInfo.sExtDispConnInfo.sHeader.ucTableContentRevision = 1;
    SystemInfoTableV3->sIntegratedSysInfo.sExtDispConnInfo.uc3DStereoPinId = Gfx->Gnb3dStereoPinIndex;
    SystemInfoTableV3->sIntegratedSysInfo.sExtDispConnInfo.ucRemoteDisplayConfig = Gfx->GnbRemoteDisplaySupport;
    SystemInfoTableV3->sIntegratedSysInfo.sExtDispConnInfo.ucFixDPVoltageSwing = (UINT8) Gfx->DpFixedVoltSwingType;
    SystemInfoTableV3->sIntegratedSysInfo.usExtDispConnInfoOffset = offsetof (ATOM_INTEGRATED_SYSTEM_INFO_V1_8, sExtDispConnInfo);

    SystemInfoTableV3->sIntegratedSysInfo.usPCIEClkSSPercentage = Gfx->PcieRefClkSpreadSpectrum;

    SystemInfoTableV3->sIntegratedSysInfo.ucLvdsMisc = Gfx->LvdsMiscControl.Value;
    IDS_HDT_CONSOLE (GNB_TRACE, "Lvds Misc control : %x\n", Gfx->LvdsMiscControl.Value);
    if (Gfx->LvdsMiscControl.Field.LvdsVoltOverwriteEn) {
      SystemInfoTableV3->sIntegratedSysInfo.ucLVDSVoltAdjust = Gfx->LVDSVoltAdjust;
      IDS_HDT_CONSOLE (GNB_TRACE, "LVDSVoltAdjust : %x\n", Gfx->LVDSVoltAdjust);
    }

    SystemInfoTableV3->sIntegratedSysInfo.ulVBIOSMisc = Gfx->DisplayMiscControl.Value;
    IDS_HDT_CONSOLE (GNB_TRACE, "Display Misc control : %x\n", Gfx->DisplayMiscControl.Value);

    // LVDS
    SystemInfoTableV3->sIntegratedSysInfo.ucLVDSPwrOnSeqDIGONtoDE_in4Ms = Gfx->LvdsPowerOnSeqDigonToDe;
    SystemInfoTableV3->sIntegratedSysInfo.ucLVDSPwrOnSeqDEtoVARY_BL_in4Ms = Gfx->LvdsPowerOnSeqDeToVaryBl;
    SystemInfoTableV3->sIntegratedSysInfo.ucLVDSPwrOffSeqVARY_BLtoDE_in4Ms = Gfx->LvdsPowerOnSeqVaryBlToDe;
    SystemInfoTableV3->sIntegratedSysInfo.ucLVDSPwrOffSeqDEtoDIGON_in4Ms = Gfx->LvdsPowerOnSeqDeToDigon;
    SystemInfoTableV3->sIntegratedSysInfo.ucLVDSOffToOnDelay_in4Ms = Gfx->LvdsPowerOnSeqOnToOffDelay;
    SystemInfoTableV3->sIntegratedSysInfo.ucLVDSPwrOnSeqVARY_BLtoBLON_in4Ms = Gfx->LvdsPowerOnSeqVaryBlToBlon;
    SystemInfoTableV3->sIntegratedSysInfo.ucLVDSPwrOffSeqBLONtoVARY_BL_in4Ms = Gfx->LvdsPowerOnSeqBlonToVaryBl;
    SystemInfoTableV3->sIntegratedSysInfo.ulLCDBitDepthControlVal = Gfx->LcdBitDepthControlValue;
    SystemInfoTableV3->sIntegratedSysInfo.usMaxLVDSPclkFreqInSingleLink = Gfx->LvdsMaxPixelClockFreq;
    SystemInfoTableV3->sIntegratedSysInfo.ucMinAllowedBL_Level = Gfx->MinAllowedBLLevel;
    Status = PcieLocateConfigurationData (GnbLibGetHeader (Gfx), &Pcie);
    ASSERT (Status == AGESA_SUCCESS);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    if (Status == AGESA_SUCCESS) {
      Status = GfxIntegratedEnumerateAllConnectors (
                 &SystemInfoTableV3->sIntegratedSysInfo.sExtDispConnInfo.sPath[0],
                 Pcie,
                 Gfx
                 );
      AGESA_STATUS_UPDATE (Status, AgesaStatus);
    }

    SystemInfoTableV3->sIntegratedSysInfo.sExtDispConnInfo.uceDPToLVDSRxId = eDP_TO_LVDS_RX_DISABLE;
    PcieConfigRunProcForAllEngines (
      DESCRIPTOR_ALLOCATED | DESCRIPTOR_VIRTUAL | DESCRIPTOR_DDI_ENGINE,
      GfxIntegrateducEDPToLVDSRxIdCallback,
      &SystemInfoTableV3->sIntegratedSysInfo.sExtDispConnInfo.uceDPToLVDSRxId,
      Pcie
      );

    // Build PP table
    PpTable = (ATOM_PPLIB_POWERPLAYTABLE4*) &SystemInfoTableV3->ulPowerplayTable;
    // Build PP table
    ///@todo
    //Status = GfxPowerPlayBuildTable (PpTable,  Gfx);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    // Assign usFormatID to 0x000B to represent V3
    ///@todo
    PpTable->usFormatID = 0xB;
    // Build Display clock info
    GfxIntegratedInfoTable289_fun (PpF1Array, &SystemInfoTableV3->sIntegratedSysInfo, Gfx);
    GfxIntegratedInfoTable318_fun (PpF1Array, &SystemInfoTableV3->sIntegratedSysInfo, Gfx);
    ///@todo review if these parameters needed
    // Fill in Nb P-state MemclkFreq Data
    GfxFillNbPstateMemclkFreqV3 (&SystemInfoTableV3->sIntegratedSysInfo, PpF1Array, Gfx);
    // Fill in HTC Data
    if (PpF1Array->HtcEn == 1) {
      SystemInfoTableV3->sIntegratedSysInfo.ucHtcTmpLmt = (UCHAR) (PpF1Array->HtcTmpLmt / 2 + 52);
      SystemInfoTableV3->sIntegratedSysInfo.ATOM_INTEGRATED_SYSTEM_INFO_V1_8_fld11 = (UCHAR) (PpF1Array->PP_FUSE_ARRAY_V2_fld20 / 2);
    } else {
      SystemInfoTableV3->sIntegratedSysInfo.ucHtcTmpLmt = 0;
      SystemInfoTableV3->sIntegratedSysInfo.ATOM_INTEGRATED_SYSTEM_INFO_V1_8_fld11 = 0;
    }
    // Fill in NB P states VID & NCLK info
    GfxFillNbPStateVidV3 (PpF1Array, &SystemInfoTableV3->sIntegratedSysInfo, Gfx);

    // Family specific data update - store default values to be updated by family specific code
    //GfxFmIntegratedInfoTableInit (&SystemInfoV1Table.sIntegratedSysInfo, Gfx);
    SystemInfoTableV3->sIntegratedSysInfo.ulDDR_DLL_PowerUpTime = 4940;
    SystemInfoTableV3->sIntegratedSysInfo.ulDDR_PLL_PowerUpTime = 2000;

    if (PpF1Array->MemPhyPllPdMode[Channel] != 0) {
      SystemInfoTableV3->sIntegratedSysInfo.ulSystemConfig |= BIT2;
    }
    if (PpF1Array->DisDllShutdownSR[Channel] == 0) {
      SystemInfoTableV3->sIntegratedSysInfo.ulSystemConfig |= BIT1;
    }
    if (GnbBuildOptions.CfgPciePowerGatingFlags != (PCIE_POWERGATING_SKIP_CORE | PCIE_POWERGATING_SKIP_PHY)) {
      SystemInfoTableV3->sIntegratedSysInfo.ulSystemConfig |= BIT0;
    }
    SystemInfoTableV3->sIntegratedSysInfo.ulGPUCapInfo = GPUCAPINFO_TMDS_HDMI_USE_CASCADE_PLL_MODE | GPUCAPINFO_DP_USE_SINGLE_PLL_MODE;

    IDS_HDT_CONSOLE (GNB_TRACE, "ulSystemConfig : %x\n", SystemInfoTableV3->sIntegratedSysInfo.ulSystemConfig);

  } else {
    Status = AGESA_ERROR;
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxIntegratedInfoTableInitV3 Exit [0x%x]\n", Status);
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Dump gfx integrated info table
 *
 *
 * @param[in] SystemInfoTableV3Ptr    Pointer to integrated info table
 * @param[in] Gfx                     Pointer to global GFX configuration
 *
 */
VOID
GfxIntInfoTableDebugDumpV3 (
  IN       ATOM_FUSION_SYSTEM_INFO_V3     *SystemInfoTableV3Ptr,
  IN       GFX_PLATFORM_CONFIG            *Gfx
  )
{
  ATOM_PPLIB_POWERPLAYTABLE4      *PpTable;
  ATOM_PPLIB_EXTENDEDHEADER       *ExtendedHeader;

  IDS_HDT_CONSOLE (GFX_MISC, "GfxIntInfoTableDebugDumpV3 Enter\n");

  PpTable = (ATOM_PPLIB_POWERPLAYTABLE4*) &SystemInfoTableV3Ptr->ulPowerplayTable;
  ExtendedHeader = (ATOM_PPLIB_EXTENDEDHEADER *) ((UINT8 *) (PpTable) + PpTable->usExtendendedHeaderOffset);
  IDS_HDT_CONSOLE (GFX_MISC, "    ExtendedHeader  usSize %d\n", ExtendedHeader->usSize);
  IDS_HDT_CONSOLE (GFX_MISC, "                    SizeOf %d\n", sizeof(ATOM_PPLIB_EXTENDEDHEADER));

  IDS_HDT_CONSOLE (GFX_MISC, "    ucHtcTmpLmt  0x%X\n", SystemInfoTableV3Ptr->sIntegratedSysInfo.ucHtcTmpLmt);
  IDS_HDT_CONSOLE (GFX_MISC, "    ATOM_INTEGRATED_SYSTEM_INFO_V1_8_fld11 0x%X\n", SystemInfoTableV3Ptr->sIntegratedSysInfo.ATOM_INTEGRATED_SYSTEM_INFO_V1_8_fld11);
  IDS_HDT_CONSOLE (GFX_MISC, "GfxIntInfoTableDebugDumpV3 Exit\n");
}

