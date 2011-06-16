/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Hudson2 AB
 *
 * Init AB bridge.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 47163 $   @e \$Date: 2011-02-16 07:23:13 +0800 (Wed, 16 Feb 2011) $
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
#include "Filecode.h"
#define FILECODE PROC_FCH_PCIE_FAMILY_HUDSON2_HUDSON2ABRESETSERVICE_FILECODE


/**
 * FchProgramAbPowerOnReset - Config Ab Bridge during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchProgramAbPowerOnReset (
  IN  VOID     *FchDataPtr
  )
{
  UINT32       AbValue;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;
  UINT8                     EfuseValue;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  // Set A-Link bridge access address.
  // This is an I/O address. The I/O address must be on 16-byte boundary.
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGE0, AccessWidth32, 00, ALINK_ACCESS_INDEX);

  //
  // Enable Hudson-2 to issue memory read/write requests in the upstream direction
  //
  WriteAlink (0x80000004, 0x04, StdHeader);

  //
  // Disable the credit variable in the downstream arbitration equation
  //
  AbValue = ReadAlink (FCH_ABCFG_REG9C | (UINT32) (ABCFG << 29), StdHeader);
  AbValue = AbValue | BIT0;
  WriteAlink (FCH_ABCFG_REG9C | (UINT32) (ABCFG << 29), AbValue, StdHeader);

  //
  // AXINDC 0x10[9]=1, Enabling Non-Posted memory write for K8 platform.
  //
  WriteAlink (0x30, 0x10, StdHeader);
  WriteAlink (0x34, ReadAlink (0x34, StdHeader) | BIT9, StdHeader);

  RwAlink (FCH_ABCFG_REG10050 | (UINT32) (ABCFG << 29), ~BIT2, 0x00, StdHeader);

  //
  // Configure UMI target link speed
  //
  EfuseValue = PCIE_FORCE_GEN1_EFUSE_LOCATION;
  GetEfuseStatus (&EfuseValue, StdHeader);
  if ( EfuseValue & BIT0 ) {
    LocalCfgPtr->NbSbGen2 = 0;
  }

  EfuseValue = FCH_Variant_EFUSE_LOCATION;
  GetEfuseStatus (&EfuseValue, StdHeader);
  if ((EfuseValue == 0x07) || (EfuseValue == 0x08)) {
    LocalCfgPtr->NbSbGen2 = 0;
  }

  if (LocalCfgPtr->NbSbGen2) {
    AbValue = 2;
  } else {
    AbValue = 1;
  }
  RwAlink ((UINT32)FCH_AX_CFG_REG88, 0xFFFFFFF0, AbValue, StdHeader);

  if (LocalCfgPtr->NbSbGen2) {
    AbValue = BIT0;
  } else {
    AbValue = 0;
  }
  RwAlink (FCH_AX_INDXP_REGA4, 0xFFFFFFFE, AbValue, StdHeader);

}

