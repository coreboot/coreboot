/*****************************************************************************
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
 ***************************************************************************/

#include "Porting.h"
#include "AGESA.h"
#include "amdlib.h"

#include <northbridge/amd/agesa/dimmSpd.h>

#define SMBUS_BASE_ADDR  0xB00

STATIC
VOID
WritePmReg (
  IN UINT8 Reg,
  IN UINT8 Data
  )
{
   __outbyte (0xCD6, Reg);
   __outbyte (0xCD7, Data);
}
STATIC
VOID
SetupFch (
  IN UINT16
  IN IoBase
  )
{
   WritePmReg (0x2D, IoBase >> 8);
   WritePmReg (0x2C, IoBase | 1);
   WritePmReg (0x29, 0x80);
   WritePmReg (0x28, 0x61);
   /* set SMBus clock to 400 KHz */
   __outbyte (IoBase + 0x0E, 66000000 / 400000 / 4);
}

/*
 *
 * ReadSmbusByteData - read a single SPD byte from any offset
 *
 */

STATIC
AGESA_STATUS
ReadSmbusByteData (
  IN UINT16 Iobase,
  IN UINT8  Address,
  OUT UINT8 *Buffer,
  IN UINTN  Offset
  )
{
   UINTN  Status;
   UINT64 Limit;

   Address |= 1; // set read bit

   __outbyte (Iobase + 0, 0xFF);                // clear error status
   __outbyte (Iobase + 1, 0x1F);                // clear error status
   __outbyte (Iobase + 3, Offset);              // offset in eeprom
   __outbyte (Iobase + 4, Address);             // slave address and read bit
   __outbyte (Iobase + 2, 0x48);                // read byte command

   /* time limit to avoid hanging for unexpected error status (should never happen) */
   Limit = __rdtsc () + 2000000000 / 10;
   for (;;) {
     Status = __inbyte (Iobase);
     if (__rdtsc () > Limit) break;
     if ((Status & 2) == 0) continue;               // SMBusInterrupt not set, keep waiting
     if ((Status & 1) == 1) continue;               // HostBusy set, keep waiting
     break;
   }

   Buffer [0] = __inbyte (Iobase + 5);
   if (Status == 2) Status = 0;                      // check for done with no errors
   return Status;
   }

/*
 *
 * ReadSmbusByte - read a single SPD byte from the default offset
 *                 this function is faster function readSmbusByteData
 *
 */

STATIC
AGESA_STATUS
ReadSmbusByte (
  IN UINT16 Iobase,
  IN UINT8  Address,
  OUT UINT8 *Buffer
  )
{
  UINTN   Status;
  UINT64  Limit;

  __outbyte (Iobase + 0, 0xFF);                // clear error status
  __outbyte (Iobase + 2, 0x44);                // read command

  // time limit to avoid hanging for unexpected error status
  Limit = __rdtsc () + 2000000000 / 10;
  for (;;) {
    Status = __inbyte (Iobase);
    if (__rdtsc () > Limit) break;
    if ((Status & 2) == 0) continue;               // SMBusInterrupt not set, keep waiting
    if ((Status & 1) == 1) continue;               // HostBusy set, keep waiting
    break;
  }

  Buffer [0] = __inbyte (Iobase + 5);
  if (Status == 2) Status = 0;                      // check for done with no errors
  return Status;
}

/*
 *
 * ReadSpd - Read one or more SPD bytes from a DIMM.
 *           Start with offset zero and read sequentially.
 *           Optimization relies on autoincrement to avoid
 *           sending offset for every byte.
 *           Reads 128 bytes in 7-8 ms at 400 KHz.
 *
 */

STATIC
AGESA_STATUS
ReadSpd (
  IN UINT16 IoBase,
  IN UINT8  SmbusSlaveAddress,
  OUT UINT8 *Buffer,
  IN UINTN  Count
  )
{
  UINTN Index, Status;

  /* read the first byte using offset zero */
  Status = ReadSmbusByteData (IoBase, SmbusSlaveAddress, Buffer, 0);
  if (Status) return Status;

  /* read the remaining bytes using auto-increment for speed */
  for (Index = 1; Index < Count; Index++){
    Status = ReadSmbusByte (IoBase, SmbusSlaveAddress, &Buffer [Index]);
    if (Status) return Status;
  }
  return 0;
}

int smbus_readSpd(int spdAddress, char *buf, size_t len)
{
   SetupFch (SMBUS_BASE_ADDR);
   return ReadSpd (SMBUS_BASE_ADDR, spdAddress, (UINT8 *) buf, len);
}
