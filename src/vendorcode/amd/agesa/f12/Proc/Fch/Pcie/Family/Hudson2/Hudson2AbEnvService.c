/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Hudson2 AB
 *
 * Init AB bridge.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 48048 $   @e \$Date: 2011-03-03 10:13:06 +0800 (Thu, 03 Mar 2011) $
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
#include "Filecode.h"
#define FILECODE PROC_FCH_PCIE_FAMILY_HUDSON2_HUDSON2ABENVSERVICE_FILECODE

//
// Declaration of local functions
//
VOID AbCfgTbl (IN AB_TBL_ENTRY  *ABTbl, IN AMD_CONFIG_PARAMS *StdHeader);

/**
 * Hudson2PcieOrderRule - AB-Link Configuration Table for ablink
 * Post Pass Np Downstream/Upstream Feature
 *
 */
AB_TBL_ENTRY Hudson2PcieOrderRule[] =
{
  //
  // abPostPassNpDownStreamTbl
  //
  {ABCFG, FCH_ABCFG_REG10060, BIT31, BIT31},
  {ABCFG, FCH_ABCFG_REG1009C, BIT4 + BIT5, BIT4 + BIT5},
  {ABCFG, FCH_ABCFG_REG9C, BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7, BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7},
  {ABCFG, FCH_ABCFG_REG90, BIT21 + BIT22 + BIT23, BIT21 + BIT22 + BIT23},
  {ABCFG, FCH_ABCFG_REGF0, BIT6 + BIT5, BIT6 + BIT5},
  {AXINDC, FCH_AX_INDXC_REG02, BIT9, BIT9},
  {ABCFG, FCH_ABCFG_REG10090, BIT9 + BIT10 + BIT11 + BIT12, BIT9 + BIT10 + BIT11 + BIT12},

  //
  // abPostPassNpUpStreamTbl
  //
  {ABCFG, FCH_ABCFG_REG58, BIT10, BIT10},
  {ABCFG, FCH_ABCFG_REGF0, BIT3 + BIT4, BIT3 + BIT4},
  {ABCFG, FCH_ABCFG_REG54, BIT1, BIT1},
  { (UINT8)0xFF, (UINT8)0xFF, (UINT8)0xFF, (UINT8)0xFF},
};

/**
 * Hudson2InitEnvAbTable - AB-Link Configuration Table for Hudson2
 *
 */
AB_TBL_ENTRY Hudson2InitEnvAbTable[] =
{
  //
  // Enable downstream posted transactions to pass non-posted transactions.
  //
  {ABCFG, FCH_ABCFG_REG10090, BIT8 + BIT16, BIT8 + BIT16},

  //
  // Enable Hudson-2 to issue memory read/write requests in the upstream direction.
  //
  {AXCFG, FCH_AB_REG04, BIT2, BIT2},

  //
  // Enabling IDE/PCIB Prefetch for Performance Enhancement
  // PCIB prefetch   ABCFG 0x10060 [20] = 1   ABCFG 0x10064 [20] = 1
  //
  {ABCFG, FCH_ABCFG_REG10060, BIT20, BIT20},                                    ///  PCIB prefetch enable
  {ABCFG, FCH_ABCFG_REG10064, BIT20, BIT20},                                    ///  PCIB prefetch enable

  //
  // Controls the USB OHCI controller prefetch used for enhancing performance of ISO out devices.
  // Setting B-Link Prefetch Mode (ABCFG 0x80 [18:17] = 11)
  //
  {ABCFG, FCH_ABCFG_REG80, BIT0 + BIT17 + BIT18, BIT0 + BIT17 + BIT18},

  //
  // Enabled SMI ordering enhancement. ABCFG 0x90[21]
  // USB Delay A-Link Express L1 State. ABCFG 0x90[17]
  //
  {ABCFG, FCH_ABCFG_REG90, BIT21 + BIT17, BIT21 + BIT17},

  //
  // Disable the credit variable in the downstream arbitration equation
  // Register bit to qualify additional address bits into downstream register programming. (A12 BIT1 default is set)
  //
  {ABCFG, FCH_ABCFG_REG9C, BIT0, BIT0},

  //
  // Enabling Detection of Upstream Interrupts ABCFG 0x94 [20] = 1
  // ABCFG 0x94 [19:0] = cpu interrupt delivery address [39:20]
  //
  {ABCFG, FCH_ABCFG_REG94, BIT20, BIT20 + 0x00FEE},

  //
  // Programming cycle delay for AB and BIF clock gating
  // Enable the AB and BIF clock-gating logic.
  // Enable the A-Link int_arbiter enhancement to allow the A-Link bandwidth to be used more efficiently
  // Enable the requester ID for upstream traffic. [16]: SB/NB link [17]: GPP
  //
  {ABCFG, FCH_ABCFG_REG10054,  0x00FFFFFF, 0x010407FF},
  {ABCFG, FCH_ABCFG_REG98,  0xFFFC00FF, 0x00034700},
  {ABCFG, FCH_ABCFG_REG54,  0x00FF0000, 0x00040000},

  //
  // Non-Posted Memory Write Support
  //
  {AXINDC, FCH_AX_INDXC_REG10, BIT9, BIT9},

  //
  // UMI L1 Configuration
  //Step 1: AXINDC_Reg 0x02[0] = 0x1 Set REGS_DLP_IGNORE_IN_L1_EN to ignore DLLPs during L1 so that txclk can be turned off.
  //Step 2: AXINDP_Reg 0x02[15] = 0x1 Sets REGS_LC_ALLOW_TX_L1_CONTROL to allow TX to prevent LC from going to L1 when there are outstanding completions.
  //
  {AXINDC, FCH_AX_INDXC_REG02, BIT0, BIT0},
  {AXINDP, FCH_AX_INDXP_REG02, BIT15, BIT15},
  {ABCFG, 0, 0, (UINT8) 0xFF},                                                 /// This dummy entry is to clear ab index
  { (UINT8)0xFF, (UINT8)0xFF, (UINT8)0xFF, (UINT8)0xFF},
};

/**
 * FchInitEnvAbLinkInit - Set ABCFG registers before PCI
 * emulation.
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvAbLinkInit (
  IN  VOID     *FchDataPtr
  )
{
  UINT32                 AbValue;
  UINT16                 AbTempVar;
  UINT8                  AbValue8;
  UINT8                  FchALinkClkGateOff;
  UINT8                  FchBLinkClkGateOff;
  UINT32                 FchResetCpuOnSyncFlood;
  AB_TBL_ENTRY           *AbTblPtr;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  FchALinkClkGateOff = (UINT8) LocalCfgPtr->Ab.ALinkClkGateOff;
  FchBLinkClkGateOff = (UINT8) LocalCfgPtr->Ab.BLinkClkGateOff;
  //
  // AB CFG programming
  //
  if ( LocalCfgPtr->Ab.SlowSpeedAbLinkClock ) {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG40, AccessWidth8, ~BIT1, BIT1);
  } else {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG40, AccessWidth8, ~BIT1, 0);
  }

  //
  // Read Arbiter address, Arbiter address is in PMIO 6Ch
  //
  ReadMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6C, AccessWidth16, &AbTempVar);
  /// Write 0 to enable the arbiter
  AbValue8 = 0;
  LibAmdIoWrite (AccessWidth8, AbTempVar, &AbValue8, StdHeader);


  FchResetCpuOnSyncFlood = LocalCfgPtr->Ab.ResetCpuOnSyncFlood;

  if ( LocalCfgPtr->Ab.PcieOrderRule == 1 ) {
    AbTblPtr = (AB_TBL_ENTRY *) (&Hudson2PcieOrderRule[0]);
    AbCfgTbl (AbTblPtr, StdHeader);
  }

  if ( LocalCfgPtr->Ab.PcieOrderRule == 2 ) {
    RwAlink (FCH_ABCFG_REG10090 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x7 << 10), (UINT32) (0x7 << 10), StdHeader);
    RwAlink (FCH_ABCFG_REG58 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1F << 11), (UINT32) (0x1C << 11), StdHeader);
    RwAlink (FCH_ABCFG_REGB4 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x3 << 0), (UINT32) (0x3 << 0), StdHeader);
  }

  AbTblPtr = (AB_TBL_ENTRY *) (&Hudson2InitEnvAbTable[0]);
  AbCfgTbl (AbTblPtr, StdHeader);

  if ( FchResetCpuOnSyncFlood ) {
    RwAlink (FCH_ABCFG_REG10050 | (UINT32) (ABCFG << 29), ~BIT2, BIT2, StdHeader);
  }

  if ( LocalCfgPtr->Ab.AbClockGating ) {
    RwAlink (FCH_ABCFG_REG10054 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xFF << 16), (UINT32) (0x4 << 16), StdHeader);
    RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xFF << 16), (UINT32) (0x4 << 16), StdHeader);
    RwAlink (FCH_ABCFG_REG10054 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 24), (UINT32) (0x1 << 24), StdHeader);
    RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 24), (UINT32) (0x1 << 24), StdHeader);
  } else {
    RwAlink (FCH_ABCFG_REG10054 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 24), (UINT32) (0x0 << 24), StdHeader);
    RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 24), (UINT32) (0x0 << 24), StdHeader);
  }


  if ( LocalCfgPtr->Ab.GppClockGating ) {
    RwAlink (FCH_ABCFG_REG98 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xF << 12), (UINT32) (0x4 << 12), StdHeader);
    RwAlink (FCH_ABCFG_REG98 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xF << 8), (UINT32) (0x7 << 8), StdHeader);
    RwAlink (FCH_ABCFG_REG90 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 0), (UINT32) (0x1 << 0), StdHeader);
  } else {
    RwAlink (FCH_ABCFG_REG98 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xF << 8), (UINT32) (0x0 << 8), StdHeader);
    RwAlink (FCH_ABCFG_REG90 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 0), (UINT32) (0x0 << 0), StdHeader);
  }

  if ( LocalCfgPtr->Ab.UmiL1TimerOverride ) {
    RwAlink (FCH_ABCFG_REG90 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x7 << 12), (UINT32) (LocalCfgPtr->Ab.UmiL1TimerOverride  << 12), StdHeader);
    RwAlink (FCH_ABCFG_REG90 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 15), (UINT32) (0x1  << 15), StdHeader);
  }

  if ( LocalCfgPtr->Ab.UmiLinkWidth ) {
//    RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xFF << 16), (UINT32) (0x4 << 16));
  }

  if ( LocalCfgPtr->Ab.UmiDynamicSpeedChange ) {
    RwAlink ((UINT32) FCH_AX_INDXP_REGA4, ~ (UINT32) (0x1 << 0), (UINT32) (0x1 << 0), StdHeader);
    RwAlink ((UINT32) FCH_AX_CFG_REG88, ~ (UINT32) (0xF << 0), (UINT32) (0x2 << 0), StdHeader);
    RwAlink ((UINT32) FCH_AX_INDXP_REGA4, ~ (UINT32) (0x1 << 18), (UINT32) (0x1 << 18), StdHeader);
  }

  if ( LocalCfgPtr->Ab.PcieRefClockOverClocking ) {
//    RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xFF << 16), (UINT32) (0x4 << 16));
  }

  if ( LocalCfgPtr->Ab.UmiGppTxDriverStrength  ) {
    RwAlink (FCH_ABCFG_REGA8 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x3 << 18), (UINT32) ((LocalCfgPtr->Ab.UmiGppTxDriverStrength - 1) << 18), StdHeader);
    RwAlink (FCH_ABCFG_REGA0 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 8), (UINT32) (0x1 << 8), StdHeader);
  }

  if ( LocalCfgPtr->Gpp.PcieAer ) {
//    RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xFF << 16), (UINT32) (0x4 << 16));
  }

  if ( LocalCfgPtr->Gpp.PcieRas ) {
//    RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xFF << 16), (UINT32) (0x4 << 16));
  }

  //
  // Ab Bridge MSI
  //
  if ( LocalCfgPtr->Ab.AbMsiEnable) {
    AbValue = ReadAlink (FCH_ABCFG_REG94 | (UINT32) (ABCFG << 29), StdHeader);
    AbValue = AbValue | BIT20;
    WriteAlink (FCH_ABCFG_REG94 | (UINT32) (ABCFG << 29), AbValue, StdHeader);
  }

  //
  // A/B Clock Gate-OFF
  //
  if ( FchALinkClkGateOff ) {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x2E, AccessWidth8, 0xFE, BIT0);
  } else {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x2E, AccessWidth8, 0xFE, 0x00);
  }

  if ( FchBLinkClkGateOff ) {
    //RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x2D, AccessWidth8, 0xEF, 0x10);      /// A11 Only
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x2E, AccessWidth8, 0xFD, BIT1);
  } else {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x2E, AccessWidth8, 0xFD, 0x00);
  }
}

/**
 * AbCfgTbl - Program ABCFG by input table.
 *
 *
 * @param[in] ABTbl  ABCFG config table.
 * @param[in] StdHeader
 *
 */
VOID
AbCfgTbl (
  IN  AB_TBL_ENTRY     *ABTbl,
  IN AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32   AbValue;

  while ( (ABTbl->RegType) != 0xFF ) {
    if ( ABTbl->RegType == AXINDC ) {
      AbValue = 0x30 | (ABTbl->RegType << 29);
      WriteAlink (AbValue, (ABTbl->RegIndex & 0x00FFFFFF), StdHeader);
      AbValue = 0x34 | (ABTbl->RegType << 29);
      WriteAlink (AbValue, ((ReadAlink (AbValue, StdHeader)) & (0xFFFFFFFF^ (ABTbl->RegMask))) | ABTbl->RegData, StdHeader);
    } else if ( ABTbl->RegType == AXINDP ) {
      AbValue = 0x38 | (ABTbl->RegType << 29);
      WriteAlink (AbValue, (ABTbl->RegIndex & 0x00FFFFFF), StdHeader);
      AbValue = 0x3C | (ABTbl->RegType << 29);
      WriteAlink (AbValue, ((ReadAlink (AbValue, StdHeader)) & (0xFFFFFFFF^ (ABTbl->RegMask))) | ABTbl->RegData, StdHeader);
    } else {
      AbValue = ABTbl->RegIndex | (ABTbl->RegType << 29);
      WriteAlink (AbValue, ((ReadAlink (AbValue, StdHeader)) & (0xFFFFFFFF^ (ABTbl->RegMask))) | ABTbl->RegData, StdHeader);
    }

    ++ABTbl;
  }

  //
  //Clear ALink Access Index
  //
  AbValue = 0;
  LibAmdIoWrite (AccessWidth32, ALINK_ACCESS_INDEX, &AbValue, StdHeader);
}

/**
 * Is UMI One Lane GEN1 Mode?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
BOOLEAN
IsUmiOneLaneGen1Mode (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32   AbValue;

  AbValue = ReadAlink ((UINT32) (FCH_AX_CFG_REG68), StdHeader);
  AbValue >>= 16;
  if (((AbValue & 0x0f) == 1) && ((AbValue & 0x03f0) == 0x0010)) {
    return (TRUE);
  } else {
    return (FALSE);
  }
}
