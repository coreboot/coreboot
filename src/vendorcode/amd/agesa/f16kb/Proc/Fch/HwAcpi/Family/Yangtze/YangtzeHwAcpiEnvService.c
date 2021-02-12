/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch HwAcpi controller
 *
 * Init HwAcpi Controller features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 87262 $   @e \$Date: 2013-01-31 09:13:43 -0600 (Thu, 31 Jan 2013) $
 *
 */
/*
*****************************************************************************
*
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
****************************************************************************
*/
#include "FchPlatform.h"
#include "amdlib.h"
#include "cpuServices.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_HWACPI_FAMILY_YANGTZE_YANGTZEHWACPIENVSERVICE_FILECODE

#define AMD_CPUID_APICID_LPC_BID    0x00000001ul  // Local APIC ID, Logical Processor Count, Brand ID

ACPI_REG_WRITE FchYangtzeInitEnvSpecificHwAcpiMmioTable[] =
{
  {00, 00, 0xB0, 0xAC},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG28, (UINT8)~(BIT0 + BIT2), BIT0},                                 // Set ASF SMBUS master function enabled here (temporary)
#ifdef ACPI_SLEEP_TRAP
  {SMI_BASE  >> 8,  FCH_SMI_REGB0, (UINT8)~(BIT2 + BIT3), BIT2},                                          // Set SLP_TYPE as SMI event
  {PMIO_BASE >> 8,  FCH_PMIOA_REGBE, (UINT8)~BIT5, 0x00},                                                 // Disabled SLP function for S1/S3/S4/S5
  {PMIO_BASE >> 8,  0x08 + 3, (UINT8)~(BIT0 + BIT1), BIT1},                                    // Set S state transition disabled (BIT0) force ACPI to
                                                                                                   //  send SMI message when writing to SLP_TYP Acpi register. (BIT1)
  {SMI_BASE  >> 8,  FCH_SMI_REG98 + 3,  (UINT8)~BIT7, 0x00},                                              // Enabled Global Smi ( BIT7 clear as 0 to enable )
#endif
  {PMIO_BASE >> 8,  0x80 + 1, (UINT8)~(BIT3 + BIT4), BIT3 + BIT4},
  {0xFF, 0xFF, 0xFF, 0xFF},
};


/**
 * FchInitEnvHwAcpiMmioTable - Fch ACPI MMIO initial
 * during POST.
 *
 */
ACPI_REG_WRITE FchYangtzeInitEnvHwAcpiMmioTable[] =
{
  {00, 00, 0xB0, 0xAC},                                         /// Signature

  //
  // HPET workaround
  //
  {PMIO_BASE >> 8, FCH_PMIOA_REG54 + 2, 0x7F, BIT7},
  {PMIO_BASE >> 8, FCH_PMIOA_REG54 + 2, 0x7F, 0x00},
  {PMIO_BASE >> 8, FCH_PMIOA_REGC4, (UINT8)~BIT2, BIT2},
  {PMIO_BASE >> 8, FCH_PMIOA_REGC0, 0, 0x3D},
  {PMIO_BASE >> 8, FCH_PMIOA_REGC0 + 1, 0x0, 0x04},
  {PMIO_BASE >> 8, FCH_PMIOA_REGC2, 0x20, 0x58},
  {PMIO_BASE >> 8, FCH_PMIOA_REGC2 + 1, 0, 0x40},
  {PMIO_BASE >> 8, FCH_PMIOA_REGC2, (UINT8)~(BIT4), BIT4},
  {PMIO_BASE >> 8, FCH_PMIOA_REGCC, 0xF8, 0x03},
  {PMIO_BASE >> 8, FCH_PMIOA_REG74, 0x00, BIT0 + BIT1 + BIT2 + BIT4},
  {PMIO_BASE >> 8, 0x74 + 3, (UINT8)~BIT5, 0},
  {PMIO_BASE >> 8, FCH_PMIOA_REGBA, (UINT8)~BIT3, BIT3},
  {PMIO_BASE >> 8, FCH_PMIOA_REGBC, (UINT8)~BIT1, BIT1},
  {PMIO_BASE >> 8, 0xDC, 0x7C, BIT1},

  {SMI_BASE >> 8, FCH_SMI_Gevent1, 0, 1},
  {SMI_BASE >> 8, FCH_SMI_Gevent3, 0, 3},
  {SMI_BASE >> 8, FCH_SMI_Gevent4, 0, 4},
  {SMI_BASE >> 8, FCH_SMI_Gevent5, 0, 5},
  {SMI_BASE >> 8, FCH_SMI_Gevent6, 0, 6},
  {SMI_BASE >> 8, FCH_SMI_Gevent23, 0, 23},
  {SMI_BASE >> 8, FCH_SMI_xHC0Pme, 0, 11},
  {SMI_BASE >> 8, FCH_SMI_xHC1Pme, 0, 11},
  {SMI_BASE >> 8, FCH_SMI_Usbwakup0, 0, 11},
  {SMI_BASE >> 8, FCH_SMI_Usbwakup1, 0, 11},
  {SMI_BASE >> 8, FCH_SMI_Usbwakup2, 0, 11},
  {SMI_BASE >> 8, FCH_SMI_Usbwakup3, 0, 11},
  {SMI_BASE >> 8, FCH_SMI_IMCGevent0, 0, 12},
  {SMI_BASE >> 8, FCH_SMI_FanThGevent, 0, 13},
  {SMI_BASE >> 8, FCH_SMI_SBGppPme0, 0, 15},
  {SMI_BASE >> 8, FCH_SMI_SBGppPme1, 0, 16},
  {SMI_BASE >> 8, FCH_SMI_SBGppPme2, 0, 17},
  {SMI_BASE >> 8, FCH_SMI_SBGppPme3, 0, 18},
  {SMI_BASE >> 8, FCH_SMI_GecPme, 0, 19},
  {SMI_BASE >> 8, FCH_SMI_CIRPme, 0, 28},
  {SMI_BASE >> 8, FCH_SMI_Gevent8, 0, 24},
//  {SMI_BASE >> 8, FCH_SMI_AzaliaPme, 0, 27},
  {SMI_BASE >> 8, FCH_SMI_SataGevent0, 0, 30},
  {SMI_BASE >> 8, FCH_SMI_SataGevent1, 0, 31},
  {SMI_BASE >> 8, FCH_SMI_REG08,  0xE7, 0},
  {SMI_BASE >> 8, FCH_SMI_REG0C + 2, (UINT8)~BIT3, BIT3},
  {SMI_BASE >> 8, FCH_SMI_TWARN, 0, 9},
  {0xFF, 0xFF, 0xFF, 0xFF},
};

/**
 * FchYangtzeInitEnvHwAcpiPciTable - PCI device registers initial
 * during early POST.
 *
 */
REG8_MASK FchYangtzeInitEnvHwAcpiPciTable[] =
{
  //
  // SMBUS Device (Bus 0, Dev 20, Func 0)
  //
  {0x00, SMBUS_BUS_DEV_FUN, 0},
  {FCH_CFG_REG10, 0X00, (FCH_VERSION & 0xFF)},                ///Program the version information
  {FCH_CFG_REG11, 0X00, (FCH_VERSION >> 8)},
  {0xFF, 0xFF, 0xFF},
};


/**
 * ProgramEnvPFchAcpiMmio - Config HwAcpi MMIO registers
 *   Acpi S3 resume won't execute this procedure (POST only)
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ProgramEnvPFchAcpiMmio (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  ProgramFchAcpiMmioTbl ((ACPI_REG_WRITE*) (&FchYangtzeInitEnvHwAcpiMmioTable[0]), StdHeader);
}

/**
 * ProgramFchEnvHwAcpiPciReg - Config HwAcpi PCI controller
 * before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ProgramFchEnvHwAcpiPciReg (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  //Early post initialization of pci config space
  //
  ProgramPciByteTable ((REG8_MASK*) (&FchYangtzeInitEnvHwAcpiPciTable[0]),
                       ARRAY_SIZE(FchYangtzeInitEnvHwAcpiPciTable), StdHeader);

  if ( LocalCfgPtr->Smbus.SmbusSsid != 0 ) {
    RwPci ((SMBUS_BUS_DEV_FUN << 16) + FCH_CFG_REG2C, AccessWidth32, 0x00, LocalCfgPtr->Smbus.SmbusSsid, StdHeader);
  }
  if ( LocalCfgPtr->Misc.NoneSioKbcSupport ) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGED, AccessWidth8, ~(UINT32) ( BIT2 + BIT1), BIT2 + BIT1);
  } else {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGED, AccessWidth8, ~(UINT32) ( BIT2 + BIT1), BIT2);
  }
  ProgramPcieNativeMode (FchDataPtr);
}

/**
 * FchVgaInit - Config VGA CODEC
 *
 * @param[in] VOID empty
 *
 */
VOID
FchVgaInit (
  OUT VOID
  )
{
}

/**
 * ProgramSpecificFchInitEnvAcpiMmio - Config HwAcpi MMIO before
 * PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ProgramSpecificFchInitEnvAcpiMmio (
  IN  VOID     *FchDataPtr
  )
{
  CPUID_DATA      CpuId;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  ProgramFchAcpiMmioTbl ((ACPI_REG_WRITE*) (&FchYangtzeInitEnvSpecificHwAcpiMmioTable[0]), StdHeader);

  LibAmdCpuidRead (AMD_CPUID_APICID_LPC_BID, &CpuId, StdHeader);

  if ((LocalCfgPtr->HwAcpi.AnyHt200MhzLink) || ((CpuId.EAX_Reg & 0x00ff00f0) == 0x100080) || ((CpuId.EAX_Reg & 0x00ff00f0) == 0x100090) || ((CpuId.EAX_Reg & 0x00ff00f0) == 0x1000A0)) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG94, AccessWidth8, 0, 0x0A);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0x80 + 3, AccessWidth8, 0xFE, 0x28);
  } else {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG94, AccessWidth8, 0, 0x01);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0x80 + 3, AccessWidth8, 0xFE, 0x20);
  }
  RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG6C + 2, AccessWidth8, 0x5F, 0xA0);
  //
  // Ac Loss Control
  //
  AcLossControl ((UINT8) LocalCfgPtr->HwAcpi.PwrFailShadow);
  //
  // FCH VGA Init
  //
  FchVgaInit ();

  //
  // Set ACPIMMIO by OEM Input table
  //
  ProgramFchAcpiMmioTbl ((ACPI_REG_WRITE *) (LocalCfgPtr->HwAcpi.OemProgrammingTablePtr), StdHeader);
}

/**
 * ValidateFchVariant - Validate FCH Variant
 *
 *
 *
 * @param[in] FchDataPtr
 *
 */
VOID
ValidateFchVariant (
  IN  VOID     *FchDataPtr
  )
{
  CPUID_DATA             CpuId;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  LibAmdCpuidRead (CPUID_FMF, &CpuId, StdHeader);
  LocalCfgPtr->Misc.FchCpuId = ( UINT32 ) (CpuId.EAX_Reg & 0xFFFFFFFF);
}

/**
 * IsExternalClockMode - Is External Clock Mode?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
BOOLEAN
IsExternalClockMode (
  IN  VOID     *FchDataPtr
  )
{
  UINT8    MISC80;
  ReadMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG80 + 2, AccessWidth8, &MISC80);
  return ( (BOOLEAN) ((MISC80 & BIT1) == 0) );
}


/**
 * ProgramFchEnvSpreadSpectrum - Config SpreadSpectrum before
 * PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ProgramFchEnvSpreadSpectrum (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        PortStatus;
  UINT8        FchSpreadSpectrum;

  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  FchSpreadSpectrum = LocalCfgPtr->HwAcpi.SpreadSpectrum;

  if ( FchSpreadSpectrum ) {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG08, AccessWidth8, 0xFE, 0x00);
    if ( LocalCfgPtr->HwAcpi.SpreadSpectrumOptions == 0 ) {
      /// -0.362%
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG18 + 1, AccessWidth8, 0xF0, 0x01);
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG14 + 2, AccessWidth16, 0, 0xCF5C);
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG14, AccessWidth16, 0, 0x0137);
    }
    if ( LocalCfgPtr->HwAcpi.SpreadSpectrumOptions == 1 ) {
      /// -0.375%
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG18 + 1, AccessWidth8, 0xF0, 0x01);
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG14 + 2, AccessWidth16, 0, 0xE000);
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG14, AccessWidth16, 0, 0x0142);
    }
    if ( LocalCfgPtr->HwAcpi.SpreadSpectrumOptions == 2 ) {
      /// -0.4%
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG18 + 1, AccessWidth8, 0xF0, 0x02);
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG14 + 2, AccessWidth16, 0, 0);
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG14, AccessWidth16, 0, 0x0158);
    }
    if ( LocalCfgPtr->HwAcpi.SpreadSpectrumOptions == 3 ) {
      /// -0.425%
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG18 + 1, AccessWidth8, 0xF0, 0x02);
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG14 + 2, AccessWidth16, 0, 0x1FFF);
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG14, AccessWidth16, 0, 0x016D);
    }
    if ( LocalCfgPtr->HwAcpi.SpreadSpectrumOptions == 4 ) {
      /// -0.45%
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG18 + 1, AccessWidth8, 0xF0, 0x02);
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG14 + 2, AccessWidth16, 0, 0x4000);
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG14, AccessWidth16, 0, 0x0183);
    }
    if ( LocalCfgPtr->HwAcpi.SpreadSpectrumOptions == 5 ) {
      /// -0.475%
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG18 + 1, AccessWidth8, 0xF0, 0x02);
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG14 + 2, AccessWidth16, 0, 0x6000);
      RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG14, AccessWidth16, 0, 0x0198);
    }
    RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG08, AccessWidth8, 0xFE, BIT0);
  } else {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG08, AccessWidth8, 0xFE, 0x00);
  }

  //
  // PLL 100Mhz Reference Clock Buffer setting for internal clock generator mode (BIT5)
  // OSC Clock setting for  internal clock generator mode (BIT6)
  //
  GetChipSysMode (&PortStatus, StdHeader);
  if ( ((PortStatus & ChipSysIntClkGen) == ChipSysIntClkGen) ) {
    RwMem (ACPI_MMIO_BASE + MISC_BASE +  FCH_MISC_REG04 + 1, AccessWidth8, (UINT32)~(BIT5 + BIT6), BIT5 + BIT6);
  }
}

/**
 * TurnOffCG2
 *
 *
 * @retval  VOID
 *
 */
VOID
TurnOffCG2 (
  OUT VOID
  )
{
}

/**
 * BackUpCG2
 *
 *
 * @retval  VOID
 *
 */
VOID
BackUpCG2 (
  OUT VOID
  )
{
}

/**
 * HpetInit - Program Fch HPET function
 *
 *
 *
 * @param[in] FchDataPtr         Fch configuration structure pointer.
 *
 */
VOID
HpetInit (
  IN  VOID     *FchDataPtr
  )
{
  DESCRIPTION_HEADER   *HpetTable;
  UINT8                FchHpetTimer;
  UINT8                FchHpetMsiDis;
  FCH_DATA_BLOCK         *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  FchHpetTimer = (UINT8) LocalCfgPtr->Hpet.HpetEnable;
  FchHpetMsiDis = (UINT8) LocalCfgPtr->Hpet.HpetMsiDis;

  HpetTable = NULL;
  if ( FchHpetTimer == TRUE ) {
    //
    //Program the HPET BAR address
    //
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG50, AccessWidth32, FCH_HPET_REG_MASK, LocalCfgPtr->Hpet.HpetBase);

    //
    //Enabling decoding of HPET MMIO
    //Enable HPET MSI support
    //Enable High Precision Event Timer (also called Multimedia Timer) interrupt
    //
    if ( FchHpetMsiDis == FALSE ) {
      RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG50, AccessWidth32, FCH_HPET_REG_MASK, BIT0 + BIT1 + BIT2 + BIT3 + BIT4);
#ifdef FCH_TIMER_TICK_INTERVAL_WA
      RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG50, AccessWidth32, FCH_HPET_REG_MASK, BIT0 + BIT1);
#endif
    } else {
      RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG50, AccessWidth32, FCH_HPET_REG_MASK, BIT0 + BIT1);
    }

  } else {
    if ( ! (LocalCfgPtr->Misc.S3Resume) ) {
      HpetTable = (DESCRIPTION_HEADER*) AcpiLocateTable (Int32FromChar('H','P','E','T'));//'TEPH'
    }
    if ( HpetTable != NULL ) {
      HpetTable->Signature = Int32FromChar('T','E','P','H');//'HPET'
    }
  }
}

/**
 * ProgramPcieNativeMode - Config Pcie Native Mode
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ProgramPcieNativeMode (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        FchNativepciesupport;
  FCH_DATA_BLOCK         *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  FchNativepciesupport = (UINT8) LocalCfgPtr->Misc.NativePcieSupport;

  //
  // PCIE Native setting
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGBA + 1, AccessWidth8, (UINT32)~BIT6, 0);
  if ( FchNativepciesupport == 1) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0x74 + 3, AccessWidth8, (UINT32)~(BIT3 + BIT1 + BIT0), BIT3 + BIT0);
  } else {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0x74 + 3, AccessWidth8, (UINT32)~(BIT3 + BIT1 + BIT0), BIT3);
  }
}
