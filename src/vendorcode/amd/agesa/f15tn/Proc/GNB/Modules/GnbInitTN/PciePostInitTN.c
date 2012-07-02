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
 * @e \$Revision: 63818 $   @e \$Date: 2012-01-09 03:02:03 -0600 (Mon, 09 Jan 2012) $
 *
 */
/*
*****************************************************************************
*
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
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
#include  "GnbPcieTrainingV1.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbPcieInitLibV4.h"
#include  "PcieLibTN.h"
#include  "GnbRegistersTN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_PCIEPOSTINITTN_FILECODE
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
PciePostEarlyInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

AGESA_STATUS
PciePostInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

AGESA_STATUS
PciePostS3InterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

VOID
PcieLateRestoreInitTNS3Script (
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
PciePostPortInitCallbackTN (
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
PciePostS3PortInitCallbackTN (
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
      PcieScratch = PciePortRegisterRead (Engine, DxF0xE4_x01_ADDRESS, Pcie);
      //
      // Hotplug device: we check ep status if reported
      //
      if ((PcieScratch & 0x1) == 0) {
        State = LinkStateResetExit;
      }
    }
    //
    // For compialnce we always leave link in enabled state
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
PciePostEarlyPortInitTN (
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
PciePostPortInitTN (
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS  Status;
  Status = AGESA_SUCCESS;
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PciePostPortInitCallbackTN,
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
PciePostS3PortInitTN (
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS  Status;
  Status = AGESA_SUCCESS;
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PciePostS3PortInitCallbackTN,
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
PciePostInitTN (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIE_LINK_SPEED_CAP  GlobalSpeedCap;

  GlobalSpeedCap = PcieUtilGlobalGenCapability (
                     PCIE_PORT_GEN_CAP_BOOT | PCIE_GLOBAL_GEN_CAP_TRAINED_PORTS | PCIE_GLOBAL_GEN_CAP_HOTPLUG_PORTS,
                     Pcie
                     );


  PcieSetVoltageTN (GlobalSpeedCap, Pcie);
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
PciePostEarlyInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          AgesaStatus;
  AGESA_STATUS          Status;
  PCIe_PLATFORM_CONFIG  *Pcie;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePostEarlyInterfaceTN Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    PciePortsVisibilityControlTN (UnhidePorts, Pcie);

    Status = PciePostEarlyPortInitTN (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PcieTraining (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    PciePortsVisibilityControlTN (HidePorts, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePostEarlyInterfaceTN Exit [0x%x]\n", AgesaStatus);
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
PciePostInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          AgesaStatus;
  AGESA_STATUS          Status;
  PCIe_PLATFORM_CONFIG  *Pcie;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePostInterfaceTN Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    PciePortsVisibilityControlTN (UnhidePorts, Pcie);

    Status = PciePostInitTN (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PciePostPortInitTN (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PcieTraining (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    PciePortsVisibilityControlTN (HidePorts, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePostInterfaceTN Exit [0x%x]\n", AgesaStatus);
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
PciePostS3InterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          AgesaStatus;
  AGESA_STATUS          Status;
  PCIe_PLATFORM_CONFIG  *Pcie;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePostS3InterfaceTN Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    PciePortsVisibilityControlTN (UnhidePorts, Pcie);

    Status = PciePostInitTN (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    if (Pcie->TrainingAlgorithm == PcieTrainingDistributed) {
      Status = PciePostS3PortInitTN (Pcie);
    } else {
      Status = PciePostPortInitTN (Pcie);
    }
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PcieTraining (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    PciePortsVisibilityControlTN (HidePorts, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePostS3InterfaceTN Exit [0x%x]\n", AgesaStatus);
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
PcieLateRestoreInitTNS3Script (
  IN      AMD_CONFIG_PARAMS               *StdHeader,
  IN      UINT16                          ContextLength,
  IN      VOID*                           Context
  )
{
  PciePostS3InterfaceTN (StdHeader);
}
