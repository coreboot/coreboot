/**
 * @file
 *
 * PCIe link width control.
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


/*----------------------------------------------------------------------------------------*/
/**
 * Allocate CPL buffers
 *
 *
 *
 * @param[in] CoreId    PCI Express Core ID
 * @param[in] pConfig   Northbridge configuration structure pointer.  *
 */
VOID
PcieLibCplBufferAllocation (
  IN      CORE              CoreId,
  IN      AMD_NB_CONFIG     *pConfig
  )
{
  PORT     PortId;
  BOOLEAN  IsAllocationEnabled;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibCplBufferAllocation Enter [CoreId = %d]\n", CoreId));
  IsAllocationEnabled = FALSE;
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if (PcieLibIsValidPortId (PortId, pConfig) && PcieLibGetCoreId (PortId, pConfig) == CoreId) {
      PCI_ADDR  Port;
      PORT_INFO *pPortInfo;
      pPortInfo = PcieLibGetPortInfo (PortId, pConfig);
      if (pPortInfo->SlaveCplBuffers != 0) {
        Port = PcieLibGetPortPciAddress (PortId, pConfig);
        CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    PortId %d , Port Address 0x%x, CplBuffers, %d\n", PortId, Port.AddressValue, pPortInfo->SlaveCplBuffers));
        LibNbPciIndexRMW (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REG10, AccessWidth32, (UINT32)~(0x3f << 8), pPortInfo->SlaveCplBuffers << 8, pConfig);
        IsAllocationEnabled = TRUE;
      }
    }
  }
  if (IsAllocationEnabled) {
    CORE  CoreAddress;
    CoreAddress = PcieLibGetCoreAddress (CoreId, pConfig);
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_BIF_INDEX, NB_BIFNB_REG20 | CoreAddress, AccessWidth32, (UINT32)~(BIT11), BIT11, pConfig);
  }
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibCplBufferAllocation Exit\n"));
}