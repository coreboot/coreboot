/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch SD controller
 *
 * Init SD Controller features.
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
#define FILECODE PROC_FCH_SD_SDENV_FILECODE

/**
 * FchInitEnvSd - Config SD controller before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvSd (
  IN  VOID     *FchDataPtr
  )
{
  UINT8                  SdData;
  UINT32                 SdData32;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  // SD Configuration
  //
  if ( LocalCfgPtr->Sd.SdConfig != SdDisable) {
    RwMem (ACPI_MMIO_BASE + MISC_BASE +  FCH_MISC_REG41, AccessWidth8, 0xF1, 0x48);
    RwMem (ACPI_MMIO_BASE + MISC_BASE +  FCH_MISC_REG42, AccessWidth8, 0xFE, 0x00);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE +  FCH_PMIOA_REGE7, AccessWidth8, 0x00, 0x12);
    //
    // INT#A SD resource
    //
    SdData = 0x97;                                 /// Azalia APIC index
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC00, &SdData, StdHeader);
    SdData = 0x10;                                 /// IRQ16 (INTA#)
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC01, &SdData, StdHeader);

    ReadPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGA4, AccessWidth32, &SdData32, StdHeader);
    SdData32 |= BIT31 + BIT24 + BIT18 + BIT16;     ///ADMA

    if ( LocalCfgPtr->Sd.SdConfig == SdDma) {
      SdData32 &=  ~(BIT16 + BIT24);               ///DMA
    } else if ( LocalCfgPtr->Sd.SdConfig == SdPio) {
      SdData32 &=  ~(BIT16 + BIT18 + BIT24);       ///PIO
    }

    SdData32 &= ~(BIT17 + BIT23);                  ///clear bitwidth
    SdData32 |= (LocalCfgPtr->Sd.SdSpeed << 17) + (LocalCfgPtr->Sd.SdBitWidth << 23);
    RwPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGA4, AccessWidth32, 0, SdData32, StdHeader);

    // SD: Some SD cards cannot be detected in HIGH speed mode
    RwPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGB0, AccessWidth32, (UINT32) (~ (0x03 << 10)), (UINT32) (0x03 << 10), StdHeader);
  } else {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE +  FCH_PMIOA_REGD3, AccessWidth8, 0xBF, 0x00);
  }
}

