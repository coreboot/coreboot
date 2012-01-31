/**
 * @file
 *
 * ACPI common library
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      Common Library
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
#include "AGESA.h"
#include "amdAcpiLib.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------*/
/**
 * Get ACPI table.
 *
 *
 *
 * @param[in] Signature  ACPI table signature
 *
 */

AGESA_STATUS
LibAmdGetAcpiTable (
  IN      UINT32    Signature,
  IN      VOID      **TablePtr,
  IN      UINTN     *TableHandle
  )
{
  UINT32  i;
  UINT32* RsdPtr;
  UINT32* Rsdt;
  DESCRIPTION_HEADER* CurrentTable;

  RsdPtr = (UINT32*) (UINTN)0xe0000;
  Rsdt = NULL;

  do {
//    if (*RsdPtr == ' DSR' && *(RsdPtr + 1) == ' RTP') {
	if ((*RsdPtr == Int32FromChar (' ', 'D', 'S', 'R')) && (*(RsdPtr+1) == Int32FromChar (' ', 'R', 'T', 'P'))) {
      Rsdt = (UINT32*) (UINTN) (((RSDP*)RsdPtr)->RsdtAddress);
      break;
    }
    RsdPtr += 4;
  } while (RsdPtr <= (UINT32*) ((UINTN)0xffff0));
  if (Rsdt != NULL && LibAmdGetAcpiTableChecksum (Rsdt) == 0) {
    for (i = 0; i < (((DESCRIPTION_HEADER*)Rsdt)->Length - sizeof (DESCRIPTION_HEADER)) / 4; i++) {
      CurrentTable = (DESCRIPTION_HEADER*) (UINTN)*(UINT32*) ((UINT8*)Rsdt + sizeof (DESCRIPTION_HEADER) + i*4);
      if (CurrentTable->Signature == Signature) {
        *TablePtr = CurrentTable;
        return AGESA_SUCCESS;
      }
    }
  }
  return AGESA_UNSUPPORTED;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set ACPI table.
 *
 *
 *
 * @param[in] Signature  ACPI table signature
 *
 */

AGESA_STATUS
LibAmdSetAcpiTable (
  IN      VOID      *TablePtr,
  IN      BOOLEAN   Checksum,
  IN      UINTN     *TableHandle
  )
{
  if (Checksum) {
    LibAmdUpdateAcpiTableChecksum (TablePtr);
  }
  return  AGESA_SUCCESS;
}
/*----------------------------------------------------------------------------------------*/
/**
 * Get ACPI table checksum
 *
 *
 *
 * @param[in] Pointer to ACPI table
 *
 */

UINT8
LibAmdGetAcpiTableChecksum (
  IN       VOID     *TablePtr
  )
{
  UINT32  i;
  UINT8 Checksum;

  Checksum = 0;

  for (i = 0; i < ((DESCRIPTION_HEADER*)TablePtr)->Length; i++) {
    Checksum = Checksum + *(UINT8*) ((UINT8*)TablePtr + i);
  }
  return Checksum;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Update ACPI table checksum
 *
 *
 *
 * @param[in] Pointer to ACPI table
 *
 */

VOID
LibAmdUpdateAcpiTableChecksum (
  IN       VOID     *TablePtr
  )
{
  UINT8 Checksum;
  Checksum = 0;
  ((DESCRIPTION_HEADER*)TablePtr)->Checksum = 0;
  Checksum = LibAmdGetAcpiTableChecksum (TablePtr);
  ((DESCRIPTION_HEADER*)TablePtr)->Checksum = 0x100 - Checksum;
}
