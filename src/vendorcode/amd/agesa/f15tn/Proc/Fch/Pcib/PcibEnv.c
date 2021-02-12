/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Pcib controller
 *
 * Init Pcib Controller features.
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
#define FILECODE PROC_FCH_PCIB_PCIBENV_FILECODE

/**
 * FchInitEnvPcibPciTable - PCI device registers initial during
 * early POST.
 *
 */
REG8_MASK FchInitEnvPcibPciTable[] =
{
  //
  // PCIB Bridge (Bus 0, Dev 20, Func 4)
  //
  {0x00, PCIB_BUS_DEV_FUN, 0},
  {FCH_PCIB_REG40, 0xFF, BIT5},      /// PCI-bridge Subtractive Decode
  {FCH_PCIB_REG4B, 0xFF, BIT7},      ///
  {0x66 , 0xFF, BIT4},      /// Enabling One-Prefetch-Channel Mode, PCIB_PCI_config 0x64 [20]
  {0x65 , 0xFF, BIT7},      /// proper operation of CLKRUN#.
  {FCH_PCIB_REG0D, 0x00, 0x40},      /// Setting Latency Timers to 0x40, Enables the PCIB to retain ownership
  {FCH_PCIB_REG1B, 0x00, 0x40},      /// of the bus on the Primary side and on the Secondary side when GNT# is deasserted.
  {FCH_PCIB_REG66 + 1, 0xFF, BIT1},  /// Enable PCI bus GNT3#..
  {0xFF, 0xFF, 0xFF},
};

/**
 * FchInitEnvPcib - Config Pcib controller before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvPcib (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        VerbPciClks;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  //Early post initialization of pci config space
  //
  ProgramPciByteTable ((REG8_MASK*) (&FchInitEnvPcibPciTable[0]),
                       ARRAY_SIZE(FchInitEnvPcibPciTable), StdHeader);

  //
  //Disable or Enable PCI Clks based on input
  //
  VerbPciClks = ((LocalCfgPtr->Pcib.PciClks & 0x0F) << 2);
  RwPci ((PCIB_BUS_DEV_FUN << 16) + FCH_PCIB_REG42, AccessWidth8, (UINT32)~(BIT5 + BIT4 + BIT3 + BIT2), VerbPciClks, StdHeader);
  VerbPciClks = ((LocalCfgPtr->Pcib.PciClks & 0x10) >> 4);
  RwPci ((PCIB_BUS_DEV_FUN << 16) + 0x4A , AccessWidth8, (UINT32)~BIT0, VerbPciClks, StdHeader);
  //
  // PCIB MSI
  //
  if (LocalCfgPtr->Pcib.PcibMsiEnable) {
    RwPci ((PCIB_BUS_DEV_FUN << 16) + 0x40 , AccessWidth8, (UINT32)~BIT3, BIT3, StdHeader);
  }
}

