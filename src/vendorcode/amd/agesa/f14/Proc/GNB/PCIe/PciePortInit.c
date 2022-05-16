/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe port initialization service procedure
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 44325 $   @e \$Date: 2010-12-22 03:29:53 -0700 (Wed, 22 Dec 2010) $
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
#include  "Ids.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbPcieFamServices.h"
#include  "PcieFamilyServices.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  GNB_MODULE_DEFINITIONS (GnbPcieInitLibV1)
#include  GNB_MODULE_DEFINITIONS (GnbPcieConfig)
#include  GNB_MODULE_DEFINITIONS (GnbPcieTrainingV1)
#include  "PciePortInit.h"
#include  "GnbRegistersON.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_PCIE_PCIEPORTINIT_FILECODE
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

CONST PCIE_PORT_REGISTER_ENTRY PortInitTable [] = {
  {
    DxF0xE4_x02_ADDRESS,
    DxF0xE4_x02_RegsLcAllowTxL1Control_MASK,
    (0x1 << DxF0xE4_x02_RegsLcAllowTxL1Control_OFFSET)
  },
  {
    DxF0xE4_x70_ADDRESS,
    DxF0xE4_x70_RxRcbCplTimeoutMode_MASK,
    (0x1 << DxF0xE4_x70_RxRcbCplTimeoutMode_OFFSET)
  },
  {
    DxF0xE4_xA0_ADDRESS,
    DxF0xE4_xA0_Lc16xClearTxPipe_MASK | DxF0xE4_xA0_LcL1ImmediateAck_MASK,
    (0x3 << DxF0xE4_xA0_Lc16xClearTxPipe_OFFSET) |
    (0x1 << DxF0xE4_xA0_LcL1ImmediateAck_OFFSET)
  },
  {
    DxF0xE4_xA1_ADDRESS,
    DxF0xE4_xA1_LcDontGotoL0sifL1Armed_MASK,
    (0x1 << DxF0xE4_xA1_LcDontGotoL0sifL1Armed_OFFSET)
  },
  {
    DxF0xE4_xA2_ADDRESS,
    DxF0xE4_xA2_LcRenegotiateEn_MASK | DxF0xE4_xA2_LcUpconfigureSupport_MASK,
    (0x1 << DxF0xE4_xA2_LcRenegotiateEn_OFFSET) |
    (0x1 << DxF0xE4_xA2_LcUpconfigureSupport_OFFSET)
  },
  {
    DxF0xE4_xA3_ADDRESS,
    DxF0xE4_xA3_LcXmitFtsBeforeRecovery_MASK,
    (0x1 << DxF0xE4_xA3_LcXmitFtsBeforeRecovery_OFFSET)
  },
  {
    DxF0xE4_xB1_ADDRESS,
    DxF0xE4_xB1_LcDeassertRxEnInL0s_MASK | DxF0xE4_xB1_LcBlockElIdleinL0_MASK,
    (0x1 << DxF0xE4_xB1_LcDeassertRxEnInL0s_OFFSET) |
    (0x1 << DxF0xE4_xB1_LcBlockElIdleinL0_OFFSET)
  }
};


/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init various features on all active ports
 *
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 * @param[in]       Pcie            Pointer to global PCIe configuration
 *
 */

VOID
STATIC
PciePortInitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  ASSERT (Engine->EngineData.EngineType == PciePortEngine);
  PciePortProgramRegisterTable (PortInitTable, (sizeof (PortInitTable) / sizeof (PCIE_PORT_REGISTER_ENTRY)), Engine, FALSE, Pcie);
  PcieSetLinkSpeedCap (PcieGen1, Engine, Pcie);
  PcieSetLinkWidthCap (Engine, Pcie);
  PcieCompletionTimeout (Engine, Pcie);
  PcieLinkSetSlotCap (Engine, Pcie);
  PcieLinkInitHotplug (Engine, Pcie);
  PcieFmPhyChannelCharacteristic (Engine, Pcie);
  if (Engine->Type.Port.PortData.MiscControls.LinkSafeMode == PcieGen1) {
    PcieLinkSafeMode (Engine, Pcie);
  }
  if (Engine->Type.Port.PortData.PortPresent == PortDisabled) {
    ASSERT (Engine->Type.Port.IsSB == FALSE);
    PcieTrainingSetPortState (Engine, LinkStateDeviceNotPresent, FALSE, Pcie);
  }
  // Train port that forced to compliance in last stage of training
  if (Engine->Type.Port.PortData.MiscControls.LinkComplianceMode == 0x1) {
    PcieTrainingSetPortState (Engine, LinkStateTrainingCompleted, FALSE, Pcie);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Master procedure to init various features on all active ports
 *
 *
 *
 *
 * @param[in]   Pcie            Pointer to global PCIe configuration
 * @retval      AGESA_STATUS
 *
 */

AGESA_STATUS
PciePortInit (
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS  Status;
  Status = AGESA_SUCCESS;
  // Leave all device in Presence Detect Presence state for distributed training will be completed at PciePortPostEarlyInit
  if (Pcie->TrainingAlgorithm == PcieTrainingDistributed) {
    Pcie->TrainingExitState = LinkStateResetExit;
  }
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PciePortInitCallback,
    NULL,
    Pcie
    );
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init various features on all ports
 *
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 * @param[in]       Pcie            Pointer to global PCIe configuration
 *
 */

VOID
STATIC
PciePortPostInitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIE_LINK_SPEED_CAP LinkSpeedCapability;
  ASSERT (Engine->EngineData.EngineType == PciePortEngine);
  if (Engine->Type.Port.PortData.MiscControls.LinkSafeMode == PcieGen1) {
    PcieLinkSafeMode (Engine, Pcie);
  }
  LinkSpeedCapability = PcieFmGetLinkSpeedCap (PCIE_PORT_GEN_CAP_BOOT, Engine, Pcie);
  PcieSetLinkSpeedCap (LinkSpeedCapability, Engine, Pcie);
  // Retrain only present port to Gen2
  if (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS) && (LinkSpeedCapability > PcieGen1) && !Engine->Type.Port.IsSB) {
    PcieTrainingSetPortState (Engine, LinkStateRetrain, FALSE, Pcie);
    PcieConfigUpdatePortStatus (Engine, 0, INIT_STATUS_PCIE_TRAINING_SUCCESS);
  }
  // Train ports forced to compliance
  if (Engine->Type.Port.PortData.MiscControls.LinkComplianceMode == 0x1) {
    PcieForceCompliance (Engine, Pcie);
    PcieTrainingSetPortState (Engine, LinkStateResetExit, FALSE, Pcie);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Master procedure to init various features on all active ports
 *
 *
 *
 *
 * @param[in]   Pcie            Pointer to global PCIe configuration
 * @retval      AGESA_STATUS
 *
 */

AGESA_STATUS
PciePortPostInit (
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS  Status;
  Status = AGESA_SUCCESS;
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PciePortPostInitCallback,
    NULL,
    Pcie
    );
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init various features on all ports on S3 resume path
 *
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 * @param[in]       Pcie            Pointer to global PCIe configuration
 *
 */

VOID
STATIC
PciePortPostS3InitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIE_LINK_SPEED_CAP LinkSpeedCapability;
  ASSERT (Engine->EngineData.EngineType == PciePortEngine);
  LinkSpeedCapability = PcieFmGetLinkSpeedCap (PCIE_PORT_GEN_CAP_BOOT, Engine, Pcie);
  PcieSetLinkSpeedCap (LinkSpeedCapability, Engine, Pcie);
  if (Engine->Type.Port.PortData.MiscControls.LinkSafeMode == PcieGen1) {
    PcieLinkSafeMode (Engine, Pcie);
  }
  if (Engine->Type.Port.PortData.MiscControls.LinkComplianceMode == 0x1) {
    PcieForceCompliance (Engine, Pcie);
  }
  if (!Engine->Type.Port.IsSB) {
    if ((PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS) ||
      ((Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled) && (Engine->Type.Port.PortData.LinkHotplug != HotplugInboard)) ||
      (Engine->Type.Port.PortData.MiscControls.LinkComplianceMode == 0x1))) {
      PcieTrainingSetPortState (Engine, LinkStateResetExit, FALSE, Pcie);
    } else {
      PcieTrainingSetPortState (Engine, LinkStateDeviceNotPresent, FALSE, Pcie);
    }
    PcieConfigUpdatePortStatus (Engine, 0, INIT_STATUS_PCIE_TRAINING_SUCCESS);
  } else {
    PcieTrainingSetPortState (Engine, LinkStateTrainingSuccess, FALSE, Pcie);
  }
}
/*----------------------------------------------------------------------------------------*/
/**
 * Init port on S3 resume during destributed training
 *
 *
 *
 *
 * @param[in]   Pcie            Pointer to global PCIe configuration
 * @retval      AGESA_STATUS
 *
 */

AGESA_STATUS
PciePortPostS3Init (
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS  Status;
  Status = AGESA_SUCCESS;
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PciePortPostS3InitCallback,
    NULL,
    Pcie
    );
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Master procedure to init various features on all active ports
 *
 *
 *
 *
 * @param[in]   Pcie            Pointer to global PCIe configuration
 * @retval      AGESA_STATUS
 *
 */

AGESA_STATUS
PciePortPostEarlyInit (
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS  Status;
  Status = AGESA_SUCCESS;
  // Distributed Training started at PciePortInit complete it now to get access to PCIe devices
  if (Pcie->TrainingAlgorithm == PcieTrainingDistributed) {
    Pcie->TrainingExitState = LinkStateTrainingCompleted;
  }
  return Status;
}
