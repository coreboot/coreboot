/* $NoKeywords:$ */
/**
 * @file
 *
 * KB specific PCIe services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 87645 $   @e \$Date: 2013-02-06 13:08:17 -0600 (Wed, 06 Feb 2013) $
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
#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbSbLib.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbPcieInitLibV5.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieTrainingV2.h"
#include  "GnbNbInitLibV4.h"
#include  "GnbNbInitLibV1.h"
#include  "GnbNbInitLibV5.h"
#include  "PcieComplexDataKB.h"
#include  "PcieLibKB.h"
#include  "GnbRegistersKB.h"
#include  "GnbRegisterAccKB.h"
#include  "GnbF1Table.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITKB_PCIELIBKB_FILECODE
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
PcieGetLinkSpeedCapKB (
  IN       UINT32                Flags,
  IN       PCIe_ENGINE_CONFIG    *Engine
  );

UINT32
GnbTimeStampKB (
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

UINT8
PcieMaxPayloadKB (
  IN       PCIe_ENGINE_CONFIG    *Engine
  );
/*----------------------------------------------------------------------------------------*/
/**
 * PLL powerdown
 *
 *
 * @param[in]  Wrapper     Pointer to Wrapper config descriptor
 * @param[in]  Pcie        Pointer to PICe configuration data area
 */

VOID
PciePifPllConfigureKB (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  D0F0xE4_PIF_0012_STRUCT D0F0xE4_PIF_0012;

  IDS_HDT_CONSOLE (GNB_TRACE, "PciePifPllConfigureKB Enter\n");
  D0F0xE4_PIF_0012.Value = PcieRegisterRead (
                             Wrapper,
                             PIF_SPACE (Wrapper->WrapId, 0, D0F0xE4_PIF_0012_ADDRESS),
                             Pcie
                             );

  D0F0xE4_PIF_0012.Field.PllRampUpTime = 0x0;

  if (Wrapper->Features.PowerOffUnusedPlls != 0) {
    D0F0xE4_PIF_0012.Field.PllPowerStateInOff = PifPowerStateOff;
    D0F0xE4_PIF_0012.Field.PllPowerStateInTxs2 = PifPowerStateOff;
  } else {
    D0F0xE4_PIF_0012.Field.PllPowerStateInOff = PifPowerStateL0;
    D0F0xE4_PIF_0012.Field.PllPowerStateInTxs2 = PifPowerStateL0;
  }

  if (Wrapper->Features.PllOffInL1 != 0) {
    D0F0xE4_PIF_0012.Field.TxPowerStateInTxs2 = PifPowerStateLS2;
    D0F0xE4_PIF_0012.Field.RxPowerStateInRxs2 = PifPowerStateLS2;
  } else {
    D0F0xE4_PIF_0012.Field.TxPowerStateInTxs2 = PifPowerStateL0;
    D0F0xE4_PIF_0012.Field.RxPowerStateInRxs2 = PifPowerStateL0;
  }

  PcieRegisterWrite (
    Wrapper,
    PIF_SPACE (Wrapper->WrapId, 0, D0F0xE4_PIF_0012_ADDRESS),
    D0F0xE4_PIF_0012.Value,
    TRUE,
    Pcie
  );
  PcieRegisterWrite (
    Wrapper,
    PIF_SPACE (Wrapper->WrapId, 0, D0F0xE4_PIF_0012_ADDRESS + 1),
    D0F0xE4_PIF_0012.Value,
    TRUE,
    Pcie
  );
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePifPllConfigureKB Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Power down unused lanes and plls
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */

VOID
PciePwrPowerDownUnusedLanesKB (
  IN       PCIe_WRAPPER_CONFIG    *Wrapper,
  IN       PCIe_PLATFORM_CONFIG   *Pcie
  )
{
  UINT32              UnusedLanes;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrPowerDownUnusedLanesKB Enter\n");
  if (Wrapper->Features.PowerOffUnusedLanes != 0) {
    UnusedLanes = PcieUtilGetWrapperLaneBitMap (LANE_TYPE_CORE_ALL, LANE_TYPE_PCIE_CORE_ALLOC_ACTIVE, Wrapper);
    PcieTopologyLaneControlV5 (
      DisableLanes,
      UnusedLanes,
      Wrapper,
      Pcie
      );
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrPowerDownUnusedLanesKB Exit\n");
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
PcieSetVoltageKB (
  IN      PCIE_LINK_SPEED_CAP   LinkCap,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8                      TargetVid;
  UINT8                      MinVidIndex;
  UINT8                      PP_FUSE_ARRAY_V2_fld32[5];
  UINT8                      Index;
  PP_F1_ARRAY_V2           *PpF1Array;
  UINT32                     Millivolt;
  UINT32   D0F0xBC_xC0104007;
  UINT32   D0F0xBC_xC0104008;
  UINT32   D0F0xBC_xC010407C;
  UINT32   D0F0xBC_xC0107064;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieSetVoltageKB Enter\n");
  PpF1Array = GnbLocateHeapBuffer (AMD_PP_F1_TABLE_HANDLE, GnbLibGetHeader (Pcie));
  if (PpF1Array == NULL) {
    GnbRegisterReadKB (GnbGetHandle (GnbLibGetHeader (Pcie)), 0x4, 0xC0104007, &D0F0xBC_xC0104007, 0, GnbLibGetHeader (Pcie));
    GnbRegisterReadKB (GnbGetHandle (GnbLibGetHeader (Pcie)), 0x4, 0xC0104008, &D0F0xBC_xC0104008, 0, GnbLibGetHeader (Pcie));
    GnbRegisterReadKB (GnbGetHandle (GnbLibGetHeader (Pcie)), 0x4, 0xC010407C, &D0F0xBC_xC010407C, 0, GnbLibGetHeader (Pcie));
    GnbRegisterReadKB (GnbGetHandle (GnbLibGetHeader (Pcie)), 0x4, 0xC0107064, &D0F0xBC_xC0107064, 0, GnbLibGetHeader (Pcie));
    PP_FUSE_ARRAY_V2_fld32[0] = (UINT8) ((D0F0xBC_xC0104007 >> 5) & 0xFF);
    PP_FUSE_ARRAY_V2_fld32[1] = (UINT8) ((D0F0xBC_xC0104008 >> 5) & 0xFF);
    PP_FUSE_ARRAY_V2_fld32[2] = (UINT8) ((D0F0xBC_xC0104008 >> 13) & 0xFF);
    PP_FUSE_ARRAY_V2_fld32[3] = (UINT8) ((D0F0xBC_xC0104008 >> 21) & 0xFF);
    PP_FUSE_ARRAY_V2_fld32[4] = (UINT8) ((D0F0xBC_xC010407C >> 20) & 0xFF);
    Index = (UINT8) ((D0F0xBC_xC0107064 >> 11) & 7);
  } else {
    PP_FUSE_ARRAY_V2_fld32[0] = PpF1Array->PP_FUSE_ARRAY_V2_fld32[0];
    PP_FUSE_ARRAY_V2_fld32[1] = PpF1Array->PP_FUSE_ARRAY_V2_fld32[1];
    PP_FUSE_ARRAY_V2_fld32[2] = PpF1Array->PP_FUSE_ARRAY_V2_fld32[2];
    PP_FUSE_ARRAY_V2_fld32[3] = PpF1Array->PP_FUSE_ARRAY_V2_fld32[3];
    PP_FUSE_ARRAY_V2_fld32[4] = PpF1Array->PP_FUSE_ARRAY_V2_fld32[4];
    Index = PpF1Array->PcieGen2Vid;
  }
  if (LinkCap > PcieGen1) {
    ASSERT (PP_FUSE_ARRAY_V2_fld32[Index] != 0);
    TargetVid = PP_FUSE_ARRAY_V2_fld32[Index];
  } else {

    MinVidIndex = 0;
    for (Index = 0; Index < 5; Index++) {
      if (PP_FUSE_ARRAY_V2_fld32[Index] > PP_FUSE_ARRAY_V2_fld32[MinVidIndex]) {
        MinVidIndex = (UINT8) Index;
      }
    }
    ASSERT (PP_FUSE_ARRAY_V2_fld32[MinVidIndex] != 0);
    TargetVid = PP_FUSE_ARRAY_V2_fld32[MinVidIndex];
  }

  IDS_HDT_CONSOLE (PCIE_MISC, "  Set Voltage for Gen %d, Vid code %d\n", LinkCap, TargetVid);
  Millivolt = GnbTranslateVidCodeToMillivoltV5 (TargetVid, GnbLibGetHeader (Pcie)) * 4 / 100;
  GnbRegisterWriteKB (GnbGetHandle (GnbLibGetHeader (Pcie)), TYPE_SMU_MSG, SMC_MSG_VDDNB_REQUEST, &Millivolt, 0, GnbLibGetHeader (Pcie));
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieSetVoltageKB Exit\n");
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
PciePifGetPllPowerUpLatencyKB (
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
PcieGetLinkSpeedCapKB (
  IN       UINT32                Flags,
  IN       PCIe_ENGINE_CONFIG    *Engine
  )
{
  PCIE_LINK_SPEED_CAP   LinkSpeedCapability;
  PCIe_WRAPPER_CONFIG   *Wrapper;
  PCIe_PLATFORM_CONFIG  *Pcie;

  Wrapper = PcieConfigGetParentWrapper (Engine);
  Pcie = PcieConfigGetPlatform (Wrapper);

  LinkSpeedCapability = PcieGen2;

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

    if (( Pcie->PsppPolicy == PsppBalanceLow ||
          Engine->Type.Port.PortData.MiscControls.LinkSafeMode == PcieGen1)
        && !PcieConfigIsSbPcieEngine (Engine)) {

      LinkSpeedCapability = PcieGen1;
    }
  }
  return LinkSpeedCapability;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Family specific time stamp function
 *
 *
 * @param[in]  StdHeader       Standard configuration header
 * @retval                     Count
 */
UINT32
GnbTimeStampKB (
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  UINT32 TimeStamp;

  TimeStamp = 0;

  GnbLibPciIndirectRead (
    MAKE_SBDFO (0, 0, 0, 0, 0xE0),
    0x13080F0,
    AccessWidth32,
    &TimeStamp,
    StdHeader
    );

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbTsKb: %08x\n", TimeStamp);
  return TimeStamp;

}

/*----------------------------------------------------------------------------------------*/
/**
 * Limit MaxPayload to 256 for x1 ports
 *
 *
 *
 * @param[in]  Engine             Pointer to engine config descriptor
 * @retval     MaxPayload         MaxPayloadSupport
 */
UINT8
PcieMaxPayloadKB (
  IN       PCIe_ENGINE_CONFIG    *Engine
  )
{
  UINT8    MaxPayload;

  MaxPayload = MAX_PAYLOAD_512;
  if (Engine->EngineData.StartLane == Engine->EngineData.EndLane) {
    MaxPayload = MAX_PAYLOAD_256;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMaxPayloadKB Exit with MaxPayload = %d for StartLane = %d and EndLane = %d\n", MaxPayload, Engine->EngineData.StartLane, Engine->EngineData.EndLane);
  return MaxPayload;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Select master PLL
 *
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[out] ConfigChanged       Pointer to boolean indicator that configuration was changed
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */

VOID
PcieTopologySelectMasterPllKB (
  IN       PCIe_WRAPPER_CONFIG   *Wrapper,
     OUT   BOOLEAN               *ConfigChanged,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIe_ENGINE_CONFIG        *EngineList;
  UINT16                    MasterLane;
  UINT16                    MasterHotplugLane;
  UINT16                    EngineMasterLane;
  D0F0xE4_WRAP_8013_STRUCT  D0F0xE4_WRAP_8013;
  D0F0xE4_WRAP_8013_STRUCT  D0F0xE4_WRAP_8013_BASE;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieTopologySelectMasterPll Enter\n");
  MasterLane = 0xFFFF;
  MasterHotplugLane = 0xFFFF;
  EngineList = PcieConfigGetChildEngine (Wrapper);
  while (EngineList != NULL) {
    if (PcieConfigIsEngineAllocated (EngineList) && EngineList->Type.Port.PortData.PortPresent != PortDisabled && PcieConfigIsPcieEngine (EngineList)) {
      EngineMasterLane = PcieConfigGetPcieEngineMasterLane (EngineList);
      if (EngineList->Type.Port.PortData.LinkHotplug != HotplugDisabled) {
        MasterHotplugLane = (EngineMasterLane < MasterHotplugLane) ? EngineMasterLane : MasterHotplugLane;
      } else {
        MasterLane = (EngineMasterLane < MasterLane) ? EngineMasterLane : MasterLane;
        if (PcieConfigIsSbPcieEngine (EngineList)) {
          break;
        }
      }
    }
    EngineList = PcieLibGetNextDescriptor (EngineList);
  }

  if (MasterLane == 0xffff) {
    if (MasterHotplugLane != 0xffff) {
      MasterLane = MasterHotplugLane;
    } else {
      MasterLane = 0x0;
    }
  }

  D0F0xE4_WRAP_8013.Value = PcieRegisterRead (
                              Wrapper,
                              WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8013_ADDRESS),
                              Pcie
                              );
  D0F0xE4_WRAP_8013_BASE.Value = D0F0xE4_WRAP_8013.Value;

  if (MasterLane <= 3 ) {
    Wrapper->MasterPll = GNB_PCIE_MASTERPLL_A;
  } else {
    Wrapper->MasterPll = GNB_PCIE_MASTERPLL_B;
  }

  IDS_OPTION_HOOK (IDS_GNB_PCIE_MASTERPLL_SELECTION, &(Wrapper->MasterPll), GnbLibGetHeader (Pcie));

  if (Wrapper->MasterPll == GNB_PCIE_MASTERPLL_A) {
    D0F0xE4_WRAP_8013.Field.MasterPciePllA = 0x1;
    D0F0xE4_WRAP_8013.Field.MasterPciePllB = 0x0;
  } else {
    D0F0xE4_WRAP_8013.Field.MasterPciePllA = 0x0;
    D0F0xE4_WRAP_8013.Field.MasterPciePllB = 0x1;
  }

  if (ConfigChanged != NULL) {
    *ConfigChanged = (D0F0xE4_WRAP_8013.Value == D0F0xE4_WRAP_8013_BASE.Value) ? FALSE : TRUE;
  }
  PcieRegisterWrite (
    Wrapper,
    WRAP_SPACE (Wrapper->WrapId, D0F0xE4_WRAP_8013_ADDRESS),
    D0F0xE4_WRAP_8013.Value,
    FALSE,
    Pcie
    );

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieTopologySelectMasterPll Exit\n");
}

