/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Sata controller
 *
 * Init Sata Controller features (PEI phase).
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
#define FILECODE PROC_FCH_SATA_FAMILY_HUDSON2_HUDSON2SATARESETSERVICE_FILECODE

/**
 * FchInitResetSataProgram - Config Sata controller during
 * Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetSataProgram (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        SataPortNum;
  UINT8        PortStatusByte;
  UINT8        EfuseByte;
  UINT8        FchSataMode;
  UINT8        FchSataInternal100Spread;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  //FchSataMode = LocalCfgPtr->Sata.SATA_MODE.SataMode.SataModeReg;
  //New structure need calculate Sata Register value
  //
  FchSataMode = 0;
  if ( LocalCfgPtr->FchReset.SataEnable ) {
    FchSataMode |= 0x01;
  }
  if ( LocalCfgPtr->Sata6AhciCap ) {
    FchSataMode |= 0x02;
  }
  if ( LocalCfgPtr->SataSetMaxGen2 ) {
    FchSataMode |= 0x04;
  }
  if ( LocalCfgPtr->FchReset.IdeEnable ) {
    FchSataMode |= 0x08;
  }

  FchSataMode |= (( LocalCfgPtr->SataClkMode ) << 4 ) ;
  LocalCfgPtr->SataModeReg = FchSataMode;            ///Save Back to Structure

  FchSataInternal100Spread = ( UINT8 ) LocalCfgPtr->SataInternal100Spread;
  SataPortNum = 0;

  //
  // Sata Workaround
  //
  for ( SataPortNum = 0; SataPortNum < 0x08; SataPortNum++ ) {
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x040  + 2), AccessWidth8, 0xFF, 1 << SataPortNum, StdHeader);
    FchStall (2, StdHeader);
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x040  + 2), AccessWidth8, (0xFF ^ (1 << SataPortNum)) , 0x00, StdHeader);
    FchStall (2, StdHeader);
  }

  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x084  + 3), AccessWidth8,  (UINT32)~BIT2, 0, StdHeader);
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x0A0 ), AccessWidth8, (UINT32)~(BIT2 + BIT3 + BIT4 + BIT5 + BIT6), BIT2 + BIT3 + BIT4 + BIT5, StdHeader);

  //
  // Sata Setting for clock mode only
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0xDA , AccessWidth8, 0, FchSataMode);

  if ( FchSataInternal100Spread ) {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x1E, AccessWidth8, 0xFF, BIT4);
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x084 ), AccessWidth32, 0xFFFFFFFB, 0x00, StdHeader);
  } else {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x1E, AccessWidth8, (UINT32)~BIT4, 0x00);
  }

  EfuseByte = SATA_FIS_BASE_EFUSE_LOC;
  GetEfuseStatus (&EfuseByte, StdHeader);

  if (EfuseByte & BIT0) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0xDA , AccessWidth8, 0xFB, 0x04);
  }

  ReadMem (ACPI_MMIO_BASE + PMIO_BASE + 0xDA , AccessWidth8, &PortStatusByte);
  if ( ((PortStatusByte & 0xF0) == 0x10) ) {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_PMIOA_REG08, AccessWidth8, 0, BIT5);
  }

  if ( FchSataInternal100Spread ) {
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x084 ), AccessWidth32, 0xFFFFFFFF, 0x04, StdHeader);
  }
}


