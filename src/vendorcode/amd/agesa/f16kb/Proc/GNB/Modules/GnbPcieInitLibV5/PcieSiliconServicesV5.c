/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific PCIe complex initialization services
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
#include  "GnbPcieInitLibV5.h"
#include  "GnbRegistersCommonV2.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV5_PCIESILICONSERVICESV5_FILECODE
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
STATIC
PcieSiliconControlPortsV5 (
  IN      PCIE_PORT_VISIBILITY    Control,
  IN      PCIe_SILICON_CONFIG     *Silicon,
  IN      PCIe_PLATFORM_CONFIG    *Pcie
  );

VOID
STATIC
PcieSiliconEnablePortsV5 (
  IN      PCIe_SILICON_CONFIG   *Silicon,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Control port visibility in PCI config space
 *
 *
 * @param[in]  Control         Control Hide/Unhide ports
 * @param[in]  Pcie            Pointer to global PCIe configuration
 */
VOID
PciePortsVisibilityControlV5 (
  IN      PCIE_PORT_VISIBILITY    Control,
  IN      PCIe_PLATFORM_CONFIG    *Pcie
  )
{
  PCIe_SILICON_CONFIG *SiliconList;
  SiliconList = (PCIe_SILICON_CONFIG *) PcieConfigGetChild (DESCRIPTOR_SILICON, &Pcie->Header);
  while (SiliconList != NULL) {
    switch (Control) {
    case UnhidePorts:
      PcieSiliconControlPortsV5 (UnhidePorts, SiliconList, Pcie);
      break;
    case HidePorts:
      PcieSiliconControlPortsV5 (HidePorts, SiliconList, Pcie);
      PcieSiliconEnablePortsV5 (SiliconList, Pcie);
      break;
    default:
      ASSERT (FALSE);
    }
    SiliconList = (PCIe_SILICON_CONFIG *) PcieConfigGetNextTopologyDescriptor (SiliconList, DESCRIPTOR_TERMINATE_TOPOLOGY);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Hide/Unhide all ports
 *
 *
 * @param[in]  Control             Control Hide/Unhide ports
 * @param[in]  Silicon             Pointer to silicon configuration descriptor
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */

VOID
STATIC
PcieSiliconControlPortsV5 (
  IN      PCIE_PORT_VISIBILITY    Control,
  IN      PCIe_SILICON_CONFIG     *Silicon,
  IN      PCIe_PLATFORM_CONFIG    *Pcie
  )
{
  PCIe_ENGINE_CONFIG  *EngineList;
  UINT32              Value;
  Value = (Control == HidePorts) ? ((1 << D0F0xCC_x01_BridgeDis_OFFSET) | (1 << D0F0xCC_x01_CfgDis_OFFSET)) : 0;
  EngineList = PcieConfigGetChildEngine (Silicon);
  while (EngineList != NULL) {
    if (PcieConfigIsPcieEngine (EngineList)) {
      GnbLibPciIndirectRMW (
        Silicon->Address.AddressValue | D0F0xC8_ADDRESS,
        D0F0xCC_x01_ADDRESS | ((EngineList->Type.Port.NativeDevNumber << 3 | EngineList->Type.Port.NativeFunNumber) << D0F0xC8_NB_DEV_IND_SEL_OFFSET),
        AccessS3SaveWidth32,
        (UINT32)~(D0F0xCC_x01_BridgeDis_MASK | D0F0xCC_x01_CfgDis_MASK | D0F0xCC_x01_CsrEnable_MASK | D0F0xCC_x01_SetPowEn_MASK),
        Value,
        GnbLibGetHeader (Pcie)
        );
    }
    EngineList = (PCIe_ENGINE_CONFIG *) PcieConfigGetNextTopologyDescriptor (EngineList, DESCRIPTOR_TERMINATE_GNB);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Hide unused ports
 *
 *
 *
 * @param[in]  Silicon             Pointer to silicon configuration data area
 * @param[in]  Pcie                Pointer to data area up to 256 byte
 */

VOID
STATIC
PcieSiliconEnablePortsV5 (
  IN      PCIe_SILICON_CONFIG   *Silicon,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIe_ENGINE_CONFIG  *EngineList;
  EngineList = PcieConfigGetChildEngine (Silicon);
  while (EngineList != NULL) {
    if (PcieConfigIsPcieEngine (EngineList)) {
      if (!PcieConfigIsSbPcieEngine (EngineList) &&
        (PcieConfigCheckPortStatus (EngineList, INIT_STATUS_PCIE_TRAINING_SUCCESS) ||
        ((EngineList->Type.Port.PortData.LinkHotplug != HotplugDisabled) &&
        (EngineList->Type.Port.PortData.LinkHotplug != HotplugInboard)))) {
        GnbLibPciIndirectRMW (
          Silicon->Address.AddressValue | D0F0xC8_ADDRESS,
          D0F0xCC_x01_ADDRESS | ((EngineList->Type.Port.PortData.DeviceNumber << 3 | EngineList->Type.Port.PortData.FunctionNumber)  << D0F0xC8_NB_DEV_IND_SEL_OFFSET),
          AccessS3SaveWidth32,
          (UINT32)~(D0F0xCC_x01_BridgeDis_MASK | D0F0xCC_x01_CfgDis_MASK | D0F0xCC_x01_CsrEnable_MASK | D0F0xCC_x01_SetPowEn_MASK),
          ((1 << D0F0xCC_x01_CsrEnable_OFFSET) | (1 << D0F0xCC_x01_SetPowEn_OFFSET)),
          GnbLibGetHeader (Pcie)
        );
      }
    }
    EngineList = (PCIe_ENGINE_CONFIG *) PcieConfigGetNextTopologyDescriptor (EngineList, DESCRIPTOR_TERMINATE_GNB);
  }
}

