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
 * 
 * ***************************************************************************
 *
 */


#include "SBPLATFORM.h"
#include "cbtypes.h"

//
// Declaration of local functions
//

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

/**
 * SetEhciP11Wr - FIXME
 *
 * @param[in] Value  Controller PCI config address (bus# + device# + function#)
 * @param[in] pConfig Southbridge configuration structure pointer.
 */
UINT32 SetEhciPllWr (IN UINT32 Value, IN AMDSBCFG* pConfig);


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
  // Disabled All USB controller
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEF, AccWidthUint8, BIT7, 0);
  // Clear PM_IO 0x65[4] UsbResetByPciRstEnable, Set this bit so that usb gets reset whenever there is PCIRST.
  // Enable UsbResumeEnable (USB PME) * Default value
  // In SB700 USB SleepCtrl set as BIT10+BIT9, but SB800 default is BIT9+BIT8 (6 uframes)
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF0, AccWidthUint16 | S3_SAVE, ~BIT2, BIT2 + BIT7 + BIT8 + BIT9);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEF, AccWidthUint8, 0, pConfig->USBMODE.UsbModeReg);
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
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGED, AccWidthUint8, ~BIT1, BIT1);

  usb1EhciInitAfterPciInit (pConfig);
  usb2EhciInitAfterPciInit (pConfig);
  usb3EhciInitAfterPciInit (pConfig);
  usb1OhciInitAfterPciInit (pConfig);
  usb2OhciInitAfterPciInit (pConfig);
  usb3OhciInitAfterPciInit (pConfig);
  usb4OhciInitAfterPciInit (pConfig);

  if ( pConfig->UsbPhyPowerDown ) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF0, AccWidthUint8, ~BIT0, BIT0);
  } else
  {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF0, AccWidthUint8, ~BIT0, 0);
  }

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
    RWMEM (ddBarAddress + SB_EHCI_BAR_REGBC, AccWidthUint32, ~BIT12, 0);
    // RPR Enable adding extra flops to PHY rsync path
    // Step 1:
    //  EHCI_BAR 0xB4 [6] = 1
    //  EHCI_BAR 0xB4 [7] = 0
    //  EHCI_BAR 0xB4 [12] = 0 ("VLoad")
    //  All other bit field untouched
    // Step 2:
    //  EHCI_BAR 0xB4[12] = 1
    RWMEM (ddBarAddress + SB_EHCI_BAR_REGB4, AccWidthUint32, ~(BIT6 + BIT7 + BIT12), 0x00);
    RWMEM (ddBarAddress + SB_EHCI_BAR_REGB4, AccWidthUint32, ~BIT12, BIT12);
    //Set EHCI_pci_configx50[6]='1' to disable EHCI MSI support
    //RPR recommended setting "EHCI Async Park Mode"
    //Set EHCI_pci_configx50[23]='0' to enable "EHCI Async Park Mode support"
    //RPR Enabling EHCI Async Stop Enhancement
    //Set EHCI_pci_configx50[29]='1' to disableEnabling EHCI Async Stop Enhancement
    RWPCI ((UINT32) Value + SB_EHCI_REG50, AccWidthUint32 | S3_SAVE, ~(BIT23), BIT29 + BIT23 + BIT8 + BIT6);
    // RPR recommended setting "EHCI Advance PHY Power Savings"
    // Set EHCI_pci_configx50[31]='1'
    // Fix for EHCI controller driver  yellow sign issue under device manager
    // when used in conjunction with HSET tool driver. EHCI PCI config 0x50[20]=1
    RWPCI ((UINT32) Value + SB_EHCI_REG50 + 2, AccWidthUint16 | S3_SAVE, (UINT16)0xFFFF, BIT15);
    // RPR USB Delay A-Link Express L1 State
    // RPR PING Response Fix Enable EHCI_PCI_Config  x54[1] = 1
    // RPR Empty-list Detection Fix Enable EHCI_PCI_Config  x54[3] = 1
    RWPCI ((UINT32) Value + SB_EHCI_REG54, AccWidthUint32 | S3_SAVE, ~BIT0, BIT0);
    if ( pConfig->BuildParameters.UsbMsi) {
      RWPCI ((UINT32) Value + SB_EHCI_REG50, AccWidthUint32 | S3_SAVE, ~BIT6, 0x00);
    }
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
  // Disable the MSI capability of USB host controllers
  RWPCI ((UINT32) Value + SB_OHCI_REG40 + 1, AccWidthUint8 | S3_SAVE, 0xFF, BIT0);
  RWPCI ((UINT32) Value + SB_OHCI_REG50, AccWidthUint8 | S3_SAVE, ~(BIT5 + BIT12), 0x00);
  // RPR USB SMI Handshake
  RWPCI ((UINT32) Value + SB_OHCI_REG50 + 1, AccWidthUint8 | S3_SAVE, ~BIT4, 0x00);
  // SB02186
  RWPCI ((UINT32) Value + SB_OHCI_REG50 + 1, AccWidthUint8 | S3_SAVE, 0xFC, 0x00);
  if (Value != (USB4_OHCI_BUS_DEV_FUN << 16)) {
    if ( pConfig->BuildParameters.OhciSsid != NULL ) {
      RWPCI ((UINT32) Value + SB_OHCI_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pConfig->BuildParameters.OhciSsid);
    }
  }
  //RPR recommended setting to, enable fix to cover the corner case S3 wake up issue from some USB 1.1 devices
  //OHCI 0_PCI_Config 0x50[30] = 1
  RWPCI ((UINT32) Value + SB_OHCI_REG50 + 3, AccWidthUint8 | S3_SAVE, ~BIT6, BIT6);
  if ( pConfig->BuildParameters.UsbMsi) {
    RWPCI ((UINT32) Value + SB_OHCI_REG40 + 1, AccWidthUint8 | S3_SAVE, ~BIT0, 0x00);
    RWPCI ((UINT32) Value + SB_OHCI_REG50, AccWidthUint8 | S3_SAVE, ~BIT5, BIT5);
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

VOID
usbSetPllDuringS3 (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32  resumeEhciPortTmp;
  UINT32  resumeEhciPort;
  resumeEhciPortTmp = 0;
  resumeEhciPort = 0;
//  UINT32  ddDeviceId;
//if Force Port Resume == 1
// {
//     clear Force Port Resume;
//     while (!(PORTSC == 0x1005)){wait 5 us; read PORTSC;}
// }
  if (pConfig->USBMODE.UsbModeReg & BIT1) {
    resumeEhciPortTmp = SetEhciPllWr (USB1_EHCI_BUS_DEV_FUN << 16, pConfig);
    if (resumeEhciPortTmp > 0) resumeEhciPort = resumeEhciPortTmp;
  }
  if (pConfig->USBMODE.UsbModeReg & BIT3) {
    resumeEhciPortTmp = SetEhciPllWr (USB2_EHCI_BUS_DEV_FUN << 16, pConfig);
    if (resumeEhciPortTmp > 0) resumeEhciPort = resumeEhciPortTmp;
  }
  if (pConfig->USBMODE.UsbModeReg & BIT5) {
    resumeEhciPortTmp = SetEhciPllWr (USB3_EHCI_BUS_DEV_FUN << 16, pConfig);
    if (resumeEhciPortTmp > 0) resumeEhciPort = resumeEhciPortTmp;
  }

  RWPCI ((UINT32) (USB1_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG50, AccWidthUint32, ~BIT29, 0);
  RWPCI ((UINT32) (USB2_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG50, AccWidthUint32, ~BIT29, 0);
  RWPCI ((UINT32) (USB3_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG50, AccWidthUint32, ~BIT29, 0);
  RWPCI ((UINT32) (USB4_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG50, AccWidthUint32, ~BIT29, 0);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF3, AccWidthUint8, 0, 0x20);
  SbStall (10);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF3, AccWidthUint8, 0, 0x00);
  RWPCI ((UINT32) (USB1_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG50, AccWidthUint32, ~BIT29, BIT29);
  RWPCI ((UINT32) (USB2_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG50, AccWidthUint32, ~BIT29, BIT29);
  RWPCI ((UINT32) (USB3_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG50, AccWidthUint32, ~BIT29, BIT29);
  RWPCI ((UINT32) (USB4_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG50, AccWidthUint32, ~BIT29, BIT29);

  if (resumeEhciPort > 0) {
    RWMEM (resumeEhciPort, AccWidthUint8, ~BIT7, BIT7);
    SbStall (4000);
    RWMEM (resumeEhciPort, AccWidthUint8, ~BIT6, BIT6);
  }

  RWPCI ((UINT32) (USB1_EHCI_BUS_DEV_FUN << 16) + 0xC4, AccWidthUint8, 0xF0, 0x03);
  RWPCI ((UINT32) (USB2_EHCI_BUS_DEV_FUN << 16) + 0xC4, AccWidthUint8, 0xF0, 0x03);
  RWPCI ((UINT32) (USB3_EHCI_BUS_DEV_FUN << 16) + 0xC4, AccWidthUint8, 0xF0, 0x03);

}

