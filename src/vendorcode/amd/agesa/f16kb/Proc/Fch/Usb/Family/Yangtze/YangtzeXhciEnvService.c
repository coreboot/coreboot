/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Yangtze FCH USB3 controller
 *
 * Init USB3 features.
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
#define FILECODE PROC_FCH_USB_FAMILY_YANGTZE_YANGTZEXHCIENVSERVICE_FILECODE

//
// Declaration of local functions
//

#define PRODUCT_INFO_REG1           0x1FC     // Product Information Register 1
#define AMD_CPU_DEV_FUN             ((0x18 << 3) + 3)


/**
 * FchUsbCommonPhyCalibration - Config USB Common PHY
 * Calibration
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
UINT8
FchUsbCommonPhyCalibration (
  IN  FCH_DATA_BLOCK     *FchDataPtr
  )
{
  UINT8                 RetEfuseValue;
  FCH_DATA_BLOCK        *LocalCfgPtr;
  AMD_CONFIG_PARAMS     *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  ReadPci ((AMD_CPU_DEV_FUN << 16) + PRODUCT_INFO_REG1, AccessWidth8, &RetEfuseValue, StdHeader);
  RetEfuseValue = ((RetEfuseValue & 0x1E) >> 1);
  return RetEfuseValue;
}
/**
 * FchXhciUsbPhyCalibrated - Config XHCI Phy
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchXhciUsbPhyCalibrated (
  IN  FCH_DATA_BLOCK     *FchDataPtr
  )
{
  UINT8                 Index;
  UINT32                DrivingStrength;
  UINT32                Port;
  UINT32                Register;
  UINT32                RegValue;
  FCH_DATA_BLOCK        *LocalCfgPtr;
  AMD_CONFIG_PARAMS     *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;


  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG8C, AccessWidth32, 0xFFF0FFFF, 0x00030000);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG8C, AccessWidth32, 0xFEFFFFFF, 0x01000000);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG8C, AccessWidth32, 0x3FFFFFFF, 0x80000000);


  DrivingStrength = 0x302;
  for (Port = 0; Port < 2; Port ++) {
    RwXhci0IndReg ( FCH_XHCI_IND60_REG00, 0xFFFFC000, (Port << 13) + BIT12 + DrivingStrength, StdHeader);
    Register = FCH_XHCI_IND60_REG00;
    Index = 0;
    do {
      WritePci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x48, AccessWidth32, &Register, StdHeader);
      ReadPci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x4C, AccessWidth32, &RegValue, StdHeader);
      Index++;
      FchStall (10, StdHeader);
    } while ((RegValue & BIT17) && (Index < 10 ));
    Index = 0;
    do {
    RwXhci0IndReg ( FCH_XHCI_IND60_REG00, 0xFFFFC000, (Port << 13) + DrivingStrength, StdHeader);
      WritePci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x48, AccessWidth32, &Register, StdHeader);
      ReadPci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x4C, AccessWidth32, &RegValue, StdHeader);
      Index++;
      FchStall (10, StdHeader);
    } while ((RegValue & BIT17) && (Index < 10 ));
    RwXhci0IndReg ( FCH_XHCI_IND60_REG00, 0xFFFFC000, (Port << 13) + BIT12 + DrivingStrength, StdHeader);
  }
  RwXhci0IndReg ( FCH_XHCI_IND60_REG50, ~ ((UINT32) (0x0f)), ((UINT32) (0x06)), StdHeader);
  RwXhci0IndReg ( FCH_XHCI_IND60_REG0C, ~ ((UINT32) (0x0f << 4)), ((UINT32) (0x02 << 4)), StdHeader);
  RwXhci0IndReg ( FCH_XHCI_IND60_REG0C, ~ ((UINT32) (0x0f << 8)), ((UINT32) (0x02 << 8)), StdHeader);
  RwXhci0IndReg ( FCH_XHCI_IND60_REG0C, ~ ((UINT32) (0x0f << 12)), ((UINT32) (0x01 << 12)), StdHeader);
  RwXhci0IndReg ( FCH_XHCI_IND60_REG0C, ~ ((UINT32) (0x0f)), ((UINT32) (0x01)), StdHeader);
  RwXhci0IndReg ( FCH_XHCI_IND60_REG0C, ~ ((UINT32) (0xff << 24)), ((UINT32) (0x90 << 24)), StdHeader);
  RwXhci0IndReg ( FCH_XHCI_IND60_REG54, ~ ((UINT32) (0x03 << 6)), ((UINT32) (0x01 << 6)), StdHeader);
  RwXhci0IndReg ( FCH_XHCI_IND60_REG54, ~ ((UINT32) (0x01 << 1)), ((UINT32) (0x00 << 1)), StdHeader);
  FchStall (200, StdHeader);
  RwXhci0IndReg ( FCH_XHCI_IND60_REG54, ~ ((UINT32) (0x01 << 1)), ((UINT32) (0x01 << 1)), StdHeader);
  FchStall (400, StdHeader);
  RwXhci0IndReg ( FCH_XHCI_IND60_REG54, ~ ((UINT32) (0x01 << 1)), ((UINT32) (0x00 << 1)), StdHeader);
}

/**
 * FchXhciInitIndirectReg - Config XHCI Indirect Registers
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchXhciInitIndirectReg (
  IN  FCH_DATA_BLOCK     *FchDataPtr
  )
{
  UINT8                 Index;
  UINT32                DrivingStrength;
  UINT32                Port;
  UINT32                Register;
  UINT32                RegValue;
  FCH_DATA_BLOCK        *LocalCfgPtr;
  AMD_CONFIG_PARAMS     *StdHeader;
  UINT8                 RetEfuseValue;
  UINT32                UsbFuseCommonCalibrationValue;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  DrivingStrength = 0;

  FchXhciUsbPhyCalibrated (LocalCfgPtr);

  RwXhci0IndReg ( FCH_XHCI_IND_REG94, 0xFFFFFC00, 0x00000021, StdHeader);
  RwXhci0IndReg ( FCH_XHCI_IND_REGD4, 0xFFFFFC00, 0x00000021, StdHeader);

  RwXhci0IndReg ( FCH_XHCI_IND_REG00, 0xF8FFFFFF, 0x07000000, StdHeader);
  for (Port = 0; Port < 2; Port ++) {
    DrivingStrength = (UINT32) (LocalCfgPtr->Usb.Xhci20Phy[Port]);
    RwXhci0IndReg ( FCH_XHCI_IND60_REG00, 0xFFFFC000, (Port << 13) + BIT12 + DrivingStrength, StdHeader);
    RwXhci0IndReg ( FCH_XHCI_IND60_REG00, 0xFFFFC000, (Port << 13) + DrivingStrength, StdHeader);
    RwXhci0IndReg ( FCH_XHCI_IND60_REG00, 0xFFFFC000, (Port << 13) + BIT12 + DrivingStrength, StdHeader);
  }


  RwXhci0IndReg ( FCH_XHCI_IND60_REG50, ~ ((UINT32) (0x0f)), ((UINT32) (0x06)), StdHeader);
  RwXhci0IndReg ( FCH_XHCI_IND60_REG0C, ~ ((UINT32) (0x0f << 4)), ((UINT32) (0x02 << 4)), StdHeader);
  RwXhci0IndReg ( FCH_XHCI_IND60_REG0C, ~ ((UINT32) (0x0f << 8)), ((UINT32) (0x02 << 8)), StdHeader);
  RetEfuseValue = FchUsbCommonPhyCalibration ( FchDataPtr );
  if ( RetEfuseValue == 0) {
    RwXhci0IndReg ( FCH_XHCI_IND60_REG08, 0x80FD00FF, (UINT32) BIT16 , StdHeader);
  } else {
    UsbFuseCommonCalibrationValue = RetEfuseValue << 11;
    RwXhci0IndReg ( FCH_XHCI_IND60_REG08, 0x80FC00FF, UsbFuseCommonCalibrationValue , StdHeader);
  }

  for (Port = 0; Port < 2; Port ++) {
    DrivingStrength = 0x1E4;
      RwXhci0IndReg ( FCH_XHCI_IND60_REG00, 0xFFFE0000, (Port << 13) + BIT12 + DrivingStrength, StdHeader);
      Register = FCH_XHCI_IND60_REG00;
      Index = 0;
      do {
        WritePci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x48, AccessWidth32, &Register, StdHeader);
        ReadPci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x4C, AccessWidth32, &RegValue, StdHeader);
        Index++;
        FchStall (10, StdHeader);
      } while ((RegValue & BIT17) && (Index < 10 ));
      RwXhci0IndReg ( FCH_XHCI_IND60_REG00, 0xFFFE0000, (Port << 13) + DrivingStrength, StdHeader);
      Register = FCH_XHCI_IND60_REG00;
      Index = 0;
      do {
        WritePci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x48, AccessWidth32, &Register, StdHeader);
        ReadPci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x4C, AccessWidth32, &RegValue, StdHeader);
        Index++;
        FchStall (10, StdHeader);
      } while ((RegValue & BIT17) && (Index < 10 ));
      RwXhci0IndReg ( FCH_XHCI_IND60_REG00, 0xFFFE0000, (Port << 13) + BIT12 + DrivingStrength, StdHeader);
  }
  for (Port = 0; Port < 2; Port ++) {
    DrivingStrength = 0x1C4;
    RwXhci0IndReg ( FCH_XHCI_IND60_REG00, 0xFFFFC000, (Port << 13) + BIT12 + DrivingStrength, StdHeader);
    RwXhci0IndReg ( FCH_XHCI_IND60_REG00, 0xFFFFC000, (Port << 13) + DrivingStrength, StdHeader);
    RwXhci0IndReg ( FCH_XHCI_IND60_REG00, 0xFFFFC000, (Port << 13) + BIT12 + DrivingStrength, StdHeader);
  }

  for (Port = 0; Port < 4; Port ++) {
    if (Port < 2) {
      ReadXhci0Phy (Port, (0x7 << 7), &DrivingStrength, StdHeader);
      DrivingStrength &= 0x0000000F8;
      DrivingStrength |= BIT0 + BIT2;
      RwXhci0IndReg ( FCH_XHCI_IND60_REG00, 0xFFFFC000, (Port << 13) + BIT12 + (0x7 << 7) + DrivingStrength, StdHeader);
      RwXhci0IndReg ( FCH_XHCI_IND60_REG00, 0xFFFFC000, (Port << 13) + (0x7 << 7) + DrivingStrength, StdHeader);
      RwXhci0IndReg ( FCH_XHCI_IND60_REG00, 0xFFFFC000, (Port << 13) + BIT12 + (0x7 << 7) + DrivingStrength, StdHeader);
    }
  }
  RwXhci0IndReg ( FCH_XHCI_IND60_REG48, 0xFFFFFFE0, BIT0, StdHeader);
}

/**
 *  FchCombineSigRomInfo - Combine SIG ROM information
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
STATIC UINT32
FchCombineSigRomInfo (
  IN  FCH_DATA_BLOCK     *FchDataPtr
  )
{
  UINTN                 RomSigStartingAddr;
  UINT32                RomStore[NUM_OF_ROMSIG_FILED];
  FCH_DATA_BLOCK        *LocalCfgPtr;
  AMD_CONFIG_PARAMS     *StdHeader;
  UINT32                SelNum;
  UINT32                RomSignatureTag;
  UINT16                XhciFwTag;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  for ( SelNum = 0; SelNum < NUM_OF_ROMSIG_FILED; SelNum++) {
    RomStore [SelNum] = 0;
  }
  GetRomSigPtr (&RomSigStartingAddr, StdHeader);
  RomSignatureTag = ACPIMMIO32 (RomSigStartingAddr);
  if ( RomSignatureTag == ROMSIG_SIG ) {
    for ( SelNum = 0; SelNum < NUM_OF_ROMSIG_FILED; SelNum++) {
      RomStore[SelNum] = ACPIMMIO32 (RomSigStartingAddr + (SelNum << 2));
    }
  } else {
    RomStore[XHCI_FILED_NUM] = 0;
  }
  if ( LocalCfgPtr->Usb.UserDefineXhciRomAddr != 0 ) {
    RomStore[XHCI_FILED_NUM] = LocalCfgPtr->Usb.UserDefineXhciRomAddr;
  }
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGC8 + 3, AccessWidth8, 0x7F, BIT7, StdHeader);
  for ( SelNum = 1; SelNum < NUM_OF_ROMSIG_FILED; SelNum++) {
    if (RomStore[SelNum] != 0) {
      RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGCC, AccessWidth32, (UINT32)~ (BIT2 + BIT1 + BIT0), (BIT2 + SelNum), StdHeader);
      RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGCC, AccessWidth32, ROMSIG_CFG_MASK, RomStore[SelNum], StdHeader);
    }
  }
  XhciFwTag = (UINT16) ACPIMMIO32 ((RomStore[XHCI_FILED_NUM]) + XHCI_BOOT_RAM_OFFSET);
  if ( XhciFwTag != INSTRUCTION_RAM_SIG ) {
    RomStore[XHCI_FILED_NUM] = 0;
  }
  return RomStore[XHCI_FILED_NUM];
}

/**
 * FchXhciInitBeforePciInit - Config XHCI controller before PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchXhciInitBeforePciInit (
  IN  FCH_DATA_BLOCK     *FchDataPtr
  )
{
  UINT16                BcdAddress;
  UINT16                BcdSize;
  UINT16                AcdAddress;
  UINT16                AcdSize;
  UINT16                FwAddress;
  UINT16                FwSize;
  UINT32                XhciFwStarting;
  UINT32                SpiValidBase;
  UINT32                RegData;
  UINT16                Index;
  BOOLEAN               Xhci0Enable;
  FCH_DATA_BLOCK        *LocalCfgPtr;
  AMD_CONFIG_PARAMS     *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  Xhci0Enable = LocalCfgPtr->Usb.Xhci0Enable;

  if ( Xhci0Enable == 0 ) {
    return;
  }
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, 0x00080000, 0x00400700);
  FchStall (20, StdHeader);

  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGC8 + 3, AccessWidth8, 0x7F, BIT7, StdHeader);
  XhciFwStarting = FchCombineSigRomInfo (FchDataPtr);
  if ( XhciFwStarting == 0 ) {
    return;
  }
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccessWidth32, 0xFFFFF9FF, 0x00000600);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG90, AccessWidth32, 0xCFF00000, 0x000AAAAA);

  RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG50 + 1, AccessWidth8, (UINT32)~BIT1, BIT1);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGEF, AccessWidth8, (UINT32)~(BIT4 + BIT5), 0);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGEF, AccessWidth8, (UINT32)~(BIT7), BIT7);                    /// Enable 2.0 devices
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, 0xF0FFFFFC, 0x01);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, 0xEFFFFFFF, 0x10000000);

  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGA0, AccessWidth32, 0x00000000, (SPI_HEAD_LENGTH << 16));

  BcdAddress = ACPIMMIO16 (XhciFwStarting + BCD_ADDR_OFFSET + XHC_BOOT_RAM_SIZE);
  BcdSize = ACPIMMIO16 (XhciFwStarting + BCD_SIZE_OFFSET + XHC_BOOT_RAM_SIZE);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGA4, AccessWidth16, 0x0000, BcdAddress);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGA4 + 2, AccessWidth16, 0x0000, BcdSize);

  AcdAddress = ACPIMMIO16 (XhciFwStarting + ACD_ADDR_OFFSET + XHC_BOOT_RAM_SIZE);
  AcdSize = ACPIMMIO16 (XhciFwStarting + ACD_SIZE_OFFSET + XHC_BOOT_RAM_SIZE);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGA8, AccessWidth16, 0x0000, AcdAddress);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGA8 + 2, AccessWidth16, 0x0000, AcdSize);

  SpiValidBase = SPI_BASE2 (XhciFwStarting + 4 + XHC_BOOT_RAM_SIZE) | SPI_BAR0_VLD | SPI_BASE0 | SPI_BAR1_VLD | SPI_BASE1 | SPI_BAR2_VLD;
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGB0, AccessWidth32, 0x00000000, SpiValidBase);
  for (Index = 0; Index < SPI_HEAD_LENGTH; Index++) {
    RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGC0 + Index, AccessWidth8, 0, ACPIMMIO8 (XhciFwStarting + XHC_BOOT_RAM_SIZE + Index));
  }

  for (Index = 0; Index < BcdSize; Index++) {
    RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGC0 + SPI_HEAD_LENGTH + Index, AccessWidth8, 0, ACPIMMIO8 (XhciFwStarting + BcdAddress + Index));
  }

  for (Index = 0; Index < AcdSize; Index++) {
    RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGC0 + SPI_HEAD_LENGTH + BcdSize + Index, AccessWidth8, 0, ACPIMMIO8 (XhciFwStarting + AcdAddress + Index));
  }


  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, (UINT32)~BIT0, BIT0);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, (UINT32)~BIT29, 0);

  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG04 + 2, AccessWidth16, 0x7FFF, BIT15);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG04, AccessWidth16, 0x0000, 0);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG08 + 2, AccessWidth16, 0x0000, 0x8000);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG08, AccessWidth16, 0x0000, 0);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, (UINT32)~BIT29, BIT29);

  for (;;) {
    ReadMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00 , AccessWidth32, &RegData);
    if (RegData & BIT30) {
      break;
    }
  }
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, (UINT32)~BIT29, 0);

  FwAddress = ACPIMMIO16 (XhciFwStarting + FW_ADDR_OFFSET + XHC_BOOT_RAM_SIZE);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGB4, AccessWidth16, 0x0000, ACPIMMIO16 (XhciFwStarting + FwAddress));
  FwAddress += 2;
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG04, AccessWidth16, 0x0000, FwAddress + XHC_BOOT_RAM_SIZE);

  FwSize = ACPIMMIO16 (XhciFwStarting + FW_SIZE_OFFSET + XHC_BOOT_RAM_SIZE);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG08, AccessWidth16, 0x0000, 0);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG08 + 2, AccessWidth16, 0x0000, FwSize);
  //
  // Step 3.5 to enable the instruction RAM preload functionality.
  //
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG04 + 2, AccessWidth16, 0x7FFF, 0);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, (UINT32)~BIT29, BIT29);

  for (;;) {
    ReadMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00 , AccessWidth32, &RegData);
    if (RegData & BIT30) {
      break;
    }
  }
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, (UINT32)~BIT29, 0);


  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, (UINT32)~U3PLL_RESET, 0);       ///Release U3PLLreset
  for (;;) {
    ReadMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00 , AccessWidth32, &RegData);
    if (RegData & U3PLL_LOCK) {
      break;
    }
  }

  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, (UINT32)~U3PHY_RESET, 0);       ///Release U3PHY
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, (UINT32)~U3CORE_RESET, 0);      ///Release core reset

  FchXhciUsbPhyCalibrated (LocalCfgPtr);

  FchXhciInitIndirectReg (LocalCfgPtr);

  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGEF, AccessWidth8, (UINT32)~(BIT4 + BIT5), 0);                /// Disable Device 22
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGEF, AccessWidth8, (UINT32)~(BIT7), BIT7);                    /// Enable 2.0 devices
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, (UINT32)~(BIT21), BIT21);
  //
  // Step 5.
  //
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + 0x00, AccessWidth32, (UINT32)~(BIT17 + BIT18 + BIT19), BIT17 + BIT18 + BIT19);

  //
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccessWidth32, ~((UINT32) (0x3 << 11)), (UINT32) (0x3 << 11));
  //
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccessWidth32, ~((UINT32) (0x1 << 16)), (UINT32) (0x1 << 16));
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccessWidth32, ~((UINT32) (0x1 << 15)), (UINT32) (0x1 << 15));
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccessWidth32, ~((UINT32) (0x7 << 18)), (UINT32) (0x7 << 18));
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGF0, AccessWidth32, ~((UINT32) (0x1 << 13)), (UINT32) (0x1 << 13));
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGF0, AccessWidth32, ~((UINT32) (0x1 << 14)), (UINT32) (0x1 << 14));

  RwXhci0IndReg ( FCH_XHCI_IND_REG54, (UINT32)~(BIT15), BIT15, StdHeader);

  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccessWidth32, (UINT32)~(BIT25 + BIT24), BIT24);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccessWidth32, (UINT32)~(BIT22), BIT22);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGB4, AccessWidth32, (UINT32)~(BIT20), BIT20);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT15), BIT15);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT1), BIT1);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT2), BIT2);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT3), BIT3);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT6), BIT6);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT7), BIT7);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT8), BIT8);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT9), BIT9);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT10), BIT10);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT11), BIT11);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT12), BIT12);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT13), BIT13);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT14), BIT14);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT16), BIT16);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG40, AccessWidth32, (UINT32)~(BIT0), BIT0);

  RwMem (ACPI_MMIO_BASE + XHCI_BASE + 0x20, AccessWidth32, (UINT32)~(BIT13 + BIT14 + BIT21), BIT13 + BIT14 + BIT21);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGB4, AccessWidth32, (UINT32)~(BIT20), BIT20);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGF2, AccessWidth8, (UINT32)~(BIT3), BIT3);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGB4, AccessWidth32, (UINT32)~(BIT22), BIT22);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT17), BIT17);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT18), BIT18);

  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGB4, AccessWidth32, (UINT32)~(BIT24), BIT24);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGB4, AccessWidth32, (UINT32)~(BIT25), BIT25);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG24, AccessWidth32, (UINT32)~(BIT8), BIT8);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG24, AccessWidth32, (UINT32)~(BIT9), BIT9);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG24, AccessWidth32, (UINT32)~(BIT10), BIT10);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG24, AccessWidth32, (UINT32)~(BIT12), BIT12);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT20 + BIT21 + BIT22), (BIT20 + BIT21 + BIT22));
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT23), BIT23);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT27), BIT27);

  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~BIT29, BIT29);

  RwMem (ACPI_MMIO_BASE + XHCI_BASE + 0x98, AccessWidth32, (UINT32)~(BIT30), BIT30);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + 0x98, AccessWidth32, (UINT32)~(BIT31), BIT31);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGB4, AccessWidth32, (UINT32)~(BIT27), BIT27);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGB4, AccessWidth32, (UINT32)~(BIT28), BIT28);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG48, AccessWidth32, 0x00000000, FCH_XHCI_IND_REG100);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG4C, AccessWidth32, (UINT32)~(BIT0 + BIT1 + BIT2), BIT0 + BIT1 + BIT2);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG48, AccessWidth32, 0x00000000, FCH_XHCI_IND_REG120);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG4C, AccessWidth32, (UINT32)~(BIT3), BIT3);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG48, AccessWidth32, 0x00000000, FCH_XHCI_IND_REG100);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG4C, AccessWidth32, (UINT32)~(BIT21), BIT21);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG48, AccessWidth32, 0x00000000, FCH_XHCI_IND_REG128);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG4C, AccessWidth32, (UINT32)~(BIT0), BIT0);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG48, AccessWidth32, 0x00000000, FCH_XHCI_IND_REG100);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG4C, AccessWidth32, (UINT32)~(BIT3 + BIT4), BIT3 + BIT4);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG48, AccessWidth32, 0x00000000, FCH_XHCI_IND_REG100);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG4C, AccessWidth32, (UINT32)~(BIT5), BIT5);

  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG48, AccessWidth32, 0x00000000, FCH_XHCI_IND_REG48);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG4C, AccessWidth32, (UINT32)~(BIT1), BIT1);

  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG48, AccessWidth32, 0x00000000, FCH_XHCI_IND_REG48);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG4C, AccessWidth32, (UINT32)~(BIT14), BIT14);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG24, AccessWidth32, (UINT32)~(BIT17), BIT17);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG24, AccessWidth32, (UINT32)~(BIT15 + BIT14),  BIT14);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG24, AccessWidth32, (UINT32)~(BIT11), BIT11);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG24, AccessWidth32, (UINT32)~(BIT16), BIT16);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + 0x28, AccessWidth32, (UINT32)~(BIT0), BIT0);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG24, AccessWidth32, (UINT32)~(BIT20 + BIT21 + BIT22), (BIT20 + BIT21 + BIT22));
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG24, AccessWidth32, (UINT32)~(BIT24 + BIT25 + BIT26), (BIT24 + BIT25 + BIT26));
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG30, AccessWidth32, (UINT32)~(BIT0 + BIT4 + BIT24), (BIT0 + BIT24));
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG48, AccessWidth32, 0x00000000, FCH_XHCI_IND_REG100);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG4C, AccessWidth32, (UINT32)~ BIT5, BIT5);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG48, AccessWidth32, 0x00000000, FCH_XHCI_IND_REG48);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG4C, AccessWidth32, (UINT32)~(BIT16), BIT16);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG10, AccessWidth32, 0xFFFF00FF, 0x2A00);
  RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG50 + 1, AccessWidth8, (UINT32)~BIT1, 0);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG48, AccessWidth32, 0x00000000, 0x80);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG4C, AccessWidth32, 0xFFFFFFF8, 0x06);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG48, AccessWidth32, 0x00000000, 0xC0);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG4C, AccessWidth32, 0xFFFFFFF8, 0x06);
}
