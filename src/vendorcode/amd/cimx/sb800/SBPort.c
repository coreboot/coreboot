
/**
 * @file
 *
 * Southbridge Init during POWER-ON
 *
 * Prepare Southbridge environment during power on stage.
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

#include    "SBPLATFORM.h"
#include "cbtypes.h"
/**
 * sbPorInitPciTable - PCI device registers initial during the power on stage.
 */
static const REG8MASK sbPorInitPciTable[] =
{
  // SATA device
  {0x00, SATA_BUS_DEV_FUN, 0},
  {SB_SATA_REG84 + 3, ~BIT2, 0},
  {SB_SATA_REG84 + 1, ~(BIT4 + BIT5), BIT4 + BIT5},
  {SB_SATA_REGA0, ~(BIT2 + BIT3 + BIT4 + BIT5 + BIT6), BIT2 + BIT3 + BIT4 + BIT5},
  {0xFF, 0xFF, 0xFF},
  // LPC Device (Bus 0, Dev 20, Func 3)
  {0x00, LPC_BUS_DEV_FUN, 0},
  {SB_LPC_REG48, 0x00, BIT0 + BIT1 + BIT2},
  {SB_LPC_REG7C, 0x00, BIT0 + BIT2},
  {SB_LPC_REGBB, 0xFF, BIT3 + BIT4 + BIT5},
  // A12 set 0xBB [5:3] = 111 to improve SPI timing margin.
  // A12 Set 0xBA [6:5] = 11 improve SPI timing margin. (SPI Prefetch enhancement)
  {SB_LPC_REGBB, 0xBE, BIT0 + BIT3 + BIT4 + BIT5},
  {SB_LPC_REGBA, 0x9F, BIT5 + BIT6},
  {0xFF, 0xFF, 0xFF},
  // P2P Bridge (Bus 0, Dev 20, Func 4)
  {0x00, PCIB_BUS_DEV_FUN, 0},
  {SB_PCIB_REG4B, 0xFF, BIT6 + BIT7 + BIT4},
  // Enable IO but not allocate any IO range. This is for post code display on debug port behind P2P bridge.
  {SB_PCIB_REG1C, 0x00, 0xF0},
  {SB_PCIB_REG1D, 0x00, 0x00},
  {SB_PCIB_REG04, 0x00, 0x21},
  {SB_PCIB_REG40, 0xDF, 0x20},
  {SB_PCIB_REG50, 0x02, 0x01},
  {0xFF, 0xFF, 0xFF},
};

/**
 * sbPmioPorInitTable - Southbridge ACPI MMIO initial during the power on stage.
 */
static const AcpiRegWrite sbPmioPorInitTable[] =
{
  {PMIO_BASE >> 8,  SB_PMIOA_REG5D, 0x00, BIT0},
  {PMIO_BASE >> 8,  SB_PMIOA_REGD2, 0xCF, BIT4 + BIT5},
  {SMBUS_BASE >> 8, SB_SMBUS_REG12, 0x00, BIT0},
  {PMIO_BASE >> 8,  SB_PMIOA_REG28, 0xFF, BIT0},
  {PMIO_BASE >> 8,  SB_PMIOA_REG44 + 3, 0x7F, BIT7},
  {PMIO_BASE >> 8,  SB_PMIOA_REG48, 0xFF, BIT0},
  {PMIO_BASE >> 8,  SB_PMIOA_REG00, 0xFF, 0x0E},
  {PMIO_BASE >> 8,  SB_PMIOA_REG00 + 2, 0xFF, 0x40},
  {PMIO_BASE >> 8,  SB_PMIOA_REG00 + 3, 0xFF, 0x08},
  {PMIO_BASE >> 8,  SB_PMIOA_REG34, 0xEF, BIT0 + BIT1},
  {PMIO_BASE >> 8,  SB_PMIOA_REGEC, 0xFD, BIT1},
  {PMIO_BASE >> 8,  SB_PMIOA_REG5B, 0xF9, BIT1 + BIT2},
  {PMIO_BASE >> 8,  SB_PMIOA_REG08, 0xFE, BIT2 + BIT4},
  {PMIO_BASE >> 8,  SB_PMIOA_REG08 + 1, 0xFF, BIT0},
  {PMIO_BASE >> 8,  SB_PMIOA_REG54, 0x00, BIT4 + BIT7},
  {PMIO_BASE >> 8,  SB_PMIOA_REG04 + 3, 0xFD, BIT1},
  {PMIO_BASE >> 8,  SB_PMIOA_REG74, 0xF6, BIT0 + BIT3},
  {PMIO_BASE >> 8,  SB_PMIOA_REGF0, ~BIT2, 0x00},
  // RPR GEC I/O Termination Setting
  // PM_Reg 0xF6 = Power-on default setting
  // PM_Reg 0xF7 = Power-on default setting
  // PM_Reg 0xF8 = 0x6C
  // PM_Reg 0xF9 = 0x21
  // PM_Reg 0xFA = 0x00 SB800 A12 GEC I/O Pad settings for 3.3V CMOS
  {PMIO_BASE >> 8, SB_PMIOA_REGF8,     0x00, 0x6C},
  {PMIO_BASE >> 8, SB_PMIOA_REGF8 + 1, 0x00, 0x27},
  {PMIO_BASE >> 8, SB_PMIOA_REGF8 + 2, 0x00, 0x00},
  {PMIO_BASE >> 8, SB_PMIOA_REGC4, 0xFE, 0x14},
  {PMIO_BASE >> 8, SB_PMIOA_REGC0 + 2, 0xBF, 0x40},

  {PMIO_BASE >> 8,  SB_PMIOA_REGBE, 0xDF, BIT5},//ENH210907  SB800: request to no longer clear kb_pcirst_en (bit 1) of PM_Reg BEh per the RPR

  {0xFF, 0xFF, 0xFF, 0xFF},
};

/**
 * sbPowerOnInit - Config Southbridge during power on stage.
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
sbPowerOnInit (
  IN       AMDSBCFG* pConfig
  )
{

  UINT8  dbPortStatus;
  UINT8  dbSysConfig;
  UINT32  abValue;
  UINT8   dbValue;
  UINT8   dbEfuse;
  UINT8   dbCg2WR;
  UINT8   dbCg1Pll;
  UINT8  cimNbSbGen2;
  UINT8  cimSpiFastReadEnable;
  UINT8  cimSpiFastReadSpeed;
  UINT8  cimSioHwmPortEnable;
  UINT8  SataPortNum;

  cimNbSbGen2 = pConfig->NbSbGen2;
// Adding Fast Read Function support
  if (pConfig->BuildParameters.SpiFastReadEnable != 0 ) {
    cimSpiFastReadEnable = (UINT8) pConfig->BuildParameters.SpiFastReadEnable;
  } else {
    cimSpiFastReadEnable = cimSpiFastReadEnableDefault;
  }
  cimSpiFastReadSpeed = (UINT8) pConfig->BuildParameters.SpiFastReadSpeed;
  cimSioHwmPortEnable = pConfig->SioHwmPortEnable;
#if  SB_CIMx_PARAMETER == 0
  cimNbSbGen2 = cimNbSbGen2Default;
  cimSataMode = (UINT8) ((cimSataMode & 0xFB) | cimSataSetMaxGen2Default);
    cimSataMode = (UINT8) ((cimSataMode & 0x0F) | (cimSATARefClkSelDefault + cimSATARefDivSelDefault));
  cimSpiFastReadEnable = cimSpiFastReadEnableDefault;
  cimSpiFastReadSpeed = cimSpiFastReadSpeedDefault;
  cimSioHwmPortEnable = cimSioHwmPortEnableDefault;
#endif

// SB800 Only Enabled (Mmio_mem_enablr)  // Default value is correct
  RWPMIO (SB_PMIOA_REG24, AccWidthUint8, 0xFF, BIT0);

// Set A-Link bridge access address. This address is set at device 14h, function 0,
// register 0f0h.   This is an I/O address. The I/O address must be on 16-byte boundary.
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGE0, AccWidthUint32, 00, ALINK_ACCESS_INDEX);
  writeAlink (0x80000004, 0x04);     // RPR 4.2 Enable SB800 to issue memory read/write requests in the upstream direction
  abValue = readAlink (SB_ABCFG_REG9C | (UINT32) (ABCFG << 29));      // RPR 4.5 Disable the credit variable in the downstream arbitration equation
  abValue = abValue | BIT0;
  writeAlink (SB_ABCFG_REG9C | (UINT32) (ABCFG << 29), abValue);
  writeAlink (0x30, 0x10);         // AXINDC 0x10[9]=1, Enabling Non-Posted memory write for K8 platform.
  writeAlink (0x34, readAlink (0x34) | BIT9);

  dbEfuse = FUSE_ID_EFUSE_LOC;
  getEfuseStatus (&dbEfuse);
  if ( dbEfuse == M1_D1_FUSE_ID ) {
    dbEfuse = MINOR_ID_EFUSE_LOC;
    getEfuseStatus (&dbEfuse);
    if ( dbEfuse == M1_MINOR_ID ) {
      // Limit ALink speed to 2.5G if Hudson-M1
      cimNbSbGen2 = 0;
    }
  }
// Step 1:
// AXINDP_Reg 0xA4[0] = 0x1
// Step 2:
// AXCFG_Reg 0x88[3:0] = 0x2
// Step3:
// AXINDP_Reg 0xA4[18] = 0x1
  if ( cimNbSbGen2 == TRUE ) {
    rwAlink (SB_AX_INDXP_REGA4, 0xFFFFFFFF, BIT0);
    rwAlink ((UINT32)SB_AX_CFG_REG88, 0xFFFFFFF0, 0x2);
    rwAlink (SB_AX_INDXP_REGA4, 0xFFFFFFFF, BIT18);
  }

// Set Build option into SB
  WritePCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG64, AccWidthUint16 | S3_SAVE, &(pConfig->BuildParameters.SioPmeBaseAddress));
  if (cimSioHwmPortEnable) {
    // Use Wide IO Port 1 to provide access to the superio HWM registers.
    WritePCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG66 , AccWidthUint16 | S3_SAVE, &(pConfig->BuildParameters.SioHwmBaseAddress));
    RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG48 + 3, AccWidthUint8  | S3_SAVE, 0xFF, BIT0); // Wide IO Port 1: enable
    RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG74    , AccWidthUint8  | S3_SAVE, 0xFF, BIT2); // set width 0:512, 1:16 bytes
  }
  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGA0, AccWidthUint32 | S3_SAVE, 0x001F, (pConfig->BuildParameters.SpiRomBaseAddress));
  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG9C, AccWidthUint32 | S3_SAVE, 0, (pConfig->BuildParameters.GecShadowRomBase + 1));
// Enabled SMBUS0/SMBUS1 (ASF) Base Address
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG2C, AccWidthUint16, 06, (pConfig->BuildParameters.Smbus0BaseAddress) + BIT0); //protect BIT[2:1]
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG28, AccWidthUint16, 00, (pConfig->BuildParameters.Smbus1BaseAddress));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG60, AccWidthUint16, 00, (pConfig->BuildParameters.AcpiPm1EvtBlkAddr));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG62, AccWidthUint16, 00, (pConfig->BuildParameters.AcpiPm1CntBlkAddr));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG64, AccWidthUint16, 00, (pConfig->BuildParameters.AcpiPmTmrBlkAddr));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG66, AccWidthUint16, 00, (pConfig->BuildParameters.CpuControlBlkAddr));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG68, AccWidthUint16, 00, (pConfig->BuildParameters.AcpiGpe0BlkAddr));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG6A, AccWidthUint16, 00, (pConfig->BuildParameters.SmiCmdPortAddr));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG6C, AccWidthUint16, 00, (pConfig->BuildParameters.AcpiPmaCntBlkAddr));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG6E, AccWidthUint16, 00, (pConfig->BuildParameters.SmiCmdPortAddr) + 8);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG48, AccWidthUint32, 00, (pConfig->BuildParameters.WatchDogTimerBase));

  dbEfuse = SATA_FIS_BASE_EFUSE_LOC;
  getEfuseStatus (&dbEfuse);

  programSbAcpiMmioTbl ((AcpiRegWrite*) FIXUP_PTR (&sbPmioPorInitTable[0]));


  SataPortNum = 0;
  for ( SataPortNum = 0; SataPortNum < 0x06; SataPortNum++ ) {
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40 + 2), AccWidthUint8, 0xFF, 1 << SataPortNum);
    SbStall (2);
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40 + 2), AccWidthUint8, (0xFF ^ (1 << SataPortNum)) , 0x00);
    SbStall (2);
  }


  //The following bits must be set before enabling SPI prefetch.
  //  Set SPI MMio bit offset 00h[19] to 1 and offset 00h[26:24] to 111, offset 0ch[21:16] to 1, Set LPC cfg BBh[6] to 0 ( by default it is 0).
  // if Ec is enable
  //    Maximum spi speed that can be supported by SB is 22M (SPI Mmio offset 0ch[13:12] to 10) if the rom can run at the speed.
  // else
  //    Maximum spi speed that can be supported by SB is 33M (SPI Mmio offset 0ch[13:12] to 01 in normal mode or offset 0ch[15:14] in fast mode) if the rom can run at
  //    the speed.
  getChipSysMode (&dbSysConfig);
  if (pConfig->BuildParameters.SpiSpeed < 0x02) {
    pConfig->BuildParameters.SpiSpeed = 0x01;
    if (dbSysConfig & ChipSysEcEnable) pConfig->BuildParameters.SpiSpeed = 0x02;
  }

  if (pConfig->SbSpiSpeedSupport) {
    RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG00, AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, (BIT19 + BIT24 + BIT25 + BIT26));
    RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG0C, AccWidthUint32 | S3_SAVE, 0xFFC0FFFF, 1 << 16 );
    RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG0C, AccWidthUint16 | S3_SAVE, ~(BIT13 + BIT12), (pConfig->BuildParameters.SpiSpeed << 12));
  }
  // SPI Fast Read Function
  if ( cimSpiFastReadEnable ) {
    RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG00, AccWidthUint32 | S3_SAVE, 0xFFFBFFFF, BIT18);
  } else {
    RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG00, AccWidthUint32 | S3_SAVE, 0xFFFBFFFF, 0x00);
  }

  if ( cimSpiFastReadSpeed ) {
    RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG0C, AccWidthUint16 | S3_SAVE, ~(BIT15 + BIT14), ( cimSpiFastReadSpeed << 14));
  }
  //Program power on pci init table
  programPciByteTable ( (REG8MASK*) FIXUP_PTR (&sbPorInitPciTable[0]), sizeof (sbPorInitPciTable) / sizeof (REG8MASK) );

  programSbAcpiMmioTbl ((AcpiRegWrite *) (pConfig->OEMPROGTBL.OemProgrammingTablePtr_Ptr));

  dbValue = 0x0A;
  WriteIO (SB_IOMAP_REG70, AccWidthUint8, &dbValue);
  ReadIO (SB_IOMAP_REG71, AccWidthUint8, &dbValue);
  dbValue &= 0xEF;
  WriteIO (SB_IOMAP_REG71, AccWidthUint8, &dbValue);

// Change the CG PLL multiplier to x1.1
  if ( pConfig->UsbRxMode !=0 ) {
    dbCg2WR = 0x00;
    dbCg1Pll = 0x3A;
    ReadMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC8, AccWidthUint8, &dbCg2WR);
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD8, AccWidthUint8, 0, 0x3A);
    ReadMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD9, AccWidthUint8, &dbCg1Pll);
    dbCg2WR &= BIT4;
    if (( dbCg2WR == 0x00 ) && ( dbCg1Pll !=0x10 ))
    {
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x18, AccWidthUint8, 0xE1, 0x10);
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD8, AccWidthUint8, 0, 0x3A);
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD9, AccWidthUint8, 0, USB_PLL_Voltage);
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC8, AccWidthUint8, 0xEF, 0x10);
      dbValue = 0x06;
      WriteIO (0xCF9, AccWidthUint8, &dbValue);
    } else {
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC8, AccWidthUint8, 0xEF, 0x00);
    }
  }

  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG6C, AccWidthUint32 | S3_SAVE, ~(pConfig->BuildParameters.BiosSize << 4), 0);

  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGDA, AccWidthUint8, 0, (pConfig->SATAMODE.SataModeReg) & 0xFD );

  if (dbEfuse & BIT0) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGDA, AccWidthUint8, 0xFB, 0x04);
  }

  ReadMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGDA, AccWidthUint8, &dbPortStatus);
  if ( ((dbPortStatus & 0xF0) == 0x10) ) {
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + SB_PMIOA_REG08, AccWidthUint8, 0, BIT5);
  }

  if ( pConfig->BuildParameters.LegacyFree ) {
    RWPCI (((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG44), AccWidthUint32 | S3_SAVE, 00, 0x0003C000);
  } else {
    RWPCI (((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG44), AccWidthUint32 | S3_SAVE, 00, 0xFF03FFD5);
  }

  dbValue = 0x09;
  WriteIO (SB_IOMAP_REGC00, AccWidthUint8, &dbValue);
  ReadIO (SB_IOMAP_REGC01, AccWidthUint8, &dbValue);
  if ( !pConfig->BuildParameters.EcKbd ) {
    // Route SIO IRQ1/IRQ12 to USB IRQ1/IRQ12 input
    dbValue = dbValue & 0xF9;
  }
  if ( pConfig->BuildParameters.LegacyFree ) {
    // Disable IRQ1/IRQ12 filter enable for Legacy free with USB KBC emulation.
    dbValue = dbValue & 0x9F;
  }
  // Enabled IRQ input
  dbValue = dbValue | BIT4;
  WriteIO (SB_IOMAP_REGC01, AccWidthUint8, &dbValue);

#ifndef NO_EC_SUPPORT
  getChipSysMode (&dbPortStatus);
  if ( ((dbPortStatus & ChipSysEcEnable) == 0x00) ) {
    // EC is disabled by jumper setting or board config
    RWPCI (((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGA4), AccWidthUint16 | S3_SAVE, 0xFFFE, BIT0);
  } else {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC4, AccWidthUint8, 0xF7, 0x08);
    ecPowerOnInit ( pConfig);
  }
#endif

  ReadMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG80, AccWidthUint8, &dbValue);
  if (dbValue & ChipSysIntClkGen) {
    ReadMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC4, AccWidthUint8, &dbValue);
    if (dbValue & BIT2) {
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC0 + 2, AccWidthUint8, 0xDF, 0x20);
    } else {
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC4, AccWidthUint8, 0xFB, 0x40);
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC0 + 2, AccWidthUint8, 0xDF, 0x20);
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC4, AccWidthUint8, 0xFB, 0x00);
    }
  }

  // Restore GPP clock to on as it may be off during last POST when some device was disabled;
  // the device can't be detected if enabled again as the values retain on S5 and warm reset.
  RWMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG00, AccWidthUint32, 0xFFFFFFFF, 0xFFFFFFFF);
  RWMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG04, AccWidthUint8, 0xFF, 0xFF);

  // Set PMx88[5]to enable LdtStp# output to do the C3 or FidVid transation
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG88, AccWidthUint8, 0xFF, BIT5);
}
