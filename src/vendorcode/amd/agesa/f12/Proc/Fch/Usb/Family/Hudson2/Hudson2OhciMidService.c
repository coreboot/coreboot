/* $NoKeywords:$ */
/**
 * @file
 *
 * Config FCH USB OHCI controller
 *
 * Init USB OHCI features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 47163 $   @e \$Date: 2011-02-16 07:23:13 +0800 (Wed, 16 Feb 2011) $
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
#define FILECODE PROC_FCH_USB_FAMILY_HUDSON2_HUDSON2OHCIMIDSERVICE_FILECODE
//
// Declaration of local functions
//

/**
 * FchOhciInitAfterPciInit - Config USB OHCI controller after
 * PCI emulation
 *
 * @param[in] Value  Controller PCI config address (bus# + device# + function#)
 * @param[in] FchDataPtr Fch configuration structure pointer.
 */
VOID
FchOhciInitAfterPciInit (
  IN  UINT32           Value,
  IN  FCH_DATA_BLOCK   *FchDataPtr
  )
{
  //
  // Disable the MSI capability of USB host controllers
  //
  RwPci ((UINT32) Value + FCH_OHCI_REG40 + 1, AccessWidth8, 0xFF, BIT0, FchDataPtr->StdHeader);
  RwPci ((UINT32) Value + FCH_OHCI_REG50, AccessWidth8, ~(BIT0 + BIT5 + BIT12), BIT0, FchDataPtr->StdHeader);
  //
  // USB SMI Handshake
  //
  RwPci ((UINT32) Value + FCH_OHCI_REG50 + 1, AccessWidth8, ~BIT4, 0x00, FchDataPtr->StdHeader);

  if (Value != (USB4_OHCI_BUS_DEV_FUN << 16)) {
    if ( FchDataPtr->Usb.OhciSsid != NULL ) {
      RwPci ((UINT32) Value + FCH_OHCI_REG2C, AccessWidth32, 0x00, FchDataPtr->Usb.OhciSsid, FchDataPtr->StdHeader);
    }
  }
  //
  // recommended setting to, enable fix to cover the corner case S3 wake up issue from some USB 1.1 devices
  //OHCI 0_PCI_Config 0x50[30] = 1
  //
  RwPci ((UINT32) Value + FCH_OHCI_REG50 + 3, AccessWidth8, ~BIT6, BIT6, FchDataPtr->StdHeader);
  //
  // L1 Early Exit
  // Set OHCI Arbiter Mode.
  // Set Enable Global Clock Gating.
  //
  RwPci ((UINT32) Value + FCH_OHCI_REG80, AccessWidth8, ~(BIT0 + BIT4 + BIT5 + BIT6 + BIT7), BIT0 + BIT4 + BIT7, FchDataPtr->StdHeader);
  //
  // Enable OHCI SOF Synchronization.
  // Enable OHCI Periodic List Advance.
  //
  RwPci ((UINT32) Value + FCH_OHCI_REG50 + 2, AccessWidth8, ~(BIT3 + BIT4), BIT3 + BIT4, FchDataPtr->StdHeader);
  if ( FchDataPtr->Usb.UsbMsiEnable) {
    RwPci ((UINT32) Value + FCH_OHCI_REG40 + 1, AccessWidth8, ~BIT0, 0x00, FchDataPtr->StdHeader);
    RwPci ((UINT32) Value + FCH_OHCI_REG50, AccessWidth8, ~BIT5, BIT5, FchDataPtr->StdHeader);
  }
  // full-speed false crc errors detected. Issue - fix enable
  RwPci ((UINT32) Value + FCH_OHCI_REG80, AccessWidth32, (UINT32) (~(0x01 << 10)), (UINT32) (0x01 << 10), FchDataPtr->StdHeader);
  // SB02643
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGB4, AccessWidth8, ~BIT7, BIT7);
  // RPR 7.27 SB02686
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGED, AccessWidth8, ~BIT2, BIT2);
  // SB02698
  RwPci ((UINT32) Value + FCH_OHCI_REG50, AccessWidth8, ~BIT0, BIT0, FchDataPtr->StdHeader);
}
