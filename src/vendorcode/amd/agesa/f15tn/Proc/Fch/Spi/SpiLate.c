/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Spi (Lpc) controller
 *
 * Init Spi (Lpc) Controller features.
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
#define FILECODE PROC_FCH_SPI_SPILATE_FILECODE

/**
 * FchInitLateSpi - Prepare Spi controller to boot to OS.
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLateSpi (
  IN  VOID     *FchDataPtr
  )
{
  FchInitLateLpc (FchDataPtr);
}

/**
 * FchSpiUnlock - Fch SPI Unlock
 *
 *
 * @param[in] FchDataPtr
 *
 */
VOID
FchSpiUnlock (
  IN  VOID     *FchDataPtr
  )
{
  UINT32                  SpiRomBase;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  SpiRomBase = LocalCfgPtr->Spi.RomBaseAddress;

  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG50, AccessWidth32, (UINT32)~(BIT0 + BIT1), 0, StdHeader);
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG54, AccessWidth32, (UINT32)~(BIT0 + BIT1), 0, StdHeader);
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG58, AccessWidth32, (UINT32)~(BIT0 + BIT1), 0, StdHeader);
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG5C, AccessWidth32, (UINT32)~(BIT0 + BIT1), 0, StdHeader);
  RwMem (SpiRomBase + FCH_SPI_MMIO_REG00, AccessWidth32, (UINT32)~(BIT22 + BIT23), (UINT32)(BIT22 + BIT23));
}

/**
 * FchSpiLock - Fch SPI lock
 *
 *
 * @param[in] FchDataPtr
 *
 */
VOID
FchSpiLock (
  IN  VOID     *FchDataPtr
  )
{
  UINT32                  SpiRomBase;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  SpiRomBase = LocalCfgPtr->Spi.RomBaseAddress;

  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG50, AccessWidth32, (UINT32)~(BIT0 + BIT1), (UINT32)(BIT0 + BIT1), StdHeader);
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG54, AccessWidth32, (UINT32)~(BIT0 + BIT1), (UINT32)(BIT0 + BIT1), StdHeader);
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG58, AccessWidth32, (UINT32)~(BIT0 + BIT1), (UINT32)(BIT0 + BIT1), StdHeader);
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG5C, AccessWidth32, (UINT32)~(BIT0 + BIT1), (UINT32)(BIT0 + BIT1), StdHeader);
  RwMem (SpiRomBase + FCH_SPI_MMIO_REG00, AccessWidth32, (UINT32)~(BIT22 + BIT23), 0);
}
