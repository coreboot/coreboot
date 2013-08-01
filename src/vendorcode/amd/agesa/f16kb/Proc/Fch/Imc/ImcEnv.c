/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Imc controller
 *
 * Init Imc Controller features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 87213 $   @e \$Date: 2013-01-30 15:37:54 -0600 (Wed, 30 Jan 2013) $
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
#define FILECODE PROC_FCH_IMC_IMCENV_FILECODE


//
// Declaration of local functions
//


/**
 * FchInitEnvImc - Config Imc controller before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvImc (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;

    SoftwareToggleImcStrapping (LocalCfgPtr);
  FchInitEnvEc (LocalCfgPtr);
}

/**
 * ValidateImcFirmware - Validate IMC Firmware.
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 * @retval        TRUE   Pass
 * @retval        FALSE  Failed
 */
BOOLEAN
ValidateImcFirmware (
  IN  VOID     *FchDataPtr
  )
{
  UINT32   ImcSig;
  UINT32   ImcSigAddr;
  UINT32   ImcAddr;
  UINT32   CurAddr;
  UINT32   ImcBinSig0;
  UINT32   ImcBinSig1;
  UINT16   ImcBinSig2;
  UINT8    IMCChecksumeByte;
  UINT8    IMCByte;

  ImcAddr = 0;

  //
  // Software IMC enable
  //
  ImcSigAddr = 0x80000;                          /// start from 512k to 64M
  ImcSig = 0x0;

  while ( ( ImcSig != FCH_IMC_ROMSIG ) && ( ImcSigAddr <= 0x4000000 ) ) {
    CurAddr = 0xffffffff - ImcSigAddr + 0x20001;
    ReadMem (CurAddr, AccessWidth32, &ImcSig);
    ReadMem ((CurAddr + 4), AccessWidth32, &ImcAddr);
    ImcSigAddr <<= 1;
  }

  IMCChecksumeByte = 0xff;

  if ( ImcSig == FCH_IMC_ROMSIG ) {
    //
    // "_AMD_IMC_C" at offset 0x2000 of the binary
    //
    ReadMem ((ImcAddr + 0x2000), AccessWidth32, &ImcBinSig0);
    ReadMem ((ImcAddr + 0x2004), AccessWidth32, &ImcBinSig1);
    ReadMem ((ImcAddr + 0x2008), AccessWidth16, &ImcBinSig2);

    if ((ImcBinSig0 == 0x444D415F) && (ImcBinSig1 == 0x434D495F) && (ImcBinSig2 == 0x435F) ) {
      IMCChecksumeByte = 0;

      for ( CurAddr = ImcAddr; CurAddr < ImcAddr + 0x10000; CurAddr++ ) {
        ReadMem (CurAddr, AccessWidth8, &IMCByte);
        IMCChecksumeByte = IMCChecksumeByte + IMCByte;
      }
    }
  }

  if ( IMCChecksumeByte ) {
    return  FALSE;
  } else {
    return  TRUE;
  }
}

