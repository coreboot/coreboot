/* $NoKeywords:$ */
/**
 * @file
 *
 * Config FCH Xhci controller
 *
 * Init Xhci Controller features (PEI phase).
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
#define FILECODE PROC_FCH_USB_FAMILY_HUDSON2_HUDSON2XHCIRESETSERVICE_FILECODE

/**
 * FchInitResetXhciProgram - Config Xhci controller during
 * Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetXhciProgram (
  IN  VOID     *FchDataPtr
  )
{
  UINT8                     IndexValue;
  UINT32                    ValueDword;
  UINT8                     ValueByte;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  ReadPci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x00, AccessWidth32, &ValueDword, StdHeader);
  ReadPci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x00, AccessWidth32, &ValueDword, StdHeader);
  if ( ValueDword == (FCH_USB_XHCI_DID << 16) + FCH_USB_XHCI_VID) {
    //
    // First Xhci controller.
    //
    ReadPci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x00, AccessWidth32, &ValueDword, StdHeader);
    ValueDword = 0;

    IndexValue = XHCI_REGISTER_BAR00;
    ReadBiosram (IndexValue, AccessWidth32, &ValueDword, StdHeader);
    WritePci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x10, AccessWidth32, &ValueDword, StdHeader);

    IndexValue = XHCI_REGISTER_04H;
    ReadBiosram (IndexValue, AccessWidth8, &ValueByte, StdHeader);
    WritePci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x04, AccessWidth8, &ValueByte, StdHeader);

    IndexValue = XHCI_REGISTER_0CH;
    ReadBiosram (IndexValue, AccessWidth8, &ValueByte, StdHeader);
    WritePci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x0C, AccessWidth8, &ValueByte, StdHeader);

    IndexValue = XHCI_REGISTER_3CH;
    ReadBiosram (IndexValue, AccessWidth8, &ValueByte, StdHeader);
    WritePci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x3C, AccessWidth8, &ValueByte, StdHeader);
    //
    // Second Xhci controller.
    //
    ReadPci ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x00, AccessWidth32, &ValueDword, StdHeader);
    ValueDword = 0;

    IndexValue = XHCI1_REGISTER_BAR00;
    ReadBiosram (IndexValue, AccessWidth32, &ValueDword, StdHeader);
    WritePci ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x10, AccessWidth32, &ValueDword, StdHeader);

    IndexValue = XHCI1_REGISTER_04H;
    ReadBiosram (IndexValue, AccessWidth8, &ValueByte, StdHeader);
    WritePci ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x04, AccessWidth8, &ValueByte, StdHeader);

    IndexValue = XHCI1_REGISTER_0CH;
    ReadBiosram (IndexValue, AccessWidth8, &ValueByte, StdHeader);
    WritePci ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x0C, AccessWidth8, &ValueByte, StdHeader);

    IndexValue = XHCI1_REGISTER_3CH;
    ReadBiosram (IndexValue, AccessWidth8, &ValueByte, StdHeader);
    WritePci ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x3C, AccessWidth8, &ValueByte, StdHeader);
  }
}

