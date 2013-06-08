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
#define FILECODE PROC_FCH_PCIB_PCIBLATE_FILECODE

/**
 * FchInitLatePcib - Prepare Pcib controller to boot to OS.
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLatePcib (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        Value;
  UINT8        NStBit;
  UINT8        NSBit;
  UINT32       VarDd;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  // We need to do the following setting in late post also because some bios core pci enumeration changes these values
  // programmed during early post.
  //  Master Latency Timer
  //
  Value = 0x40;
  WritePci ((PCIB_BUS_DEV_FUN << 16) + FCH_PCIB_REG0D, AccessWidth8, &Value, StdHeader);
  WritePci ((PCIB_BUS_DEV_FUN << 16) + FCH_PCIB_REG1B, AccessWidth8, &Value, StdHeader);

  //
  // CLKRUN#
  // FCH P2P AutoClock control settings.
  // VarDd = (FchDataPtr->PcibAutoClkCtrlLow) | (FchDataPtr->PcibAutoClkCtrlLow);
  //
  if ( LocalCfgPtr->Pcib.PcibClockRun ) {
    ReadMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG54, AccessWidth8, &Value);
    NStBit = Value & 0x03;
    NSBit = (Value & 0x3F ) >> 2;
    VarDd = (4 + (NStBit * 2) + (( 17 + NSBit) * 3) + 4) | 0x01;

    VarDd = 9; // for A12
    WritePci ((PCIB_BUS_DEV_FUN << 16) + FCH_PCIB_REG4C, AccessWidth32, &VarDd, StdHeader);
  }

  VarDd = (LocalCfgPtr->Pcib.PcibClkStopOverride);
  RwPci ((PCIB_BUS_DEV_FUN << 16) + 0x50 , AccessWidth16, 0x3F, (UINT16) (VarDd << 6), StdHeader);
}
