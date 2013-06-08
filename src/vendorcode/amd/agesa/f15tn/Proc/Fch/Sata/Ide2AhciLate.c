/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch SATA controller (Ide2Ahci mode)
 *
 * Init SATA Ide2Ahci features.
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
#define FILECODE PROC_FCH_SATA_IDE2AHCILATE_FILECODE

/**
 * FchInitLateSataIde2Ahci - Prepare SATA Ide2Ahci controller to
 * boot to OS.
 *
 *              - Set class ID to Ide2Ahci (if set to Ide2Ahci * Mode)
 *              - Enable Ide2Ahci interrupt
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLateSataIde2Ahci (
  IN  VOID     *FchDataPtr
  )
{
  UINT32       Bar5;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  //program the AHCI class code
  //
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x08), AccessWidth32, 0, 0x01060100, StdHeader);
  //
  // Device ID
  //
  if ( LocalCfgPtr->Sata.SataClass == SataIde2Ahci7804 ) {
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x02), AccessWidth16, 0, FCH_SATA_AMDAHCI_DID, StdHeader);
  } else {
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x02), AccessWidth16, 0, FCH_SATA_AHCI_DID, StdHeader);
  }
  SataBar5setting (LocalCfgPtr, &Bar5);

  //
  //Set interrupt enable bit
  //
  RwMem ((Bar5 + 0x04), AccessWidth8, (UINT32)~0, BIT1);
  ShutdownUnconnectedSataPortClock (LocalCfgPtr, Bar5);
}
