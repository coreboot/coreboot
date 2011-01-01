/*
 *****************************************************************************
 *
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 * ***************************************************************************
 *
 */


#include "SBPLATFORM.h"

VOID
ReadPCI (
  IN       UINT32 Address,
  IN       UINT8 OpFlag,
  IN       VOID* Value
  )
{
  OpFlag = OpFlag & 0x7f;

  if ( (UINT16)Address < 0xff ) {
    //Normal Config Access
    UINT32 AddrCf8;
    AddrCf8 = (1 << 31) + ((Address >> 8) & 0x0FFFF00) + (Address & 0xFC);
    WriteIO (0xCf8, AccWidthUint32, &AddrCf8);
    ReadIO ((UINT16) (0xCfC + (Address & 0x3)), OpFlag, Value);
  }
}

VOID
WritePCI (
  IN       UINT32 Address,
  IN       UINT8 OpFlag,
  IN       VOID* Value
  )
{
  OpFlag = OpFlag & 0x7f;
  if ( (UINT16)Address < 0xff ) {
    //Normal Config Access
    UINT32 AddrCf8;
    AddrCf8 = (1 << 31) + ((Address >> 8)&0x0FFFF00) + (Address & 0xFC);
    WriteIO (0xCf8, AccWidthUint32, &AddrCf8);
    WriteIO ((UINT16) (0xCfC + (Address & 0x3)), OpFlag, Value);
  }
}

VOID
RWPCI (
  IN       UINT32 Address,
  IN       UINT8 OpFlag,
  IN       UINT32 Mask,
  IN       UINT32 Data
  )
{
  UINT32 Result;
  Result = 0;
  OpFlag = OpFlag & 0x7f;
  ReadPCI (Address, OpFlag, &Result);
  Result = (Result & Mask) | Data;
  WritePCI (Address, OpFlag, &Result);
}
