/* $NoKeywords:$ */
/**
 * @file
 *
 *  Family specific PCIe PHY initialization services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 49532 $   @e \$Date: 2011-03-25 02:54:43 +0800 (Fri, 25 Mar 2011) $
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
#include  "Ids.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLibV1.h"
#include  "PcieFamilyServices.h"
#include  "LlanoDefinitions.h"
#include  "cpuRegisters.h"
#include  "GnbRegistersLN.h"
#include  "cpuFamilyTranslation.h"
#include  "NbSmuLib.h"
#include  "GnbSbLib.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_PCIE_FAMILY_LN_F12PCIEPHYSERVICES_FILECODE
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
AGESA_STATUS
PcieFmPreOscPifInitCallback (
  IN       PCIe_WRAPPER_CONFIG   *Wrapper,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  );

AGESA_STATUS
PcieFmPostOscPifInitCallback (
  IN       PCIe_WRAPPER_CONFIG   *Wrapper,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  );


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
STATIC
PcieFmSetPhyPersonality (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8                   Phy;
  UINT8                   Mode;
  if (Wrapper->WrapId == GFX_WRAP_ID || Wrapper->WrapId == DDI_WRAP_ID) {
    for (Phy = 0; Phy < Wrapper->NumberOfPIFs; Phy++) {
      if (Wrapper->WrapId == GFX_WRAP_ID) {
        Mode = (Phy == 0) ? 0x3 : 0x1;
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
 * PHY Pll Personality Init
 *
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  Buffer              Pointer to buffer
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
AGESA_STATUS
PcieFmPhyLetPllPersonalityInitCallback (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      VOID                  *Buffer,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieFmPhyLetPllPersonalityInitCallback Enter\n");
  PciePifPllPowerControl (PowerDownPifs, Wrapper, Pcie);
  PciePifSetPllRampTime (NormalRampup, Wrapper, Pcie);
  PciePollPifForCompeletion (Wrapper, Pcie);
  PcieTopologyLaneControl (
    DisableLanes,
    PcieUtilGetWrapperLaneBitMap (LANE_TYPE_CORE_ALL, LANE_TYPE_PCIE_SB_CORE_CONFIG, Wrapper),
    Wrapper,
    Pcie
    );
  PciePollPifForCompeletion (Wrapper, Pcie);
  PcieFmSetPhyPersonality (Wrapper, Pcie);
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
  PciePifPllPowerControl (PowerUpPifs, Wrapper, Pcie);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieFmPhyLetPllPersonalityInitCallback Exit\n");
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set PHY channel characteristic
 *
 *
 *
 * @param[in]  Engine              Pointer to engine configuration
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
VOID
PcieFmPhyChannelCharacteristic (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  //@todo
}

/*----------------------------------------------------------------------------------------*/
/**
 * Point "virtual" PLL clock picker away from PCIe
 *
 *
 *
 * @param[in]  Wrapper             Pointer to internal configuration data area
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
VOID
PcieFmAvertClockPickers (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PciePhyAvertClockPickers (Wrapper, Pcie);
}

/*----------------------------------------------------------------------------------------*/
/**
 * PHY lane ganging
 *
 *
 *
 * @param[out] Wrapper             Pointer to internal configuration data area
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
VOID
PcieFmPhyApplyGanging (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PciePhyApplyGanging (Wrapper, Pcie);
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

AGESA_STATUS
PcieFmForceDccRecalibrationCallback (
  IN       PCIe_WRAPPER_CONFIG   *Wrapper,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  if (Wrapper->WrapId != DDI_WRAP_ID) {
    PciePhyForceDccRecalibration (Wrapper, Pcie);
  }
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

AGESA_STATUS
PcieFmPreOscPifInitCallback (
  IN       PCIe_WRAPPER_CONFIG   *Wrapper,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  if (Wrapper->WrapId != DDI_WRAP_ID) {
    PciePifFullPowerStateControl (PowerDownPifs, Wrapper, Pcie);
    PcieTopologyLaneControl (
      DisableLanes,
      PcieUtilGetWrapperLaneBitMap (LANE_TYPE_CORE_ALL, LANE_TYPE_PCIE_SB_CORE_CONFIG, Wrapper),
      Wrapper,
      Pcie
      );
    PciePifSetPllRampTime (LongRampup, Wrapper, Pcie);
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
AGESA_STATUS
PcieFmPostOscPifInitCallback (
  IN       PCIe_WRAPPER_CONFIG   *Wrapper,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  if (Wrapper->WrapId != DDI_WRAP_ID) {
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
  }
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
PcieFmOscInitPhyForGen2 (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  F12_COMPLEX_CONFIG        *ComplexData;
  F12_PCIe_SILICON_CONFIG   *FmSilicon;
  D0F0xE4_WRAP_FFF1_STRUCT  D0F0xE4_WRAP_FFF1;
  AGESA_STATUS              Status;
  UINT8                     SaveSbLinkAspm;
  CPU_LOGICAL_ID            LogicalId;
  UINT32                    Value;

  Value = 0;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieFmOscInitPhyForGen2 Enter\n");
  ComplexData = (F12_COMPLEX_CONFIG *) PcieConfigGetChild (DESCRIPTOR_SILICON, &Pcie->Header);
  FmSilicon = &ComplexData->FmSilicon;
  GetLogicalIdOfCurrentCore (&LogicalId, GnbLibGetHeader (Pcie));

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
      if ((LogicalId.Revision & (AMD_F12_LN_A0 | AMD_F12_LN_A1)) != 0) {
        FmSilicon->OscMode = OscRO;
      } else {
        FmSilicon->OscMode = OscDefault;
      }
    }
    IDS_HDT_CONSOLE (GNB_TRACE, "  OSC Mode From Fuses - %s\n",
      (FmSilicon->OscMode == OscFuses) ? "Fuses" : (
      (FmSilicon->OscMode == OscRO) ? "RO" : (
      (FmSilicon->OscMode == OscLC) ? "LC" : (
      (FmSilicon->OscMode == OscDefault) ? "Skip" : "Unknown")))
      );
  }
  if (FmSilicon->OscMode != OscDefault) {
    //Gang SB pif/phy lanes
    PcieRegisterRMW (
      &ComplexData->GppWrapper,
      PIF_SPACE (ComplexData->GppWrapper.WrapId, 0, D0F0xE4_PIF_0011_ADDRESS),
      D0F0xE4_PIF_0011_MultiPif_MASK | D0F0xE4_PIF_0011_X4Lane30_MASK | D0F0xE4_PIF_0011_X4Lane74_MASK,
      (1 << D0F0xE4_PIF_0011_X4Lane30_OFFSET) | (1 << D0F0xE4_PIF_0011_X4Lane74_OFFSET),
      FALSE,
      Pcie
      );
    PcieConfigRunProcForAllWrappers (
      DESCRIPTOR_ALL_WRAPPERS,
      PcieFmPreOscPifInitCallback,
      NULL,
      Pcie
      );
    switch (FmSilicon->OscMode) {
    case  OscLC:
      PcieRegisterWriteField (
        &ComplexData->GppWrapper,
        PHY_SPACE (ComplexData->GppWrapper.WrapId, 0, D0F0xE4_PHY_2002_ADDRESS),
        D0F0xE4_PHY_2002_IsLc_OFFSET,
        D0F0xE4_PHY_2002_IsLc_WIDTH,
        0x1,
        FALSE,
        Pcie
        );
      break;
    case  OscRO:
      PcieRegisterWriteField (
        &ComplexData->GppWrapper,
        PHY_SPACE (ComplexData->GppWrapper.WrapId, 0, D0F0xE4_PHY_2002_ADDRESS),
        D0F0xE4_PHY_2002_RoCalEn_OFFSET,
        D0F0xE4_PHY_2002_RoCalEn_WIDTH,
        0x0,
        FALSE,
        Pcie
        );
      PcieRegisterWriteField (
        &ComplexData->GppWrapper,
        PHY_SPACE (ComplexData->GppWrapper.WrapId, 0, D0F0xE4_PHY_2002_ADDRESS),
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
    PcieConfigRunProcForAllWrappers (
      DESCRIPTOR_ALL_WRAPPERS,
      PcieFmForceDccRecalibrationCallback,
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
    // Use SMU service
    NbSmuRcuRegisterRead (
      SMUx0B_x85B0_ADDRESS,
      &Value,
      1,
      GnbLibGetHeader (Pcie)
      );
    Value = (Value & (~0xff)) | 60;
    NbSmuRcuRegisterWrite (
      SMUx0B_x85B0_ADDRESS,
      &Value,
      1,
      FALSE,
      GnbLibGetHeader (Pcie)
      );
    NbSmuServiceRequest (4, FALSE, GnbLibGetHeader (Pcie));
#endif
    ComplexData->Port8.Type.Port.PortData.LinkAspm = AspmDisabled;
    SbPcieLinkAspmControl (&ComplexData->Port8, Pcie);
    PciePollLinkForL0Exit (&ComplexData->Port8, Pcie);

    ComplexData->Port8.Type.Port.PortData.LinkAspm = SaveSbLinkAspm;

    PcieConfigRunProcForAllWrappers (
      DESCRIPTOR_ALL_WRAPPERS,
      PcieFmPostOscPifInitCallback,
      NULL,
      Pcie
      );
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieFmOscInitPhyForGen2 Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Program receiver detection power mode
 *
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */

VOID
PcieFmPifSetRxDetectPowerMode (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
   PciePifSetRxDetectPowerMode (Wrapper, Pcie);
}

/*----------------------------------------------------------------------------------------*/
/**
 * PHY lane parameter Init
 *
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  Buffer              Pointer to buffer
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
AGESA_STATUS
PcieFmPhyLaneInitInitCallback (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      VOID                  *Buffer,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8  Phy;
  UINT8  PhyLane;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieFmPhyLaneInitInitCallback Enter\n");
  for (Phy = 0; Phy < Wrapper->NumberOfPIFs; Phy++) {
    for (PhyLane = 0; PhyLane < MAX_NUM_LANE_PER_PHY; PhyLane++) {
      PcieRegisterRMW (
        Wrapper,
        PHY_SPACE (Wrapper->WrapId, Phy, D0F0xE4_PHY_400A_ADDRESS + PhyLane * 0x80),
        D0F0xE4_PHY_400A_BiasDisInLs2_MASK | D0F0xE4_PHY_400A_Ls2ExitTime_MASK,
        (1 << D0F0xE4_PHY_400A_BiasDisInLs2_OFFSET) | (1 << D0F0xE4_PHY_400A_Ls2ExitTime_OFFSET),
        FALSE,
        Pcie
        );
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieFmPhyLaneInitInitCallback Exit\n");
  return AGESA_SUCCESS;
}
