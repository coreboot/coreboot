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

#define COM_BASE_ADDRESS   0x3f8
#define DIVISOR            115200
#define LF                 0x0a
#define CR                 0x0d


#ifdef CIM_DEBUG

VOID SendBytePort (IN UINT8 Data);
VOID SendStringPort (IN CHAR8* pstr);
VOID ItoA (IN UINT32 Value, IN UINT32 Radix, IN CHAR8* pstr);

#ifndef CIM_DEBUG_LEVEL
 #define CIM_DEBUG_LEVEL 0xf
#endif

VOID
TraceCode (
  IN       UINT32 Level,
  IN       UINT32 Code
  )
{
  if ( ! (Level & CIM_DEBUG_LEVEL) ) {
    return;
  }
#if CIM_DEBUG & 1
  if ( Code != 0xFF ) {
    WriteIO (0x80, AccWidthUint8, &Code);
  }
#endif
}


VOID
TraceDebug (
  IN       UINT32 Level,
  IN       CHAR8 *Format,
  )
{
  CHAR8 temp[16];
  va_list  ArgList;
  UINT32 Radix;

  if ( ! (Level & CIM_DEBUG_LEVEL) ) {
    return;
  }

#if CIM_DEBUG & 2
  ArgList = va_start (ArgList, Format);
  Format = (CHAR8*) (Format);
  while ( 1 ) {
    if ( *Format == 0 ) {
      break;
    }
    if ( *Format == ' % ' ) {
      Radix = 0;
      if ( *(Format + 1) == 's' || *(Format + 1) == 'S' ) {
        SendStringPort ((CHAR8*) (va_arg (ArgList, CHAR8*)));
        Format += 2;
        continue;
      }

      if ( *(Format + 1) == 'd' || *(Format + 1) == 'D' ) {
        Radix = 10;
      }
      if ( *(Format + 1) == 'x' || *(Format + 1) == 'X' ) {
        Radix = 16;
      }
      if ( Radix ) {
        ItoA (va_arg (ArgList, intqq), Radix, temp);
        SendStringPort (temp);
        Format += 2;
        continue;
      }
    }
    SendBytePort (*Format);
    if ( *(Format) == 0x0a ) {
      SendBytePort (0x0d);
    }
    Format++;
  }
  va_end (ArgList);
#endif
}


VOID
ItoA (
  IN       UINT32 Value,
  IN       UINT32 Radix,
  IN       CHAR8* pstr
  )
{
  CHAR8* tsptr;
  CHAR8* rsptr;
  CHAR8  ch1;
  CHAR8  ch2;
  UINT32 Reminder;
  tsptr = pstr;
  rsptr = pstr;
//Create String
  do {
    Reminder = Value % Radix;
    Value = Value / Radix;
    if ( Reminder < 0xa ) {
      *tsptr = Reminder + '0';
    } else {
      *tsptr = Reminder - 0xa + 'a';
    }
    tsptr++;
  } while ( Value );
//Reverse String
  *tsptr = 0;
  tsptr--;
  while ( tsptr > rsptr ) {
    ch1 = *tsptr;
    ch2 = *rsptr;
    *rsptr = ch1;
    *tsptr = ch2;
    tsptr--;
    rsptr++;
  }
}

VOID
InitSerialOut (
  )
{
  UINT8 Data;
  UINT16   Divisor;
  Data = 0x87;
  WriteIO (COM_BASE_ADDRESS + 0x3, AccWidthUint8, &Data);
  Divisor = 115200 / DIVISOR;
  Data = Divisor & 0xFF;
  WriteIO (COM_BASE_ADDRESS + 0x00, AccWidthUint8, &Data);
  Data = Divisor >> 8;
  WriteIO (COM_BASE_ADDRESS + 0x01, AccWidthUint8, &Data);
  Data = 0x07;
  WriteIO (COM_BASE_ADDRESS + 0x3, AccWidthUint8, &Data);
}


VOID
SendStringPort (
  IN       CHAR8* pstr
  )
{

  while ( *pstr! = 0 ) {
    SendBytePort (*pstr);
    pstr++;
  }
}

VOID
SendBytePort (
  IN       UINT8 Data
  )
{
  INT8   Count;
  UINT8 Status;
  Count = 80;
  do {
    ReadIO ((COM_BASE_ADDRESS + 0x05), AccWidthUint8, &Status);
    if ( Status == 0xff ) {
      break;
    }
    // Loop  port is ready
  } while ( (Status & 0x20) == 0 && (--Count) != 0 );
  WriteIO (COM_BASE_ADDRESS + 0x00, AccWidthUint8, &Data);
}
#endif
