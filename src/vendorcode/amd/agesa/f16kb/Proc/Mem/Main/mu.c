/* $NoKeywords:$ */
/**
 * @file
 *
 * HyperTransport features and sequence implementation.
 *
 * Implements the external AmdHtInitialize entry point.
 * Contains routines for directing the sequence of available features.
 * Mostly, but not exclusively, AGESA_TESTPOINT invocations should be
 * contained in this file, and not in the feature code.
 *
 * From a build option perspective, it may be that a few lines could be removed
 * from compilation in this file for certain options.  It is considered that
 * the code savings from this are too small to be of concern and this file
 * should not have any explicit build option implementation.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 35978 $   @e \$Date: 2010-08-07 02:18:50 +0800 (Sat, 07 Aug 2010) $
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
 *
 * ***************************************************************************
 *
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "amdlib.h"
#include "Filecode.h"

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

VOID
MemUWriteCachelines (
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  );

VOID
MemUReadCachelines (
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

VOID
MemUDummyCLRead (
  IN       UINT32 Address
  );

VOID
MemUMFenceInstr (
  VOID
  );

VOID
MemUFlushPattern (
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

VOID
AlignPointerTo16Byte (
  IN OUT   UINT8 **BufferPtrPtr
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

//----------------------------------------------------------------------------

VOID
MemUWriteCachelines (
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  )
{
  UINTN Index;
  CHAR8 *Position;
  __m128i *Src = (void *) Pattern;
  __m128i *Dest = (void *) (size_t)Address;

  Position = (void *) Pattern;

  // ssd - important: without this, the src data may get evicted from cache
  _mm_mfence ();

  for (Index = 0; Index < ClCount * 4; Index++){
    _mm_stream_si128_fs (Dest, Src);
    Src++;
    Dest++;
  }

  // ssd - might not be required, but no measurable boot time impact
  _mm_mfence ();
}

//----------------------------------------------------------------------------
// MemUReadCachelines:
//
// Read a pattern of 72 bit times (per DQ), to test dram functionality.  The
// pattern is a stress pattern which exercises both ISI and crosstalk.  The number
// of cache lines to fill is dependent on DCT width mode and burstlength.
//
//             In: Buffer    - pointer to a buffer where read data will be stored
//                 Address   - Physical address to be read
//                 ClCount   - number of cachelines to be read

VOID
MemUReadCachelines (
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  )
{
  UINTN Index;
  UINT32 *Dest;

  for (Index = 0; Index < ClCount * 16; Index++) {
    Dest = (void *) &Buffer [Index * 4];
    *Dest = __readfsdword (Address + Index * 4);
    _mm_mfence ();
  }
}

//----------------------------------------------------------------------------
// MemUDummyCLRead:
//
//    Perform a single cache line read from a given physical address.
//
//              In: Address   - Physical address to be read
//                  ClCount   - number of cachelines to be read

//FUNC_ATTRIBUTE (noinline)
VOID
MemUDummyCLRead (
  IN       UINT32 Address
  )
{
  _mm_sfence ();
  __readfsbyte (Address);
}

//----------------------------------------------------------------------------

VOID
MemUMFenceInstr (
  VOID
  )
{
  _mm_mfence ();
}

//----------------------------------------------------------------------------
// MemUFlushPattern:
//
//  Flush a pattern of 72 bit times (per DQ) from cache.  This procedure is used
//  to ensure cache miss on the next read training.
//
//              In: Address   - Physical address to be flushed
//                  ClCount   - number of cachelines to be flushed
//FUNC_ATTRIBUTE(noinline)
VOID
MemUFlushPattern (
  IN       UINT32 Address,
  IN       UINT16 ClCount
  )
{
  UINTN Index;

  // ssd - theory: a tlb flush is needed to avoid problems with clflush
  __writemsr (0x20F, __readmsr (0x20F));

  for (Index = 0; Index < ClCount; Index++) {
    // mfence prevents speculative execution of the clflush
    _mm_mfence ();
    _mm_clflush_fs ((void *) (size_t) (Address + Index * 64));
  }
}

//----------------------------------------------------------------------------

//FUNC_ATTRIBUTE(noinline)
VOID
AlignPointerTo16Byte (
  IN OUT   UINT8 **BufferPtrPtr
  )
{
  size_t Address = (size_t) *BufferPtrPtr;
  Address += 15;
  Address -= Address % 16;
  *BufferPtrPtr = (void *) Address;
}

//----------------------------------------------------------------------------
