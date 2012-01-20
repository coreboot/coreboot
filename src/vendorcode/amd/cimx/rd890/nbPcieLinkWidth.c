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
 * Set Pcie Link Width
 *
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] LinkWidth New Link Width
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
AGESA_STATUS
PcieLibSetLinkWidth (
  IN      PORT             PortId,
  IN      PCIE_LINK_WIDTH  LinkWidth,
  IN      AMD_NB_CONFIG    *pConfig
  )
{
  AGESA_STATUS    Status;
  PCIE_LINK_WIDTH NewLinkWidth;
  PCIE_LINK_WIDTH CurrentLinkWidth;

  Status = AGESA_SUCCESS;
  NewLinkWidth = LinkWidth;
  CurrentLinkWidth = PcieLibGetLinkWidth (PortId, pConfig);
  if (NewLinkWidth == 0 || NewLinkWidth > CurrentLinkWidth) {
    NewLinkWidth = CurrentLinkWidth;
  }
  if (NewLinkWidth == PcieLinkWidth_x12) {
    NewLinkWidth = PcieLinkWidth_x8;
  }
  if (NewLinkWidth < CurrentLinkWidth) {
    CORE      CoreId;
    UINT32    Value;
    UINT32    CoreAddress;
    BOOLEAN   PoolPortStatus;
    PCI_ADDR  Port;

    CoreId = PcieLibGetCoreId (PortId, pConfig);
    CoreAddress = PcieLibGetCoreAddress (CoreId, pConfig);
    PoolPortStatus = TRUE;
    Port = PcieLibGetPortPciAddress (PortId, pConfig);
    LibNbPciIndexRMW (NB_SBDFO | NB_BIF_INDEX, NB_BIFNB_REG40 | CoreAddress, AccessWidth32, (UINT32)~BIT0, BIT0, pConfig);
    LibNbPciIndexRMW (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REGA2, AccessWidth32, 0xfffffff8, (NewLinkWidth) | BIT8 | BIT7, pConfig);
    while (PoolPortStatus) {
      LibNbPciRead (Port.AddressValue | NB_PCIP_REG6A, AccessWidth16, &Value, pConfig);
      if ((Value & BIT11) == 0) {
        LibNbPciIndexRead (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REGA2, AccessWidth32, &Value, pConfig);
        if ((Value & BIT8) == 0) {
          LibNbPciRead (Port.AddressValue | NB_PCIP_REG12A, AccessWidth16, &Value, pConfig);
          if ((Value & BIT1) == 0) {
            PoolPortStatus = FALSE;
          }
        }
      }
    }
    LibNbPciIndexRMW (NB_SBDFO | NB_BIF_INDEX, NB_BIFNB_REG40 | CoreAddress, AccessWidth32, (UINT32)~BIT0, 0 , pConfig);
    LibNbPciIndexRMW (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REGA2, AccessWidth32, (UINT32)~BIT7, 0, pConfig);
  }
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Return link with
 *
 *
 *
 *
 * @param[in] PortId    PCI Express Port ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
PCIE_LINK_WIDTH
PcieLibGetLinkWidth (
  IN      PORT               PortId,
  IN      AMD_NB_CONFIG      *pConfig
  )
{
  PCIE_LINK_WIDTH LinkWidth;
  UINT32          Value;
  PCI_ADDR        Port;

  Port = PcieLibGetPortPciAddress (PortId, pConfig);
//  Read current link State
  LibNbPciIndexRead (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REGA5, AccessWidth32, &Value, pConfig);
  if ((Value & 0x3f) == 0x10) {
  //Read current link width
    LibNbPciIndexRead (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REGA2, AccessWidth32, &Value, pConfig);
    LinkWidth = (Value >> 4) & 0xf;
  } else {
  //Link not in L0
    LinkWidth = PcieLinkWidth_x0;
  }
  return  LinkWidth;
}
