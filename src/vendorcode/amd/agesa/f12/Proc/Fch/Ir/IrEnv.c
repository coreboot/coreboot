/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Ir controller
 *
 * Init Ir Controller features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 46088 $   @e \$Date: 2011-01-28 11:24:26 +0800 (Fri, 28 Jan 2011) $
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
#define FILECODE PROC_FCH_IR_IRENV_FILECODE

/**
 * FchInitEnvIr - Config Ir controller before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvIr (
  IN  VOID     *FchDataPtr
  )
{
  IR_CONFIG    FchIrConfig;
  UINT8        Data;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  FchIrConfig = LocalCfgPtr->Ir.IrConfig;

  //
  //IR init Logical device 0x05
  //
  if (FchIrConfig != IrDisable) {
    EnterEcConfig (StdHeader);

    RwEc8 (0x07, 0x00, 0x05, StdHeader);         ///Select logical device 05, IR controller
    RwEc8 (0x60, 0x00, 0x05, StdHeader);         ///Set Base Address to 550h
    RwEc8 (0x61, 0x00, 0x50, StdHeader);
    RwEc8 (0x70, 0xF0, 0x05, StdHeader);         ///Set IRQ to 05h
    RwEc8 (0x30, 0x00, 0x01, StdHeader);         ///Enable logical device 5, IR controller

    Data = 0xAB;
    LibAmdIoWrite (AccessWidth8, 0x550, &Data, StdHeader);
    LibAmdIoRead (AccessWidth8, 0x551, &Data, StdHeader);
    Data = (((Data & 0xFC ) | 0x20) | (UINT8) FchIrConfig);
    LibAmdIoWrite (AccessWidth8, 0x551, &Data, StdHeader);

    ExitEcConfig (StdHeader);

    Data = 0xA0;                      /// EC APIC index
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC00, &Data, StdHeader);
    Data = 0x05;                      /// IRQ5
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC01, &Data, StdHeader);
  } else {
    EnterEcConfig (StdHeader);

    RwEc8 (0x07, 0x00, 0x05, StdHeader);         ///Select logical device 05, IR controller
    RwEc8 (0x30, 0x00, 0x00, StdHeader);         ///Disable logical device 5, IR controller

    ExitEcConfig (StdHeader);
  }
}
