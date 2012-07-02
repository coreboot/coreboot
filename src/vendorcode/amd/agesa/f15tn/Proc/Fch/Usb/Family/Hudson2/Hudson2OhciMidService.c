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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*;********************************************************************************
;
; Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
;
; AMD is granting you permission to use this software (the Materials)
; pursuant to the terms and conditions of your Software License Agreement
; with AMD.  This header does *NOT* give you permission to use the Materials
; or any rights under AMD's intellectual property.  Your use of any portion
; of these Materials shall constitute your acceptance of those terms and
; conditions.  If you do not agree to the terms and conditions of the Software
; License Agreement, please do not use any portion of these Materials.
;
; CONFIDENTIALITY:  The Materials and all other information, identified as
; confidential and provided to you by AMD shall be kept confidential in
; accordance with the terms and conditions of the Software License Agreement.
;
; LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
; PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
; WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
; MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
; OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
; IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
; (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
; INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
; GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
; RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
; THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
; EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
; THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
;
; AMD does not assume any responsibility for any errors which may appear in
; the Materials or any other related information provided to you by AMD, or
; result from use of the Materials or any related information.
;
; You agree that you will not reverse engineer or decompile the Materials.
;
; NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
; further information, software, technical information, know-how, or show-how
; available to you.  Additionally, AMD retains the right to modify the
; Materials at any time, without notice, and is not obligated to provide such
; modified Materials to you.
;
; U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
; "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
; subject to the restrictions as set forth in FAR 52.227-14 and
; DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
; Government constitutes acknowledgement of AMD's proprietary rights in them.
;
; EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
; direct product thereof will be exported directly or indirectly, into any
; country prohibited by the United States Export Administration Act and the
; regulations thereunder, without the required authorization from the U.S.
; government nor will be used for any purpose prohibited by the same.
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
  RwPci ((UINT32) Value + 0x50 , AccessWidth8, (UINT32)~(BIT0 + BIT5 + BIT12), 0, FchDataPtr->StdHeader);
  //
  // USB SMI Handshake
  //
  RwPci ((UINT32) Value + 0x50  + 1, AccessWidth8, (UINT32)~BIT4, 0x00, FchDataPtr->StdHeader);

  if (Value != (USB4_OHCI_BUS_DEV_FUN << 16)) {
    if ( FchDataPtr->Usb.OhciSsid != 0 ) {
      RwPci ((UINT32) Value + FCH_OHCI_REG2C, AccessWidth32, 0x00, FchDataPtr->Usb.OhciSsid, FchDataPtr->StdHeader);
    }
  }
  //
  // recommended setting to, enable fix to cover the corner case S3 wake up issue from some USB 1.1 devices
  //OHCI 0_PCI_Config 0x50[30] = 1
  //
  RwPci ((UINT32) Value + 0x50  + 3, AccessWidth8, (UINT32)~BIT6, (UINT32)BIT6, FchDataPtr->StdHeader);
  //
  // L1 Early Exit
  // Set OHCI Arbiter Mode.
  // Set Enable Global Clock Gating.
  //
  RwPci ((UINT32) Value + FCH_OHCI_REG80, AccessWidth8, (UINT32)~(BIT0 + BIT4 + BIT5 + BIT6 + BIT7), (UINT32)(BIT0 + BIT4 + BIT7), FchDataPtr->StdHeader);
  RwPci ((UINT32) Value + FCH_OHCI_REG80, AccessWidth16, (UINT32)~(BIT4 + BIT5 + BIT8), (UINT32)(BIT4 + BIT5 + BIT8), FchDataPtr->StdHeader);
  //
  // Enable OHCI SOF Synchronization.
  // Enable OHCI Periodic List Advance.
  //
  RwPci ((UINT32) Value + 0x50  + 2, AccessWidth8, (UINT32)~(BIT3 + BIT4 + BIT6 + BIT7), (UINT32)(BIT3 + BIT4 + BIT6 + BIT7), FchDataPtr->StdHeader);
  if ( FchDataPtr->Usb.UsbMsiEnable) {
    RwPci ((UINT32) Value + FCH_OHCI_REG40 + 1, AccessWidth8, (UINT32)~BIT0, 0x00, FchDataPtr->StdHeader);
    RwPci ((UINT32) Value + 0x50 , AccessWidth8, (UINT32)~BIT5, (UINT32)BIT5, FchDataPtr->StdHeader);
  }
  // full-speed false crc errors detected. Issue - fix enable
  RwPci ((UINT32) Value + FCH_OHCI_REG80, AccessWidth32, (UINT32) (~(0x01 << 10)), (UINT32) (0x01 << 10), FchDataPtr->StdHeader);
  // SB02643
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGB4, AccessWidth8, (UINT32)~BIT7, (UINT32)BIT7);
  // RPR 7.27 SB02686
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGED, AccessWidth8, (UINT32)~BIT2, (UINT32)BIT2);
  // SB02698
  RwPci ((UINT32) Value + 0x50 , AccessWidth8, (UINT32)~BIT0, 0, FchDataPtr->StdHeader);
}
