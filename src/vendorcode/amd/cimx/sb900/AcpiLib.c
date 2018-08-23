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
  IN       unsigned int Signature
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
//    if ( *RsdPtr == ' DSR' && *(RsdPtr + 1) == ' RTP' ) {
//    if ( (*RsdPtr == 0x52534420) && (*(RsdPtr + 1) == 0x50545220) ) {
    if ( (*RsdPtr == 0x20445352) && (*(RsdPtr + 1) == 0x20525450) ) {
      Rsdt = (UINT32*) (UINTN) ((RSDP_HEADER*)RsdPtr)->RsdtAddress;
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
  ((DESCRIPTION_HEADER*)TablePtr)->Checksum = (UINT8) (0x100 - Checksum);
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

VOID
SetAcpiPma (
  IN     UINT8    pmaControl
  )
{
  UINT16 pmaBase;
  UINT16 dwValue;
  ReadMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG74, AccWidthUint16, &dwValue);
  dwValue &= ~BIT6;
  WriteMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG74, AccWidthUint16, &dwValue);
  ReadMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG6E, AccWidthUint16, &pmaBase);
  WriteIo8 (pmaBase, pmaControl);
  RWMEM (ACPI_MMIO_BASE + SMI_BASE + SB_PMIOA_REG98 + 3, AccWidthUint8, ~BIT7, pmaControl << 7);
}

