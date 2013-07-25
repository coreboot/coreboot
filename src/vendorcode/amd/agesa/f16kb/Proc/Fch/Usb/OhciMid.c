/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch USB OHCI controller
 *
 * Init USB OHCI features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*;********************************************************************************
;
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
;*********************************************************************************/
#include "FchPlatform.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_USB_OHCIMID_FILECODE
//
// Declaration of local functions
//

/**
 * OhciInitAfterPciInit - Config USB OHCI controller after PCI emulation
 *
 * @param[in] Value  Controller PCI config address (bus# + device# + function#)
 * @param[in] FchDataPtr Fch configuration structure pointer.
 */
VOID OhciInitAfterPciInit (IN UINT32 Value, IN FCH_DATA_BLOCK* FchDataPtr);

/**
 * FchInitMidUsbOhci - Config USB OHCI controller after PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitMidUsbOhci (
  IN  VOID     *FchDataPtr
  )
{
  FCH_INTERFACE   *LocalCfgPtr;

  LocalCfgPtr = (FCH_INTERFACE *)FchDataPtr;

  FchInitMidUsbOhci1 (LocalCfgPtr);
  FchInitMidUsbOhci2 (LocalCfgPtr);
  FchInitMidUsbOhci3 (LocalCfgPtr);
  FchInitMidUsbOhci4 (LocalCfgPtr);
}

/**
 * FchInitMidUsbOhci1 - Config USB1 OHCI controller after PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitMidUsbOhci1 (
  IN  VOID     *FchDataPtr
  )
{
  UINT32       DeviceId;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  DeviceId = (USB1_OHCI_BUS_DEV_FUN << 16);
  OhciInitAfterPciInit (DeviceId, LocalCfgPtr);

  if (LocalCfgPtr->Usb.OhciSsid != 0 ) {
    RwPci ((USB1_OHCI_BUS_DEV_FUN << 16) + FCH_OHCI_REG2C, AccessWidth32, 0x00, LocalCfgPtr->Usb.OhciSsid, StdHeader);
  }
}

/**
 * FchInitMidUsbOhci2 - Config USB2 OHCI controller after PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitMidUsbOhci2 (
  IN  VOID     *FchDataPtr
  )
{
  UINT32       DeviceId;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  DeviceId = (USB2_OHCI_BUS_DEV_FUN << 16);
  OhciInitAfterPciInit (DeviceId, LocalCfgPtr);

  if (LocalCfgPtr->Usb.OhciSsid != 0 ) {
    RwPci ((USB2_OHCI_BUS_DEV_FUN << 16) + FCH_OHCI_REG2C, AccessWidth32, 0x00, LocalCfgPtr->Usb.OhciSsid, StdHeader);
  }
}

/**
 * FchInitMidUsbOhci3 - Config USB3 OHCI controller after PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitMidUsbOhci3 (
  IN  VOID     *FchDataPtr
  )
{
  UINT32       DeviceId;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  DeviceId = (USB3_OHCI_BUS_DEV_FUN << 16);
  OhciInitAfterPciInit (DeviceId, LocalCfgPtr);

  if (LocalCfgPtr->Usb.OhciSsid != 0 ) {
    RwPci ((USB3_OHCI_BUS_DEV_FUN << 16) + FCH_OHCI_REG2C, AccessWidth32, 0x00, LocalCfgPtr->Usb.OhciSsid, StdHeader);
  }
}

/**
 * FchInitMidUsbOhci4 - Config USB4 OHCI controller after PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitMidUsbOhci4 (
  IN  VOID     *FchDataPtr
  )
{
  UINT32       DeviceId;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  DeviceId = (USB4_OHCI_BUS_DEV_FUN << 16);
  OhciInitAfterPciInit (DeviceId, LocalCfgPtr);

  if (LocalCfgPtr->Usb.OhciSsid != 0 ) {
    RwPci ((USB4_OHCI_BUS_DEV_FUN << 16) + FCH_OHCI_REG2C, AccessWidth32, 0x00, LocalCfgPtr->Usb.OhciSsid, StdHeader);
  }
}

/**
 * OhciInitAfterPciInit - Config OHCI controller after PCI
 * emulation
 *
 *
 * @param[in] Value OHCI Controler info.
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
OhciInitAfterPciInit (
  IN  UINT32           Value,
  IN  FCH_DATA_BLOCK   *FchDataPtr
  )
{
  FchOhciInitAfterPciInit ( Value, FchDataPtr);
}
