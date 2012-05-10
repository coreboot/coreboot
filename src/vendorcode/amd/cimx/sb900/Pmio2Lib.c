/**
 * @file
 *
 * Southbridge PMIO2 access common routine
 *
 */
/*;********************************************************************************
;
; Copyright (c) 2011, Advanced Micro Devices, Inc.
; All rights reserved.
; 
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
;     * Neither the name of Advanced Micro Devices, Inc. nor the names of 
;       its contributors may be used to endorse or promote products derived 
;       from this software without specific prior written permission.
; 
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; 
;*********************************************************************************/

#include "SbPlatform.h"
#include "cbtypes.h"


/*----------------------------------------------------------------------------------------*/
/**
 * Read PMIO2
 *
 *
 *
 * @param[in] Address  - PMIO2 Offset value
 * @param[in] OpFlag   - Access sizes
 * @param[in] Value    - Read Data Buffer
 *
 */
VOID
ReadPMIO2 (
  IN       UINT8 Address,
  IN       UINT8 OpFlag,
  IN       VOID* Value
  )
{
  UINT8 i;
  OpFlag = OpFlag & 0x7f;

  if ( OpFlag == 0x02 ) {
    OpFlag = 0x03;
  }
  for ( i = 0; i <= OpFlag; i++ ) {
    WriteIO (0xCD0, AccWidthUint8, &Address);                   // SB_IOMAP_REGCD0
    Address++;
    ReadIO (0xCD1, AccWidthUint8, (UINT8 *) Value + i);         // SB_IOMAP_REGCD1
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write PMIO 2
 *
 *
 *
 * @param[in] Address  - PMIO2 Offset value
 * @param[in] OpFlag   - Access sizes
 * @param[in] Value    - Write Data Buffer
 *
 */
VOID
WritePMIO2 (
  IN       UINT8 Address,
  IN       UINT8 OpFlag,
  IN       VOID* Value
  )
{
  UINT8 i;
  OpFlag = OpFlag & 0x7f;

  if ( OpFlag == 0x02 ) {
    OpFlag = 0x03;
  }
  for ( i = 0; i <= OpFlag; i++ ) {
    WriteIO (0xCD0, AccWidthUint8, &Address);                   // SB_IOMAP_REGCD0
    Address++;
    WriteIO (0xCD1, AccWidthUint8, (UINT8 *)Value + i);            // SB_IOMAP_REGCD1
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * RWPMIO2 - Read/Write PMIO2
 *
 *
 *
 * @param[in] Address    - PMIO2 Offset value
 * @param[in] OpFlag     - Access sizes
 * @param[in] AndMask    - Data And Mask 32 bits
 * @param[in] OrMask     - Data OR Mask 32 bits
 *
 */
VOID
RWPMIO2 (
  IN       UINT8 Address,
  IN       UINT8  OpFlag,
  IN       UINT32 AndMask,
  IN       UINT32 OrMask
  )
{
  UINT32 Result;
  OpFlag = OpFlag & 0x7f;
  ReadPMIO2 (Address, OpFlag, &Result);
  Result = (Result & AndMask) | OrMask;
  WritePMIO2 (Address, OpFlag, &Result);
}
