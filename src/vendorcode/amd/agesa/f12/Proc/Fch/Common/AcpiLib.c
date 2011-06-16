/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH ACPI lib
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
#define FILECODE PROC_FCH_COMMON_ACPILIB_FILECODE
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
AcpiLocateTable (
  IN  UINT32     Signature
  )
{
  UINT32              Index;
  UINT32              *RsdPtr;
  UINT32              *Rsdt;
  UINTN               TableOffset;
  DESCRIPTION_HEADER  *CurrentTable;

  RsdPtr = (UINT32*) (UINTN) FCHOEM_ACPI_TABLE_RANGE_LOW;
  Rsdt = NULL;
  do {
    if ( *RsdPtr == ' DSR' && *(RsdPtr + 1) == ' RTP' ) {
      Rsdt = (UINT32*) (UINTN) ((RSDP_HEADER*)RsdPtr)->RsdtAddress;
      break;
    }
    RsdPtr += 4;
  } while ( RsdPtr <= (UINT32*) (UINTN) FCHOEM_ACPI_TABLE_RANGE_HIGH );

  if ( Rsdt != NULL && AcpiGetTableCheckSum (Rsdt) == 0 ) {
    for ( Index = 0; Index < (((DESCRIPTION_HEADER*)Rsdt)->Length - sizeof (DESCRIPTION_HEADER)) / 4; Index++ ) {
      TableOffset = *(UINTN*) ((UINT8*)Rsdt + sizeof (DESCRIPTION_HEADER) + Index * 4);
      CurrentTable = (DESCRIPTION_HEADER*)TableOffset;
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
//  Update table CheckSum
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
AcpiSetTableCheckSum (
  IN  VOID     *TablePtr
  )
{
  UINT8   CheckSum;

  CheckSum = 0;
  ((DESCRIPTION_HEADER*)TablePtr)->CheckSum = 0;
  CheckSum = AcpiGetTableCheckSum (TablePtr);
  ((DESCRIPTION_HEADER*)TablePtr)->CheckSum = (UINT8) (FCHOEM_ACPI_BYTE_CHECHSUM - CheckSum);
}

//
//
//  Routine Description:
//
//  Get table CheckSum - Get ACPI table checksum
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
AcpiGetTableCheckSum (
  IN  VOID     *TablePtr
  )
{
  return GetByteSum (TablePtr, ((DESCRIPTION_HEADER*)TablePtr)->Length);
}


//
//
//  Routine Description:
//
//  GetByteSum - Get BYTE checksum value
//
//  Arguments:
//
//  DataPtr - table pointer
//  Length  - table length
//
//  Returns:
//
//  CheckSum - CheckSum value
//
//
UINT8
GetByteSum (
  IN  VOID       *DataPtr,
  IN  UINT32     Length
  )
{
  UINT32   Index;
  UINT8    CheckSum;

  CheckSum = 0;
  for ( Index = 0; Index < Length; Index++ ) {
    CheckSum = CheckSum + (*((UINT8*)DataPtr + Index));
  }
  return CheckSum;
}

//
//
//  Routine Description:
//
//  GetFchAcpiMmioBase - Get FCH HwAcpi MMIO Base Address
//
//  Arguments:
//
//  AcpiMmioBase - HwAcpi MMIO Base Address
//  StdHeader    - Amd Stand Header
//
//  Returns:
//
//  AcpiMmioBase - HwAcpi MMIO Base Address
//
//
VOID
GetFchAcpiMmioBase (
     OUT   UINT32                *AcpiMmioBase,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32    AcpiMmioBaseAddressDword;

  ReadPmio (FCH_PMIOA_REG24 + 2, AccessWidth16, &AcpiMmioBaseAddressDword, StdHeader);
  *AcpiMmioBase = AcpiMmioBaseAddressDword << 16;
}

//
//
//  Routine Description:
//
//  GetFchAcpiPmBase - Get FCH HwAcpi PM Base Address
//
//  Arguments:
//
//  AcpiPmBase   - HwAcpi PM Base Address
//  StdHeader    - Amd Stand Header
//
//  Returns:
//
//  AcpiPmBase   - HwAcpi PM Base Address
//
//
VOID
GetFchAcpiPmBase (
     OUT   UINT16                *AcpiPmBase,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  ReadPmio (FCH_PMIOA_REG60, AccessWidth16, AcpiPmBase, StdHeader);
}


