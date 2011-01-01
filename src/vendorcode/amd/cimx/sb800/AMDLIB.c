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

UINT8
getNumberOfCpuCores (
  OUT VOID
  )
{
  UINT8 Result;
  Result = 1;
  Result = ReadNumberOfCpuCores ();
  return Result;
}

UINT32
readAlink (
  IN       UINT32 Index
  )
{
  UINT32 Data;
  WriteIO (ALINK_ACCESS_INDEX, AccWidthUint32, &Index);
  ReadIO (ALINK_ACCESS_DATA, AccWidthUint32, &Data);
  //Clear Index
  Index = 0;
  WriteIO (ALINK_ACCESS_INDEX, AccWidthUint32, &Index);
  return Data;
}

VOID
writeAlink (
  IN       UINT32 Index,
  IN       UINT32 Data
  )
{
  WriteIO (ALINK_ACCESS_INDEX, AccWidthUint32 | S3_SAVE, &Index);
  WriteIO (ALINK_ACCESS_DATA, AccWidthUint32 | S3_SAVE, &Data);
  //Clear Index
  Index = 0;
  WriteIO (ALINK_ACCESS_INDEX, AccWidthUint32 | S3_SAVE, &Index);
}

VOID
rwAlink (
  IN       UINT32 Index,
  IN       UINT32 AndMask,
  IN       UINT32 OrMask
  )
{
  UINT32 AccesType;
  AccesType = Index & 0xE0000000;
  if (AccesType == (AXINDC << 29)) {
    writeAlink ((SB_AX_INDXC_REG30 | AccesType), Index & 0x1FFFFFFF);
    Index = (SB_AX_DATAC_REG34 | AccesType);
  } else if (AccesType == (AXINDP << 29)) {
    writeAlink ((SB_AX_INDXP_REG38 | AccesType), Index & 0x1FFFFFFF);
    Index = (SB_AX_DATAP_REG3C | AccesType);
  }
  writeAlink (Index, (readAlink (Index) & AndMask) | OrMask );
}

