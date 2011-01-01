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
  UINT8 Temp;
  Temp = OpFlag;
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
