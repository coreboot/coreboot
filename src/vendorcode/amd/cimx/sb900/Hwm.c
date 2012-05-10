/**
 * @file
 *
 * Southbridge Initial routine
 *
 *
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
//
// Declaration of local functions
//
VOID
hwmInitRegister (
  IN       AMDSBCFG* pConfig
  );

VOID
hwmGetRawData (
  IN       AMDSBCFG* pConfig
  );

VOID
hwmCaculate (
  IN       AMDSBCFG* pConfig
  );

VOID
hwmGetCalibrationFactor (
  IN       AMDSBCFG* pConfig
  );

VOID
hwmProcessParameter (
  IN       AMDSBCFG* pConfig
  );

VOID
hwmSetRegister (
  IN       AMDSBCFG* pConfig
  );


HWM_temp_par_struct tempParDefault[] = {
  { 5219, 27365 , 0 },
  { 5222, 27435 , 0 },
  { 5219, 27516 , BIT0 },  //High Ratio
  { 5221, 27580 , BIT1 },  //High Current
  { 5123, 27866 , 0 }
};

//#ifndef NO_HWM_SUPPORT
/**
 * hwmInitRegister - Init Hardware Monitor Register.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
hwmInitRegister (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32 LinearRangeOutLimit;

  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xB2, AccWidthUint8, 0, 0x55);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xB3, AccWidthUint8, 0, 0x55);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x91, AccWidthUint8, 0, 0x55);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x92, AccWidthUint8, 0, 0x55);

  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x00, AccWidthUint8, 0, 0x06);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x10, AccWidthUint8, 0, 0x06);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x20, AccWidthUint8, 0, 0x06);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x30, AccWidthUint8, 0, 0x06);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x40, AccWidthUint8, 0, 0x06);

  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x66, AccWidthUint8, 0, 0x01);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x6B, AccWidthUint8, 0, 0x01);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x70, AccWidthUint8, 0, 0x01);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x75, AccWidthUint8, 0, 0x01);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x7A, AccWidthUint8, 0, 0x01);

  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xE6, AccWidthUint8, 0xff, 0x02);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xF8, AccWidthUint8, 0, 0x05);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xF9, AccWidthUint8, 0, 0x06);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xFF, AccWidthUint8, 0, 0x42);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xE9, AccWidthUint8, 0, 0xFF);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xEB, AccWidthUint8, 0, 0x1F);
  //2.13 HWM Sensor Clk
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xEF, AccWidthUint8, 0, 0x0A);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xFB, AccWidthUint8, 0, 0x00);

  //2.9 Enhancement of FanOut0 Control
  //check for fanLinearEnhanceEn
  if (pConfig->hwm.fanLinearEnhanceEn == 0) {
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG50, AccWidthUint32, ~ BIT11, BIT11);
    LinearRangeOutLimit = (UINT32) (pConfig->hwm.fanLinearRangeOutLimit);
    LinearRangeOutLimit = LinearRangeOutLimit << 20;
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGB4, AccWidthUint32, 0xFF0FFFFF, LinearRangeOutLimit);
  } else {
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG50, AccWidthUint32, ~ BIT11, 0);
  }
  //check for fanLinearHoldFix
  if (pConfig->hwm.fanLinearHoldFix == 0) {
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG50, AccWidthUint32, ~ BIT20, BIT20);
  } else {
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG50, AccWidthUint32, ~ BIT20, 0);
  }
}


/**
 * hwmSbtsiAutoPolling - Hardware Monitor Auto Poll SB-TSI.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
hwmSbtsiAutoPolling (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8 dbValue;
  UINT16 smbusBase;

  smbusBase = (UINT16) (pConfig->BuildParameters.Smbus0BaseAddress);
  if (pConfig->hwm.hwmSbtsiAutoPoll == 1) {
    hwmSbtsiAutoPollingPause (pConfig);

    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG2E, AccWidthUint8, ~(BIT1 + BIT2), BIT2);
    dbValue = 0xff;
    WriteIO (smbusBase, AccWidthUint8, &dbValue);
    dbValue = 0x08;
    WriteIO (smbusBase + 2, AccWidthUint8, &dbValue);
    dbValue = 0x09;
    WriteIO (smbusBase + 3, AccWidthUint8, &dbValue);
    dbValue = 0x98;
    WriteIO (smbusBase + 4, AccWidthUint8, &dbValue);
    if ( IsSbA11 () ) {
      dbValue = 0x00;
    } else {
      dbValue = 0x20;
    }
    WriteIO (smbusBase + 5, AccWidthUint8, &dbValue);
    dbValue = 0x48;
    WriteIO (smbusBase + 2, AccWidthUint8, &dbValue);

    ReadIO (smbusBase + 0, AccWidthUint8, &dbValue);
    while ( dbValue & BIT0 ) {
      ReadIO (smbusBase + 0, AccWidthUint8, &dbValue);
    }

    if ( IsSbA11 () ) {
      dbValue = 0x09;
    } else {
      dbValue = 0x08;
    }
    WriteIO (smbusBase + 2, AccWidthUint8, &dbValue);
    if ( IsSbA11 () ) {
      dbValue = 0x01;
    } else {
      dbValue = 0x10;
    }
    WriteIO (smbusBase + 3, AccWidthUint8, &dbValue);
    dbValue = 0x99;
    WriteIO (smbusBase + 4, AccWidthUint8, &dbValue);
    if ( IsSbA11 () ) {
      dbValue = 0x0f;
      WriteIO (smbusBase + 0x14, AccWidthUint8, &dbValue);
    }

    if ( IsSbA12Plus () ) {
      dbValue = 0x80;
      WriteIO (smbusBase + 0x14, AccWidthUint8, &dbValue);
      dbValue = 0x01;
      WriteIO (smbusBase + 0x17, AccWidthUint8, &dbValue);
      dbValue = 0x81;
      WriteIO (smbusBase + 0x14, AccWidthUint8, &dbValue);
    }
    //map SB-TSI to tempin0
    RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + SB_PMIO2_REG92, AccWidthUint8, ~BIT3, BIT3);
    dbValue = 0x00;
    WriteIO (smbusBase + 0x16, AccWidthUint8, &dbValue);
    pConfig->hwm.hwmSbtsiAutoPollStarted = TRUE;
  } else {
    hwmSbtsiAutoPollingOff (pConfig);
  }
}

/**
 * hwmSbtsiAutoPollingOff - Hardware Monitor Auto Poll SB-TSI
 * Off.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
hwmSbtsiAutoPollingOff (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8 dbValue;
  UINT16 smbusBase;

  if ( pConfig->hwm.hwmEnable ) {
    smbusBase = (UINT16) (pConfig->BuildParameters.Smbus0BaseAddress);
    dbValue = 0x00;
    WriteIO (smbusBase + 0x14, AccWidthUint8, &dbValue);
    hwmSbtsiAutoPollingPause (pConfig);
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG2E, AccWidthUint8, ~(BIT1 + BIT2), 0);
    RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + SB_PMIO2_REG92, AccWidthUint8, ~BIT3, 0x00);
    pConfig->hwm.hwmSbtsiAutoPollStarted = FALSE;
  }
}

/**
 * hwmSbtsiAutoPollingPause - Pause Hardware Monitor Auto Poll
 * SB-TSI Off.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
hwmSbtsiAutoPollingPause (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8 dbValue;
  UINT16 smbusBase;

  if ( pConfig->hwm.hwmEnable && (pConfig->hwm.hwmSbtsiAutoPoll == 1) ) {
    smbusBase = (UINT16) (pConfig->BuildParameters.Smbus0BaseAddress);
    dbValue = 0x01;
    WriteIO (smbusBase + 0x16, AccWidthUint8, &dbValue);
    dbValue = 0x00;
    while ( dbValue == 0x00 ) {
      ReadIO (smbusBase + 0x16, AccWidthUint8, &dbValue);
    }
  }
}

/**
 * hwmGetRawData - Hardware Monitor Get Raw Data.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
hwmGetRawData (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8 i;
  UINT16 dwValue;
//_asm { jmp $ };
  //fan speed
  for ( i = 0; i < 5 ; i++ ) {
    ReadPMIO2 (SB_PMIO2_REG69 + i * 5, AccWidthUint16, &dwValue);
    if ( (dwValue & 0xFFC0) != 0xFFC0 ) {
      pConfig->hwm.hwmCurrentRaw.fanSpeed[i] = dwValue;
    } else {
      pConfig->hwm.hwmCurrentRaw.fanSpeed[i] = 0xFFFF;
    }
  }
  //temperatue
  for ( i = 0; i < 5 ; i++ ) {
    ReadPMIO2 (SB_PMIO2_REG95 + i * 4, AccWidthUint16, &dwValue);
    if ( ( i == 1 ) || (dwValue > 0x4000) ) {
      pConfig->hwm.hwmCurrentRaw.temperature[i] = dwValue;
    }
  }
  //voltage
  for ( i = 0; i < 8 ; i++ ) {
    ReadPMIO2 (SB_PMIO2_REGB8 + i * 4, AccWidthUint16, &dwValue);
    if ( (dwValue & 0xFFC0) != 0xFFC0 ) {
      pConfig->hwm.hwmCurrentRaw.voltage[i] = dwValue;
    }
  }
}

/**
 * hwmCaculate - Hardware Monitor Caculate Raw Data to Display Data.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
hwmCaculate (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8 i;
  UINT16 dwValue;
  //UINT32 ddValue;
  //fan speed
  for ( i = 0; i < 5 ; i++ ) {
    dwValue = pConfig->hwm.hwmCurrentRaw.fanSpeed[i];
    if ((dwValue == 0xffff) || (dwValue == 0x0000)) {
      pConfig->hwm.hwmCurrent.fanSpeed[i] = 0;
    } else {
      pConfig->hwm.hwmCurrent.fanSpeed[i] = ( 22720 >> pConfig->hwm.fanSampleFreqDiv ) * 60 / dwValue / 2;
    }
  }
  //temperatue
  for ( i = 0; i < 5 ; i++ ) {
    dwValue = pConfig->hwm.hwmCurrentRaw.temperature[i];
    if ((pConfig->hwm.hwmSbtsiAutoPoll == 1) && (i == 1)) {
      if ( IsSbA11 () ) {
        dwValue = (dwValue >> 8) * 10;
      } else {
        dwValue = ((dwValue & 0xff00) >> 8) * 10 + (((dwValue & 0x00ff) * 10 ) >> 8);
      }
    } else {
      dwValue = ((dwValue << 3) * pConfig->hwm.hwmTempPar[i].At / pConfig->hwm.hwmCalibrationFactor / 100 - pConfig->hwm.hwmTempPar[i].Ct) / 10  ;
    }
    if ( pConfig->hwm.hwmCurrentRaw.temperature[i] == 0 ) {
      dwValue = 0;
    }
    if ( dwValue < 10000 ) {
      pConfig->hwm.hwmCurrent.temperature[i] = dwValue;
    } else {
      pConfig->hwm.hwmCurrent.temperature[i] = 0;
    }
  }
  //voltage
  for ( i = 0; i < 8 ; i++ ) {
    dwValue = pConfig->hwm.hwmCurrentRaw.voltage[i];
    pConfig->hwm.hwmCurrent.voltage[i] = (dwValue >> 6) * 512 / pConfig->hwm.hwmCalibrationFactor;
  }
}

/**
 * hwmGetCalibrationFactor - Hardware Monitor Get Calibration
 * Factor
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
hwmGetCalibrationFactor (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8 dbValue;
//  UINT16 dwValue;
//  UINT32 ddValue;
  //temperatue parameter
  ReadPMIO2 (SB_PMIO2_REGEA, AccWidthUint8, &dbValue);
  if ( dbValue & BIT7 ) {
    if ( dbValue & BIT6 ) {pConfig->hwm.hwmCalibrationFactor = 0x100 + dbValue;
      } else {pConfig->hwm.hwmCalibrationFactor = 0x200 + (dbValue & 0x3f ); }
  } else {
    pConfig->hwm.hwmCalibrationFactor = 0x200;
  }
}

/**
 * hwmProcessParameter - Hardware Monitor process Parameter
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
hwmProcessParameter (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8 i;
  UINT8 tempChannel;
  UINT8 dbValue;
  UINT16 dwValue;
//  UINT32 ddValue;
  hwmGetCalibrationFactor (pConfig);
  //temperatue parameter
  for ( i = 0; i < 5 ; i++ ) {
    if ( pConfig->hwm.hwmTempPar[i].At == 0 ) {
      pConfig->hwm.hwmTempPar[i] = tempParDefault[i];
    }
  }
  for ( i = 0; i < 5 ; i++ ) {
    if ( pConfig->hwm.hwmFanControl[i].LowDuty_reg03 == 100 ) {
      pConfig->hwm.hwmFanControlCooked[i].LowDuty_reg03 = 255;
    } else {
      pConfig->hwm.hwmFanControlCooked[i].LowDuty_reg03 = (pConfig->hwm.hwmFanControl[i].LowDuty_reg03 << 8) / 100;
    }
    if ( pConfig->hwm.hwmFanControl[i].MedDuty_reg04 == 100 ) {
      pConfig->hwm.hwmFanControlCooked[i].MedDuty_reg04 = 255;
    } else {
      pConfig->hwm.hwmFanControlCooked[i].MedDuty_reg04 = (pConfig->hwm.hwmFanControl[i].MedDuty_reg04 << 8) / 100;
    }

    if ( pConfig->hwm.hwmFanControl[i].HighTemp_reg0A > pConfig->hwm.hwmFanControl[i].MedTemp_reg08 ) {
      dbValue = (UINT8) ((256 - pConfig->hwm.hwmFanControlCooked[i].LowDuty_reg03) / (pConfig->hwm.hwmFanControl[i].HighTemp_reg0A - pConfig->hwm.hwmFanControl[i].MedTemp_reg08));
    } else {
      dbValue = (UINT8) ((256 - pConfig->hwm.hwmFanControlCooked[i].LowDuty_reg03));
    }

    dwValue = pConfig->hwm.hwmFanControl[i].LowTemp_reg06;
    if (pConfig->hwm.hwmFanControl[i].InputControl_reg00 > 4) {
      tempChannel = 0;
    } else {
      tempChannel = pConfig->hwm.hwmFanControl[i].InputControl_reg00;
    }
    if ((pConfig->hwm.hwmSbtsiAutoPoll == 1) && (i == 0)) {
      dwValue = dwValue << 8;
    } else {
      dbValue = (UINT8) (dbValue * 10000 * pConfig->hwm.hwmCalibrationFactor / pConfig->hwm.hwmTempPar[tempChannel].At / 512);
      dwValue = ((dwValue * 100 + pConfig->hwm.hwmTempPar[tempChannel].Ct ) * 100 * pConfig->hwm.hwmCalibrationFactor / pConfig->hwm.hwmTempPar[tempChannel].At) >> 3;
    }
    pConfig->hwm.hwmFanControlCooked[i].LowTemp_reg06 = dwValue;
    pConfig->hwm.hwmFanControlCooked[i].Multiplier_reg05 = dbValue & 0x3f;

    dwValue = pConfig->hwm.hwmFanControl[i].MedTemp_reg08;
    if ((pConfig->hwm.hwmSbtsiAutoPoll == 1) && (i == 0)) {
      dwValue = dwValue << 8;
    } else {
      dwValue = ((dwValue * 100 + pConfig->hwm.hwmTempPar[tempChannel].Ct ) * 100 * pConfig->hwm.hwmCalibrationFactor / pConfig->hwm.hwmTempPar[tempChannel].At) >> 3;
    }
    pConfig->hwm.hwmFanControlCooked[i].MedTemp_reg08 = dwValue;
    dwValue = pConfig->hwm.hwmFanControl[i].HighTemp_reg0A;
    if ((pConfig->hwm.hwmSbtsiAutoPoll == 1) && (i == 0)) {
      dwValue = dwValue << 8;
    } else {
      dwValue = ((dwValue * 100 + pConfig->hwm.hwmTempPar[tempChannel].Ct ) * 100 * pConfig->hwm.hwmCalibrationFactor / pConfig->hwm.hwmTempPar[tempChannel].At) >> 3;
    }
    pConfig->hwm.hwmFanControlCooked[i].HighTemp_reg0A = dwValue;
  }
}

/**
 * hwmSetRegister - Hardware Monitor Set Parameter
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
hwmSetRegister (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8 *pDbValue;
  UINT8 i;
  UINT8 registerN;
  UINT8 registerPM2RegF8;
  UINT8 registerPM2RegF9;

  //UINT16 dwValue;
//  UINT32 ddValue;
  //Configure Fans
  for ( i = 0; i < 5 ; i++ ) {
    pDbValue = &(pConfig->hwm.hwmFanControlCooked[i].InputControl_reg00);
    for ( registerN = 0; registerN < 0x0E ; registerN++ ) {
      WritePMIO2 (i * 0x10 + registerN, AccWidthUint8, pDbValue);
      pDbValue ++;
    }
  }
  //Configure Sample Frequency Divider
  WritePMIO2 (SB_PMIO2_REG63, AccWidthUint8, &(pConfig->hwm.fanSampleFreqDiv));

  //Configure Mode
  ReadPMIO2 (0xF8, AccWidthUint8, &registerPM2RegF8);
  ReadPMIO2 (0xF9, AccWidthUint8, &registerPM2RegF9);
  for ( i = 0; i < 5 ; i++ ) {
    if (pConfig->hwm.hwmTempPar[i].Mode == BIT0) {
      registerPM2RegF8 |= 1 << (i + 3);
    } else if (pConfig->hwm.hwmTempPar[i].Mode == BIT1) {
      registerPM2RegF9 |= 1 << (i + 3);
    }
  }
  WritePMIO2 (0xF8, AccWidthUint8, &registerPM2RegF8);
  WritePMIO2 (0xF9, AccWidthUint8, &registerPM2RegF9);
}

/**
 * hwmUpdateData - Hardware Monitor Update Data.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
hwmUpdateData (
  IN       AMDSBCFG* pConfig
  )
{
  if ( pConfig->hwm.hwmEnable ) {
    hwmSbtsiAutoPolling (pConfig);
    hwmGetRawData (pConfig);
    hwmCaculate (pConfig);
  }
}

/**
 * hwmCopyFanControl - Hardware Monitor Copy Fan Control Data.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
hwmCopyFanControl (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8 *fanControl;
  UINT8 *fanControlCooked;
  if ( pConfig->hwm.hwmEnable ) {
    fanControl = & pConfig->hwm.hwmFanControl[0].InputControl_reg00;
    fanControlCooked = & pConfig->hwm.hwmFanControlCooked[0].InputControl_reg00;
    MemoryCopy (fanControlCooked, fanControl, (sizeof (HWM_fan_ctl_struct) * 5));
  }
}
/**
 * hwmInit - Init Hardware Monitor.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
hwmInit (
  IN       AMDSBCFG* pConfig
  )
{
  hwmInitRegister (pConfig);
  if ( pConfig->hwm.hwmEnable ) {
    hwmCopyFanControl (pConfig);
    hwmProcessParameter (pConfig);
    hwmSetRegister (pConfig);
    hwmSbtsiAutoPolling (pConfig);
  }
}

//#endif
