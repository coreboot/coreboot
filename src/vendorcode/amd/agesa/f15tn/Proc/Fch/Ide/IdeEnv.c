/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch IDE controller
 *
 * Init IDE Controller features.
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
#define FILECODE PROC_FCH_IDE_IDEENV_FILECODE

/**
 * FchInitEnvIde - Config Ide controller before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvIde (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        Channel;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  RwPci (((IDE_BUS_DEV_FUN << 16) + FCH_IDE_REG40), AccessWidth8, 0xff, BIT0, StdHeader);

  //
  // Enabling IDE Explicit Pre-Fetch  IDE PCI Config 0x62[8]=0
  //
  RwPci (((IDE_BUS_DEV_FUN << 16) + 0x62  + 1), AccessWidth8,  (UINT32)~BIT0, BIT5, StdHeader);

  //
  // Disable SATA MSI
  //
  RwPci (((IDE_BUS_DEV_FUN << 16) + FCH_IDE_REG34), AccessWidth8, 0x00, 0x00, StdHeader);
  RwPci (((IDE_BUS_DEV_FUN << 16) + FCH_IDE_REG06), AccessWidth8, 0xEF, 0x00, StdHeader);

  //
  // Set Ide Channel enable/disable by parameter
  //
  ReadPci (((IDE_BUS_DEV_FUN << 16) + 0x040  + 11), AccessWidth8, &Channel, StdHeader);
  Channel &= 0xCF;
  if ( LocalCfgPtr->Sata.IdeDisUnusedIdePChannel ) {
    Channel |= 0x10;
  }
  if ( LocalCfgPtr->Sata.IdeDisUnusedIdeSChannel ) {
    Channel |= 0x20;
  }
  WritePci (((IDE_BUS_DEV_FUN << 16) + FCH_IDE_REG40 + 11), AccessWidth8, &Channel, StdHeader);

  //
  // IDE Controller Class ID & SSID
  // ** Get Sata Configuration ** for sync Sata & Ide with only one Legacy Ide device
  //
  if ( (LocalCfgPtr->Sata.SataIdeMode == 1) && (LocalCfgPtr->Sata.SataClass != SataLegacyIde) ) {
    //
    // Write the class code to IDE PCI register 08h-0Bh
    //
    RwPci (((IDE_BUS_DEV_FUN << 16) + FCH_IDE_REG08), AccessWidth32, 0, 0x01018F40, StdHeader);
  }
  if ( LocalCfgPtr->Sata.SataClass == SataLegacyIde ) {
    //
    //Set SATA controller to native mode
    //
    RwPci (((IDE_BUS_DEV_FUN << 16) + FCH_IDE_REG09), AccessWidth8, 0x00, 0x08F, StdHeader);
  }
  if (LocalCfgPtr->Ide.IdeSsid != 0 ) {
    RwPci ((IDE_BUS_DEV_FUN << 16) + 0x2C , AccessWidth32, 0x00, LocalCfgPtr->Ide.IdeSsid, StdHeader);
  }

  //
  // Disable write access to PCI header
  //
  RwPci (((IDE_BUS_DEV_FUN << 16) + FCH_IDE_REG40), AccessWidth8, (UINT32)~BIT0, 0, StdHeader);
}



