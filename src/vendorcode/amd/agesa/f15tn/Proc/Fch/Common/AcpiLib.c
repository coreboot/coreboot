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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
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
      if ( *RsdPtr == Int32FromChar('R','S','D',' ') && *(RsdPtr + 1) == Int32FromChar('P','T','R',' ') ) { /* ' DSR' & ' RTP' */
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


UINT8
ReadFchSleepType (
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT16     Value16;

  ReadPmio (FCH_PMIOA_REG62, AccessWidth16, &Value16, StdHeader);
  LibAmdIoRead (AccessWidth16, Value16, &Value16, StdHeader);
  return (UINT8) ((Value16 >> 10) & 7);
}

