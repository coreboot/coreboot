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
#include "amdAcpiIvrs.h"

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
 * @param[in] IoVirtualizationEntryType   Type of IVHD or IVMD entry (IVHD = 0x10, IVMD = 0x20-0x22, any = 0xFF))
 * @param[in] Pointer to IVRS ACPI table
 *
 */
VOID*
LibAmdGetFirstIvrsBlockEntry (
  IN       UINT8  IoVirtualizationEntryType,
  IN       VOID   *IvrsPtr
  )
{
  // Start at IVRS pointer + 48 (48 is always the size of IVRS header)
  UINT8* BlockPtr;
  // If our pointer is not to an IVRS, return error

//  if (((DESCRIPTION_HEADER*)IvrsPtr)->Signature != 'SRVI') return NULL;
  if (((DESCRIPTION_HEADER*)IvrsPtr)->Signature != Int32FromChar ('S', 'R', 'V', 'I')) return NULL;
  BlockPtr = (UINT8*)IvrsPtr + 48;
  // Search each entry incrementing by it's size field in offset 2 until
  // we reach the end of the IVRS
  do {
    if (*BlockPtr == IoVirtualizationEntryType) {
      return BlockPtr;
    }
    BlockPtr += *((UINT16*) (BlockPtr + 2));
  } while (BlockPtr < (UINT8*)IvrsPtr + ((DESCRIPTION_HEADER*)IvrsPtr)->Length);
  return NULL;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get next block entry in an IVRS (IOMMU) table
 *
 *
 *
 * @param[in] IoVirtualizationEntryType   Type of IVHD or IVMD entry (IVHD = 0x10, IVMD = 0x20-0x22, any = 0xFF))
 * @param[in] CurrentStructurePtr         Pointer to current IVHD or IVMD block in IVRS
 * @param[in] IvrsPtr                     Pointer to IVRS table
 *
 */

VOID*
LibAmdGetNextIvrsBlockEntry (
  IN       UINT8 IoVirtualizationEntryType,
  IN       VOID* CurrentStructurePtr,
  IN       VOID* IvrsPtr
  )
{
  // Start at the current device entry
  // Start searching after the current entry
  UINT8* BlockPtr;
  // If our pointer is not to an IVRS, return error
//  if (((DESCRIPTION_HEADER*)IvrsPtr)->Signature != 'SRVI') return NULL;
  if (((DESCRIPTION_HEADER*)IvrsPtr)->Signature != Int32FromChar ('S', 'R', 'V', 'I')) return NULL;

  BlockPtr = (UINT8*)CurrentStructurePtr + *((UINT16*) ((UINT8*)CurrentStructurePtr + 2));

  // Search each entry incrementing by it's size field in offset 2 until
  // we reach the end of the IVRS
  while (BlockPtr < ((UINT8*)IvrsPtr + ((DESCRIPTION_HEADER*)IvrsPtr)->Length)) {
    if (*BlockPtr == IoVirtualizationEntryType) {
      return BlockPtr;
    }
    BlockPtr += *((UINT16*) (BlockPtr + 2));
  }
  return NULL;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get pointer to first Device Entry of an IVHD
 *
 *
 *
 * @param[in] IoVirtualizationEntryType   Type of IVHD device entry to search for. (0xFF returns next entry of any type)
 *                                        Typical values: 4 byte entries (0-63), 8 byte entries (64-127)
 * @param[in] IvhdBlockPtr                Pointer to current IVHD block
 *
 */
VOID*
LibAmdGetFirstDeviceEntry (
  IN       UINT8 DeviceEntryType,
  IN       VOID* IvhdBlockPtr
  )
{
  // Start at IVHD pointer + 24 (24 is always the size of IVHD header)
  // Not much we can do do validate an IVHD input, the only field we know for sure is the type
  UINT8   *EntryPtr;
  UINT16  IvhdSize;
  EntryPtr = (UINT8*)IvhdBlockPtr + 24;
  IvhdSize = *(UINT16*) ((UINT8*)IvhdBlockPtr + 2);
  // Search each entry incrementing by it's type field until the end of the IVHD
  // Types 0-63 are 4 byte size, 64-127 are 8 byte size
  do {
    if (*EntryPtr == DeviceEntryType) {
      return EntryPtr;
    }
    if (*EntryPtr < 64) {
      EntryPtr += 4;
    } else if ((*EntryPtr >= 64) && (*EntryPtr < 128)) {
      EntryPtr += 8;
    } else {
      ASSERT (TRUE);
      return NULL;
    }
  } while (EntryPtr < ((UINT8*)IvhdBlockPtr + IvhdSize));
  return NULL;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get pointer to Next Device Entry of an IVHD
 *
 *
 *
 *
 * @param[in] IoVirtualizationEntryType   Type of IVHD entry. (0xFF returns next entry of any type)
 *                                        Typical values: 4 byte entries (0-63), 8 byte entries (64-127)
 * @param[in] CurrentStructurePtr         Pointer to current device entry in IVHD block
 * @param[in] IvhdBlockPtr                Pointer to current IVHD block
 */
VOID*
LibAmdGetNextDeviceEntry (
  IN       UINT8 DeviceEntryType,
  IN       VOID* CurrentDeviceEntry,
  IN       VOID* IvhdBlockPtr
  )
{
  UINT8   *EntryPtr;
  UINT16  IvhdSize;
  // Start at IVHD pointer + 24 (24 is always the size of IVHD header)
  // Not much we can do do validate an IVHD input, the only field we know for sure is the type
  EntryPtr = CurrentDeviceEntry;

  if ((*EntryPtr != 0) && (*EntryPtr < 64)) {
    EntryPtr += 4;
  } else if ((*EntryPtr >= 64) && (*EntryPtr < 128)) {
    EntryPtr += 8;
  }
  IvhdSize = *(UINT16*) ((UINT8*)IvhdBlockPtr + 2);
  // Search each entry incrementing by it's type field until the end of the IVHD
  // Types 0-63 are 4 byte size, 64-127 are 8 byte size
  while (EntryPtr < ((UINT8*)IvhdBlockPtr + IvhdSize)) {
    if (*EntryPtr == DeviceEntryType) {
      return EntryPtr;
    }
    if (*EntryPtr < 64) {
      EntryPtr += 4;
    } else if ((*EntryPtr >= 64) && (*EntryPtr < 128)) {
      EntryPtr += 8;
    } else {
      ASSERT (TRUE);
      return NULL;
    }
  }
  return NULL;
}