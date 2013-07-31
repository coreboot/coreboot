/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Yangtze SD
 *
 * Init SD Controller.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 88054 $   @e \$Date: 2013-02-15 10:57:15 -0600 (Fri, 15 Feb 2013) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
#define FILECODE PROC_FCH_SD_FAMILY_YANGTZE_YANGTZESDENVSERVICE_FILECODE
/**
 * FchInitEnvSdProgram - Config SD controller before PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvSdProgram (
  IN  VOID     *FchDataPtr
  )
{
  UINT32                 SdData32;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;
  UINT32                 Sd30Control;
  UINT8                  ApuStepping;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  ApuStepping =  (UINT8) LocalCfgPtr->Misc.FchCpuId;
  //
  // SD Configuration
  //
  if ( LocalCfgPtr->Sd.SdConfig != SdDisable) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGD3, AccessWidth8, 0xBF, 0x40);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGE8, AccessWidth8, 0xFE, BIT0);
    Sd30Control = 0;
    SdData32 = 0x30FE00B2;
    ReadPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGA4, AccessWidth32, &SdData32, StdHeader);
    SdData32 &= 0xEFDF00FF;
    SdData32 |= BIT19 + BIT22 + BIT20;             ///ADMA

    if ( LocalCfgPtr->Sd.SdConfig == SdDma) {
      SdData32 &=  ~(BIT20 + BIT19);               ///DMA
    } else if ( LocalCfgPtr->Sd.SdConfig == SdPio) {
      SdData32 &=  ~(BIT20 + BIT22 + BIT19);       ///PIO
    }
    SdData32 |= BIT24 + BIT21;
    if ( LocalCfgPtr->Sd.SdHostControllerVersion == 1) {
      SdData32 |= 0x3200;
      if ( (ApuStepping & 0x0F) == 0) {
        SdData32 &= ~BIT21;
      }
      RwPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGB0, AccessWidth32, (UINT32) (~ (0x03 << 24)), (UINT32) (0x01 << 24), StdHeader);
      RwPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGB0, AccessWidth32, (UINT32) (~ (0xFF)), (UINT32) 0x19, StdHeader);
    } else {
      if ( LocalCfgPtr->Sd.SdHostControllerVersion == 2) {
        SdData32 |= 0xC800;
        RwPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGB0, AccessWidth32, (UINT32) (~ (0x03 << 24)), (UINT32) (0x02 << 24), StdHeader);
        RwPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGB0, AccessWidth32, (UINT32) (~ (0xFF)), (UINT32) 0x19, StdHeader);
      }
    }
    RwPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGA4, AccessWidth32, 0, SdData32, StdHeader);

    RwPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGB0, AccessWidth32, (UINT32) (~ (0x03 << 10)), (UINT32) (0x03 << 10), StdHeader);
    if (LocalCfgPtr->Sd.SdSsid != 0 ) {
      RwPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REG2C, AccessWidth32, 0, LocalCfgPtr->Sd.SdSsid, StdHeader);
    }
    if ( LocalCfgPtr->Sd.SdClockMultiplier ) {
      Sd30Control |= (BIT16 + BIT17);
    }
    Sd30Control &= ~(BIT0 + BIT1);
    // Sd30Control |= LocalCfgPtr->Sd.SdrCapabilities;
    Sd30Control |= LocalCfgPtr->Sd.SdReTuningMode << 14;
    if ( LocalCfgPtr->Sd.SdHostControllerVersion == 2) {
      Sd30Control &= 0xFFFF00FF;
      Sd30Control |= ( BIT0 + BIT1 + BIT8 + BIT10 + BIT13 );
    }
    Sd30Control |= ( BIT4 + BIT5 + BIT6 );
    RwPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGD0 + 1, AccessWidth8, 0xFD, BIT1, StdHeader);
    RwPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGA8, AccessWidth32, 0x3FFC, Sd30Control, StdHeader);
    RwPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGB0 + 3, AccessWidth8, 0, LocalCfgPtr->Sd.SdHostControllerVersion, StdHeader);
    RwPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGBC, AccessWidth32, 0, 0x0044CC98, StdHeader);
    RwPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGF0, AccessWidth32, 0, 0x000400FA, StdHeader);
    RwPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGF4, AccessWidth32, 0, 0x00040002, StdHeader);
    RwPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGF8, AccessWidth32, 0, 0x00010002, StdHeader);
    RwPci ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGFC, AccessWidth32, 0, 0x00014000, StdHeader);
  } else {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGD3, AccessWidth8, 0xBF, 0x00);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGE8, AccessWidth8, 0xFE, 0x00);
  }
}
