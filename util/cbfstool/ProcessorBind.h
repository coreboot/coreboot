/** @file
  Processor or Compiler specific defines and types for IA-32 architecture.

Copyright 2015 Google Inc.
Copyright (c) 2006 - 2013, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PROCESSOR_BIND_H__
#define __PROCESSOR_BIND_H__


/*
 * This to mimic a processor binding for EDK. This is just to provide the
 * processor types.
 */
#include <inttypes.h>

///
/// Define the processor type so other code can make processor based choices.
///
#define MDE_CPU_IA32

///
/// 8-byte unsigned value.
///
typedef uint64_t UINT64;
///
/// 8-byte signed value.
///
typedef int64_t INT64;
///
/// 4-byte unsigned value.
///
typedef uint32_t UINT32;
///
/// 4-byte signed value.
///
typedef int32_t INT32;
///
/// 2-byte unsigned value.
///
typedef uint16_t UINT16;
///
/// 2-byte Character.  Unless otherwise specified all strings are stored in the
/// UTF-16 encoding format as defined by Unicode 2.1 and ISO/IEC 10646 standards.
///
typedef uint16_t CHAR16;
///
/// 2-byte signed value.
///
typedef int16_t INT16;
///
/// Logical Boolean.  1-byte value containing 0 for FALSE or a 1 for TRUE.  Other
/// values are undefined.
///
typedef unsigned char BOOLEAN;
///
/// 1-byte unsigned value.
///
typedef unsigned char UINT8;
///
/// 1-byte Character
///
typedef char CHAR8;
///
/// 1-byte signed value
///
typedef signed char INT8;

///
/// Unsigned value of native width.  (4 bytes on supported 32-bit processor instructions;
/// 8 bytes on supported 64-bit processor instructions.)
///
typedef uintptr_t UINTN;
///
/// Signed value of native width.  (4 bytes on supported 32-bit processor instructions;
/// 8 bytes on supported 64-bit processor instructions.)
///
typedef intptr_t INTN;

//
// Processor specific defines
//

///
/// A value of native width with the highest bit set.
//  Not needed for non-runtime, but it shouldb
///
//#define MAX_BIT     0x80000000

// No API requirements as this is not for runtime.
#define EFIAPI

#endif
