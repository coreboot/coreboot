
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

/**
 * sbPorInitPciTable - PCI device registers initial during the power on stage.
 *
 *
 *
 *
 */
REG8MASK sbPorInitPciTable[] =
{
  // SATA device
  {0x00, SATA_BUS_DEV_FUN, 0},
  {SB_SATA_REG84 + 3, ~BIT2, 0},
  {SB_SATA_REGA0, ~(BIT2 + BIT3 + BIT4 + BIT5 + BIT6), BIT2 + BIT3 + BIT4 + BIT5},
  {0xFF, 0xFF, 0xFF},
  // LPC Device (Bus 0, Dev 20, Func 3)
  {0x00, LPC_BUS_DEV_FUN, 0},
  {SB_LPC_REG44, 0xFF, BIT6 + BIT7}, //Enable COM1 and COM2
  {SB_LPC_REG47, 0xFF, BIT5},
  {SB_LPC_REG48, 0x00, BIT0 + BIT1 + BIT2},
  {SB_LPC_REG7C, 0x00, BIT0 + BIT2},
  {SB_LPC_REG78, 0xF0, BIT2 + BIT3},     // Enable LDRQ pin
  {SB_LPC_REGBB, 0xFF, BIT3 + BIT4 + BIT5},
  // A12 set 0xBB [5:3] = 111 to improve SPI timing margin.
  // A12 Set 0xBA [6:5] = 11 improve SPI timing margin. (SPI Prefetch enhancement)
  {SB_LPC_REGBB, 0xBE, BIT0 + BIT3 + BIT4 + BIT5},
  {SB_LPC_REGBA, 0x9F, BIT5 + BIT6},
  {SB_LPC_REGA4, ~ BIT0, BIT0}, //[BUG Fix] Force EC_PortActive to 1 to fix possible IR non function issue when NO_EC_SUPPORT is defined
  {0xFF, 0xFF, 0xFF},
  // P2P Bridge (Bus 0, Dev 20, Func 4)
  {0x00, PCIB_BUS_DEV_FUN, 0},
  {SB_PCIB_REG4B, 0xFF, BIT6 + BIT7 + BIT4},
  // ENH230012: Disable P2P bridge decoder for IO address 0x1000-0x1FFF in SBPOR
  // ENH260809: Add PCI port 80 support in Hudson-2/3
#ifdef SB_PCIB_PORT_80_SUPPORT
  {SB_PCIB_REG1C, 0x00, 0xF0},
  {SB_PCIB_REG1D, 0x00, 0x00},
  {SB_PCIB_REG04, 0x00, 0x21},
#endif
  {SB_PCIB_REG40, 0xDF, 0x20},
  {SB_PCIB_REG50, 0x02, 0x01},
  {0xFF, 0xFF, 0xFF},
};

/**
 * sbPmioPorInitTable - Southbridge ACPI MMIO initial during the power on stage.
 *
 *
 *
 *
 */
AcpiRegWrite sbPmioPorInitTable[] =
{
  {00, 00, 0xB0, 0xAC}, // Signature
  {MISC_BASE >> 8,  SB_MISC_REG41, 0x1F, 0x40}, //keep Auxiliary_14Mclk_Sel [12]
  //RPR 8.9 USB 3.0 Reference Clock MISC_REG 0x40 [4] = 0 Enable spread-spectrum reference clock.
  {MISC_BASE >> 8,  SB_MISC_REG40, 0xEF, 0x00},
//  {MISC_BASE >> 8,  0x24 + 2, 0xFF, 0x20}, Testing CPU clk strength
  {PMIO_BASE >> 8,  SB_PMIOA_REG5D, 0x00, BIT0},
  {PMIO_BASE >> 8,  SB_PMIOA_REGD2, 0xCF, BIT4 + BIT5},
  {SMBUS_BASE >> 8, SB_SMBUS_REG12, 0x00, BIT0},
  {PMIO_BASE >> 8,  SB_PMIOA_REG28, 0xFF, BIT0 + BIT2},
  {PMIO_BASE >> 8,  SB_PMIOA_REG44 + 3, 0x67, BIT7 + BIT3}, // 2.5 Enable Boot Timer
  {PMIO_BASE >> 8,  SB_PMIOA_REG48, 0xFF, BIT0},
  {PMIO_BASE >> 8,  SB_PMIOA_REG00, 0xFF, 0x0E},
  {PMIO_BASE >> 8,  SB_PMIOA_REG00 + 2, 0xFF, 0x40},
  {PMIO_BASE >> 8,  SB_PMIOA_REG00 + 3, 0xFF, 0x08},
  {PMIO_BASE >> 8,  SB_PMIOA_REG34, 0xEF, BIT0 + BIT1},
  {PMIO_BASE >> 8,  SB_PMIOA_REGEC, 0xFD, BIT1},
  //{PMIO_BASE >> 8,  SB_PMIOA_REG5B, 0xF9, BIT1 + BIT2},
  {PMIO_BASE >> 8,  SB_PMIOA_REG08, 0xFE, BIT2 + BIT4},
  {PMIO_BASE >> 8,  SB_PMIOA_REG08 + 1, 0xFF, BIT0},
  {PMIO_BASE >> 8,  SB_PMIOA_REG54, 0x00, BIT4 + BIT6 + BIT7},
  {PMIO_BASE >> 8,  SB_PMIOA_REG04 + 3, 0xFD, BIT1},
  {PMIO_BASE >> 8,  SB_PMIOA_REG74, 0xF6, BIT0 + BIT3},
  {PMIO_BASE >> 8,  SB_PMIOA_REGF0, ~BIT2, 0x00},
  // RPR GEC I/O Termination Setting
  // PM_Reg 0xF6 = Power-on default setting
  // PM_Reg 0xF7 = Power-on default setting
  // PM_Reg 0xF8 = 0x6C
  // PM_Reg 0xF9 = 0x21
  // PM_Reg 0xFA = 0x00 Hudson-2 A12 GEC I/O Pad settings for 3.3V CMOS
  {PMIO_BASE >> 8, SB_PMIOA_REGF8,     0x00, 0x6C},
  {PMIO_BASE >> 8, SB_PMIOA_REGF8 + 1, 0x00, 0x07},
  {PMIO_BASE >> 8, SB_PMIOA_REGF8 + 2, 0x00, 0x00},
  // PRP GEC -end
  {PMIO_BASE >> 8, SB_PMIOA_REGC4, 0xee, 0x04},      // Release NB_PCIE_RST
  {PMIO_BASE >> 8, SB_PMIOA_REGC0 + 2, 0xBF, 0x40},

  {PMIO_BASE >> 8,  SB_PMIOA_REGBE, 0xDF, BIT5},

  //OBS200280
  //{PMIO_BASE >> 8,  SB_PMIOA_REGBE, 0xFF, BIT1},


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
  UINT8   dbPortStatus;
  //UINT8   dbSysConfig;
  UINT32  abValue;
  UINT32  abValue2;
  UINT8   dbValue;
  UINT8   dbEfuse;
  UINT32   dbSpiMode;
  UINT16  dwAsfPort;
  UINT16  smbusBase;
  UINT8   cimSataMode;
//  UINT8   cimSpiFastReadEnable;
//  UINT8   cimSpiFastReadSpeed;
  UINT8   cimSataInternal100Spread;
  UINT8   indexValue;
  UINT32  ddValue;
  UINT8  SataPortNum;
  UINT8     XhciEfuse;
  XhciEfuse = XHCI_EFUSE_LOCATION;

  cimSataMode = pConfig->SATAMODE.SataModeReg;
//  if (pConfig->BuildParameters.SpiFastReadEnable != NULL ) {
//    cimSpiFastReadEnable = (UINT8) pConfig->BuildParameters.SpiFastReadEnable;
//  } else {
//    cimSpiFastReadEnable = cimSpiFastReadEnableDefault;
//  }
//  cimSpiFastReadSpeed = (UINT8) pConfig->BuildParameters.SpiFastReadSpeed;
  cimSataInternal100Spread = ( UINT8 ) pConfig->SataInternal100Spread;

#if  SB_CIMx_PARAMETER == 0
  cimSataMode = (UINT8) ((cimSataMode & 0xFB) | cimSataSetMaxGen2Default);
  cimSataMode = (UINT8) ((cimSataMode & 0x0F) | cimSataClkModeDefault);
  cimSpiFastReadEnable = cimSpiFastReadEnableDefault;
  cimSpiFastReadSpeed = cimSpiFastReadSpeedDefault;
  cimSataInternal100Spread = SataInternal100SpreadDefault;
#endif

  TRACE ((DMSG_SB_TRACE, "CIMx - Entering sbPowerOnInit \n"));

// Hudson-2 Only Enabled (Mmio_mem_enablr)  // Default value is correct
  RWPMIO (SB_PMIOA_REG24, AccWidthUint8, 0xFF, BIT0);

  RWPMIO (0xD3, AccWidthUint8, ~BIT4, 0);
  RWPMIO (0xD3, AccWidthUint8, ~BIT4, BIT4);

  if ( pConfig->Cg2Pll == 1 ) {
    TurnOffCG2 ();
    pConfig->SATAMODE.SataMode.SataClkMode = 0x0a;
  }

  //enable CF9
  RWPMIO (0xD2, AccWidthUint8, ~BIT6, 0);

// Set A-Link bridge access address. This address is set at device 14h, function 0,
// register 0f0h.   This is an I/O address. The I/O address must be on 16-byte boundary.
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGE0, AccWidthUint32, 00, ALINK_ACCESS_INDEX);
  writeAlink (0x80000004, 0x04);     // RPR 4.2 Enable Hudson-2 to issue memory read/write requests in the upstream direction
  abValue = readAlink (SB_ABCFG_REG9C | (UINT32) (ABCFG << 29));      // RPR 4.5 Disable the credit variable in the downstream arbitration equation
  abValue = abValue | BIT0;
  writeAlink (SB_ABCFG_REG9C | (UINT32) (ABCFG << 29), abValue);
  writeAlink (0x30, 0x10);         // AXINDC 0x10[9]=1, Enabling Non-Posted memory write for K8 platform.
  writeAlink (0x34, readAlink (0x34) | BIT9);
  rwAlink (SB_ABCFG_REG10050 | (UINT32) (ABCFG << 29), ~BIT2, 0x00);

  // Enable external Stickybit register reset feature
  //writeAlink (SB_AX_INDXC_REG30 | (UINT32) (AXINDC << 29), 0x30);
  //abValue = readAlink (SB_AX_DATAC_REG34 | (UINT32) (AXINDC << 29));
  //abValue |= BIT6 + BIT5;
  //writeAlink (SB_AX_DATAC_REG34 | (UINT32) (AXINDC << 29), abValue);

  // Configure UMI target link speed
  dbEfuse = PCIE_FORCE_GEN1_EFUSE_LOCATION;
  getEfuseStatus (&dbEfuse);
  if ( dbEfuse & BIT0 ) {
    pConfig->NbSbGen2 = 0;
  }

  dbEfuse = FCH_Variant_EFUSE_LOCATION;
  getEfuseStatus (&dbEfuse);
  if ((dbEfuse == 0x07) || (dbEfuse == 0x08)) {
    pConfig->NbSbGen2 = 0;
  }

  if (pConfig->NbSbGen2) {
    abValue = 2;
    abValue2 = BIT0;
  } else {
    abValue = 1;
    abValue2 = 0;
  }
  rwAlink (SB_AX_INDXP_REGA4, 0xFFFFFFFE, abValue2);
  rwAlink ((UINT32)SB_AX_CFG_REG88, 0xFFFFFFF0, abValue);

  if (pConfig->sdbEnable) {
    rwAlink (SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29), ~BIT12, 0x00);
    RWMEM (ACPI_MMIO_BASE + SERIAL_DEBUG_BASE + 0, AccWidthUint8, 0, pConfig->Debug_Reg00);
    RWMEM (ACPI_MMIO_BASE + SERIAL_DEBUG_BASE + 2, AccWidthUint8, 0, pConfig->Debug_Reg02);
    RWMEM (ACPI_MMIO_BASE + SERIAL_DEBUG_BASE + 4, AccWidthUint8, 0, pConfig->Debug_Reg04);
    RWMEM (ACPI_MMIO_BASE + SERIAL_DEBUG_BASE + 1, AccWidthUint8, 0, pConfig->Debug_Reg01);
    RWMEM (ACPI_MMIO_BASE + SERIAL_DEBUG_BASE + 3, AccWidthUint8, 0, pConfig->Debug_Reg03);
    RWMEM (ACPI_MMIO_BASE + SERIAL_DEBUG_BASE + 5, AccWidthUint8, 0, pConfig->Debug_Reg05);
  }

// Set Build option into SB
  WritePCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG64, AccWidthUint16 | S3_SAVE, &(pConfig->BuildParameters.SioPmeBaseAddress));
  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGA0, AccWidthUint32 | S3_SAVE, 0x001F, (pConfig->BuildParameters.SpiRomBaseAddress));
  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG9C, AccWidthUint32 | S3_SAVE, 0, (pConfig->BuildParameters.GecShadowRomBase + 1));
// Enabled SMBUS0/SMBUS1 (ASF) Base Address
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG2C, AccWidthUint16, 06, (pConfig->BuildParameters.Smbus0BaseAddress) + BIT0); //protect BIT[2:1]
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG28, AccWidthUint16, 06, (pConfig->BuildParameters.Smbus1BaseAddress) + BIT0);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG60, AccWidthUint16, 00, (pConfig->BuildParameters.AcpiPm1EvtBlkAddr));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG62, AccWidthUint16, 00, (pConfig->BuildParameters.AcpiPm1CntBlkAddr));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG64, AccWidthUint16, 00, (pConfig->BuildParameters.AcpiPmTmrBlkAddr));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG66, AccWidthUint16, 00, (pConfig->BuildParameters.CpuControlBlkAddr));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG68, AccWidthUint16, 00, (pConfig->BuildParameters.AcpiGpe0BlkAddr));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG6A, AccWidthUint16, 00, (pConfig->BuildParameters.SmiCmdPortAddr));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG6C, AccWidthUint16, 00, (pConfig->BuildParameters.AcpiPmaCntBlkAddr));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG6E, AccWidthUint16, 00, (pConfig->BuildParameters.SmiCmdPortAddr) + 8);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG48, AccWidthUint32, 00, (pConfig->BuildParameters.WatchDogTimerBase));

  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG2E, AccWidthUint8, ~(BIT1 + BIT2), 0); //clear BIT[2:1]
  smbusBase = (UINT16) (pConfig->BuildParameters.Smbus0BaseAddress);
  dbValue = 0x00;
  WriteIO (smbusBase + 0x14, AccWidthUint8, &dbValue);

  dbEfuse = SATA_FIS_BASE_EFUSE_LOC;
  getEfuseStatus (&dbEfuse);

  programSbAcpiMmioTbl ((AcpiRegWrite*) FIXUP_PTR (&sbPmioPorInitTable[0]));

  //RPR 3.4 Enabling ClkRun Function
  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGBB, AccWidthUint8, ~ BIT2, BIT2);
  //BUG265683: Mismatch clkrun enable register setting between RPR and CIMX code
  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGD0, AccWidthUint8, ~ BIT2, 0);

  SataPortNum = 0;
  for ( SataPortNum = 0; SataPortNum < 0x06; SataPortNum++ ) {
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40 + 2), AccWidthUint8, 0xFF, 1 << SataPortNum);
    SbStall (2);
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40 + 2), AccWidthUint8, (0xFF ^ (1 << SataPortNum)) , 0x00);
    SbStall (2);
  }

  dbValue = 0x0A;
  WriteIO (SB_IOMAP_REG70, AccWidthUint8, &dbValue);
  ReadIO (SB_IOMAP_REG71, AccWidthUint8, &dbValue);
  dbValue &= 0xEF;
  WriteIO (SB_IOMAP_REG71, AccWidthUint8, &dbValue);

  RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG00, AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, (BIT19 + BIT24 + BIT25 + BIT26));
  RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG0C, AccWidthUint32 | S3_SAVE, 0xFFC0FFFF, 0 );
  if (pConfig->BuildParameters.SpiSpeed) {
    RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG0C, AccWidthUint32 | S3_SAVE, ~(BIT13 + BIT12), ((pConfig->BuildParameters.SpiSpeed - 1 ) << 12));
  }
  if (pConfig->BuildParameters.SpiFastSpeed) {
    RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG0C, AccWidthUint32 | S3_SAVE, ~(BIT15 + BIT14), ((pConfig->BuildParameters.SpiFastSpeed - 1 ) << 14));
  }
  //if (pConfig->BuildParameters.SpiBurstWrite) {
  RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG1C, AccWidthUint32 | S3_SAVE, ~(BIT10), ((pConfig->BuildParameters.SpiBurstWrite) << 10));
  //}
  dbSpiMode = pConfig->BuildParameters.SpiMode;
  if (pConfig->BuildParameters.SpiMode) {
    if ((dbSpiMode == SB_SPI_MODE_QUAL_114) || (dbSpiMode == SB_SPI_MODE_QUAL_112) || (dbSpiMode == SB_SPI_MODE_QUAL_144) || (dbSpiMode == SB_SPI_MODE_QUAL_122)) {
    //  RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG00, AccWidthUint32 | S3_SAVE, 0xFFFF0000, 0x013e);
    //  RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG0C, AccWidthUint32 | S3_SAVE, 0xFFFFFF00, 0x80 );
    //  RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG00, AccWidthUint32 | S3_SAVE, 0xFFFEFFFF, 0x10000);
    //  SbStall (1000);
    }
    RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG00, AccWidthUint32 | S3_SAVE, ~( BIT18 + BIT29 + BIT30), ((pConfig->BuildParameters.SpiMode & 1) << 18) + ((pConfig->BuildParameters.SpiMode & 6) << 28));
  }

//  if ( cimSpiFastReadSpeed ) {
//    RWMEM ((pConfig->BuildParameters.SpiRomBaseAddress) + SB_SPI_MMIO_REG0C, AccWidthUint16 | S3_SAVE, ~(BIT15 + BIT14), ( cimSpiFastReadSpeed << 14));
//  }
  //Program power on pci init table
  programPciByteTable ( (REG8MASK*) FIXUP_PTR (&sbPorInitPciTable[0]),
                       ARRAY_SIZE(sbPorInitPciTable));

  programSbAcpiMmioTbl ((AcpiRegWrite *) (pConfig->OEMPROGTBL.OemProgrammingTablePtr_Ptr));

  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG6C, AccWidthUint32 | S3_SAVE, 0xFFFFFF00, 0);

  if (pConfig->SATAMODE.SataModeReg == 0) {
    pConfig->SATAMODE.SataModeReg = (pConfig->SATAMODE.SataMode.SataController << 0) \
      + (pConfig->SATAMODE.SataMode.SataIdeCombMdPriSecOpt << 1) \
      + (pConfig->SATAMODE.SataMode.SataSetMaxGen2 << 2) \
      + (pConfig->SATAMODE.SataMode.SataIdeCombinedMode << 3) \
      + (pConfig->SATAMODE.SataMode.SataClkMode << 4);
  }
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGDA, AccWidthUint8, 0x00, pConfig->SATAMODE.SataModeReg);

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

  if ( cimSataInternal100Spread ) {
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x1E, AccWidthUint8, 0xFF, BIT4);
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG84), AccWidthUint32, 0xFFFFFFFB, 0x00);
  } else {
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x1E, AccWidthUint8, ~BIT4, 0x00);
  }
  // Toggle GEVENT4 to reset all GPP devices
  sbGppTogglePcieReset (pConfig);

  if ( cimSataInternal100Spread ) {
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG84), AccWidthUint32, 0xFFFFFFFF, 0x04);
  }

  dbValue = 0x08;
  WriteIO (SB_IOMAP_REGC00, AccWidthUint8, &dbValue);
  ReadIO (SB_IOMAP_REGC01, AccWidthUint8, &dbValue);
  if ( !pConfig->BuildParameters.EcKbd ) {
    // Route SIO IRQ1/IRQ12 to USB IRQ1/IRQ12 input
    dbValue = dbValue | 0x0A;
  }
  WriteIO (SB_IOMAP_REGC01, AccWidthUint8, &dbValue);

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

  dwAsfPort = ((UINT16) pConfig->BuildParameters.Smbus1BaseAddress & 0xFFF0);
  if ( dwAsfPort != 0 ) {
    RWIO (dwAsfPort + 0x0E, AccWidthUint8, 0x0, 0x70);  // 0x70 will change to EQU ( Remote control address)
  }

#ifndef NO_EC_SUPPORT
  getChipSysMode (&dbPortStatus);
  if ( ((dbPortStatus & ChipSysEcEnable) == 0x00) ) {
    // EC is disabled by jumper setting or board config
    RWPCI (((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGA4), AccWidthUint16 | S3_SAVE, 0xFFFE, BIT0);
  } else {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC4, AccWidthUint8, 0xF7, 0x08);
    ecPowerOnInit ( pConfig);
    imcSleep ( pConfig);
  }
#endif


  ReadPCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x00, AccWidthUint32, &ddValue);
  ReadPCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x00, AccWidthUint32, &ddValue);
  if ( ddValue == 0x78121022 ) {
//
// First Xhci controller.
//
    ReadPCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x00, AccWidthUint32, &ddValue);
    ddValue = 0;
    indexValue = XHCI_REGISTER_BAR03;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    ReadIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);
    ddValue = (UINT32) dbValue;

    indexValue = XHCI_REGISTER_BAR02;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    ReadIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);
    ddValue <<= 8;
    ddValue |= (UINT32) dbValue;

    indexValue = XHCI_REGISTER_BAR01;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    ReadIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);
    ddValue <<= 8;
    ddValue |= (UINT32) dbValue;

    indexValue = XHCI_REGISTER_BAR00;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    ReadIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);
    ddValue <<= 8;
    ddValue |= (UINT32) dbValue;
    WritePCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x10, AccWidthUint32, &ddValue);

    indexValue = XHCI_REGISTER_04H;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    ReadIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);
    WritePCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x04, AccWidthUint8, &dbValue);

    indexValue = XHCI_REGISTER_0CH;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    ReadIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);
    WritePCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x0C, AccWidthUint8, &dbValue);

    indexValue = XHCI_REGISTER_3CH;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    ReadIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);
    WritePCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x3C, AccWidthUint8, &dbValue);
//
// Second Xhci controller.
//
    ReadPCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x00, AccWidthUint32, &ddValue);
    ddValue = 0;
    indexValue = XHCI1_REGISTER_BAR03;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    ReadIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);
    ddValue = (UINT32) dbValue;

    indexValue = XHCI1_REGISTER_BAR02;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    ReadIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);
    ddValue <<= 8;
    ddValue |= (UINT32) dbValue;

    indexValue = XHCI1_REGISTER_BAR01;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    ReadIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);
    ddValue <<= 8;
    ddValue |= (UINT32) dbValue;

    indexValue = XHCI1_REGISTER_BAR00;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    ReadIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);
    ddValue <<= 8;
    ddValue |= (UINT32) dbValue;
    WritePCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x10, AccWidthUint32, &ddValue);

    indexValue = XHCI1_REGISTER_04H;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    ReadIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);
    WritePCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x04, AccWidthUint8, &dbValue);

    indexValue = XHCI1_REGISTER_0CH;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    ReadIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);
    WritePCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x0C, AccWidthUint8, &dbValue);

    indexValue = XHCI1_REGISTER_3CH;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    ReadIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);
    WritePCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x3C, AccWidthUint8, &dbValue);
  }
  // RPR 3.2 Enabling SPI ROM Prefetch
  // Set LPC cfg 0xBA bit 8
  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGBA, AccWidthUint16 | S3_SAVE, 0xFFFF, BIT8);
  if (IsSbA12Plus ()) {
    // Enable SPI Prefetch for USB, set LPC cfg 0xBA bit 7 to 1 for A12 and above
    RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGBA, AccWidthUint16 | S3_SAVE, 0xFFFF, BIT7);
  }
#ifdef XHCI_SUPPORT
#ifdef XHCI_INIT_IN_ROM_SUPPORT
  if ( pConfig->XhciSwitch == 1 ) {
    if ( pConfig->S3Resume == 0 ) {
      XhciEarlyInit ();
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

#ifdef XHCI_SUPPORT
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
//
// RPR 8.13 xHCI USB 2.0 PHY Settings
// Step 1 is done by hardware default
// Step 2
#ifdef USB3_EHCI_DRIVING_STRENGTH
  for (port = 0; port < 4; port ++) {
    ddDrivingStrength = (USB3_EHCI_DRIVING_STRENGTH >> (port * 4)) & 0xF;
    if (ddDrivingStrength & BIT3) {
      ddDrivingStrength &= 0x07;
      if (port < 2) {
        RWXhci0IndReg ( SB_XHCI_IND60_REG00, 0xFFFE0FF8, (port << 13) + ddDrivingStrength);
        RWXhci0IndReg ( SB_XHCI_IND60_REG00, 0xFFFFEFFF, 0x00001000);
      } else {
        RWXhci1IndReg ( SB_XHCI_IND60_REG00, 0xFFFE0FF8, (port << 13) + ddDrivingStrength);
        RWXhci1IndReg ( SB_XHCI_IND60_REG00, 0xFFFFEFFF, 0x00001000);
      }
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

VOID
XhciEarlyInit (
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
  //if (!(pConfig->S4Resume)) {
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccWidthUint32, ~(BIT21), BIT21); //SMI
  //}
//
// Step 5.
//
  RWMEM (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccWidthUint32, ~(BIT17 + BIT18 + BIT19), BIT17 + BIT18);
}
#endif
