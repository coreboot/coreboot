/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch USB controller
 *
 * Init USB features.
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
#define FILECODE PROC_FCH_USB_USBENV_FILECODE
/**
 * FchInitEnvUsb - Config USB controller before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvUsb (
  IN  VOID     *FchDataPtr
  )
{
  //
  // Disabled All USB controller *** Move to each controller ***
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGEF, AccessWidth8, BIT7, 0);
  //
  // Clear PM_IO 0x65[4] UsbResetByPciRstEnable, Set this bit so that usb gets reset whenever there is PCIRST.
  // Enable UsbResumeEnable (USB PME) * Default value
  // USB SleepCtrl set as BIT9+BIT8 (6 uframes)
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGF0, AccessWidth16, ~BIT2, BIT2 + BIT7 + BIT8 + BIT9);

  SetUsbEnableReg (FchDataPtr);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGEE, AccessWidth8, ~(BIT2), 0 );
}


/**
 * SetUsbEnableReg
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
SetUsbEnableReg (
  IN  FCH_DATA_BLOCK   *FchDataPtr
  )
{
  UINT8                  UsbModeReg;

  UsbModeReg = 0;

  // Overwrite EHCI3/OHCI3 by Xhci1Enable
  if (FchDataPtr->Usb.Xhci1Enable) {
    FchDataPtr->Usb.Ohci3Enable = FALSE;
    FchDataPtr->Usb.Ehci3Enable = FALSE;
  }

  if ( FchDataPtr->Usb.Ohci1Enable ) UsbModeReg |= 0x01;
  if ( FchDataPtr->Usb.Ehci1Enable ) UsbModeReg |= 0x02;
  if ( FchDataPtr->Usb.Ohci2Enable ) UsbModeReg |= 0x04;
  if ( FchDataPtr->Usb.Ehci2Enable ) UsbModeReg |= 0x08;
  if ( FchDataPtr->Usb.Ohci3Enable ) UsbModeReg |= 0x10;
  if ( FchDataPtr->Usb.Ehci3Enable ) UsbModeReg |= 0x20;
  if ( FchDataPtr->Usb.Ohci4Enable ) UsbModeReg |= 0x40;

  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGEF, AccessWidth8, BIT7, UsbModeReg);
}
