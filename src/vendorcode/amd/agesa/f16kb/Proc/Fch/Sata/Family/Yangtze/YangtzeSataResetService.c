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
 * @e \$Revision: 86582 $   @e \$Date: 2013-01-23 12:25:55 -0600 (Wed, 23 Jan 2013) $
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
#define FILECODE PROC_FCH_SATA_FAMILY_YANGTZE_YANGTZESATARESETSERVICE_FILECODE

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
  UINT8        FchSataMode;
  UINT8        FchSataClkMode;

  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  //New structure need calculate Sata Register value
  //
  FchSataMode = 0;
  if ( LocalCfgPtr->FchReset.SataEnable ) {
    FchSataMode |= 0x01;
  }
  if ( LocalCfgPtr->SataSetMaxGen2 ) {
    FchSataMode |= 0x04;
  }
  FchSataMode |= 0x04;

  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x0A0), AccessWidth8, (UINT32)~(BIT2 + BIT3 + BIT4 + BIT5 + BIT6), 0, StdHeader);
  FchSataClkMode = LocalCfgPtr->SataClkMode;
  //
  // Sata Setting for clock mode only
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0xDA, AccessWidth8, 0, FchSataMode);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0xDA, AccessWidth8, 0x0F, (FchSataClkMode << 4));
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x084 + 3), AccessWidth8,  (UINT32)~BIT2, BIT2, StdHeader);

  //
  // For Yangtze design (reference board) is connected both external and internal clock.
  // Sata clock mode will set by AGESA FCH (FCH_RESET_DATA_BLOCK) SataClkMode.
  //
  switch ( FchSataClkMode ) {
  case 0:
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x08C), AccessWidth32, 0xFFFFFF00, 0xF0, StdHeader);
    break;
  case 1:
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x08C), AccessWidth32, 0xFFFFFF00, 0x7D, StdHeader);
    break;
  case 9:
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x08C), AccessWidth32, 0xFFFFFF00, 0xF0, StdHeader);
    break;
  }
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x088), AccessWidth32, 0x0FFFFFFF, 0x20000000, StdHeader);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0xDC + 2, AccessWidth8, 0xFE, 0x01);
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x084), AccessWidth32, 0xFFFFFFFB, 0x00, StdHeader);
  FchStall (1000, StdHeader);
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x084), AccessWidth32, 0xFFFFFFFF, 0x04, StdHeader);
  if ( LocalCfgPtr->FchReset.SataEnable ) {
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x04C), AccessWidth32, 0xFDFDFCFE, 0x02020301, StdHeader);
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x040), AccessWidth32, (UINT32) (~ (0x3 << 1)), (UINT32) (0x00 << 1), StdHeader);
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x48), AccessWidth32, 0x00, 0xC0070800, StdHeader);
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x040), AccessWidth32, (UINT32) (~ (0x3 << 1)), (UINT32) (0x01 << 1), StdHeader);
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x48), AccessWidth32, 0x00, 0x2188, StdHeader);
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x040), AccessWidth32, (UINT32) (~ (0x3 << 1)), (UINT32) (0x02 << 1), StdHeader);
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x48), AccessWidth32, 0x00, 0x10EA, StdHeader);
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x040), AccessWidth32, (UINT32) (~ (0x1 << 13)), (UINT32) (0x01 << 13), StdHeader);

  }
}


