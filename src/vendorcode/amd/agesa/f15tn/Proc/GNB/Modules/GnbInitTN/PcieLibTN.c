/* $NoKeywords:$ */
/**
 * @file
 *
 * TN specific PCIe configuration data services
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
#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbFuseTable.h"
#include  "GnbPcieConfig.h"
#include  "GnbCommonLib.h"
#include  "GnbSbLib.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieTrainingV1.h"
#include  "GnbNbInitLibV4.h"
#include  "GnbNbInitLibV1.h"
#include  "PcieComplexDataTN.h"
#include  "PcieLibTN.h"
#include  "GnbRegistersTN.h"
#include  "GnbRegisterAccTN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_PCIELIBTN_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
PCIE_LINK_SPEED_CAP
PcieGetLinkSpeedCapTN (
  IN       UINT32                Flags,
  IN       PCIe_ENGINE_CONFIG    *Engine
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Control port visibility in PCI config space
 *
 *
 * @param[in]  Control         Make port Hide/Unhide ports
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */
VOID
PciePortsVisibilityControlTN (
  IN      PCIE_PORT_VISIBILITY    Control,
  IN      PCIe_PLATFORM_CONFIG    *Pcie
  )
{
  PCIe_SILICON_CONFIG *Silicon;
  Silicon = (PCIe_SILICON_CONFIG *) PcieConfigGetChild (DESCRIPTOR_SILICON, &Pcie->Header);
  ASSERT (Silicon != NULL);
  switch (Control) {
  case UnhidePorts:
    PcieSiliconUnHidePorts (Silicon, Pcie);
    break;
  case HidePorts:
    PcieSiliconHidePorts (Silicon, Pcie);
    break;
  default:
    ASSERT (FALSE);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Power down inactive lanes
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */

VOID
PciePowerDownPllInL1TN (
  IN       PCIe_WRAPPER_CONFIG    *Wrapper,
  IN       PCIe_PLATFORM_CONFIG   *Pcie
  )
{

  UINT32              LaneBitmapForPllOffInL1;
  UINT32              ActiveLaneBitmap;
  UINT8               PllPowerUpLatency;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerDownPllInL1TN Enter\n");
  ActiveLaneBitmap = PcieUtilGetWrapperLaneBitMap (LANE_TYPE_PCIE_PHY_NATIVE_ACTIVE | LANE_TYPE_PCIE_PHY_NATIVE_HOTPLUG, 0, Wrapper);
  if (ActiveLaneBitmap != 0) {
    PllPowerUpLatency = PciePifGetPllPowerUpLatencyTN (Wrapper, Pcie);
    LaneBitmapForPllOffInL1 = PcieLanesToPowerDownPllInL1 (PllPowerUpLatency, Wrapper, Pcie);
    if ((LaneBitmapForPllOffInL1 != 0) && ((ActiveLaneBitmap & LaneBitmapForPllOffInL1) == ActiveLaneBitmap)) {
      LaneBitmapForPllOffInL1 &= PcieUtilGetWrapperLaneBitMap (LANE_TYPE_PHY_NATIVE_ALL, 0, Wrapper);
      LaneBitmapForPllOffInL1 |= PcieUtilGetWrapperLaneBitMap (LANE_TYPE_PCIE_PHY_NATIVE_MASTER_PLL, 0, Wrapper);
      PciePifSetPllModeForL1 (LaneBitmapForPllOffInL1, Wrapper, Pcie);
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerDownPllInL1TN Exit\n");
}



/*----------------------------------------------------------------------------------------*/
/**
 * Request boot up voltage
 *
 *
 *
 * @param[in]  LinkCap             Global GEN capability
 * @param[in]  Pcie                Pointer to PCIe configuration data area
 */
VOID
PcieSetVoltageTN (
  IN      PCIE_LINK_SPEED_CAP   LinkCap,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8                      TargetVid;
  D0F0xBC_xE010705C_STRUCT   D0F0xBC_xE010705C;
  GMMx63C_STRUCT             GMMx63C;
  GMMx640_STRUCT             GMMx640;
  UINT8                      MinVidIndex;
  D0F0xBC_xE0001008_STRUCT   D0F0xBC_xE0001008;
  UINT8                      SclkVid[4];
  UINT8                      Index;
  PP_FUSE_ARRAY              *PpFuseArray;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieSetVoltageTN Enter\n");
  PpFuseArray = GnbLocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, GnbLibGetHeader (Pcie));
  if (PpFuseArray == NULL) {
    GnbRegisterReadTN (D0F0xBC_xE0001008_TYPE, D0F0xBC_xE0001008_ADDRESS, &D0F0xBC_xE0001008, 0, GnbLibGetHeader (Pcie));
    SclkVid[0] = (UINT8) D0F0xBC_xE0001008.Field.SClkVid0;
    SclkVid[1] = (UINT8) D0F0xBC_xE0001008.Field.SClkVid1;
    SclkVid[2] = (UINT8) D0F0xBC_xE0001008.Field.SClkVid2;
    SclkVid[3] = (UINT8) D0F0xBC_xE0001008.Field.SClkVid3;
    GnbRegisterReadTN (TYPE_D0F0xBC, D0F0xBC_xE010705C_ADDRESS, &D0F0xBC_xE010705C, 0, GnbLibGetHeader (Pcie));
    Index = (UINT8) D0F0xBC_xE010705C.Field.PcieGen2Vid;
  } else {
    SclkVid[0] = PpFuseArray->SclkVid[0];
    SclkVid[1] = PpFuseArray->SclkVid[1];
    SclkVid[2] = PpFuseArray->SclkVid[2];
    SclkVid[3] = PpFuseArray->SclkVid[3];
    Index = PpFuseArray->PcieGen2Vid;
  }
  if (LinkCap > PcieGen1) {
    ASSERT (SclkVid[Index] != 0);
    TargetVid = SclkVid[Index];
  } else {

    MinVidIndex = 0;
    for (Index = 0; Index < 4; Index++) {
      if (SclkVid[Index] > SclkVid[MinVidIndex]) {
        MinVidIndex = (UINT8) Index;
      }
    }
    ASSERT (SclkVid[MinVidIndex] != 0);
    TargetVid = SclkVid[MinVidIndex];
  }
  IDS_HDT_CONSOLE (PCIE_MISC, "  Set Voltage for Gen %d, Vid code %d\n", LinkCap, TargetVid);

  GnbRegisterReadTN (GMMx63C_TYPE, GMMx63C_ADDRESS, &GMMx63C, 0, GnbLibGetHeader (Pcie));
  //Wait for voltage change to complete before it can issue next voltage change request
  do {
    GnbRegisterReadTN (GMMx640_TYPE, GMMx640_ADDRESS, &GMMx640, 0, GnbLibGetHeader (Pcie));
  } while (GMMx640.Field.VoltageChangeAck != GMMx63C.Field.VoltageChangeReq);
  //Enable voltage client
  if (LinkCap == PcieGen1) {
    GMMx63C.Field.VoltageChangeEn = 0;
  } else {
    GMMx63C.Field.VoltageChangeEn = 1;
  }
  GnbRegisterWriteTN (GMMx63C_TYPE, GMMx63C_ADDRESS, &GMMx63C, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Pcie));
  //Program level and toggle request
  GMMx63C.Field.VoltageLevel = TargetVid;
  GMMx63C.Field.VoltageChangeReq = !GMMx63C.Field.VoltageChangeReq;
  GnbRegisterWriteTN (GMMx63C_TYPE, GMMx63C_ADDRESS, &GMMx63C, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Pcie));
  //Wait for voltage change to complete before it can issue next voltage change request
  do {
    GnbRegisterReadTN (GMMx640_TYPE, GMMx640_ADDRESS, &GMMx640, 0, GnbLibGetHeader (Pcie));
  } while (GMMx640.Field.VoltageChangeAck != GMMx63C.Field.VoltageChangeReq);

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieSetVoltageTN Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * PLL power up latency
 *
 *
 * @param[in]  Wrapper     Pointer to Wrapper config descriptor
 * @param[in]  Pcie        Pointer to PICe configuration data area
 * @retval                 Pll wake up latency in us
 */
UINT8
PciePifGetPllPowerUpLatencyTN (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  return 35;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get max link speed capability supported by this port
 *
 *
 *
 * @param[in]  Flags              See Flags PCIE_PORT_GEN_CAP_BOOT / PCIE_PORT_GEN_CAP_MAX
 * @param[in]  Engine             Pointer to engine config descriptor
 * @retval     PcieGen1/PcieGen2  Max supported link gen capability
 */
PCIE_LINK_SPEED_CAP
PcieGetLinkSpeedCapTN (
  IN       UINT32                Flags,
  IN       PCIe_ENGINE_CONFIG    *Engine
  )
{
  PCIE_LINK_SPEED_CAP   LinkSpeedCapability;
  TN_COMPLEX_CONFIG     *ComplexData;
  PCIe_PLATFORM_CONFIG  *Pcie;

  ASSERT (Engine->Type.Port.PortData.LinkSpeedCapability < MaxPcieGen);
  Pcie = (PCIe_PLATFORM_CONFIG *) PcieConfigGetParent (DESCRIPTOR_PLATFORM, &Engine->Header);
  LinkSpeedCapability = PcieGen2;
  ComplexData = (TN_COMPLEX_CONFIG *) PcieConfigGetParent (DESCRIPTOR_SILICON, &Engine->Header);
  if (ComplexData->FmSilicon.OscMode == OscRO || ComplexData->FmSilicon.OscMode == OscLC || ComplexData->FmSilicon.OscMode == OscDefault) {
    LinkSpeedCapability = PcieGen2;
  } else {
    LinkSpeedCapability = PcieGen1;
  }
  if (Engine->Type.Port.PortData.LinkSpeedCapability == PcieGenMaxSupported) {
    Engine->Type.Port.PortData.LinkSpeedCapability = (UINT8) LinkSpeedCapability;
  }
  if (Pcie->PsppPolicy == PsppPowerSaving) {
    LinkSpeedCapability = PcieGen1;
  }
  if (Engine->Type.Port.PortData.LinkSpeedCapability < LinkSpeedCapability) {
    LinkSpeedCapability = Engine->Type.Port.PortData.LinkSpeedCapability;
  }
  if ((Flags & PCIE_PORT_GEN_CAP_BOOT) != 0) {
    if ((Pcie->PsppPolicy == PsppBalanceLow || Engine->Type.Port.PortData.MiscControls.LinkSafeMode == PcieGen1) && !PcieConfigIsSbPcieEngine (Engine)) {
      LinkSpeedCapability = PcieGen1;
    }
  }
  return LinkSpeedCapability;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Set PLL personality
 *
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
VOID
PcieSetPhyPersonalityTN (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8                   Phy;
  UINT8                   Mode;
  if (Wrapper->WrapId == GFX_WRAP_ID || Wrapper->WrapId == DDI_WRAP_ID || Wrapper->WrapId == DDI2_WRAP_ID) {
    for (Phy = 0; Phy < Wrapper->NumberOfPIFs; Phy++) {
      if (Wrapper->WrapId == GFX_WRAP_ID) {
        Mode = (Phy == 0)? 0x1 : 0;
      } else {
        Mode = 0x2;
      }
      PcieRegisterWriteField (
        Wrapper,
        PHY_SPACE (Wrapper->WrapId, Phy, D0F0xE4_PHY_2005_ADDRESS),
        D0F0xE4_PHY_2005_PllMode_OFFSET,
        D0F0xE4_PHY_2005_PllMode_WIDTH,
        Mode,
        FALSE,
        Pcie
        );
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * DCC recalibration
 *
 *
 *
 * @param[in]     Wrapper           Pointer to internal configuration data area
 * @param[in,out] Buffer            Pointer to buffer
 * @param[in]     Pcie              Pointer to global PCIe configuration
 */

STATIC AGESA_STATUS
PcieForceDccRecalibrationCallbackTN (
  IN       PCIe_WRAPPER_CONFIG   *Wrapper,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PciePhyForceDccRecalibration (Wrapper, Pcie);
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Prepare for Osc switch
 *
 *
 *
 * @param[in]  Wrapper             Pointer to Wrapper config descriptor
 * @param[in]  Buffer              Pointer to buffer
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */

STATIC AGESA_STATUS
PcieOscPifInitPrePowerdownCallback (
  IN       PCIe_WRAPPER_CONFIG   *Wrapper,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PciePifFullPowerStateControl (PowerDownPifs, Wrapper, Pcie);
  PcieTopologyLaneControl (
    DisableLanes,
    PcieUtilGetWrapperLaneBitMap (LANE_TYPE_CORE_ALL, LANE_TYPE_PCIE_SB_CORE_CONFIG, Wrapper),
    Wrapper,
    Pcie
    );
  PciePifSetPllRampTime (LongRampup, Wrapper, Pcie);
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Do Osc switch
 *
 *
 *
 * @param[in]  Wrapper             Pointer to Wrapper config descriptor
 * @param[in]  Buffer              Pointer to buffer
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */

STATIC AGESA_STATUS
PcieOscInitPllModeCallback (
  IN       PCIe_WRAPPER_CONFIG   *Wrapper,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  TN_COMPLEX_CONFIG         *ComplexData;
  TN_PCIe_SILICON_CONFIG    *FmSilicon;
  UINT8                     Phy;
  ComplexData = (TN_COMPLEX_CONFIG *) PcieConfigGetParentSilicon (Wrapper);
  ASSERT (ComplexData != NULL);
  FmSilicon = &ComplexData->FmSilicon;
  if (Wrapper->WrapId == GFX_WRAP_ID) {
    Phy = 1;
  } else if (Wrapper->WrapId == GPP_WRAP_ID) {
    Phy = 0;
  } else {
    ASSERT (FALSE);
    return AGESA_ERROR;
  }
  switch (FmSilicon->OscMode) {
  case  OscLC:
    PcieRegisterWriteField (
      Wrapper,
      PHY_SPACE (Wrapper->WrapId, Phy, D0F0xE4_PHY_2002_ADDRESS),
      D0F0xE4_PHY_2002_IsLc_OFFSET,
      D0F0xE4_PHY_2002_IsLc_WIDTH,
      0x1,
      FALSE,
      Pcie
      );
    break;
  case  OscRO:
    PcieRegisterWriteField (
      Wrapper,
      PHY_SPACE (Wrapper->WrapId, Phy, D0F0xE4_PHY_2002_ADDRESS),
      D0F0xE4_PHY_2002_RoCalEn_OFFSET,
      D0F0xE4_PHY_2002_RoCalEn_WIDTH,
      0x0,
      FALSE,
      Pcie
      );
    PcieRegisterWriteField (
      Wrapper,
      PHY_SPACE (Wrapper->WrapId, Phy, D0F0xE4_PHY_2002_ADDRESS),
      D0F0xE4_PHY_2002_RoCalEn_OFFSET,
      D0F0xE4_PHY_2002_RoCalEn_WIDTH,
      0x1,
      FALSE,
      Pcie
      );
    break;
  default:
    ASSERT (FALSE);
  }
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Post Osc init
 *
 *
 *
 * @param[in]  Wrapper             Pointer to Wrapper config descriptor
 * @param[in]  Buffer              Pointer to buffer
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */

STATIC AGESA_STATUS
PcieOscPifInitPostPowerdownCallback (
  IN       PCIe_WRAPPER_CONFIG   *Wrapper,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PcieWrapSetTxS1CtrlForLaneMux (Wrapper, Pcie);
  PciePollPifForCompeletion (Wrapper, Pcie);
  PcieTopologyLaneControl (
    EnableLanes,
    PcieUtilGetWrapperLaneBitMap (LANE_TYPE_CORE_ALL, 0, Wrapper),
    Wrapper,
    Pcie
    );
  PcieWrapSetTxOffCtrlForLaneMux (Wrapper, Pcie);
  PciePollPifForCompeletion (Wrapper, Pcie);
  PciePifSetPllRampTime (NormalRampup, Wrapper, Pcie);
  PciePifFullPowerStateControl (PowerUpPifs, Wrapper, Pcie);
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Prepare PHY for Gen2
 *
 *
 *
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */

VOID
PcieOscInitTN (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  TN_COMPLEX_CONFIG         *ComplexData;
  TN_PCIe_SILICON_CONFIG    *FmSilicon;
  D0F0xE4_WRAP_FFF1_STRUCT  D0F0xE4_WRAP_FFF1;
  AGESA_STATUS              Status;
  UINT8                     SaveSbLinkAspm;
  UINT32                    Value;

  Value = 0;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieOscInitTN Enter\n");
  ComplexData = (TN_COMPLEX_CONFIG *) PcieConfigGetChild (DESCRIPTOR_SILICON, &Pcie->Header);
  ASSERT (ComplexData != NULL);
  FmSilicon = &ComplexData->FmSilicon;

  IDS_HDT_CONSOLE (GNB_TRACE, "  OSC Mode - %s\n",
    (FmSilicon->OscMode == OscFuses) ? "Fuses" : (
    (FmSilicon->OscMode == OscRO) ? "RO" : (
    (FmSilicon->OscMode == OscLC) ? "LC" : (
    (FmSilicon->OscMode == OscDefault) ? "Skip" : "Unknown")))
    );

  if (FmSilicon->OscMode == OscFuses) {
    D0F0xE4_WRAP_FFF1.Value = PcieRegisterRead (
                                &ComplexData->GppWrapper,
                                WRAP_SPACE (ComplexData->GppWrapper.WrapId, D0F0xE4_WRAP_FFF1_ADDRESS),
                                Pcie
                                );

    if (D0F0xE4_WRAP_FFF1.Field.ROSupportGen2) {
      FmSilicon->OscMode = OscRO;
    } else if (D0F0xE4_WRAP_FFF1.Field.LcSupportGen2) {
      FmSilicon->OscMode = OscLC;
    } else {
      FmSilicon->OscMode = OscDefault;
    }

    IDS_HDT_CONSOLE (GNB_TRACE, "  OSC Mode From Fuses - %s\n",
      (FmSilicon->OscMode == OscFuses) ? "Fuses" : (
      (FmSilicon->OscMode == OscRO) ? "RO" : (
      (FmSilicon->OscMode == OscLC) ? "LC" : (
      (FmSilicon->OscMode == OscDefault) ? "Skip" : "Unknown")))
      );
  }
  if (FmSilicon->OscMode != OscDefault) {

    PcieConfigRunProcForAllWrappers (
      DESCRIPTOR_PCIE_WRAPPER,
      PcieOscPifInitPrePowerdownCallback,
      NULL,
      Pcie
      );
    PcieConfigRunProcForAllWrappers (
      DESCRIPTOR_PCIE_WRAPPER,
      PcieOscInitPllModeCallback,
      NULL,
      Pcie
      );
    PcieConfigRunProcForAllWrappers (
      DESCRIPTOR_PCIE_WRAPPER,
      PcieForceDccRecalibrationCallbackTN,
      NULL,
      Pcie
      );

    SaveSbLinkAspm = ComplexData->Port8.Type.Port.PortData.LinkAspm;
    ComplexData->Port8.Type.Port.PortData.LinkAspm = AspmL1;

    Status = SbPcieLinkAspmControl (&ComplexData->Port8, Pcie);
    ASSERT (Status == AGESA_SUCCESS);
#ifdef  USE_L1_POLLING
    //Use L1 Entry pooling
    PciePollLinkForL1Entry (&ComplexData->Port8, Pcie);
#else
    {
      D0F0xBC_x1F630_STRUCT  D0F0xBC_x1F630;

      GnbRegisterReadTN (D0F0xBC_x1F630_TYPE,  D0F0xBC_x1F630_ADDRESS, &D0F0xBC_x1F630.Value, 0, GnbLibGetHeader (Pcie));
      D0F0xBC_x1F630.Field.RECONF_WAIT = 60;
      GnbRegisterWriteTN (D0F0xBC_x1F630_TYPE,  D0F0xBC_x1F630_ADDRESS, &D0F0xBC_x1F630.Value, 0, GnbLibGetHeader (Pcie));

      GnbSmuServiceRequestV4 (
        ComplexData->Silicon.Address,
        SMC_MSG_PCIE_PLLSWITCH,
        0,
        GnbLibGetHeader (Pcie)
      );
    }
#endif
    ComplexData->Port8.Type.Port.PortData.LinkAspm = AspmDisabled;

    SbPcieLinkAspmControl (&ComplexData->Port8, Pcie);
    PciePollLinkForL0Exit (&ComplexData->Port8, Pcie);

    ComplexData->Port8.Type.Port.PortData.LinkAspm = SaveSbLinkAspm;

    PcieConfigRunProcForAllWrappers (
      DESCRIPTOR_PCIE_WRAPPER,
      PcieOscPifInitPostPowerdownCallback,
      NULL,
      Pcie
      );
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieOscInitTN Exit\n");
}


