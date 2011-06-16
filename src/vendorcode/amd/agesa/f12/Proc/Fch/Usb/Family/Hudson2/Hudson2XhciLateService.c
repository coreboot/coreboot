/* $NoKeywords:$ */
/**
 * @file
 *
 * Config FCH USB3 controller
 *
 * Init USB3 features.
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
#define FILECODE PROC_FCH_USB_FAMILY_HUDSON2_HUDSON2XHCILATESERVICE_FILECODE

//
// Declaration of local functions
//

/**
 * FchInitLateUsbXhciProgram - Config USB3 controller before OS
 * Boot
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLateUsbXhciProgram (
  IN  VOID     *FchDataPtr
  )
{
  UINT8                 IndexValue;
  UINT8                 Value;
  FCH_DATA_BLOCK        *LocalCfgPtr;
  AMD_CONFIG_PARAMS     *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  if ( LocalCfgPtr->Usb.Xhci1Enable == TRUE ) {
    ReadPci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x10, AccessWidth8, &Value, StdHeader);
    IndexValue = XHCI_REGISTER_BAR00;
    WriteBiosram (IndexValue, AccessWidth8, &Value, StdHeader);

    ReadPci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x11, AccessWidth8, &Value, StdHeader);
    IndexValue = XHCI_REGISTER_BAR01;
    WriteBiosram (IndexValue, AccessWidth8, &Value, StdHeader);

    ReadPci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x12, AccessWidth8, &Value, StdHeader);
    IndexValue = XHCI_REGISTER_BAR02;
    WriteBiosram (IndexValue, AccessWidth8, &Value, StdHeader);

    ReadPci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x13, AccessWidth8, &Value, StdHeader);
    IndexValue = XHCI_REGISTER_BAR03;
    WriteBiosram (IndexValue, AccessWidth8, &Value, StdHeader);

    ReadPci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x04, AccessWidth8, &Value, StdHeader);
    IndexValue = XHCI_REGISTER_04H;
    WriteBiosram (IndexValue, AccessWidth8, &Value, StdHeader);

    ReadPci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x0C, AccessWidth8, &Value, StdHeader);
    IndexValue = XHCI_REGISTER_0CH;
    WriteBiosram (IndexValue, AccessWidth8, &Value, StdHeader);

    ReadPci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x3C, AccessWidth8, &Value, StdHeader);
    IndexValue = XHCI_REGISTER_3CH;
    WriteBiosram (IndexValue, AccessWidth8, &Value, StdHeader);

    ReadPci ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x10, AccessWidth8, &Value, StdHeader);
    IndexValue = XHCI1_REGISTER_BAR00;
    WriteBiosram (IndexValue, AccessWidth8, &Value, StdHeader);

    ReadPci ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x11, AccessWidth8, &Value, StdHeader);
    IndexValue = XHCI1_REGISTER_BAR01;
    WriteBiosram (IndexValue, AccessWidth8, &Value, StdHeader);

    ReadPci ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x12, AccessWidth8, &Value, StdHeader);
    IndexValue = XHCI1_REGISTER_BAR02;
    WriteBiosram (IndexValue, AccessWidth8, &Value, StdHeader);

    ReadPci ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x13, AccessWidth8, &Value, StdHeader);
    IndexValue = XHCI1_REGISTER_BAR03;
    WriteBiosram (IndexValue, AccessWidth8, &Value, StdHeader);

    ReadPci ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x04, AccessWidth8, &Value, StdHeader);
    IndexValue = XHCI1_REGISTER_04H;
    WriteBiosram (IndexValue, AccessWidth8, &Value, StdHeader);

    ReadPci ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x0C, AccessWidth8, &Value, StdHeader);
    IndexValue = XHCI1_REGISTER_0CH;
    WriteBiosram (IndexValue, AccessWidth8, &Value, StdHeader);

    ReadPci ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x3C, AccessWidth8, &Value, StdHeader);
    IndexValue = XHCI1_REGISTER_3CH;
    WriteBiosram (IndexValue, AccessWidth8, &Value, StdHeader);
  }
}
