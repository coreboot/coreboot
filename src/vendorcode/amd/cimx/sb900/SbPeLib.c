/**
 * @file
 *
 * Southbridge IO access common routine
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-SB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*;********************************************************************************
;
; Copyright (c) 2011, Advanced Micro Devices, Inc.
; All rights reserved.
; 
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
;     * Neither the name of Advanced Micro Devices, Inc. nor the names of 
;       its contributors may be used to endorse or promote products derived 
;       from this software without specific prior written permission.
; 
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; 
;*********************************************************************************/

#include "SbPlatform.h"
#include "cbtypes.h"
#include "AmdSbLib.h"

/**
 * Read Southbridge Revision ID cie Base
 *
 *
 * @retval  0xXXXXXXXX   Revision ID
 *
 */
UINT8
getRevisionID (
  OUT VOID
  )
{
  UINT8  dbVar0;
  ReadPCI (((SMBUS_BUS_DEV_FUN << 16) + SB_CFG_REG08), AccWidthUint8, &dbVar0);
  return dbVar0;
}

/**
 * Is SB A11?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
BOOLEAN
IsSbA11 (
  OUT VOID
  )
{
  return ( getRevisionID () == AMD_SB_A11 );
}

/**
 * Is SB A12?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
BOOLEAN
IsSbA12 (
  OUT VOID
  )
{
  return ( getRevisionID () == AMD_SB_A12 );
}

/**
 * Is SB A12 Plus?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
BOOLEAN
IsSbA12Plus (
  OUT VOID
  )
{
  return ( getRevisionID () >= AMD_SB_A12 );
}

/**
 * Is SB A13 Plus?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
BOOLEAN
IsSbA13Plus (
  OUT VOID
  )
{
  return ( getRevisionID () >= AMD_SB_A13 );
}

/**
 * Is External Clock Mode?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
BOOLEAN
IsExternalClockMode (
  OUT VOID
  )
{
  return ( (BOOLEAN) ((ACPIMMIO32 (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG80) & BIT4) == 0) );
}

/**
 * Is LPC Rom?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
BOOLEAN
IsLpcRom (
  OUT VOID
  )
{
  return ( (BOOLEAN) ((ACPIMMIO32 (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG80) & BIT1) == 0) );
}

/**
 * Memory Copy
 *
 *
 * @retval  VOID
 *
 */
VOID
MemoryCopy (
  IN       UINT8 *Dest,
  IN       UINT8 *Source,
  IN       UINTN Size
  )
{
  UINTN i;
  for ( i = 0; i < Size; i++  ) {
    *Dest = *Source;
    Dest++;
    Source++;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * programPciByteTable - Program PCI register by table (8 bits data)
 *
 *
 *
 * @param[in] pPciByteTable    - Table data pointer
 * @param[in] dwTableSize      - Table length
 *
 */
VOID
programPciByteTable (
  IN       REG8MASK* pPciByteTable,
  IN       UINT16 dwTableSize
  )
{
  UINT8  i;
  UINT8  dbBusNo;
  UINT8  dbDevFnNo;
  UINT32  ddBDFR;

  dbBusNo = pPciByteTable->bRegIndex;
  dbDevFnNo = pPciByteTable->bANDMask;
  pPciByteTable++;

  for ( i = 1; i < dwTableSize; i++ ) {
    if ( (pPciByteTable->bRegIndex == 0xFF) && (pPciByteTable->bANDMask == 0xFF) && (pPciByteTable->bORMask == 0xFF) ) {
      pPciByteTable++;
      dbBusNo = pPciByteTable->bRegIndex;
      dbDevFnNo = pPciByteTable->bANDMask;
      pPciByteTable++;
      i++;
    } else {
      ddBDFR = (dbBusNo << 24) + (dbDevFnNo << 16) + (pPciByteTable->bRegIndex) ;
      TRACE ((DMSG_SB_TRACE, "PFA = %X  AND = %X, OR = %X", ddBDFR, pPciByteTable->bANDMask, pPciByteTable->bORMask));
      RWPCI (ddBDFR, AccWidthUint8 | S3_SAVE, pPciByteTable->bANDMask, pPciByteTable->bORMask);
      pPciByteTable++;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * programSbAcpiMmioTbl - Program SB ACPI MMIO register by table (8 bits data)
 *
 *
 *
 * @param[in] pAcpiTbl   - Table data pointer
 *
 */
VOID
programSbAcpiMmioTbl (
  IN       AcpiRegWrite *pAcpiTbl
  )
{
  UINT8 i;
  UINT32 ddtempVar;
  if (pAcpiTbl != NULL) {
    if ((pAcpiTbl->MmioReg == 0) && (pAcpiTbl->MmioBase == 0) && (pAcpiTbl->DataANDMask == 0xB0) && (pAcpiTbl->DataOrMask == 0xAC)) {
      // Signature Checking
      pAcpiTbl++;
      for ( i = 1; pAcpiTbl->MmioBase < 0x1D; i++ ) {
        ddtempVar = 0xFED80000 | (pAcpiTbl->MmioBase) << 8 | pAcpiTbl->MmioReg;
        RWMEM (ddtempVar, AccWidthUint8, ((pAcpiTbl->DataANDMask) | 0xFFFFFF00), pAcpiTbl->DataOrMask);
        pAcpiTbl++;
      }
    }
  }
}

/**
 * getChipSysMode - Get Chip status
 *
 *
 * @param[in] Value - Return Chip strap status
 *   StrapStatus [15.0] - Hudson-2 chip Strap Status
 *    @li <b>0001</b> - Not USED FWH
 *    @li <b>0002</b> - Not USED LPC ROM
 *    @li <b>0004</b> - EC enabled
 *    @li <b>0008</b> - Reserved
 *    @li <b>0010</b> - Internal Clock mode
 *
 */
VOID
getChipSysMode (
  IN       VOID* Value
  )
{
  ReadMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG80, AccWidthUint8, Value);
}

/**
 * isImcEnabled - Is IMC Enabled
 * @retval  TRUE for IMC Enabled; FALSE for IMC Disabled
 */
BOOLEAN
isImcEnabled (
  )
{
  UINT8   dbSysConfig;
  getChipSysMode (&dbSysConfig);
  if (dbSysConfig & ChipSysEcEnable) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read Southbridge CIMx configuration structure pointer
 *
 *
 *
 * @retval  0xXXXXXXXX   CIMx configuration structure pointer.
 *
 */
AMDSBCFG*
getConfigPointer (
  OUT VOID
  )
{
  UINT8  dbReg;
  UINT8  dbValue;
  UINT8  i;
  UINT32  ddValue;
  ddValue = 0;
  dbReg = SB_ECMOS_REG08;

  for ( i = 0; i <= 3; i++ ) {
    WriteIO (SB_IOMAP_REG72, AccWidthUint8, &dbReg);
    ReadIO (SB_IOMAP_REG73, AccWidthUint8, &dbValue);
    ddValue |= (dbValue << (i * 8));
    dbReg++;
  }
  return ( (AMDSBCFG*) (UINTN)ddValue);
}

/**
 * getEfuseStatue - Get Efuse status
 *
 *
 * @param[in] Value - Return Chip strap status
 *
 */
VOID
getEfuseStatus (
  IN       VOID* Value
  )
{
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC8, AccWidthUint8, ~BIT5, BIT5);
  WriteMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD8, AccWidthUint8, Value);
  ReadMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD8 + 1, AccWidthUint8, Value);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC8, AccWidthUint8, ~BIT5, 0);
}

/**
 * getEfuseByte - Get Efuse Byte
 *
 *
 * @param[in] Index - Efuse Index value
 *
 */
UINT8
getEfuseByte (
  IN       UINT8 Index
  )
{
  UINT8 Data;
  WriteMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD8, AccWidthUint8, &Index);
  ReadMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD8 + 1, AccWidthUint8, &Data);
  return Data;
}


/**
 * SbResetGppDevice - Toggle GEVENT4 to assert/deassert GPP device reset
 *
 *
 * @param[in] ResetBlock - PCIE reset for SB GPP or NB PCIE
 * @param[in] ResetOp    - Assert or deassert PCIE reset
 *
 */
VOID
SbResetPcie (
  IN       RESET_BLOCK  ResetBlock,
  IN       RESET_OP     ResetOp
  )
{

  if (ResetBlock == NbBlock) {
    if (ResetOp == AssertReset) {
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC4, AccWidthUint8, 0xFF, BIT4);
    } else if (ResetOp == DeassertReset) {
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC4, AccWidthUint8, ~BIT4, 0);
    }
  } else if (ResetBlock == SbBlock) {
    RWMEM (ACPI_MMIO_BASE + IOMUX_BASE + SB_GEVENT_REG04, AccWidthUint8, ~(BIT1 + BIT0), 0x02);
    if (ResetOp == AssertReset) {
      RWMEM (ACPI_MMIO_BASE + GPIO_BASE + SB_GEVENT_REG04, AccWidthUint8, ~BIT5, 0);
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGBF, AccWidthUint8, 0xFF, BIT4);
    } else if (ResetOp == DeassertReset) {
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGBF, AccWidthUint8, ~BIT4, 0);
      RWMEM (ACPI_MMIO_BASE + GPIO_BASE + SB_GEVENT_REG04, AccWidthUint8, 0xff, BIT5);
    }
  }
}


/**
 * sbGppTogglePcieReset - Toggle PCIE_RST2#
 *
 *
 * @param[in] pConfig
 *
 */
VOID
sbGppTogglePcieReset (
  IN     AMDSBCFG*   pConfig
  )
{
  if (pConfig->GppToggleReset) {
    SbResetPcie (SbBlock, AssertReset);
    SbStall (500);
    SbResetPcie (SbBlock, DeassertReset);
  }
}

/**
 * sbSpiUnlock - Sb SPI Unlock
 *
 *
 * @param[in] pConfig
 *
 */
VOID
sbSpiUnlock (
  IN     AMDSBCFG*   pConfig
  )
{
  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG50, AccWidthUint32, ~(BIT0 + BIT1), 0);
  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG54, AccWidthUint32, ~(BIT0 + BIT1), 0);
  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG58, AccWidthUint32, ~(BIT0 + BIT1), 0);
  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG5C, AccWidthUint32, ~(BIT0 + BIT1), 0);
  RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG00, AccWidthUint32, ~(BIT22 + BIT23), (BIT22 + BIT23));
}

/**
 * sbSpilock - Sb SPI lock
 *
 *
 * @param[in] pConfig
 *
 */
VOID
sbSpilock (
  IN     AMDSBCFG*   pConfig
  )
{
  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG50, AccWidthUint32, ~(BIT0 + BIT1), (BIT0 + BIT1));
  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG54, AccWidthUint32, ~(BIT0 + BIT1), (BIT0 + BIT1));
  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG58, AccWidthUint32, ~(BIT0 + BIT1), (BIT0 + BIT1));
  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG5C, AccWidthUint32, ~(BIT0 + BIT1), (BIT0 + BIT1));
  RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG00, AccWidthUint32, ~(BIT22 + BIT23), 0);
}

/**
 * TurnOffCG2
 *
 *
 * @retval  VOID
 *
 */
VOID
TurnOffCG2 (
  OUT VOID
  )
{
  RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x40, AccWidthUint8, ~BIT6, 0);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGDA, AccWidthUint8, 0x0F, 0xA0);
  RWMEM (ACPI_MMIO_BASE + IOMUX_BASE + 0x41, AccWidthUint8, ~(BIT1 + BIT0), (BIT1 + BIT0));
  RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x41, AccWidthUint8, ~( BIT4), (BIT4));
  RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x41, AccWidthUint8, ~(BIT6), (BIT6));
  RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x08, AccWidthUint8, ~BIT6, BIT6);
  RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x1C, AccWidthUint8, ~BIT6, BIT6);
}

/**
 * BackUpCG2
 *
 *
 * @retval  VOID
 *
 */
VOID
BackUpCG2 (
  OUT VOID
  )
{
  UINT8 dByte;
  ReadMEM (ACPI_MMIO_BASE + MISC_BASE + 0x1C, AccWidthUint8, &dByte);
  if (dByte & BIT6) {
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x41, AccWidthUint8, ~(BIT6), (0));
  }
}
