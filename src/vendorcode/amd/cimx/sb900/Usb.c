/**
 * @file
 *
 * Config Southbridge USB controller
 *
 * Init USB features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-SB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*;********************************************************************************
;
; Copyright (c) 2011, Advanced Micro Devices, Inc.
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
;     * Neither the name of Advanced Micro Devices, Inc. nor the names of
;       its contributors may be used to endorse or promote products derived
;       from this software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
;*********************************************************************************/
#include "SbPlatform.h"
#include "cbtypes.h"
#include "AmdSbLib.h"
#include "Hudson-2.h"

//
// Declaration of local functions
//
VOID
usbOverCurrentControl (
  IN       AMDSBCFG* pConfig
  );

UINT32
SetEhciPllWr (
  IN       UINT32 Value,
  IN       AMDSBCFG* pConfig
  );

/**
 * EhciInitAfterPciInit - Config USB controller after PCI emulation
 *
 * @param[in] Value Controller PCI config address (bus# + device# + function#)
 * @param[in] pConfig Southbridge configuration structure pointer.
 */
VOID EhciInitAfterPciInit (IN UINT32 Value, IN AMDSBCFG* pConfig);

/**
 * OhciInitAfterPciInit - Config USB OHCI controller after PCI emulation
 *
 * @param[in] Value  Controller PCI config address (bus# + device# + function#)
 * @param[in] pConfig Southbridge configuration structure pointer.
 */
VOID OhciInitAfterPciInit (IN UINT32 Value, IN AMDSBCFG* pConfig);

#ifdef XHCI_SUPPORT
  VOID XhciInitBeforePciInit (IN AMDSBCFG* pConfig);
  VOID XhciInitAfterPciInit (IN AMDSBCFG* pConfig);
  VOID XhciInitLate (IN AMDSBCFG* pConfig);
#endif
/**
 * usbInitBeforePciEnum - Config USB controller before PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
usbInitBeforePciEnum (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8     XhciEfuse;
  // add Efuse checking for Xhci enable/disable
  XhciEfuse = XHCI_EFUSE_LOCATION;
  TRACE ((DMSG_SB_TRACE, "Entering PreInit Usb \n"));
  // Disabled All USB controller
  // RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEF, AccWidthUint8, BIT7, 0);
  // Clear PM_IO 0x65[4] UsbResetByPciRstEnable, Set this bit so that usb gets reset whenever there is PCIRST.
  // Enable UsbResumeEnable (USB PME) * Default value
  // In SB700 USB SleepCtrl set as BIT10+BIT9, but Hudson-2 default is BIT9+BIT8 (6 uframes)
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF0, AccWidthUint16 | S3_SAVE, ~BIT2, BIT2 + BIT7 + BIT8 + BIT9);
  // Overwrite EHCI bit by OHCI bit
  pConfig->USBMODE.UsbModeReg = pConfig->USBMODE.UsbMode.Ohci1 \
    + (pConfig->USBMODE.UsbMode.Ohci1 << 1) \
    + (pConfig->USBMODE.UsbMode.Ohci2 << 2) \
    + (pConfig->USBMODE.UsbMode.Ohci2 << 3) \
    + (pConfig->USBMODE.UsbMode.Ohci3 << 4) \
    + (pConfig->USBMODE.UsbMode.Ohci3 << 5) \
    + (pConfig->USBMODE.UsbMode.Ohci4 << 6);

  // Overwrite EHCI3/OHCI3 by XhciSwitch
  if (pConfig->XhciSwitch) {
    pConfig->USBMODE.UsbModeReg &= 0xCF;
    pConfig->USBMODE.UsbModeReg |= 0x80;
  } else {
    pConfig->USBMODE.UsbModeReg &= 0x7F;
#ifdef USB_LOGO_SUPPORT
    if (pConfig->USBMODE.UsbMode.Ohci3) {
      RWMEM (ACPI_MMIO_BASE + SMI_BASE + SB_SMI_Usbwakup2, AccWidthUint8, 0, 0x0B);
    }
#endif
  }
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEF, AccWidthUint8, 0, (pConfig->USBMODE.UsbModeReg));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEE, AccWidthUint8, ~(BIT2), 0 );
//  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEF, AccWidthUint8, 0, (pConfig->USBMODE.UsbModeReg & ~( 0x15 << 1 )) + (( pConfig->USBMODE.UsbModeReg & 0x15 ) << 1 ) );

//  if ( pConfig->XhciSwitch == 1 && pConfig->S3Resume == 0 && pConfig->S4Resume == 0  ) {
//    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEF, AccWidthUint8, ~(BIT7) , BIT7);
//    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEF, AccWidthUint8, ~(BIT4 + BIT5) , 0);
//    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEE, AccWidthUint8, ~(BIT0 + BIT1 + BIT2), (BIT1 + BIT0 + BIT2) );
//  }
//  else {
//    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEF, AccWidthUint8, ~(BIT7) , ~(BIT7));
//  }
//  RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x40, AccWidthUint8, ~BIT6, 0);
//  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + 0xF3, AccWidthUint8, ~BIT4, BIT4);
//  SbStall (500);
//  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + 0xF3, AccWidthUint8, ~BIT4, 0);

#ifdef XHCI_SUPPORT
#ifndef XHCI_INIT_IN_ROM_SUPPORT
  if ( pConfig->XhciSwitch == 1 ) {
    if ( pConfig->S3Resume == 0 ) {
      XhciInitBeforePciInit (pConfig);
    } else {
      XhciInitIndirectReg ();
    }
  } else {
    // for power saving.

    // add Efuse checking for Xhci enable/disable
    getEfuseStatus (&XhciEfuse);
    if ((XhciEfuse & (BIT0 + BIT1)) != (BIT0 + BIT1)) {
      RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccWidthUint32, 0xF0FFFBFF, 0x0);
    }
  }
#endif
#endif

}

/**
 * usbInitAfterPciInit - Config USB controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
usbInitAfterPciInit (
  IN       AMDSBCFG* pConfig
  )
{
  if (IsSbA11 ()) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEF, AccWidthUint8, BIT7, 0x7F);
  }

  usb1EhciInitAfterPciInit (pConfig);
  usb2EhciInitAfterPciInit (pConfig);
  usb3EhciInitAfterPciInit (pConfig);
  usb3OhciInitAfterPciInit (pConfig);
  usb1OhciInitAfterPciInit (pConfig);
  usb2OhciInitAfterPciInit (pConfig);
  usb4OhciInitAfterPciInit (pConfig);
  // Restore back
  if (IsSbA11 ()) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEF, AccWidthUint8, BIT7, (pConfig->USBMODE.UsbModeReg));
  }
#ifdef XHCI_SUPPORT
  if ( pConfig->XhciSwitch == 1 ) {
    XhciInitAfterPciInit (pConfig);
  }
#endif

  if ( pConfig->UsbPhyPowerDown ) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF0, AccWidthUint8, ~BIT0, BIT0);
  } else  {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF0, AccWidthUint8, ~BIT0, 0);
  }

  if (IsSbA13Plus ()) {
    //SB02643
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGB4, AccWidthUint8, ~BIT7, BIT7);
    //RPR7.27 SB02686
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGED, AccWidthUint8, ~BIT2, BIT2);
  }
}

/**
 * usbOverCurrentControl - Config USB Over Current Control
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
usbOverCurrentControl (
  IN       AMDSBCFG* pConfig
  )
{

#ifdef SB_USB1_OVER_CURRENT_CONTROL
  RWPCI ((USB1_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG58, AccWidthUint32 | S3_SAVE, 0xfff00000, SB_USB1_OVER_CURRENT_CONTROL & 0x000FFFFF);
#endif

#ifdef SB_USB2_OVER_CURRENT_CONTROL
  RWPCI ((USB2_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG58, AccWidthUint32 | S3_SAVE, 0xfff00000, SB_USB2_OVER_CURRENT_CONTROL & 0x000FFFFF);
#endif

#ifdef SB_USB3_OVER_CURRENT_CONTROL
  RWPCI ((USB3_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG58, AccWidthUint32 | S3_SAVE, 0xffff0000, SB_USB3_OVER_CURRENT_CONTROL & 0x0000FFFF);
#endif

#ifdef SB_USB4_OVER_CURRENT_CONTROL
  RWPCI ((USB4_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG58, AccWidthUint32 | S3_SAVE, 0xffffff00, SB_USB4_OVER_CURRENT_CONTROL & 0x000000FF);
#endif

#ifdef SB_XHCI0_OVER_CURRENT_CONTROL
  RWXhci0IndReg ( SB_XHCI_IND_REG04, 0xFF00FFFF, (SB_XHCI0_OVER_CURRENT_CONTROL & 0xFF) << 16);
#endif

#ifdef SB_XHCI1_OVER_CURRENT_CONTROL
  RWXhci1IndReg ( SB_XHCI_IND_REG04, 0xFF00FFFF, (SB_XHCI1_OVER_CURRENT_CONTROL & 0xFF) << 16);
#endif

}

/**
 * usbInitLate - Config USB controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
usbInitLate (
  IN       AMDSBCFG* pConfig
  )
{
  if (pConfig->S4Resume) {
    //RWPCI (0x9004, AccWidthUint8, 0, 0x10);
    //RWPCI (0x9804, AccWidthUint8, 0, 0x10);
    //RWPCI (0xA504, AccWidthUint8, 0, 0x10);
    //RWPCI (0xB004, AccWidthUint8, 0, 0x10);
    //RWPCI (0x903C, AccWidthUint8, 0, 0x12);
    //RWPCI (0x983C, AccWidthUint8, 0, 0x12);
    //RWPCI (0xA53C, AccWidthUint8, 0, 0x12);
    //RWPCI (0xB03C, AccWidthUint8, 0, 0x12);
  }
#ifdef XHCI_SUPPORT
  if ( pConfig->XhciSwitch == 1 ) {
    XhciInitLate (pConfig);
  }
#endif
  SbEnableUsbIrq1Irq12ToPicApic ();
  usbOverCurrentControl (pConfig);
}

/**
 * usb1EhciInitAfterPciInit - Config USB1 EHCI controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
usb1EhciInitAfterPciInit (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32  ddDeviceId;
  ddDeviceId = (USB1_EHCI_BUS_DEV_FUN << 16);
  EhciInitAfterPciInit (ddDeviceId, pConfig);
}

/**
 * usb2EhciInitAfterPciInit - Config USB2 EHCI controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
usb2EhciInitAfterPciInit (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32  ddDeviceId;
  ddDeviceId = (USB2_EHCI_BUS_DEV_FUN << 16);
  EhciInitAfterPciInit (ddDeviceId, pConfig);
}

/**
 * usb3EhciInitAfterPciInit - Config USB3 EHCI controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */

VOID
usb3EhciInitAfterPciInit (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32  ddDeviceId;
  ddDeviceId = (USB3_EHCI_BUS_DEV_FUN << 16);
  EhciInitAfterPciInit (ddDeviceId, pConfig);
}


VOID
EhciInitAfterPciInit (
  IN       UINT32 Value,
  IN       AMDSBCFG* pConfig
  )
{
  UINT32  ddBarAddress;
  UINT32  ddVar;
  UINT32  ddDrivingStrength;
  UINT32  ddPortDrivingStrength;
  UINT32  portNumber;
  UINT32  port;
  ddDrivingStrength = 0;
  portNumber = 0;
  //Get BAR address
  ReadPCI ((UINT32) Value + SB_EHCI_REG10, AccWidthUint32, &ddBarAddress);
  if ( (ddBarAddress != - 1) && (ddBarAddress != 0) ) {
    //Enable Memory access
    RWPCI ((UINT32) Value + SB_EHCI_REG04, AccWidthUint8, 0, BIT1);
    if (pConfig->BuildParameters.EhciSsid != NULL ) {
      RWPCI ((UINT32) Value + SB_EHCI_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pConfig->BuildParameters.EhciSsid);
    }
    //USB Common PHY CAL & Control Register setting
    ddVar = 0x00020F00;
    WriteMEM (ddBarAddress + SB_EHCI_BAR_REGC0, AccWidthUint32, &ddVar);
    // RPR IN AND OUT DATA PACKET FIFO THRESHOLD
    // EHCI BAR 0xA4 //IN threshold bits[7:0]=0x40 //OUT threshold bits[23:16]=0x40
    RWMEM (ddBarAddress + SB_EHCI_BAR_REGA4, AccWidthUint32, 0xFF00FF00, 0x00400040);
    // RPR EHCI Dynamic Clock Gating Feature
    // RPR Enable Global Clock Gating (BIT14)
    RWMEM (ddBarAddress + SB_EHCI_BAR_REGBC, AccWidthUint32, ~(BIT12 + BIT14), BIT12 + BIT14);
    RWMEM (ddBarAddress + SB_EHCI_BAR_REGB0, AccWidthUint32, ~BIT5, BIT5);
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF0, AccWidthUint16, ~BIT12, BIT12);
    // RPR Enable adding extra flops to PHY rsync path
    // Step 1:
    //  EHCI_BAR 0xB4 [6] = 1
    //  EHCI_BAR 0xB4 [7] = 0
    //  EHCI_BAR 0xB4 [12] = 0 ("VLoad")
    //  All other bit field untouched
    // Step 2:
    //  EHCI_BAR 0xB4[12] = 1
    //RWMEM (ddBarAddress + SB_EHCI_BAR_REGB4, AccWidthUint32, ~(BIT6 + BIT7 + BIT12), 0x00);
    //RWMEM (ddBarAddress + SB_EHCI_BAR_REGB4, AccWidthUint32, ~BIT12, BIT12);

    // RPR7.8 USB 2.0 Ports Driving Strength
    // Step1 is done by default
    // Add support USBx_EHCI_DRIVING_STRENGTH port4 is at [19:16] and  port0 is [3:0]; 4 bits for per port, [3] is enable,[2:0] is driving strength.
    // For Cobia, USB3_EHCI_DRIVING_STRENGTH = 0x00CC
#ifdef USB1_EHCI_DRIVING_STRENGTH
    if (Value == (USB1_EHCI_BUS_DEV_FUN << 16)) {
      ddDrivingStrength = USB1_EHCI_DRIVING_STRENGTH;
      portNumber = 5;
    }
#endif
#ifdef USB2_EHCI_DRIVING_STRENGTH
    if (Value == (USB2_EHCI_BUS_DEV_FUN << 16)) {
      ddDrivingStrength = USB2_EHCI_DRIVING_STRENGTH;
      portNumber = 5;
    }
#endif
#ifdef USB3_EHCI_DRIVING_STRENGTH
    if (Value == (USB3_EHCI_BUS_DEV_FUN << 16)) {
      ddDrivingStrength = USB3_EHCI_DRIVING_STRENGTH;
      portNumber = 4;
    }
#endif
    if (portNumber > 0) {
      for (port = 0; port < portNumber; port ++) {
        ddPortDrivingStrength = (ddDrivingStrength >> (port * 4)) & 0x0F;
        if (ddPortDrivingStrength & BIT3) {
          ddPortDrivingStrength &= 0x7;
          RWMEM (ddBarAddress + SB_EHCI_BAR_REGB4, AccWidthUint32, ~ (BIT16 + BIT15 + BIT14 + BIT13 + BIT12 + BIT2 + BIT1 + BIT0), (port << 13) + ddPortDrivingStrength);
          RWMEM (ddBarAddress + SB_EHCI_BAR_REGB4, AccWidthUint32, ~BIT12, BIT12);
        }
      }
    }
    // Step2
    RWMEM (ddBarAddress + SB_EHCI_BAR_REGB4, AccWidthUint32, ~BIT12, BIT12);
    // Step3
    if (IsSbA11 ()) {
      RWMEM (ddBarAddress + SB_EHCI_BAR_REGC4, AccWidthUint32, (UINT32) (~ 0x00000f00), 0x00000000);
      RWMEM (ddBarAddress + SB_EHCI_BAR_REGC0, AccWidthUint32, (UINT32) (~ 0x0000ff00), 0x00001500);
    } else {
      RWMEM (ddBarAddress + SB_EHCI_BAR_REGC4, AccWidthUint32, (UINT32) (~ 0x00000f00), 0x00000200);
      RWMEM (ddBarAddress + SB_EHCI_BAR_REGC0, AccWidthUint32, (UINT32) (~ 0x0000ff00), 0x00000f00);
    }
    //Set EHCI_pci_configx50[6]='1' to disable EHCI MSI support
    RWPCI ((UINT32) Value + SB_EHCI_REG50, AccWidthUint32 | S3_SAVE, ~ ((UINT32) (0x01 << 6)), (0x01 << 6));
    //RPR 7.13 EHCI Async Park Mode
    //Set EHCI_pci_configx50[11:8]=0x1
    //Set EHCI_pci_configx50[15:12]=0x1
    //Set EHCI_pci_configx50[17]=0x1
    RWPCI ((UINT32) Value + SB_EHCI_REG50, AccWidthUint32 | S3_SAVE, ~ ((UINT32) (0x0F << 8)), (0x01 << 8));
    RWPCI ((UINT32) Value + SB_EHCI_REG50, AccWidthUint32 | S3_SAVE, ~ ((UINT32) (0x0F << 12)), (0x01 << 12));
    RWPCI ((UINT32) Value + SB_EHCI_REG50, AccWidthUint32 | S3_SAVE, ~ ((UINT32) (0x01 << 17)), (0x01 << 17));
    //RPR Enabling EHCI Async Stop Enhancement
    //Set EHCI_pci_configx50[29]='1' to disableEnabling EHCI Async Stop Enhancement
    RWPCI ((UINT32) Value + SB_EHCI_REG50, AccWidthUint32 | S3_SAVE, ~ ((UINT32) (0x01 << 29)), (0x01 << 29));
    //RWPCI ((UINT32) Value + SB_EHCI_REG50, AccWidthUint32 | S3_SAVE, 0xFF7F0FFF, 0x20221140);
    // RPR recommended setting "EHCI Advance PHY Power Savings"
    // Set EHCI_pci_configx50[31]='1'
    // Fix for EHCI controller driver  yellow sign issue under device manager
    // when used in conjunction with HSET tool driver. EHCI PCI config 0x50[20]=1
    RWPCI ((UINT32) Value + SB_EHCI_REG50 + 2, AccWidthUint16 | S3_SAVE, (UINT16)0xFFFF, BIT15);
    // ENH246436: Disable USB data cache to resolve USB controller hang issue with Lan adaptor.
    // EHCI PCI config register 50h bit 26 to `1'.
    if (pConfig->EhciDataCacheDis) {
      RWPCI ((UINT32) Value + SB_EHCI_REG50 + 2, AccWidthUint16 | S3_SAVE, (UINT16)0xFFFF, BIT10);
    }

    if (IsSbA13Plus ()) {
      //RPR 7.25  SB02674
      RWPCI ((UINT32) Value + SB_EHCI_REG54, AccWidthUint16 | S3_SAVE, (UINT16)0x5FFF, BIT13 + BIT15);
      //RPR 7.26  SB02684
      RWPCI ((UINT32) Value + SB_EHCI_REG50 + 2, AccWidthUint16 | S3_SAVE, ~ (BIT3), BIT3);
      //RPR 7.26  SB02687
      RWPCI ((UINT32) Value + SB_EHCI_REG54 + 2, AccWidthUint16 | S3_SAVE, (UINT16)0xFFFC, BIT0 + BIT1);
      //RPR 7.28  SB02700
      RWPCI ((UINT32) Value + SB_EHCI_REG54 + 2, AccWidthUint16 | S3_SAVE, (UINT16)0xFFFB, BIT2);
      //RPR 7.29  SB02703
      RWPCI ((UINT32) Value + SB_EHCI_REG54 + 2, AccWidthUint16 | S3_SAVE, (UINT16)0xFFF7, BIT3);
    }

    // RPR USB Delay A-Link Express L1 State
    // RPR PING Response Fix Enable EHCI_PCI_Config  x54[1] = 1
    // RPR Enable empty list mode. x54[3]
    // RPR Enable "L1 Early Exit" functionality. 0x54 [6:5] = 0x3 0x54 [9:7] = 0x4
    // RPR EHCI PING Response Fix Enable 0x54 [1] = 0x1
    RWPCI ((UINT32) Value + SB_EHCI_REG54, AccWidthUint32 | S3_SAVE, ~BIT0, 0x0000026b);
    if ( pConfig->BuildParameters.UsbMsi) {
      RWPCI ((UINT32) Value + SB_EHCI_REG50, AccWidthUint32 | S3_SAVE, ~BIT6, 0x00);
    }
    if (IsSbA12Plus ()) {
      // RPR 7.24 Long Delay on Framelist Read Causing EHCI DMA to Address 0 - Fix
      // RWPCI ((UINT32) Value + SB_EHCI_REG54, AccWidthUint32 | S3_SAVE, ~BIT13, BIT13);
      // RPR 7.25 LS connection can't wake up system from S3/S4/S5 when EHCI owns the port - Fix
      RWPCI ((UINT32) Value + SB_EHCI_REG54, AccWidthUint32 | S3_SAVE, ~BIT4, BIT4);
      // RPR 7.30 EHCI lMU Hangs when Run/Stop is Set First and PDC is Enabled Near End uFrame 7 - Fix Enable
      RWPCI ((UINT32) Value + SB_EHCI_REG54, AccWidthUint32 | S3_SAVE, ~BIT11, BIT11);
    }
    // ENH244924: Platform specific Hudson-2/3 settings for all Sabine Platforms
    // EHCI_PCI_Config 0x50[21]=1
    // EHCI_PCI_Config 0x54[9:7] = 0x4
    if ( IsGCPU () ) {
      RWPCI ((UINT32) Value + SB_EHCI_REG50, AccWidthUint32 | S3_SAVE, ~ ((UINT32) (0x01 << 21)), (0x01 << 21));
      RWPCI ((UINT32) Value + SB_EHCI_REG54, AccWidthUint32 | S3_SAVE, ~ ((UINT32) (0x07 << 7)), (0x04 << 7));
    }
//    RWMEM (ddBarAddress + SB_EHCI_BAR_REGB0, AccWidthUint32, ~BIT5, BIT5);
  } else {
    // Fake Bar
    ddBarAddress = 0x58830000;
    WritePCI ((UINT32) Value + SB_EHCI_REG10, AccWidthUint32, &ddBarAddress);
    //Enable Memory access
    RWPCI ((UINT32) Value + SB_EHCI_REG04, AccWidthUint8, 0, BIT1);
    // RPR Enable Global Clock Gating (BIT14)
    RWMEM (ddBarAddress + SB_EHCI_BAR_REGBC, AccWidthUint32, ~(BIT12 + BIT14), BIT12 + BIT14);
    RWMEM (ddBarAddress + SB_EHCI_BAR_REGB0, AccWidthUint32, ~BIT5, BIT5);
    RWPCI ((UINT32) Value + SB_EHCI_REG04, AccWidthUint8, 0, 0);
//    RWMEM (ddBarAddress + SB_EHCI_BAR_REGB0, AccWidthUint32, ~BIT5, BIT5);
  }
}

/**
 * usb1OhciInitAfterPciInit - Config USB1 OHCI controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
usb1OhciInitAfterPciInit (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32  ddDeviceId;
  ddDeviceId = (USB1_OHCI_BUS_DEV_FUN << 16);
  OhciInitAfterPciInit (ddDeviceId, pConfig);
  RWPCI ((USB1_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG78, AccWidthUint32 | S3_SAVE, 0xffffffff, BIT28 + BIT29);
}

/**
 * usb2OhciInitAfterPciInit - Config USB2 OHCI controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
usb2OhciInitAfterPciInit (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32  ddDeviceId;
  ddDeviceId = (USB2_OHCI_BUS_DEV_FUN << 16);
  OhciInitAfterPciInit (ddDeviceId, pConfig);
}

/**
 * usb3OhciInitAfterPciInit - Config USB3 OHCI controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
usb3OhciInitAfterPciInit (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32  ddDeviceId;
  ddDeviceId = (USB3_OHCI_BUS_DEV_FUN << 16);
  OhciInitAfterPciInit (ddDeviceId, pConfig);
}

/**
 * usb4OhciInitAfterPciInit - Config USB4 OHCI controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
usb4OhciInitAfterPciInit (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32  ddDeviceId;
  ddDeviceId = (USB4_OHCI_BUS_DEV_FUN << 16);
  OhciInitAfterPciInit (ddDeviceId, pConfig);
  if (pConfig->BuildParameters.Ohci4Ssid != NULL ) {
    RWPCI ((USB4_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pConfig->BuildParameters.Ohci4Ssid);
  }
}

VOID
OhciInitAfterPciInit (
  IN       UINT32 Value,
  IN       AMDSBCFG* pConfig
  )
{
#ifdef SB_USB_BATTERY_CHARGE_SUPPORT
  RWPCI ((UINT32) Value + SB_OHCI_REG40 + 6, AccWidthUint8 | S3_SAVE, 0xFF, 0x1F);
#endif
  // Disable the MSI capability of USB host controllers
  RWPCI ((UINT32) Value + SB_OHCI_REG40 + 1, AccWidthUint8 | S3_SAVE, 0xFF, BIT0);
  if ((IsSbA11 ()) || (IsSbA12 ())) {
    RWPCI ((UINT32) Value + SB_OHCI_REG50, AccWidthUint8 | S3_SAVE, ~(BIT0 + BIT5 + BIT12), 0);
  } else {
    RWPCI ((UINT32) Value + SB_OHCI_REG50, AccWidthUint8 | S3_SAVE, ~(BIT0 + BIT5 + BIT12), BIT0);
  }
  // RPR USB SMI Handshake
  RWPCI ((UINT32) Value + SB_OHCI_REG50 + 1, AccWidthUint8 | S3_SAVE, ~BIT4, 0x00);
  if (Value != (USB4_OHCI_BUS_DEV_FUN << 16)) {
    if ( pConfig->BuildParameters.OhciSsid != NULL ) {
      RWPCI ((UINT32) Value + SB_OHCI_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pConfig->BuildParameters.OhciSsid);
    }
  }
  //RPR recommended setting to, enable fix to cover the corner case S3 wake up issue from some USB 1.1 devices
  //OHCI 0_PCI_Config 0x50[30] = 1
  RWPCI ((UINT32) Value + SB_OHCI_REG50 + 3, AccWidthUint8 | S3_SAVE, ~BIT6, BIT6);
  // RPR L1 Early Exit
  // RPR Set OHCI Arbiter Mode.
  // RPR Set Enable Global Clock Gating.
  RWPCI ((UINT32) Value + SB_OHCI_REG80, AccWidthUint8 | S3_SAVE, ~(BIT0 + BIT4 + BIT5 + BIT6 + BIT7), BIT0 + BIT4 + BIT7);
  //   SB02698
  RWPCI ((UINT32) Value + SB_OHCI_REG50, AccWidthUint8 | S3_SAVE, ~BIT0, 0);
  // RPR 7.21 OHCI Arbiter Mode
  // A13 Remove Arbitor Reset
  if ( IsSbA12Plus () ) {
    RWPCI ((UINT32) Value + SB_OHCI_REG80, AccWidthUint16 | S3_SAVE, ~(BIT4 + BIT5 + BIT8), (BIT4 + BIT5 + BIT8));
  } else {
    RWPCI ((UINT32) Value + SB_OHCI_REG80, AccWidthUint16 | S3_SAVE, ~(BIT4 + BIT5), (BIT4));
  }
  // RPR Enable OHCI SOF Synchronization.
  // RPR Enable OHCI Periodic List Advance.
  RWPCI ((UINT32) Value + SB_OHCI_REG50 + 2, AccWidthUint8 | S3_SAVE, ~(BIT3 + BIT4), BIT3 + BIT4);
  if ( pConfig->BuildParameters.UsbMsi) {
    RWPCI ((UINT32) Value + SB_OHCI_REG40 + 1, AccWidthUint8 | S3_SAVE, ~BIT0, 0x00);
    RWPCI ((UINT32) Value + SB_OHCI_REG50, AccWidthUint8 | S3_SAVE, ~BIT5, BIT5);
  }
  //7.23  USB1.1 full-speed false crc errors detected. Issue - fix enable
  if ( IsSbA12Plus () ) {
    RWPCI ((UINT32) Value + SB_OHCI_REG80, AccWidthUint32 | S3_SAVE, (UINT32) (~(0x01 << 10)), (UINT32) (0x01 << 10));
  }
}


UINT32
SetEhciPllWr (
  IN       UINT32 Value,
  IN       AMDSBCFG* pConfig
  )
{
  UINT32  ddRetureValue;
  UINT32  ddBarAddress;
  UINT16  dwVar;
  UINT16  dwData;
  UINT8  portSC;
  ddRetureValue = 0;
  dwData = 0;
  // Memory, and etc.
  //_asm { jmp $};
  RWPCI ((UINT32) Value + 0xC4, AccWidthUint8, 0xF0, 0x00);
  RWPCI ((UINT32) Value + 0x04, AccWidthUint8, 0xFF, 0x02);
  // Get Bar address
  ReadPCI ((UINT32) Value + 0x10, AccWidthUint32, &ddBarAddress);
  for (portSC = 0x64; portSC < 0x75; portSC += 0x04 ) {
    // Get OHCI command registers
    ReadMEM (ddBarAddress + portSC, AccWidthUint16, &dwVar);
    if ( dwVar & BIT6 ) {
      ddRetureValue = ddBarAddress + portSC;
      RWMEM (ddBarAddress + portSC, AccWidthUint16, ~BIT6, 0);
      for (;;) {
        SbStall (5);
        ReadMEM (ddBarAddress + portSC, AccWidthUint16, &dwData);
        if (dwData == 0x1005) break;
      }
      dwData = 0;
    }
  }
  return ddRetureValue;
}

#ifdef XHCI_SUPPORT
/*
VOID
XhciInitIndirectReg (
  )
{
  UINT32 ddDrivingStrength;
  UINT32 port;
  ddDrivingStrength = 0;
  port = 0;
#ifdef SB_USB_BATTERY_CHARGE_SUPPORT
  RWXhciIndReg ( 0x40000018, 0xFFFFFFFF, 0x00000030);
#endif
//
// RPR SuperSpeed PHY Configuration (adaptation mode setting)
//
  RWXhciIndReg ( SB_XHCI_IND_REG94, 0xFFFFFC00, 0x00000021);
  RWXhciIndReg ( SB_XHCI_IND_REGD4, 0xFFFFFC00, 0x00000021);
//
// RPR SuperSpeed PHY Configuration (CR phase and frequency filter settings)
//
  RWXhciIndReg ( SB_XHCI_IND_REG98, 0xFFFFFFC0, 0x0000000A);
  RWXhciIndReg ( SB_XHCI_IND_REGD8, 0xFFFFFFC0, 0x0000000A);

//
// RPR BLM Meaasge
//
  RWXhciIndReg ( SB_XHCI_IND_REG00, 0xF8FFFFFF, 0x07000000);

#ifdef USB3_EHCI_DRIVING_STRENGTH
//
// RPR 8.13 xHCI USB 2.0 PHY Settings
// Step 1 is done by hardware default
// Step 2
  for (port = 0; port < 4; port ++) {
    ddDrivingStrength = (USB3_EHCI_DRIVING_STRENGTH >> (port * 4)) & 0xF;
    if (ddDrivingStrength & BIT3) {
      ddDrivingStrength &= 0x07;
      if (port < 2) {
        RWXhci0IndReg ( SB_XHCI_IND60_REG00, 0xFFFE0E78, (port << 13) + ddDrivingStrength);
        RWXhci0IndReg ( SB_XHCI_IND60_REG00, 0xFFFFEFFF, 0x00001000);
      } else {
        RWXhci1IndReg ( SB_XHCI_IND60_REG00, 0xFFFE0E78, ((port - 2) << 13) + ddDrivingStrength);
        RWXhci1IndReg ( SB_XHCI_IND60_REG00, 0xFFFFEFFF, 0x00001000);
      }
    }
  }
#endif

#ifdef USB_LOGO_SUPPORT
//for D3 USB LOGO [6:0]= 4; [8:7] = 3; [12] = 0; [16:13] port;
  for (port = 0; port < 4; port ++) {
    if (port < 2) {
      RWXhci0IndReg ( SB_XHCI_IND60_REG00, 0xFFFE0E00, (port << 13) + 0x184 );
    } else {
      RWXhci1IndReg ( SB_XHCI_IND60_REG00, 0xFFFE0E00, ((port - 2) << 13) + 0x184 );
    }
  }
#endif

// Step 3
  if (IsSbA11 ()) {
    RWXhciIndReg ( SB_XHCI_IND60_REG0C, ~ ((UINT32) (0x0f << 8)), ((UINT32) (0x00 << 8)));
    RWXhciIndReg ( SB_XHCI_IND60_REG08, ~ ((UINT32) (0xff << 8)), ((UINT32) (0x15 << 8)));
  } else {
    RWXhciIndReg ( SB_XHCI_IND60_REG0C, ~ ((UINT32) (0x0f << 8)), ((UINT32) (0x02 << 8)));
    RWXhciIndReg ( SB_XHCI_IND60_REG08, ~ ((UINT32) (0xff << 8)), ((UINT32) (0x0f << 8)));
  }
}
*/

VOID
XhciA12Fix (
  VOID
  )
{
//
// 8.14 PLUG/UNPLUG of USB 2.0 devices make the XHCI USB 2.0 ports unfunctional - fix enable
// ACPI_USB3.0_REG 0x20[12:11] = 2'b11
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccWidthUint32, ~((UINT32) (0x3 << 11)), (UINT32) (0x3 << 11));
//
// 8.15 XHC 2 USB2 ports interactional issue - fix enable
// ACPI_USB3.0_REG 0x20[16] = 1'b1
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccWidthUint32, ~((UINT32) (0x1 << 16)), (UINT32) (0x1 << 16));
//
// 8.16 xHCI USB 2.0 Ports Suspend Enhancement
// ACPI_USB3.0_REG 0x20[15] = 1'b1
//
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccWidthUint32, ~((UINT32) (0x1 << 15)), (UINT32) (0x1 << 15));
//
// 8.17 XHC HS/FS IN Data Buffer Underflow issue - fix enable
// ACPI_USB3.0_REG 0x20[20:18] = 0x7
//
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccWidthUint32, ~((UINT32) (0x7 << 18)), (UINT32) (0x7 << 18));
//
// 8.18 Allow XHCI to Resume from S3 Enhancement
// ACPI_USB3.0_REG 0x98[19] = 1'b1
//
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG98, AccWidthUint32, ~((UINT32) (0x1 << 19)), (UINT32) (0x1 << 19));
//
// 8.19 Assign xHC1 ( Dev 16 function 1) Interrupt Pin register to INTB#
// ACPI_PMIO_F0[18] =1
//
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF0, AccWidthUint32, ~((UINT32) (0x1 << 18)), (UINT32) (0x1 << 18));
//
// 8.20 Allow B-Link Clock Gating when EHCI3/OHCI3 is only Controller Enabled
// ACPI_PMIO_F0[13] =1
//
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF0, AccWidthUint32, ~((UINT32) (0x1 << 13)), (UINT32) (0x1 << 13));
//
// 8.21 Access register through JTAG fail when switch from XHCI to EHCI/OHCI - Fix enable
// ACPI_PMIO_F0[17] =1
//
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF0, AccWidthUint32, ~((UINT32) (0x1 << 17)), (UINT32) (0x1 << 17));
//
// 8.22 USB leakage current on differential lines when ports are switched to XHCI - Fix enable
// ACPI_PMIO_F0[14] =1
//
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF0, AccWidthUint32, ~((UINT32) (0x1 << 14)), (UINT32) (0x1 << 14));
//  RWMEM (ACPI_MMIO_BASE + SMI_BASE + SB_SMI_xHC0Pme, AccWidthUint16, 0, 0x0B0B);
//
// 8.26 Fix for Incorrect Gated Signals in xhc_to_s5
// ACPI_PMIO_F0[16] =1
//
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF0, AccWidthUint32, ~((UINT32) (0x1 << 16)), (UINT32) (0x1 << 16));
}

VOID
XhciInitBeforePciInit (
  IN       AMDSBCFG* pConfig
  )
{
  UINT16  BcdAddress;
  UINT16  BcdSize;
  UINT16  AcdAddress;
  UINT16  AcdSize;
  UINT16  FwAddress;
  UINT16  FwSize;
  UINTN   XhciFwStarting;
  UINT32  SpiValidBase;
  UINT32  RegData;
  UINT16  i;

  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccWidthUint32, 0x00000000, 0x00400700);
  SbStall (20);
//
// Get ROM SIG starting address for USB firmware starting address (offset 0x0C to SIG address)
//
  GetRomSigPtr (&XhciFwStarting);

  if (XhciFwStarting == 0) {
    return;
  }

  XhciFwStarting = ACPIMMIO32 (XhciFwStarting + FW_TO_SIGADDR_OFFSET);
  if (IsLpcRom ()) {
  //XHCI firmware re-load
    RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGCC, AccWidthUint32 | S3_SAVE, ~BIT2, (BIT2 + BIT1 + BIT0));
    RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGCC, AccWidthUint32 | S3_SAVE, 0x00000FFF, (UINT32) (XhciFwStarting));
  }
//
// RPR Enable SuperSpeed receive special error case logic. 0x20 bit8
// RPR Enable USB2.0 RX_Valid Synchronization. 0x20 bit9
// Enable USB2.0 DIN/SE0 Synchronization. 0x20 bit10
//
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccWidthUint32, 0xFFFFF8FF, 0x00000700);
//
// RPR SuperSpeed PHY Configuration (adaptation timer setting)
//
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG90, AccWidthUint32, 0xFFF00000, 0x000AAAAA);
  //RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG90 + 0x40, AccWidthUint32, 0xFFF00000, 0x000AAAAA);

//
// Step 1. to enable Xhci IO and Firmware load mode
//
#ifdef XHCI_SUPPORT_ONE_CONTROLLER
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccWidthUint32, 0xF0FFFFFC, 0x00000001);
#else
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccWidthUint32, 0xF0FFFFFC, 0x00000003);
#endif
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccWidthUint32, 0xEFFFFFFF, 0x10000000);

//
// Step 2. to read a portion of the USB3_APPLICATION_CODE from BIOS ROM area and program certain registers.
//

  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGA0, AccWidthUint32, 0x00000000, (SPI_HEAD_LENGTH << 16));

  BcdAddress = ACPIMMIO16 (XhciFwStarting + BCD_ADDR_OFFSET);
  BcdSize = ACPIMMIO16 (XhciFwStarting + BCD_SIZE_OFFSET);
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGA4, AccWidthUint16, 0x0000, BcdAddress);
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGA4 + 2, AccWidthUint16, 0x0000, BcdSize);

  AcdAddress = ACPIMMIO16 (XhciFwStarting + ACD_ADDR_OFFSET);
  AcdSize = ACPIMMIO16 (XhciFwStarting + ACD_SIZE_OFFSET);
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGA8, AccWidthUint16, 0x0000, AcdAddress);
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGA8 + 2, AccWidthUint16, 0x0000, AcdSize);

  SpiValidBase = SPI_BASE2 (XhciFwStarting + 4) | SPI_BAR0_VLD | SPI_BASE0 | SPI_BAR1_VLD | SPI_BASE1 | SPI_BAR2_VLD;
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGB0, AccWidthUint32, 0x00000000, SpiValidBase);

    //
    // Copy Type0/1/2 data block from ROM image to MMIO starting from 0xC0
    //
  for (i = 0; i < SPI_HEAD_LENGTH; i++) {
    RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGC0 + i, AccWidthUint8, 0, ACPIMMIO8 (XhciFwStarting + i));
  }

  for (i = 0; i < BcdSize; i++) {
    RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGC0 + SPI_HEAD_LENGTH + i, AccWidthUint8, 0, ACPIMMIO8 (XhciFwStarting + BcdAddress + i));
  }

  for (i = 0; i < AcdSize; i++) {
    RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGC0 + SPI_HEAD_LENGTH + BcdSize + i, AccWidthUint8, 0, ACPIMMIO8 (XhciFwStarting + AcdAddress + i));
  }

//
// Step 3. to enable the instruction RAM preload functionality.
//
  FwAddress = ACPIMMIO16 (XhciFwStarting + FW_ADDR_OFFSET);
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGB4, AccWidthUint16, 0x0000, ACPIMMIO16 (XhciFwStarting + FwAddress));
  FwAddress += 2;
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG04, AccWidthUint16, 0x0000, FwAddress);

  FwSize = ACPIMMIO16 (XhciFwStarting + FW_SIZE_OFFSET);
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG04 + 2, AccWidthUint16, 0x0000, FwSize);

    //
    // Set the starting address offset for Instruction RAM preload.
    //
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG08, AccWidthUint16, 0x0000, 0);

  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccWidthUint32, ~BIT29, BIT29);

  for (;;) {
    ReadMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00 , AccWidthUint32, &RegData);
    if (RegData & BIT30) break;
  }
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccWidthUint32, ~BIT29, 0);

//
// Step 4. to release resets in XHCI_ACPI_MMIO_AMD_REG00. wait for USPLL to lock by polling USPLL lock.
//

  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccWidthUint32, ~U3PLL_RESET, 0); //Release U3PLLreset
  for (;;) {
    ReadMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00 , AccWidthUint32, &RegData);
    if (RegData & U3PLL_LOCK) break;
  }

  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccWidthUint32, ~U3PHY_RESET, 0); //Release U3PHY
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccWidthUint32, ~U3CORE_RESET, 0); //Release core reset

// RPR 8.8 SuperSpeed PHY Configuration, it is only for A11.
  if (IsSbA11 ()) {
    RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG90, AccWidthUint32, 0xFFF00000, 0x000AAAAA); //
    RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGD0, AccWidthUint32, 0xFFF00000, 0x000AAAAA); //
  }

  XhciInitIndirectReg ();

  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEF, AccWidthUint8, ~(BIT4 + BIT5), 0); // Disable Device 22
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEF, AccWidthUint8, ~(BIT7), BIT7); // Enable 2.0 devices
  if (!(pConfig->S4Resume)) {
    RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccWidthUint32, ~(BIT21), BIT21); //SMI
  }
//
// Step 5.
//
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccWidthUint32, ~(BIT17 + BIT18 + BIT19), BIT17 + BIT18);

// Step 5.
  if (IsSbA12Plus ()) {
    XhciA12Fix ();
  }
  //8.22 UMI Lane Configuration Information for XHCI Firmware to Calculate the Bandwidth for USB 3.0 ISOC Devices
  if (!(IsUmiOneLaneGen1Mode ())) {
    RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccWidthUint32, ~(BIT25 + BIT24), BIT24);
  }
}

VOID
XhciInitAfterPciInit (
  IN       AMDSBCFG* pConfig
  )
{
  // RPR8.12 Block Write to DID & SID to pass DTM
  RWXhciIndReg ( SB_XHCI_IND_REG04, ~BIT8, BIT8);

  if (IsSbA13Plus ()) {
    //8.23 FS/LS devices not functional after resume from S4 fix enable (SB02699)
    RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccWidthUint32, ~(BIT22), BIT22);
  }
  //8.24 XHC USB2.0 Hub disable issue fix enable (SB02702)
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGB4, AccWidthUint32, ~(BIT20), BIT20);
}

VOID
XhciInitLate (
  IN       AMDSBCFG* pConfig
  )
{
  //OBS221599: USB 3.0 controller shows bang in Windows Device Manager
  UINT8  Data;
  if ( IsSbA11 () ) {
    Data = 0xB5; //
    WriteIO (SB_IOMAP_REGC00, AccWidthUint8, &Data);
    Data = 0x12; //
    WriteIO (SB_IOMAP_REGC01, AccWidthUint8, &Data);
  }
  if (pConfig->S4Resume) {
    RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccWidthUint32, ~(BIT21), BIT21); //SMI
    SbFlashUsbSmi ();
  }
}
#endif
