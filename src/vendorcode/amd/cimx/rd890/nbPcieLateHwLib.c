/**
 * @file
 *
 *  PCIe silicon specific functions library.
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
#include "amdDebugOutLib.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */



/*----------------------------------------------------------------------------------------*/
/**
 * Misc Initialization in late init
 *
 *
 *
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 */

VOID
PcieLibLateInit (
  IN      AMD_NB_CONFIG     *pConfig
  )
{
  CORE  CoreId;
  PORT  PortId;
  PCIE_CONFIG *pPcieConfig;
  PCI_ADDR ClkPciAddress;

  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  ClkPciAddress = pConfig->NbPciAddress;
  ClkPciAddress.Address.Function = 1;
  //Restore general setting in scratch
  LibNbEnableClkConfig (pConfig);
  LibNbPciRead (ClkPciAddress.AddressValue | NB_CLK_REG78, AccessWidth32, &pPcieConfig->PcieConfiguration, pConfig);
  LibNbDisableClkConfig (pConfig);
  // Restore Core setting from scratch
  for (CoreId = 0; CoreId <= MAX_CORE_ID; CoreId++) {
//    if (PcieLibIsCoreAccessible (CoreId, pConfig) && pPcieConfig->CoreSetting[CoreId].CoreDisabled != ON ) {
    UINT32  CoreAddress;
    CoreAddress = PcieLibGetCoreAddress (CoreId, pConfig);
    LibNbPciIndexRead (
      pConfig->NbPciAddress.AddressValue | NB_BIF_INDEX,
      NB_BIFNB_REG01 | CoreAddress,
      AccessWidth32,
      (UINT32*)&pPcieConfig->CoreSetting[CoreId],
      pConfig
      );
//  } else {
//    pPcieConfig->CoreSetting[CoreId].CoreDisabled = ON;
//  }
//    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "   Recover Core Setting CoreId %d Setting %x Enter\n", CoreId, (UINT32)(pPcieConfig->CoreSetting[CoreId])));
  }
  // Restore port Setting from scratch
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if (PcieLibIsValidPortId (PortId, pConfig)) {
      PCI_ADDR Port;
      Port = PcieLibGetPortPciAddress (PortId, pConfig);
      //Reload port configuration from scratch register
      LibNbPciIndexRead (
        Port.AddressValue | NB_BIF_INDEX,
        NB_BIFNBP_REG01,
        AccessWidth32,
        (UINT32*)&pPcieConfig->PortConfiguration[PortId],
        pConfig
        );
      LibNbPciRead (Port.AddressValue | NB_PCIP_REG108, AccessWidth32, (UINT32*)&pPcieConfig->ExtPortConfiguration[PortId], pConfig);
//      CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "   Recover Port setting PortId %d Setting %x Enter\n", PortId, (UINT32)(pPcieConfig->PortConfiguration[PortId])));
    } else {
      *((UINT32*)&pPcieConfig->ExtPortConfiguration[PortId]) = 0;
      *((UINT32*)&pPcieConfig->PortConfiguration[PortId]) = 0;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Misc Initialization in validate port state
 *
 *
 *
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 */

VOID
PcieLibValidatePortStateInit (
  IN      AMD_NB_CONFIG     *pConfig
  )
{
  CORE        CoreId;
  PORT        PortId;
  UINT32      PortAlwaysVisible;
  UINT32      ForcePortDisable;
  PCIE_CONFIG *pPcieConfig;
  PCI_ADDR    ClkPciAddress;

  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  ClkPciAddress = pConfig->NbPciAddress;
  ClkPciAddress.Address.Function = 1;
  //Restore general setting in scratch
  LibNbEnableClkConfig (pConfig);
  LibNbPciRead (ClkPciAddress.AddressValue | NB_CLK_REG78, AccessWidth32, &pPcieConfig->PcieConfiguration, pConfig);
  LibNbDisableClkConfig (pConfig);
  // Restore Core setting from scratch
  for (CoreId = 0; CoreId <= MAX_CORE_ID; CoreId++) {
//    if (PcieLibIsCoreAccessible (CoreId, pConfig) && pPcieConfig->CoreSetting[CoreId].CoreDisabled != ON ) {
    UINT32  CoreAddress;
    CoreAddress = PcieLibGetCoreAddress (CoreId, pConfig);
    LibNbPciIndexRead (
      pConfig->NbPciAddress.AddressValue | NB_BIF_INDEX,
      NB_BIFNB_REG01 | CoreAddress,
      AccessWidth32,
      (UINT32*)&pPcieConfig->CoreSetting[CoreId],
      pConfig
      );
//  } else {
//    pPcieConfig->CoreSetting[CoreId].CoreDisabled = ON;
//  }
//    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "   Recover Core Setting CoreId %d Setting %x Enter\n", CoreId, (UINT32)(pPcieConfig->CoreSetting[CoreId])));
  }
  // Restore port Setting from scratch
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if (PcieLibIsValidPortId (PortId, pConfig)) {
      PCI_ADDR Port;
      Port = PcieLibGetPortPciAddress (PortId, pConfig);
      PortAlwaysVisible = pPcieConfig->PortConfiguration[PortId].PortAlwaysVisible;
      ForcePortDisable = pPcieConfig->PortConfiguration[PortId].ForcePortDisable;
      //Reload port configuration from scratch register
      LibNbPciIndexRead (
        Port.AddressValue | NB_BIF_INDEX,
        NB_BIFNBP_REG01,
        AccessWidth32,
        (UINT32*)&pPcieConfig->PortConfiguration[PortId],
        pConfig
        );
      pPcieConfig->PortConfiguration[PortId].PortAlwaysVisible = PortAlwaysVisible;
      pPcieConfig->PortConfiguration[PortId].ForcePortDisable = ForcePortDisable;
      LibNbPciRead (Port.AddressValue | NB_PCIP_REG108, AccessWidth32, (UINT32*)&pPcieConfig->ExtPortConfiguration[PortId], pConfig);
//      CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "   Recover Port setting PortId %d Setting %x Enter\n", PortId, (UINT32)(pPcieConfig->PortConfiguration[PortId])));
    } else {
      *((UINT32*)&pPcieConfig->ExtPortConfiguration[PortId]) = 0;
      *((UINT32*)&pPcieConfig->PortConfiguration[PortId]) = 0;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Enable LCLK clock gating or shutdown LCLK clock banch if possible
 *
 *
 *
 * @param[in] CoreId    PCI Express Core ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 *
 */
VOID
PcieLibManageLclkClock (
  IN      CORE            CoreId,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  UINT32      Value;
  UINT32      Mask;
  PCI_ADDR    ClkPciAddress;
  UINT32      CoreAddress;
  PCIE_CONFIG *pPcieConfig;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLibManageLclkClock [CoreId %d] Enter \n", CoreId));
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  CoreAddress = PcieLibGetCoreAddress (CoreId, pConfig);
  ClkPciAddress = pConfig->NbPciAddress;
  ClkPciAddress.Address.Function = 1;
  LibNbEnableClkConfig (pConfig);

  if (pPcieConfig->CoreSetting[CoreId].LclkClockGating == ON) {
    ClkPciAddress.Address.Register = NB_CLK_REGE8;
    Value = 0;
    Mask = 0;
    switch (CoreAddress) {
    case  GPP1_CORE:
      ClkPciAddress.Address.Register = NB_CLK_REG94;
      Mask = BIT16;
      break;
    case  GPP2_CORE:
      Value = BIT28;
      break;
    case  GPP3a_CORE:
      Value = BIT31;
      break;
    case  GPP3b_CORE:
      Value = BIT25;
      break;
    case  SB_CORE:
      ClkPciAddress.Address.Register = NB_CLK_REG94;
      Mask = BIT24;
      break;
    default:
      CIMX_ASSERT (FALSE);
    }
    LibNbPciRMW (ClkPciAddress.AddressValue, AccessS3SaveWidth32, ~Mask, Value, pConfig);
  }
  if (pPcieConfig->CoreSetting[CoreId].LclkClockOff == ON) {
    UINT8   ActiveCoreMap;
    ActiveCoreMap = PcieLibGetActiveCoreMap (pConfig);
    if ((ActiveCoreMap & (1 << CoreId)) == 0) {
      //Core not active we can shutdown LCLK permanantly
      CORE_INFO *pCoreInfo;
      CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    Shutdown LCKL clock\n"));
      pCoreInfo = PcieLibGetCoreInfo (CoreId, pConfig);
      ClkPciAddress.Address.Register = NB_CLK_REGE0;
      pPcieConfig->CoreSetting[CoreId].CoreDisableStatus = ON;
      // We have to setup Index for BIFNB to point out to SB core. After this point core registers no longer accesasable
      LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_BIF_INDEX, 0x00 | SB_CORE, AccessS3SaveWidth32, 0xffffffff, 0x00, pConfig);
      LibNbPciRMW (ClkPciAddress.AddressValue, AccessS3SaveWidth32, 0xffffffff, 1 << pCoreInfo->LclkOffOffset, pConfig);

      Value = 0;
      if (CoreAddress == GPP1_CORE) {
        if ((ActiveCoreMap & 0xb) == 0 && !LibNbIsIommuEnabled (pConfig)) {
        // Can shutdown master core
          Value = 1 << pCoreInfo->LclkPermOffOffset;
        }
      } else {
        Value = 1 << pCoreInfo->LclkPermOffOffset;
      }
      if (Value != 0) {
        NbIommuDisconnectPcieCore (CoreId, pConfig);
        LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG27, AccessS3SaveWidth32, 0xffffffff, Value, pConfig);

      }
    }
  }
  LibNbDisableClkConfig (pConfig);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Power Off Pll for unused lanes.
 *
 *
 *
 * @param[in] CoreId    PCI Express Core ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
VOID
PcieLibPowerOffPll (
  IN      CORE            CoreId,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  PCIE_CONFIG *pPcieConfig;
  UINT32      CoreAddress;
  UINT32      PowerOfPllValue;
  UINT32      PadsMap;
  //UINT32      TxClockOffValue;
  UINT32      PowerOfPllRegister;

  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  CoreAddress = PcieLibGetCoreAddress (CoreId, pConfig);
  PowerOfPllValue = 0;
  PadsMap = 0;
  //TxClockOffValue = 0;
  PowerOfPllRegister = NB_MISC_REG23;

  LibNbPciIndexRead (pConfig->NbPciAddress.AddressValue | NB_BIF_INDEX, NB_BIFNB_REG65 | CoreAddress, AccessS3SaveWidth32, &PadsMap, pConfig);
  if (CoreAddress == GPP1_CORE || CoreAddress == GPP2_CORE) {
    if ((PadsMap & 0xf0) == 0xf0) {
      //Power Off PLL1
      PowerOfPllValue |= (BIT1 | BIT3);
      if ((PadsMap & 0x0f) == 0x0f && pPcieConfig->CoreConfiguration[CoreId] != GFX_CONFIG_AABB) {
        //Power Off PLL0
        PowerOfPllValue |= (BIT0 | BIT2);
      }
    }
    if (CoreAddress == GPP2_CORE) {
      PowerOfPllValue <<= 8;
      //TxClockOffValue = BIT1;
    } else {
      //TxClockOffValue = BIT0;
    }
    if ((UINT16)PadsMap != 0xffff) {
      //TxClockOffValue = 0;          //Do not disable TX clock in case any line is ON
    }
  }
  if (CoreAddress == GPP3a_CORE ) {
    if ((UINT16)PadsMap == 0x3F3F) {
      PowerOfPllValue = BIT18 | BIT16;
      //TxClockOffValue = BIT2;
    }
  }
  if (CoreAddress == GPP3b_CORE ) {
    PowerOfPllRegister = NB_MISC_REG2E;
    if ((UINT16)PadsMap == 0x0F0F) {
      PowerOfPllValue = BIT8 | BIT6;
      //TxClockOffValue = BIT3;
    }
  }
  //Power Off Pll
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, PowerOfPllRegister , AccessS3SaveWidth32, 0xffffffff, PowerOfPllValue, pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    Power off PLL CoreId %d, Value 0x%x\n", CoreId, PowerOfPllValue));
  //Turn off TXCLK
  //LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG07, AccessS3SaveWidth32, 0xffffffff, TxClockOffValue, pConfig);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Enable TX clock gating or shutdown TX clock if possible
 *
 *
 *
 * @param[in] CoreId    PCI Express Core ID
 * @param[in] pConfig   Northbridge configuration structure pointer.  *
 */
VOID
PcieLibManageTxClock (
  IN      CORE            CoreId,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  PCIE_CONFIG *pPcieConfig;
  UINT32      CoreAddress;
  UINT32      Value;
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  CoreAddress = PcieLibGetCoreAddress (CoreId, pConfig);
  if (pPcieConfig->CoreSetting[CoreId].TxClockGating == ON) {
    switch (CoreAddress) {
    case  GPP1_CORE:
      Value = BIT4;
      break;
    case  GPP2_CORE:
      Value = BIT5;
      break;
    case  GPP3a_CORE:
      Value = BIT6;
      break;
    case  GPP3b_CORE:
      Value = BIT24;
      break;
    case  SB_CORE:
      Value = BIT7;
      break;
    default:
      Value = 0;
      CIMX_ASSERT (FALSE);
    }
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG07, AccessS3SaveWidth32, 0xffffffff, Value, pConfig);
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_BIF_INDEX, NB_BIFNB_REG40 | CoreAddress, AccessS3SaveWidth32, (UINT32)~BIT6, BIT6, pConfig);
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_BIF_INDEX, NB_BIFNB_REG11 | CoreAddress, AccessS3SaveWidth32, 0xfffffff0, 0x0C, pConfig);
  }
  if (pPcieConfig->CoreSetting[CoreId].TxClockOff == ON) {
    UINT8   ActiveCoreMap;
    ActiveCoreMap = PcieLibGetActiveCoreMap (pConfig);
    if ((ActiveCoreMap & (1 << CoreId)) == 0) {
      //Core not active we can shutdown TX clk permanantly
      CORE_INFO *pCoreInfo;
      CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    Shutdown TX clock\n"));
      pPcieConfig->CoreSetting[CoreId].CoreDisableStatus = ON;
      pCoreInfo = PcieLibGetCoreInfo (CoreId, pConfig);
      LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG07, AccessS3SaveWidth32, 0xffffffff, 1 << pCoreInfo->TxOffOffset, pConfig);
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Enable Pll Power Down in L1.
 *
 *
 *
 * @param[in] CoreId    PCI Express Core ID
 * @param[in] pConfig   Northbridge configuration structure pointer.  *
 */
VOID
PcieLibEnablePllPowerOffInL1 (
  IN      CORE            CoreId,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  PCIE_CONFIG *pPcieConfig;
  UINT32      Value;
  UINT32      CoreAddress;
  PORT        PortId;
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  if (pPcieConfig->CoreSetting[CoreId].DetectPowerOffPllInL1 == ON && !PciePllOffComatibilityTest (CoreId, pConfig)) {
    return;
  }
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if (PcieLibIsValidPortId (PortId, pConfig) && PcieLibGetCoreId (PortId, pConfig) == CoreId) {
      if (pPcieConfig->PortConfiguration[PortId].PortHotplug != OFF) {
        // set up max exit latency requirment for hotplug ports
        PCI_ADDR  Port;
        Port = PcieLibGetPortPciAddress (PortId, pConfig);
        LibNbPciIndexRMW (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REGC1 , AccessS3SaveWidth32, 0xffffffff, 0xf, pConfig);
      }
    }
  }
  CoreAddress = PcieLibGetCoreAddress (CoreId, pConfig);
  Value = BIT8;
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_BIF_INDEX, NB_BIFNB_REG40 | CoreAddress, AccessS3SaveWidth32, (UINT32)~(BIT9 + BIT4), BIT3 + BIT0 + BIT12, pConfig);
  if (CoreAddress == GPP3b_CORE || CoreAddress == GPP3a_CORE || CoreAddress == SB_CORE) {
    Value |= BIT3;
  }
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_BIF_INDEX, NB_BIFNB_REG02 | CoreAddress, AccessS3SaveWidth32, 0xffffffff, Value, pConfig);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Misc. core setting.
 *
 *
 *
 * @param[in] CoreId    PCI Express- Core ID
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */
VOID
PcieLibMiscLateCoreSetting (
  IN       CORE  CoreId,
  IN       AMD_NB_CONFIG   *pConfig
  )
{
//Lock
  LibNbPciIndexRMW (
    pConfig->NbPciAddress.AddressValue | NB_BIF_INDEX,
    NB_BIFNB_REG10 | PcieLibGetCoreAddress (CoreId, pConfig),
    AccessS3SaveWidth32,
    0xffffffff,
    BIT0,
    pConfig
    );
}
