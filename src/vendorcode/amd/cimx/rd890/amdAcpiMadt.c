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
#include "amdAcpiMadt.h"
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
 * Get first block entry in an IVRS (IOMMU) table
 *
 *
 *
 * @param[in] StructureType         Type of structure entry to find (APIC = 1, 0xFF = next structure)
 * @param[in] MadtPtr               Pointer to MADT ACPI table
 *
 */

VOID*
LibAmdGetFirstMadtStructure (
  IN       UINT8  StructureType,
  IN       VOID   *MadtPtr
  )
{
  // Start at MADT pointer + 48 (48 is always the size of IVRS header)
  UINT8* BlockPtr;

  // If our pointer is not to an IVRS, return error
//  if (((DESCRIPTION_HEADER*)MadtPtr)->Signature != 'CIPA') {
  if (((DESCRIPTION_HEADER*)MadtPtr)->Signature != Int32FromChar ('C', 'I', 'P', 'A')) {
    return NULL;
  }
  BlockPtr = (UINT8*)MadtPtr + 44;

  // Search each entry incrementing by it's size field in offset 2 until
  // we reach the end of the IVRS
  do {
    if (*BlockPtr == StructureType) {
      return BlockPtr;
    }
    BlockPtr += *(BlockPtr + 1);
  } while (BlockPtr < (UINT8*)MadtPtr + ((DESCRIPTION_HEADER*)MadtPtr)->Length);
  return NULL;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get next block entry in an IVRS (IOMMU) table
 *
 *
 *
 * @param[in] StructureType         Type of structure entry to find (APIC = 1, ..., 0xFF = next structure)
 * @param[in] CurrentStructurePtr   Pointer to current structure in IVRS
 * @param[in] MadtPtr               Pointer to MADT ACPI table
 *
 */

VOID*
LibAmdGetNextMadtStructure (
  IN       UINT8  StructureType,
  IN       VOID   *CurrentStructurePtr,
  IN       VOID   *MadtPtr
  )
{
  UINT8 *BlockPtr;
  BlockPtr = (UINT8*)CurrentStructurePtr + (*(UINT8*) ((UINT8*)CurrentStructurePtr + 1));

  // If our pointer is not to an IVRS, return error
//  if (((DESCRIPTION_HEADER*)MadtPtr)->Signature != 'CIPA') {
  if (((DESCRIPTION_HEADER*)MadtPtr)->Signature != Int32FromChar ('C', 'I', 'P', 'A')) {
    return NULL;
  }

  // Search each entry incrementing by it's size field in offset 2 until
  // we reach the end of the IVRS
  while (BlockPtr < ((UINT8*)MadtPtr + ((DESCRIPTION_HEADER*)MadtPtr)->Length)) {
    if (*BlockPtr == StructureType) {
      return BlockPtr;
    }
    BlockPtr += *(BlockPtr + 1);
  }
  return NULL;
}
