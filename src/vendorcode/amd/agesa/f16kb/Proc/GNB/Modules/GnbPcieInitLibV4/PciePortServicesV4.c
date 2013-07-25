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
#include  "GnbPcieConfig.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbRegistersCommonV2.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV4_PCIEPORTSERVICESV4_FILECODE
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
VOID
PcieInitPortForIommuV4 (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieSetLinkSpeedCapV4 (
  IN      PCIE_LINK_SPEED_CAP   LinkSpeedCapability,
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );
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
PcieSetLinkSpeedCapV4 (
  IN      PCIE_LINK_SPEED_CAP   LinkSpeedCapability,
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  D2FxxE4_xA4_STRUCT  D2FxxE4_xA4;
  D2FxxE4_xC0_STRUCT  D2FxxE4_xC0;
  D2Fxx88_STRUCT      D2Fxx88;
  GnbLibPciRead (
    Engine->Type.Port.Address.AddressValue | D2Fxx88_ADDRESS,
    AccessWidth32,
    &D2Fxx88.Value,
    GnbLibGetHeader (Pcie)
   );
  D2FxxE4_xA4.Value = PciePortRegisterRead (
                        Engine,
                        D2FxxE4_xA4_ADDRESS,
                        Pcie
                        );
  D2FxxE4_xC0.Value = PciePortRegisterRead (
                        Engine,
                        D2FxxE4_xC0_ADDRESS,
                        Pcie
                        );

  switch (LinkSpeedCapability) {
  case PcieGen2:
    D2FxxE4_xA4.Field.LcGen2EnStrap = 0x1;
    D2FxxE4_xA4.Field.LcMultUpstreamAutoSpdChngEn = 0x1;
    D2Fxx88.Field.TargetLinkSpeed = 0x2;
    D2Fxx88.Field.HwAutonomousSpeedDisable = 0x0;
    break;
  case PcieGen1:
    D2FxxE4_xA4.Field.LcGen2EnStrap = 0x0;
    D2FxxE4_xA4.Field.LcMultUpstreamAutoSpdChngEn = 0x0;
    D2Fxx88.Field.TargetLinkSpeed = 0x1;
    D2Fxx88.Field.HwAutonomousSpeedDisable = 0x1;
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

  if (Pcie->PsppPolicy == PsppDisabled) {
    D2FxxE4_xC0.Field.StrapAutoRcSpeedNegotiationDis = 0x0;
  } else {
    D2FxxE4_xC0.Field.StrapAutoRcSpeedNegotiationDis = 0x1;
  }

  PciePortRegisterWrite (
    Engine,
    D2FxxE4_xA4_ADDRESS,
    D2FxxE4_xA4.Value,
    FALSE,
    Pcie
    );
  PciePortRegisterWrite (
    Engine,
    D2FxxE4_xC0_ADDRESS,
    D2FxxE4_xC0.Value,
    FALSE,
    Pcie
    );
  GnbLibPciWrite (
    Engine->Type.Port.Address.AddressValue | D2Fxx88_ADDRESS,
    AccessWidth32,
    &D2Fxx88.Value,
    GnbLibGetHeader (Pcie)
   );
}
/*----------------------------------------------------------------------------------------*/
/**
 * Enable passing TLP prefix to IOMMU if IOMMU enabled
 *
 *
 * @param[in]  Engine               Pointer to engine configuration descriptor
 * @param[in]  Pcie                 Pointer to global PCIe configuration
 *
 */
VOID
PcieInitPortForIommuV4 (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PciePortRegisterRMW (
    Engine,
    D2FxxE4_xC1_ADDRESS,
    D2FxxE4_xC1_StrapE2EPrefixEn_MASK | D2FxxE4_xC1_StrapExtendedFmtSupported_MASK,
    (1 << D2FxxE4_xC1_StrapE2EPrefixEn_OFFSET) | (1 << D2FxxE4_xC1_StrapExtendedFmtSupported_OFFSET),
    TRUE,
    Pcie
    );
}
