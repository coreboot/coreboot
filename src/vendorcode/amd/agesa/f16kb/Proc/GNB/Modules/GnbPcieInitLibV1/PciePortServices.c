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
#include  "Ids.h"
#include  "amdlib.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbPcieFamServices.h"
#include  "GnbSbLib.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbRegistersCommon.h"
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
UINT8 L1State = 0x1b;

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
    Engine->Type.Port.Address.AddressValue | 0x80,
    AccessWidth32,
    0xffffffff,
    0x6 << 0,
    GnbLibGetHeader (Pcie)
    );
  if (Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled) {
    PciePortRegisterWriteField (
      Engine,
      0x20,
      15,
      1,
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
  UINT32 Value;
  if ((Engine->Type.Port.PortData.LinkHotplug == HotplugEnhanced) || (Engine->Type.Port.PortData.LinkHotplug == HotplugInboard)) {
    Value = PciePortRegisterRead (Engine, 0xb5, Pcie);
    Value |= 3 << 12;
    Value |= 3 << 14;
    Value |= 1 << 10;
    PciePortRegisterWrite (
      Engine,
      0xb5,
      Value,
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
      Engine->Type.Port.Address.AddressValue | 0x6c,
      AccessS3SaveWidth32,
      0xffffffff,
      1 << 6,
      GnbLibGetHeader (Pcie)
      );
    PciePortRegisterWriteField (
      Engine,
      0x20,
      15,
      1,
      0x0,
      TRUE,
      Pcie
      );
    PciePortRegisterWriteField (
      Engine,
      0x70,
      19,
      1,
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
    Engine->Type.Port.Address.AddressValue | 0x58,
    AccessWidth32,
    0xffffffff,
    1 << 24,
    GnbLibGetHeader (Pcie)
    );
  GnbLibPciRMW (
    Engine->Type.Port.Address.AddressValue | 0x3c,
    AccessWidth32,
    0xffffffff,
    1 << 8,
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
  PcieFmSetLinkSpeedCap (PcieGen1, Engine, Pcie);
  PciePortRegisterRMW (
    Engine,
    0xa2,
    0x2000,
    (1 << 13),
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
    0xa2,
    0x2000,
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
      Engine->Type.Port.Address.AddressValue | 0x88,
      AccessWidth32,
      0xffffffff,
      0x1 << 4,
      GnbLibGetHeader (Pcie)
      );
  } else if (Engine->Type.Port.PortData.LinkSpeedCapability == PcieGen1) {
    PciePortRegisterWriteField (
      Engine,
      0xc0,
      13,
      1,
      0x1,
      FALSE,
      Pcie
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
 * Poll for link to get into L0
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
