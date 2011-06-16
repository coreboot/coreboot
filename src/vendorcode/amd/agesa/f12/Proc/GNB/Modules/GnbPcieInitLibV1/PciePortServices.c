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
#include  "amdlib.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
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
    DxF0xE4_xB5.Field.LcEhpRxPhyCmd = 0x3;
    DxF0xE4_xB5.Field.LcEhpTxPhyCmd = 0x3;
    DxF0xE4_xB5.Field.LcEnhancedHotPlugEn = 0x1;
    PciePortRegisterWrite (
      Engine,
      DxF0xE4_xB5_ADDRESS,
      DxF0xE4_xB5.Value,
      TRUE,
      Pcie
      );
    PcieRegisterWriteField (
      PcieConfigGetParentWrapper (Engine),
      CORE_SPACE (Engine->Type.Port.CoreId, 0x10),
      1,
      3,
      0x5,
      TRUE,
      Pcie
      );
    PcieRegisterWriteField (
      PcieConfigGetParentWrapper (Engine),
      WRAP_SPACE (PcieConfigGetParentWrapper (Engine)->WrapId, D0F0xE4_WRAP_8011_ADDRESS),
      D0F0xE4_WRAP_8011_RcvrDetClkEnable_OFFSET,
      D0F0xE4_WRAP_8011_RcvrDetClkEnable_WIDTH,
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
  PcieSetLinkSpeedCap (PcieGen1, Engine, Pcie);
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
  DxF0xE4_xA4_STRUCT  DxF0xE4_xA4;
  DxF0xE4_xC0_STRUCT  DxF0xE4_xC0;
  DxF0x88_STRUCT      DxF0x88;
  GnbLibPciRead (
    Engine->Type.Port.Address.AddressValue | DxF0x88_ADDRESS,
    AccessWidth32,
    &DxF0x88.Value,
    GnbLibGetHeader (Pcie)
   );
  DxF0xE4_xA4.Value = PciePortRegisterRead (
                        Engine,
                        DxF0xE4_xA4_ADDRESS,
                        Pcie
                        );
  DxF0xE4_xC0.Value = PciePortRegisterRead (
                        Engine,
                        DxF0xE4_xC0_ADDRESS,
                        Pcie
                        );

  switch (LinkSpeedCapability) {
  case PcieGen2:
    DxF0xE4_xA4.Field.LcGen2EnStrap = 0x1;
    DxF0xE4_xA4.Field.LcMultUpstreamAutoSpdChngEn = 0x1;
    DxF0xE4_xC0.Field.StrapAutoRcSpeedNegotiationDis = 0x0;
    DxF0x88.Field.TargetLinkSpeed = 0x2;
    DxF0x88.Field.HwAutonomousSpeedDisable = 0x0;
    break;
  case PcieGen1:
    DxF0xE4_xA4.Field.LcGen2EnStrap = 0x0;
    DxF0xE4_xA4.Field.LcMultUpstreamAutoSpdChngEn = 0x0;
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
    DxF0xE4_xA4_ADDRESS,
    DxF0xE4_xA4.Value,
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
 * Set slo power limit
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
      (D0F0x64_x51_ADDRESS + (Engine->Type.Port.Address.Address.Device - 2) * 2) | IOC_WRITE_ENABLE,
      AccessS3SaveWidth32,
      0xffffffff,
      1 << D0F0x64_x51_SetPowEn_OFFSET,
      GnbLibGetHeader (Pcie)
    );
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Enable ASPM
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
    } else {
      PcieLinkAspmEnable (
        Engine->Type.Port.Address,
        Engine->Type.Port.PortData.LinkAspm,
        GnbLibGetHeader (Pcie)
        );
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
