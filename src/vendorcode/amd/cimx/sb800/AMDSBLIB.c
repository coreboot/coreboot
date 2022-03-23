/**
 * @file
 *
 * Southbridge IO access common routine
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-SB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
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

#include "SBPLATFORM.h"
#include "cbtypes.h"

/*----------------------------------------------------------------------------------------*/
/**
 * SbStall - Delay routine
 *
 *
 *
 * @param[in] uSec
 *
 */
VOID
SbStall (
  IN       UINT32 uSec
  )
{
  UINT16 timerAddr;
  UINT32 startTime;
  UINT32 elapsedTime;

  ReadMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG64, AccWidthUint16, &timerAddr);
  if ( timerAddr == 0 ) {
    uSec = uSec / 2;
    while ( uSec != 0 ) {
      ReadIO (0x80, AccWidthUint8, (UINT8 *) (&startTime));
      uSec--;
    }
  } else {
    ReadIO (timerAddr, AccWidthUint32, &startTime);
    for ( ;; ) {
      ReadIO (timerAddr, AccWidthUint32, &elapsedTime);
      if ( elapsedTime < startTime ) {
        elapsedTime = elapsedTime + 0xFFFFFFFF - startTime;
      } else {
        elapsedTime = elapsedTime - startTime;
      }
      if ( (elapsedTime * 28 / 100) > uSec ) {
        break;
      }
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * SbReset - Generate a reset command
 *
 *
 *
 * @param[in] OpFlag - Dummy
 *
 */
VOID
SbReset (
  IN       UINT8 OpFlag
  )
{
  RWIO (0xcf9, AccWidthUint8, 0x0, 0x06);
}

/*----------------------------------------------------------------------------------------*/
/**
 * outPort80 - Send data to PORT 80 (debug port)
 *
 *
 *
 * @param[in] pcode - debug code (32 bits)
 *
 */
VOID
outPort80 (
  IN       UINT32 pcode
  )
{
  WriteIO (0x80, AccWidthUint8, &pcode);
  return;
}

/**
 * AmdSbCopyMem - Memory copy
 *
 * @param[in] pDest - Destance address point
 * @param[in] pSource - Source Address point
 * @param[in] Length - Data length
 *
 */
VOID
AmdSbCopyMem (
  IN      VOID*   pDest,
  IN      VOID*   pSource,
  IN      UINTN   Length
  )
{
  UINTN  i;
  UINT8  *Ptr;
  UINT8  *Source;
  Ptr = (UINT8*)pDest;
  Source = (UINT8*)pSource;
  for (i = 0; i < Length; i++) {
    *Ptr = *Source;
    Source++;
    Ptr++;
  }
}
