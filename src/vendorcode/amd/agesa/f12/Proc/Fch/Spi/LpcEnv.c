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
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
 *
 */
/*
*****************************************************************************
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
****************************************************************************
*/
#include "FchPlatform.h"
#define FILECODE PROC_FCH_SPI_LPCENV_FILECODE
/**
 * FchInitEnvLpcPciTable - PCI device registers initial during
 * early POST.
 *
 */
REG8_MASK FchInitEnvLpcPciTable[] =
{
  //
  // LPC Device (Bus 0, Dev 20, Func 3)
  //
  {0x00, LPC_BUS_DEV_FUN, 0},
  {FCH_LPC_REG40, ~BIT2, BIT2},                             /// Enabling LPC DMA Function  0x40[2]=1b 0x78[0]=0b
  {FCH_LPC_REG48, 0x00, BIT0 + BIT1 + BIT2},
  {FCH_LPC_REG78, 0xFC, 00},                                /// Enabling LPC DMA Function  0x40[2]=1b 0x78[0]=0b / Disables MSI capability
  {FCH_LPC_REGBB, ~BIT0, BIT0 + BIT3 + BIT4 + BIT5},        /// Enabled SPI Prefetch from HOST.
  {0xFF, 0xFF, 0xFF},
};

/**
 * FchInitEnvLpc - Config LPC controller before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvLpc (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  // LPC CFG programming
  //
  //
  // Turn on and configure LPC clock (48MHz)
  //
  RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x28, AccessWidth32, ~(BIT21 + BIT20 + BIT19), 2 << 19);
  RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG40, AccessWidth8, ~BIT7, 0);

  //
  // Initialization of pci config space
  //
  ProgramPciByteTable ((REG8_MASK*) (&FchInitEnvLpcPciTable[0]), sizeof (FchInitEnvLpcPciTable) / sizeof (REG8_MASK), StdHeader);

  //
  // SSID for LPC Controller
  //
  if (LocalCfgPtr->Spi.LpcSsid != NULL ) {
    RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG2C, AccessWidth32, 0x00, LocalCfgPtr->Spi.LpcSsid, StdHeader);
  }
  //
  // LPC MSI
  //
  if ( LocalCfgPtr->Spi.LpcMsiEnable ) {
    RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG78, AccessWidth32, ~BIT1, BIT1, StdHeader);
  }
}

