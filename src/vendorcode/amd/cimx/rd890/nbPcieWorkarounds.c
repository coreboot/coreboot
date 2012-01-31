/**
 * @file
 *
 * Routines to support misc PCIe workarounds.
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
AGESA_STATUS
PcieConfigureBridgeResources (
  IN      PCI_ADDR          Port,
  IN      AMD_NB_CONFIG     *pConfig
  );

VOID
PcieFreeBridgeResources (
  IN      PCI_ADDR          Port,
  IN      AMD_NB_CONFIG     *pConfig
  );

 AGESA_STATUS
PcieDeskewWorkaround (
  IN      PCI_ADDR          Device,
  IN      AMD_NB_CONFIG     *pConfig
  );

AGESA_STATUS
PcieNvWorkaround (
  IN      PCI_ADDR          Device,
  IN      AMD_NB_CONFIG     *pConfig
  );

BOOLEAN
PcieIsDeskewCardDetected (
  IN      UINT16            DeviceId
  );

 /*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Misc GFX Card Workaround
 *  RV3780/RV380 desk workaround. NV43 lost SSID workaround.
 *
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
AGESA_STATUS
PcieGfxWorkarounds (
  IN      PORT            PortId,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  AGESA_STATUS  Status;
  UINT32        Count;
  UINT16        DeviceId;
  UINT16        VendorId;
  UINT8         DevClassCode;
  PCI_ADDR      Port;
  PCI_ADDR      Ep;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieGfxWorkarounds PortId %d Enter\n", PortId));
  Status = AGESA_SUCCESS;
  Port = PcieLibGetPortPciAddress (PortId, pConfig);
  Ep.AddressValue = MAKE_SBDFO (0, Port.Address.Bus + 5, 0, 0, 0);
  if (PcieConfigureBridgeResources (Port, pConfig) != AGESA_SUCCESS) {
    return AGESA_SUCCESS;
  }
  for (Count = 0; Count <= 5000; Count++) {
    LibNbPciRead (Ep.AddressValue | 0x02, AccessWidth16, &DeviceId, pConfig);
    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    Endpoint Device ID %x\n", DeviceId));
    if (DeviceId != 0xffff) {
      break;
    };
    STALL (GET_BLOCK_CONFIG_PTR (pConfig), 1000, 0);
  }
  if (Count >= 5000) {
    PcieLibRequestPciReset (pConfig);
    return  AGESA_WARNING;
  }
  LibNbPciRead (Ep.AddressValue | 0x02, AccessWidth16, &DeviceId, pConfig);
  LibNbPciRead (Ep.AddressValue, AccessWidth16, &VendorId, pConfig);
  if (VendorId == 0xffff) {
    PcieLibRequestPciReset (pConfig);
    return  AGESA_WARNING;
  }
  LibNbPciRead (Ep.AddressValue | 0x0B , AccessWidth8, &DevClassCode, pConfig);
  if (DevClassCode == 3) {
    if (VendorId == 0x1002 && PcieIsDeskewCardDetected (DeviceId)) {
      Status = PcieDeskewWorkaround (Ep, pConfig);
    } else {
      if (VendorId == 0x10DE) {
        Status = PcieNvWorkaround (Ep, pConfig);
      }
    }
  }
  PcieFreeBridgeResources (Port, pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieGfxWorkarounds Exit [Status = 0x%x]\n", Status));
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * RV370/RV380 Deskew workaround
 *
 *
 *
 * @param[in] Device    Pcie Address of ATI RV370/RV380 card.
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
AGESA_STATUS
PcieDeskewWorkaround (
  IN      PCI_ADDR        Device,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  PCIE_CONFIG *pPcieConfig;
  UINTN       MmioBase;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieDeskewWorkaround Enter\n"));
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  MmioBase = pPcieConfig->TempMmioBaseAddress << 20;
  if (MmioBase == 0) {
    return AGESA_SUCCESS;
  }
  LibNbPciWrite (Device.AddressValue | 0x18, AccessWidth32, &MmioBase, pConfig);
  LibNbPciRMW (Device.AddressValue | 0x04, AccessWidth8 , (UINT32)~BIT1, BIT1, pConfig);
  *(UINT16*)(MmioBase + 0x120) = 0xb700;
  if (*(UINT16*) (MmioBase + 0x120) == 0xb700) {
    *(UINT32*)(MmioBase + 0x124) = 0x13;
    if (*(UINT32*) (MmioBase + 0x124) == 0x13) {
      if (*(UINT32*) (MmioBase + 0x12C) & BIT8) {
//    TRACE((DMSG_PCIE_MISC,"Deskew ERROR Generate Reset\n"));
        PcieLibRequestPciReset (pConfig);
        return AGESA_WARNING;
      }
    }
  }
  LibNbPciRMW (Device.AddressValue | 0x04, AccessWidth8, (UINT32)~BIT1, 0x0, pConfig);
  LibNbPciRMW (Device.AddressValue | 0x18, AccessWidth32, 0x0, 0x0, pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieDeskewWorkaround Exit\n"));
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 *  NV43 card workaround (lost SSID)
 *
 *
 *
 * @param[in] Device    Pcie Address of NV43 card.
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
AGESA_STATUS
PcieNvWorkaround (
  IN      PCI_ADDR        Device,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  UINT32      DeviceSSID;
  PCIE_CONFIG *pPcieConfig;
  UINTN       MmioBase;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieNvWorkaround Enter\n"));
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG0B, AccessS3SaveWidth32, 0xffffffff, BIT2 + BIT1, pConfig);
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  MmioBase = pPcieConfig->TempMmioBaseAddress << 20;
  if (MmioBase == 0) {
    return AGESA_SUCCESS;
  }
  LibNbPciRMW (Device.AddressValue | 0x30, AccessWidth32, 0x0, ((UINT32)MmioBase) | 1, pConfig);
  LibNbPciRMW (Device.AddressValue | 0x4, AccessWidth8, 0x0, 0x2, pConfig);
  LibNbPciRead (Device.AddressValue | 0x2c, AccessWidth32, &DeviceSSID, pConfig);
  if (DeviceSSID != *(UINT32*) (MmioBase + 0x54)) {
    LibNbPciRMW (Device.AddressValue | 0x40, AccessWidth32, 0x0, *(UINT32*) (MmioBase + 0x54), pConfig);
  }
  LibNbPciRMW (Device.AddressValue | 0x30, AccessWidth32, 0x0, 0x0, pConfig);
  LibNbPciRMW (Device.AddressValue | 0x4, AccessWidth8, 0x0, 0x0, pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieNvWorkaround Exit\n"));
  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 *  Allocate temporary resources for Pcie P2P bridge
 *
 *
 *
 * @param[in] Port      Pci Address of Port to initialize.
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
AGESA_STATUS
PcieConfigureBridgeResources (
  IN      PCI_ADDR          Port,
  IN      AMD_NB_CONFIG     *pConfig
  )
{
  PCIE_CONFIG *pPcieConfig;
  UINT32      Value;
  UINT32      MmioBase;

  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  MmioBase = pPcieConfig->TempMmioBaseAddress << 20;
  if (MmioBase == 0) {
    return AGESA_WARNING;
  }
  Value = Port.Address.Bus + ((Port.Address.Bus + 5) << 8) + ((Port.Address.Bus + 5) << 16);
  LibNbPciWrite (Port.AddressValue | NB_PCIP_REG18, AccessWidth32, &Value, pConfig);
  Value = MmioBase + (MmioBase >> 16);
  LibNbPciWrite (Port.AddressValue | NB_PCIP_REG20, AccessWidth32, &Value, pConfig);
  Value = 0x000fff0;
  LibNbPciWrite (Port.AddressValue | NB_PCIP_REG24, AccessWidth32, &Value, pConfig);
  Value = 0x2;
  LibNbPciWrite (Port.AddressValue | NB_PCIP_REG04, AccessWidth8, &Value, pConfig);
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 *  Free temporary resources for Pcie P2P bridge
 *
 *
 *
 * @param[in] Port      Pci Address of Port to clear resource allocation.
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
VOID
PcieFreeBridgeResources (
  IN      PCI_ADDR          Port,
  IN      AMD_NB_CONFIG     *pConfig
  )
{
  UINT32 Value;

  Value = 0;
  LibNbPciWrite (Port.AddressValue | NB_PCIP_REG04, AccessWidth8, &Value, pConfig);
  LibNbPciWrite (Port.AddressValue | NB_PCIP_REG18, AccessWidth32, &Value, pConfig);
  LibNbPciWrite (Port.AddressValue | NB_PCIP_REG20, AccessWidth32, &Value, pConfig);
  LibNbPciWrite (Port.AddressValue | NB_PCIP_REG24, AccessWidth32, &Value, pConfig);

}

/*----------------------------------------------------------------------------------------*/
/*
 * Check if card required test for deskew workaround
 *
 *
 *
 *
 *
 */

BOOLEAN
PcieIsDeskewCardDetected (
  IN      UINT16    DeviceId
  )
{
  if ((DeviceId >= 0x3150 && DeviceId <= 0x3152) || (DeviceId == 0x3154) ||
     (DeviceId == 0x3E50) || (DeviceId == 0x3E54) ||
     ((DeviceId & 0xfff8) == 0x5460) || ((DeviceId & 0xfff8)  == 0x5B60)) {
    return TRUE;
  }
  return FALSE;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check if we can accsee to EP. Wait for up to 1 sec if EP requred extra time to initialize.
 *
 *
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
PcieEpReadyWorkaround (
  IN       AMD_NB_CONFIG      *pConfig
  )
{
  PORT  PortId;
  UINT8 TempBus;
  PCI_ADDR  Device;
  TempBus = (UINT8) (pConfig->NbPciAddress.Address.Bus + 5);
  Device.AddressValue = MAKE_SBDFO (0, TempBus, 0, 0, 0);
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    PCI_ADDR  Port;
    Port.AddressValue = MAKE_SBDFO (0, pConfig->NbPciAddress.Address.Bus , PortId, 0, 0);
    if (LibNbIsDevicePresent (Port, pConfig)) {
      UINT32  LinkState;
      LibNbPciIndexRead (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REGA5, AccessWidth32, &LinkState, pConfig);
      LinkState &= 0x3F;
      if (LinkState == 0x10) {
        BOOLEAN   IsDevicePresent;
        UINT32    PortBusConfiguration;
        UINT32    Count;
        Count = 1000;
        LibNbPciRead (Port.AddressValue | 0x18, AccessWidth32, &PortBusConfiguration, pConfig);
        LibNbPciRMW (Port.AddressValue | 0x18, AccessWidth32, 0x0, (TempBus << 8) | (TempBus << 16), pConfig);
        do {
          IsDevicePresent = LibNbIsDevicePresent (Device, pConfig);
          STALL (GET_BLOCK_CONFIG_PTR (pConfig), 1000, CIMX_S3_SAVE);
        } while (IsDevicePresent == FALSE && Count-- != 0 );
        LibNbPciWrite (Port.AddressValue | 0x18, AccessWidth32, &PortBusConfiguration, pConfig);
      }
    }
  }
}

UINT16  AspmBrDeviceTable[] = {
 0x1002, 0x9441, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10B5, 0xFFFF, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x0402, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x0193, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x0422, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x0292, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x00F9, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x0141, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x0092, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x01D0, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x01D1, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x01D2, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x01D3, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x01D5, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x01D7, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x01D8, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x01DC, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x01DE, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x01DF, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x016A, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x10DE, 0x0392, (UINT16)~(ASPM_L1 | ASPM_L0s),
 0x168C, 0xFFFF, (UINT16)~(ASPM_L0s)
};


/*----------------------------------------------------------------------------------------*/
/**
 * Misc PCIe ASPM workarounds
 *
 *  @param[in] AspmLinkInfoPtr  Pointer to link ASPM info.
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
PcieAspmWorkarounds (
  IN OUT   ASPM_LINK_INFO     *AspmLinkInfoPtr,
  IN       AMD_NB_CONFIG      *pConfig
  )
{
  UINT32  UpstreamDeviceId;
  UINT32  DownstreamDeviceId;
  UINTN   i;
  LibNbPciRead (AspmLinkInfoPtr->UpstreamPort.AddressValue, AccessWidth32, &UpstreamDeviceId, pConfig);
  LibNbPciRead (AspmLinkInfoPtr->UpstreamPort.AddressValue, AccessWidth32, &DownstreamDeviceId, pConfig);
  for (i = 0; i < (sizeof (AspmBrDeviceTable) / sizeof (UINT16)); i = i + 3) {
    UINT32 DeviceId;
    UINT32 VendorId;
    VendorId = AspmBrDeviceTable[i];
    DeviceId = AspmBrDeviceTable[i + 1];
    if (VendorId == (UINT16)UpstreamDeviceId || VendorId == (UINT16)DownstreamDeviceId ) {
      if (DeviceId == 0xFFFF || DeviceId == (UpstreamDeviceId >> 16) || DeviceId == (DownstreamDeviceId >> 16)) {
        AspmLinkInfoPtr->UpstreamLx &= AspmBrDeviceTable[i + 2];
        AspmLinkInfoPtr->DownstreamLx &= AspmBrDeviceTable[i + 2];
      }
    }
  }
  if ((UINT16)UpstreamDeviceId == 0x168c) {
    // Atheros (Ignore dev capability enable L1 if requested)
    AspmLinkInfoPtr->UpstreamLx = AspmLinkInfoPtr->RequestedLx & ASPM_L1;
    AspmLinkInfoPtr->DownstreamLx = AspmLinkInfoPtr->UpstreamLx;
    LibNbPciRMW (AspmLinkInfoPtr->UpstreamPort.AddressValue | 0x70C, AccessS3SaveWidth32, 0x0, 0x0F003F01, pConfig);
  }
}
