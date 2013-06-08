/* $NoKeywords:$ */
/**
 * @file
 *
 * Config FCH Hwm controller
 *
 * Init Hwm Controller features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#include "Filecode.h"
#define FILECODE PROC_FCH_HWM_FAMILY_HUDSON2_HUDSON2HWMENVSERVICE_FILECODE

FCH_HWM_TEMP_PAR TempParDefault[] = {
  { 5220, 27365 , 0 },
  { 5225, 27435 , 0 },
  { 5220, 27516 , BIT0 },         ///High Ratio
  { 5212, 27580 , BIT1 },         ///High Current
  { 5123, 27866 , 0 }
};

/**
 * HwmInitRegister - Init Hardware Monitor Register.
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
HwmInitRegister (
  IN  VOID     *FchDataPtr
  )
{
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0xB2, AccessWidth8, 0, 0x55);
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0xB3, AccessWidth8, 0, 0x55);
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0x91, AccessWidth8, 0, 0x55);
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0x92, AccessWidth8, 0, 0x55);

  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0x00, AccessWidth8, 0, 0x06);
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0x10, AccessWidth8, 0, 0x06);
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0x20, AccessWidth8, 0, 0x06);
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0x30, AccessWidth8, 0, 0x06);
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0x40, AccessWidth8, 0, 0x06);

  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0x66, AccessWidth8, 0, 0x01);
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0x6B, AccessWidth8, 0, 0x01);
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0x70, AccessWidth8, 0, 0x01);
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0x75, AccessWidth8, 0, 0x01);
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0x7A, AccessWidth8, 0, 0x01);

  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0xE6, AccessWidth8, 0xff, 0x02);
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0xF8, AccessWidth8, 0, 0x05);
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0xF9, AccessWidth8, 0, 0x06);
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0xFF, AccessWidth8, 0, 0x42);
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0xE9, AccessWidth8, 0, 0xFF);
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0xEB, AccessWidth8, 0, 0x1F);
  //RPR 2.12 HWM Sensor Clk
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0xEF, AccessWidth8, 0, 0x0A);
  RwMem (ACPI_MMIO_BASE + PMIO2_BASE + 0xFB, AccessWidth8, 0, 0x00);
  //2.9 Enhancement of FanOut0 Control
  RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x50 , AccessWidth32, (UINT32)~ (BIT11 + BIT20), (BIT11 + BIT20));
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0xB6 , AccessWidth8, 0x0F, 0x10);
}

/**
 * HwmProcessParameter - Hardware Monitor process Parameter
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
HwmProcessParameter (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        Index;
  UINT8        TempChannel;
  UINT8        ValueByte;
  UINT16       ValueWord;
  FCH_DATA_BLOCK         *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  LibAmdMemCopy ((VOID *) (LocalCfgPtr->Hwm.HwmFanControlCooked), (VOID *) (LocalCfgPtr->Hwm.HwmFanControl), (sizeof (FCH_HWM_FAN_CTR) * 5), LocalCfgPtr->StdHeader);

  HwmGetCalibrationFactor (LocalCfgPtr);
  //
  //temperatue parameter
  //
  for ( Index = 0; Index < 5 ; Index++ ) {
    if ( LocalCfgPtr->Hwm.HwmTempPar[Index].At == 0 ) {
      LocalCfgPtr->Hwm.HwmTempPar[Index] = TempParDefault[Index];
    }
  }

  for ( Index = 0; Index < 5 ; Index++ ) {
    if ( LocalCfgPtr->Hwm.HwmFanControl[Index].LowDutyReg03 == 100 ) {
      LocalCfgPtr->Hwm.HwmFanControlCooked[Index].LowDutyReg03 = 255;
    } else {
      LocalCfgPtr->Hwm.HwmFanControlCooked[Index].LowDutyReg03 = (LocalCfgPtr->Hwm.HwmFanControl[Index].LowDutyReg03 << 8) / 100;
    }

    if ( LocalCfgPtr->Hwm.HwmFanControl[Index].MedDutyReg04 == 100 ) {
      LocalCfgPtr->Hwm.HwmFanControlCooked[Index].MedDutyReg04 = 255;
    } else {
      LocalCfgPtr->Hwm.HwmFanControlCooked[Index].MedDutyReg04 = (LocalCfgPtr->Hwm.HwmFanControl[Index].MedDutyReg04 << 8) / 100;
    }

    ValueByte = (UINT8) ((256 - LocalCfgPtr->Hwm.HwmFanControl[Index].LowDutyReg03) / (LocalCfgPtr->Hwm.HwmFanControl[Index].HighTempReg0A - LocalCfgPtr->Hwm.HwmFanControl[Index].MedTempReg08));
    ValueWord = LocalCfgPtr->Hwm.HwmFanControl[Index].LowTempReg06;

    if (LocalCfgPtr->Hwm.HwmFanControl[Index].InputControlReg00 > 4) {
      TempChannel = 0;
    } else {
      TempChannel = LocalCfgPtr->Hwm.HwmFanControl[Index].InputControlReg00;
    }

    if ((LocalCfgPtr->Hwm.HwmFchtsiAutoPoll == 1) && (Index == 0)) {
      ValueWord = ValueWord << 8;
    } else {
      ValueByte = (UINT8) (ValueByte * 10000 / LocalCfgPtr->Hwm.HwmTempPar[TempChannel].At);
      ValueWord = ((ValueWord * 100 + LocalCfgPtr->Hwm.HwmTempPar[TempChannel].Ct ) * 100 * LocalCfgPtr->Hwm.HwmCalibrationFactor / LocalCfgPtr->Hwm.HwmTempPar[TempChannel].At) >> 3;
    }
    LocalCfgPtr->Hwm.HwmFanControlCooked[Index].LowTempReg06 = ValueWord;
    LocalCfgPtr->Hwm.HwmFanControlCooked[Index].MultiplierReg05 = ValueByte & 0x3f;

    ValueWord = LocalCfgPtr->Hwm.HwmFanControl[Index].MedTempReg08;
    if ((LocalCfgPtr->Hwm.HwmFchtsiAutoPoll == 1) && (Index == 0)) {
      ValueWord = ValueWord << 8;
    } else {
      ValueWord = ((ValueWord * 100 + LocalCfgPtr->Hwm.HwmTempPar[TempChannel].Ct ) * 100 * LocalCfgPtr->Hwm.HwmCalibrationFactor / LocalCfgPtr->Hwm.HwmTempPar[TempChannel].At) >> 3;
    }
    LocalCfgPtr->Hwm.HwmFanControlCooked[Index].MedTempReg08 = ValueWord;

    ValueWord = LocalCfgPtr->Hwm.HwmFanControl[Index].HighTempReg0A;
    if ((LocalCfgPtr->Hwm.HwmFchtsiAutoPoll == 1) && (Index == 0)) {
      ValueWord = ValueWord << 8;
    } else {
      ValueWord = ((ValueWord * 100 + LocalCfgPtr->Hwm.HwmTempPar[TempChannel].Ct ) * 100 * LocalCfgPtr->Hwm.HwmCalibrationFactor / LocalCfgPtr->Hwm.HwmTempPar[TempChannel].At) >> 3;
    }
    LocalCfgPtr->Hwm.HwmFanControlCooked[Index].HighTempReg0A = ValueWord;
  }
}

/**
 * hwmSetRegister - Hardware Monitor Set Parameter
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
HwmSetRegister (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        *DbValuePtr;
  UINT8        Index;
  UINT8        RegisterN;
  UINT8        RegisterPM2RegF8;
  UINT8        RegisterPM2RegF9;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  //Configure Fans
  //
  for ( Index = 0; Index < 5 ; Index++ ) {
    DbValuePtr = &(LocalCfgPtr->Hwm.HwmFanControlCooked[Index].InputControlReg00);
    for ( RegisterN = 0; RegisterN < 0x0E ; RegisterN++ ) {
      WritePmio2 (Index * 0x10 + RegisterN, AccessWidth8, DbValuePtr, StdHeader);
      DbValuePtr ++;
    }
  }

  //
  //Configure Sample Frequency Divider
  //
  WritePmio2 (0x63 , AccessWidth8, &(LocalCfgPtr->Hwm.FanSampleFreqDiv), StdHeader);

  //
  //Configure Mode
  //
  ReadPmio2 (0xF8, AccessWidth8, &RegisterPM2RegF8, StdHeader);
  ReadPmio2 (0xF9, AccessWidth8, &RegisterPM2RegF9, StdHeader);
  for ( Index = 0; Index < 5 ; Index++ ) {
    if (LocalCfgPtr->Hwm.HwmTempPar[Index].Mode == BIT0) {
      RegisterPM2RegF8 |= 1 << (Index + 3);
    } else if (LocalCfgPtr->Hwm.HwmTempPar[Index].Mode == BIT1) {
      RegisterPM2RegF9 |= 1 << (Index + 3);
    }
  }
  WritePmio2 (0xF8, AccessWidth8, &RegisterPM2RegF8, StdHeader);
  WritePmio2 (0xF9, AccessWidth8, &RegisterPM2RegF9, StdHeader);
}

/**
 * hwmGetCalibrationFactor - Hardware Monitor Get Calibration
 * Factor
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
HwmGetCalibrationFactor (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        ValueByte;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  //temperatue parameter
  //
  ReadPmio2 (FCH_PMIO2_REGEA, AccessWidth8, &ValueByte, StdHeader);
  if ( ValueByte & BIT7 ) {
    if ( ValueByte & BIT6 ) {
      LocalCfgPtr->Hwm.HwmCalibrationFactor = 0x100 + ValueByte;
    } else {
      LocalCfgPtr->Hwm.HwmCalibrationFactor = 0x200 + (ValueByte & 0x3f );
    }
  } else {
    LocalCfgPtr->Hwm.HwmCalibrationFactor = 0x200;
  }
}
