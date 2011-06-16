
/**
 * @file
 *
 * Config Southbridge GPP controller
 *
 * Init GPP features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-SB
 * @e sub-project
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*
*****************************************************************************
*
* Copyright (c) 2011, Advanced Micro Devices, Inc.
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
****************************************************************************
*/
#include "SbPlatform.h"
#include "cbtypes.h"
#include "AmdSbLib.h"

/**
 * PCIE_CAP_ID - PCIe Cap ID
 *
 */
#define  PCIE_CAP_ID             0x10

//
// Declaration of local functions
//
UINT8
sbFindPciCap (
  IN       UINT32  pciAddress,
  IN       UINT8  targetCapId
  );

VOID
sbGppSetAspm (
  IN       UINT32    pciAddress,
  IN       UINT8     LxState
  );

VOID
sbGppSetEPAspm (
  IN       UINT32    pciAddress,
  IN       UINT8     LxState
  );

VOID
sbGppValidateAspm (
  IN       UINT32    pciAddress,
  IN       UINT8     *LxState
  );

VOID
sbGppForceGen2 (
  IN       AMDSBCFG        *pConfig,
  IN       CONST UINT8     ActivePorts
  );

VOID
sbGppForceGen1 (
  IN       AMDSBCFG      *pConfig,
  IN       CONST UINT8   ActivePorts
  );

VOID
PreInitGppLink (
  IN       AMDSBCFG* pConfig
  );

UINT8
GppPortPollingLtssm (
  IN       AMDSBCFG*      pConfig,
  IN       UINT8          ActivePorts,
  IN       BOOLEAN        IsGen2
  );

UINT8
CheckGppLinkStatus (
  IN       AMDSBCFG* pConfig
  );

VOID
AfterGppLinkInit (
  IN       AMDSBCFG* pConfig
  );

VOID
sbGppDynamicPowerSaving (
  IN       AMDSBCFG* pConfig
  );

VOID
sbGppAerInitialization (
  IN       AMDSBCFG* pConfig
  );

VOID
sbGppRasInitialization (
  IN     AMDSBCFG*   pConfig
  );

//
// Declaration of external functions
//


/**
 * sbFindPciCap - Find PCI Cap
 *
 *
 * @param[in] pciAddress     PCI Address.
 * @param[in] targetCapId    Target Cap ID.
 *
 */
UINT8
sbFindPciCap (
  IN       UINT32  pciAddress,
  IN       UINT8  targetCapId
  )
{
  UINT8       NextCapPtr;
  UINT8       CapId;

  NextCapPtr = 0x34;
  while (NextCapPtr != 0) {
    ReadPCI (pciAddress + NextCapPtr, AccWidthUint8, &NextCapPtr);
    if (NextCapPtr == 0xff) {
      return 0;
    }
    if (NextCapPtr != 0) {
      ReadPCI (pciAddress + NextCapPtr, AccWidthUint8, &CapId);
      if (CapId == targetCapId) {
        break;
      } else {
        NextCapPtr++;
      }
    }
  }
  return NextCapPtr;
}

/**
 * sbGppSetAspm - Set GPP ASPM
 *
 *
 * @param[in] pciAddress PCI Address.
 * @param[in] LxState    Lane State.
 *
 */
VOID
sbGppSetAspm (
  IN       UINT32    pciAddress,
  IN       UINT8     LxState
  )
{
  UINT8      pcieCapOffset;
  UINT8      value8;

  pcieCapOffset = sbFindPciCap (pciAddress, PCIE_CAP_ID);
  if (pcieCapOffset) {
    // Read link capabilities register (0x0C[11:10] - ASPM support)
    ReadPCI (pciAddress + pcieCapOffset + 0x0D, AccWidthUint8, &value8);
    if (value8 & BIT2) {
      value8 = (value8 >> 2) & (BIT1 + BIT0);
      // Set ASPM state in link control register
      RWPCI (pciAddress + pcieCapOffset + 0x10, AccWidthUint8, 0xffffffff, LxState & value8);
    }
  }
}

/**
 * sbGppSetEPAspm - Set EP GPP ASPM
 *
 *
 * @param[in] pciAddress PCI Address.
 * @param[in] LxState    Lane State.
 *
 */
VOID
sbGppSetEPAspm (
  IN       UINT32    pciAddress,
  IN       UINT8     LxState
  )
{
  UINT8      value8;
  UINT8      maxFuncs;
  UINT32     devBDF;

  maxFuncs = 1;
  ReadPCI (pciAddress + 0x0E, AccWidthUint8, &value8);

  if (value8 & BIT7) {
    maxFuncs = 8;              // multi-function device
  }
  while (maxFuncs != 0) {
    devBDF = pciAddress + (UINT32) ((maxFuncs - 1) << 16);
    sbGppSetAspm (devBDF, LxState);
    maxFuncs--;
  }
}

/**
 * sbGppValidateAspm - Validate endpoint support for GPP ASPM
 *
 *
 * @param[in] pciAddress PCI Address.
 * @param[in] LxState    Lane State.
 *
 */
VOID
sbGppValidateAspm (
  IN       UINT32    pciAddress,
  IN       UINT8     *LxState
  )
{
  UINT8      pcieCapOffset;
  UINT8      value8;
  UINT8      maxFuncs;
  UINT32     devBDF;

  maxFuncs = 1;
  ReadPCI (pciAddress + 0x0E, AccWidthUint8, &value8);

  if (value8 & BIT7) {
    maxFuncs = 8;              // multi-function device
  }
  while (maxFuncs != 0) {
    devBDF = pciAddress + (UINT32) ((maxFuncs - 1) << 16);
    pcieCapOffset = sbFindPciCap (devBDF, PCIE_CAP_ID);
    if (pcieCapOffset) {
      // Read link capabilities register (0x0C[11:10] - ASPM support)
      ReadPCI (devBDF + pcieCapOffset + 0x0D, AccWidthUint8, &value8);
      if (value8 & BIT2) {
        value8 = (value8 >> 2) & (BIT1 + BIT0);
        // Set ASPM state as what's endpoint support
        *LxState &= value8;
      }
    }
    maxFuncs--;
  }
}


/**
 * sbGppForceGen2 - Set GPP to Gen2
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 * @param[in] ActivePorts Activate Ports.
 *
 */
VOID
sbGppForceGen2 (
  IN       AMDSBCFG        *pConfig,
  IN       CONST UINT8     ActivePorts
  )
{
  UINT32       portId;

  for ( portId = 0; portId < MAX_GPP_PORTS; portId++ ) {
    if (ActivePorts & (1 << portId)) {
      rwAlink (SB_RCINDXP_REGA4 | portId << 24, 0xFFFFFFFF, BIT29 + BIT0);
      rwAlink ((SB_ABCFG_REG340 + portId * 4) | (UINT32) (ABCFG << 29), 0xFFFFFFFF, BIT21);
      rwAlink (SB_RCINDXP_REGA2 | portId << 24, ~BIT13, 0);
      rwAlink (SB_RCINDXP_REGC0 | portId << 24, ~BIT15, 0);
      RWPCI (PCI_ADDRESS (0, GPP_DEV_NUM, portId, 0x88), AccWidthUint8, 0xf0, 0x02);

      (&pConfig->PORTCONFIG[portId].PortCfg)->PortIsGen2 = 2;
    }
  }
}

/**
 * sbGppForceGen1 - Set GPP to Gen1
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 * @param[in] ActivePorts Activate Ports.
 *
 */
VOID
sbGppForceGen1 (
  IN       AMDSBCFG      *pConfig,
  IN       CONST UINT8   ActivePorts
  )
{
  UINT32       portId;

  for ( portId = 0; portId < MAX_GPP_PORTS; portId++ ) {
    if (ActivePorts & (1 << portId) && pConfig->GppHardwareDowngrade != portId + 1) {
      rwAlink ((SB_ABCFG_REG340 + portId * 4) | (UINT32) (ABCFG << 29), ~BIT21, 0);
      rwAlink (SB_RCINDXP_REGA4 | portId << 24, ~BIT0, BIT29);
      rwAlink (SB_RCINDXP_REGA2 | portId << 24, 0xFFFFFFFF, BIT13);
      rwAlink (SB_RCINDXP_REGC0 | portId << 24, ~BIT15, 0);
      RWPCI (PCI_ADDRESS (0, GPP_DEV_NUM, portId, 0x88), AccWidthUint8, 0xf0, 0x01);

      (&pConfig->PORTCONFIG[portId].PortCfg)->PortIsGen2 = 1;
    }
  }
}


/**
 * PreInitGppLink - Enable GPP link training.
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
PreInitGppLink (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8 portMask[5] = {
    0x01,
    0x00,
    0x03,
    0x07,
    0x0F
  };
  UINT8  cfgMode;
  UINT8  portId;
  UINT32 reg32Value;
  UINT16 tmp16Value;

//  PCIE_GPP_ENABLE (abcfg:0xC0):
//
//     GPP_LINK_CONFIG ([3:0])  PortA   PortB   PortC   PortD    Description
//    ----------------------------------------------------------------------------------
//                       0000    0-3                             x4 Config
//                       0001                                    N/A
//                       0010    0-1     2-3      0              2:2 Config
//                       0011    0-1      2       3              2:1:1 Config
//                       0100     0       1       2       3      1:1:1:1 Config
//
//  For A12 and above:
//                  ABCFG:0xC0[12] - Port A hold training (default 1)
//                  ABCFG:0xC0[13] - Port B hold training (default 1)
//                  ABCFG:0xC0[14] - Port C hold training (default 1)
//                  ABCFG:0xC0[15] - Port D hold training (default 1)
//
//
  //
  // Set port enable bit fields based on current GPP link configuration mode
  //
  cfgMode = (UINT8) pConfig->GppLinkConfig;
  if ( cfgMode > GPP_CFGMODE_X1111 || cfgMode == 1 ) {
    cfgMode = GPP_CFGMODE_X4000;
    pConfig->GppLinkConfig = GPP_CFGMODE_X4000;
  }
  reg32Value = (UINT32) portMask[cfgMode];

  // Mask out non-applicable ports according to the target link configuration mode
  for ( portId = 0; portId < MAX_GPP_PORTS; portId++ ) {
    pConfig->PORTCONFIG[portId].PortCfg.PortPresent &= (reg32Value >> portId) & BIT0;
  }

  //
  // Deassert GPP reset and pull EP out of reset - Clear GPP_RESET (abcfg:0xC0[8] = 0)
  //
  tmp16Value = (UINT16) (~reg32Value << 12);
  reg32Value = (UINT32) (tmp16Value + (reg32Value << 4) + cfgMode);
  writeAlink (SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29), reg32Value);

  reg32Value = readAlink (0xC0 | (UINT32) (RCINDXC << 29));
  writeAlink (0xC0 | (UINT32) (RCINDXC << 29), reg32Value | 0x400);  // Set STRAP_F0_MSI_EN

  // A-Link L1 Entry Delay Shortening
  // AXINDP_Reg 0xA0[7:4] = 0x3
  rwAlink (SB_AX_INDXP_REGA0, 0xFFFFFF0F, 0x30);
  rwAlink (SB_AX_INDXP_REGB1, 0xFFFFFFFF, BIT19);
  rwAlink (SB_AX_INDXP_REGB1, 0xFFFFFFFF, BIT28);

  // RPR5.22 GPP L1 Entry Delay Shortening
  // RCINDP_Reg 0xA0[7:4] = 0x1 Enter L1 sooner after ACK'ing PM request.
  // This is done to reduce number of NAK received with L1 enabled.
  // ENH254401: Program L0S/L1 activity timer to enable L0S/L1 on GPP
  // RCINDP_Reg 0xA0[11:8] = 0x9
  // RCINDP_Reg 0xA0[15:12] = 0x6
  for ( portId = 0; portId < MAX_GPP_PORTS; portId++ ) {
    rwAlink (SB_RCINDXP_REGA0 | portId << 24, 0xFFFF000F, 0x6910);
    //OBS220313: Hard System Hang running MeatGrinder Test on multiple blocks
    //RPR 5.13 GPP Error Reporting Configuration
    rwAlink (SB_RCINDXP_REG6A | portId << 24, ~(BIT1), 0);
  }

  if (pConfig->S3Resume) {
    SBGPPPORTCONFIG  *portCfg;

    for ( portId = 0; portId < MAX_GPP_PORTS; portId++ ) {
      portCfg = &pConfig->PORTCONFIG[portId].PortCfg;
      if (portCfg->PortHotPlug == TRUE) {
        portCfg->PortDetected = FALSE;
      } else {
        if (portCfg->PortIsGen2 == 1) {
          sbGppForceGen1 (pConfig, (UINT8) (1 << portId));
        } else {
          sbGppForceGen2 (pConfig, (UINT8) (1 << portId));
        }
      }
    }
  }

  // Obtain original Gen2 strap value (LC_GEN2_EN_STRAP)
  pConfig->GppGen2Strap = (UINT8) (readAlink (SB_RCINDXP_REGA4 | 0 << 24) & BIT0);
}


/**
 * GppPortPollingLtssm - Loop polling the LTSSM for each GPP port marked in PortMap
 *
 *
 * Return:     FailedPortMap   = A bitmap of ports which failed to train
 *
 * @param[in]    pConfig      Southbridge configuration structure pointer.
 * @param[in]    ActivePorts  A bitmap of ports which should be polled
 * @param[in]    IsGen2       TRUE if the polling is in Gen2 mode
 *
 */
UINT8
GppPortPollingLtssm (
  IN       AMDSBCFG*      pConfig,
  IN       UINT8          ActivePorts,
  IN       BOOLEAN        IsGen2
  )
{
  UINT32            retryCounter;
  UINT8             PortId;
  UINT8             FailedPorts;
  SBGPPPORTCONFIG  *portCfg;
  UINT32            abIndex;
  UINT32            Data32;
  UINT8             EmptyPorts;

  FailedPorts = 0;
  retryCounter = MAX_LT_POLLINGS;
  EmptyPorts = ActivePorts;

  while (retryCounter-- && ActivePorts) {
    for (PortId = 0; PortId < MAX_GPP_PORTS; PortId++) {
      if (ActivePorts & (1 << PortId)) {
        portCfg = &pConfig->PORTCONFIG[PortId].PortCfg;
        abIndex = SB_RCINDXP_REGA5 | (UINT32) (RCINDXP << 29) | (PortId << 24);
        Data32 = readAlink (abIndex) & 0x3F3F3F3F;

        if ((UINT8) (Data32) > 0x04) {
          EmptyPorts &= ~(1 << PortId);
        }

        if ((UINT8) (Data32) == 0x10) {
          ActivePorts &= ~(1 << PortId);
          portCfg->PortDetected = TRUE;
          break;
        }
        if (IsGen2) {
          UINT8      i;

          for (i = 0; i < 4; i++) {
            if ((UINT8) (Data32) == 0x29 || (UINT8) (Data32) == 0x2A ) {
              ActivePorts &= ~(1 << PortId);
              FailedPorts |= (1 << PortId);
              break;
            }
            Data32 >>= 8;
          }
        }
      }
    }
    if (EmptyPorts && retryCounter < (MAX_LT_POLLINGS - 200)) {
      ActivePorts &= ~EmptyPorts;
    }
    SbStall (1000);
  }
  FailedPorts |= ActivePorts;
  return FailedPorts;
}


/**
 * CheckGppLinkStatus - loop polling the link status for each GPP port
 *
 *
 * Return:     ToggleStatus[3:0] = Port bitmap for those need to clear De-emphasis
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
UINT8
CheckGppLinkStatus (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32            portId;
  UINT8             portScanMap;
  UINT8             GppHwDowngrade;
  SBGPPPORTCONFIG  *portCfg;
  UINT8             FailedPorts;


  portScanMap = 0;
  FailedPorts = 0;

  // Obtain a list of ports to be checked
  for ( portId = 0; portId < MAX_GPP_PORTS; portId++ ) {
    portCfg = &pConfig->PORTCONFIG[portId].PortCfg;
    if ( portCfg->PortPresent == TRUE && portCfg->PortDetected == FALSE ) {
      portScanMap |= 1 << portId;
    }
  }
  GppHwDowngrade = (UINT8)pConfig->GppHardwareDowngrade;
  if (GppHwDowngrade != 0) {
    // Skip polling and always assume this port to be present
    portScanMap &= ~(1 << (GppHwDowngrade - 1));
  }

  //GPP Gen2 Speed Change
  // if ((GPP Gen2 == enabled) and (RCINDP_Reg 0xA4[0] == 0x1)) {
  //   PCIe_Cfg 0x88[3:0]  = 0x2
  //   RCINDP_Reg 0xA2[13] = 0x0
  //   RCINDP_Reg 0xC0[15] = 0x0
  //   RCINDP_Reg 0xA4[29] = 0x1
  // } else {
  //   PCIe_Cfg 0x88[3:0]  = 0x1
  //   RCINDP_Reg 0xA4[0]  = 0x0
  //   RCINDP_Reg 0xA2[13] = 0x1
  //   RCINDP_Reg 0xC0[15] = 0x0
  //   RCINDP_Reg 0xA4[29] = 0x1
  // }
  SbStall (5000);
  if (pConfig->GppGen2 && pConfig->GppGen2Strap) {
    sbGppForceGen2 (pConfig, portScanMap);
    FailedPorts = GppPortPollingLtssm (pConfig, portScanMap, TRUE);

    if (FailedPorts) {
      sbGppForceGen1 (pConfig, FailedPorts);
      FailedPorts = GppPortPollingLtssm (pConfig, FailedPorts, FALSE);
    }
  } else {
    sbGppForceGen1 (pConfig, portScanMap);
    FailedPorts = GppPortPollingLtssm (pConfig, portScanMap, FALSE);
  }
  return FailedPorts;
}


/**
 * AfterGppLinkInit
 *       - Search for display device behind each GPP port
 *       - If the port is empty AND not hotplug-capable:
 *           * Turn off link training
 *           * (optional) Power down the port
 *           * Hide the configuration space (Turn off the port)
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
AfterGppLinkInit (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32           portId;
  SBGPPPORTCONFIG  *portCfg;
  UINT32           regBusNumber;
  UINT32           abValue;
  UINT32           abIndex;
  UINT8            bValue;
  UINT8            cimGppGen2;

  cimGppGen2 = pConfig->GppGen2;
#if  SB_CIMx_PARAMETER == 0
  cimGppGen2 = cimGppGen2Default;
#endif

  pConfig->GppFoundGfxDev = 0;
  abValue = readAlink (SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29));
  //RPR 5.9 Link Bandwidth Notification Capability Enable
  //RCINDC:0xC1[0] = 1
  rwAlink (SB_RCINDXC_REGC1, 0xFFFFFFFF, BIT0);

  for ( portId = 0; portId < MAX_GPP_PORTS; portId++ ) {
    // Program requester ID for every port
    abIndex = SB_RCINDXP_REG21 | (UINT32) (RCINDXP << 29) | (portId << 24);
    writeAlink (abIndex, (SB_GPP_DEV << 3) + portId);

    //RPR 5.9 Link Bandwidth Notification Capability Enable
    //PCIe Cfg 0x68[10] = 0
    //PCIe Cfg 0x68[11] = 0
    RWPCI (PCI_ADDRESS (0, GPP_DEV_NUM, portId, 0x68), AccWidthUint16, ~(BIT10 + BIT11), 0);

    portCfg = &pConfig->PORTCONFIG[portId].PortCfg;
    // Check if there is GFX device behind each GPP port
    if ( portCfg->PortDetected == TRUE ) {
      regBusNumber = (SBTEMP_BUS << 16) + (SBTEMP_BUS << 8);
      WritePCI (PCI_ADDRESS (0, GPP_DEV_NUM, portId, 0x18), AccWidthUint32, &regBusNumber);
      // *** Stall ();
      ReadPCI (PCI_ADDRESS (SBTEMP_BUS, 0, 0, 0x0B), AccWidthUint8, &bValue);
      if ( bValue == 3 ) {
        pConfig->GppFoundGfxDev |= (1 << portId);
      }
      regBusNumber = 0;
      WritePCI (PCI_ADDRESS (0, GPP_DEV_NUM, portId, 0x18), AccWidthUint32, &regBusNumber);
    } else if ( portCfg->PortPresent == FALSE || portCfg->PortHotPlug == FALSE ) {
      // Mask off non-applicable ports
      abValue &= ~(1 << (portId + 4));
    }

    if ( portCfg->PortHotPlug == TRUE ) {
      // RPR5.12 Hot Plug: PCIe Native Support
      // RCINDP_Reg 0x10[3] = 0x1
      // PCIe_Cfg 0x5A[8] = 0x1
      // PCIe_Cfg 0x6C[6] = 0x1
      // RCINDP_Reg 0x20[19] = 0x0
      rwAlink ((SB_RCINDXP_REG10 | (UINT32) (RCINDXP << 29) | (portId << 24)), 0xFFFFFFFF, BIT3);
      RWPCI (PCI_ADDRESS (0, GPP_DEV_NUM, portId, 0x5b), AccWidthUint8, 0xff, BIT0);
      RWPCI (PCI_ADDRESS (0, GPP_DEV_NUM, portId, 0x6c), AccWidthUint8, 0xff, BIT6);
      rwAlink ((SB_RCINDXP_REG20 | (UINT32) (RCINDXP << 29) | (portId << 24)), ~BIT19, 0);
    }
  }
  if ( pConfig->GppUnhidePorts == FALSE ) {
    if ((abValue & 0xF0) == 0) {
      abValue = BIT8;         // if all ports are empty set GPP_RESET
    } else if ((abValue & 0xE0) != 0 && (abValue & 0x10) == 0) {
      abValue |= BIT4;        // PortA should always be visible whenever other ports are exist
    }

    // Update GPP_Portx_Enable (abcfg:0xC0[7:5])
    writeAlink (SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29), abValue);
  }

  //
  // Common initialization for open GPP ports
  //
  for ( portId = 0; portId < MAX_GPP_PORTS; portId++ ) {
    ReadPCI (PCI_ADDRESS (0, GPP_DEV_NUM, portId, 0x80), AccWidthUint8, &bValue);
    if (bValue != 0xff) {
      // Set pciCfg:PCIE_DEVICE_CNTL2[3:0] = 4'h6 (0x80[3:0])
      bValue &= 0xf0;
      bValue |= 0x06;
      WritePCI (PCI_ADDRESS (0, GPP_DEV_NUM, portId, 0x80), AccWidthUint8, &bValue);

        // Set PCIEIND_P:PCIE_RX_CNTL[RX_RCB_CPL_TIMEOUT_MODE] (0x70:[19]) = 1
      abIndex = SB_RCINDXP_REG70 | (UINT32) (RCINDXP << 29) | (portId << 24);
      abValue = readAlink (abIndex) | BIT19;
      writeAlink (abIndex, abValue);

        // Set PCIEIND_P:PCIE_TX_CNTL[TX_FLUSH_TLP_DIS] (0x20:[19]) = 0
      abIndex = SB_RCINDXP_REG20 | (UINT32) (RCINDXP << 29) | (portId << 24);
      abValue = readAlink (abIndex) & ~BIT19;
      writeAlink (abIndex, abValue);

        // Set Immediate Ack PM_Active_State_Request_L1 (0xA0:[23]) = 0
      abIndex = SB_RCINDXP_REGA0 | (UINT32) (RCINDXP << 29) | (portId << 24);
      abValue = readAlink (abIndex) & ~BIT23;
      if ( pConfig->L1ImmediateAck == 0) {
        abValue |= BIT23;
      }
      writeAlink (abIndex, abValue);
    }
  }
}


/**
 * sbPcieGppLateInit - Late PCIE initialization for Hudson-2 GPP component
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
sbPcieGppLateInit (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8            portId;
  UINT8            busNum;
  UINT8            aspmValue;
  UINT8            PortaspmValue;
  UINT8            reg8Value;
  UINT8            cimGppPhyPllPowerDown;
  SBGPPPORTCONFIG  *portCfg;
  UINT32           reg32Value;

  // Disable hidden register decode and serial number capability
  reg32Value = readAlink (SB_ABCFG_REG330 | (UINT32) (ABCFG << 29));
  writeAlink (SB_ABCFG_REG330 | (UINT32) (ABCFG << 29), reg32Value & ~(BIT26 + BIT10));

  if (readAlink (SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29)) & BIT8) {
    return;
  }

  //
  // Configure ASPM
  //
  //  writeAlink (0xC0 | (UINT32) (RCINDXC << 29), 0x400);  // Set STRAP_F0_MSI_EN
  aspmValue = (UINT8)pConfig->GppPortAspm;
  cimGppPhyPllPowerDown = (UINT8) pConfig->GppPhyPllPowerDown;
#if  SB_CIMx_PARAMETER == 0
  aspmValue = cimGppPortAspmDefault;
  cimGppPhyPllPowerDown = cimGppPhyPllPowerDownDefault;
#endif

  reg8Value = 0x01;
  for ( portId = 0; portId < MAX_GPP_PORTS; portId++ ) {
    // write pci_reg3d with 0x01 to fix yellow mark for GPP bridge under Vista
    // when native PCIE is enabled but MSI is not available
    // SB02029: Hudson-2 BIF/GPP allowing strap STRAP_BIF_INTERRUPT_PIN_SB controlled by AB reg
    portCfg = &pConfig->PORTCONFIG[portId].PortCfg;
    if (portCfg->PortHotPlug) {
      RWPCI (PCI_ADDRESS (0, 21, portId, 0x04), AccWidthUint8, 0xFE, 0x00);  //clear IO enable to fix possible hotplug hang
    }
    WritePCI (PCI_ADDRESS (0, 21, portId, 0x3d), AccWidthUint8, &reg8Value);
    ReadPCI (PCI_ADDRESS (0, 21, portId, 0x19), AccWidthUint8, &busNum);
    if (busNum != 0xFF) {
      ReadPCI (PCI_ADDRESS (busNum, 0, 0, 0x00), AccWidthUint32, &reg32Value);
      if (reg32Value != 0xffffffff) {
        PortaspmValue = aspmValue;
        // Vlidate if EP support ASPM
        sbGppValidateAspm (PCI_ADDRESS (busNum, 0, 0, 0), &PortaspmValue);
        // Set ASPM on EP side
        sbGppSetEPAspm (PCI_ADDRESS (busNum, 0, 0, 0), PortaspmValue);
        // Set ASPM on port side
        sbGppSetAspm (PCI_ADDRESS (0, 21, portId, 0), PortaspmValue);
      }
    }
    rwAlink ((SB_RCINDXP_REG02 | (UINT32) (RCINDXP << 29) | (portId << 24) ), ~(BIT15), (BIT15));
  }
  rwAlink ((SB_RCINDXC_REG02 | (UINT32) (RCINDXC << 29)), ~(BIT0), (BIT0));

  //
  // Configure Lock HWInit registers
  //
  reg32Value = readAlink (SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29));
  if (reg32Value & 0xF0) {
    reg32Value = readAlink (SB_RCINDXC_REG10 | (UINT32) (RCINDXC << 29));
    writeAlink (SB_RCINDXC_REG10 | (UINT32) (RCINDXC << 29), reg32Value | BIT0);  // Set HWINIT_WR_LOCK

    if ( cimGppPhyPllPowerDown == TRUE ) {
      //
      // RPR 5.4 Power Saving Feature for GPP Lanes
      //
      UINT32  abValue;

      // Set PCIE_P_CNTL in Alink PCIEIND space
      abValue = readAlink (RC_INDXC_REG40 | (UINT32) (RCINDXC << 29));
      abValue |= BIT12 + BIT3 + BIT0;
      abValue &= ~(BIT9 + BIT4);
      writeAlink (RC_INDXC_REG40 | (UINT32) (RCINDXC << 29), abValue);
      rwAlink (SB_RCINDXC_REG02, ~(BIT8), (BIT8));
      rwAlink (SB_RCINDXC_REG02, ~(BIT3), (BIT3));
    }
  }

  // Restore strap0 via override
  if (pConfig->PcieAER) {
    rwAlink (SB_ABCFG_REG310 | (UINT32) (ABCFG << 29), 0xFFFFFFFF, BIT7);
    rwAlink (RC_INDXC_REGC0, 0xFFFFFFFF, BIT9);
  }
}


/**
 * sbGppDynamicPowerSaving - RPR 5.19 GPP Dynamic Power Saving
 *
 *
 * @param[in] pConfig
 *
 */
VOID
sbGppDynamicPowerSaving (
  IN       AMDSBCFG* pConfig
  )
{
  SBGPPPORTCONFIG  *portCfg;
  UINT8            cimGppLaneReversal;
  UINT8            cimAlinkPhyPllPowerDown;
  UINT8            cimGppPhyPllPowerDown;
  UINT32           Data32;
  UINT32           HoldData32;
  UINT32           abValue;

  if (!pConfig->GppDynamicPowerSaving || pConfig->sdbEnable) {
    return;
  }

  cimAlinkPhyPllPowerDown = (UINT8) pConfig->AlinkPhyPllPowerDown;
  cimGppLaneReversal =  (UINT8) pConfig->GppLaneReversal;
  cimGppPhyPllPowerDown =  (UINT8) pConfig->GppPhyPllPowerDown;
#if  SB_CIMx_PARAMETER == 0
  cimGppLaneReversal = cimGppLaneReversalDefault;
  cimAlinkPhyPllPowerDown = cimAlinkPhyPllPowerDownDefault;
  cimGppPhyPllPowerDown = cimGppPhyPllPowerDownDefault;
#endif
  if (pConfig->GppHardwareDowngrade) {
    portCfg = &pConfig->PORTCONFIG[pConfig->GppHardwareDowngrade - 1].PortCfg;
    portCfg->PortDetected = TRUE;
  }

  Data32 = 0;
  HoldData32 = 0;
  switch ( pConfig->GppLinkConfig ) {
  case GPP_CFGMODE_X4000:
    portCfg = &pConfig->PORTCONFIG[0].PortCfg;
    if ( portCfg->PortDetected == FALSE ) {
      Data32 |= 0x0f0f;
      HoldData32 |= 0x1000;
    }
    break;
  case GPP_CFGMODE_X2200:
    portCfg = &pConfig->PORTCONFIG[0].PortCfg;
    if ( portCfg->PortDetected == FALSE ) {
      Data32 |= ( cimGppLaneReversal )? 0x0c0c:0x0303;
      HoldData32 |= 0x1000;
    }
    portCfg = &pConfig->PORTCONFIG[1].PortCfg;
    if ( portCfg->PortDetected == FALSE ) {
      Data32 |= ( cimGppLaneReversal )? 0x0303:0x0c0c;
      HoldData32 |= 0x2000;
    }
    break;
  case GPP_CFGMODE_X2110:
    portCfg = &pConfig->PORTCONFIG[0].PortCfg;
    if ( portCfg->PortDetected == FALSE ) {
      Data32 |= ( cimGppLaneReversal )? 0x0c0c:0x0303;
      HoldData32 |= 0x1000;
    }
    portCfg = &pConfig->PORTCONFIG[1].PortCfg;
    if ( portCfg->PortDetected == FALSE ) {
      Data32 |= ( cimGppLaneReversal )? 0x0202:0x0404;
      HoldData32 |= 0x2000;
    }
    portCfg = &pConfig->PORTCONFIG[2].PortCfg;
    if ( portCfg->PortDetected == FALSE ) {
      Data32 |= ( cimGppLaneReversal )? 0x0101:0x0808;
      HoldData32 |= 0x4000;
    }
    break;
  case GPP_CFGMODE_X1111:
    portCfg = &pConfig->PORTCONFIG[0].PortCfg;
    if ( portCfg->PortDetected == FALSE ) {
      Data32 |= ( cimGppLaneReversal )? 0x0808:0x0101;
      HoldData32 |= 0x1000;
    }
    portCfg = &pConfig->PORTCONFIG[1].PortCfg;
    if ( portCfg->PortDetected == FALSE ) {
      Data32 |= ( cimGppLaneReversal )? 0x0404:0x0202;
      HoldData32 |= 0x2000;
    }
    portCfg = &pConfig->PORTCONFIG[2].PortCfg;
    if ( portCfg->PortDetected == FALSE ) {
      Data32 |= ( cimGppLaneReversal )? 0x0202:0x0404;
      HoldData32 |= 0x4000;
    }
    portCfg = &pConfig->PORTCONFIG[3].PortCfg;
    if ( portCfg->PortDetected == FALSE ) {
      Data32 |= ( cimGppLaneReversal )? 0x0101:0x0808;
      HoldData32 |= 0x8000;
    }
    break;
  default:
    break;
  }

  // RPR 5.11 Power Saving With GPP Disable
  // ABCFG 0xC0[8] = 0x0
  // ABCFG 0xC0[15:12] = 0xF
  // Enable "Power Saving Feature for A-Link Express Lanes"
  // Enable "Power Saving Feature for GPP Lanes"
  // ABCFG 0x90[19] = 1
  // ABCFG 0x90[6] = 1
  // RCINDC_Reg 0x65 [27:0] = 0xFFFFFFF
  // ABCFG 0xC0[7:4] = 0x0
  if ( cimAlinkPhyPllPowerDown && cimGppPhyPllPowerDown ) {
    abValue = readAlink (SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29));
    writeAlink (SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29), (( abValue | HoldData32 ) & (~ BIT8 )));
    rwAlink (SB_AX_INDXC_REG40, ~(BIT9 + BIT4), (BIT0 + BIT3 + BIT12));
    rwAlink ((SB_ABCFG_REG90 | (UINT32) (ABCFG << 29)), 0xFFFFFFFF, (BIT6 + BIT19));
    rwAlink (RC_INDXC_REG65, 0xFFFFFFFF, ((Data32 & 0x0F) == 0x0F) ? Data32 | 0x0CFF0000 : Data32);
    rwAlink (RC_INDXC_REG40, ~(BIT9 + BIT4), (BIT0 + BIT3 + BIT12));
  }
}


/**
 * sbGppAerInitialization - Initializing AER
 *
 *
 * @param[in] pConfig
 *
 */
VOID
sbGppAerInitialization (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8    PortId;
  UINT32   ValueDd;

  if (pConfig->PcieAER) {
    // GPP strap configuration
    rwAlink (SB_ABCFG_REG310 | (UINT32) (ABCFG << 29), ~(BIT7 + BIT4), BIT28 + BIT27 + BIT26 + BIT1);
    rwAlink (SB_ABCFG_REG314 | (UINT32) (ABCFG << 29), ~(UINT32) (0xfff << 15), 0);

    for (PortId = 0; PortId < MAX_GPP_PORTS; PortId++) {
      ReadPCI (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x00), AccWidthUint32, &ValueDd);
      if (ValueDd != 0xffffffff) {
        rwAlink ((SB_RCINDXP_REG6A | (UINT32) (RCINDXP << 29) | (PortId << 24)), ~BIT1, 0);
        rwAlink ((SB_RCINDXP_REG70 | (UINT32) (RCINDXP << 29) | (PortId << 24)), 0xFFFFE000, 0);
      }
    }

    rwAlink (SB_RCINDXC_REG10, ~(BIT18 + BIT21 + BIT22), 0);

    // AB strap configuration
    rwAlink (SB_ABCFG_REGF0 | (UINT32) (ABCFG << 29), 0xFFFFFFFF, BIT15 + BIT14);
    rwAlink (SB_ABCFG_REGF4 | (UINT32) (ABCFG << 29), 0xFFFFFFFF, BIT3);

    // Enable GPP function0 error reporting
    rwAlink (SB_ABCFG_REG310 | (UINT32) (ABCFG << 29), ~BIT7, BIT7);
    rwAlink (SB_RCINDXC_REGC0, ~BIT9, BIT9);
  } else {
    //OBS220313: Hard System Hang running MeatGrinder Test on multiple blocks
    //RPR 5.13 GPP Error Reporting Configuration
    rwAlink (SB_ABCFG_REGF0 | (UINT32) (ABCFG << 29), ~(BIT1), 0);
    //rwAlink (SB_ABCFG_REG310 | (UINT32) (ABCFG << 29), ~BIT7, 0);
    //rwAlink (SB_RCINDXC_REGC0, ~BIT8, 0);
  }
  //RPR 5.13 GPP Error Reporting Configuration
  rwAlink (SB_ABCFG_REGB8 | (UINT32) (ABCFG << 29), ~(BIT8 + BIT24 + BIT25 + BIT26 + BIT28), BIT8 + BIT24 + BIT26 + BIT28);
}

/**
 * sbGppRasInitialization - Initializing RAS
 *
 *
 * @param[in] pConfig
 *
 */
VOID
sbGppRasInitialization (
  IN     AMDSBCFG*   pConfig
  )
{
  if (pConfig->PcieRAS) {
    rwAlink (SB_ABCFG_REGF4 | (UINT32) (ABCFG << 29), 0xFFFFFFFF, BIT0);
  }
}


//
//-----------------------------------------------------------------------------------
// Early Hudson-2 GPP initialization sequence:
//
// 1) Set port enable bit fields by current GPP link configuration mode
// 2) Deassert GPP reset and pull EP out of reset - Clear GPP_RESET (abcfg:0xC0[8] = 0)
// 3) Loop polling for the link status of all ports
// 4) Misc operations after link training:
//      - (optional) Detect GFX device
//      - Hide empty GPP configuration spaces (Disable empty GPP ports)
//      - (optional) Power down unused GPP ports
//      - (optional) Configure PCIE_P2P_Int_Map (abcfg:0xC4[7:0])
// 5) GPP init completed
//
//
// *) Gen2 vs Gen1
//                                   Gen2 mode     Gen1 mode
//  ---------------------------------------------------------------
//    STRAP_PHY_PLL_CLKF[6:0]          7'h32         7'h19
//    STRAP_BIF_GEN2_EN                  1             0
//
//    PCIE_PHY_PLL clock locks @       5GHz
//
//

/**
 * GPP early programming and link training. On exit all populated EPs should be fully operational.
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
sbPcieGppEarlyInit (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32           reg32Value;
  UINT8            cimNbSbGen2;
  UINT8            cimGppMemWrImprove;
  UINT8            cimGppLaneReversal;
  UINT8            cimAlinkPhyPllPowerDown;
  UINT32           abValue;

  cimNbSbGen2 = pConfig->NbSbGen2;
  cimGppMemWrImprove = pConfig->GppMemWrImprove;
  cimGppLaneReversal = (UINT8) pConfig->GppLaneReversal;
  cimAlinkPhyPllPowerDown = (UINT8) pConfig->AlinkPhyPllPowerDown;
#if  SB_CIMx_PARAMETER == 0
  cimNbSbGen2 = cimNbSbGen2Default;
  cimGppMemWrImprove = cimGppMemWrImproveDefault;
  cimGppLaneReversal = cimGppLaneReversalDefault;
  cimAlinkPhyPllPowerDown = cimAlinkPhyPllPowerDownDefault;
#endif

  outPort80 (0x90);
  //
  // Configure NB-SB link PCIE PHY PLL power down for L1
  //
  if ( cimAlinkPhyPllPowerDown == TRUE ) {
    // Set PCIE_P_CNTL in Alink PCIEIND space
    writeAlink (SB_AX_INDXC_REG30 | (UINT32) (AXINDC << 29), 0x40);
    abValue = readAlink (SB_AX_DATAC_REG34 | (UINT32) (AXINDC << 29));
    abValue |= BIT12 + BIT3 + BIT0;
    abValue &= ~(BIT9 + BIT4);
    writeAlink (SB_AX_DATAC_REG34 | (UINT32) (AXINDC << 29), abValue);
    rwAlink (SB_AX_INDXC_REG02 | (UINT32) (AXINDC << 29), ~(BIT8), (BIT8));
    rwAlink (SB_AX_INDXC_REG02 | (UINT32) (AXINDC << 29), ~(BIT3), (BIT3));
  }

  // AXINDC_Reg 0xA4[18] = 0x1
  writeAlink (SB_AX_INDXP_REG38 | (UINT32) (AXINDP << 29), 0xA4);
  abValue = readAlink (SB_AX_DATAP_REG3C | (UINT32) (AXINDP << 29));
  abValue |= BIT18;
  writeAlink (SB_AX_DATAP_REG3C | (UINT32) (AXINDP << 29), abValue);


  //
  // Set ABCFG 0x031C[0] = 1 to enable lane reversal
  //
  reg32Value = readAlink (SB_ABCFG_REG31C | (UINT32) (ABCFG << 29));
  if ( cimGppLaneReversal ) {
    writeAlink (SB_ABCFG_REG31C | (UINT32) (ABCFG << 29), reg32Value | BIT0);
  } else {
    writeAlink (SB_ABCFG_REG31C | (UINT32) (ABCFG << 29), reg32Value | 0x00);
  }
  //
  // Set abcfg:0x90[20] = 1 to enable GPP bridge multi-function
  //
  reg32Value = readAlink (SB_ABCFG_REG90 | (UINT32) (ABCFG << 29));
  writeAlink (SB_ABCFG_REG90 | (UINT32) (ABCFG << 29), reg32Value | BIT20);


  //
  // Initialize and configure GPP
  //
  if (pConfig->GppFunctionEnable) {
    sbGppTogglePcieReset (pConfig);

    // PreInit - Enable GPP link training
    PreInitGppLink (pConfig);

    //
    // GPP Upstream Memory Write Arbitration Enhancement ABCFG 0x54[26] = 1
    // GPP Memory Write Max Payload Improvement RCINDC_Reg 0x10[12:10] = 0x4
    //
    if ( cimGppMemWrImprove == TRUE ) {
      rwAlink (SB_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~BIT26, (BIT26));
      rwAlink (SB_RCINDXC_REG10 | (UINT32) (RCINDXC << 29), ~(BIT12 + BIT11 + BIT10), (BIT12));
    }

    if (CheckGppLinkStatus (pConfig) && !pConfig->S3Resume) {
      // Toggle GPP reset (Note this affects all Hudson-2 GPP ports)
      sbGppTogglePcieReset (pConfig);
    }

    // Misc operations after link training
    AfterGppLinkInit (pConfig);


    sbGppAerInitialization (pConfig);
    sbGppRasInitialization (pConfig);
  }
  sbGppDynamicPowerSaving (pConfig);
  outPort80 (0x9F);
}

