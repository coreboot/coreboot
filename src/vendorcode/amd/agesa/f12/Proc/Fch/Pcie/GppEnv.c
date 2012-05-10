/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Gpp controller
 *
 * Init Gpp Controller features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 49753 $   @e \$Date: 2011-03-29 04:51:46 +0800 (Tue, 29 Mar 2011) $
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
****************************************************************************
*/
#include "FchPlatform.h"
#include "Ids.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_PCIE_GPPENV_FILECODE
//
// Declaration of local functions
//
VOID  FchGppRasInitialization (IN FCH_DATA_BLOCK* FchDataPtr);
VOID  FchGppAerInitialization (IN FCH_DATA_BLOCK* FchDataPtr);
VOID  PreInitGppLink (IN FCH_DATA_BLOCK* FchDataPtr);
UINT8 CheckGppLinkStatus (IN FCH_DATA_BLOCK* FchDataPtr);
VOID  AfterGppLinkInit (IN FCH_DATA_BLOCK* FchDataPtr);

//
//-----------------------------------------------------------------------------------
// Early GPP initialization sequence:
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
 * FchInitEnvGpp - Config Gpp controller before PCI emulation
 *
 *  - GppEarlyInit
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvGpp (
  IN  VOID     *FchDataPtr
  )
{
  //
  // GppEarlyInit
  //
  UINT8        FchGppMemWrImprove;
  UINT8        FchGppLaneReversal;
  UINT8        FchAlinkPhyPllPowerDown;
  UINT32       AbValue;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  FchGppMemWrImprove = LocalCfgPtr->Gpp.GppMemWrImprove;
  FchGppLaneReversal = (UINT8) LocalCfgPtr->Gpp.GppLaneReversal;
  FchAlinkPhyPllPowerDown = (UINT8) LocalCfgPtr->Ab.UmiPhyPllPowerDown;

  OutPort80 (0x90, StdHeader);

  //
  // Configure NB-FCH link PCIE PHY PLL power down for L1
  //
  if ( FchAlinkPhyPllPowerDown == TRUE ) {
    //
    // Set PCIE_P_CNTL in Alink PCIEIND space
    //
    WriteAlink (FCH_AX_INDXC_REG30 | (UINT32) (AXINDC << 29), 0x40, StdHeader);
    AbValue = ReadAlink (FCH_AX_DATAC_REG34 | (UINT32) (AXINDC << 29), StdHeader);
    AbValue |= BIT12 + BIT3 + BIT0;
    AbValue &= ~(BIT9 + BIT4);
    WriteAlink (FCH_AX_DATAC_REG34 | (UINT32) (AXINDC << 29), AbValue, StdHeader);
    RwAlink (FCH_AX_INDXC_REG02 | (UINT32) (AXINDC << 29), ~(BIT8), (BIT8), StdHeader);
    RwAlink (FCH_AX_INDXC_REG02 | (UINT32) (AXINDC << 29), ~(BIT3), (BIT3), StdHeader);
  }

  //
  // AXINDC_Reg 0xA4[18] = 0x1
  //
  WriteAlink (FCH_AX_INDXP_REG38 | (UINT32) (AXINDP << 29), 0xA4, StdHeader);
  AbValue = ReadAlink (FCH_AX_DATAP_REG3C | (UINT32) (AXINDP << 29), StdHeader);
  AbValue |= BIT18;
  WriteAlink (FCH_AX_DATAP_REG3C | (UINT32) (AXINDP << 29), AbValue, StdHeader);

  //
  // Set ABCFG 0x031C[0] = 1 to enable lane reversal
  //
  AbValue = ReadAlink (FCH_ABCFG_REG31C | (UINT32) (ABCFG << 29), StdHeader);
  if ( FchGppLaneReversal ) {
    WriteAlink (FCH_ABCFG_REG31C | (UINT32) (ABCFG << 29), AbValue | BIT0, StdHeader);
  } else {
    WriteAlink (FCH_ABCFG_REG31C | (UINT32) (ABCFG << 29), AbValue | 0x00, StdHeader);
  }

  //
  // Set abcfg:0x90[20] = 1 to enable GPP bridge multi-function
  //
  AbValue = ReadAlink (FCH_ABCFG_REG90 | (UINT32) (ABCFG << 29), StdHeader);
  WriteAlink (FCH_ABCFG_REG90 | (UINT32) (ABCFG << 29), AbValue | BIT20, StdHeader);

  //
  // Initialize and configure GPP
  //
  if (LocalCfgPtr->Gpp.GppFunctionEnable) {
    ProgramGppTogglePcieReset (LocalCfgPtr->Gpp.GppToggleReset, StdHeader);
    FchGppAerInitialization (LocalCfgPtr);
    FchGppRasInitialization (LocalCfgPtr);

    //
    // PreInit - Enable GPP link training
    //
    PreInitGppLink (LocalCfgPtr);

    //
    // GPP Upstream Memory Write Arbitration Enhancement ABCFG 0x54[26] = 1
    // GPP Memory Write Max Payload Improvement RCINDC_Reg 0x10[12:10] = 0x4
    //
    if ( FchGppMemWrImprove == TRUE ) {
      RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~BIT26, (BIT26), StdHeader);
      RwAlink (FCH_RCINDXC_REG10 | (UINT32) (RCINDXC << 29), ~(BIT12 + BIT11 + BIT10), (BIT12), StdHeader);
    }

    if (CheckGppLinkStatus (LocalCfgPtr) && !LocalCfgPtr->Misc.S3Resume) {
      //
      // Toggle GPP reset (Note this affects all Hudson-2 GPP ports)
      //
      ProgramGppTogglePcieReset (LocalCfgPtr->Gpp.GppToggleReset, StdHeader);
    }

    //
    // Misc operations after link training
    //
    AfterGppLinkInit (LocalCfgPtr);
  }
  FchGppDynamicPowerSaving (LocalCfgPtr);

  OutPort80 (0x9F, StdHeader);
}

/**
 * FchGppAerInitialization - Initializing AER
 *
 *
 * @param[in] FchDataPtr
 *
 */
VOID
FchGppAerInitialization (
  IN  FCH_DATA_BLOCK     *FchDataPtr
  )
{
  AMD_CONFIG_PARAMS     *StdHeader;

  StdHeader = FchDataPtr->StdHeader;

  if (FchDataPtr->Gpp.PcieAer) {
    //
    // GPP strap configuration
    //
    RwAlink (FCH_ABCFG_REG310 | (UINT32) (ABCFG << 29), ~(BIT7 + BIT4), BIT28 + BIT27 + BIT26 + BIT1, StdHeader);
    RwAlink (FCH_ABCFG_REG314 | (UINT32) (ABCFG << 29), ~(UINT32) (0xfff << 15), 0, StdHeader);

    //
    // AB strap configuration
    //
    RwAlink (FCH_ABCFG_REGF0 | (UINT32) (ABCFG << 29), 0xFFFFFFFF, BIT15 + BIT14, StdHeader);
    RwAlink (FCH_ABCFG_REGF4 | (UINT32) (ABCFG << 29), 0xFFFFFFFF, BIT3, StdHeader);
  } else {
    //
    // Hard System Hang running MeatGrinder Test on multiple blocks
    // GPP Error Reporting Configuration
    RwAlink (FCH_ABCFG_REGF0 | (UINT32) (ABCFG << 29), ~(BIT1), 0, StdHeader);
  }

}

/**
 * FchGppRasInitialization - Initializing RAS
 *
 *
 * @param[in] FchDataPtr
 *
 */
VOID
FchGppRasInitialization (
  IN  FCH_DATA_BLOCK     *FchDataPtr
  )
{
  if (FchDataPtr->Gpp.PcieRas) {
    RwAlink (FCH_ABCFG_REGF4 | (UINT32) (ABCFG << 29), 0xFFFFFFFF, BIT0, FchDataPtr->StdHeader);
  }
}

/**
 * PreInitGppLink - Enable GPP link training.
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
PreInitGppLink (
  IN  FCH_DATA_BLOCK     *FchDataPtr
  )
{
  GPP_LINKMODE        CfgMode;
  UINT8               PortId;
  UINT32              GppPortCfg;
  UINT16              Tmp16Value;
  FCH_GPP_PORT_CONFIG  *PortCfg;
  AMD_CONFIG_PARAMS     *StdHeader;

  UINT8               PortMask[5] = {
    0x01,
    0x00,
    0x03,
    0x07,
    0x0F
  };

  //
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
  CfgMode = FchDataPtr->Gpp.GppLinkConfig;
  StdHeader = FchDataPtr->StdHeader;

  ASSERT (CfgMode == PortA4 || CfgMode == PortA2B2 || CfgMode == PortA2B1C1 || CfgMode == PortA1B1C1D1);

  GppPortCfg = (UINT32) PortMask[CfgMode];

  //
  // Mask out non-applicable ports according to the target link configuration mode
  //
  for ( PortId = 0; PortId < MAX_GPP_PORTS; PortId++ ) {
    FchDataPtr->Gpp.PortCfg[PortId].PortPresent &= (UINT8 ) (GppPortCfg >> PortId) & BIT0;
  }

  //
  // Deassert GPP reset and pull EP out of reset - Clear GPP_RESET (abcfg:0xC0[8] = 0)
  //
  Tmp16Value = (UINT16) (~GppPortCfg << 12);
  GppPortCfg = (UINT32) (Tmp16Value + (GppPortCfg << 4) + CfgMode);
  WriteAlink (FCH_ABCFG_REGC0 | (UINT32) (ABCFG << 29), GppPortCfg, StdHeader);

  GppPortCfg = ReadAlink (0xC0 | (UINT32) (RCINDXC << 29), StdHeader);
  WriteAlink (0xC0 | (UINT32) (RCINDXC << 29), GppPortCfg | 0x400, StdHeader);                 /// Set STRAP_F0_MSI_EN

  //
  // A-Link L1 Entry Delay Shortening
  // AXINDP_Reg 0xA0[7:4] = 0x3
  // KR Does not need this portion of code.
  RwAlink (FCH_AX_INDXP_REGA0, 0xFFFFFF0F, 0x30, StdHeader);
  RwAlink (FCH_AX_INDXP_REGB1, 0xFFFFFFFF, BIT19, StdHeader);
  RwAlink (FCH_AX_INDXP_REGB1, 0xFFFFFFFF, BIT28, StdHeader);

  //
  // GPP L1 Entry Delay Shortening
  // RCINDP_Reg 0xA0[7:4] = 0x1 Enter L1 sooner after ACK'ing PM request.
  // This is done to reduce number of NAK received with L1 enabled.
  //
  for ( PortId = 0; PortId < MAX_GPP_PORTS; PortId++ ) {
    RwAlink (FCH_RCINDXP_REGA0 | PortId << 24, 0xFFFFFF0F, 0x10, StdHeader);
    // Hard System Hang running MeatGrinder Test on multiple blocks
    // GPP Error Reporting Configuration
    RwAlink (FCH_RCINDXP_REG6A | PortId << 24, ~(BIT1), 0, StdHeader);
  }

  if (FchDataPtr->Misc.S3Resume) {
    for ( PortId = 0; PortId < MAX_GPP_PORTS; PortId++ ) {
      PortCfg = &FchDataPtr->Gpp.PortCfg[PortId];
      if (PortCfg->PortHotPlug == TRUE) {
        PortCfg->PortDetected = FALSE;
      } else {
        if (PortCfg->PortIsGen2 == 1) {
          FchGppForceGen1 (FchDataPtr, (UINT8) (1 << PortId));
        } else {
          FchGppForceGen2 (FchDataPtr, (UINT8) (1 << PortId));
        }
      }
    }
  }

  //
  // Obtain original Gen2 strap value (LC_GEN2_EN_STRAP)
  //
  FchDataPtr->Gpp.GppGen2Strap = (UINT8) (ReadAlink (FCH_RCINDXP_REGA4 | 0 << 24, StdHeader) & BIT0);
}

/**
 * CheckGppLinkStatus - loop polling the link status for each GPP port
 *
 *
 * Return:     ToggleStatus[3:0] = Port bitmap for those need to clear De-emphasis
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
UINT8
CheckGppLinkStatus (
  IN  FCH_DATA_BLOCK     *FchDataPtr
  )
{
  UINT32              PortId;
  UINT8               PortScanMap;
  UINT8               GppHwDowngrade;
  FCH_GPP_PORT_CONFIG  *PortCfg;
  UINT8               FailedPorts;


  PortScanMap = 0;
  FailedPorts = 0;

  //
  // Obtain a list of ports to be checked
  //
  for ( PortId = 0; PortId < MAX_GPP_PORTS; PortId++ ) {
    PortCfg = &FchDataPtr->Gpp.PortCfg[PortId];
    if ( PortCfg->PortPresent == TRUE && PortCfg->PortDetected == FALSE ) {
      PortScanMap |= 1 << PortId;
    }
  }

  GppHwDowngrade = (UINT8)FchDataPtr->Gpp.GppHardwareDownGrade;
  if (GppHwDowngrade != 0) {
    //
    // Skip polling and always assume this port to be present
    //
    PortScanMap &= ~(1 << (GppHwDowngrade - 1));
  }

  //
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
  //
  FchStall (5000, FchDataPtr->StdHeader);
  if (FchDataPtr->Gpp.GppGen2 && FchDataPtr->Gpp.GppGen2Strap) {
    FchGppForceGen2 (FchDataPtr, PortScanMap);
    FailedPorts = GppPortPollingLtssm (FchDataPtr, PortScanMap, TRUE);

    if (FailedPorts) {
      FchGppForceGen1 (FchDataPtr, FailedPorts);
      FailedPorts = GppPortPollingLtssm (FchDataPtr, FailedPorts, FALSE);
    }
  } else {
    FchGppForceGen1 (FchDataPtr, PortScanMap);
    FailedPorts = GppPortPollingLtssm (FchDataPtr, PortScanMap, FALSE);
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
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
AfterGppLinkInit (
  IN  FCH_DATA_BLOCK     *FchDataPtr
  )
{
  UINT32              PortId;
  FCH_GPP_PORT_CONFIG  *PortCfg;
  UINT32              RegBusNumber;
  UINT32              AbValue;
  UINT32              AbIndex;
  UINT8               Value;
  UINT8               FchGppGen2;
  AMD_CONFIG_PARAMS     *StdHeader;

  StdHeader = FchDataPtr->StdHeader;
  FchGppGen2 = FchDataPtr->Gpp.GppGen2;

  FchDataPtr->Gpp.GppFoundGfxDev = 0;
  AbValue = ReadAlink (FCH_ABCFG_REGC0 | (UINT32) (ABCFG << 29), StdHeader);
  //
  // Link Bandwidth Notification Capability Enable
  //RCINDC:0xC1[0] = 1
  //
  RwAlink (FCH_RCINDXC_REGC1, 0xFFFFFFFF, BIT0, StdHeader);

  for ( PortId = 0; PortId < MAX_GPP_PORTS; PortId++ ) {
    //
    // Program requester ID for every port
    //
    AbIndex = FCH_RCINDXP_REG21 | (UINT32) (RCINDXP << 29) | (PortId << 24);
    WriteAlink (AbIndex, (FCH_GPP_DEV << 3) + PortId, StdHeader);
    //
    // Link Bandwidth Notification Capability Enable
    //PCIe Cfg 0x68[10] = 0
    //PCIe Cfg 0x68[11] = 0
    //
    RwPci (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x68), AccessWidth16, ~(BIT10 + BIT11), 0, StdHeader);

    PortCfg = &FchDataPtr->Gpp.PortCfg[PortId];
    //
    // Check if there is GFX device behind each GPP port
    //
    if ( PortCfg->PortDetected == TRUE ) {
      RegBusNumber = (SBTEMP_BUS << 16) + (SBTEMP_BUS << 8);
      WritePci (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x18), AccessWidth32, &RegBusNumber, StdHeader);
      ReadPci (PCI_ADDRESS (SBTEMP_BUS, 0, 0, 0x0B), AccessWidth8, &Value, StdHeader);
      if ( Value == 3 ) {
        FchDataPtr->Gpp.GppFoundGfxDev |= (1 << PortId);
      }

      RegBusNumber = 0;
      WritePci (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x18), AccessWidth32, &RegBusNumber, StdHeader);
    } else if ( PortCfg->PortPresent == FALSE || PortCfg->PortHotPlug == FALSE ) {
      //
      // Mask off non-applicable ports
      //
      AbValue &= ~(1 << (PortId + 4));
    }

    if ( PortCfg->PortHotPlug == TRUE ) {
      //
      // Hot Plug: PCIe Native Support
      // RCINDP_Reg 0x10[3] = 0x1
      // PCIe_Cfg 0x5A[8] = 0x1
      // PCIe_Cfg 0x6C[6] = 0x1
      // RCINDP_Reg 0x20[19] = 0x0
      //
      RwAlink ((FCH_RCINDXP_REG10 | (UINT32) (RCINDXP << 29) | (PortId << 24)), 0xFFFFFFFF, BIT3, StdHeader);
      RwPci (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x5b), AccessWidth8, 0xff, BIT0, StdHeader);
      RwPci (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x6c), AccessWidth8, 0xff, BIT6, StdHeader);
      RwAlink ((FCH_RCINDXP_REG20 | (UINT32) (RCINDXP << 29) | (PortId << 24)), ~BIT19, 0, StdHeader);
    }
  }

  if ( FchDataPtr->Gpp.GppUnhidePorts == FALSE ) {
    if ((AbValue & 0xF0) == 0) {
      AbValue = BIT8;                                                               /// if all ports are empty set GPP_RESET
    } else if ((AbValue & 0xE0) != 0 && (AbValue & 0x10) == 0) {
      AbValue |= BIT4;                                                              /// PortA should always be visible whenever other ports are exist
    }

    //
    // Update GPP_Portx_Enable (abcfg:0xC0[7:5])
    //
    WriteAlink (FCH_ABCFG_REGC0 | (UINT32) (ABCFG << 29), AbValue, StdHeader);
  }

  //
  // Common initialization for open GPP ports
  //
  for ( PortId = 0; PortId < MAX_GPP_PORTS; PortId++ ) {
    ReadPci (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x80), AccessWidth8, &Value, StdHeader);
    if (Value != 0xff) {
      //
      // Set pciCfg:PCIE_DEVICE_CNTL2[3:0] = 4'h6 (0x80[3:0])
      //
      Value &= 0xf0;
      Value |= 0x06;
      WritePci (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x80), AccessWidth8, &Value, StdHeader);

      //
      // Set PCIEIND_P:PCIE_RX_CNTL[RX_RCB_CPL_TIMEOUT_MODE] (0x70:[19]) = 1
      //
      AbIndex = FCH_RCINDXP_REG70 | (UINT32) (RCINDXP << 29) | (PortId << 24);
      AbValue = ReadAlink (AbIndex, StdHeader) | BIT19;
      WriteAlink (AbIndex, AbValue, StdHeader);

      //
      // Set PCIEIND_P:PCIE_TX_CNTL[TX_FLUSH_TLP_DIS] (0x20:[19]) = 0
      //
      AbIndex = FCH_RCINDXP_REG20 | (UINT32) (RCINDXP << 29) | (PortId << 24);
      AbValue = ReadAlink (AbIndex, StdHeader) & ~BIT19;
      WriteAlink (AbIndex, AbValue, StdHeader);

    }
  }
}

