/* $NoKeywords:$ */
/**
 * @file
 *
 *  PCIe link training
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 85361 $   @e \$Date: 2013-01-07 11:15:28 -0600 (Mon, 07 Jan 2013) $
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
#include  "GeneralServices.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbPcieInitLibV5.h"
#include  "GnbPcieTrainingV2.h"
#include  "GnbRegistersCommonV2.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIETRAININGV2_PCIETRAININGV2_FILECODE
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

/*----------------------------------------------------------------------------------------*/
/**
 * Set link State
 *
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  State               State to set
 * @param[in]  UpdateTimeStamp     Update time stamp
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */
VOID
PcieTrainingSetPortState (
  IN      PCIe_ENGINE_CONFIG        *CurrentEngine,
  IN      PCIE_LINK_TRAINING_STATE  State,
  IN      BOOLEAN                   UpdateTimeStamp,
  IN      PCIe_PLATFORM_CONFIG      *Pcie
  )
{
  UINT32 TimeStamp;
  CurrentEngine->Type.Port.State = (UINT8) State;
  if (UpdateTimeStamp) {
    TimeStamp = PcieTimerGetTimeStamp (Pcie);
    CurrentEngine->Type.Port.TimeStamp = TimeStamp;
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Set state for all engines connected to same reset ID
 *
 *
 *
 * @param[in]       Engine              Pointer to engine config descriptor
 * @param[in, out]  Buffer              Pointer to Reset Id
 * @param[in]       Pcie                Pointer to global PCIe configuration
 *
 */
STATIC VOID
PcieSetResetStateOnEngines (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8 ResetId;
  ResetId = *(UINT8 *)Buffer;
  if (Engine->Type.Port.PortData.ResetId == ResetId && !PcieConfigIsSbPcieEngine (Engine)) {
    PcieTrainingSetPortState (Engine, LinkStateResetDuration, TRUE, Pcie);
    GnbLibPciRMW (
      Engine->Type.Port.Address.AddressValue | DxFxx68_ADDRESS,
      AccessWidth32,
      (UINT32) ~DxFxx68_LinkDis_MASK,
      1 << DxFxx68_LinkDis_OFFSET,
      GnbLibGetHeader (Pcie)
      );
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Assert GPIO port reset.
 *
 *  Transition to LinkStateResetDuration state
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */
VOID
STATIC
PcieTrainingAssertReset (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIe_SLOT_RESET_INFO ResetInfo;
  ResetInfo.ResetControl = AssertSlotReset;
  ResetInfo.ResetId = CurrentEngine->Type.Port.PortData.ResetId;
  LibAmdMemCopy (&ResetInfo.StdHeader, GnbLibGetHeader (Pcie), sizeof (AMD_CONFIG_PARAMS), GnbLibGetHeader (Pcie));
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PcieSetResetStateOnEngines,
    (VOID *)&CurrentEngine->Type.Port.PortData.ResetId,
    Pcie
    );
  AgesaPcieSlotResetControl (0, &ResetInfo);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Check for reset duration
 *
 *  Transition to LinkStateResetDuration state
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */
STATIC VOID
PcieTrainingCheckResetDuration (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32 TimeStamp;
  TimeStamp = PcieTimerGetTimeStamp (Pcie);
  if (TIMESTAMPS_DELTA (TimeStamp, CurrentEngine->Type.Port.TimeStamp) >= Pcie->LinkGpioResetAssertionTime) {
    PcieTrainingSetPortState (CurrentEngine, LinkStateResetExit, FALSE, Pcie);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Deassert GPIO port reset.
 *
 *  Transition to LinkStateResetDuration state
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Platform configuration
 *
 */
STATIC VOID
PcieTrainingDeassertReset (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIe_SLOT_RESET_INFO ResetInfo;
  ResetInfo.ResetControl = DeassertSlotReset;
  ResetInfo.ResetId = CurrentEngine->Type.Port.PortData.ResetId;
  LibAmdMemCopy (&ResetInfo.StdHeader, GnbLibGetHeader (Pcie), sizeof (AMD_CONFIG_PARAMS), GnbLibGetHeader (Pcie));
  AgesaPcieSlotResetControl (0, &ResetInfo);
  GnbLibPciRMW (
    CurrentEngine->Type.Port.Address.AddressValue | DxFxx68_ADDRESS,
    AccessWidth32,
    (UINT32) ~DxFxx68_LinkDis_MASK,
    0 << DxFxx68_LinkDis_OFFSET,
    GnbLibGetHeader (Pcie)
    );
  PcieTrainingSetPortState (CurrentEngine, LinkTrainingResetTimeout, TRUE, Pcie);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check for after reset deassertion timeout
 *
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */
VOID
STATIC
PcieTrainingCheckResetTimeout (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32 TimeStamp;
  TimeStamp = PcieTimerGetTimeStamp (Pcie);
  if (TIMESTAMPS_DELTA (TimeStamp, CurrentEngine->Type.Port.TimeStamp) >= Pcie->LinkResetToTrainingTime) {
    PcieTrainingSetPortState (CurrentEngine, LinkStateReleaseTraining, FALSE, Pcie);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Release training
 *
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */
VOID
STATIC
PcieTrainingRelease (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8   LinkTrainingState;
  PcieRegisterWriteField (
    PcieConfigGetParentWrapper (CurrentEngine),
    WRAP_SPACE (PcieConfigGetParentWrapper (CurrentEngine)->WrapId, D0F0xE4_WRAP_0800_ADDRESS + 0x100 * CurrentEngine->Type.Port.PortId),
    D0F0xE4_WRAP_0800_HoldTraining_OFFSET,
    D0F0xE4_WRAP_0800_HoldTraining_WIDTH,
    0,
    FALSE,
    Pcie
    );
  if (CurrentEngine->Type.Port.PortData.MiscControls.LinkComplianceMode == 0x1) {
    LinkTrainingState = LinkStateCompliance;
  } else {
    LinkTrainingState = LinkStateDetectPresence;
  }
  PcieTrainingSetPortState (CurrentEngine, LinkTrainingState, TRUE, Pcie);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Detect presence of any EP on the link
 *
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */

VOID
STATIC
PcieTrainingDetectPresence (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8   LinkHwStateHistory[4];
  UINT32  TimeStamp;
  PcieUtilGetLinkHwStateHistory (CurrentEngine, &LinkHwStateHistory[0], 4, Pcie);
  if (LinkHwStateHistory[0] > 4) {
    PcieTrainingSetPortState (CurrentEngine, LinkStateDetecting, TRUE, Pcie);
    return;
  }
  TimeStamp = PcieTimerGetTimeStamp (Pcie);
  if (TIMESTAMPS_DELTA (TimeStamp, CurrentEngine->Type.Port.TimeStamp) >= Pcie->LinkReceiverDetectionPooling) {
    PcieTrainingSetPortState (CurrentEngine, LinkStateDeviceNotPresent, FALSE, Pcie);
  }
}

CONST UINT8 FailPattern1 [] = {0x2a, 0x6};
CONST UINT8 FailPattern2 [] = {0x2a, 0x9};
CONST UINT8 FailPattern3 [] = {0x2a, 0xb};

/*----------------------------------------------------------------------------------------*/
/**
 * Detect Link State
 *
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */

VOID
STATIC
PcieTrainingDetectLinkState (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8   LinkHwStateHistory[16];
  UINT32  TimeStamp;
  UINT8   LinkTrainingState;
  PcieUtilGetLinkHwStateHistory (CurrentEngine, &LinkHwStateHistory[0], 4, Pcie);
  if (LinkHwStateHistory[0] == 0x10) {
    PcieTrainingSetPortState (CurrentEngine, LinkStateL0, FALSE, Pcie);
    return;
  };
  TimeStamp = PcieTimerGetTimeStamp (Pcie);
  if (TIMESTAMPS_DELTA (TimeStamp, CurrentEngine->Type.Port.TimeStamp) >= Pcie->LinkL0Pooling) {
    LinkTrainingState = LinkStateTrainingFail;
    PcieUtilGetLinkHwStateHistory (CurrentEngine, &LinkHwStateHistory[0], 16, Pcie);
    if (LinkHwStateHistory[0] == 0x7) {
      LinkTrainingState = LinkStateCompliance;
    } else if (PcieUtilSearchArray (LinkHwStateHistory, sizeof (LinkHwStateHistory), FailPattern1, sizeof (FailPattern1))) {
      LinkTrainingState = LinkStateBrokenLane;
    } else if (PcieUtilSearchArray (LinkHwStateHistory, sizeof (LinkHwStateHistory), FailPattern2, sizeof (FailPattern2))) {
      LinkTrainingState = LinkStateGen2Fail;
    } else if (PcieUtilSearchArray (LinkHwStateHistory, sizeof (LinkHwStateHistory), FailPattern3, sizeof (FailPattern3))) {
      LinkTrainingState = LinkStateGen2Fail;
    }
    PcieTrainingSetPortState (CurrentEngine, LinkTrainingState, FALSE, Pcie);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Broken Lane
 *
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */

STATIC VOID
PcieTrainingBrokenLineV2 (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8 CurrentLinkWidth;
  UINT8 LinkTrainingState;
  CurrentLinkWidth = PcieUtilGetLinkWidth (CurrentEngine, Pcie);
  if (CurrentLinkWidth < PcieConfigGetNumberOfPhyLane (CurrentEngine) && CurrentLinkWidth > 0) {
    CurrentEngine->InitStatus |= INIT_STATUS_PCIE_PORT_BROKEN_LANE_RECOVERY;
    PcieTopologyReduceLinkWidthV5 (CurrentLinkWidth, CurrentEngine, Pcie);
    LinkTrainingState = LinkStateResetAssert;
    PutEventLog (
      AGESA_WARNING,
      GNB_EVENT_BROKEN_LANE_RECOVERY,
      CurrentEngine->Type.Port.Address.AddressValue,
      0,
      0,
      0,
      GnbLibGetHeader (Pcie)
      );
  } else {
    LinkTrainingState = LinkStateGen2Fail;
  }
  PcieTrainingSetPortState (CurrentEngine, LinkTrainingState, FALSE, Pcie);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check if link fail because device does not support Gen2
 *
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */
STATIC VOID
PcieTrainingGen2Fail (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8 LinkTrainingState;
  if (CurrentEngine->Type.Port.PortData.MiscControls.LinkSafeMode != PcieGen1) {
    PcieConfigUpdatePortStatus (CurrentEngine, INIT_STATUS_PCIE_PORT_GEN2_RECOVERY, 0);
    CurrentEngine->Type.Port.PortData.MiscControls.LinkSafeMode = PcieGen1;
    PcieLinkSafeMode (CurrentEngine, Pcie);
    LinkTrainingState = LinkStateResetAssert;
    PutEventLog (
      AGESA_WARNING,
      GNB_EVENT_BROKEN_LANE_RECOVERY,
      CurrentEngine->Type.Port.Address.AddressValue,
      0,
      0,
      0,
      GnbLibGetHeader (Pcie)
      );
  } else {
    LinkTrainingState = LinkStateTrainingFail;
  }
  PcieTrainingSetPortState (CurrentEngine, LinkTrainingState, FALSE, Pcie);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Link in L0
 *
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */
VOID
STATIC
PcieCheckLinkL0 (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PcieTrainingSetPortState (CurrentEngine, LinkStateVcoNegotiation, TRUE, Pcie);
}
/*----------------------------------------------------------------------------------------*/
/**
 * Check if link fail because device does not support Gen X
 *
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */
VOID
STATIC
PcieTrainingCheckVcoNegotiation (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32          TimeStamp;
  DxFxx128_STRUCT DxFxx128;
  TimeStamp = PcieTimerGetTimeStamp (Pcie);
  GnbLibPciRead (CurrentEngine->Type.Port.Address.AddressValue | DxFxx128_ADDRESS, AccessWidth32, &DxFxx128, GnbLibGetHeader (Pcie));
  if (DxFxx128.Field.VcNegotiationPending == 0) {
    UINT16  NumberOfPhyLane;
    NumberOfPhyLane = PcieConfigGetNumberOfPhyLane (CurrentEngine);
    if (Pcie->GfxCardWorkaround == GfxWorkaroundEnable && NumberOfPhyLane >= 8) {
      // Limit exposure of workaround to x8 and x16 port.
      PcieTrainingSetPortState (CurrentEngine, LinkStateGfxWorkaround, TRUE, Pcie);
    } else {
      PcieTrainingSetPortState (CurrentEngine, LinkStateTrainingSuccess, FALSE, Pcie);
    }
    return;
  }
  if (TIMESTAMPS_DELTA (TimeStamp, CurrentEngine->Type.Port.TimeStamp) >= 1000 * 1000) {
    PcieTrainingSetPortState (CurrentEngine, LinkStateRetrain, FALSE, Pcie);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check if for GFX workaround condition
 *
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */
VOID
STATIC
PcieTrainingGfxWorkaround (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32                TimeStamp;
  GFX_WORKAROUND_STATUS GfxWorkaroundStatus;
  TimeStamp = PcieTimerGetTimeStamp (Pcie);

  GfxWorkaroundStatus = PcieGfxCardWorkaroundV2 (CurrentEngine->Type.Port.Address, GnbLibGetHeader (Pcie));
  switch (GfxWorkaroundStatus) {
  case GFX_WORKAROUND_DEVICE_NOT_READY:
    if (TIMESTAMPS_DELTA (TimeStamp, CurrentEngine->Type.Port.TimeStamp) >= (3 * 1000000)) {
      PcieTrainingSetPortState (CurrentEngine, LinkStateTrainingFail, TRUE, Pcie);
    }
    break;
  case GFX_WORKAROUND_SUCCESS:
    PcieTrainingSetPortState (CurrentEngine, LinkStateTrainingSuccess, FALSE, Pcie);
    break;
  case GFX_WORKAROUND_RESET_DEVICE:
    if (CurrentEngine->Type.Port.GfxWrkRetryCount < 5) {
      CurrentEngine->Type.Port.GfxWrkRetryCount++;
      PcieTrainingSetPortState (CurrentEngine, LinkStateResetAssert, TRUE, Pcie);
    } else {
      PcieTrainingSetPortState (CurrentEngine, LinkStateTrainingFail, TRUE, Pcie);
    }
    break;
  default:
    ASSERT (FALSE);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Retrain link
 *
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */
VOID
STATIC
PcieTrainingRetrainLink (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  GnbLibPciRMW (
    CurrentEngine->Type.Port.Address.AddressValue | DxFxx68_ADDRESS,
    AccessWidth32,
    (UINT32) ~DxFxx68_RetrainLink_MASK,
    1 << DxFxx68_RetrainLink_OFFSET,
    GnbLibGetHeader (Pcie)
    );
  PcieTrainingSetPortState (CurrentEngine, LinkStateDetecting, TRUE, Pcie);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Training fail on this port
 *
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */
VOID
STATIC
PcieTrainingFail (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PcieConfigUpdatePortStatus (CurrentEngine, INIT_STATUS_PCIE_PORT_TRAINING_FAIL, 0);
  PcieTrainingSetPortState (CurrentEngine, LinkStateDeviceNotPresent, FALSE, Pcie);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Links training success
 *
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */

VOID
STATIC
PcieTrainingSuccess (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PcieConfigUpdatePortStatus (CurrentEngine, INIT_STATUS_PCIE_TRAINING_SUCCESS, 0);
  PcieTrainingSetPortState (CurrentEngine, LinkStateTrainingCompleted, FALSE, Pcie);
  GnbLibPciRMW (
    CurrentEngine->Type.Port.Address.AddressValue | DxFxx68_ADDRESS,
    AccessWidth32,
    (UINT32) ~DxFxx68_LinkBWManagementStatus_MASK,
    1 << DxFxx68_LinkBWManagementStatus_OFFSET,
    GnbLibGetHeader (Pcie)
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Links in compliance
 *
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */
VOID
STATIC
PcieTrainingCompliance (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PcieConfigUpdatePortStatus (CurrentEngine,  INIT_STATUS_PCIE_PORT_IN_COMPLIANCE, 0);
  PcieTrainingSetPortState (CurrentEngine, LinkStateTrainingCompleted, FALSE, Pcie);
}

/*----------------------------------------------------------------------------------------*/
/**
 * PCie EP not present
 *
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */
VOID
STATIC
PcieTrainingNotPresent (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  if ((CurrentEngine->Type.Port.PortData.LinkHotplug == HotplugEnhanced) || (CurrentEngine->Type.Port.PortData.LinkHotplug == HotplugServer)) {
  } else {
    PcieRegisterWriteField (
      PcieConfigGetParentWrapper (CurrentEngine),
      WRAP_SPACE (PcieConfigGetParentWrapper (CurrentEngine)->WrapId, D0F0xE4_WRAP_0800_ADDRESS + 0x100 * CurrentEngine->Type.Port.PortId),
      D0F0xE4_WRAP_0800_HoldTraining_OFFSET,
      D0F0xE4_WRAP_0800_HoldTraining_WIDTH,
      1,
      FALSE,
      Pcie
      );
  }
  PcieTrainingSetPortState (CurrentEngine, LinkStateTrainingCompleted, FALSE, Pcie);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Final state. Port training completed.
 *
 * Initialization status recorded in PCIe_ENGINE_CONFIG.InitStatus
 *
 * @param[in]  CurrentEngine       Pointer to engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 *
 */
VOID
STATIC
PcieTrainingCompleted (
  IN      PCIe_ENGINE_CONFIG    *CurrentEngine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
}

/*----------------------------------------------------------------------------------------*/
/**
 * Training state handling
 *
 *
 *
 * @param[in]       Engine              Pointer to engine config descriptor
 * @param[in, out]  Buffer              Indicate if engine in non final state
 * @param[in]       Pcie                Pointer to global PCIe configuration
 *
 */

VOID
STATIC
PcieTrainingPortCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  BOOLEAN *TrainingComplete;
  TrainingComplete = (BOOLEAN *) Buffer;
  if (Engine->Type.Port.State < Pcie->TrainingExitState) {
    *TrainingComplete = FALSE;
  } else {
    return;
  }
  switch (Engine->Type.Port.State) {
  case LinkStateResetAssert:
    PcieTrainingAssertReset (Engine, Pcie);
    break;
  case LinkStateResetDuration:
    PcieTrainingCheckResetDuration (Engine, Pcie);
    break;
  case LinkStateResetExit:
    PcieTrainingDeassertReset (Engine, Pcie);
    break;
  case LinkTrainingResetTimeout:
    PcieTrainingCheckResetTimeout (Engine, Pcie);
    break;
  case LinkStateReleaseTraining:
    PcieTrainingRelease (Engine, Pcie);
    break;
  case LinkStateDetectPresence:
    PcieTrainingDetectPresence (Engine, Pcie);
    break;
  case LinkStateDetecting:
    PcieTrainingDetectLinkState (Engine, Pcie);
    break;
  case LinkStateBrokenLane:
    PcieTrainingBrokenLineV2 (Engine, Pcie);
    break;
  case LinkStateGen2Fail:
    PcieTrainingGen2Fail (Engine, Pcie);
    break;
  case LinkStateL0:
    PcieCheckLinkL0 (Engine, Pcie);
    break;
  case LinkStateVcoNegotiation:
    PcieTrainingCheckVcoNegotiation (Engine, Pcie);
    break;
  case LinkStateRetrain:
    PcieTrainingRetrainLink (Engine, Pcie);
    break;
  case LinkStateTrainingFail:
    PcieTrainingFail (Engine, Pcie);
    break;
  case LinkStateGfxWorkaround:
    PcieTrainingGfxWorkaround (Engine, Pcie);
    break;
  case LinkStateTrainingSuccess:
    PcieTrainingSuccess (Engine, Pcie);
    break;
  case LinkStateCompliance:
    PcieTrainingCompliance (Engine, Pcie);
    break;
  case LinkStateDeviceNotPresent:
    PcieTrainingNotPresent (Engine, Pcie);
    break;
  case LinkStateTrainingCompleted:
    PcieTrainingCompleted (Engine, Pcie);
    break;
  default:
    break;
  }

}


/*----------------------------------------------------------------------------------------*/
/**
 * Main link training procedure
 *
 *  Port end up in three possible state  LinkStateTrainingNotPresent/LinkStateCompliance/
 *                                       LinkStateTrainingSuccess
 *
 * @param[in]   Pcie                Pointer to global PCIe configuration
 * @retval      AGESA_STATUS
 *
 */

AGESA_STATUS
PcieTraining (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS  Status;
  BOOLEAN       TrainingComplete;

  IDS_PERF_TIMESTAMP (TP_BEGINGNBPCIETRAINING, GnbLibGetHeader (Pcie));
  Status = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieTraining Enter\n");
  do {
    TrainingComplete = TRUE;
    PcieConfigRunProcForAllEngines (
      DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
      PcieTrainingPortCallback,
      &TrainingComplete,
      Pcie
      );
  } while (!TrainingComplete);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieTraining Exit [%x]\n", Status);
  IDS_PERF_TIMESTAMP (TP_ENDGNBPCIETRAINING, GnbLibGetHeader (Pcie));
  return Status;
}
