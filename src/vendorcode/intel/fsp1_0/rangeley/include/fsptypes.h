/******************************************************************************

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

 *****************************************************************************/

/** \file  fsptypes.h
 *
 *
 */

#ifndef __FSP_TYPES_H__
#define __FSP_TYPES_H__

///
/// 8-byte unsigned value.
///
typedef unsigned long long  UINT64;
///
/// 8-byte signed value.
///
typedef long long           INT64;
///
/// 4-byte unsigned value.
///
typedef unsigned int        UINT32;
///
/// 4-byte signed value.
///
typedef int                 INT32;
///
/// 2-byte unsigned value.
///
typedef unsigned short      UINT16;
///
/// 2-byte Character.  Unless otherwise specified all strings are stored in the
/// UTF-16 encoding format as defined by Unicode 2.1 and ISO/IEC 10646 standards.
///
typedef unsigned short      CHAR16;
///
/// 2-byte signed value.
///
typedef short               INT16;
///
/// Logical Boolean.  1-byte value containing 0 for FALSE or a 1 for TRUE.  Other
/// values are undefined.
///
typedef unsigned char       BOOLEAN;
///
/// 1-byte unsigned value.
///
typedef unsigned char       UINT8;
///
/// 1-byte Character
///
typedef char                CHAR8;
///
/// 1-byte signed value
///
typedef char                INT8;

typedef void                VOID;

typedef UINT64              EFI_PHYSICAL_ADDRESS;

typedef struct {
  UINT32  Data1;
  UINT16  Data2;
  UINT16  Data3;
  UINT8   Data4[8];
} EFI_GUID;

#define CONST     const
#define STATIC    static

#define TRUE  ((BOOLEAN)(1==1))
#define FALSE ((BOOLEAN)(0==1))

#define FSPAPI __attribute__((cdecl))
#define EFIAPI __attribute__((cdecl))

#define ASSERT(Expression)      \
  do {                          \
    if (!(Expression)) {        \
      for (;;);                 \
    }                           \
  } while (FALSE)

typedef UINT32 FSP_STATUS;
typedef UINT32 EFI_STATUS;

///
/// Compatiable with EFI_STATUS defined in PI Spec.
#define FSP_SUCCESS               0
#define FSP_INVALID_PARAMETER     0x80000002
#define FSP_UNSUPPORTED           0x80000003
#define FSP_DEVICE_ERROR          0x80000007
#define FSP_NOT_FOUND             0x8000000E
#define FSP_ALREADY_STARTED       0x80000014

/**
  Returns a 16-bit signature built from 2 ASCII characters.

  This macro returns a 16-bit value built from the two ASCII characters specified
  by A and B.

  @param  A    The first ASCII character.
  @param  B    The second ASCII character.

  @return A 16-bit value built from the two ASCII characters specified by A and B.

**/
#define SIGNATURE_16(A, B)        ((A) | (B << 8))

/**
  Returns a 32-bit signature built from 4 ASCII characters.

  This macro returns a 32-bit value built from the four ASCII characters specified
  by A, B, C, and D.

  @param  A    The first ASCII character.
  @param  B    The second ASCII character.
  @param  C    The third ASCII character.
  @param  D    The fourth ASCII character.

  @return A 32-bit value built from the two ASCII characters specified by A, B,
          C and D.

**/
#define SIGNATURE_32(A, B, C, D)  (SIGNATURE_16 (A, B) | (SIGNATURE_16 (C, D) << 16))

/**
  Returns a 64-bit signature built from 8 ASCII characters.

  This macro returns a 64-bit value built from the eight ASCII characters specified
  by A, B, C, D, E, F, G,and H.

  @param  A    The first ASCII character.
  @param  B    The second ASCII character.
  @param  C    The third ASCII character.
  @param  D    The fourth ASCII character.
  @param  E    The fifth ASCII character.
  @param  F    The sixth ASCII character.
  @param  G    The seventh ASCII character.
  @param  H    The eighth ASCII character.

  @return A 64-bit value built from the two ASCII characters specified by A, B,
          C, D, E, F, G and H.

**/
#define SIGNATURE_64(A, B, C, D, E, F, G, H) \
    (SIGNATURE_32 (A, B, C, D) | ((UINT64) (SIGNATURE_32 (E, F, G, H)) << 32))

#endif
