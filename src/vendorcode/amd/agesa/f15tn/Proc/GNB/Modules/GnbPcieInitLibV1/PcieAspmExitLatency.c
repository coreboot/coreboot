/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to calculate PCIe topology segment maximum exit latency
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
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieInitLibV1.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEINITLIBV1_PCIEASPMEXITLATENCY_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
  GNB_PCI_SCAN_DATA       ScanData;
  PCIe_ASPM_LATENCY_INFO  *AspmLatencyInfo;
  PCI_ADDR                DownstreamPort;
  UINT8                   LinkCount;
} PCIE_EXIT_LATENCY_DATA;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

SCAN_STATUS
PcieAspmGetMaxExitLatencyCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Determine ASPM L-state maximum exit latency for PCIe segment
 *
 *  Scan through all link in segment to determine maxim exit latency requirement by EPs.
 *
 * @param[in]   DownstreamPort    PCI address of PCIe port
 * @param[out]  AspmLatencyInfo   Latency info
 * @param[in]   StdHeader         Standard configuration header
 *
 */

VOID
PcieAspmGetMaxExitLatency (
  IN       PCI_ADDR                DownstreamPort,
     OUT   PCIe_ASPM_LATENCY_INFO  *AspmLatencyInfo,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  PCIE_EXIT_LATENCY_DATA  PcieExitLatencyData;
  PcieExitLatencyData.AspmLatencyInfo = AspmLatencyInfo;
  PcieExitLatencyData.ScanData.StdHeader = StdHeader;
  PcieExitLatencyData.LinkCount = 0;
  PcieExitLatencyData.ScanData.GnbScanCallback = PcieAspmGetMaxExitLatencyCallback;
  GnbLibPciScan (DownstreamPort, DownstreamPort, &PcieExitLatencyData.ScanData);
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
PcieAspmGetMaxExitLatencyCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS             ScanStatus;
  PCIE_EXIT_LATENCY_DATA  *PcieExitLatencyData;
  PCIE_DEVICE_TYPE        DeviceType;
  UINT32                  Value;
  UINT8                   PcieCapPtr;
  UINT8                   L1AcceptableLatency;

  PcieExitLatencyData = (PCIE_EXIT_LATENCY_DATA*) ScanData;
  ScanStatus = SCAN_SUCCESS;
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieAspmGetMaxExitLatencyCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    );
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
    PcieExitLatencyData->DownstreamPort = Device;
    PcieExitLatencyData->LinkCount++;
    GnbLibPciScanSecondaryBus (Device, &PcieExitLatencyData->ScanData);
    PcieExitLatencyData->LinkCount--;
    break;
  case  PcieDeviceUpstreamPort:
    GnbLibPciScanSecondaryBus (Device, &PcieExitLatencyData->ScanData);
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, ScanData->StdHeader);
    ASSERT (PcieCapPtr != 0);
    GnbLibPciRead (
      Device.AddressValue | (PcieCapPtr + PCIE_LINK_CAP_REGISTER),
      AccessWidth32,
      &Value,
      ScanData->StdHeader
      );
    if ((Value & PCIE_ASPM_L1_SUPPORT_CAP) != 0) {
      GnbLibPciRead (
        Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CAP_REGISTER),
        AccessWidth32,
        &Value,
        ScanData->StdHeader
        );
      L1AcceptableLatency = (UINT8) (1 << ((Value >> 9) & 0x7));
      if (PcieExitLatencyData->LinkCount > 1) {
        L1AcceptableLatency = L1AcceptableLatency + PcieExitLatencyData->LinkCount;
      }
      if (PcieExitLatencyData->AspmLatencyInfo->MaxL1ExitLatency <  L1AcceptableLatency) {
        PcieExitLatencyData->AspmLatencyInfo->MaxL1ExitLatency = L1AcceptableLatency;
      }
      IDS_HDT_CONSOLE (PCIE_MISC, "  Device max exit latency  L1 - %d us\n",
        L1AcceptableLatency
        );
    }
    break;
  default:
    break;
  }
  return SCAN_SUCCESS;
}

