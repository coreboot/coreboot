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
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
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
