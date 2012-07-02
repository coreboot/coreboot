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
#define FILECODE PROC_FCH_USB_FAMILY_HUDSON2_HUDSON2EHCIMIDSERVICE_FILECODE
//
// Declaration of local functions
//

/**
 * FchEhciInitAfterPciInit - Config USB controller after PCI emulation
 *
 * @param[in] Value Controller PCI config address (bus# + device# + function#)
 * @param[in] FchDataPtr Fch configuration structure pointer.
 */
VOID
FchEhciInitAfterPciInit (
  IN  UINT32           Value,
  IN  FCH_DATA_BLOCK   *FchDataPtr
  )
{
  UINT32  BarAddress;
  UINT32  Var;

  //
  //Get BAR address
  //
  ReadPci ((UINT32) Value + FCH_EHCI_REG10, AccessWidth32, &BarAddress, FchDataPtr->StdHeader);
  if ( (BarAddress != - 1) && (BarAddress != 0) ) {
    //
    //Enable Memory access
    //
    RwPci ((UINT32) Value + FCH_EHCI_REG04, AccessWidth8, 0, BIT1, FchDataPtr->StdHeader);
    if (FchDataPtr->Usb.EhciSsid != 0 ) {
      RwPci ((UINT32) Value + FCH_EHCI_REG2C, AccessWidth32, 0x00, FchDataPtr->Usb.EhciSsid, FchDataPtr->StdHeader);
    }
    //
    //USB Common PHY CAL & Control Register setting
    //
    Var = 0x00020F00;
    WriteMem (BarAddress + 0x0C0 , AccessWidth32, &Var);
    //
    // IN AND OUT DATA PACKET FIFO THRESHOLD
    // EHCI BAR 0xA4 //IN threshold bits[7:0]=0x40 //OUT threshold bits[23:16]=0x40
    //
    RwMem (BarAddress + FCH_EHCI_BAR_REGA4, AccessWidth32, 0xFF00FF00, 0x00400040);
    //
    // EHCI Dynamic Clock Gating Feature
    // Enable Global Clock Gating (BIT14)
    //
    RwMem (BarAddress + FCH_EHCI_BAR_REGBC, AccessWidth32, (UINT32)~( BIT12 + BIT14), (UINT32)(BIT12 + BIT14));
    RwMem (BarAddress + 0x0B0 , AccessWidth32, (UINT32)~BIT5, (UINT32)BIT5);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGF0, AccessWidth16, (UINT32)~BIT12, (UINT32)BIT12);
    //RPR 8.26 Incorrect gated signals in xhc_to_s5
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGF0, AccessWidth32, (UINT32)~BIT16, (UINT32)BIT16);
    //
    // Enable adding extra flops to PHY rsync path
    // Step 1:
    //  EHCI_BAR 0xB4 [6] = 1
    //  EHCI_BAR 0xB4 [7] = 0
    //  EHCI_BAR 0xB4 [12] = 0 ("VLoad")
    //  All other bit field untouched
    // Step 2:
    //  EHCI_BAR 0xB4[12] = 1
    //
    // USB 2.0 Ports Driving Strength
    // Step1 is done by default
    // Step2
    RwMem (BarAddress + FCH_EHCI_BAR_REGB4, AccessWidth32, (UINT32)~BIT12, (UINT32)BIT12);
    // Step3
    RwMem (BarAddress + FCH_EHCI_BAR_REGC4, AccessWidth32, (UINT32) (~ 0x00000f00), 0x00000200);
    RwMem (BarAddress + 0x0C0 , AccessWidth32, (UINT32) (~ 0x0000ff00), 0x00000f00);

    //Set EHCI_pci_configx50[6]='1' to disable EHCI MSI support
    RwPci ((UINT32) Value + 0x50 , AccessWidth32, ~ ((UINT32) (0x01 << 6)), (UINT32) (0x01 << 6), FchDataPtr->StdHeader);
    // EHCI Async Park Mode
    //Set EHCI_pci_configx50[11:8]=0x1
    //Set EHCI_pci_configx50[15:12]=0x1
    //Set EHCI_pci_configx50[17]=0x1
    RwPci ((UINT32) Value + 0x50 , AccessWidth32, ~ ((UINT32) (0x0F << 8)), (UINT32) (0x01 << 8), FchDataPtr->StdHeader);
    RwPci ((UINT32) Value + 0x50 , AccessWidth32, ~ ((UINT32) (0x0F << 12)), (UINT32) (0x01 << 12), FchDataPtr->StdHeader);
    RwPci ((UINT32) Value + 0x50 , AccessWidth32, ~ ((UINT32) (0x01 << 17)), (UINT32) (0x01 << 17), FchDataPtr->StdHeader);
    //RPR 7.14 Extend InterPacket Gap
    RwPci ((UINT32) Value + 0x50 , AccessWidth32, ~ ((UINT32) (0x01 << 21)), (UINT32) (0x01 << 21), FchDataPtr->StdHeader);

    // Enabling EHCI Async Stop Enhancement
    //Set EHCI_pci_configx50[29]='1' to disableEnabling EHCI Async Stop Enhancement
    //
    RwPci ((UINT32) Value + 0x50 , AccessWidth32, ~ ((UINT32) (0x01 << 29)), (UINT32) (0x01 << 29), FchDataPtr->StdHeader);
    //
    // recommended setting "EHCI Advance PHY Power Savings"
    // Set EHCI_pci_configx50[31]='1'
    // Fix for EHCI controller driver  yellow sign issue under device manager
    // when used in conjunction with HSET tool driver. EHCI PCI config 0x50[20]=1
    // Disable USB data cache to resolve USB controller hang issue with Lan adaptor.
    // EHCI PCI config register 50h bit 26 to `1'.
    //
    RwPci ((UINT32) Value + 0x50  + 2, AccessWidth16, (UINT16)0xFFFF, BIT16 + BIT10, FchDataPtr->StdHeader);
    //
    // USB Delay A-Link Express L1 State
    // PING Response Fix Enable EHCI_PCI_Config  x54[1] = 1
    // Enable empty list mode. x54[3]
    // Enable "L1 Early Exit" functionality. 0x54 [6:5] = 0x3 0x54 [9:7] = 0x4
    //
    RwPci ((UINT32) Value + 0x54 , AccessWidth32, (UINT32)~BIT0, 0x0000027b, FchDataPtr->StdHeader);
    RwAlink ((FCH_ABCFG_REG90 | (UINT32) (ABCFG << 29)), 0xFFFEFFFF, (UINT32)BIT16, FchDataPtr->StdHeader);
    if ( FchDataPtr->Usb.UsbMsiEnable) {
      RwPci ((UINT32) Value + 0x50 , AccessWidth32, (UINT32)~BIT6, 0x00, FchDataPtr->StdHeader);
    }
    // Long Delay on Framelist Read Causing EHCI DMA to Address 0 - Fix
    // RWPCI ((UINT32) Value + 0x54 , AccWidthUint32 | S3_SAVE, ~BIT13, BIT13);
    // LS connection can't wake up system from S3/S4/S5 when EHCI owns the port - Fix
    RwPci ((UINT32) Value + 0x54 , AccessWidth32, (UINT32)~BIT4, (UINT32)BIT4, FchDataPtr->StdHeader);
    // EHCI lMU Hangs when Run/Stop is Set First and PDC is Enabled Near End uFrame 7 - Fix Enable
    RwPci ((UINT32) Value + 0x54 , AccessWidth32, (UINT32)~BIT11, (UINT32)BIT11, FchDataPtr->StdHeader);
    // RPR 7.25  SB02674
    RwPci ((UINT32) Value + 0x54 , AccessWidth16, (UINT16)0x5FFF, BIT13 + BIT15, FchDataPtr->StdHeader);
    // RPR 7.26  SB02684
    RwPci ((UINT32) Value + 0x50  + 2, AccessWidth16, (UINT32)~BIT3, (UINT32)BIT3, FchDataPtr->StdHeader);
    // RPR 7.26  SB02687
    RwPci ((UINT32) Value + 0x54  + 2, AccessWidth16, (UINT16)0xFFFC, BIT0 + BIT1, FchDataPtr->StdHeader);
    // RPR 7.28  SB02700
    RwPci ((UINT32) Value + 0x54  + 2, AccessWidth16, (UINT16)0xFFFB, BIT2, FchDataPtr->StdHeader);
    // RPR 7.29  SB02703
    RwPci ((UINT32) Value + 0x54  + 2, AccessWidth16, (UINT16)0xFFF7, BIT3, FchDataPtr->StdHeader);
  } else {
    //
    // Fake Bar
    //
    BarAddress = FCH_FAKE_USB_BAR_ADDRESS;
    WritePci ((UINT32) Value + FCH_EHCI_REG10, AccessWidth32, &BarAddress, FchDataPtr->StdHeader);
    //
    //Enable Memory access
    //
    RwPci ((UINT32) Value + FCH_EHCI_REG04, AccessWidth8, 0, BIT1, FchDataPtr->StdHeader);
    //
    // Enable Global Clock Gating (BIT14)
    //
    RwMem (BarAddress + FCH_EHCI_BAR_REGBC, AccessWidth32, (UINT32)~( BIT12 + BIT14), (UINT32)(BIT12 + BIT14));
    RwMem (BarAddress + 0x0B0 , AccessWidth32, (UINT32)~BIT5, (UINT32)BIT5);
    RwPci ((UINT32) Value + FCH_EHCI_REG04, AccessWidth8, 0, 0, FchDataPtr->StdHeader);
  }
}

