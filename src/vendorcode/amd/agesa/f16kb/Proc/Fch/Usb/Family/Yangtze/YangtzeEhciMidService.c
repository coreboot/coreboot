/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Yangtze FCH USB EHCI controller
 *
 * Init USB EHCI features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 87830 $   @e \$Date: 2013-02-11 12:48:20 -0600 (Mon, 11 Feb 2013) $
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
#define FILECODE PROC_FCH_USB_FAMILY_YANGTZE_YANGTZEEHCIMIDSERVICE_FILECODE
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
  UINT8                  Index;
  UINT32                 BarAddress;
  UINT32                 Var;
  UINT8                  UsbS3WakeResumeOnlyDisable;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;
  UINT32                 PortNum;
  UINT32                 DrivingStrength;
  UINT8                  RetEfuseValue;
  UINT32                 UsbFuseCommonCalibrationValue;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  //
  //Get BAR address
  //
  ReadPci ((UINT32) Value + FCH_EHCI_REG10, AccessWidth32, &BarAddress, StdHeader);
  if ( (BarAddress != - 1) && (BarAddress != 0) ) {
    //
    //Enable Memory access
    //
    RwPci ((UINT32) Value + FCH_EHCI_REG04, AccessWidth8, 0, BIT1, StdHeader);
    if (FchDataPtr->Usb.EhciSsid != 0 ) {
      RwPci ((UINT32) Value + FCH_EHCI_REG2C, AccessWidth32, 0x00, FchDataPtr->Usb.EhciSsid, StdHeader);
    }
    RwMem (BarAddress + FCH_EHCI_BAR_REGA4, AccessWidth32, 0xFF00FF00, 0x00400040);
    RwMem (BarAddress + FCH_EHCI_BAR_REGBC, AccessWidth32, (UINT32)~( BIT12 + BIT14), BIT12 + BIT14);
    RwMem (BarAddress + 0x0B0, AccessWidth32, (UINT32)~BIT5, BIT5);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGF0, AccessWidth16, (UINT32)~BIT12, BIT12);
    for (PortNum = 0; PortNum < 5; PortNum ++) {
      if (Value == (USB1_EHCI_BUS_DEV_FUN << 16)) {
        DrivingStrength = (UINT32) (LocalCfgPtr->Usb.Ehci1Phy[PortNum]);
      } else if (Value == (USB2_EHCI_BUS_DEV_FUN << 16)) {
        DrivingStrength = (UINT32) (LocalCfgPtr->Usb.Ehci2Phy[PortNum]);
      } else if ((Value == (USB3_EHCI_BUS_DEV_FUN << 16)) && (PortNum < 4)) {
        DrivingStrength = (UINT32) (LocalCfgPtr->Usb.Ehci3Phy[PortNum]);
      } else {
        break;
      }
      RwMem (BarAddress + FCH_EHCI_BAR_REGB4, AccessWidth32, 0xFFFE0000, (PortNum << 13) + DrivingStrength);
      RwMem (BarAddress + FCH_EHCI_BAR_REGB4, AccessWidth32, (UINT32)~BIT12, BIT12);
      DrivingStrength = 0x302;
      RwMem (BarAddress + FCH_EHCI_BAR_REGB4, AccessWidth32, 0xFFFFC000, (PortNum << 13) + BIT12 + DrivingStrength);
      Index = 0;
      do {
        ReadMem ( BarAddress + FCH_EHCI_BAR_REGB4, AccessWidth32, &Var);
        Index++;
        FchStall (10, StdHeader);
      } while (( Var & BIT17) && (Index < 10 ));
      Index = 0;
      RwMem (BarAddress + FCH_EHCI_BAR_REGB4, AccessWidth32, 0xFFFFC000, (PortNum << 13) + DrivingStrength);
      do {
        ReadMem ( BarAddress + FCH_EHCI_BAR_REGB4, AccessWidth32, &Var);
        Index++;
        FchStall (10, StdHeader);
      } while (( Var & BIT17) && (Index < 10 ));
      RwMem (BarAddress + FCH_EHCI_BAR_REGB4, AccessWidth32, 0xFFFFC000, (PortNum << 13) + BIT12 + DrivingStrength);
    }
    // Step3
    RwMem (BarAddress + FCH_EHCI_BAR_REGD0, AccessWidth32, ~((UINT32) (0x0F)), (UINT32) (0x6));
    RwMem (BarAddress + FCH_EHCI_BAR_REGC4, AccessWidth32, (UINT32) (~ 0xff00ffff), 0x90001221);
    RwMem (BarAddress + FCH_EHCI_BAR_REGD4, AccessWidth32, ~((UINT32) (0xC2)), (UINT32) (0x40));
    FchStall (200, StdHeader);
    RwMem (BarAddress + FCH_EHCI_BAR_REGD4, AccessWidth32, ~((UINT32) (0x02)), (UINT32) (0x02));
    FchStall (400, StdHeader);
    RwMem (BarAddress + FCH_EHCI_BAR_REGD4, AccessWidth32, ~((UINT32) (0x02)), (UINT32) (0x0));
    RetEfuseValue = FchUsbCommonPhyCalibration ( FchDataPtr );
    if ( RetEfuseValue == 0 ) {
      RwMem (BarAddress + FCH_EHCI_BAR_REGC0, AccessWidth32, (UINT32) (~ 0x00030000), BIT16);
    } else {
      UsbFuseCommonCalibrationValue = RetEfuseValue << 11;
      RwMem (BarAddress + FCH_EHCI_BAR_REGC0, AccessWidth32, (UINT32) (~ 0x0003FF00), UsbFuseCommonCalibrationValue);
    }
    RwPci ((UINT32) Value + 0x50, AccessWidth32, ~ ((UINT32) (0x01 << 6)), (UINT32) (0x01 << 6), StdHeader);
    RwPci ((UINT32) Value + 0x50, AccessWidth32, ~ ((UINT32) (0x0F << 8)), (UINT32) (0x01 << 8), StdHeader);
    RwPci ((UINT32) Value + 0x50, AccessWidth32, ~ ((UINT32) (0x0F << 12)), (UINT32) (0x01 << 12), StdHeader);
    RwPci ((UINT32) Value + 0x50, AccessWidth32, ~ ((UINT32) (0x01 << 17)), (UINT32) (0x01 << 17), StdHeader);
    RwPci ((UINT32) Value + 0x50, AccessWidth32, ~ ((UINT32) (0x01 << 21)), (UINT32) (0x01 << 21), StdHeader);

    RwPci ((UINT32) Value + 0x50, AccessWidth32, ~ ((UINT32) (0x01 << 29)), (UINT32) (0x01 << 29), StdHeader);
    RwPci ((UINT32) Value + 0x50 + 2, AccessWidth16, (UINT16)0xFFFF, BIT16 + BIT10, StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG54, AccessWidth16, 0xCC04, 0x0000027b, StdHeader);
    RwAlink ((FCH_ABCFG_REG90 | (UINT32) (ABCFG << 29)), 0xFFFEFFFF, BIT16, StdHeader);
    if ( FchDataPtr->Usb.UsbMsiEnable) {
      RwPci ((UINT32) Value + 0x50, AccessWidth32, (UINT32)~BIT6, 0x00, StdHeader);
    }
    RwPci ((UINT32) Value + FCH_EHCI_REG54, AccessWidth32, (UINT32)~BIT4, BIT4, StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG54, AccessWidth32, (UINT32)~BIT11, BIT11, StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG54, AccessWidth16, (UINT16)0x5FFF, BIT15, StdHeader);
    RwPci ((UINT32) Value + 0x50 + 2, AccessWidth16, (UINT32)~BIT3, BIT3, StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG54 + 2, AccessWidth16, (UINT16)0xFFFC, BIT0 + BIT1, StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG54 + 2, AccessWidth16, (UINT16)0xFFFB, BIT2, StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG54 + 2, AccessWidth16, (UINT16)0xFFF7, BIT3, StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG54 + 2, AccessWidth16, (UINT32)~BIT5, BIT5, StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG54 + 2, AccessWidth16, (UINT32)~BIT8, BIT8, StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG54 + 2, AccessWidth16, (UINT32)~BIT4, BIT4, StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG54 + 2, AccessWidth16, (UINT32)~BIT6, BIT6, StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG54 + 2, AccessWidth16, (UINT32)~BIT9, BIT9, StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG54 + 2, AccessWidth16, (UINT32)~BIT11, BIT11, StdHeader);
    RwPci ((UINT32) Value + 0x50, AccessWidth16, (UINT32)~BIT0, BIT0, StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG54 + 2, AccessWidth16, (UINT32)~BIT12, BIT12, StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG54 + 2, AccessWidth16, (UINT32)~BIT13, BIT13, StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG54, AccessWidth16, (UINT32)~BIT12, BIT12, StdHeader);
    RwPci ((UINT32) Value + FCH_EHCI_REG54 + 2, AccessWidth16, (UINT32)~BIT14, BIT14, StdHeader);
    for ( PortNum = 0; PortNum < 5; PortNum++ ) {
      RwMem (BarAddress + FCH_EHCI_BAR_REGB4, AccessWidth32, 0xFFFE0000, (UINT32) ((PortNum << 13) + BIT12 + (0x7 << 7)));
      if (PortNum < 4) {
        ReadMem (BarAddress + FCH_EHCI_BAR_REGA8, AccessWidth32, &Var);
        Var = (Var >> (PortNum * 8)) & 0x000000FF;
      } else {
        ReadMem (BarAddress + FCH_EHCI_BAR_REGAC, AccessWidth32, &Var);
        Var = Var & 0x000000FF;
      }
      Var &= 0xF8;
      Var |= BIT0 + BIT2;
      RwMem (BarAddress + FCH_EHCI_BAR_REGB4, AccessWidth32, 0xFFFE0000, (UINT32) ((PortNum << 13) + BIT12 + (0x7 << 7) + Var));
      RwMem (BarAddress + FCH_EHCI_BAR_REGB4, AccessWidth32, ~((UINT32) (1 << 12)), 0);
      RwMem (BarAddress + FCH_EHCI_BAR_REGB4, AccessWidth32, ~((UINT32) (1 << 12)), (UINT32) (0x1 << 12));
    }
  }

  ReadPmio (FCH_PMIOA_REGF0, AccessWidth8, &UsbS3WakeResumeOnlyDisable, StdHeader);
  if ( (UsbS3WakeResumeOnlyDisable &= BIT6) == BIT6 ) {
    RwPmio (FCH_PMIOA_REGF4, AccessWidth8, (UINT32)~BIT2, 0, StdHeader);
  } else {
    RwPmio (FCH_PMIOA_REGF4, AccessWidth8, (UINT32)~BIT2, BIT2, StdHeader);
  }
}

