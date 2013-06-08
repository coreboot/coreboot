/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH Embedded Controller
 *
 * Init Ec Controller features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
****************************************************************************
*/
#include "FchPlatform.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_IMC_FCHECENV_FILECODE


/**
 * FchInitEnvEc - Config Ec controller before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvEc (
  IN  VOID     *FchDataPtr
  )
{
}

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
  IN AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT16   EcIndexPortDword;
  UINT8    FchEcData8;

  ReadPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGA4, AccessWidth16, &EcIndexPortDword, StdHeader);
  EcIndexPortDword &= ~(BIT0);
  FchEcData8 = 0x5A;
  LibAmdIoWrite (AccessWidth8, EcIndexPortDword, &FchEcData8, StdHeader);
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
  IN AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT16   EcIndexPortDword;
  UINT8    FchEcData8;

  ReadPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGA4, AccessWidth16, &EcIndexPortDword, StdHeader);
  EcIndexPortDword &= ~(BIT0);
  FchEcData8 = 0xA5;
  LibAmdIoWrite (AccessWidth8, EcIndexPortDword, &FchEcData8, StdHeader);
}

/*----------------------------------------------------------------------------------------*/
/**
 * ReadEc8 - Read EC register data
 *
 *
 *
 * @param[in] Address  - EC Register Offset Value
 * @param[in] Value    - Read Data Buffer
 * @param[in] StdHeader
 *
 */
VOID
ReadEc8 (
  IN  UINT8     Address,
  IN  UINT8     *Value,
  IN AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT16   EcIndexPortDword;

  ReadPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGA4, AccessWidth16, &EcIndexPortDword, StdHeader);
  EcIndexPortDword &= ~(BIT0);
  LibAmdIoWrite (AccessWidth8, EcIndexPortDword, &Address, StdHeader);
  LibAmdIoRead (AccessWidth8, EcIndexPortDword + 1, Value, StdHeader);
}

/*----------------------------------------------------------------------------------------*/
/**
 * RwEc8 - Read/Write EC register
 *
 *
 *
 * @param[in] Address    - EC Register Offset Value
 * @param[in] AndMask    - Data And Mask 8 bits
 * @param[in] OrMask     - Data OR Mask 8 bits
 * @param[in] StdHeader
 *
 */
VOID
RwEc8 (
  IN  UINT8     Address,
  IN  UINT8     AndMask,
  IN  UINT8     OrMask,
  IN AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8   Result;

  ReadEc8 (Address, &Result, StdHeader);
  Result = (Result & AndMask) | OrMask;
  WriteEc8 (Address, &Result, StdHeader);
}

/*----------------------------------------------------------------------------------------*/
/**
 * WriteEc8 - Write date into EC register
 *
 *
 *
 * @param[in] Address  - EC Register Offset Value
 * @param[in] Value    - Write Data Buffer
 * @param[in] StdHeader
 *
 */
VOID
WriteEc8 (
  IN  UINT8     Address,
  IN  UINT8     *Value,
  IN AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT16   EcIndexPortDword;

  ReadPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGA4, AccessWidth16, &EcIndexPortDword, StdHeader);
  EcIndexPortDword &= ~(BIT0);
  LibAmdIoWrite (AccessWidth8, EcIndexPortDword, &Address, StdHeader);
  LibAmdIoWrite (AccessWidth8, EcIndexPortDword + 1, Value, StdHeader);
}
