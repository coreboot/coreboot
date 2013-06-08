/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Imc controller
 *
 * Init Imc Controller features.
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
#define FILECODE PROC_FCH_IMC_FAMILY_HUDSON2_HUDSON2IMCSERVICE_FILECODE

//
// Declaration of local functions
//


/**
 * SoftwareToggleImcStrapping - Software Toggle IMC Firmware Strapping.
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
SoftwareToggleImcStrapping (
  IN  VOID     *FchDataPtr
  )
{
  UINT8    ValueByte;
  UINT8    PortStatusByte;
  UINT32   AbValue;
  UINT32   ABStrapOverrideReg;
  AMD_CONFIG_PARAMS     *StdHeader;

  StdHeader = ((FCH_DATA_BLOCK *) FchDataPtr)->StdHeader;
  GetChipSysMode (&PortStatusByte, StdHeader);

  ReadPmio (FCH_PMIOA_REGBF, AccessWidth8, &ValueByte, StdHeader);

  //
  //if ( (ValueByte & (BIT6 + BIT7)) != 0xC0 ) {  // PwrGoodOut =1, PwrGoodEnB=1
  //The strapStatus register is not mapped into StrapOveride not in the same bit position. The following is difference.
  //
  //StrapStatus                               StrapOverride
  //   bit4                                            bit17
  //   bit6                                            bit12
  //   bit12                                           bit15
  //   bit15                                           bit16
  //   bit16                                           bit18
  //
  ReadMem ((ACPI_MMIO_BASE + MISC_BASE + 0x80 ), AccessWidth32, &AbValue);
  ABStrapOverrideReg = AbValue;

  if (AbValue & BIT4) {
    ABStrapOverrideReg = (ABStrapOverrideReg & ~BIT4) | BIT17;
  }

  if (AbValue & BIT6) {
    ABStrapOverrideReg = (ABStrapOverrideReg & ~BIT6) | BIT12;
  }

  if (AbValue & BIT12) {
    ABStrapOverrideReg = (ABStrapOverrideReg & ~BIT12) | BIT15;
  }

  if (AbValue & BIT15) {
    ABStrapOverrideReg = (ABStrapOverrideReg & ~BIT15) | BIT16;
  }

  if (AbValue & BIT16) {
    ABStrapOverrideReg = (ABStrapOverrideReg & ~BIT16) | BIT18;
  }

  ABStrapOverrideReg |= BIT31;                             /// Overwrite enable

  if ((PortStatusByte & ChipSysEcEnable) == 0) {
    ABStrapOverrideReg |= BIT2;                            /// bit2- EcEnableStrap
  } else {
    ABStrapOverrideReg &= ~BIT2;                           /// bit2=0 EcEnableStrap
  }

  WriteMem ((ACPI_MMIO_BASE + MISC_BASE + 0x84 ), AccessWidth32, &ABStrapOverrideReg);
  ValueByte |= (BIT6 + BIT7);                    /// PwrGoodOut =1, PwrGoodEnB=1
  WritePmio (FCH_PMIOA_REGBF, AccessWidth8, &ValueByte, StdHeader);

  ValueByte = 06;
  LibAmdIoWrite (AccessWidth8, 0xcf9, &ValueByte, StdHeader);
  FchStall (0xffffffff, StdHeader);
}

