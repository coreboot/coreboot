/**

Copyright (C) 2013-2016 Intel Corporation

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

#include <types.h>
#include <string.h>
#include <fsp.h>

/**
  Reads a 64-bit value from memory that may be unaligned.

  This function returns the 64-bit value pointed to by Buffer. The function
  guarantees that the read operation does not produce an alignment fault.

  If the Buffer is NULL, then ASSERT().

  @param  Buffer  Pointer to a 64-bit value that may be unaligned.
  @return The 64-bit value read from Buffer.

**/
STATIC
UINT64
EFIAPI
ReadUnaligned64 (
  CONST UINT64        *Buffer
  )
{
  ASSERT (Buffer != NULL);
  return *Buffer;
}

/**
  Compares two GUIDs.

  This function compares Guid1 to Guid2.  If the GUIDs are identical then TRUE is returned.
  If there are any bit differences in the two GUIDs, then FALSE is returned.

  If Guid1 is NULL, then ASSERT().
  If Guid2 is NULL, then ASSERT().

  @param  Guid1     A pointer to a 128 bit GUID.
  @param  Guid2     A pointer to a 128 bit GUID.
  @retval TRUE      Guid1 and Guid2 are identical.
  @retval FALSE     Guid1 and Guid2 are not identical.

**/
STATIC
BOOLEAN
EFIAPI
CompareGuid (
  CONST EFI_GUID  *Guid1,
  CONST EFI_GUID  *Guid2
  )
{
  UINT64  LowPartOfGuid1;
  UINT64  LowPartOfGuid2;
  UINT64  HighPartOfGuid1;
  UINT64  HighPartOfGuid2;

  LowPartOfGuid1  = ReadUnaligned64 ((CONST UINT64*) Guid1);
  LowPartOfGuid2  = ReadUnaligned64 ((CONST UINT64*) Guid2);
  HighPartOfGuid1 = ReadUnaligned64 ((CONST UINT64*) Guid1 + 1);
  HighPartOfGuid2 = ReadUnaligned64 ((CONST UINT64*) Guid2 + 1);

  return (BOOLEAN) (LowPartOfGuid1 == LowPartOfGuid2 && HighPartOfGuid1 == HighPartOfGuid2);
}

/**
  Returns the next instance of a HOB type from the starting HOB.

  @param  Type      HOB type to search
  @param  HobStart  A pointer to the HOB list
  @retval a HOB object with matching type; Otherwise NULL.

**/
VOID *
EFIAPI
GetNextHob (
  UINT16           Type,
  CONST VOID       *HobStart
  )
{
  EFI_PEI_HOB_POINTERS  Hob;

  ASSERT (HobStart != NULL);

  Hob.Raw = (UINT8 *) HobStart;
  /*
   * Parse the HOB list until end of list or matching type is found.
   */
  while (!END_OF_HOB_LIST(Hob.Raw)) {
    if (Hob.Header->HobType == Type) {
      return Hob.Raw;
    }
    Hob.Raw = GET_NEXT_HOB(Hob.Raw);
  }

  return NULL;
}

/**
  Returns the next instance of the matched GUID HOB from the starting HOB.

  @param  Guid      GUID to search
  @param  HobStart  A pointer to the HOB list
  @retval a HOB object with matching GUID; Otherwise NULL.

**/
VOID *
EFIAPI
GetNextGuidHob (
  CONST EFI_GUID     *Guid,
  CONST VOID         *HobStart
  )
{
  EFI_PEI_HOB_POINTERS  GuidHob;

  GuidHob.Raw = (UINT8 *) HobStart;
  while ((GuidHob.Raw = GetNextHob(EFI_HOB_TYPE_GUID_EXTENSION, GuidHob.Raw)) != NULL) {
    if (CompareGuid (Guid, &GuidHob.Guid->Name)) {
      break;
    }
    GuidHob.Raw = GET_NEXT_HOB(GuidHob.Raw);
  }

  return GuidHob.Raw;
}

/**
  This function retrieves the top of usable low memory.

  @param    HobListPtr   A HOB list pointer.
  @retval                Usable low memory top.

**/
UINT32
GetUsableLowMemTop (
  CONST VOID       *HobStart
  )
{
  EFI_PEI_HOB_POINTERS  Hob;
  UINT32          MemLen;
  /*
   * Get the HOB list for processing
   */
  Hob.Raw = (VOID *)HobStart;

  /*
   * Collect memory ranges
   */
  MemLen = 0x100000;
  while (!END_OF_HOB_LIST(Hob.Raw)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      if (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) {
        /*
         * Need memory above 1MB to be collected here
         */
        if (Hob.ResourceDescriptor->PhysicalStart >= 0x100000 &&
            Hob.ResourceDescriptor->PhysicalStart < (EFI_PHYSICAL_ADDRESS) 0x100000000) {
          MemLen += (UINT32) (Hob.ResourceDescriptor->ResourceLength);
        }
      }
    }
    Hob.Raw = GET_NEXT_HOB(Hob.Raw);
  }

  return MemLen;
}

/**
  This function retrieves the top of system low memory.

  @param    HobListPtr   A HOB list pointer.
  @retval                Usable low memory top.

**/
UINT32
GetPhysicalLowMemTop (
  CONST VOID       *HobStart
  )
{
  EFI_PEI_HOB_POINTERS  Hob;
  UINT32            MemBase;
  UINT32            MemLen;

  /*
   * Get the HOB list for processing
   */
  Hob.Raw = (VOID *)HobStart;

  /*
   * Collect memory ranges
   */
  MemBase = 0x100000;
  MemLen  = 0;
  while (!END_OF_HOB_LIST(Hob.Raw)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      if ((Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) ||
           (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_MEMORY_RESERVED)) {
        /*
         * Need memory above 1MB to be collected here
         */
        if (Hob.ResourceDescriptor->PhysicalStart >= MemBase &&
            Hob.ResourceDescriptor->PhysicalStart < (EFI_PHYSICAL_ADDRESS) 0x100000000) {
          MemBase = (UINT32) (Hob.ResourceDescriptor->PhysicalStart);
          MemLen  = (UINT32) (Hob.ResourceDescriptor->ResourceLength);
        }
      }
    }
    Hob.Raw = GET_NEXT_HOB(Hob.Raw);
  }

  return MemBase + MemLen;
}

/**
  This function retrieves the top of usable high memory.

  @param  HobListPtr   A HOB list pointer.
  @retval              Usable high memory top.

**/
UINT64
GetUsableHighMemTop (
  CONST VOID       *HobStart
  )
{
  EFI_PEI_HOB_POINTERS  Hob;
  UINT64                MemTop;
  /*
   * Get the HOB list for processing
   */
  Hob.Raw = (VOID *)HobStart;

  /*
   * Collect memory ranges
   */
  MemTop = 0x100000000;
  while (!END_OF_HOB_LIST(Hob.Raw)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      if (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) {
        /*
         * Need memory above 1MB to be collected here
         */
        if (Hob.ResourceDescriptor->PhysicalStart >= (EFI_PHYSICAL_ADDRESS) 0x100000000) {
          MemTop += (UINT64) (Hob.ResourceDescriptor->ResourceLength);
        }
      }
    }
    Hob.Raw = GET_NEXT_HOB(Hob.Raw);
  }

  return MemTop;
}

/**
  This function retrieves a special reserved memory region.

  @param  HobListPtr   A HOB list pointer.
  @param  Length       A pointer to the GUID HOB data buffer length.  If the GUID HOB is
                       located, the length will be updated.
  @param  OwnerGuid    A pointer to the owner guild.
  @retval              Reserved region start address.  0 if this region does not exist.

**/
UINT64
GetFspReservedMemoryFromGuid (
  CONST VOID     *HobListPtr,
  UINT64         *Length,
  EFI_GUID       *OwnerGuid
  )
{
  EFI_PEI_HOB_POINTERS    Hob;

  /*
   * Get the HOB list for processing
   */
  Hob.Raw = (VOID *)HobListPtr;

  /*
   * Collect memory ranges
   */
  while (!END_OF_HOB_LIST(Hob.Raw)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      if (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_MEMORY_RESERVED) {
        if (CompareGuid(&Hob.ResourceDescriptor->Owner, OwnerGuid)) {
          if (Length) {
            *Length = (UINT32) (Hob.ResourceDescriptor->ResourceLength);
          }
          return (UINT64)(Hob.ResourceDescriptor->PhysicalStart);
        }
      }
    }
    Hob.Raw = GET_NEXT_HOB(Hob.Raw);
  }

  return 0;
}

/**
  This function retrieves the TSEG reserved normal memory.

  @param  HobListPtr   A HOB list pointer.
  @param  Length       A pointer to the TSEG reserved memory length buffer.  If the GUID HOB is
                       located, the length will be updated.
  @param  Guid         A pointer to owner HOB GUID.
  @retval NULL         Failed to find the TSEG reserved memory.
  @retval others       TSEG reserved memory base.

**/
UINT32
GetTsegReservedMemory (
  CONST VOID       *HobListPtr,
  UINT32           *Length
  )
{
  const EFI_GUID    TsegOwnerHobGuid = FSP_HOB_RESOURCE_OWNER_TSEG_GUID;
  UINT64            Length64;
  UINT32            Base;

  Base = (UINT32)GetFspReservedMemoryFromGuid (HobListPtr, &Length64, (EFI_GUID *)&TsegOwnerHobGuid);
  if ((Length != 0) && (Base != 0)) {
    *Length = (UINT32)Length64;
  }

  return Base;
}

/**
  This function retrieves the FSP reserved normal memory.

  @param  HobListPtr   A HOB list pointer.
  @param  Length       A pointer to the FSP reserved memory length buffer.  If the GUID HOB is
                       located, the length will be updated.
  @param  Guid         A pointer to owner HOB GUID.
  @retval NULL         Failed to find the FSP reserved memory.
  @retval others       FSP reserved memory base.

**/
UINT32
GetFspReservedMemory (
  CONST VOID       *HobListPtr,
  UINT32           *Length
  )
{
  const EFI_GUID    FspOwnerHobGuid = FSP_HOB_RESOURCE_OWNER_FSP_GUID;
  UINT64            Length64;
  UINT32            Base;

  Base = (UINT32)GetFspReservedMemoryFromGuid (HobListPtr, &Length64, (EFI_GUID *)&FspOwnerHobGuid);
  if ((Length != 0) && (Base != 0)) {
    *Length = (UINT32)Length64;
  }

  return Base;
}

/**
  This function retrieves a GUIDed HOB data buffer and size.

  @param  HobListPtr   A HOB list pointer.
  @param  Length       A pointer to the GUID HOB data buffer length.  If the
                       GUID HOB is located, the length will be updated.
  @param  Guid         A pointer to HOB GUID.
  @retval NULL         Failed to find the GUID HOB.
  @retval others       GUID HOB data buffer pointer.

**/
VOID *
GetGuidHobDataBuffer (
  CONST VOID       *HobListPtr,
  UINT32           *Length,
  EFI_GUID         *Guid
  )
{
  UINT8        *GuidHob;

  /* FSP NVS DATA HOB */
  GuidHob = GetNextGuidHob(Guid, HobListPtr);
  if (GuidHob == NULL) {
    return NULL;
  } else {
    if (Length) {
      *Length = GET_GUID_HOB_DATA_SIZE (GuidHob);
    }
    return GET_GUID_HOB_DATA (GuidHob);
  }
}

/**
  This function retrieves FSP Non-volatile Storage HOB buffer and size.

  @param  HobListPtr   A HOB list pointer.
  @param  Length       A pointer to the NVS data buffer length.  If the FSP NVS
                       HOB is located, the length will be updated.
  @retval NULL         Failed to find the NVS HOB.
  @retval others       FSP NVS data buffer pointer.

**/
VOID *
GetFspNvsDataBuffer (
  CONST VOID       *HobListPtr,
  UINT32           *Length
  )
{
  const EFI_GUID    FspNvsHobGuid = FSP_NON_VOLATILE_STORAGE_HOB_GUID;
  return GetGuidHobDataBuffer (HobListPtr, Length, (EFI_GUID *)&FspNvsHobGuid);
}

/**
  This function retrieves Bootloader temporary stack buffer and size.

  @param  HobListPtr   A HOB list pointer.
  @param  Length       A pointer to the Bootloader temporary stack length.
                       If the HOB is located, the length will be updated.
  @retval NULL         Failed to find the Bootloader temporary stack HOB.
  @retval others       Bootloader temporary stackbuffer pointer.

**/
VOID *
GetBootloaderTempMemoryBuffer (
  CONST VOID       *HobListPtr,
  UINT32           *Length
  )
{
  const EFI_GUID    FspBootloaderTemporaryMemoryHobGuid = FSP_BOOTLOADER_TEMPORARY_MEMORY_HOB_GUID;
  return GetGuidHobDataBuffer (HobListPtr, Length, (EFI_GUID *)&FspBootloaderTemporaryMemoryHobGuid);
}
