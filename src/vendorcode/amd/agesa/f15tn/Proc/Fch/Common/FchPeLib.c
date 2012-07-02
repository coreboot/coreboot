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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*;********************************************************************************
;
; Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
;
; AMD is granting you permission to use this software (the Materials)
; pursuant to the terms and conditions of your Software License Agreement
; with AMD.  This header does *NOT* give you permission to use the Materials
; or any rights under AMD's intellectual property.  Your use of any portion
; of these Materials shall constitute your acceptance of those terms and
; conditions.  If you do not agree to the terms and conditions of the Software
; License Agreement, please do not use any portion of these Materials.
;
; CONFIDENTIALITY:  The Materials and all other information, identified as
; confidential and provided to you by AMD shall be kept confidential in
; accordance with the terms and conditions of the Software License Agreement.
;
; LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
; PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
; WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
; MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
; OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
; IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
; (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
; INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
; GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
; RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
; THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
; EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
; THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
;
; AMD does not assume any responsibility for any errors which may appear in
; the Materials or any other related information provided to you by AMD, or
; result from use of the Materials or any related information.
;
; You agree that you will not reverse engineer or decompile the Materials.
;
; NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
; further information, software, technical information, know-how, or show-how
; available to you.  Additionally, AMD retains the right to modify the
; Materials at any time, without notice, and is not obligated to provide such
; modified Materials to you.
;
; U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
; "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
; subject to the restrictions as set forth in FAR 52.227-14 and
; DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
; Government constitutes acknowledgement of AMD's proprietary rights in them.
;
; EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
; direct product thereof will be exported directly or indirectly, into any
; country prohibited by the United States Export Administration Act and the
; regulations thereunder, without the required authorization from the U.S.
; government nor will be used for any purpose prohibited by the same.
;*********************************************************************************/

#include "FchPlatform.h"
#define FILECODE PROC_FCH_COMMON_FCHPELIB_FILECODE

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
        if (((pSciMapTbl->InputPin == 0x78 ) && (FchResetDataBlock->FchReset.Xhci0Enable == 0)) || \
           ((pSciMapTbl->InputPin == 0x79 ) && (FchResetDataBlock->FchReset.Xhci1Enable == 0))) {
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
 * @param[in] FchResetDataBlock
 *
 */
VOID
ProgramFchGpioTbl (
  IN       GPIO_CONTROL  *pGpioTbl,
  IN       FCH_RESET_DATA_BLOCK *FchResetDataBlock
  )
{
  AMD_CONFIG_PARAMS   *StdHeader;

  UINT32  ddtempVar;
  StdHeader = FchResetDataBlock->StdHeader;

  if (pGpioTbl != NULL) {
    while (pGpioTbl->GpioPin != 0xFF) {
      ddtempVar = ACPI_MMIO_BASE | IOMUX_BASE | pGpioTbl->GpioPin;
      LibAmdMemWrite (AccessWidth8, (UINT64) ddtempVar, &pGpioTbl->PinFunction, StdHeader);
      ddtempVar = ACPI_MMIO_BASE | GPIO_BASE | pGpioTbl->GpioPin;
      LibAmdMemWrite (AccessWidth8, (UINT64) ddtempVar, &pGpioTbl->CfgByte, StdHeader);
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
  AMD_CONFIG_PARAMS   *StdHeader;

  //UINT32  ddtempVar;
  StdHeader = FchResetDataBlock->StdHeader;

  if (pSataPhyTbl != NULL) {
    while (pSataPhyTbl->PhyData != 0xFFFFFFFF) {
      //to be implemented
      pSataPhyTbl++;
    }
  }
}

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
  LibAmdMemWrite (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + PMIO_BASE + 0xD8 ), Value, StdHeader);
  LibAmdMemRead (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + PMIO_BASE + 0xD8  + 1), Value, StdHeader);
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

    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xBE ) &= ~ (BIT5);
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) &= ~ (BIT0 + BIT1);
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) |= BIT1;
  } else {
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xBE ) |= BIT5;
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) &= ~ (BIT0 + BIT1);
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) |= BIT0;

    ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0) &= ~(BIT2 + BIT3);
  }
}
