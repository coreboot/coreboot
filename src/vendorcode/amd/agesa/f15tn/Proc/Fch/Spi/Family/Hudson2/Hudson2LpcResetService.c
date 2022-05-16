/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch LPC controller
 *
 * Init LPC Controller features.
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

#define FILECODE PROC_FCH_SPI_FAMILY_HUDSON2_HUDSON2LPCRESETSERVICE_FILECODE

/**
 * FchInitHudson2ResetLpcPciTable - Lpc (Spi) device registers
 * initial during the power on stage.
 *
 *
 *
 *
 */
CONST REG8_MASK FchInitHudson2ResetLpcPciTable[] =
{
  //
  // LPC Device (Bus 0, Dev 20, Func 3)
  //
  {0x00, LPC_BUS_DEV_FUN, 0},

  {FCH_LPC_REG48, 0x00, BIT0 + BIT1 + BIT2},
  {FCH_LPC_REG7C, 0x00, BIT0 + BIT2},
  {0x78 , 0xF0, BIT2 + BIT3},                       /// Enable LDRQ pin
  {FCH_LPC_REGBB, 0xFF, BIT3 + BIT4 + BIT5},
  //
  // Set 0xBB [5:3] = 111 to improve SPI timing margin.
  // Set 0xBA [6:5] = 11 improve SPI timing margin. (SPI Prefetch enhancement)
  //
  {FCH_LPC_REGBB, 0xBE, BIT0 + BIT3 + BIT4 + BIT5},
  {FCH_LPC_REGBA, 0x9F, BIT5 + BIT6},
  // Force EC_PortActive to 1 to fix possible IR non function issue when NO_EC_SUPPORT is defined
  {FCH_LPC_REGA4, (UINT8)~ BIT0, BIT0},
  {0xFF, 0xFF, 0xFF},
};

/**
 * FchInitResetLpcProgram - Config Lpc controller during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetLpcProgram (
  IN  VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  //
  // enable prefetch on Host, set LPC cfg 0xBB bit 0 to 1
  //
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGBA, AccessWidth16, 0xFFFF, BIT8, StdHeader);

  ProgramPciByteTable ( (REG8_MASK*) (&FchInitHudson2ResetLpcPciTable[0]),
                       ARRAY_SIZE(FchInitHudson2ResetLpcPciTable), StdHeader);

  //
  // Enabling ClkRun Function
  //
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGBB, AccessWidth8, 0xFB, BIT2, StdHeader);
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGD0, AccessWidth8, 0xFB, 0, StdHeader);
  //
  // LPC CLK0 Power-Down Function
  //
  if (!IsImcEnabled (StdHeader)) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGD2, AccessWidth8, 0xFF, BIT3);
  } else {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGD2, AccessWidth8, (UINT32)~ (BIT3), 0);
  }

  if ( LocalCfgPtr->LegacyFree ) {
    RwPci (((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG44), AccessWidth32, 00, 0x0003C000, StdHeader);
  } else {
    RwPci (((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG44), AccessWidth32, 00, 0xFF03FFD5, StdHeader);
  }
}
