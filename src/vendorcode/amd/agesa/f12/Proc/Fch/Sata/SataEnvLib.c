/* $NoKeywords:$ */
/**
 * @file
 *
 * Fch SATA controller Library
 *
 * SATA Library
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
#include "Filecode.h"
#define FILECODE PROC_FCH_SATA_SATAENVLIB_FILECODE

/**
 * sataSetIrqIntResource - Config SATA IRQ/INT# resource
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 * @param[in] StdHeader
 *
 */
VOID
SataSetIrqIntResource (
  IN  VOID                 *FchDataPtr,
  IN  AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINT8                  ValueByte;
  FCH_DATA_BLOCK         *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  //
  // IRQ14/IRQ15 come from IDE or SATA
  //
  ValueByte = 0x08;
  LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC00, &ValueByte, StdHeader);
  LibAmdIoRead (AccessWidth8, FCH_IOMAP_REGC01, &ValueByte, StdHeader);
  ValueByte = ValueByte & 0x0F;

  if (LocalCfgPtr->Sata.SataClass == SataLegacyIde) {
    ValueByte = ValueByte | 0x50;
  } else {
    if (LocalCfgPtr->Sata.SataIdeMode == 1) {
      //
      // Both IDE & SATA set to Native mode
      //
      ValueByte = ValueByte | 0xF0;
    }
  }

  LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC01, &ValueByte, StdHeader);
}

