
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
 * ***************************************************************************
 *
 */
 
#include "SBPLATFORM.h"
#include "cbtypes.h"

/**
 * PCIE_CAP_ID - PCIe Cap ID
 *
 */
#define  PCIE_CAP_ID             0x10

//
// Declaration of local functions
//

/**
 * PreInitGppLink - Enable GPP link training.
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
  VOID  PreInitGppLink (IN AMDSBCFG* pConfig);
  UINT8 CheckGppLinkStatus (IN AMDSBCFG* pConfig);
  VOID  AfterGppLinkInit (IN AMDSBCFG* pConfig);
  VOID  sbGppForceGen2 (IN UINT32 portId );
  VOID  sbGppForceGen1 (IN UINT32 portId );
  VOID  sbGppDisableUnusedPadMap (IN AMDSBCFG* pConfig );
  VOID  sbGppSetAspm (IN UINT32 pciAddress, IN UINT8  LxState);
  UINT8 sbFindPciCap (IN UINT32 pciAddress, IN UINT8 targetCapId);

//
// Declaration of external functions
//

//
//-----------------------------------------------------------------------------------
// Early SB800 GPP initialization sequence:
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
  UINT8   TogglePort;
  UINT8   portNum;
  UINT32  reg32Value;
  UINT8   retryCount;
  UINT8   cimGppMemWrImprove;
  UINT8   cimGppLaneReversal;
  UINT8   cimAlinkPhyPllPowerDown;

  cimGppMemWrImprove = pConfig->GppMemWrImprove;
  cimGppLaneReversal = (UINT8) pConfig->GppLaneReversal;
  cimAlinkPhyPllPowerDown = (UINT8) pConfig->AlinkPhyPllPowerDown;
#if  SB_CIMx_PARAMETER == 0
  cimGppMemWrImprove = cimGppMemWrImproveDefault;
  cimGppLaneReversal = cimGppLaneReversalDefault;
  cimAlinkPhyPllPowerDown = cimAlinkPhyPllPowerDownDefault;
#endif

//
// Configure NB-SB link PCIE PHY PLL power down for L1
//
  if ( cimAlinkPhyPllPowerDown == TRUE ) {
    UINT32  abValue;
    // Set PCIE_P_CNTL in Alink PCIEIND space
    writeAlink (SB_AX_INDXC_REG30 | (UINT32) (AXINDC << 29), 0x40);
    abValue = readAlink (SB_AX_DATAC_REG34 | (UINT32) (AXINDC << 29));
    abValue |= BIT12 + BIT3 + BIT0;
    abValue &= ~(BIT9 + BIT4);
    writeAlink (SB_AX_DATAC_REG34 | (UINT32) (AXINDC << 29), abValue);
    rwAlink (SB_AX_INDXC_REG02 | (UINT32) (AXINDC << 29), ~BIT8, (BIT8));
  }

//
// Set ABCFG 0x031C[0] = 1 enable the lane reversal support.
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

    if ( pConfig->S3Resume ) {
      for ( portNum = 0; portNum < MAX_GPP_PORTS; portNum++ ) {
        reg32Value = readAlink ((SB_ABCFG_REG340 + portNum * 4) | (UINT32) (ABCFG << 29));
        writeAlink ((SB_ABCFG_REG340 + portNum * 4) | (UINT32) (ABCFG << 29), reg32Value & ~BIT21);
      }
    }
    //
    //  a) Loop polling regA5 -> LcState (timeout ~100ms);
    //  b) if (LcState[5:0] == 0x10), training successful, go to g);
    //  c) if any of (LcState[13:8], [21:16], [29:24]) == 0x29 or 0x2A:
    //  d) Clear De-emphasis bit for relevant ports;
    //  e) Toggle GPP reset signal (via OEM callback);
    //  f) go back to a);
    //  g) exit;
    //
    for (retryCount = 0; retryCount < MAX_GPP_RESETS; retryCount++) {
      // Polling each GPP port for link status
      TogglePort = CheckGppLinkStatus (pConfig);

      if (TogglePort == 0) {
        break;
      } else {
        // Check failure port and clear STRAP_BIF_DE_EMPHASIS_SEL_x_GPP bit (abcfg:0x34[0, 4, 8, C][21]=0)
        for ( portNum = 0; portNum < MAX_GPP_PORTS; portNum++ ) {
          if (TogglePort & (1 << portNum)) {
            reg32Value = readAlink ((SB_ABCFG_REG340 + portNum * 4) | (UINT32) (ABCFG << 29));
            writeAlink ((SB_ABCFG_REG340 + portNum * 4) | (UINT32) (ABCFG << 29), reg32Value & ~BIT21);
          }
          sbGppForceGen1 (portNum);
        }

        // Toggle GPP reset (Note this affects all SB800 GPP ports)
        CallBackToOEM (CB_SBGPP_RESET_ASSERT, (UINT32)TogglePort, pConfig);
        SbStall (500);
        CallBackToOEM (CB_SBGPP_RESET_DEASSERT, (UINT32)TogglePort, pConfig);
      }
    };

    // Misc operations after link training
    AfterGppLinkInit (pConfig);
  } else {

// RPR 5.11 Power Saving With GPP Disable
// ABCFG 0xC0[8] = 0x0
// ABCFG 0xC0[15:12] = 0xF
// Enable "Power Saving Feature for A-Link Express Lanes"
// Enable "Power Saving Feature for GPP Lanes"
// ABCFG 0x90[19] = 1
// ABCFG 0x90[6] = 1
// RCINDC_Reg 0x65 [27:0] = 0xFFFFFFF
// ABCFG 0xC0[7:4] = 0x0

    rwAlink (SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29), ~BIT8, (BIT4 + BIT5 + BIT6 + BIT7));
    rwAlink (SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29), 0xFFFFFFFF, (BIT12 + BIT13 + BIT14 + BIT15));
    rwAlink (SB_AX_INDXC_REG40, ~(BIT9 + BIT4), (BIT0 + BIT3 + BIT12));
    rwAlink (RC_INDXC_REG40, ~(BIT9 + BIT4), (BIT0 + BIT3 + BIT12));
    rwAlink ((SB_ABCFG_REG90 | (UINT32) (ABCFG << 29)), 0xFFFFFFFF, (BIT6 + BIT19));
    rwAlink (RC_INDXC_REG65, 0xFFFFFFFF, 0x0fffffff);
    rwAlink ((SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29)), ~(BIT4 + BIT5 + BIT6 + BIT7), 0);
  }
  sbGppDisableUnusedPadMap ( pConfig );
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
  UINT8 portMask[5] = {0x01,
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
  for ( portId = 0; portId < MAX_GPP_PORTS; portId++ ) {
    rwAlink (SB_RCINDXP_REGA0 | portId << 24, 0xFFFFFF0F, 0x10);
  }
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
  UINT32  retryCounter;
  UINT32  portId;
  UINT32  abIndex;
  UINT32  Data32;
  UINT8   portScanMap;
  UINT8   portScanMap2;
  UINT8   ToggleStatus;
  UINT16  i;
  SBGPPPORTCONFIG  *portCfg;


  portScanMap = 0;
  retryCounter = MAX_TRAINING_RETRY;
  ToggleStatus = 0;

  // Obtain a list of ports to be checked
  for ( portId = 0; portId < MAX_GPP_PORTS; portId++ ) {
    portCfg = &pConfig->PORTCONFIG[portId].PortCfg;
    if ( portCfg->PortPresent == TRUE && portCfg->PortDetected == FALSE ) {
      portScanMap |= 1 << portId;
    }
  }
  portScanMap2 = portScanMap;

  //
  // After training is enabled, Check LCSTATE for each port, if LCSTATE<= 4, then keep
  // polling for up to 40ms. If LCSTATE still <= 4, then assume the port to be empty.
  //
  i = 400;
  while ( --i && portScanMap2) {
    for (portId = 0; portId < MAX_GPP_PORTS; portId++) {
      portCfg = &pConfig->PORTCONFIG[portId].PortCfg;
      if (((portCfg->PortHotPlug == FALSE) || ((portCfg->PortHotPlug == TRUE) && (pConfig->S3Resume == FALSE)) ) && (portScanMap2 & (1 << portId))) {
        //
        // Get port link state (reading LC_CURRENT_STATE of PCIEIND_P)
        //
        abIndex = SB_RCINDXP_REGA5 | (UINT32) (RCINDXP << 29) | (portId << 24);
        Data32 = readAlink (abIndex) & 0x3F;
        if ((UINT8) (Data32) > 4) {
          portScanMap2 &= ~(1 << portId);       // This port is not empty
          break;
        }
        SbStall (100);                          // Delay 100us
      }
    }
  }
  portScanMap &= ~portScanMap2;                 // Mark remaining ports as empty


  while ( --retryCounter && portScanMap ) {
    for ( portId = 0; portId < MAX_GPP_PORTS; portId++ ) {
      portCfg = &pConfig->PORTCONFIG[portId].PortCfg;
      if (( portCfg->PortHotPlug == TRUE ) && ( pConfig->S3Resume )) {
        continue;
      }
      if ( portCfg->PortPresent == TRUE && portCfg->PortDetected == FALSE ) {
        //
        // Get port link state (reading LC_CURRENT_STATE of PCIEIND_P)
        //
        SbStall (1000);                          // Delay 400us
        abIndex = SB_RCINDXP_REGA5 | (UINT32) (RCINDXP << 29) | (portId << 24);
        Data32 = readAlink (abIndex) & 0x3F3F3F3F;

        if ( (UINT8) (Data32) == 0x10 ) {
          portCfg->PortDetected = TRUE;
          portScanMap &= ~(1 << portId);
        } else {
          for (i = 0; i < 4; i++) {
            //
            // Compliance mode (0x7), downgrade from Gen2 to Gen1 (*A12)
            //
            if ((UINT8) (Data32) == 0x29 || (UINT8) (Data32) == 0x2A || (UINT8) (Data32) == 0x7 ) {
              ToggleStatus |= (1 << portId);                 // A11 only: need to toggle GPP reset
              portScanMap &= ~(1 << portId);
            }
            Data32 >>= 8;
          }
        }
      }
    }
  }
  return ToggleStatus;
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
  UINT32  portId;
  SBGPPPORTCONFIG  *portCfg;
  UINT32  regBusNumber;
  UINT32  abValue;
  UINT32  abIndex;
  UINT32  i;
  UINT32  Data32;
  UINT8   bValue;
  UINT8   cimGppGen2;

  cimGppGen2 = pConfig->GppGen2;
#if  SB_CIMx_PARAMETER == 0
  cimGppGen2 = cimGppGen2Default;
#endif

  bValue = GPP_EFUSE_LOCATION;
  getEfuseStatus (&bValue);
  if ( (bValue & GPP_GEN2_EFUSE_BIT) != 0  ) {
    cimGppGen2 = FALSE;
  } else {
    pConfig->CoreGen2Enable = TRUE;          // Output for platform use
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
  for ( portId = 0; portId < MAX_GPP_PORTS; portId++ ) {
    portCfg = &pConfig->PORTCONFIG[portId].PortCfg;
    abValue = readAlink (SB_RCINDXP_REGA4 | portId << 24) & BIT0;
    if (( cimGppGen2 == TRUE ) && (abValue == BIT0) && (portCfg->PortDetected == TRUE))  {
      portCfg->PortIsGen2 = TRUE;            // Output for platform use
      sbGppForceGen2 (portId);
      //_asm {jmp $};
      SbStall (400);                          // Delay 400us
      i = 500;
      Data32 = 0;
      while ( --i ) {
        abIndex = SB_RCINDXP_REGA5 | (UINT32) (RCINDXP << 29) | (portId << 24);
        Data32 = readAlink (abIndex) & 0x3F;
        if ((UINT8) (Data32) == 0x10) {
          break;
        }
        SbStall (400);                          // Delay 100us
      }
      if (!( (UINT8) (Data32) == 0x10 )) {
        if (pConfig->GppCompliance == FALSE) {
          portCfg->PortIsGen2 = FALSE;       // Revert to default; output for platform use
          sbGppForceGen1 (portId);
        }
      }
    } else {
      if (pConfig->GppCompliance == FALSE) {
        sbGppForceGen1 (portId);
      }
    }
//RPR 5.9 Link Bandwidth Notification Capability Enable
//RCINDC 0xC1[0] = 1
//PCIe Cfg 0x68[10] = 0
//PCIe Cfg 0x68[11] = 0

    rwAlink (SB_RCINDXC_REGC1, 0xFFFFFFFF, BIT0);
    RWPCI (PCI_ADDRESS (0, GPP_DEV_NUM, portId, 0x68), AccWidthUint16, ~(BIT10 + BIT11), 0);
  }

//  Status = AGESA_SUCCESS;
  pConfig->GppFoundGfxDev = 0;
  abValue = readAlink (SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29));

  for ( portId = 0; portId < MAX_GPP_PORTS; portId++ ) {
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
    }

    // Mask off non-applicable ports
    else if ( portCfg->PortPresent == FALSE ) {
      abValue &= ~(1 << (portId + 4));
    }
    // Mask off empty port if the port is not hotplug-capable
    else if ( portCfg->PortHotPlug == FALSE ) {
      abValue &= ~(1 << (portId + 4));
    }
    // Clear STRAP_BIF_DE_EMPHASIS_SEL_x_GPP bit (abcfg:0x34[0, 4, 8, C][21]=0) to make hotplug working
    if ( portCfg->PortHotPlug == TRUE ) {
      rwAlink ((SB_ABCFG_REG340 + portId * 4) | (UINT32) (ABCFG << 29), ~BIT21, 0);

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

    }
  }
}


/**
 * sbPcieGppLateInit - Late PCIE initialization for SB800 GPP component
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
  UINT32  reg32Value;
  UINT8   portId;
  UINT8   busNum;
  UINT8   aspmValue;
  UINT8   reg8Value;
  UINT8   cimGppPhyPllPowerDown;

  reg8Value = 0x01;
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

  for ( portId = 0; portId < MAX_GPP_PORTS; portId++ ) {
    // write pci_reg3d with 0x01 to fix yellow mark for GPP bridge under Vista
    // when native PCIE is enabled but MSI is not available
    // SB02029: SB800 BIF/GPP allowing strap STRAP_BIF_INTERRUPT_PIN_SB controlled by AB reg
    WritePCI (PCI_ADDRESS (0, 21, portId, 0x3d), AccWidthUint8, &reg8Value);
    ReadPCI (PCI_ADDRESS (0, 21, portId, 0x19), AccWidthUint8, &busNum);
    if (busNum != 0xFF) {
      ReadPCI (PCI_ADDRESS (busNum, 0, 0, 0x00), AccWidthUint32, &reg32Value);
      if (reg32Value != 0xffffffff) {
        // Set ASPM on EP side
        sbGppSetAspm (PCI_ADDRESS (busNum, 0, 0, 0), aspmValue & 0x3);
        // Set ASPM on port side
        sbGppSetAspm (PCI_ADDRESS (0, 21, portId, 0), aspmValue & 0x3);
      }
    }
    aspmValue = aspmValue >> 2;
  }

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
    }
  }

//
// Configure Lock HWInit registers
//
  reg32Value = readAlink (SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29));
//
// Disable hidden register decode and serial number capability
//
  reg32Value = readAlink (SB_ABCFG_REG330 | (UINT32) (ABCFG << 29));
  writeAlink (SB_ABCFG_REG330 | (UINT32) (ABCFG << 29), reg32Value & ~(BIT26 + BIT10));
}

/**
 * sbGppSetAspm - Set SPP ASPM
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
        // Set ASPM state in link control register
        RWPCI (devBDF + pcieCapOffset + 0x10, AccWidthUint8, 0xffffffff, LxState & value8);
      }
    }
  maxFuncs--;
  }
}

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
 * sbGppForceGen2 - Set SPP to GENII
 *
 *
 * @param[in] portId
 *
 */
VOID
sbGppForceGen2 (
  IN       UINT32     portId
  )
{
  RWPCI (PCI_ADDRESS (0, GPP_DEV_NUM, portId, 0x88), AccWidthUint8, 0xf0, 0x02);
  rwAlink (SB_RCINDXP_REGA2 | portId << 24, ~BIT13, 0);
  rwAlink (SB_RCINDXP_REGC0 | portId << 24, ~BIT15, 0);
  rwAlink (SB_RCINDXP_REGA4 | portId << 24, 0xFFFFFFFF, BIT29);
}

/**
 * sbGppForceGen1 - Set SPP to GENI
 *
 *
 * @param[in] portId
 *
 */
VOID
sbGppForceGen1 (
  IN       UINT32     portId
  )
{
  RWPCI (PCI_ADDRESS (0, GPP_DEV_NUM, portId, 0x88), AccWidthUint8, 0xf0, 0x01);
  rwAlink (SB_RCINDXP_REGA4 | portId << 24, ~BIT0, 0);
  rwAlink (SB_RCINDXP_REGA2 | portId << 24, 0xFFFFFFFF, BIT13);
  rwAlink (SB_RCINDXP_REGC0 | portId << 24, ~BIT15, 0);
  rwAlink (SB_RCINDXP_REGA4 | portId << 24, 0xFFFFFFFF, BIT29);
}

/**
 * sbGppDisableUnusedPadMap - Return GPP Pad Map
 *
 *
 * @param[in] pConfig
 *
 */
VOID
sbGppDisableUnusedPadMap (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32  Data32;
  UINT32  HoldData32;
  SBGPPPORTCONFIG  *portCfg;
  UINT8  cimGppLaneReversal;
  UINT8  cimAlinkPhyPllPowerDown;
  UINT8  cimGppPhyPllPowerDown;

  cimAlinkPhyPllPowerDown = (UINT8) pConfig->AlinkPhyPllPowerDown;
  cimGppLaneReversal =  (UINT8) pConfig->GppLaneReversal;
  cimGppPhyPllPowerDown =  (UINT8) pConfig->GppPhyPllPowerDown;
#if  SB_CIMx_PARAMETER == 0
  cimGppLaneReversal = cimGppLaneReversalDefault;
  cimAlinkPhyPllPowerDown = cimAlinkPhyPllPowerDownDefault;
  cimGppPhyPllPowerDown = cimGppPhyPllPowerDownDefault;
#endif

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
  if ( (Data32 & 0xf) == 0xf ) Data32 |= 0x0cff0000;
  if ( cimAlinkPhyPllPowerDown && cimGppPhyPllPowerDown ) {
    rwAlink (SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29), ~BIT8, 0);
    rwAlink (SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29), 0xFFFFFFFF, HoldData32);
    rwAlink (SB_AX_INDXC_REG40, ~(BIT9 + BIT4), (BIT0 + BIT3 + BIT12));
    rwAlink (RC_INDXC_REG40, ~(BIT9 + BIT4), (BIT0 + BIT3 + BIT12));
    rwAlink ((SB_ABCFG_REG90 | (UINT32) (ABCFG << 29)), 0xFFFFFFFF, (BIT6 + BIT19));
    rwAlink (RC_INDXC_REG65, 0xFFFFFFFF, Data32);
  }
}
