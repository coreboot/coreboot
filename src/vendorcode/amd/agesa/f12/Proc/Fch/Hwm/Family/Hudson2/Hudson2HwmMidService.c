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
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
****************************************************************************
*/
#include "FchPlatform.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_HWM_FAMILY_HUDSON2_HUDSON2HWMMIDSERVICE_FILECODE

/**
 * hwmFchtsiAutoPolling - Hardware Monitor Auto Poll SB-TSI.
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
HwmFchtsiAutoPolling (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        ValueByte;
  UINT16       SmbusBase;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  SmbusBase = (UINT16) (LocalCfgPtr->HwAcpi.Smbus0BaseAddress);

  if (LocalCfgPtr->Hwm.HwmFchtsiAutoPoll == 1) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG2E, AccessWidth8, ~(BIT1 + BIT2), BIT2);
    ValueByte = 0xff;
    LibAmdIoWrite (AccessWidth8, SmbusBase, &ValueByte, StdHeader);
    ValueByte = 0x08;
    LibAmdIoWrite (AccessWidth8, SmbusBase + 2, &ValueByte, StdHeader);
    ValueByte = 0x09;
    LibAmdIoWrite (AccessWidth8, SmbusBase + 3, &ValueByte, StdHeader);
    ValueByte = 0x98;
    LibAmdIoWrite (AccessWidth8, SmbusBase + 4, &ValueByte, StdHeader);
    ValueByte = 0x20;
    LibAmdIoWrite (AccessWidth8, SmbusBase + 5, &ValueByte, StdHeader);
    ValueByte = 0x48;
    LibAmdIoWrite (AccessWidth8, SmbusBase + 2, &ValueByte, StdHeader);

    LibAmdIoRead (AccessWidth8, SmbusBase + 0, &ValueByte, StdHeader);
    while ( ValueByte & BIT0 ) {
      LibAmdIoRead (AccessWidth8, SmbusBase + 0, &ValueByte, StdHeader);
    }

    ValueByte = 0x08;
    LibAmdIoWrite (AccessWidth8, SmbusBase + 2, &ValueByte, StdHeader);
    ValueByte = 0x10;
    LibAmdIoWrite (AccessWidth8, SmbusBase + 3, &ValueByte, StdHeader);
    ValueByte = 0x99;
    LibAmdIoWrite (AccessWidth8, SmbusBase + 4, &ValueByte, StdHeader);

    ValueByte = 0x80;
    LibAmdIoWrite (AccessWidth8, SmbusBase + 0x14, &ValueByte, StdHeader);
    ValueByte = 0x01;
    LibAmdIoWrite (AccessWidth8, SmbusBase + 0x17, &ValueByte, StdHeader);
    ValueByte = 0x81;
    LibAmdIoWrite (AccessWidth8, SmbusBase + 0x14, &ValueByte, StdHeader);

    //
    //map SB-TSI to tempin0
    //
    RwMem (ACPI_MMIO_BASE + PMIO2_BASE + FCH_PMIO2_REG92, AccessWidth8, ~BIT3, BIT3);
  } else {
    HwmFchtsiAutoPollingOff (LocalCfgPtr);
  }
}

/**
 * HwmFchtsiAutoPollingOff - Hardware Monitor Auto Poll SB-TSI
 * Off.
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
HwmFchtsiAutoPollingOff (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        ValueByte;
  UINT16       SmbusBase;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  if ( LocalCfgPtr->Hwm.HwMonitorEnable ) {
    SmbusBase = (UINT16) (LocalCfgPtr->HwAcpi.Smbus0BaseAddress);
    ValueByte = 0x00;
    LibAmdIoWrite (AccessWidth8, SmbusBase + 0x14, &ValueByte, StdHeader);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG2E, AccessWidth8, ~(BIT1 + BIT2), 0);
    RwMem (ACPI_MMIO_BASE + PMIO2_BASE + FCH_PMIO2_REG92, AccessWidth8, ~BIT3, 0x00);
  }
}

/**
 * HwmGetRawData - Hardware Monitor Get Raw Data.
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
HwmGetRawData (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        Index;
  UINT16       ValueWord;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  //fan speed
  //
  for ( Index = 0; Index < 5 ; Index++ ) {
    ReadPmio2 (FCH_PMIO2_REG69 + Index * 5, AccessWidth16, &ValueWord, StdHeader);
    if ( (ValueWord & 0xFFC0) != 0xFFC0 ) {
      LocalCfgPtr->Hwm.HwmCurrentRaw.FanSpeed[Index] = ValueWord;
    }
  }
  //
  //temperatue
  //
  for ( Index = 0; Index < 5 ; Index++ ) {
    ReadPmio2 (FCH_PMIO2_REG95 + Index * 4, AccessWidth16, &ValueWord, StdHeader);
    if ( ( Index == 1 ) || (ValueWord > 0x4000) ) {
      LocalCfgPtr->Hwm.HwmCurrentRaw.Temperature[Index] = ValueWord;
    }
  }
  //
  //voltage
  //
  for ( Index = 0; Index < 8 ; Index++ ) {
    ReadPmio2 (FCH_PMIO2_REGB8 + Index * 4, AccessWidth16, &ValueWord, StdHeader);
    if ( (ValueWord & 0xFFC0) != 0xFFC0 ) {
      LocalCfgPtr->Hwm.HwmCurrentRaw.Voltage[Index] = ValueWord;
    }
  }
}

/**
 * HwmCaculate - Hardware Monitor Caculate Raw Data to Display Data.
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
HwmCaculate (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        Index;
  UINT16       ValueWord;
  FCH_DATA_BLOCK         *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;

  //
  //fan speed
  //
  for ( Index = 0; Index < 5 ; Index++ ) {
    ValueWord = LocalCfgPtr->Hwm.HwmCurrentRaw.FanSpeed[Index];
    if ((ValueWord == 0xffff) || (ValueWord == 0x0000)) {
      LocalCfgPtr->Hwm.HwmCurrent.FanSpeed[Index] = 0;
    } else {
      LocalCfgPtr->Hwm.HwmCurrent.FanSpeed[Index] = ( 22720 >> LocalCfgPtr->Hwm.FanSampleFreqDiv ) * 60 / ValueWord / 2;
    }
  }
  //
  //temperatue
  //
  for ( Index = 0; Index < 5 ; Index++ ) {
    ValueWord = LocalCfgPtr->Hwm.HwmCurrentRaw.Temperature[Index];
    if ((LocalCfgPtr->Hwm.HwmFchtsiAutoPoll == 1) && (Index == 1)) {
      ValueWord = ((ValueWord & 0xff00) >> 8) * 10 + (((ValueWord & 0x00ff) * 10 ) >> 8);
    } else {
      ValueWord = ((ValueWord << 3) * LocalCfgPtr->Hwm.HwmTempPar[Index].At / LocalCfgPtr->Hwm.HwmCalibrationFactor / 100 - LocalCfgPtr->Hwm.HwmTempPar[Index].Ct) / 100  ;
    }
    if ( LocalCfgPtr->Hwm.HwmCurrent.Temperature[Index] == 0 ) {
      ValueWord = 0;
    }
    if ( ValueWord < 10000 ) {
      LocalCfgPtr->Hwm.HwmCurrent.Temperature[Index] = ValueWord;
    } else {
      LocalCfgPtr->Hwm.HwmCurrent.Temperature[Index] = 0;
    }
  }
  //
  //voltage
  //
  for ( Index = 0; Index < 8 ; Index++ ) {
    ValueWord = LocalCfgPtr->Hwm.HwmCurrentRaw.Voltage[Index];
    LocalCfgPtr->Hwm.HwmCurrent.Voltage[Index] = (ValueWord >> 6) * 512 / LocalCfgPtr->Hwm.HwmCalibrationFactor;
  }
}

