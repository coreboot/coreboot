/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch USB EHCI controller
 *
 * Init USB EHCI features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*;********************************************************************************
;
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
;*********************************************************************************/
#include "FchPlatform.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_USB_EHCIMID_FILECODE

/* extern VOID FchEhciInitAfterPciInit (IN UINT32 Value, IN FCH_DATA_BLOCK* FchDataPtr); */
//
// Declaration of local functions
//
/**
 * EhciInitAfterPciInit - Config USB controller after PCI emulation
 *
 * @param[in] Value Controller PCI config address (bus# + device# + function#)
 * @param[in] FchDataPtr Fch configuration structure pointer.
 */
VOID EhciInitAfterPciInit (IN UINT32 Value, IN FCH_DATA_BLOCK* FchDataPtr);

/**
 * FchInitMidUsbEhci - Config USB EHCI controller after PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitMidUsbEhci (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;

  FchInitMidUsbEhci1 (LocalCfgPtr);
  FchInitMidUsbEhci2 (LocalCfgPtr);
  FchInitMidUsbEhci3 (LocalCfgPtr);
}

/**
 * FchInitMidUsbEhci1 - Config USB1 EHCI controller after PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitMidUsbEhci1 (
  IN  FCH_DATA_BLOCK     *FchDataPtr
  )
{
  UINT32       DeviceId;

  DeviceId = (USB1_EHCI_BUS_DEV_FUN << 16);
  EhciInitAfterPciInit (DeviceId, FchDataPtr);

}

/**
 * FchInitMidUsbEhci2 - Config USB2 EHCI controller after PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitMidUsbEhci2 (
  IN  FCH_DATA_BLOCK     *FchDataPtr
  )
{
  UINT32       DeviceId;

  DeviceId = (USB2_EHCI_BUS_DEV_FUN << 16);
  EhciInitAfterPciInit (DeviceId, FchDataPtr);

}

/**
 * FchInitMidUsbEhci3 - Config USB3 EHCI controller after PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */

VOID
FchInitMidUsbEhci3 (
  IN  FCH_DATA_BLOCK     *FchDataPtr
  )
{
  UINT32       DeviceId;

  DeviceId = (USB3_EHCI_BUS_DEV_FUN << 16);
  EhciInitAfterPciInit (DeviceId, FchDataPtr);

}

/**
 * EhciInitAfterPciInit - Config EHCI controller after PCI
 * emulation
 *
 *
 * @param[in] Value EHCI Controler info.
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
EhciInitAfterPciInit (
  IN  UINT32           Value,
  IN  FCH_DATA_BLOCK   *FchDataPtr
  )
{
  FchEhciInitAfterPciInit ( Value, FchDataPtr);
}

