/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe post initialization.
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
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbPcieFamServices.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieTrainingV2.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbPcieInitLibV4.h"
#include  "GnbPcieInitLibV5.h"
#include  "PcieLibKB.h"
#include  "GnbRegistersKB.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITKB_PCIEPOSTINITKB_FILECODE
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
PciePostEarlyInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

AGESA_STATUS
PciePostInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

VOID
PcieLateRestoreInitKBS3Script (
  IN      AMD_CONFIG_PARAMS               *StdHeader,
  IN      UINT16                          ContextLength,
  IN      VOID*                           Context
  );

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
PciePostPortInitCallbackKB (
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
  LinkSpeedCapability = PcieFmGetLinkSpeedCap (PCIE_PORT_GEN_CAP_BOOT, Engine);
  PcieSetLinkSpeedCapV4 (LinkSpeedCapability, Engine, Pcie);
  if (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS) && (LinkSpeedCapability > PcieGen1) && !PcieConfigIsSbPcieEngine (Engine)) {
    PcieTrainingSetPortState (Engine, LinkStateRetrain, FALSE, Pcie);
    PcieConfigUpdatePortStatus (Engine, 0, INIT_STATUS_PCIE_TRAINING_SUCCESS);
  }
  if (Engine->Type.Port.PortData.MiscControls.LinkComplianceMode == 0x1) {
    PcieForceCompliance (Engine, Pcie);
    PcieTrainingSetPortState (Engine, LinkStateResetExit, FALSE, Pcie);
  }
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
PciePostS3PortInitCallbackKB (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIE_LINK_SPEED_CAP       LinkSpeedCapability;
  PCIE_LINK_TRAINING_STATE  State;

  ASSERT (Engine->EngineData.EngineType == PciePortEngine);

  LinkSpeedCapability = PcieFmGetLinkSpeedCap (PCIE_PORT_GEN_CAP_BOOT, Engine);
  PcieSetLinkSpeedCapV4 (LinkSpeedCapability, Engine, Pcie);

  if (Engine->Type.Port.PortData.MiscControls.LinkSafeMode == PcieGen1) {
    PcieLinkSafeMode (Engine, Pcie);
  }

  if (!PcieConfigIsSbPcieEngine (Engine)) {
    //
    // General Port
    //
    State = LinkStateDeviceNotPresent;
    if (Engine->Type.Port.PortData.LinkHotplug == HotplugDisabled || Engine->Type.Port.PortData.LinkHotplug == HotplugInboard) {
      //
      // Non hotplug device: we only check status from previous boot
      //
      if (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
        State = LinkStateResetExit;
      }
    } else {
      UINT32  PcieScratch;
      //
      //  Get endpoint staus from scratch
      //
      PcieScratch = PciePortRegisterRead (Engine, 0x1, Pcie);
      //
      // Hotplug device: we check ep status if reported
      //
      if ((PcieScratch & 0x1) == 0) {
        State = LinkStateResetExit;
      }
    }
    //
    // For compliance we always leave link in enabled state
    //
    if (Engine->Type.Port.PortData.MiscControls.LinkComplianceMode) {
      State = LinkStateResetExit;
    }
    PcieConfigUpdatePortStatus (Engine, 0, INIT_STATUS_PCIE_TRAINING_SUCCESS);
  } else {
    //
    // SB port
    //
    State = LinkStateTrainingSuccess;
  }
  PcieTrainingSetPortState (Engine, State, FALSE, Pcie);
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
STATIC
PciePostEarlyPortInitKB (
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
STATIC
PciePostPortInitKB (
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS  Status;
  Status = AGESA_SUCCESS;
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PciePostPortInitCallbackKB,
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
STATIC
PciePostS3PortInitKB (
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS  Status;
  Status = AGESA_SUCCESS;
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PciePostS3PortInitCallbackKB,
    NULL,
    Pcie
    );
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Pcie Init
 *
 *
 *
 * @param[in]  Pcie                Pointer to global PCIe configuration
 * @retval     AGESA_SUCCESS       Topology successfully mapped
 * @retval     AGESA_ERROR         Topology can not be mapped
 */

AGESA_STATUS
STATIC
PciePostInitKB (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIE_LINK_SPEED_CAP  GlobalSpeedCap;

  GlobalSpeedCap = PcieUtilGlobalGenCapability (
                     PCIE_PORT_GEN_CAP_BOOT | PCIE_GLOBAL_GEN_CAP_TRAINED_PORTS | PCIE_GLOBAL_GEN_CAP_HOTPLUG_PORTS,
                     Pcie
                     );


  PcieSetVoltageKB (GlobalSpeedCap, Pcie);
  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * PCIe Post Init
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */
AGESA_STATUS
PciePostEarlyInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          AgesaStatus;
  AGESA_STATUS          Status;
  PCIe_PLATFORM_CONFIG  *Pcie;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePostEarlyInterfaceKB Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  IDS_OPTION_HOOK (IDS_BEFORE_GPP_TRAINING, Pcie, StdHeader);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    PciePortsVisibilityControlV5 (UnhidePorts, Pcie);

    Status = PciePostEarlyPortInitKB (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PcieTraining (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    PciePortsVisibilityControlV5 (HidePorts, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePostEarlyInterfaceKB Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * PCIe Post Init
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */
AGESA_STATUS
PciePostInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          AgesaStatus;
  AGESA_STATUS          Status;
  PCIe_PLATFORM_CONFIG  *Pcie;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePostInterfaceKB Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  IDS_OPTION_HOOK (IDS_BEFORE_GEN2_INIT, Pcie, StdHeader);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    PciePortsVisibilityControlV5 (UnhidePorts, Pcie);

    Status = PciePostInitKB (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PciePostPortInitKB (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PcieTraining (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    PciePortsVisibilityControlV5 (HidePorts, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePostInterfaceKB Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}


/*----------------------------------------------------------------------------------------*/
/**
 * PCIe Post Init
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */
STATIC AGESA_STATUS
PciePostS3InterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          AgesaStatus;
  AGESA_STATUS          Status;
  PCIe_PLATFORM_CONFIG  *Pcie;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePostS3InterfaceKB Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    PciePortsVisibilityControlV5 (UnhidePorts, Pcie);

    Status = PciePostInitKB (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    if (Pcie->TrainingAlgorithm == PcieTrainingDistributed) {
      Status = PciePostS3PortInitKB (Pcie);
    } else {
      Status = PciePostPortInitKB (Pcie);
    }
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PcieTraining (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    PciePortsVisibilityControlV5 (HidePorts, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePostS3InterfaceKB Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * PCIe S3 restore
 *
 *
 *
 * @param[in]  StdHeader      Standard configuration header
 * @param[in]  ContextLength  Context Length (not used)
 * @param[in]  Context        Context pointer (not used)
 */
VOID
PcieLateRestoreInitKBS3Script (
  IN      AMD_CONFIG_PARAMS               *StdHeader,
  IN      UINT16                          ContextLength,
  IN      VOID*                           Context
  )
{
  PciePostS3InterfaceKB (StdHeader);
}
