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
/*
 *****************************************************************************
 *
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 * ***************************************************************************
 *
 */

#include "SBPLATFORM.h"

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
    for ( i = 1; pAcpiTbl->MmioBase < 0xf0; i++ ) {
      ddtempVar = 0xFED80000 | (pAcpiTbl->MmioBase) << 8 | pAcpiTbl->MmioReg;
      RWMEM (ddtempVar, AccWidthUint8, ((pAcpiTbl->DataANDMask) | 0xFFFFFF00), pAcpiTbl->DataOrMask);
      pAcpiTbl++;
    }
  }
}

/**
 * getChipSysMode - Get Chip status
 *
 *
 * @param[in] Value - Return Chip strap status
 *   StrapStatus [15.0] - SB800 chip Strap Status
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
