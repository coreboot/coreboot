/**

Copyright (C) 2013, Intel Corporation

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.

**/

/***********************************************************************
 *
 * board_fsp.c
 *
 * Parse HOB to get system data.
 *
 **********************************************************************/
#include "fsp.h"

void
GetLowMemorySize (
  uint32_t         *LowMemoryLength
  )
{
  EFI_PEI_HOB_POINTERS    Hob;

  *LowMemoryLength = 0x100000;

  //
  // Get the HOB list for processing
  //
  Hob.Raw = GetHobList();

  //
  // Collect memory ranges
  //
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      if (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) {
        //
        // Need memory above 1MB to be collected here
        //
        if (Hob.ResourceDescriptor->PhysicalStart >= 0x100000 &&
            Hob.ResourceDescriptor->PhysicalStart < (EFI_PHYSICAL_ADDRESS) 0x100000000) {
          *LowMemoryLength += (uint32_t) (Hob.ResourceDescriptor->ResourceLength);
        }
      }
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  return;
}

void
GetHighMemorySize (
  uint64_t         *HighMemoryLength
  )
{
  EFI_PEI_HOB_POINTERS    Hob;

  *HighMemoryLength = 0x0;

  //
  // Get the HOB list for processing
  //
  Hob.Raw = GetHobList();

  //
  // Collect memory ranges
  //
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      if (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) {
        //
        // Need memory above 4GB to be collected here
        //
        if (Hob.ResourceDescriptor->PhysicalStart >= (EFI_PHYSICAL_ADDRESS) 0x100000000) {
          *HighMemoryLength += (uint64_t) (Hob.ResourceDescriptor->ResourceLength);
        }
      }
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  return;
}

void
GetFspReservedMemoryFromGuid (
  uint32_t         *FspMemoryBase,
  uint32_t         *FspMemoryLength,
  EFI_GUID          FspReservedMemoryGuid
  )
{
  EFI_PEI_HOB_POINTERS    Hob;

  //
  // Get the HOB list for processing
  //
  Hob.Raw = GetHobList();
  *FspMemoryBase = 0;
  *FspMemoryLength = 0;

  //
  // Collect memory ranges
  //
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      if (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_MEMORY_RESERVED) {
        if (CompareGuid(&Hob.ResourceDescriptor->Owner, &FspReservedMemoryGuid)) {
          *FspMemoryBase = (uint32_t) (Hob.ResourceDescriptor->PhysicalStart);
          *FspMemoryLength = (uint32_t) (Hob.ResourceDescriptor->ResourceLength);
		      break;
        }
      }
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  return;
}

void
GetFspNVStorageMemory (
  VOID         **FspNVStorageHob,
  uint16_t     *DataSize
  )
{

  EFI_GUID                FspNVStorageHobGuid = FSP_NON_VOLATILE_STORAGE_HOB_GUID;
  uint8_t                 *GuidHob;
  EFI_HOB_GENERIC_HEADER  *GuidHobHdr;

  GuidHob = GetFirstGuidHob(&FspNVStorageHobGuid);
  if (!GuidHob) {
    *FspNVStorageHob = 0;
    *DataSize = 0;
  } else {
    *FspNVStorageHob = GET_GUID_HOB_DATA (GuidHob);
    GuidHobHdr = (EFI_HOB_GENERIC_HEADER *)GuidHob;
    *DataSize =  GET_GUID_HOB_DATA_SIZE (GuidHobHdr);
  }
}

void
GetTempRamStack (
  VOID         **TempRamStackPtr,
  uint16_t     *DataSize
  )
{

  EFI_GUID                FspBootloaderTemporaryMemoryHobGuid = FSP_BOOTLOADER_TEMPORARY_MEMORY_HOB_GUID;
  uint8_t                 *GuidHob;
  EFI_HOB_GENERIC_HEADER  *GuidHobHdr;

  GuidHob = GetFirstGuidHob(&FspBootloaderTemporaryMemoryHobGuid);
  if (!GuidHob) {
    *TempRamStackPtr = 0;
    *DataSize = 0;
  } else {
    *TempRamStackPtr = GET_GUID_HOB_DATA (GuidHob);
    GuidHobHdr = (EFI_HOB_GENERIC_HEADER *)GuidHob;
    *DataSize =  GET_GUID_HOB_DATA_SIZE (GuidHobHdr);
  }
}
