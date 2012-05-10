/**
 * @file
 *
 * Southbridge EC IO access common routine
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

// #ifndef NO_EC_SUPPORT

/*----------------------------------------------------------------------------------------*/
/**
 * EnterEcConfig - Force EC into Config mode
 *
 *
 *
 *
 */
VOID
EnterEcConfig (
  )
{
  UINT16   dwEcIndexPort;

  ReadPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGA4, AccWidthUint16 | S3_SAVE, &dwEcIndexPort);
  dwEcIndexPort &= ~(BIT0);
  RWIO (dwEcIndexPort, AccWidthUint8, 0x00, 0x5A);
}

/*----------------------------------------------------------------------------------------*/
/**
 * ExitEcConfig - Force EC exit Config mode
 *
 *
 *
 *
 */
VOID
ExitEcConfig (
  )
{
  UINT16   dwEcIndexPort;

  ReadPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGA4, AccWidthUint16 | S3_SAVE, &dwEcIndexPort);
  dwEcIndexPort &= ~(BIT0);
  RWIO (dwEcIndexPort, AccWidthUint8, 0x00, 0xA5);
}

/*----------------------------------------------------------------------------------------*/
/**
 * ReadEC8 - Read EC register data
 *
 *
 *
 * @param[in] Address  - EC Register Offset Value
 * @param[in] Value    - Read Data Buffer
 *
 */
VOID
ReadEC8 (
  IN       UINT8 Address,
  IN       UINT8* Value
  )
{
  UINT16   dwEcIndexPort;

  ReadPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGA4, AccWidthUint16 | S3_SAVE, &dwEcIndexPort);
  dwEcIndexPort &= ~(BIT0);
  WriteIO (dwEcIndexPort, AccWidthUint8, &Address);
  ReadIO (dwEcIndexPort + 1, AccWidthUint8, Value);
}

/*----------------------------------------------------------------------------------------*/
/**
 * WriteEC8 - Write date into EC register
 *
 *
 *
 * @param[in] Address  - EC Register Offset Value
 * @param[in] Value    - Write Data Buffer
 *
 */
VOID
WriteEC8 (
  IN       UINT8 Address,
  IN       UINT8* Value
  )
{
  UINT16   dwEcIndexPort;

  ReadPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGA4, AccWidthUint16 | S3_SAVE, &dwEcIndexPort);
  dwEcIndexPort &= ~(BIT0);

  WriteIO (dwEcIndexPort, AccWidthUint8, &Address);
  WriteIO (dwEcIndexPort + 1, AccWidthUint8, Value);
}

/*----------------------------------------------------------------------------------------*/
/**
 * RWEC8 - Read/Write EC register
 *
 *
 *
 * @param[in] Address    - EC Register Offset Value
 * @param[in] AndMask    - Data And Mask 8 bits
 * @param[in] OrMask     - Data OR Mask 8 bits
 *
 */
VOID
RWEC8 (
  IN       UINT8 Address,
  IN       UINT8 AndMask,
  IN       UINT8 OrMask
  )
{
  UINT8 Result;
  ReadEC8 (Address, &Result);
  Result = (Result & AndMask) | OrMask;
  WriteEC8 (Address, &Result);
}

// #endif

