/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH memory access lib
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision$   @e \$Date$
 *
 */
 /*****************************************************************************
 *
 * Copyright (c) 2008 - 2016, Advanced Micro Devices, Inc.
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
 ***************************************************************************/
#include "FchPlatform.h"
 /*
  * Header removed for coreboot.
  * #include  "Ids.h"
  */
#define FILECODE PROC_FCH_COMMON_MEMLIB_FILECODE


/**
 * ReadMem - Read FCH BAR Memory
 *
 * @param[in] Address    - Memory BAR address
 * @param[in] OpFlag     - Access width
 * @param[in] *ValuePtr  - In/Out value pointer
 *
 */
VOID
ReadMem (
  IN  UINT32     Address,
  IN  UINT8      OpFlag,
  IN  VOID       *ValuePtr
  )
{
  OpFlag = OpFlag & 0x7f;

  switch ( OpFlag ) {
  case AccessWidth8:
    *((UINT8*)ValuePtr) = *((volatile UINT8*) ((UINTN)Address));
    break;

  case AccessWidth16:
    *((UINT16*)ValuePtr) = *((volatile UINT16*) ((UINTN)Address));
    break;

  case AccessWidth32:
    *((UINT32*)ValuePtr) = *((volatile UINT32*) ((UINTN)Address));
    break;

  default:
    ASSERT (FALSE);
    break;
  }
}

/**
 * WriteMem - Write FCH BAR Memory
 *
 * @param[in] Address    - Memory BAR address
 * @param[in] OpFlag     - Access width
 * @param[in] *ValuePtr  - In/Out Value pointer
 *
 */
VOID
WriteMem (
  IN  UINT32     Address,
  IN  UINT8      OpFlag,
  IN  VOID       *ValuePtr
  )
{
  OpFlag = OpFlag & 0x7f;

  switch ( OpFlag ) {
  case AccessWidth8 :
    *((volatile UINT8*) ((UINTN)Address)) = *((UINT8*)ValuePtr);
    break;

  case AccessWidth16:
    *((volatile UINT16*) ((UINTN)Address)) = *((UINT16*)ValuePtr);
    break;

  case AccessWidth32:
    *((volatile UINT32*) ((UINTN)Address)) = *((UINT32*)ValuePtr);
    break;

  default:
    ASSERT (FALSE);
    break;
  }
}

/**
 * RwMem - Read & Write FCH BAR Memory
 *
 * @param[in] Address    - Memory BAR address
 * @param[in] OpFlag     - Access width
 * @param[in] Mask       - Mask Value of data
 * @param[in] Data       - Write data
 *
 */
VOID
RwMem (
  IN  UINT32     Address,
  IN  UINT8      OpFlag,
  IN  UINT32     Mask,
  IN  UINT32     Data
  )
{
  UINT32 Result;

  ReadMem (Address, OpFlag, &Result);
  Result = (Result & Mask) | Data;
  WriteMem (Address, OpFlag, &Result);
  ReadMem (Address, OpFlag, &Result);
}

