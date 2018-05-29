/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH IO access common routine
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision$   @e \$Date$
 *
 */
 /*****************************************************************************
 *
 * Copyright (c) 2008 - 2016, Advanced Micro Devices, Inc.
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
 ***************************************************************************/

#include "FchPlatform.h"
#include  "cpuFamilyTranslation.h"
 /*
  * Headers removed for coreboot.
  * #include  "Porting.h"
  * #include  "AMD.h"
  * #include  "amdlib.h"
  * #include  "heapManager.h"
  */
#define FILECODE PROC_FCH_COMMON_FCHPELIB_FILECODE

#if IS_ENABLED(CONFIG_VENDORCODE_FULL_SUPPORT)
/*----------------------------------------------------------------------------------------*/
/**
 * ProgramPciByteTable - Program PCI register by table (8 bits data)
 *
 *
 *
 * @param[in] pPciByteTable    - Table data pointer
 * @param[in] dwTableSize      - Table length
 * @param[in] StdHeader
 *
 */
VOID
ProgramPciByteTable (
  IN       REG8_MASK           *pPciByteTable,
  IN       UINT16              dwTableSize,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8     i;
  UINT8     dbBusNo;
  UINT8     dbDevFnNo;
  UINT8     Or8;
  UINT8     Mask8;
  PCI_ADDR  PciAddress;

  dbBusNo = pPciByteTable->RegIndex;
  dbDevFnNo = pPciByteTable->AndMask;
  pPciByteTable++;

  for ( i = 1; i < dwTableSize; i++ ) {
    if ( (pPciByteTable->RegIndex == 0xFF) && (pPciByteTable->AndMask == 0xFF) && (pPciByteTable->OrMask == 0xFF) ) {
      pPciByteTable++;
      dbBusNo = pPciByteTable->RegIndex;
      dbDevFnNo = pPciByteTable->AndMask;
      pPciByteTable++;
      i++;
    } else {
      PciAddress.AddressValue = (dbBusNo << 20) + (dbDevFnNo << 12) + pPciByteTable->RegIndex;
      Or8 = pPciByteTable->OrMask;
      Mask8 = ~pPciByteTable->AndMask;
      LibAmdPciRMW (AccessWidth8, PciAddress, &Or8, &Mask8, StdHeader);
      pPciByteTable++;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * ProgramFchAcpiMmioTbl - Program FCH ACPI MMIO register by table (8 bits data)
 *
 *
 *
 * @param[in] pAcpiTbl   - Table data pointer
 * @param[in] StdHeader
 *
 */
VOID
ProgramFchAcpiMmioTbl (
  IN       ACPI_REG_WRITE      *pAcpiTbl,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8   i;
  UINT8   Or8;
  UINT8   Mask8;
  UINT32  ddtempVar;

  if (pAcpiTbl != NULL) {
    if ((pAcpiTbl->MmioReg == 0) && (pAcpiTbl->MmioBase == 0) && (pAcpiTbl->DataAndMask == 0xB0) && (pAcpiTbl->DataOrMask == 0xAC)) {
      // Signature Checking
      pAcpiTbl++;
      for ( i = 1; pAcpiTbl->MmioBase < 0x1D; i++ ) {
        ddtempVar = ACPI_MMIO_BASE | (pAcpiTbl->MmioBase) << 8 | pAcpiTbl->MmioReg;
        Or8 = pAcpiTbl->DataOrMask;
        Mask8 = ~pAcpiTbl->DataAndMask;
        LibAmdMemRMW (AccessWidth8, (UINT64) ddtempVar, &Or8, &Mask8, StdHeader);
        pAcpiTbl++;
      }
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * ProgramFchSciMapTbl - Program FCH SCI Map table (8 bits data)
 *
 *
 *
 * @param[in] pSciMapTbl   - Table data pointer
 * @param[in] FchResetDataBlock
 *
 */
VOID
ProgramFchSciMapTbl (
  IN       SCI_MAP_CONTROL  *pSciMapTbl,
  IN       FCH_RESET_DATA_BLOCK *FchResetDataBlock
  )
{
  AMD_CONFIG_PARAMS   *StdHeader;

  UINT32  ddtempVar;
  StdHeader = FchResetDataBlock->StdHeader;

  if (pSciMapTbl != NULL) {
    while (pSciMapTbl->InputPin != 0xFF) {
      if ((pSciMapTbl->InputPin >= 0x40) && (pSciMapTbl->InputPin < 0x80) && (pSciMapTbl->GpeMap < 0x20)) {
        ddtempVar = ACPI_MMIO_BASE | SMI_BASE | pSciMapTbl->InputPin;
        if (((pSciMapTbl->InputPin == FCH_SMI_xHC0Pme) && (FchResetDataBlock->FchReset.Xhci0Enable == 0)) || \
           ((pSciMapTbl->InputPin == FCH_SMI_xHC1Pme) && (FchResetDataBlock->FchReset.Xhci1Enable == 0))) {
        } else {
          LibAmdMemWrite (AccessWidth8, (UINT64) ddtempVar, &pSciMapTbl->GpeMap, StdHeader);
        }
      } else {
        //Assert Warning "SCI map is invalid"
      }
      pSciMapTbl++;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * ProgramFchGpioTbl - Program FCH Gpio table (8 bits data)
 *
 *
 *
 * @param[in] pGpioTbl   - Table data pointer
 *
 */
VOID
ProgramFchGpioTbl (
  IN       GPIO_CONTROL  *pGpioTbl
  )
{
  if (pGpioTbl != NULL) {
    while (pGpioTbl->GpioPin != 0xFF) {
      ACPIMMIO8 (ACPI_MMIO_BASE + IOMUX_BASE + pGpioTbl->GpioPin) = (UINT8) (pGpioTbl->PinFunction);
      ACPIMMIO8 (ACPI_MMIO_BASE + GPIO_BANK0_BASE + (pGpioTbl->GpioPin << 2) + 2) = (UINT8) (pGpioTbl->CfgByte);
      pGpioTbl++;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * ProgramSataPhyTbl - Program FCH Sata Phy table (8 bits data)
 *
 *
 *
 * @param[in] pSataPhyTbl   - Table data pointer
 * @param[in] FchResetDataBlock
 *
 */
VOID
ProgramFchSataPhyTbl (
  IN       SATA_PHY_CONTROL  *pSataPhyTbl,
  IN       FCH_RESET_DATA_BLOCK *FchResetDataBlock
  )
{
  if (pSataPhyTbl != NULL) {
    while (pSataPhyTbl->PhyData != 0xFFFFFFFF) {
      //to be implemented
      pSataPhyTbl++;
    }
  }
}
#endif /* IS_ENABLED(CONFIG_VENDORCODE_FULL_SUPPORT) */

/**
 * GetChipSysMode - Get Chip status
 *
 *
 * @param[in] Value - Return Chip strap status
 *   StrapStatus [15.0] - Hudson-2 chip Strap Status
 *    @li <b>0001</b> - Not USED FWH
 *    @li <b>0002</b> - Not USED LPC ROM
 *    @li <b>0004</b> - EC enabled
 *    @li <b>0008</b> - Reserved
 *    @li <b>0010</b> - Internal Clock mode
 * @param[in] StdHeader
 *
 */
VOID
GetChipSysMode (
  IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  LibAmdMemRead (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG80), Value, StdHeader);
}

/**
 * IsImcEnabled - Is IMC Enabled
 * @retval  TRUE for IMC Enabled; FALSE for IMC Disabled
 */
BOOLEAN
IsImcEnabled (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8   dbSysConfig;
  GetChipSysMode (&dbSysConfig, StdHeader);
  if (dbSysConfig & ChipSysEcEnable) {
    return TRUE;
  } else {
    return FALSE;
  }
}

#if IS_ENABLED(CONFIG_VENDORCODE_FULL_SUPPORT)
/**
 * GetEfuseStatue - Get Efuse status
 *
 *
 * @param[in] Value - Return Chip strap status
 * @param[in] StdHeader
 *
 */
VOID
GetEfuseStatus (
  IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8    Or8;
  UINT8    Mask8;

  Or8 = BIT5;
  Mask8 = BIT5;
  LibAmdMemRMW (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGC8), &Or8, &Mask8, StdHeader);
  LibAmdMemWrite (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGD8), Value, StdHeader);
  LibAmdMemRead (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGD8 + 1), Value, StdHeader);
  Or8 = 0;
  Mask8 = BIT5;
  LibAmdMemRMW (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGC8), &Or8, &Mask8, StdHeader);
}

/*----------------------------------------------------------------------------------------*/
/**
 * SbSleepTrapControl - SB Sleep Trap Control
 *
 *
 *
 * @param[in] SleepTrap    - Whether sleep trap is enabled
 *
 */
VOID
SbSleepTrapControl (
  IN        BOOLEAN          SleepTrap
  )
{
  if (SleepTrap) {
    ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0) &= ~(BIT2 + BIT3);
    ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0) |= BIT2;

    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGBE) &= ~ (BIT5);
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) &= ~ (BIT0 + BIT1);
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) |= BIT1;
  } else {
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGBE) |= BIT5;
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) &= ~ (BIT0 + BIT1);
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) |= BIT0;

    ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0) &= ~(BIT2 + BIT3);
  }
}

/**
 * FchUsb3D3ColdCallback - Fch Usb3 D3Cold Callback
 *
 *
 * @param[in] FchDataPtr
 *
 */
VOID
FchUsb3D3ColdCallback (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;
  UINT8 Value8;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  //FCH_DEADLOOP ();
  ACPIMMIO32 (FCH_AOACxA0_PwrGood_Control) |= FCH_AOACxA0_PwrGood_Control_SwUsb3SlpShutdown;
  do {
  } while ((ACPIMMIO32 (FCH_AOACxA0_PwrGood_Control) & FCH_AOACxA0_PwrGood_Control_SwUsb3SlpShutdown) == 0);

  ACPIMMIO32 (FCH_XHC_PMx00_Configure0) |= FCH_XHC_PMx00_Configure0_U3P_D3Cold_PWRDN;

  ACPIMMIO8 (FCH_AOACx6E_USB3_D3_CONTROL) &= ~ (AOAC_PWR_ON_DEV);
  do {
  } while ((ACPIMMIO8 (FCH_AOACx6F_USB3_D3_STATE) & 0x07) != 0);

  ACPIMMIO8 (FCH_AOACx6E_USB3_D3_CONTROL) |= 3;

  ACPIMMIO32 (FCH_MISCx28_ClkDrvStr2) |= FCH_MISCx28_ClkDrvStr2_USB3_RefClk_Pwdn;

  if ((ACPIMMIO8 (FCH_AOACx64_EHCI_D3_CONTROL) & 0x03) == 3) {
    ACPIMMIO32 (FCH_AOACxA0_PwrGood_Control) &= ~ (FCH_AOACxA0_PwrGood_Control_SwUsb2S5RstB + FCH_AOACxA0_PwrGood_Control_SwUsb3SlpShutdown);
    ACPIMMIO32 (FCH_MISCx28_ClkDrvStr2) |= FCH_MISCx28_ClkDrvStr2_USB2_RefClk_Pwdn;
  }

  ACPIMMIO32 (FCH_AOACxA0_PwrGood_Control) &= ~ (FCH_AOACxA0_PwrGood_Control_XhcPwrGood + FCH_AOACxA0_PwrGood_Control_SwUsb3SlpShutdown);
  Value8 = ACPIMMIO8 (0xFED803EE);
  Value8 &= 0xFC;
  Value8 |= 0x01;
  ACPIMMIO8 (0xFED803EE) = Value8;
}

/**
 * FchUsb3D0Callback - Fch Usb3 D0 Callback
 *
 *
 * @param[in] FchDataPtr
 *
 */
VOID
FchUsb3D0Callback (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;
  UINT32 Dword32;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  ACPIMMIO8 (0xFED803EE) &= 0xFC;

  ACPIMMIO8 (FCH_AOACxA0_PwrGood_Control) |= (FCH_AOACxA0_PwrGood_Control_XhcPwrGood);
  ACPIMMIO32 (FCH_MISCx28_ClkDrvStr2) &= ~ (FCH_MISCx28_ClkDrvStr2_USB2_RefClk_Pwdn);
  ACPIMMIO32 (FCH_MISCx28_ClkDrvStr2) &= ~ (FCH_MISCx28_ClkDrvStr2_USB3_RefClk_Pwdn);
  Dword32 = ACPIMMIO32 (FCH_AOACxA0_PwrGood_Control);
  Dword32 &= ~(FCH_AOACxA0_PwrGood_Control_SwUsb3SlpShutdown);
  ACPIMMIO32 (FCH_AOACxA0_PwrGood_Control) =  ((FCH_AOACxA0_PwrGood_Control_SwUsb2S5RstB | Dword32) & (~ BIT29));

  ACPIMMIO8 (FCH_AOACx6E_USB3_D3_CONTROL) &= 0xFC;
  ACPIMMIO8 (FCH_AOACx6E_USB3_D3_CONTROL) |= (AOAC_PWR_ON_DEV);
  do {
  } while ((ACPIMMIO8 (FCH_AOACx6F_USB3_D3_STATE) & 0x07) != 7);

  do {
  } while ((ACPIMMIO32 (FCH_XHC_PMx00_Configure0) & BIT7) != BIT7);

  ACPIMMIO32 (FCH_XHC_PMx00_Configure0) &= ~ (BIT16);

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function Checks Bristol or Stoney
 *
 *   NOTE:
 *
 * @param[in] StdHeader
 *
 */
BOOLEAN
FchCheckBR_ST (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  CPU_LOGICAL_ID LogicalId;

  // Only initialize on CZ processors, otherwise exit.
  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  if ((LogicalId.Revision & AMD_F15_BR_ALL) != 0) {
    return TRUE;
  }
  if ((LogicalId.Revision & AMD_F15_ST_ALL) != 0) {
    return TRUE;
  }

  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function Checks Bristol
 *
 *   NOTE:
 *
 * @param[in] StdHeader
 *
 */
BOOLEAN
FchCheckBR (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  CPU_LOGICAL_ID LogicalId;

  // Only initialize on CZ processors, otherwise exit.
  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  if ((LogicalId.Revision & AMD_F15_BR_ALL) != 0) {
    return TRUE;
  }

  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function Checks Stoney
 *
 *   NOTE:
 *
 * @param[in] StdHeader
 *
 */
BOOLEAN
FchCheckST (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  CPU_LOGICAL_ID LogicalId;

  // Only initialize on CZ processors, otherwise exit.
  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  if ((LogicalId.Revision & AMD_F15_ST_ALL) != 0) {
    return TRUE;
  }

  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function Checks Carrizo
 *
 *   NOTE:
 *
 * @param[in] StdHeader
 *
 */
BOOLEAN
FchCheckCZ (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  CPU_LOGICAL_ID LogicalId;

  // Only initialize on CZ processors, otherwise exit.
  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  if ((LogicalId.Revision & AMD_F15_CZ_ALL) != 0) {
    return TRUE;
  }

  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function Checks Package AM4
 *
 *   NOTE:
 *
 * @param[in] StdHeader
 *
 */
BOOLEAN
FchCheckPackageAM4 (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  CPUID_DATA CpuId;
  UINT8 RegValue;

  LibAmdCpuidRead (AMD_CPUID_FMF, &CpuId, StdHeader);
  RegValue = (UINT8) (CpuId.EBX_Reg >> 28) & 0xF; // bit 31:28
  ///@todo - update the PkgType once it is reflected in BKDG
  if (RegValue == 2) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function Get Scratch Fuse
 *
 *   NOTE:
 *
 * @param[in] StdHeader
 *
 */
UINT64
FchGetScratchFuse (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  PCI_ADDR D0F0xB8_SMU_Index_Address;
  PCI_ADDR D0F0xBC_SMU_Index_Data;
  UINT64  TempData64;
  UINT32  TempData32;

  D0F0xB8_SMU_Index_Address.AddressValue = (MAKE_SBDFO (0, 0, 0, 0, 0xB8));
  D0F0xBC_SMU_Index_Data.AddressValue = (MAKE_SBDFO (0, 0, 0, 0, 0xBC));
  TempData64 = 0;
  TempData32 = 0xC0016028;
  LibAmdPciWrite (AccessWidth32, D0F0xB8_SMU_Index_Address, &TempData32, StdHeader);
  LibAmdPciRead (AccessWidth32, D0F0xBC_SMU_Index_Data, &TempData32, StdHeader);
  TempData64 |= (((UINT64) TempData32) & 0xFFFFFFFF) >> 9;
  TempData32 = 0xC001602C;
  LibAmdPciWrite (AccessWidth32, D0F0xB8_SMU_Index_Address, &TempData32, StdHeader);
  LibAmdPciRead (AccessWidth32, D0F0xBC_SMU_Index_Data, &TempData32, StdHeader);
  TempData64 |= (((UINT64) TempData32) & 0xFFFFFFFF) << (32 - 9);
  TempData32 = 0xC0016030;
  LibAmdPciWrite (AccessWidth32, D0F0xB8_SMU_Index_Address, &TempData32, StdHeader);
  LibAmdPciRead (AccessWidth32, D0F0xBC_SMU_Index_Data, &TempData32, StdHeader);
  TempData64 |= (((UINT64) TempData32) & 0xFFFFFFFF) << (64 - 9);

  return TempData64;
}
#endif /* IS_ENABLED(CONFIG_VENDORCODE_FULL_SUPPORT) */
