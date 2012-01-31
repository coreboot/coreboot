/**
 * @file
 *
 *  PCIe Port device number remapping.
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

SCAN_STATUS
PciePllOffCheckFunction (
  IN      PCI_SCAN_PROTOCOL   *This,
  IN      PCI_ADDR            Function
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Check if EP allowed exit latency allowed PLL in L1 to be disabled on non hotplug ports.
 *
 *
 * @param[in] CoreId    CoreId
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
BOOLEAN
PciePllOffComatibilityTest (
  IN      CORE              CoreId,
  IN      AMD_NB_CONFIG     *pConfig
  )
{
  PLLOFF_WORKSPACE  PllOffWorkspace;
  PORT              PortId;
  BOOLEAN           Result;
  PCIE_CONFIG       *pPcieConfig;
  BOOLEAN           IsHotplugPorst;
  BOOLEAN           IsNonHotplugPorts;
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PciePllOffInL1ComatibilityTest Enter Core [%d]\n", CoreId));
  LibAmdMemFill (&PllOffWorkspace, 0, sizeof (PllOffWorkspace), (AMD_CONFIG_PARAMS *)&(pConfig->sHeader));
  PllOffWorkspace.ScanPciePort.pConfig = pConfig;
  PllOffWorkspace.ScanPciePort.ScanBus = LibNbScanPciBus;
  PllOffWorkspace.ScanPciePort.ScanDevice = LibNbScanPciDevice;
  PllOffWorkspace.ScanPciePort.ScanFunction = PciePllOffCheckFunction;
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if (PcieLibIsValidPortId (PortId, pConfig) && PcieLibGetCoreId (PortId, pConfig) == CoreId) {
      if (pPcieConfig->PortConfiguration[PortId].PortHotplug == ON) {
        IsHotplugPorst = TRUE;
        continue;     // Skip hotplug ports . Will make decision later.
      }
      if (pPcieConfig->PortConfiguration[PortId].PortDetected == ON) {
        PCI_ADDR  Port;
        IsNonHotplugPorts = TRUE;
        Port = PcieLibGetPortPciAddress (PortId, pConfig);
        PllOffWorkspace.ScanPciePort.ScanFunction (&PllOffWorkspace.ScanPciePort, Port);
      }
    }
  }
  if (PllOffWorkspace.MaxL1Latency != 0 && PllOffWorkspace.MaxL1Latency < 34) {
    Result = FALSE;
  } else {
    Result = TRUE;
  }
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PciePllOffInL1ComatibilityTest Exit [%d]\n", Result));
  return  Result;
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
PciePllOffCheckFunction (
  IN      PCI_SCAN_PROTOCOL   *This,
  IN      PCI_ADDR            Function
  )
{
  PLLOFF_WORKSPACE  *WorkspacePtr;
  PCIE_DEVICE_TYPE  DeviceType;
  UINT8             SecondaryBus;
  PCI_ADDR  Port;

  WorkspacePtr = (PLLOFF_WORKSPACE*) This;
  DeviceType = PcieGetDeviceType (Function, This->pConfig);
  if (DeviceType == PcieDeviceRootComplex || DeviceType == PcieDeviceDownstreamPort) {
    WorkspacePtr->LinkCount++;
    //Lets enable Common clock
    LibNbPciRead (Function.AddressValue | 0x19, AccessWidth8, &SecondaryBus, This->pConfig);
    if (SecondaryBus == 0) {
      return  SCAN_FINISHED;
    }
    Port.AddressValue = MAKE_SBDFO (0, SecondaryBus, 0, 0, 0);
    This->ScanBus (This, Port);
    WorkspacePtr->LinkCount--;
  } else if (DeviceType == PcieDeviceUpstreamPort ) {
    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (This->pConfig), CIMX_NBPCIE_TRACE), "    Reached upstream port\n"));
    LibNbPciRead (Function.AddressValue | 0x19, AccessWidth8, &SecondaryBus, This->pConfig);
    if (SecondaryBus == 0) {
      return  SCAN_FINISHED;
    }
    Port.AddressValue = MAKE_SBDFO (0, SecondaryBus, 0, 0, 0);
    This->ScanBus (This, Port);
  } else if (DeviceType <= PcieDeviceLegacyEndPoint) {
    // We reach end of the link
    UINT8       PcieCapPtr;
    UINT32      Value;
    UINT8       L1AcceptableLatency;
    PcieCapPtr = LibNbFindPciCapability (Function.AddressValue, PCIE_CAP_ID, This->pConfig);
    if (PcieCapPtr != 0) {
      LibNbPciRead (Function.AddressValue | (PcieCapPtr + 0x0D) , AccessWidth8, &Value, This->pConfig);
      if (((Value >> 2) & ASPM_L1) != 0) {
        LibNbPciRead ((Function.AddressValue | (PcieCapPtr + 4)), AccessWidth32, &Value, This->pConfig);
        L1AcceptableLatency = ((UINT8) (1 << ((Value >> 9) & 0x7)) & 0x7F);
        if (WorkspacePtr->LinkCount > 1) {
          L1AcceptableLatency = L1AcceptableLatency + WorkspacePtr->LinkCount;
        }
        if (WorkspacePtr->MaxL1Latency <  L1AcceptableLatency) {
          WorkspacePtr->MaxL1Latency = L1AcceptableLatency;
        }
        CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (This->pConfig), CIMX_NBPCIE_TRACE), "    Reached end of link at 0x%x with Acceptable Exit Latency %dus \n", Function.AddressValue, L1AcceptableLatency));
      }
    }
  }
  return  SCAN_FINISHED;
}
