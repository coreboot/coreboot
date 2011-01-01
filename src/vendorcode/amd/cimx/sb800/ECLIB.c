/**
 * @file
 *
 * Southbridge EC IO access common routine
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

