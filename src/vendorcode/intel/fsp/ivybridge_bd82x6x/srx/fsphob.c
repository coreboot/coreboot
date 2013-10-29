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
 * fsphob.c
 *
 * HOB infrastructure code.
 *
 **********************************************************************/
#include <string.h>

#include "fsptypes.h"
#include "fsphob.h"

//
// Pointer to the HOB should be initialized with the output of FSP INIT PARAMS
//
extern volatile void *FspHobListPtr;

//
// Function prototype
//
UINT64
EFIAPI
ReadUnaligned64 (
  CONST UINT64              *Buffer
  );

/**
  Reads a 64-bit value from memory that may be unaligned.

  This function returns the 64-bit value pointed to by Buffer. The function
  guarantees that the read operation does not produce an alignment fault.

  If the Buffer is NULL, then ASSERT().

  @param  Buffer  Pointer to a 64-bit value that may be unaligned.

  @return The 64-bit value read from Buffer.

**/
UINT64
EFIAPI
ReadUnaligned64 (
  CONST UINT64              *Buffer
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

  @param  Guid1       A pointer to a 128 bit GUID.
  @param  Guid2       A pointer to a 128 bit GUID.

  @retval TRUE        Guid1 and Guid2 are identical.
  @retval FALSE       Guid1 and Guid2 are not identical.

**/
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
  Returns the pointer to the HOB list.
**/
VOID *
EFIAPI
GetHobList (
  VOID
  )
{
  ASSERT (FspHobListPtr != NULL);
  return ((VOID *)FspHobListPtr);
}

/**
  Returns the next instance of a HOB type from the starting HOB.
**/
VOID *
EFIAPI
GetNextHob (
  UINT16                 Type,
  CONST VOID             *HobStart
  )
{
  EFI_PEI_HOB_POINTERS  Hob;

  ASSERT (HobStart != NULL);

  Hob.Raw = (UINT8 *) HobStart;
  //
  // Parse the HOB list until end of list or matching type is found.
  //
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == Type) {
      return Hob.Raw;
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }
  return NULL;
}

/**
  Returns the first instance of a HOB type among the whole HOB list.
**/
VOID *
EFIAPI
GetFirstHob (
  UINT16                 Type
  )
{
  VOID      *HobList;

  HobList = GetHobList ();
  return GetNextHob (Type, HobList);
}

/**
  Returns the next instance of the matched GUID HOB from the starting HOB.
**/
VOID *
EFIAPI
GetNextGuidHob (
  CONST EFI_GUID         *Guid,
  CONST VOID             *HobStart
  )
{
  EFI_PEI_HOB_POINTERS  GuidHob;

  GuidHob.Raw = (UINT8 *) HobStart;
  while ((GuidHob.Raw = GetNextHob (EFI_HOB_TYPE_GUID_EXTENSION, GuidHob.Raw)) != NULL) {
    if (CompareGuid (Guid, &GuidHob.Guid->Name)) {
      break;
    }
    GuidHob.Raw = GET_NEXT_HOB (GuidHob);
  }
  return GuidHob.Raw;
}

/**
  Returns the first instance of the matched GUID HOB among the whole HOB list.
**/
VOID *
EFIAPI
GetFirstGuidHob (
  CONST EFI_GUID         *Guid
  )
{
  VOID      *HobList;

  HobList = GetHobList ();
  return GetNextGuidHob (Guid, HobList);
}
