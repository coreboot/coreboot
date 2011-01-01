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
ReadMEM (
  IN       UINT32 Address,
  IN       UINT8 OpFlag,
  IN       VOID* Value
  )
{
  OpFlag = OpFlag & 0x7f;
  switch ( OpFlag ) {
  case AccWidthUint8:
    *((UINT8*)Value) = *((UINT8*) ((UINTN)Address));
    break;
  case AccWidthUint16:
    //*((UINT16*)Value) = *((UINT16*) ((UINTN)Address)); //gcc break strict-aliasing rules
    *((UINT8*)Value) = *((UINT8*) ((UINTN)Address));
    *((UINT8*)Value + 1) = *((UINT8*)((UINTN)Address) + 1);
    break;
  case AccWidthUint32:
    *((UINT32*)Value) = *((UINT32*) ((UINTN)Address));
    break;
  }
}

VOID
WriteMEM (
  IN       UINT32 Address,
  IN       UINT8 OpFlag,
  IN       VOID* Value
  )
{
  OpFlag = OpFlag & 0x7f;
  switch ( OpFlag ) {
  case AccWidthUint8 :
    *((UINT8*) ((UINTN)Address)) = *((UINT8*)Value);
    break;
  case AccWidthUint16:
    //*((UINT16*) ((UINTN)Address)) = *((UINT16*)Value); //gcc break strict-aliasing rules
    *((UINT8*)((UINTN)Address)) = *((UINT8*)Value);
    *((UINT8*)((UINTN)Address) + 1) = *((UINT8*)Value + 1);
    break;
  case AccWidthUint32:
    *((UINT32*) ((UINTN)Address)) = *((UINT32*)Value);
    break;
  }
}

VOID
RWMEM (
  IN       UINT32 Address,
  IN       UINT8 OpFlag,
  IN       UINT32 Mask,
  IN       UINT32 Data
  )
{
  UINT32 Result;
  ReadMEM (Address, OpFlag, &Result);
  Result = (Result & Mask) | Data;
  WriteMEM (Address, OpFlag, &Result);
}


