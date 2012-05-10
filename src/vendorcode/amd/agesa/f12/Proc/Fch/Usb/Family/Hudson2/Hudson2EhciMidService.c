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
    if (FchDataPtr->Usb.EhciSsid != NULL ) {
      RwPci ((UINT32) Value + FCH_EHCI_REG2C, AccessWidth32, 0x00, FchDataPtr->Usb.EhciSsid, FchDataPtr->StdHeader);
    }
    //
    //USB Common PHY CAL & Control Register setting
    //
    Var = 0x00020F00;
    WriteMem (BarAddress + FCH_EHCI_BAR_REGC0, AccessWidth32, &Var);
    //
    // IN AND OUT DATA PACKET FIFO THRESHOLD
    // EHCI BAR 0xA4 //IN threshold bits[7:0]=0x40 //OUT threshold bits[23:16]=0x40
    //
    RwMem (BarAddress + FCH_EHCI_BAR_REGA4, AccessWidth32, 0xFF00FF00, 0x00400040);
    //
    // EHCI Dynamic Clock Gating Feature
    // Enable Global Clock Gating (BIT14)
    //
    RwMem (BarAddress + FCH_EHCI_BAR_REGBC, AccessWidth32, ~( BIT12 + BIT14), BIT12 + BIT14);
    RwMem (BarAddress + FCH_EHCI_BAR_REGB0, AccessWidth32, ~BIT5, BIT5);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGF0, AccessWidth16, ~BIT12, BIT12);
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
    RwMem (BarAddress + FCH_EHCI_BAR_REGB4, AccessWidth32, ~BIT12, BIT12);
    // Step3
    RwMem (BarAddress + FCH_EHCI_BAR_REGC4, AccessWidth32, (UINT32) (~ 0x00000f00), 0x00000200);
    RwMem (BarAddress + FCH_EHCI_BAR_REGC0, AccessWidth32, (UINT32) (~ 0x0000ff00), 0x00000f00);

    //Set EHCI_pci_configx50[6]='1' to disable EHCI MSI support
    RwPci ((UINT32) Value + FCH_EHCI_REG50, AccessWidth32, ~ ((UINT32) (0x01 << 6)), (UINT32) (0x01 << 6), FchDataPtr->StdHeader);
    // EHCI Async Park Mode
    //Set EHCI_pci_configx50[11:8]=0x1
    //Set EHCI_pci_configx50[15:12]=0x1
    //Set EHCI_pci_configx50[17]=0x1
    RwPci ((UINT32) Value + FCH_EHCI_REG50, AccessWidth32, ~ ((UINT32) (0x0F << 8)), (UINT32) (0x01 << 8), FchDataPtr->StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG50, AccessWidth32, ~ ((UINT32) (0x0F << 12)), (UINT32) (0x01 << 12), FchDataPtr->StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG50, AccessWidth32, ~ ((UINT32) (0x01 << 17)), (UINT32) (0x01 << 17), FchDataPtr->StdHeader);

    // Enabling EHCI Async Stop Enhancement
    //Set EHCI_pci_configx50[29]='1' to disableEnabling EHCI Async Stop Enhancement
    //
    RwPci ((UINT32) Value + FCH_EHCI_REG50, AccessWidth32, ~ ((UINT32) (0x01 << 29)), (UINT32) (0x01 << 29), FchDataPtr->StdHeader);
    //
    // recommended setting "EHCI Advance PHY Power Savings"
    // Set EHCI_pci_configx50[31]='1'
    // Fix for EHCI controller driver  yellow sign issue under device manager
    // when used in conjunction with HSET tool driver. EHCI PCI config 0x50[20]=1
    // Disable USB data cache to resolve USB controller hang issue with Lan adaptor.
    // EHCI PCI config register 50h bit 26 to `1'.
    //
    RwPci ((UINT32) Value + FCH_EHCI_REG50 + 2, AccessWidth16, (UINT16)0xFFFF, BIT16 + BIT10, FchDataPtr->StdHeader);
    //
    // USB Delay A-Link Express L1 State
    // PING Response Fix Enable EHCI_PCI_Config  x54[1] = 1
    // Enable empty list mode. x54[3]
    // Enable "L1 Early Exit" functionality. 0x54 [6:5] = 0x3 0x54 [9:7] = 0x4
    // EHCI PING Response Fix Enable 0x54 [1] = 0x1
    //
    RwPci ((UINT32) Value + FCH_EHCI_REG54, AccessWidth32, ~BIT0, 0x0000026b, FchDataPtr->StdHeader);
    if ( FchDataPtr->Usb.UsbMsiEnable) {
      RwPci ((UINT32) Value + FCH_EHCI_REG50, AccessWidth32, ~BIT6, 0x00, FchDataPtr->StdHeader);
    }
    // Long Delay on Framelist Read Causing EHCI DMA to Address 0 - Fix
    // RWPCI ((UINT32) Value + FCH_EHCI_REG54, AccWidthUint32 | S3_SAVE, ~BIT13, BIT13);
    // LS connection can't wake up system from S3/S4/S5 when EHCI owns the port - Fix
    RwPci ((UINT32) Value + FCH_EHCI_REG54, AccessWidth32, ~BIT4, BIT4, FchDataPtr->StdHeader);
    // EHCI lMU Hangs when Run/Stop is Set First and PDC is Enabled Near End uFrame 7 - Fix Enable
    RwPci ((UINT32) Value + FCH_EHCI_REG54, AccessWidth32, ~BIT11, BIT11, FchDataPtr->StdHeader);
    // RPR 7.25  SB02674
    RwPci ((UINT32) Value + FCH_EHCI_REG54, AccessWidth16, (UINT16)0x5FFF, BIT13 + BIT15, FchDataPtr->StdHeader);
    // RPR 7.26  SB02684
    RwPci ((UINT32) Value + FCH_EHCI_REG50 + 2, AccessWidth16, ~BIT3, BIT3, FchDataPtr->StdHeader);
    // RPR 7.26  SB02687
    RwPci ((UINT32) Value + FCH_EHCI_REG54 + 2, AccessWidth16, (UINT16)0xFFFC, BIT0 + BIT1, FchDataPtr->StdHeader);
    // RPR 7.28  SB02700
    RwPci ((UINT32) Value + FCH_EHCI_REG54 + 2, AccessWidth16, (UINT16)0xFFFB, BIT2, FchDataPtr->StdHeader);
    // RPR 7.29  SB02703
    RwPci ((UINT32) Value + FCH_EHCI_REG54 + 2, AccessWidth16, (UINT16)0xFFF7, BIT3, FchDataPtr->StdHeader);
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
    RwMem (BarAddress + FCH_EHCI_BAR_REGBC, AccessWidth32, ~( BIT12 + BIT14), BIT12 + BIT14);
    RwMem (BarAddress + FCH_EHCI_BAR_REGB0, AccessWidth32, ~BIT5, BIT5);
    RwPci ((UINT32) Value + FCH_EHCI_REG04, AccessWidth8, 0, 0, FchDataPtr->StdHeader);
  }
}

