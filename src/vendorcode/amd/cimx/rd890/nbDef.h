/**
 * @file
 *
 *
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-NB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 ***************************************************************************/

#ifndef _NBDEF_H_
#define _NBDEF_H_

#pragma pack (push, 1)

AGESA_STATUS
AmdPowerOnResetInit (
  IN  OUT AMD_NB_CONFIG_BLOCK *ConfigPtr
  );

AGESA_STATUS
NbPowerOnResetInit (
  IN      AMD_NB_CONFIG       *NbConfigPtr
  );

AGESA_STATUS
AmdHtInit (
  IN  OUT AMD_NB_CONFIG_BLOCK *ConfigPtr
  );

AGESA_STATUS
NbHtInit (
  IN  OUT AMD_NB_CONFIG       *NbConfigPtr
  );

AGESA_STATUS
AmdEarlyPostInit (
  IN  OUT AMD_NB_CONFIG_BLOCK *ConfigPtr
  );

AGESA_STATUS
NbEarlyPostInit (
  IN  OUT AMD_NB_CONFIG       *NbConfigPtr
  );

AGESA_STATUS
AmdMidPostInit (
  IN  OUT AMD_NB_CONFIG_BLOCK *ConfigPtr
  );

AGESA_STATUS
NbMidPostInit (
  IN  OUT AMD_NB_CONFIG       *NbConfigPtr
  );

AGESA_STATUS
AmdLatePostInit (
  IN  OUT AMD_NB_CONFIG_BLOCK *ConfigPtr
  );

AGESA_STATUS
NbLatePostInit (
  IN  OUT AMD_NB_CONFIG       *NbConfigPtr
  );

AGESA_STATUS
AmdPcieEarlyInit (
  IN  OUT AMD_NB_CONFIG_BLOCK *ConfigPtr
  );

AGESA_STATUS
PcieEarlyInit (
  IN  OUT AMD_NB_CONFIG       *NbConfigPtr
  );


AGESA_STATUS
AmdS3Init (
  IN  OUT AMD_NB_CONFIG_BLOCK *ConfigPtr
  );

AGESA_STATUS
AmdS3InitIommu (
  IN OUT   AMD_NB_CONFIG_BLOCK *ConfigPtr
  );

AGESA_STATUS
NbS3Init (
  IN  OUT AMD_NB_CONFIG       *NbConfigPtr
  );

AGESA_STATUS
AmdPcieLateInit (
  IN      AMD_NB_CONFIG_BLOCK *ConfigPtr
  );

AGESA_STATUS
AmdPcieLateInitWa (
  IN      AMD_NB_CONFIG_BLOCK *ConfigPtr
  );

AGESA_STATUS
AmdPcieValidatePortState (
  IN      AMD_NB_CONFIG_BLOCK *ConfigPtr
  );

AGESA_STATUS
PcieLateInit (
  IN      AMD_NB_CONFIG       *NbConfigPtr
  );

AGESA_STATUS
PcieLateInitWa (
  IN      AMD_NB_CONFIG    *NbConfigPtr
  );

AGESA_STATUS
PcieValidatePortState (
  IN      AMD_NB_CONFIG       *NbConfigPtr
  );

AGESA_STATUS
PcieLateInitPorts (
  IN      AMD_NB_CONFIG       *NbConfigPtr
  );


AGESA_STATUS
AmdPcieS3Init (
  IN      AMD_NB_CONFIG_BLOCK *ConfigPtr
  );

BOOLEAN
PcieLibCheckGen2Disabled (
  IN      PORT                PortId,
  IN  OUT AMD_NB_CONFIG       *pConfig
  );

VOID
PcieLibSetGen2Disabled (
  IN      PORT                PortId,
  IN  OUT AMD_NB_CONFIG       *pConfig
  );


PCI_ADDR
PcieLibGetPortPciAddress (
  IN        PORT              PortId,
  IN        AMD_NB_CONFIG     *pConfig
  );

VOID
PcieLibSetLinkCompliance (
  IN        PORT              PortId,
  IN        AMD_NB_CONFIG     *pConfig
  );


AGESA_STATUS
PcieLibInitValidateInput (
  IN  OUT AMD_NB_CONFIG       *pConfig
  );

VOID
PcieLibSetPcieMmioBase (
  IN    UINT16                PcieMmioBase,
  IN    UINT16                PcieMmioSize,
  IN    AMD_NB_CONFIG         *pConfig
 );

AGESA_STATUS
PcieCheckSelectedPorts (
  IN      UINT16              SelectedPortMask,
  IN      AMD_NB_CONFIG       *pConfig
  );

PCIE_LINK_STATUS
PcieGetPortsLinkStatus (
  IN      UINT16              SelectedPortMask,
  IN  OUT PCIE_LINK_STATUS    *PortLinkStatus,
  IN      UINT32              Pooling,
  IN      AMD_NB_CONFIG       *pConfig
  );


UINT16
PcieFindPortsWithLinkStatus (
  IN     PCIE_LINK_STATUS     *PortLinkStatus,
  IN     PCIE_LINK_STATUS     LinkStatus
  );

AGESA_STATUS
PcieBrokenLaneWorkaround (
  IN      UINT16              SelectedPortMask,
  IN      AMD_NB_CONFIG       *pConfig
  );

AGESA_STATUS
PcieGen2Workaround (
  IN      UINT16              SelectedPortMask,
  IN      AMD_NB_CONFIG       *pConfig
  );

AGESA_STATUS
PcieMiscWorkaround (
  IN      PCIE_LINK_STATUS    *PortsLinkStatus,
  IN      AMD_NB_CONFIG       *pConfig
  );

AGESA_STATUS
PcieCheckVco (
  IN      UINT16              SelectedPortMask,
  IN      PCIE_LINK_STATUS    *PortsLinkStatus,
  IN      AMD_NB_CONFIG       *pConfig
  );


VOID
PcieLibPortTrainingControl (
  IN       PORT               PortId,
  IN       PCIE_LINK_TRAINING Operation,
  IN       AMD_NB_CONFIG      *pConfig
  );

VOID
PcieLibSetCoreConfiguration (
  IN        CORE              CoreId,
  IN        AMD_NB_CONFIG     *pConfig
  );

VOID
PcieLibCommonCoreInit (
  IN      CORE                CoreId,
  IN      AMD_NB_CONFIG       *pConfig
  );

LINK_INFO
PcieLibGetPortLinkInfo (
  IN        PORT              PortId,
  IN        AMD_NB_CONFIG     *pConfig
  );

VOID
PcieLibPowerOffPortLanes (
  IN      PORT                PortId,
  IN      PCIE_LINK_WIDTH     Width,
  IN      AMD_NB_CONFIG       *pConfig
  );

BOOLEAN
PcieLibIsPortReversed (
  IN        PORT              PortId,
  IN        AMD_NB_CONFIG     *pConfig
);

VOID
PcieLibPowerOffPll (
  IN      CORE  CoreId,
  IN      AMD_NB_CONFIG       *pConfig
  );

AGESA_STATUS
PcieInitSelectedPorts (
  IN      UINT16              SelectedPortMask,
  IN  OUT AMD_NB_CONFIG       *pConfig
  );

VOID
PcieLibCommonPortInit (
  IN      PORT                PortId,
  IN      AMD_NB_CONFIG       *pConfig
  );

VOID
PcieLibSetLinkMode (
  IN      PORT                PortId,
  IN      PCIE_LINK_MODE      Operation,
  IN      AMD_NB_CONFIG       *pConfig
  );

AGESA_STATUS
PciePreTrainingInit (
  IN  OUT AMD_NB_CONFIG       *pConfig
  );

AGESA_STATUS
PcieInitPorts (
  IN  OUT AMD_NB_CONFIG       *pConfig
  );

AGESA_STATUS
PcieAfterTrainingInit (
  IN      AMD_NB_CONFIG       *pConfig
  );

VOID
PcieLibUnHidePorts (
  IN      AMD_NB_CONFIG       *pConfig
  );

VOID
PcieLibHidePorts (
  IN      AMD_NB_CONFIG       *pConfig
  );

AGESA_STATUS
PcieLibRequestPciReset (
  IN      AMD_NB_CONFIG       *pConfig
  );

AGESA_STATUS
PcieLibResetSlot (
  IN      PORT                PortId,
  IN      AMD_NB_CONFIG       *pConfig
  );


VOID
PcieLibManageTxClock (
  IN      CORE                CoreId,
  IN      AMD_NB_CONFIG       *pConfig
  );

VOID
PcieLibManageLclkClock (
  IN      CORE                CoreId,
  IN      AMD_NB_CONFIG       *pConfig
  );

VOID
PcieLibEnablePllPowerOffInL1 (
  IN      CORE  CoreId,
  IN      AMD_NB_CONFIG       *pConfig
  );


CORE
PcieLibGetCoreId (
  IN      PORT                PortId,
  IN      AMD_NB_CONFIG       *pConfig
  );

UINT32
PcieLibGetCoreAddress (
  IN      CORE                CoreId,
  IN      AMD_NB_CONFIG       *pConfig
  );


AGESA_STATUS
AmdPcieInitializer (
  IN OUT  AMD_NB_CONFIG_BLOCK *ConfigPtr
  );

AGESA_STATUS
PcieLibInitializer (
  IN  OUT AMD_NB_CONFIG       *pConfig
  );


BOOLEAN
PcieLibIsValidPortId (
  IN      PORT                PortId,
  IN      AMD_NB_CONFIG       *pConfig
);

BOOLEAN
PcieLibIsValidCoreId (
  IN      CORE                CoreId,
  IN      AMD_NB_CONFIG       *pConfig
);

VOID
PcieLibPreTrainingInit (
  IN      AMD_NB_CONFIG       *pConfig
  );

VOID
PcieLibMiscLateCoreSetting (
  IN      CORE  CoreId,
  IN      AMD_NB_CONFIG       *pConfig
  );


AGESA_STATUS
PcieLateValidateConfiguration (
  IN      AMD_NB_CONFIG       *pConfig
  );

VOID
PcieForcePortsVisibleOrDisable (
  IN      AMD_NB_CONFIG       *NbConfigPtr
  );

/*
VOID
PcieReportCoreDisableStatus (
  IN      AMD_NB_CONFIG       *NbConfigPtr
  );
*/

AGESA_STATUS
PcieLateInitCores (
  IN      AMD_NB_CONFIG       *pConfig
  );

AGESA_STATUS
PcieLateCommonPortInit (
  IN      PORT                PortId,
  IN      AMD_NB_CONFIG       *pConfig
  );


VOID
PcieLibStrapModeControl (
  IN      CORE                CoreId,
  IN      PCIE_STRAP_MODE     Operation,
  IN  OUT AMD_NB_CONFIG       *pConfig
  );

VOID
PcieLibCoreAfterTrainingInit (
  IN      CORE                CoreId,
  IN      AMD_NB_CONFIG       *pConfig
  );

PORT_STATIC_INFO*
PcieLibGetStaticPortInfo (
  IN      PORT                PortId,
  IN  OUT AMD_NB_CONFIG       *pConfig
  );

CORE_INFO*
PcieLibGetCoreInfo (
  IN      CORE                CoreId,
  IN      AMD_NB_CONFIG       *pConfig
  );

PORT
PcieLibNativePortId (
  IN      PORT                PortId,
  IN  OUT AMD_NB_CONFIG       *pConfig
  );


VOID
PcieNbSbSetupVc (
  IN      AMD_NB_CONFIG       *pConfig
 );


VOID
PcieLibLateInit (
  IN      AMD_NB_CONFIG       *pConfig
 );

VOID
PcieLibValidatePortStateInit (
  IN      AMD_NB_CONFIG     *pConfig
  );

VOID
PcieInitiateSoftwareGen2 (
  IN      PORT                PortId,
  IN      AMD_NB_CONFIG       *pConfig
 );

AGESA_STATUS
PcieRecoveryInitializer (
  IN OUT  AMD_NB_CONFIG       *NbConfigPtr
  );

PCI_CORE_RESET
PcieLibCoreReset (
  IN      CORE                CoreId,
  IN      PCI_CORE_RESET      Operation,
  IN      AMD_NB_CONFIG       *pConfig
 );


PORT_INFO*
PcieLibGetPortInfo (
  IN       PORT               PortId,
  IN  OUT  AMD_NB_CONFIG      *pConfig
  );

PCIE_DEVICE_TYPE
PcieGetDeviceType (
  IN      PCI_ADDR            Device,
  IN      AMD_NB_CONFIG       *pConfig
  );

UINT8
PcieLibGetActiveCoreMap (
  IN      AMD_NB_CONFIG       *pConfig
  );

#pragma pack (pop)
#endif