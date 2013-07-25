/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch SATA controller
 *
 * Init SATA features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
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
#define FILECODE PROC_FCH_SATA_SATALATE_FILECODE

/**
 * FchInitLateSata - Prepare SATA controller to boot to OS.
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLateSata (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        SataPciCommandByte;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  //Return immediately is sata controller is not enabled
  //
  if ( LocalCfgPtr->Sata.SataMode.SataEnable == 0 ) {
    return;
  }

  //
  // Set Sata PCI Configuration Space Write enable
  //
  SataEnableWriteAccess (StdHeader);

  //
  // Set Sata Controller Memory & IO access enable
  //
  ReadPci (((SATA_BUS_DEV_FUN << 16) + 0x04), AccessWidth8, &SataPciCommandByte, StdHeader);
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x04), AccessWidth8, 0xFF, 0x03, StdHeader);

  //
  // Call Sub-function for each Sata mode
  //
  if (( LocalCfgPtr->Sata.SataClass == SataAhci7804) || (LocalCfgPtr->Sata.SataClass == SataAhci )) {
    FchInitLateSataAhci ( LocalCfgPtr );
  }

  if (( LocalCfgPtr->Sata.SataClass == SataIde2Ahci) || (LocalCfgPtr->Sata.SataClass == SataIde2Ahci7804 )) {
    FchInitLateSataIde2Ahci ( LocalCfgPtr );
  }

  if (( LocalCfgPtr->Sata.SataClass == SataNativeIde) || (LocalCfgPtr->Sata.SataClass == SataLegacyIde )) {
    FchInitLateSataIde ( LocalCfgPtr );
  }

  if ( LocalCfgPtr->Sata.SataClass == SataRaid) {
    FchInitLateSataRaid ( LocalCfgPtr );
  }

  FchInitLateProgramSataRegs  ( LocalCfgPtr );

  //
  // Restore Sata Controller Memory & IO access status
  //
  WritePci (((SATA_BUS_DEV_FUN << 16) + 0x04), AccessWidth8, &SataPciCommandByte, StdHeader);

  //
  // Set Sata PCI Configuration Space Write disable
  //
  SataDisableWriteAccess (StdHeader);
}

