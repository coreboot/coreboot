/**
 * @file
 *
 * ASPM support.
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
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "NbPlatform.h"
#include "amdDebugOutLib.h"
#include "amdSbLib.h"

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
PcieAspmSetOnRc (
  IN      PCI_ADDR        Device,
  IN      UINT8           Lx,
  IN      AMD_NB_CONFIG   *pConfig
  );

SCAN_STATUS
PcieSetDeviceAspm (
  IN      PCI_SCAN_PROTOCOL   *This,
  IN      PCI_ADDR            Function
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Init Port ASPM.
 * Enable ASPM states on RC and EP. Only states supported  by both RC and EP
 * will be enabled.
 *
 *
 * @param[in] PortId    Pcie Port ID
 * @param[in] AsmpState ASPM states to enable.
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
PcieAsmpEnableOnPort (
  IN      PORT            PortId,
  IN      PCIE_LINK_ASPM  AsmpState,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  PCI_ADDR  Port;
  UINT8     Lx;
  PCI_ADDR  NbPciAddress;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "PcieAsmpEnableOnPort Enter PortId [%d]\n", PortId));
  NbPciAddress.AddressValue = NB_SBDFO;
  switch (AsmpState) {
  case PcieLinkAspmDisabled:
    return ;
  case PcieLinkAspmL0s:
    Lx = ASPM_UPSTREAM_L0s | ASPM_DOWNSTREAM_L0s;
    break;
  case PcieLinkAspmL1:
    Lx = ASPM_L1;
    break;
  case PcieLinkAspmL0sAndL1:
    Lx = ASPM_UPSTREAM_L0s | ASPM_DOWNSTREAM_L0s | ASPM_L1;
    break;
  case PcieLinkAspmL0sDownstreamOnly:
    Lx = ASPM_DOWNSTREAM_L0s;
    break;
  case PcieLinkAspmL0sDownstreamOnlyAndL1:
    Lx = ASPM_DOWNSTREAM_L0s | ASPM_L1;
    break;
  default:
    CIMX_ASSERT (FALSE);
    return ;
  }
  Port = PcieLibGetPortPciAddress (PortId, pConfig);
//NB-SB link
  if (PortId == 8 && NbPciAddress.AddressValue == 0) {
    if (PcieSbInitAspm ((Lx & ASPM_L1) | ((Lx & ASPM_UPSTREAM_L0s)?ASPM_L0s:0), pConfig) == AGESA_SUCCESS) {
      PcieAspmEnableOnFunction (Port, (Lx & ASPM_L1) | ((Lx & ASPM_DOWNSTREAM_L0s)?ASPM_L0s:0), pConfig);
    }
    return ;
  }
  PcieAspmSetOnRc (Port, Lx, pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "PcieAsmpEnableOnPort Exit. Lx[0x%x]\n", Lx));
  return ;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Enable Common Clock on PCIe Link
 *
 *
 *
 * @param[in] Downstream  Downstream PCIe port PCI address
 * @param[in] Upstream    Upstream PCIe port PCI address
 * @param[in] pConfig     Northbridge configuration structure pointer.
 *
 */
 /*----------------------------------------------------------------------------------------*/

VOID
PcieAspmEnableCommonClock (
  IN      PCI_ADDR        Downstream,
  IN      PCI_ADDR        Upstream,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  UINT16  DownstreamCommonClockCap;
  UINT16  UpstreamCommonClockCap;
  UINT16  Value;
  UINT8   DownstreamPcieCapPtr;
  UINT8   UpstreamPcieCapPtr;
  DownstreamPcieCapPtr = LibNbFindPciCapability (Downstream.AddressValue, PCIE_CAP_ID, pConfig);
  UpstreamPcieCapPtr = LibNbFindPciCapability (Upstream.AddressValue, PCIE_CAP_ID, pConfig);
  if (DownstreamPcieCapPtr == 0 || UpstreamPcieCapPtr == 0) {
    return ;
  }
  LibNbPciRead (Downstream.AddressValue | (DownstreamPcieCapPtr + 0x10) , AccessWidth16, &DownstreamCommonClockCap, pConfig);
  if ((DownstreamCommonClockCap & BIT6) != 0) {
    //Aready enabled
    return ;
  }
  LibNbPciRead (Downstream.AddressValue | (DownstreamPcieCapPtr + 0x12) , AccessWidth16, &DownstreamCommonClockCap, pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    Downstream Common Clock Capability %d:%d:%d - %x\n", Downstream.Address.Bus, Downstream.Address.Device, Downstream.Address.Function,  DownstreamCommonClockCap));
  LibNbPciRead (Upstream.AddressValue | (UpstreamPcieCapPtr + 0x12) , AccessWidth16, &UpstreamCommonClockCap, pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    Upstream   Common Clock Capability %d:%d:%d - %x\n", Upstream.Address.Bus, Upstream.Address.Device, Upstream.Address.Function,  UpstreamCommonClockCap));
  if ((DownstreamCommonClockCap & UpstreamCommonClockCap & BIT12) != 0) {
    //Enable common clock
    PcieAspmCommonClockOnFunction (Downstream, pConfig);
    PcieAspmCommonClockOnDevice (Upstream, pConfig);
//    LibNbPciRMW (Downstream.AddressValue | (DownstreamPcieCapPtr + 0x10) , AccessS3SaveWidth8, 0xff, BIT6, pConfig);
//    LibNbPciRMW (Upstream.AddressValue | (UpstreamPcieCapPtr + 0x10) , AccessS3SaveWidth8, 0xff, BIT6, pConfig);
    //Reatrain link
    LibNbPciRMW (Downstream.AddressValue | (DownstreamPcieCapPtr + 0x10) , AccessS3SaveWidth8, 0xff, BIT5, pConfig);
    do {
      LibNbPciRead (Downstream.AddressValue | (DownstreamPcieCapPtr + 0x12) , AccessWidth16, (UINT16*)&Value, pConfig);
      STALL (GET_BLOCK_CONFIG_PTR (pConfig), 200, CIMX_S3_SAVE);
    } while ((Value & BIT11) != 0);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set  "Common Clock" enable on function
 *
 *
 *
 * @param[in] Device      PCI address of function.
 * @param[in] pConfig     Northbridge configuration structure pointer.
 *
 */
 /*----------------------------------------------------------------------------------------*/
VOID
PcieAspmCommonClockOnDevice (
  IN      PCI_ADDR        Device,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  UINT8 Value;
  UINT8 MaxFunc;
  UINT8 CurrentFunc;
  LibNbPciRead (Device.AddressValue | 0x0E , AccessWidth8, &Value, pConfig);
  MaxFunc = (Value & BIT7)?7:0;
  for (CurrentFunc = 0; CurrentFunc <= MaxFunc; CurrentFunc++) {
    Device.Address.Function = CurrentFunc;
    if (LibNbIsDevicePresent (Device, pConfig)) {
      PcieAspmCommonClockOnFunction (Device, pConfig);
    }
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Set  "Common Clock" enable on function
 *
 *
 *
 * @param[in] Function    PCI address of function.
 * @param[in] pConfig     Northbridge configuration structure pointer.
 *
 */
 /*----------------------------------------------------------------------------------------*/
VOID
PcieAspmCommonClockOnFunction (
  IN      PCI_ADDR        Function,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  UINT8       PcieCapPtr;
  PcieCapPtr = LibNbFindPciCapability (Function.AddressValue, PCIE_CAP_ID, pConfig);
  if (PcieCapPtr != 0) {
    LibNbPciRMW (Function.AddressValue | (PcieCapPtr + 0x10) , AccessS3SaveWidth8, (UINT32)~(BIT6), BIT6, pConfig);
  }
}
/*----------------------------------------------------------------------------------------*/
/**
 * Enable ASPM on PCIe Link
 *
 *
 *
 * @param[in] Downstream  Downstream PCIe port PCI address
 * @param[in] Upstream    Upstream PCIe port PCI address
 * @param[in] Lx          Lx ASPM bitmap.
 *                          Lx[0] - reserved
 *                          Lx[1] - L1 enable
 *                          Lx[2] - L0s enable for upstream ports
 *                          Lx[3] - L0s enable for downstream ports
 * @param[in] pConfig     Northbridge configuration structure pointer.
 *
 */
 /*----------------------------------------------------------------------------------------*/

VOID
PcieAspmEnableOnLink (
  IN      PCI_ADDR        Downstream,
  IN      PCI_ADDR        Upstream,
  IN      UINT8           Lx,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  ASPM_LINK_INFO AspmLinkInfo;
  AspmLinkInfo.UpstreamLxCap = PcieAspmGetPmCapability (Upstream, pConfig);
  AspmLinkInfo.DownstreamLxCap = PcieAspmGetPmCapability (Downstream, pConfig);
  AspmLinkInfo.DownstreamPort = Downstream;
  AspmLinkInfo.UpstreamPort = Upstream;
  AspmLinkInfo.RequestedLx = Lx;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "   Downstream ASPM Capability %d:%d:%d - %x\n", Downstream.Address.Bus, Downstream.Address.Device, Downstream.Address.Function,  AspmLinkInfo.DownstreamLxCap));
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "   Upstream   ASPM Capability %d:%d:%d - %x\n", Upstream.Address.Bus, Upstream.Address.Device, Upstream.Address.Function,  AspmLinkInfo.UpstreamLxCap));
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "   Requested  ASPM State - %x\n", Lx));
  AspmLinkInfo.DownstreamLx = AspmLinkInfo.UpstreamLxCap & AspmLinkInfo.DownstreamLxCap & Lx & ASPM_L1;
  AspmLinkInfo.UpstreamLx = AspmLinkInfo.DownstreamLx;
  if ((AspmLinkInfo.UpstreamLxCap & ASPM_L0s) != 0 && (Lx & ASPM_UPSTREAM_L0s) != 0) {
    AspmLinkInfo.UpstreamLx |= ASPM_L0s;
  }
  if ((AspmLinkInfo.DownstreamLxCap & ASPM_L0s) != 0 && (Lx & ASPM_DOWNSTREAM_L0s) != 0) {
    AspmLinkInfo.DownstreamLx |= ASPM_L0s;
  }
#ifndef ASPM_WORKAROUND_DISABLE
  PcieAspmWorkarounds (&AspmLinkInfo, pConfig);
#endif
  LibNbCallBack (PHCB_AmdPcieAsmpInfo, (UINTN)&AspmLinkInfo, pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "   Upstream   ASPM State - %x\n", AspmLinkInfo.UpstreamLx));
  PcieAspmEnableOnDevice (Upstream, AspmLinkInfo.UpstreamLx, pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "   Downstream ASPM State - %x\n", AspmLinkInfo.DownstreamLx));
  PcieAspmEnableOnFunction (Downstream, AspmLinkInfo.DownstreamLx, pConfig);

}

/*----------------------------------------------------------------------------------------*/
/**
 * Set ASMP State on all function of PCI device
 *
 *
 *
 * @param[in] Device    PCI address of device.
 * @param[in] Lx        Lx ASPM bitmap.
 *                        Lx[0] = L0s enable
 *                        Lx[1] - L1 enable
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 */
 /*----------------------------------------------------------------------------------------*/
VOID
PcieAspmEnableOnDevice (
  IN      PCI_ADDR        Device,
  IN      UINT8           Lx,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  UINT8 Value;
  UINT8 MaxFunc;
  UINT8 CurrentFunc;

  LibNbPciRead (Device.AddressValue | 0x0E , AccessWidth8, &Value, pConfig);
  MaxFunc = (Value & BIT7)?7:0;
  for (CurrentFunc = 0; CurrentFunc <= MaxFunc; CurrentFunc++) {
    Device.Address.Function = CurrentFunc;
    if (LibNbIsDevicePresent (Device, pConfig)) {
      PcieAspmEnableOnFunction (Device, Lx, pConfig);
    }
  }
}
/*----------------------------------------------------------------------------------------*/
/**
 * Set  ASMP State on PCIe device function
 *
 *
 *
 * @param[in] Function    PCI address of function.
 * @param[in] Lx          Lx ASPM bitmap.
 *                          Lx[0] = L0s enable
 *                          Lx[1] - L1 enable
 * @param[in] pConfig     Northbridge configuration structure pointer.
 *
 */
 /*----------------------------------------------------------------------------------------*/
VOID
PcieAspmEnableOnFunction (
  IN      PCI_ADDR        Function,
  IN      UINT8           Lx,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  UINT8       PcieCapPtr;
  PcieCapPtr = LibNbFindPciCapability (Function.AddressValue, PCIE_CAP_ID, pConfig);
  if (PcieCapPtr != 0) {
    LibNbPciRMW (Function.AddressValue | (PcieCapPtr + 0x10) , AccessS3SaveWidth8, (UINT32)~(BIT0 & BIT1), Lx, pConfig);
  }
}

/**----------------------------------------------------------------------------------------*/
/**
 * Port/Endpoint ASMP capability
 *
 *
 *
 * @param[in] Device    PCI address of downstream port.
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 * @retval    Bitmap of actual supported Lx states
 */
 /*----------------------------------------------------------------------------------------*/
UINT8
PcieAspmGetPmCapability (
  IN      PCI_ADDR        Device,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  UINT8 PcieCapPtr;
  UINT8 Value;
  PcieCapPtr = LibNbFindPciCapability (Device.AddressValue, PCIE_CAP_ID, pConfig);
  if (PcieCapPtr == 0) {
    return 0;
  }
  LibNbPciRead (Device.AddressValue | (PcieCapPtr + 0x0D) , AccessWidth8, &Value, pConfig);
  return (Value >> 2) & 3;
}


/**----------------------------------------------------------------------------------------*/
/**
 * Scan PCIe topology
 *
 *
 *
 * @param[in] This          Pointer to instance of scan protocol
 * @param[in] Function      PCI address of found device/function.
 *
 * @retval    SCAN_FINISHED Scan for device finished.
 */
 /*----------------------------------------------------------------------------------------*/
SCAN_STATUS
PcieSetDeviceAspm (
  IN      PCI_SCAN_PROTOCOL   *This,
  IN      PCI_ADDR            Function
  )
{
  PCIE_DEVICE_TYPE  DeviceType;
  UINT8             SecondaryBus;
  ASPM_WORKSPACE    *WorkspacePtr;
  WorkspacePtr =    (ASPM_WORKSPACE*)This;

  DeviceType = PcieGetDeviceType (Function, This->pConfig);
  if (DeviceType == PcieDeviceRootComplex || DeviceType == PcieDeviceDownstreamPort) {
    PCI_ADDR        UpstreamDevice;
    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (This->pConfig), CIMX_NBPCIE_TRACE), "    Reached downstream port\n"));
    //Lets enable Common clock
    LibNbPciRead (Function.AddressValue | 0x19, AccessWidth8, &SecondaryBus, This->pConfig);
    LibNbPciRMW(Function.AddressValue | 0x18, AccessS3SaveWidth32, 0xffffffff, 0, This->pConfig);  //This done to help UEFI bootscript restore bud topology.
    if (SecondaryBus == 0) {
      return  SCAN_FINISHED;
    }
    //New Downstream Port
    WorkspacePtr->LinkCount++;
    if (WorkspacePtr->DownstreamPort.AddressValue == 0) {
      WorkspacePtr->DownstreamPort.AddressValue = Function.AddressValue;
    }
    //Lets enable Common clock
    UpstreamDevice.AddressValue = MAKE_SBDFO (0, SecondaryBus, 0, 0, 0);
    if (LibNbIsDevicePresent (UpstreamDevice, This->pConfig)) {
      PcieAspmEnableCommonClock (Function, UpstreamDevice, This->pConfig);
    }
    This->ScanBus (This, UpstreamDevice);
    if (WorkspacePtr->DownstreamPort.AddressValue == Function.AddressValue) {
      WorkspacePtr->DownstreamPort.AddressValue = 0;
      PcieAspmEnableOnLink (Function, UpstreamDevice, WorkspacePtr->Lx, This->pConfig);
    }
  } else if (DeviceType == PcieDeviceUpstreamPort ) {
    PCI_ADDR DownstreamDevice;

    if (WorkspacePtr->DownstreamPort.AddressValue == 0) {
      return  SCAN_FINISHED;
    }
    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (This->pConfig), CIMX_NBPCIE_TRACE), "    Reached upstream port\n"));
    LibNbPciRead (Function.AddressValue | 0x19, AccessWidth8, &SecondaryBus, This->pConfig);
    LibNbPciRMW(Function.AddressValue | 0x18, AccessS3SaveWidth32, 0xffffffff, 0, This->pConfig); //This done to help UEFI bootscript restore bud topology.
    if (SecondaryBus == 0) {
      return  SCAN_FINISHED;
    }
    DownstreamDevice.AddressValue = MAKE_SBDFO (0, SecondaryBus, 0, 0, 0);
    This->ScanBus (This, DownstreamDevice);
  } else if (DeviceType < PcieDeviceLegacyEndPoint) {
    // We reach end of link @toDo code to check exit latency.
    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (This->pConfig), CIMX_NBPCIE_TRACE), "    Reached endpoint \n"));
  }
  return  SCAN_FINISHED;
}

/**----------------------------------------------------------------------------------------*/
/**
 * Scan RC PCIe topology to setup ASPM
 *
 *
 *
 * @param[in] Device    PCI address of downstream port.
 * @param[in] Lx        Lx ASPM bitmap.
 *                        Lx[0] - reserved
 *                        Lx[1] - L1 enable
 *                        Lx[2] - L0s enable for upstream ports
 *                        Lx[3] - L0s enable for downstream ports
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 */
 /*----------------------------------------------------------------------------------------*/
VOID
PcieAspmSetOnRc (
  IN      PCI_ADDR        Device,
  IN      UINT8           Lx,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  ASPM_WORKSPACE AspmWorkspace;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieSetPortAspm Enter\n"));
  LibAmdMemFill (&AspmWorkspace, 0, sizeof (AspmWorkspace), (AMD_CONFIG_PARAMS *)&(pConfig->sHeader));
  AspmWorkspace.ScanPciePort.pConfig = pConfig;
  AspmWorkspace.ScanPciePort.ScanBus = LibNbScanPciBus;
  AspmWorkspace.ScanPciePort.ScanDevice = LibNbScanPciDevice;
  AspmWorkspace.ScanPciePort.ScanFunction = PcieSetDeviceAspm;
  AspmWorkspace.Lx = Lx;
  AspmWorkspace.ScanPciePort.ScanFunction (&AspmWorkspace.ScanPciePort, Device);
  if (AspmWorkspace.LinkCount > 1) {
    LibNbScanPciBridgeBuses (&AspmWorkspace.ScanPciePort, Device);
  }
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieSetPortAspm Exit\n"));
}
