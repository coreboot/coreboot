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
ReadIO (
  IN       UINT16 Address,
  IN       UINT8 OpFlag,
  IN       VOID* Value
  )
{
  OpFlag = OpFlag & 0x7f;
  switch ( OpFlag ) {
  case AccWidthUint8:
    *(UINT8*)Value = ReadIo8 (Address);
    break;
  case AccWidthUint16:
    *(UINT16*)Value = ReadIo16 (Address);
    break;
  case AccWidthUint32:
    *(UINT32*)Value = ReadIo32 (Address);
    break;
  }
}

VOID
WriteIO (
  IN       UINT16 Address,
  IN       UINT8 OpFlag,
  IN       VOID* Value
  )
{
  OpFlag = OpFlag & 0x7f;
  switch ( OpFlag ) {
  case AccWidthUint8:
    WriteIo8 (Address, *(UINT8*)Value);
    break;
  case AccWidthUint16:
    WriteIo16 (Address, *(UINT16*)Value);
    break;
  case AccWidthUint32:
    WriteIo32 (Address, *(UINT32*)Value);
    break;
  }
}

VOID
RWIO (
  IN       UINT16 Address,
  IN       UINT8 OpFlag,
  IN       UINT32 Mask,
  IN       UINT32 Data
  )
{
  UINT32 Result;
  ReadIO (Address, OpFlag, &Result);
  Result = (Result & Mask) | Data;
  WriteIO (Address, OpFlag, &Result);
}
