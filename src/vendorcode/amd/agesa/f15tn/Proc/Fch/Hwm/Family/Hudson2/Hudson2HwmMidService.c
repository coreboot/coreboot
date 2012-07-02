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
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG2E, AccessWidth8, (UINT32)~(BIT1 + BIT2), BIT2);
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
    RwMem (ACPI_MMIO_BASE + PMIO2_BASE + FCH_PMIO2_REG92, AccessWidth8, (UINT32)~BIT3, BIT3);
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
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG2E, AccessWidth8, (UINT32)~(BIT1 + BIT2), 0);
    RwMem (ACPI_MMIO_BASE + PMIO2_BASE + FCH_PMIO2_REG92, AccessWidth8, (UINT32)~BIT3, 0x00);
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

