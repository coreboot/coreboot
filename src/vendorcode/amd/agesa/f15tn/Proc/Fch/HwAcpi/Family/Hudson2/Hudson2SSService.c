/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch HwAcpi controller
 *
 * Init Spread Spectrum features.
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
#include "amdlib.h"
#include "cpuServices.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_HWACPI_FAMILY_HUDSON2_HUDSON2SSSERVICE_FILECODE

/**
 * FchInitResetAcpiMmioTable - Fch ACPI MMIO initial
 * during the power on stage.
 *
 *
 *
 *
 */
CONST ACPI_REG_WRITE FchInitResetAcpiMmioTable[] =
{
  {00, 00, 0xB0, 0xAC},                                         /// Signature
  {MISC_BASE >> 8,  FCH_MISC_REG41, 0x1F, 0x40}, //keep Auxiliary_14Mclk_Sel [12]
  //
  // USB 3.0 Reference Clock MISC_REG 0x40 [4] = 0 Enable spread-spectrum reference clock.
  //
  {MISC_BASE >> 8,  FCH_MISC_REG40, 0xEF, 0x00},

  {PMIO_BASE >> 8,  0x5D , 0x00, BIT0},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGD2, 0xCF, BIT4 + BIT5},
  {SMBUS_BASE >> 8, FCH_SMBUS_REG12, 0x00, BIT0},
  {PMIO_BASE >> 8,  0x28 , 0xFF, BIT0 + BIT2},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG44 + 3, 0x67, 0},            /// Stop Boot timer
  {PMIO_BASE >> 8,  FCH_PMIOA_REG48, 0xFF, BIT0},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG00, 0xFF, 0x0E},
  {PMIO_BASE >> 8,  0x00  + 2, 0xFF, 0x40},
  {PMIO_BASE >> 8,  0x00  + 3, 0xFF, 0x08},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG34, 0xEF, BIT0 + BIT1},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGEC, 0xFD, BIT1},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG08, 0xFE, BIT2 + BIT4},
  {PMIO_BASE >> 8,  0x04  + 1, 0xFF, BIT0},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG54, 0x00, BIT4 + BIT6 + BIT7},
  {PMIO_BASE >> 8,  0x04  + 3, 0xFD, BIT1},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG74, 0xF6, BIT0 + BIT3},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGF0, (UINT8)~BIT2, 0x00},

  //
  // GEC I/O Termination Setting
  // PM_Reg 0xF6 = Power-on default setting
  // PM_Reg 0xF7 = Power-on default setting
  // PM_Reg 0xF8 = 0x6C
  // PM_Reg 0xF9 = 0x21
  // PM_Reg 0xFA = 0x00 Hudson-2 A12 GEC I/O Pad settings for 3.3V CMOS
  //
  {PMIO_BASE >> 8, FCH_PMIOA_REGF8,     0x00, 0x6C},
  {PMIO_BASE >> 8, FCH_PMIOA_REGF8 + 1, 0x00, 0x07},
  {PMIO_BASE >> 8, FCH_PMIOA_REGF8 + 2, 0x00, 0x00},
  //
  // GEC -end
  //

  {PMIO_BASE >> 8, FCH_PMIOA_REGC4, 0xee, 0x04},                 /// Release NB_PCIE_RST
  {PMIO_BASE >> 8, FCH_PMIOA_REGC0 + 2, 0xBF, 0x40},
  {PMIO_BASE >> 8, FCH_PMIOA_REGBE, 0xDF, BIT5},

  //
  // Enabling ClkRun Function
  //
  {PMIO_BASE >> 8,  FCH_PMIOA_REGBB, 0xFF, BIT2},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGD0, (UINT8)~BIT2, 0},

  {0xFF, 0xFF, 0xFF, 0xFF},
};

/**
 * ProgramFchHwAcpiResetP  - Config SpreadSpectrum before PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ProgramFchHwAcpiResetP (
  IN VOID  *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = &((AMD_RESET_PARAMS *)FchDataPtr)->StdHeader;

  RwPmio (0xD3, AccessWidth8, (UINT32)~BIT4, 0, StdHeader);
  RwPmio (0xD3, AccessWidth8, (UINT32)~BIT4, BIT4, StdHeader);

  if ( LocalCfgPtr->Cg2Pll == 1 ) {
    TurnOffCG2 ();
    LocalCfgPtr->SataClkMode = 0x0a;
  }
}
