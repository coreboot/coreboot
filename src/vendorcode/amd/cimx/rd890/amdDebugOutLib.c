/**
 * @file
 *
 * Debug out functions.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      Common Library
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 ***************************************************************************/
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "amdDebugOutLib.h"
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define COM_BASE_ADDRESS  0x3f8
#define DIVISOR           115200
#define LF                0x0a
#define CR                0x0d

typedef CHAR8   *va_list;
#ifndef _INTSIZEOF
#define _INTSIZEOF(n)( (sizeof(n) + sizeof(UINTN) - 1) & ~(sizeof(UINTN) - 1) )
#endif
#ifndef va_start
#define va_start(ap, v)  ( ap = (va_list)&(v) + _INTSIZEOF(v) )
#endif
#ifndef va_arg
#define va_arg(ap, t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#endif
#ifndef  va_end
#define va_end(ap)      ( ap = (va_list)0 )
#endif

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
  UINT8 Index;
  CHAR8 Buffer[256];
} StringBuffer;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
SendByteToBuffer (
  IN  OUT  StringBuffer  *Buffer,
  IN       CHAR8         Data
  );

VOID
SendStringToBuffer (
     OUT  StringBuffer  *Buffer,
  IN      CHAR8         *pstr
  );

VOID
SendBufferToDebugOut (
  IN      CHAR8*  Buffer
  );

VOID
ItoA (
  IN      UINT32  Value,
  IN      UINTN   Radix,
     OUT  CHAR8   *pstr
  );

VOID
SendBufferToHdtOut (
  IN      CHAR8*  Buffer
  );

VOID
SendBufferToSerialOut (
  IN      CHAR8*  Buffer
  );

VOID
InitDebugOut (VOID);

VOID
InitSerialOut (VOID);

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------*/
/**
 * Send format string to debug out
 *
 *
 *
 * @param[in] pConfig
 *
 */

VOID
LibAmdTraceDebug (
  IN       UINT32 Level,
  IN       CHAR8 *Format,
  IN       ...
  )
{
  CHAR8         TemBuffer[16];
  UINT8         Index;
  StringBuffer  Buffer;
  va_list       ArgList;
  if (Level == 0) {
    return;
  }
  Buffer.Index = 0;
  Index = 1;
  va_start (ArgList, Format);
  while (Index != 0) {
    if (*Format == 0) break;
    if (*Format == '%') {
      INT32 Radix;
      Radix = 0;
      if (*(Format + 1) == 'd' || *(Format + 1) == 'D') {
        Radix = 10;
      }
        if (*(Format + 1) == 'x' || *(Format + 1) == 'X' ) {
        Radix = 16;
      }
      if (Radix != 0) {
        ItoA (va_arg (ArgList, INT32), Radix, TemBuffer);
        SendStringToBuffer (&Buffer, TemBuffer);
        Format += 2;
        continue;
      }
    }
    SendByteToBuffer (&Buffer, *Format);
    if (*(Format) == 0x0a) SendByteToBuffer (&Buffer, 0x0d);
    Format++;
  }
  SendBufferToDebugOut (&Buffer.Buffer[0]);
  va_end (ArgList);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Write string to message buffer
 *
 *
 *
 * @param[in] pConfig
 *
 */

VOID
SendStringToBuffer (
     OUT   StringBuffer  *Buffer,
  IN       CHAR8         *pstr
  )
{
  while (*pstr != 0) {
    SendByteToBuffer (Buffer, *pstr);
    pstr++;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write byte to message buffer
 *
 *
 *
 * @param[in] pConfig
 *
 */

VOID
SendByteToBuffer (
  IN OUT   StringBuffer  *Buffer,
  IN       CHAR8         Data
  )
{
  if (Buffer->Index < 255) {
    Buffer->Buffer[Buffer->Index] = Data;
    Buffer->Buffer[++Buffer->Index] = 0;
  } else {
    SendBufferToDebugOut (Buffer->Buffer);
    Buffer->Index = 0;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Integer To String
 *
 *
 *
 * @param[in] pConfig
 *
 */

VOID
ItoA (
  IN       UINT32  Value,
  IN       UINTN   Radix,
     OUT   CHAR8   *pstr
  )
{
  CHAR8 *tsptr;
  CHAR8 *rsptr;
  CHAR8  ch1;
  CHAR8  ch2;
  UINTN  Reminder;

  tsptr = pstr;
  rsptr = pstr;
//Create String
  do {
    Reminder = Value % Radix;
    Value = Value / Radix;
    if (Reminder < 0xa) {
      *tsptr = (UINT8)Reminder + '0';
    } else {
      *tsptr = (UINT8)Reminder - 0xa + 'a';
    }
    tsptr++;
  } while (Value != 0);
//Reverse String
  *tsptr = 0;
  tsptr--;
  while (tsptr > rsptr) {
    ch1 = *tsptr;
    ch2 = *rsptr;
    *rsptr = ch1;
    *tsptr = ch2;
    tsptr--;
    rsptr++;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init debug Output
 *
 *
 *
 * @param[in] pConfig
 *
 */
VOID
InitDebugOut (
  VOID
  )
{
#ifdef  SERIAL_OUT_SUPPORT
  InitSerialOut ();
#endif
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init Serial Output
 *
 *
 *
 * @param[in] pConfig
 *
 */
VOID
InitSerialOut (
  VOID
  )
{
  UINT8   Data;
  UINT16  Divisor;

  Data = 0x87;
  LibAmdIoWrite (AccessWidth8, COM_BASE_ADDRESS + 0x3, &Data, NULL);
  Divisor = 115200 / DIVISOR;
  Data = (UINT8) (Divisor & 0xFF);
  LibAmdIoWrite (AccessWidth8 , COM_BASE_ADDRESS + 0x00, &Data, NULL);
  Data = (UINT8) (Divisor >> 8);
  LibAmdIoWrite (AccessWidth8, COM_BASE_ADDRESS + 0x01, &Data, NULL);
  Data = 0x07;
  LibAmdIoWrite (AccessWidth8, COM_BASE_ADDRESS + 0x3, &Data, NULL);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init HDT Output
 *
 *
 *
 * @param[in] pConfig
 *
 */


/*----------------------------------------------------------------------------------------*/
/**
 * Send Buffer to debug Output
 *
 *
 *
 * @param[in] pConfig
 *
 */
VOID
SendBufferToDebugOut (
  IN       CHAR8*  Buffer
  )
{
#ifdef  HDT_OUT_SUPPORT
  SendBufferToHdtOut (Buffer);
#endif

#ifdef  SERIAL_OUT_SUPPORT
  SendBufferToSerialOut (Buffer);
#endif
}

#ifdef  HDT_OUT_SUPPORT
/*----------------------------------------------------------------------------------------*/
/**
 * Send Buffer to debug Output
 *
 *
 *
 * @param[in] pConfig
 *
 */
VOID
SendBufferToHdtOut (
  IN       CHAR8*  Buffer
  )
{
  UINT32 Dr0Reg;
  UINT32 Dr7Reg;
  UINT32 Cr4Reg;
  UINT64 MsrCurrentValue;
  UINT64 MsrNewValue;

  // Save the CPU debug registers for restoration at the end of the routine
  LibAmdMsrRead (0xC001100A, &MsrCurrentValue, NULL);
  LibAmdReadCpuReg (DR0_REG, &Dr0Reg);
  LibAmdReadCpuReg (DR7_REG, &Dr7Reg);
  LibAmdReadCpuReg (CR4_REG, &Cr4Reg);

  //Modify the registers for HDT out
  LibAmdWriteCpuReg (DR0_REG, 0x8F0);
  LibAmdWriteCpuReg (DR7_REG, 0x20402);
  LibAmdWriteCpuReg (CR4_REG, Cr4Reg | 0x8);
  MsrNewValue = MsrCurrentValue | BIT0;
  LibAmdMsrWrite (0xC001100A, &MsrNewValue, NULL);

  //HDT out
  LibAmdIoWrite (AccessWidth32, 0x8F0, &Buffer, NULL);

  // Restore the CPU debug registers
  LibAmdWriteCpuReg (CR4_REG, Cr4Reg);
  LibAmdWriteCpuReg (DR7_REG, Dr7Reg);
  LibAmdWriteCpuReg (DR0_REG, Dr0Reg);
  LibAmdMsrWrite (0xC001100A, &MsrCurrentValue, NULL);
}
#endif
/*----------------------------------------------------------------------------------------*/
/**
 * Send Buffer to debug Output
 *
 *
 *
 * @param[in] pConfig
 *
 */
VOID
SendBufferToSerialOut (
  IN       CHAR8*  Buffer
  )
{
  UINT8   Status;
  UINT32  Count;

  Count = 10000;
  while (*Buffer != 0) {
    do {
      LibAmdIoRead (AccessWidth8, COM_BASE_ADDRESS + 0x05, &Status, NULL);
      if (Status == 0xff) return;
      // Loop  port is ready
    } while ((Status & 0x20) == 0 && (--Count) != 0);
    LibAmdIoWrite (AccessWidth8, COM_BASE_ADDRESS + 0x00, Buffer, NULL);
    Buffer++;
  }
}
