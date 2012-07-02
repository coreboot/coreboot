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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
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
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbPcieInitLibV4.h"
#include  "GnbFamServices.h"
#include  "PcieLibTN.h"
#include  "PciePowerGateTN.h"
#include  "PciePortServicesV4.h"
#include  "PcieMaxPayloadV4.h"
#include  "OptionGnb.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_PCIEMIDINITTN_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_BUILD_OPTIONS  GnbBuildOptions;
extern CONST PCIE_PORT_REGISTER_TABLE_HEADER ROMDATA PortInitMidTableTN;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
PcieMidInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );



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
PcieMidPortInitCallbackTN (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PciePortProgramRegisterTable (PortInitMidTableTN.Table, PortInitMidTableTN.Length, Engine, TRUE, Pcie);
  if (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS) || Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled) {
    PcieEnableSlotPowerLimit (Engine, Pcie);
    if (GnbFmCheckIommuPresent ((GNB_HANDLE*) PcieConfigGetParentSilicon (Engine), GnbLibGetHeader (Pcie))) {
      PcieInitPortForIommuV4 (Engine, Pcie);
    }
  }
  PcieEnableAspm (Engine,  Pcie);
  if (GnbBuildOptions.CfgMaxPayloadEnable) {
    PcieSetMaxPayload (Engine->Type.Port.Address, GnbLibGetHeader (Pcie));
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
STATIC
PcieMidPortInitTN (
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS         Status;
  PCIE_LINK_SPEED_CAP  GlobalSpeedCap;

  Status = AGESA_SUCCESS;
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PcieMidPortInitCallbackTN,
    NULL,
    Pcie
    );

  GlobalSpeedCap = PcieUtilGlobalGenCapability (
                     PCIE_PORT_GEN_CAP_BOOT | PCIE_GLOBAL_GEN_CAP_TRAINED_PORTS | PCIE_GLOBAL_GEN_CAP_HOTPLUG_PORTS,
                     Pcie
                     );


  PcieSetVoltageTN (GlobalSpeedCap, Pcie);

  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Per wrapper Pcie Late Init.
 *
 *
 * @param[in]  Wrapper         Pointer to wrapper configuration descriptor
 * @param[in]  Buffer          Pointer buffer
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */
AGESA_STATUS
STATIC
PcieMidInitCallbackTN (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  PciePwrPowerDownUnusedLanes (Wrapper, Pcie);
  PciePowerDownPllInL1TN (Wrapper, Pcie);
  PciePwrClockGatingV4 (Wrapper, Pcie);
  PcieLockRegisters (Wrapper, Pcie);
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Pcie Late Init
 *
 *   Late PCIe initialization
 *
 * @param[in]  Pcie                Pointer to global PCIe configuration
 * @retval     AGESA_SUCCESS       Topology successfully mapped
 * @retval     AGESA_ERROR         Topology can not be mapped
 */

AGESA_STATUS
STATIC
PcieMidInitTN (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  AGESA_STATUS          AgesaStatus;
  AGESA_STATUS          Status;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMidInitTN Enter\n");
  AgesaStatus = AGESA_SUCCESS;

  Status = PcieConfigRunProcForAllWrappers (DESCRIPTOR_ALL_WRAPPERS, PcieMidInitCallbackTN, NULL, Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);

  Status = PciePowerGateTN (Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMidInitTN Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * PCIe Mid Init
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */
AGESA_STATUS
PcieMidInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          AgesaStatus;
  AGESA_STATUS          Status;
  PCIe_PLATFORM_CONFIG  *Pcie;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMidInterfaceTN Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    PciePortsVisibilityControlTN (UnhidePorts, Pcie);

    Status = PcieMidPortInitTN (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PcieMidInitTN (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    PciePortsVisibilityControlTN (HidePorts, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMidInterfaceTN Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}
