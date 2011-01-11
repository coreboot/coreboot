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

//
//
// Routine Description:
//
//  Locate ACPI table
//
// Arguments:
//
//  Signature - table signature
//
//Returns:
//
//  pointer to ACPI table
//
//
VOID*
ACPI_LocateTable (
  IN       UINT32 Signature
  )
{
  UINT32  i;
  UINT32* RsdPtr;
  UINT32* Rsdt;
  UINTN   tableOffset;
  DESCRIPTION_HEADER* CurrentTable;

  RsdPtr = (UINT32*) (UINTN)0xe0000;
  Rsdt = NULL;
  do {
    if ( *RsdPtr == Int32FromChar('R', 'S', 'D', ' ') && *(RsdPtr + 1) == Int32FromChar('P', 'T', 'R', ' ')) {
      Rsdt = (UINT32*) (UINTN) ((RSDP*)RsdPtr)->RsdtAddress;
      break;
    }
    RsdPtr += 4;
  } while ( RsdPtr <= (UINT32*) (UINTN)0xffff0 );
  if ( Rsdt != NULL && ACPI_GetTableChecksum (Rsdt) == 0 ) {
    for ( i = 0; i < (((DESCRIPTION_HEADER*)Rsdt)->Length - sizeof (DESCRIPTION_HEADER)) / 4; i++ ) {
      tableOffset = *(UINTN*) ((UINT8*)Rsdt + sizeof (DESCRIPTION_HEADER) + i * 4);
      CurrentTable = (DESCRIPTION_HEADER*)tableOffset;
      if ( CurrentTable->Signature == Signature ) {
        return CurrentTable;
      }
    }
  }
  return NULL;
}

//
//
//  Routine Description:
//
//  Update table checksum
//
//  Arguments:
//
//  TablePtr - table pointer
//
//  Returns:
//
//  none
//
//
VOID
ACPI_SetTableChecksum (
  IN       VOID* TablePtr
  )
{
  UINT8 Checksum;
  Checksum = 0;
  ((DESCRIPTION_HEADER*)TablePtr)->Checksum = 0;
  Checksum = ACPI_GetTableChecksum (TablePtr);
  ((DESCRIPTION_HEADER*)TablePtr)->Checksum = (UINT8)(0x100 - Checksum);
}

//
//
//  Routine Description:
//
//  Get table checksum
//
//  Arguments:
//
//  TablePtr - table pointer
//
//  Returns:
//
//  none
//
//
UINT8
ACPI_GetTableChecksum (
  IN       VOID* TablePtr
  )
{
  return GetByteSum (TablePtr, ((DESCRIPTION_HEADER*)TablePtr)->Length);
}


UINT8
GetByteSum (
  IN       VOID* pData,
  IN       UINT32 Length
  )
{
  UINT32  i;
  UINT8 Checksum;
  Checksum = 0;
  for ( i = 0; i < Length; i++ ) {
    Checksum = Checksum + (*((UINT8*)pData + i));
  }
  return Checksum;
}
VOID
GetSbAcpiMmioBase (
  OUT     UINT32*    AcpiMmioBase
  )
{
  UINT32    Value16;

  ReadPMIO (SB_PMIOA_REG24 + 2, AccWidthUint16, &Value16);
  *AcpiMmioBase = Value16 << 16;
}

VOID
GetSbAcpiPmBase (
  OUT     UINT16*    AcpiPmBase
  )
{
  ReadPMIO (SB_PMIOA_REG60, AccWidthUint16, AcpiPmBase);
}

