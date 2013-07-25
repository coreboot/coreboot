/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe link ASPM
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
#include  "Gnb.h"
#include  "GnbPcieConfig.h"
#include  "OptionGnb.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieInitLibV1.h"
#include  "PcieAspmBlackList.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEASPM_PCIEASPM_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_BUILD_OPTIONS GnbBuildOptions;
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
  GNB_PCI_SCAN_DATA       ScanData;
  PCIE_ASPM_TYPE          Aspm;
  PCI_ADDR                DownstreamPort;
  BOOLEAN                 AspmL0sBlackList;
} PCIE_ASPM_DATA;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


SCAN_STATUS
PcieAspmCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  );

VOID
excel950_fun0 (
  IN       PCI_ADDR                 Downstream,
  IN       PCI_ADDR                 Upstream,
  IN       PCIE_ASPM_TYPE           Aspm,
  IN OUT   BOOLEAN                  *AspmL0sBlackList,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

PCIE_ASPM_TYPE
excel950_fun1 (
  IN       PCI_ADDR                 Device,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

VOID
excel950_fun2 (
  IN       PCI_ADDR                DownstreamPort,
  IN       PCIE_ASPM_TYPE          Aspm,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  PCIE_ASPM_DATA PcieAspmData;
  PcieAspmData.Aspm = Aspm;
  PcieAspmData.ScanData.StdHeader = StdHeader;
  PcieAspmData.ScanData.GnbScanCallback = PcieAspmCallback;
  PcieAspmData.AspmL0sBlackList = FALSE;
  GnbLibPciScan (DownstreamPort, DownstreamPort, &PcieAspmData.ScanData);
}

AGESA_STATUS
PcieAspmInterface (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Evaluate device
 *
 *
 *
 * @param[in]     Device          PCI Address
 * @param[in,out] ScanData        Scan configuration data
 * @retval                        Scan Status of 0
 */

SCAN_STATUS
PcieAspmCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS             ScanStatus;
  PCIE_ASPM_DATA          *PcieAspmData;
  PCIE_DEVICE_TYPE        DeviceType;
  ScanStatus = SCAN_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieAspmCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    );
  PcieAspmData = (PCIE_ASPM_DATA *) ScanData;
  ScanStatus = SCAN_SUCCESS;
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
    PcieAspmData->DownstreamPort = Device;
    //PcieExitLatencyData->LinkCount++;
    GnbLibPciRMW (Device.AddressValue | 0x18, AccessS3SaveWidth32, 0xffffffffull, 0x0, ScanData->StdHeader);
    GnbLibPciScanSecondaryBus (Device, &PcieAspmData->ScanData);
    //PcieExitLatencyData->LinkCount--;

    //Pcie ASPM Black List for L0s with HW method change
    if ((DeviceType == PcieDeviceRootComplex) && (PcieAspmData->AspmL0sBlackList == TRUE)) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  Black List L0s disabled = %d:%d:%d\n", Device.Address.Bus, Device.Address.Device, Device.Address.Function);
      GnbLibPciIndirectRMW (Device.AddressValue | 0xE0, 0xA0, AccessS3SaveWidth32, 0xfffff0ff, 0, ScanData->StdHeader);
    }
    break;
  case  PcieDeviceUpstreamPort:
    excel950_fun0 (
      PcieAspmData->DownstreamPort,
      Device,
      PcieAspmData->Aspm,
      &PcieAspmData->AspmL0sBlackList,
      ScanData->StdHeader
      );
    GnbLibPciRMW (Device.AddressValue | 0x18, AccessS3SaveWidth32, 0xffffffffull, 0x0, ScanData->StdHeader);
    GnbLibPciScanSecondaryBus (Device, &PcieAspmData->ScanData);
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    excel950_fun0 (
      PcieAspmData->DownstreamPort,
      Device,
      PcieAspmData->Aspm,
      &PcieAspmData->AspmL0sBlackList,
      ScanData->StdHeader
      );
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  default:
    break;
  }
  return ScanStatus;
}

VOID
excel950_fun4 (
  IN       PCI_ADDR             Function,
  IN       PCIE_ASPM_TYPE       Aspm,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINT8       PcieCapPtr;
  PcieCapPtr = GnbLibFindPciCapability (Function.AddressValue, PCIE_CAP_ID, StdHeader);
  if (PcieCapPtr != 0) {
    GnbLibPciRMW (
      Function.AddressValue | (PcieCapPtr + PCIE_LINK_CTRL_REGISTER) ,
      AccessS3SaveWidth8,
      (UINT32)~(BIT0 | BIT1),
      Aspm,
      StdHeader
      );
  }
}

STATIC VOID
excel950_fun5 (
  IN       PCI_ADDR             Device,
  IN       PCIE_ASPM_TYPE       Aspm,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINT8 MaxFunc;
  UINT8 CurrentFunc;
  MaxFunc = GnbLibPciIsMultiFunctionDevice (Device.AddressValue, StdHeader) ? 7 : 0;
  for (CurrentFunc = 0; CurrentFunc <= MaxFunc; CurrentFunc++) {
    Device.Address.Function = CurrentFunc;
    if (GnbLibPciIsDevicePresent (Device.AddressValue, StdHeader)) {
      excel950_fun4 (Device, Aspm, StdHeader);
    }
  }
}

VOID
excel950_fun0 (
  IN       PCI_ADDR                 Downstream,
  IN       PCI_ADDR                 Upstream,
  IN       PCIE_ASPM_TYPE           Aspm,
  IN OUT   BOOLEAN                  *AspmL0sBlackList,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  PCIe_LINK_ASPM  LinkAsmp;
  PCIE_ASPM_TYPE  DownstreamCap;
  PCIE_ASPM_TYPE  UpstreamCap;
  LinkAsmp.DownstreamPort = Downstream;
  DownstreamCap = excel950_fun1 (Downstream, StdHeader);
  LinkAsmp.UpstreamPort = Upstream;
  UpstreamCap = excel950_fun1 (Upstream, StdHeader);
  LinkAsmp.DownstreamAspm = DownstreamCap & UpstreamCap & Aspm & AspmL1;
  LinkAsmp.UpstreamAspm = LinkAsmp.DownstreamAspm;
  LinkAsmp.RequestedAspm = Aspm;
  if ((UpstreamCap & Aspm & AspmL0s) != 0) {
    LinkAsmp.UpstreamAspm |= AspmL0s;
  }
  if ((DownstreamCap & Aspm & AspmL0s) != 0) {
    LinkAsmp.DownstreamAspm |= AspmL0s;
  }
  if (GnbBuildOptions.PcieAspmBlackListEnable == 1) {
    PcieAspmBlackListFeature (&LinkAsmp, StdHeader);
    if ((LinkAsmp.DownstreamAspm & AspmL0s) == 0) {
      *AspmL0sBlackList = LinkAsmp.BlackList;
    }
  }
  //AgesaPcieLinkAspm (&LinkAsmp, StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "  Set ASPM [%d]  for Device = %d:%d:%d\n",
    (LinkAsmp.UpstreamAspm) ,
    LinkAsmp.UpstreamPort.Address.Bus,
    LinkAsmp.UpstreamPort.Address.Device,
    LinkAsmp.UpstreamPort.Address.Function
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "  Set ASPM [%d]  for Device = %d:%d:%d\n",
    (LinkAsmp.DownstreamAspm) ,
    LinkAsmp.DownstreamPort.Address.Bus,
    LinkAsmp.DownstreamPort.Address.Device,
    LinkAsmp.DownstreamPort.Address.Function
    );
  // Disable ASPM  Upstream component
  excel950_fun5 (Upstream, AspmDisabled, StdHeader);
  // Enable  ASPM  Donstream component
  excel950_fun4 (Downstream, LinkAsmp.DownstreamAspm, StdHeader);
  // Enable  ASPM  Upstream component
  excel950_fun5 (Upstream, LinkAsmp.UpstreamAspm, StdHeader);
}



PCIE_ASPM_TYPE
excel950_fun1 (
  IN       PCI_ADDR                 Device,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  UINT8   PcieCapPtr;
  UINT32  Value;
  PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, StdHeader);
  if (PcieCapPtr == 0) {
    return 0;
  }
  GnbLibPciRead (
    Device.AddressValue | (PcieCapPtr + PCIE_LINK_CAP_REGISTER),
    AccessWidth32,
    &Value,
    StdHeader
    );
  return (Value >> 10) & 3;
}

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
PcieAspmPortInitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  if (Engine->Type.Port.PortData.LinkAspm != AspmDisabled &&
      !PcieConfigIsSbPcieEngine (Engine) &&
      PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
    excel950_fun2 (
      Engine->Type.Port.Address,
      Engine->Type.Port.PortData.LinkAspm,
      GnbLibGetHeader (Pcie)
      );
  }
}


/**----------------------------------------------------------------------------------------*/
/**
 * Interface to enable Clock Power Managment
 *
 *
 *
 * @param[in] StdHeader   Standard configuration header
 *
 * @retval    AGESA_STATUS
 */
 /*----------------------------------------------------------------------------------------*/
AGESA_STATUS
PcieAspmInterface (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  AGESA_STATUS          AgesaStatus;
  PCIe_PLATFORM_CONFIG  *Pcie;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAspmInterface Enter\n");
  AgesaStatus = PcieLocateConfigurationData (StdHeader, &Pcie);
  if (AgesaStatus == AGESA_SUCCESS) {
    PcieConfigRunProcForAllEngines (
      DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
      PcieAspmPortInitCallback,
      NULL,
      Pcie
      );
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAspmInterface Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}
