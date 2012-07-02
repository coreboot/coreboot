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
#include  "Ids.h"
#include  "amdlib.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbPcieFamServices.h"
#include  "GnbSbLib.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbRegistersLN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV1_PCIEPORTSERVICES_FILECODE
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
 * Set completion timeout
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */

VOID
PcieCompletionTimeout (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  GnbLibPciRMW (
    Engine->Type.Port.Address.AddressValue | DxF0x80_ADDRESS,
    AccessWidth32,
    0xffffffff,
    0x6 << DxF0x80_CplTimeoutValue_OFFSET,
    GnbLibGetHeader (Pcie)
    );
  if (Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled) {
    PciePortRegisterWriteField (
      Engine,
      DxF0xE4_x20_ADDRESS,
      DxF0xE4_x20_TxFlushTlpDis_OFFSET,
      DxF0xE4_x20_TxFlushTlpDis_WIDTH,
      0x0,
      TRUE,
      Pcie
      );
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init hotplug port
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */

VOID
PcieLinkInitHotplug (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  DxF0xE4_xB5_STRUCT  DxF0xE4_xB5;
  if ((Engine->Type.Port.PortData.LinkHotplug == HotplugEnhanced) || (Engine->Type.Port.PortData.LinkHotplug == HotplugInboard)) {
    DxF0xE4_xB5.Value = PciePortRegisterRead (Engine, DxF0xE4_xB5_ADDRESS, Pcie);
    DxF0xE4_xB5.Field.line521  = 0x3;
    DxF0xE4_xB5.Field.line522  = 0x3;
    DxF0xE4_xB5.Field.line519  = 0x1;
    PciePortRegisterWrite (
      Engine,
      DxF0xE4_xB5_ADDRESS,
      DxF0xE4_xB5.Value,
      TRUE,
      Pcie
      );
    PcieRegisterWriteField (
      PcieConfigGetParentWrapper (Engine),
      CORE_SPACE (Engine->Type.Port.CoreId, D0F0xE4_CORE_0010_ADDRESS),
      D0F0xE4_CORE_0010_LcHotPlugDelSel_OFFSET,
      D0F0xE4_CORE_0010_LcHotPlugDelSel_WIDTH,
      0x5,
      TRUE,
      Pcie
      );
    PcieRegisterWriteField (
      PcieConfigGetParentWrapper (Engine),
      WRAP_SPACE (PcieConfigGetParentWrapper (Engine)->WrapId, 0x8011 ),
      16 ,
      1 ,
      0x1,
      TRUE,
      Pcie
      );
  }
  if (Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled) {
    GnbLibPciRMW (
      Engine->Type.Port.Address.AddressValue | DxF0x6C_ADDRESS,
      AccessS3SaveWidth32,
      0xffffffff,
      1 << DxF0x6C_HotplugCapable_OFFSET,
      GnbLibGetHeader (Pcie)
      );
    PciePortRegisterWriteField (
      Engine,
      DxF0xE4_x20_ADDRESS,
      DxF0xE4_x20_TxFlushTlpDis_OFFSET,
      DxF0xE4_x20_TxFlushTlpDis_WIDTH,
      0x0,
      TRUE,
      Pcie
      );
    PciePortRegisterWriteField (
      Engine,
      DxF0xE4_x70_ADDRESS,
      DxF0xE4_x70_RxRcbCplTimeoutMode_OFFSET,
      DxF0xE4_x70_RxRcbCplTimeoutMode_WIDTH,
      0x1,
      FALSE,
      Pcie
      );
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set misc slot capability
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */

VOID
PcieLinkSetSlotCap (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  GnbLibPciRMW (
    Engine->Type.Port.Address.AddressValue | DxF0x58_ADDRESS,
    AccessWidth32,
    0xffffffff,
    1 << DxF0x58_SlotImplemented_OFFSET,
    GnbLibGetHeader (Pcie)
    );
  GnbLibPciRMW (
    Engine->Type.Port.Address.AddressValue | DxF0x3C_ADDRESS,
    AccessWidth32,
    0xffffffff,
    1 << DxF0x3C_IntPin_OFFSET,
    GnbLibGetHeader (Pcie)
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Safe mode to force link advertize Gen1 only capability in TS
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */

VOID
PcieLinkSafeMode (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  //Engine->Type.Port.PortData.LinkSpeedCapability = PcieGen1;
  PcieFmSetLinkSpeedCap (PcieGen1, Engine, Pcie);
  PciePortRegisterRMW (
    Engine,
    DxF0xE4_xA2_ADDRESS,
    DxF0xE4_xA2_LcUpconfigureDis_MASK,
    (1 << DxF0xE4_xA2_LcUpconfigureDis_OFFSET),
    FALSE,
    Pcie
    );
}


/*----------------------------------------------------------------------------------------*/
/**
 * Set current link speed
 *
 *
 * @param[in]  Engine               Pointer to engine configuration descriptor
 * @param[in]  Pcie                 Pointer to global PCIe configuration
 *
 */
VOID
PcieSetLinkWidthCap (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PciePortRegisterRMW (
    Engine,
    DxF0xE4_xA2_ADDRESS,
    DxF0xE4_xA2_LcUpconfigureDis_MASK,
    0,
    FALSE,
    Pcie
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set current link speed
 *
 *
 * @param[in]  LinkSpeedCapability  Link Speed Capability
 * @param[in]  Engine               Pointer to engine configuration descriptor
 * @param[in]  Pcie                 Pointer to global PCIe configuration
 *
 */
VOID
PcieSetLinkSpeedCap (
  IN      PCIE_LINK_SPEED_CAP   LinkSpeedCapability,
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  ex548_STRUCT   ex548 ;
  DxF0xE4_xC0_STRUCT  DxF0xE4_xC0;
  DxF0x88_STRUCT      DxF0x88;
  GnbLibPciRead (
    Engine->Type.Port.Address.AddressValue | DxF0x88_ADDRESS,
    AccessWidth32,
    &DxF0x88.Value,
    GnbLibGetHeader (Pcie)
   );
  ex548.Value = PciePortRegisterRead (
                        Engine,
                        0xa4 ,
                        Pcie
                        );
  DxF0xE4_xC0.Value = PciePortRegisterRead (
                        Engine,
                        DxF0xE4_xC0_ADDRESS,
                        Pcie
                        );

  switch (LinkSpeedCapability) {
  case PcieGen2:
    ex548.Field.LcGen2EnStrap = 0x1;
    ex548.Field.LcMultUpstreamAutoSpdChngEn = 0x1;
    DxF0xE4_xC0.Field.StrapAutoRcSpeedNegotiationDis = 0x0;
    DxF0x88.Field.TargetLinkSpeed = 0x2;
    DxF0x88.Field.HwAutonomousSpeedDisable = 0x0;
    break;
  case PcieGen1:
    ex548.Field.LcGen2EnStrap = 0x0;
    ex548.Field.LcMultUpstreamAutoSpdChngEn = 0x0;
    DxF0xE4_xC0.Field.StrapAutoRcSpeedNegotiationDis = 0x1;
    DxF0x88.Field.TargetLinkSpeed = 0x1;
    DxF0x88.Field.HwAutonomousSpeedDisable = 0x1;
    PcieRegisterWriteField (
      PcieConfigGetParentWrapper (Engine),
      WRAP_SPACE (PcieConfigGetParentWrapper (Engine)->WrapId, D0F0xE4_WRAP_0803_ADDRESS + 0x100 * Engine->Type.Port.PortId),
      D0F0xE4_WRAP_0803_StrapBifDeemphasisSel_OFFSET,
      D0F0xE4_WRAP_0803_StrapBifDeemphasisSel_WIDTH,
      0,
      FALSE,
      Pcie
      );
    break;
  default:
    ASSERT (FALSE);
    break;
  }
  PciePortRegisterWrite (
    Engine,
    0xa4 ,
    ex548.Value,
    FALSE,
    Pcie
    );
  PciePortRegisterWrite (
    Engine,
    DxF0xE4_xC0_ADDRESS,
    DxF0xE4_xC0.Value,
    FALSE,
    Pcie
    );
  GnbLibPciWrite (
    Engine->Type.Port.Address.AddressValue | DxF0x88_ADDRESS,
    AccessWidth32,
    &DxF0x88.Value,
    GnbLibGetHeader (Pcie)
   );
}


/*----------------------------------------------------------------------------------------*/
/**
 * Force compliance
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */

VOID
PcieForceCompliance (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  if (Engine->Type.Port.PortData.LinkSpeedCapability >= PcieGen2) {
    GnbLibPciRMW (
      Engine->Type.Port.Address.AddressValue | DxF0x88_ADDRESS,
      AccessWidth32,
      0xffffffff,
      0x1 << DxF0x88_EnterCompliance_OFFSET,
      GnbLibGetHeader (Pcie)
      );
  } else if (Engine->Type.Port.PortData.LinkSpeedCapability == PcieGen1) {
    PciePortRegisterWriteField (
      Engine,
      DxF0xE4_xC0_ADDRESS,
      DxF0xE4_xC0_StrapForceCompliance_OFFSET,
      DxF0xE4_xC0_StrapForceCompliance_WIDTH,
      0x1,
      FALSE,
      Pcie
      );
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set slot power limit
 *
 *
 *
 * @param[in]  Engine              Pointer to engine configuration
 * @param[in]  Pcie                Pointer to PCIe configuration
 */


VOID
PcieEnableSlotPowerLimit (
  IN      PCIe_ENGINE_CONFIG     *Engine,
  IN      PCIe_PLATFORM_CONFIG   *Pcie
  )
{
  ASSERT (Engine->EngineData.EngineType == PciePortEngine);
  if (PcieLibIsEngineAllocated (Engine) && Engine->Type.Port.PortData.PortPresent != PortDisabled && !PcieConfigIsSbPcieEngine (Engine)) {
    IDS_HDT_CONSOLE (PCIE_MISC, "   Enable Slot Power Limit for Port % d\n", Engine->Type.Port.Address.Address.Device);
    GnbLibPciIndirectRMW (
      MAKE_SBDFO (0, 0, 0, 0, D0F0x60_ADDRESS),
      (0x51  + (Engine->Type.Port.Address.Address.Device - 2) * 2) | IOC_WRITE_ENABLE,
      AccessS3SaveWidth32,
      0xffffffff,
      1 << 20 ,
      GnbLibGetHeader (Pcie)
    );
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Enable ASPM on SB link
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */

VOID
PcieEnableAspm (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  if (Engine->Type.Port.PortData.LinkAspm != AspmDisabled) {
    if (PcieConfigIsSbPcieEngine (Engine)) {
      SbPcieLinkAspmControl (Engine, Pcie);
    }
  }
}


UINT8 L1State = 0x1b;
/*----------------------------------------------------------------------------------------*/
/**
 * Poll for link to get into L1
 *
 *
 *
 * @param[in]  Engine              Pointer to Engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
VOID
PciePollLinkForL1Entry (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8 LinkHwStateHistory[8];
  do {
    PcieUtilGetLinkHwStateHistory (Engine, &LinkHwStateHistory[0], sizeof (LinkHwStateHistory), Pcie);
  } while (!PcieUtilSearchArray (LinkHwStateHistory, sizeof (LinkHwStateHistory), &L1State, sizeof (L1State)));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Poll for link to get into L1
 *
 *
 *
 * @param[in]  Engine              Pointer to Engine config descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
VOID
PciePollLinkForL0Exit (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8 LinkHwStateHistory[4];
  do {
    PcieUtilGetLinkHwStateHistory (Engine, &LinkHwStateHistory[0], sizeof (LinkHwStateHistory), Pcie);
  } while (LinkHwStateHistory[0] != 0x10);
}
