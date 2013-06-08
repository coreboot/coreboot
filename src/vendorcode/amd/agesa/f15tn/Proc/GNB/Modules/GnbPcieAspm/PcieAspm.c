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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV1_PCIEASPM_FILECODE
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
} PCIE_ASPM_DATA;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
PcieAspmInterface (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );


SCAN_STATUS
PcieAspmCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  );

VOID
PcieAspmEnableOnLink (
  IN       PCI_ADDR                 Downstream,
  IN       PCI_ADDR                 Upstream,
  IN       PCIE_ASPM_TYPE           Aspm,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

PCIE_ASPM_TYPE
PcieAspmGetPmCapability (
  IN       PCI_ADDR                 Device,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Enable PCIE Advance state power management
 *
 *
 *
 * @param[in] DownstreamPort      PCI Address of the downstream port
 * @param[in] Aspm                ASPM type
 * @param[in] StdHeader           Standard configuration header
 * @retval     AGESA_STATUS
 */

VOID
PcieLinkAspmEnable (
  IN       PCI_ADDR                DownstreamPort,
  IN       PCIE_ASPM_TYPE          Aspm,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  PCIE_ASPM_DATA PcieAspmData;
  PcieAspmData.Aspm = Aspm;
  PcieAspmData.ScanData.StdHeader = StdHeader;
  PcieAspmData.ScanData.GnbScanCallback = PcieAspmCallback;
  GnbLibPciScan (DownstreamPort, DownstreamPort, &PcieAspmData.ScanData);
}

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
    break;
  case  PcieDeviceUpstreamPort:
    PcieAspmEnableOnLink (
      PcieAspmData->DownstreamPort,
      Device,
      PcieAspmData->Aspm,
      ScanData->StdHeader
      );
    GnbLibPciRMW (Device.AddressValue | 0x18, AccessS3SaveWidth32, 0xffffffffull, 0x0, ScanData->StdHeader);
    GnbLibPciScanSecondaryBus (Device, &PcieAspmData->ScanData);
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    PcieAspmEnableOnLink (
      PcieAspmData->DownstreamPort,
      Device,
      PcieAspmData->Aspm,
      ScanData->StdHeader
      );
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  default:
    break;
  }
  return ScanStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set  ASMP State on PCIe device function
 *
 *
 *
 * @param[in] Function    PCI address of function.
 * @param[in] Aspm                Aspm capability to enable
 * @param[in] StdHeader           Standard configuration header
 *
 */
 /*----------------------------------------------------------------------------------------*/
VOID
PcieAspmEnableOnFunction (
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

/*----------------------------------------------------------------------------------------*/
/**
 * Set ASMP State on all function of PCI device
 *
 *
 *
 * @param[in] Device    PCI address of device.
 * @param[in] Aspm                Aspm capability to enable
 * @param[in] StdHeader           Standard configuration header
 *
 */
 /*----------------------------------------------------------------------------------------*/
STATIC VOID
PcieAspmEnableOnDevice (
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
      PcieAspmEnableOnFunction (Device, Aspm, StdHeader);
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 *  Enable ASPM on link
 *
 *
 *
 * @param[in] Downstream          PCI Address of downstrteam port
 * @param[in] Upstream            PCI Address of upstream port
 * @param[in] Aspm                Aspm capability to enable
 * @param[in] StdHeader           Standard configuration header
 */

VOID
PcieAspmEnableOnLink (
  IN       PCI_ADDR                 Downstream,
  IN       PCI_ADDR                 Upstream,
  IN       PCIE_ASPM_TYPE           Aspm,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  PCIe_LINK_ASPM  LinkAsmp;
  PCIE_ASPM_TYPE  DownstreamCap;
  PCIE_ASPM_TYPE  UpstreamCap;
  LinkAsmp.DownstreamPort = Downstream;
  DownstreamCap = PcieAspmGetPmCapability (Downstream, StdHeader);
  LinkAsmp.UpstreamPort = Upstream;
  UpstreamCap = PcieAspmGetPmCapability (Upstream, StdHeader);
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
  PcieAspmEnableOnDevice (Upstream, AspmDisabled, StdHeader);
  // Enable  ASPM  Donstream component
  PcieAspmEnableOnFunction (Downstream, LinkAsmp.DownstreamAspm, StdHeader);
  // Enable  ASPM  Upstream component
  PcieAspmEnableOnDevice (Upstream, LinkAsmp.UpstreamAspm, StdHeader);
}



/**----------------------------------------------------------------------------------------*/
/**
 * Port/Endpoint ASMP capability
 *
 *
 *
 * @param[in] Device      PCI address of downstream port
 * @param[in] StdHeader   Standard configuration header
 *
 * @retval    PCIE_ASPM_TYPE
 */
 /*----------------------------------------------------------------------------------------*/
PCIE_ASPM_TYPE
PcieAspmGetPmCapability (
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
    PcieLinkAspmEnable (
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
