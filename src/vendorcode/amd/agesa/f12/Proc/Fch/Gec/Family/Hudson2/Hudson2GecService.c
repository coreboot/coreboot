/* $NoKeywords:$ */
/**
 * @file
 *
 * Config FCH GEC controller
 *
 * Init GEC features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
 *
 */
/*;********************************************************************************
;
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
;*********************************************************************************/

#include "FchPlatform.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_GEC_FAMILY_HUDSON2_HUDSON2GECSERVICE_FILECODE

/**
 * FchSwInitGecBootRom - Config GEC Boot ROM by Platform define
 * ROM address
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchSwInitGecBootRom (
  IN  VOID     *FchDataPtr
  )
{
  VOID*        GecRomAddress;
  VOID*        GecShadowRomAddress;
  UINT32       Temp;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  if ( !LocalCfgPtr->Gec.PtrDynamicGecRomAddress == NULL ) {
    GecRomAddress = LocalCfgPtr->Gec.PtrDynamicGecRomAddress;
    GecShadowRomAddress = (VOID*) (UINTN) LocalCfgPtr->Gec.GecShadowRomBase;
    FchCopyMem (GecShadowRomAddress, GecRomAddress, 0x100);
    ReadPci ((GEC_BUS_DEV_FUN << 16) + FCH_GEC_REG10, AccessWidth32, &Temp, StdHeader);
    Temp = Temp & 0xFFFFFFF0;
    RwMem (Temp + FCH_GEC_INTERNAL_REG, AccessWidth32, 0, BIT0 + BIT29);
  }
}
