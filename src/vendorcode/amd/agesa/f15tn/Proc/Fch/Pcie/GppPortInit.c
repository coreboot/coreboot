/* $NoKeywords:$ */
/**
 * @file
 *
 * Config and Train Fch Gpp Ports
 *
 * Init Gpp Controller features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
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
****************************************************************************
*/
#include "FchPlatform.h"
#include "Ids.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_PCIE_GPPPORTINIT_FILECODE

//
// Declaration of local functions
//
/**
 * GppPortPollingLtssmS3 - Loop polling the LTSSM for each GPP port marked in PortMap (New Algorithm S3)
 *
 *
 * @param[in] FchGpp        Pointer to Fch GPP configuration structure
 * @param[in] ActivePorts   A bitmap of ports which should be polled
 * @param[in] IsGen2        TRUE if the polling is in Gen2 mode
 * @param[in] StdHeader     Pointer to AMD_CONFIG_PARAMS
 *
 * @retval       FailedPorts     A bitmap of ports which failed to train
 *
 */
STATIC UINT8
GppPortPollingLtssmS3 (
  IN       FCH_GPP             *FchGpp,
  IN       UINT8               ActivePorts,
  IN       BOOLEAN             IsGen2,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8                  PortId;
  UINT8                  FailedPorts;
  FCH_GPP_PORT_CONFIG    *PortCfg;
  UINT32                 AbIndex;
  UINT32                 GppData32;
  UINT8                  EmptyPorts;
  UINT8                  RetryCounter;

  FailedPorts = 0;
  EmptyPorts = ActivePorts;
  RetryCounter = 2;

  while (RetryCounter-- ) {
    for (PortId = 0; PortId < MAX_GPP_PORTS; PortId++) {
      if (ActivePorts & (1 << PortId)) {
        PortCfg = &FchGpp->PortCfg[PortId];
        if ( PortCfg->PortDetected == TRUE ) {
          AbIndex = FCH_RCINDXP_REGA5 | (UINT32) (PortId << 24);
          GppData32 = ReadAlink (AbIndex, StdHeader) & 0x3F3F3F3F;

          if ((UINT8) (GppData32) > 0x04) {
            EmptyPorts &= ~(1 << PortId);
          }

          if ((UINT8) (GppData32) == 0x10) {
            break;
          }
        }
      }
    }
    FchStall (180, StdHeader);
  }
  FailedPorts |= ActivePorts;
  return FailedPorts;
}

/**
 * PreInitGppLink - Enable GPP link training.
 *
 *
 *
 * @param[in] FchGpp      Pointer to Fch GPP configuration structure
 * @param[in] StdHeader   Pointer to AMD_CONFIG_PARAMS
 *
 */
STATIC VOID
PreInitGppLink (
  IN       FCH_GPP             *FchGpp,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  GPP_LINKMODE          CfgMode;
  UINT8                 PortId;
  UINT32                GppPortCfg;
  UINT16                Tmp16Value;
  UINT8                 GppS3Data;
  UINT8                 HotPlugPorts;

  UINT8               PortMask[5] = {
    0x01,
    0x00,
    0x03,
    0x07,
    0x0F
  };

  HotPlugPorts = 0;
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
  CfgMode = FchGpp->GppLinkConfig;
  ASSERT (CfgMode == PortA4 || CfgMode == PortA2B2 || CfgMode == PortA2B1C1 || CfgMode == PortA1B1C1D1);
  GppPortCfg = (UINT32) PortMask[CfgMode];

  //
  // Mask out non-applicable ports according to the target link configuration mode
  //
  for ( PortId = 0; PortId < MAX_GPP_PORTS; PortId++ ) {
    FchGpp->PortCfg[PortId].PortPresent &= (UINT8 ) (GppPortCfg >> PortId) & BIT0;
    if ( FchGpp->PortCfg[PortId].PortHotPlug == TRUE ) {
      HotPlugPorts |= ( 1 << PortId);
    }
  }

  //
  // Deassert GPP reset and pull EP out of reset - Clear GPP_RESET (abcfg:0xC0[8] = 0)
  //
  Tmp16Value = (UINT16) (~GppPortCfg << 12);
  GppPortCfg = (UINT32) (Tmp16Value + (GppPortCfg << 4) + CfgMode);
  WriteAlink (FCH_ABCFG_REGC0 | (UINT32) (ABCFG << 29), GppPortCfg, StdHeader);

  GppPortCfg = ReadAlink (0xC0 | (UINT32) (RCINDXC << 29), StdHeader);
  WriteAlink (0xC0 | (UINT32) (RCINDXC << 29), GppPortCfg | 0x400, StdHeader);      /// Set STRAP_F0_MSI_EN

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
    RwAlink (FCH_RCINDXP_REG6A | PortId << 24, (UINT32)~(BIT1), 0, StdHeader);
  }


  if (ReadFchSleepType (StdHeader) == ACPI_SLPTYP_S3) {

    ReadMem ( ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0D, AccessWidth8, &GppS3Data);
    for ( PortId = 0; PortId < MAX_GPP_PORTS; PortId++ ) {
      if ( GppS3Data & (1 << (PortId + 4))) {
        if ( GppS3Data & (1 << PortId)) {
          FchGppForceGen1 (FchGpp, (1 << PortId), StdHeader);
        } else {
          FchGppForceGen2 (FchGpp, (1 << PortId), StdHeader);
        }
      }
    }
  }
  //
  // Obtain original Gen2 strap value (LC_GEN2_EN_STRAP)
  //
  FchGpp->GppGen2Strap = (UINT8) (ReadAlink (FCH_RCINDXP_REGA4 | 0 << 24, StdHeader) & BIT0);
  FchGpp->HotPlugPortsStatus = HotPlugPorts;
}


/**
 * CheckGppLinkStatus - loop polling the link status for each GPP port
 *
 *
 * Return:     ToggleStatus[3:0] = Port bitmap for those need to clear De-emphasis
 *
 * @param[in] FchGpp      Pointer to Fch GPP configuration structure
 * @param[in] StdHeader   Pointer to AMD_CONFIG_PARAMS
 *
 */
STATIC UINT8
CheckGppLinkStatus (
  IN       FCH_GPP             *FchGpp,
  IN       AMD_CONFIG_PARAMS   *StdHeader
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
    PortCfg = &FchGpp->PortCfg[PortId];
    if ( PortCfg->PortPresent == TRUE && PortCfg->PortDetected == FALSE ) {
      PortScanMap |= 1 << PortId;
    }
  }

  GppHwDowngrade = (UINT8) FchGpp->GppHardwareDownGrade;
  if (GppHwDowngrade != 0) {
    //
    // Skip polling and always assume this port to be present
    //
    PortScanMap &= ~(1 << (GppHwDowngrade - 1));
  }

  FchStall (5000, StdHeader);
  if (FchGpp->GppGen2 && FchGpp->GppGen2Strap) {
    AGESA_TESTPOINT (TpFchGppGen2PortPolling, StdHeader);
    FchGppForceGen2 (FchGpp, PortScanMap, StdHeader);
    FailedPorts = GppPortPollingLtssm (FchGpp, PortScanMap, TRUE, StdHeader);

    if (FailedPorts) {
      AGESA_TESTPOINT (TpFchGppGen1PortPolling, StdHeader);
      FchGppForceGen1 (FchGpp, FailedPorts, StdHeader);
      FailedPorts = GppPortPollingLtssm (FchGpp, FailedPorts, FALSE, StdHeader);
    }
  } else {
    AGESA_TESTPOINT (TpFchGppGen1PortPolling, StdHeader);
    FchGppForceGen1 (FchGpp, PortScanMap, StdHeader);
    FailedPorts = GppPortPollingLtssm (FchGpp, PortScanMap, FALSE, StdHeader);
  }
  return FailedPorts;
}

STATIC
BOOLEAN
FoundInfiniteCrs (
  IN     FCH_GPP             *FchGpp,
  IN     AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32                PortId;
  UINT32                Value32;
  UINT32                RegBusNo;
  UINT32                FchTempBus;
  FCH_GPP_PORT_CONFIG   *PortCfg;

  FchTempBus = GppGetFchTempBus (StdHeader);
  for ( PortId = 0; PortId < MAX_GPP_PORTS; PortId++ ) {
    PortCfg = &FchGpp->PortCfg[PortId];
    if ( PortCfg->PortDetected == TRUE ) {
      RegBusNo = (FchTempBus << 16) + (FchTempBus << 8);
      WritePci (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x18), AccessWidth32, &RegBusNo, StdHeader);
      ReadPci (PCI_ADDRESS (FchTempBus, 0, 0, 0x08), AccessWidth32, &Value32, StdHeader);
      RegBusNo = 0;
      WritePci (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x18), AccessWidth32, &RegBusNo, StdHeader);

      if ( Value32 == 0xFFFFFFFF ) {
        return TRUE;
      }
    }
  }
  return FALSE;
}


/**
 * AfterGppLinkInit
 *       - Search for display device behind each GPP port
 *       - If the port is empty AND not hotplug-capable:
 *           * Turn off link training
 *           * (optional) Power down the port
 *           * Hide the configuration space (Turn off the port)
 *
 * @param[in] FchGpp      Pointer to Fch GPP configuration structure
 * @param[in] StdHeader   Pointer to AMD_CONFIG_PARAMS
 *
 */
STATIC VOID
AfterGppLinkInit (
  IN       FCH_GPP             *FchGpp,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32                PortId;
  FCH_GPP_PORT_CONFIG   *PortCfg;
  UINT32                RegBusNumber;
  UINT32                FchTempBus;
  UINT32                AbValue;
  UINT32                AbIndex;
  UINT8                 Value;

  FchGpp->GppFoundGfxDev = 0;
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
    AbIndex = FCH_RCINDXP_REG21 | (UINT32) (PortId << 24);
    WriteAlink (AbIndex, (FCH_GPP_DEV << 3) + PortId, StdHeader);
    //
    // Link Bandwidth Notification Capability Enable
    //PCIe Cfg 0x68[10] = 0
    //PCIe Cfg 0x68[11] = 0
    //
    RwPci (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x68), AccessWidth16, (UINT32)~(BIT10 + BIT11), 0, StdHeader);

    PortCfg = &FchGpp->PortCfg[PortId];
    //
    // Check if there is GFX device behind each GPP port
    //
    FchTempBus = GppGetFchTempBus (StdHeader);
    if ( PortCfg->PortDetected == TRUE ) {
      RegBusNumber = (FchTempBus << 16) + (FchTempBus << 8);
      WritePci (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x18), AccessWidth32, &RegBusNumber, StdHeader);
      ReadPci (PCI_ADDRESS (FchTempBus, 0, 0, 0x0B), AccessWidth8, &Value, StdHeader);
      if ( Value == 3 ) {
        FchGpp->GppFoundGfxDev |= (1 << PortId);
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
      RwAlink ((FCH_RCINDXP_REG10 | (UINT32) (PortId << 24)), 0xFFFFFFFF, BIT3, StdHeader);
      RwPci (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x5b), AccessWidth8, 0xff, BIT0, StdHeader);
      RwPci (PCI_ADDRESS (0, GPP_DEV_NUM, PortId, 0x6c), AccessWidth8, 0xff, BIT6, StdHeader);
      RwAlink ((FCH_RCINDXP_REG20 | (UINT32) (PortId << 24)), (UINT32)~BIT19, 0, StdHeader);
    }
  }

  if ( FchGpp->GppUnhidePorts == FALSE ) {
    if ((AbValue & 0xF0) == 0) {
      //comment out the following line for BUG284426: GPP_RESET causes S3 resume hard hang on Pumori
      //AbValue = BIT8;                // if all ports are empty set GPP_RESET
    } else if ((AbValue & 0xE0) != 0 && (AbValue & 0x10) == 0) {
      AbValue |= BIT4;               // PortA should always be visible whenever other ports are exist
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
      AbIndex = FCH_RCINDXP_REG70 | (UINT32) (PortId << 24);
      AbValue = ReadAlink (AbIndex, StdHeader) | BIT19;
      WriteAlink (AbIndex, AbValue, StdHeader);

      //
      // Set PCIEIND_P:PCIE_TX_CNTL[TX_FLUSH_TLP_DIS] (0x20:[19]) = 0
      //
      AbIndex = FCH_RCINDXP_REG20 | (UINT32) (PortId << 24);
      AbValue = ReadAlink (AbIndex, StdHeader) & ~BIT19;
      WriteAlink (AbIndex, AbValue, StdHeader);

      //
      // Set Immediate Ack PM_Active_State_Request_L1 (0xA0:[23]) = 1
      //
      AbIndex = FCH_RCINDXP_REGA0 | (UINT32) (PortId << 24);
      AbValue = ReadAlink (AbIndex, StdHeader) & ~BIT23;
      if ( FchGpp->GppL1ImmediateAck == 0) {
        AbValue |= BIT23;
      }
      WriteAlink (AbIndex, AbValue, StdHeader);
    }
  }
}


/**
 * FchGppAerInitialization - Initializing AER
 *
 *
 * @param[in] FchGpp      Pointer to Fch GPP configuration structure
 * @param[in] StdHeader   Pointer to AMD_CONFIG_PARAMS
 *
 */
STATIC VOID
FchGppAerInitialization (
  IN       FCH_GPP             *FchGpp,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  if (FchGpp->PcieAer) {
    //
    // GPP strap configuration
    //
    RwAlink (0x310  | (UINT32) (ABCFG << 29), (UINT32)~(BIT7 + BIT4), BIT28 + BIT27 + BIT26 + BIT1, StdHeader);
    RwAlink (0x314  | (UINT32) (ABCFG << 29), ~(UINT32) (0xfff << 15), 0, StdHeader);

    //
    // AB strap configuration
    //
    RwAlink (FCH_ABCFG_REGF0 | (UINT32) (ABCFG << 29), 0xFFFFFFFF, BIT15 + BIT14, StdHeader);
    RwAlink (FCH_ABCFG_REGF4 | (UINT32) (ABCFG << 29), 0xFFFFFFFF, BIT3, StdHeader);
  } else {
    //
    // Hard System Hang running MeatGrinder Test on multiple blocks
    // GPP Error Reporting Configuration
    RwAlink (FCH_ABCFG_REGF0 | (UINT32) (ABCFG << 29), (UINT32)~(BIT1), 0, StdHeader);
  }

}

/**
 * FchGppRasInitialization - Initializing RAS
 *
 *
 * @param[in] FchGpp      Pointer to Fch GPP configuration structure
 * @param[in] StdHeader   Pointer to AMD_CONFIG_PARAMS
 *
 */
STATIC VOID
FchGppRasInitialization (
  IN       FCH_GPP             *FchGpp,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  if (FchGpp->PcieRas) {
    RwAlink (FCH_ABCFG_REGF4 | (UINT32) (ABCFG << 29), 0xFFFFFFFF, BIT0, StdHeader);
  }
}


/**
 * FchGppPortInit - GPP port training and initialization
 *
 *
 * @param[in] FchGpp      Pointer to Fch GPP configuration structure
 * @param[in] StdHeader   Pointer to AMD_CONFIG_PARAMS
 *
 */
VOID
FchGppPortInit (
  IN       FCH_GPP               *FchGpp,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  //
  // GppEarlyInit
  //
  UINT32             AbValue;
  UINT8              ResetCounter;
  UINT8              FailPorts;

  AGESA_TESTPOINT (TpFchGppBeforePortTraining, StdHeader);

  //
  // Configure NB-FCH link PCIE PHY PLL power down for L1
  //
  if ( FchGpp->UmiPhyPllPowerDown == TRUE ) {
    //
    // Set PCIE_P_CNTL in Alink PCIEIND space
    //
    WriteAlink (FCH_AX_INDXC_REG30 | (UINT32) (AXINDC << 29), 0x40, StdHeader);
    AbValue = ReadAlink (FCH_AX_DATAC_REG34 | (UINT32) (AXINDC << 29), StdHeader);
    AbValue |= BIT12 + BIT3 + BIT0;
    AbValue &= (UINT32)~(BIT9 + BIT4);
    WriteAlink (FCH_AX_DATAC_REG34 | (UINT32) (AXINDC << 29), AbValue, StdHeader);
    RwAlink (FCH_AX_INDXC_REG02 | (UINT32) (AXINDC << 29), (UINT32)~(BIT8), (BIT8), StdHeader);
    RwAlink (FCH_AX_INDXC_REG02 | (UINT32) (AXINDC << 29), (UINT32)~(BIT3), (BIT3), StdHeader);
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
  if ( FchGpp->GppLaneReversal == TRUE ) {
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
  if (FchGpp->GppFunctionEnable) {
    if (( FchGpp->NewGppAlgorithm == FALSE ) || ( (ReadFchSleepType (StdHeader) != ACPI_SLPTYP_S3) )) {
      ProgramGppTogglePcieReset (FchGpp->GppToggleReset, StdHeader);
    }
    FchGppAerInitialization (FchGpp, StdHeader);
    FchGppRasInitialization (FchGpp, StdHeader);

    //
    // PreInit - Enable GPP link training
    //
    if (( FchGpp->NewGppAlgorithm == FALSE ) || ( (ReadFchSleepType (StdHeader) != ACPI_SLPTYP_S3) )) {
      PreInitGppLink (FchGpp, StdHeader);
    }
    //
    // GPP Upstream Memory Write Arbitration Enhancement ABCFG 0x54[26] = 1
    // GPP Memory Write Max Payload Improvement RCINDC_Reg 0x10[12:10] = 0x4
    //
    if ( FchGpp->GppMemWrImprove == TRUE ) {
      RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), (UINT32)~BIT26, (BIT26), StdHeader);
      RwAlink (FCH_RCINDXC_REG10, (UINT32)~(BIT12 + BIT11 + BIT10), (BIT12), StdHeader);
    }

    if ( FchGpp->NewGppAlgorithm == TRUE ) {
      if (ReadFchSleepType (StdHeader) == ACPI_SLPTYP_S3) {
        if ( FchGpp->HotPlugPortsStatus == 0 ) {
        // S3 Procedure
          FchStall (5000, StdHeader);
          FailPorts = FchGpp->FailPortsStatus;
          if ( FchGpp->FailPortsStatus != 0 ) {
            AGESA_TESTPOINT (TpFchGppGen1PortPolling, StdHeader);
            FchGppForceGen1 (FchGpp, FailPorts, StdHeader);
          }
        }
      }
    } else {
      ResetCounter = 3;
      while (ResetCounter--) {
        FailPorts = CheckGppLinkStatus (FchGpp, StdHeader);
        if (FoundInfiniteCrs (FchGpp, StdHeader)) {
          ProgramGppTogglePcieReset (TRUE, StdHeader);
        } else if ((FailPorts != 0) && (ReadFchSleepType (StdHeader) != ACPI_SLPTYP_S3)) {
          ProgramGppTogglePcieReset (FchGpp->GppToggleReset, StdHeader);
        } else {
          break;
        }
      }
    }

    //
    // Misc operations after link training
    //
    if ( FchGpp->NewGppAlgorithm == FALSE ) {
      AfterGppLinkInit (FchGpp, StdHeader);
    }
  }
  if ( FchGpp->NewGppAlgorithm == FALSE ) {
    FchGppDynamicPowerSaving (FchGpp, StdHeader);
    AGESA_TESTPOINT (TpFchGppAfterPortTraining, StdHeader);
  }
}

/**
 * FchGppPortInitS3Phase - GPP port training and initialization S3 phase for new algorithm
 *
 *
 * @param[in] FchGpp      Pointer to Fch GPP configuration structure
 * @param[in] StdHeader   Pointer to AMD_CONFIG_PARAMS
 *
 */
VOID
FchGppPortInitS3Phase (
  IN       FCH_GPP               *FchGpp,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT8              ResetCounter;
  UINT8              FailPorts;

  if (FchGpp->GppFunctionEnable) {
    ProgramGppTogglePcieReset (FchGpp->GppToggleReset, StdHeader);
    PreInitGppLink (FchGpp, StdHeader);
    // For S3 With HotPlug port setting.
    if ( FchGpp->HotPlugPortsStatus != 0 ) {
      ResetCounter = 3;
      while (ResetCounter--) {
        FailPorts = CheckGppLinkStatus (FchGpp, StdHeader);
        if (FoundInfiniteCrs (FchGpp, StdHeader)) {
          ProgramGppTogglePcieReset (TRUE, StdHeader);
        } else if (FailPorts != 0) {
          ProgramGppTogglePcieReset (FchGpp->GppToggleReset, StdHeader);
        } else {
          break;
        }
      }
      AfterGppLinkInit (FchGpp, StdHeader);
    }
  }
}

/**
 * FchGppPortInitPhaseII - GPP port training and initialization phase II for new algorithm
 *
 *
 * @param[in] FchGpp      Pointer to Fch GPP configuration structure
 * @param[in] StdHeader   Pointer to AMD_CONFIG_PARAMS
 *
 */
VOID
FchGppPortInitPhaseII (
  IN       FCH_GPP               *FchGpp,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT8              ResetCounter;
  UINT8              FailPorts;
  UINT8              HotPlugPorts;

  if (FchGpp->GppFunctionEnable) {
    //
    // Check Link status for the new algorithm
    //
    HotPlugPorts = 0;
    FailPorts = 0;
    //
    // Read previously HotPlug port status
    //
    if ( ReadFchSleepType (StdHeader) == ACPI_SLPTYP_S3) {
      if ( FchGpp->HotPlugPortsStatus == 0 ) {
        FailPorts = FchGpp->FailPortsStatus;
        FailPorts = GppPortPollingLtssmS3 (FchGpp, FailPorts, FALSE, StdHeader);
        AfterGppLinkInit (FchGpp, StdHeader);
      }
    } else {
      ResetCounter = 3;
      while (ResetCounter--) {
        FailPorts = CheckGppLinkStatus (FchGpp, StdHeader);
        if (FoundInfiniteCrs (FchGpp, StdHeader)) {
          ProgramGppTogglePcieReset (TRUE, StdHeader);
        } else if ((FailPorts != 0) && (ReadFchSleepType (StdHeader) != ACPI_SLPTYP_S3)) {
          // CMOS record need
          FchGpp->FailPortsStatus = FailPorts;
          ProgramGppTogglePcieReset (FchGpp->GppToggleReset, StdHeader);
        } else {
          // CMOS clear need
          FchGpp->FailPortsStatus = FailPorts;
          break;
        }
      }
      AfterGppLinkInit (FchGpp, StdHeader);
    }
  }
  FchGppDynamicPowerSaving (FchGpp, StdHeader);
  AGESA_TESTPOINT (TpFchGppAfterPortTraining, StdHeader);
}
