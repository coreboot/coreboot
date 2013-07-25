/* $NoKeywords:$ */
/**
 * @file
 *
 * NB services
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
#include  "S3SaveState.h"
#include  "Gnb.h"
#include  "GnbPcieConfig.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbNbInitLibV4.h"
#include  "GnbRegistersCommonV2.h"
#include  "heapManager.h"
#include  "GnbFamServices.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBNBINITLIBV4_GNBNBINITLIBV4_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define SMC_RAM_START_ADDR 0x10000ul

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
  GNB_PCI_SCAN_DATA     ScanData;
  GNB_TOPOLOGY_INFO     *TopologyInfo;
} GNB_TOPOLOGY_INFO_DATA;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
GnbSmuServiceRequestV4S3Script (
  IN      AMD_CONFIG_PARAMS     *StdHeader,
  IN      UINT16                ContextLength,
  IN      VOID                  *Context
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Check a PCIE device to see if it supports phantom functions
 *
 * @param[in] Device      Device pci address
 * @param[in] StdHeader   Standard configuration header
 * @return    TRUE        Current device supports phantom functions
 */
STATIC BOOLEAN
GnbCheckPhantomFuncSupport (
  IN       PCI_ADDR                 Device,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  UINT8   PcieCapPtr;
  UINT32  Value;
  Value = 0;

  PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, StdHeader);
  if (PcieCapPtr != 0) {
    GnbLibPciRead (Device.AddressValue | (PcieCapPtr + 4), AccessWidth32, &Value, StdHeader);
  }
  return ((Value & (BIT3 | BIT4)) != 0) ? TRUE : FALSE;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Evaluate device
 *
 *
 *
 * @param[in]     Device          PCI Address
 * @param[in,out] ScanData        Scan configuration data
 * @retval                        Scan Status
 */

SCAN_STATUS
STATIC
GnbTopologyInfoScanCallback (
  IN       PCI_ADDR                 Device,
  IN OUT   GNB_PCI_SCAN_DATA        *ScanData
  )
{
  SCAN_STATUS             ScanStatus;
  GNB_TOPOLOGY_INFO_DATA  *GnbTopologyInfo;
  PCIE_DEVICE_TYPE        DeviceType;
  ScanStatus = SCAN_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "  GnbIommuInfoScanCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    );
  GnbTopologyInfo = (GNB_TOPOLOGY_INFO_DATA *)ScanData;
  ScanStatus = SCAN_SUCCESS;
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
    GnbLibPciScanSecondaryBus (Device, &GnbTopologyInfo->ScanData);
    break;
  case  PcieDeviceUpstreamPort:
    GnbLibPciScanSecondaryBus (Device, &GnbTopologyInfo->ScanData);
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  case  PcieDevicePcieToPcix:
    GnbTopologyInfo->TopologyInfo->PcieToPciexBridge = TRUE;
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    if (GnbCheckPhantomFuncSupport (Device, ScanData->StdHeader)) {
      GnbTopologyInfo->TopologyInfo->PhantomFunction = TRUE;
    }
    ScanStatus = SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  default:
    break;
  }
  return ScanStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get IOMMU topology info
 *
 *
 *
 * @param[in] StartPciAddress   Start PCI address
 * @param[in] EndPciAddress     End PCI address
 * @param[in] TopologyInfo      Topology info structure
 * @param[in] StdHeader         Standard Configuration Header
 */

AGESA_STATUS
GnbGetTopologyInfoV4 (
  IN       PCI_ADDR                  StartPciAddress,
  IN       PCI_ADDR                  EndPciAddress,
     OUT   GNB_TOPOLOGY_INFO         *TopologyInfo,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  GNB_TOPOLOGY_INFO_DATA  GnbTopologyInfo;
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbGetTopologyInfoV4 Enter\n");
  GnbTopologyInfo.ScanData.GnbScanCallback = GnbTopologyInfoScanCallback;
  GnbTopologyInfo.ScanData.StdHeader = StdHeader;
  GnbTopologyInfo.TopologyInfo = TopologyInfo;
  GnbLibPciScan (StartPciAddress, EndPciAddress, &GnbTopologyInfo.ScanData);
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbGetTopologyInfoV4 Exit\n");
  return AGESA_SUCCESS;
}



/*----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------*/
/**
 * SMU firmware download
 *
 *
 * @param[in]  GnbPciAddress   GNB Pci Address
 * @param[in]  Firmware        Pointer tp firmware
 * @param[in]  StdHeader       Standard configuration header
 */


/*----------------------------------------------------------------------------------------*/
/**
 * Get IOMMU PCI address
 *
 *
 * @param[in]  GnbHandle       GNB handle
 * @param[in]  StdHeader       Standard configuration header
 */

PCI_ADDR
GnbGetIommuPciAddressV4 (
  IN       GNB_HANDLE               *GnbHandle,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  PCI_ADDR  GnbIommuPciAddress;
  GnbIommuPciAddress = GnbGetHostPciAddress (GnbHandle);
  GnbIommuPciAddress.Address.Function = 0x2;
  return  GnbIommuPciAddress;
}


/*----------------------------------------------------------------------------------------*/
/**
 * UnitID Clumping
 *
 *
 * @param[in]  GnbHandle       GNB handle
 * @param[in]  StdHeader       Standard configuration header
 */

VOID
GnbClumpUnitIdV4 (
  IN      GNB_HANDLE                *GnbHandle,
  IN      AMD_CONFIG_PARAMS         *StdHeader
  )
{

  PCIe_ENGINE_CONFIG  *EngineList;
  UINT32              Value;

  Value = 0;
  EngineList = (PCIe_ENGINE_CONFIG *) PcieConfigGetChild (DESCRIPTOR_PCIE_ENGINE, &GnbHandle->Header);
  while (EngineList != NULL) {
    if (EngineList->Type.Port.NumberOfUnitId != 0) {
      if (!PcieConfigIsActivePcieEngine (EngineList)) {
        Value |= (((1 << EngineList->Type.Port.NumberOfUnitId) - 1) << EngineList->Type.Port.UnitId);
      } else {
        if (EngineList->Type.Port.NumberOfUnitId > 1) {
          Value |= (((1 << (EngineList->Type.Port.NumberOfUnitId - 1)) - 1) << (EngineList->Type.Port.UnitId + 1));
        }
      }
    }
    EngineList = (PCIe_ENGINE_CONFIG *) PcieConfigGetNextTopologyDescriptor (EngineList, DESCRIPTOR_TERMINATE_GNB);
  }
  // Set GNB
  GnbLibPciIndirectRMW (
    GnbHandle->Address.AddressValue | D0F0x94_ADDRESS,
    D0F0x98_x3A_ADDRESS,
    AccessS3SaveWidth32,
    (UINT32) ~Value,
    Value,
    StdHeader
    );
  //Set UNB
  GnbLibPciRMW (
    MAKE_SBDFO (0, 0, GnbHandle->NodeId + 0x18, 0, D18F0x110_ADDRESS + GnbHandle->LinkId * 4),
    AccessS3SaveWidth32,
    (UINT32) ~Value,
    Value,
    StdHeader
    );
}



/*----------------------------------------------------------------------------------------*/
/**
 * Config GNB to prevent LPC deadlock scenario
 *
 *
 * @param[in]  GnbHandle       GNB handle
 * @param[in]  StdHeader       Standard configuration header
 */

VOID
GnbLpcDmaDeadlockPreventionV4 (
  IN       GNB_HANDLE               *GnbHandle,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  PCIe_PLATFORM_CONFIG  *Pcie;
  PCIe_ENGINE_CONFIG    *EngineList;

  Pcie = (PCIe_PLATFORM_CONFIG *) PcieConfigGetParent (DESCRIPTOR_PLATFORM, &GnbHandle->Header);
  EngineList = (PCIe_ENGINE_CONFIG *) PcieConfigGetChild (DESCRIPTOR_ALL_ENGINES, &GnbHandle->Header);
  while (EngineList != NULL) {
    if (PcieConfigIsPcieEngine (EngineList) && PcieConfigIsSbPcieEngine (EngineList)) {
      PcieRegisterRMW (
        PcieConfigGetParentWrapper (EngineList),
        CORE_SPACE (EngineList->Type.Port.CoreId, D0F0xE4_CORE_0010_ADDRESS),
        D0F0xE4_CORE_0010_UmiNpMemWrite_MASK,
        1 << D0F0xE4_CORE_0010_UmiNpMemWrite_OFFSET,
        TRUE,
        Pcie
        );
      //Enable special NP memory write protocol in ORB
      GnbLibPciIndirectRMW (
        GnbHandle->Address.AddressValue | D0F0x94_ADDRESS,
        D0F0x98_x06_ADDRESS,
        AccessS3SaveWidth32,
        0xFFFFFFFF,
        1 << D0F0x98_x06_UmiNpMemWrEn_OFFSET,
        StdHeader
        );
      break;
    }
    EngineList = (PCIe_ENGINE_CONFIG *) PcieConfigGetNextTopologyDescriptor (EngineList, DESCRIPTOR_TERMINATE_GNB);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Enable IOMMU base address. (MMIO space )
 *
 *
 * @param[in]     GnbHandle       GNB handle
 * @param[in]     StdHeader       Standard Configuration Header
 * @retval        AGESA_SUCCESS
 * @retval        AGESA_ERROR
 */

AGESA_STATUS
GnbEnableIommuMmioV4 (
  IN       GNB_HANDLE           *GnbHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  AGESA_STATUS            Status;
  UINT16                  CapabilityOffset;
  UINT64                  BaseAddress;
  UINT32                  Value;
  PCI_ADDR                GnbIommuPciAddress;

  Status = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEnableIommuMmio Enter\n");

  if (GnbFmCheckIommuPresent (GnbHandle, StdHeader)) {
    GnbIommuPciAddress = GnbGetIommuPciAddressV4 (GnbHandle, StdHeader);
    CapabilityOffset = GnbLibFindPciCapability (GnbIommuPciAddress.AddressValue, IOMMU_CAP_ID, StdHeader);

    GnbLibPciRead (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x4), AccessWidth32, &Value, StdHeader);
    BaseAddress = (UINT64) Value << 32;
    GnbLibPciRead (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x8), AccessWidth32, &Value, StdHeader);
    BaseAddress |= Value;

    if ((BaseAddress & 0xfffffffffffffffe) != 0x0) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  Enable IOMMU MMIO at address %x for Socket %d Silicon %d\n", BaseAddress, GnbGetSocketId (GnbHandle) , GnbGetSiliconId (GnbHandle));
      GnbLibPciRMW (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x8), AccessS3SaveWidth32, 0xFFFFFFFF, 0x0, StdHeader);
      GnbLibPciRMW (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x4), AccessS3SaveWidth32, 0xFFFFFFFE, 0x1, StdHeader);
    } else {
      ASSERT (FALSE);
      Status = AGESA_ERROR;
    }
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEnableIommuMmio Exit\n");
  return Status;
}

