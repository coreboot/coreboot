
/**
 * @file
 *
 * Southbridge CIMx configuration structure define
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

#ifndef _AMD_SBTYPE_H_
#define _AMD_SBTYPE_H_

#ifndef  LEGACY_BIOS_SUPPORT
  #pragma pack (push, 1)
#endif
/**
 * Entry point of Southbridge CIMx
 *
 *
 * @param[in] Param1  Southbridge CIMx Function ID.
 * @param[in] Param2  Southbridge Input Data.
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
typedef unsigned int (*SBCIM_HOOK_ENTRY) (unsigned int Param1, unsigned int Param2, void* pConfig);
/**
 * SMM_SERVICE_ROUTINE - Southbridge SMI service routine
 *
 */
typedef  void (*SMM_SERVICE_ROUTINE) (void);


/**
 * The STATIC platform information for CIMx Module.
 *
 */
typedef struct _BUILDPARAM {
  unsigned char   BiosSize;           /**< BiosSize 00-00-01
                                   *   @par
                                   *   BIOSSize [2.0] - BIOS Image Size
                                   *    @li <b>0</b> - 1M
                                   *    @li <b>1</b> - 2M
                                   *    @li <b>3</b> - 4M
                                   *    @li <b>7</b> - 8M
                                   *  In Hudson-2, default ROM size is 1M Bytes, if your platform ROM bigger then 1M
                                   *  you have to set the ROM size outside CIMx module and before AGESA module get call
                                   *
                                   */
  unsigned char   LegacyFree;         /**< LegacyFree 00-00-02
                                   *   @par
                                   * Config Southbridge CIMx module for Legacy Free Mode
                                   */
  unsigned char   SpiSpeed;           /**< SpiSpeed  00-00-03
                                   *   @par
                                   *    SPI Speed [1.0] - the clock speed for non-fast read command
                                   *    @li <b>000</b> - Default
                                   *    @li <b>001</b> - 66Mhz
                                   *    @li <b>010</b> - 33Mhz
                                   *    @li <b>011</b> - 22Mhz
                                   *    @li <b>100</b> - 16.5Mhz
                                   *
                                   */
  unsigned char   ImcEnableOverWrite; /**< ImcEnableOverWrite  00-00-04
                                   *   @par
                                   *    Imc Enable OverWrite
                                   *    @li <b>00</b> - by default strapping
                                   *    @li <b>01</b> - On
                                   *    @li <b>10</b> - Off
                                   *
                                   */
  unsigned char   SpiFastSpeed;   /**< SpiFastSpeed  00-00-05
                                   *   @par
                                   *    SPI Speed [1.0] - the clock speed for fast read command
                                   *    @li <b>000</b> - Default
                                   *    @li <b>001</b> - 66Mhz
                                   *    @li <b>010</b> - 33Mhz
                                   *    @li <b>011</b> - 22Mhz
                                   *    @li <b>100</b> - 16.5Mhz
                                   *
                                   */
  unsigned char   SpiMode;   /**< SpiMode  00-00-06
                                   *   @par
                                   *     @li <b>000</b> - Default
                                   *     @li <b>101</b> - Qual-io 1-4-4
                                   *     @li <b>100</b> - Dual-io 1-2-2
                                   *     @li <b>011</b> - Qual-io 1-1-4
                                   *     @li <b>010</b> - Dual-io 1-1-2
                                   *     @li <b>111</b> - FastRead
                                   *     @li <b>110</b> - Normal
                                   */
  unsigned char   SpiAutoMode; /**< SpiAutoMode  00-00-07
                                   *   @par
                                   *    SPI Auto Mode
                                   *    @li <b>0</b> - Disabled
                                   *    @li <b>1</b> - Enabled
                                   */
/** SpiBurstWrite - Reserved  00-00-08 */
  unsigned char   SpiBurstWrite;
  unsigned char   EcKbd;              /**< EcKbd  00-00-09
                                   *   @par
                                   *   EcKbd [16] - Platform use EC (as SIO) or SIO chip for PS/2 Keyboard and Mouse
                                   *    @li <b>0</b> - Use SIO PS/2 function.
                                   *    @li <b>1</b> - Use EC PS/2 function instead of SIO PS/2 function. **
                                   *    @li <b>**</b> When set 1, EC function have to enable, otherwise, CIMx treat as legacy-free system.
                                   */
/** EcChannel0 - Reserved  00-00-10 */
  unsigned char   EcChannel0;
/** UsbMsi - Reserved  00-00-11*/
  unsigned char   UsbMsi;
/** HdAudioMsi - Reserved  00-00-12*/
  unsigned char   HdAudioMsi;
/** LpcMsi - Reserved  00-00-13*/
  unsigned char   LpcMsi;
/** PcibMsi - Reserved  00-00-14*/
  unsigned char   PcibMsi;
/** AbMsi - Reserved  00-00-15*/
  unsigned char   AbMsi;
  unsigned char   SpiWriteSpeed;   /**< SpiReadSpeed  00-00-16
                                   *   @par
                                   *     TBD
                                   */

  unsigned int   Smbus0BaseAddress;    /**< Smbus0BaseAddress  00-00-17
                                   *   @par
                                   * Smbus BASE Address
                                   */
  unsigned int   Smbus1BaseAddress;    /**< Smbus1BaseAddress  00-00-18
                                   *   @par
                                   * Smbus1 (ASF) BASE Address
                                   */
  unsigned int   SioPmeBaseAddress;    /**< SioPmeBaseAddress  00-00-19
                                   *   @par
                                   * SIO PME BASE Address
                                   */
  unsigned int   WatchDogTimerBase;    /**< WatchDogTimerBase  00-00-20
                                   *   @par
                                   *  Watch Dog Timer Address
                                   */
  unsigned int   GecShadowRomBase;     /**< GecShadowRomBase  00-00-21
                                   *   @par
                                   * GEC (NIC) SHADOWROM BASE Address
                                   */
  unsigned int   SpiRomBaseAddress;    /**< SpiRomBaseAddress  00-00-22
                                   *   @par
                                   * SPI ROM BASE Address
                                   */
  unsigned short   AcpiPm1EvtBlkAddr;    /**< AcpiPm1EvtBlkAddr  00-00-23
                                   *   @par
                                   * ACPI PM1 event block Address
                                   */
  unsigned short   AcpiPm1CntBlkAddr;    /**< AcpiPm1CntBlkAddr  00-00-24
                                   *   @par
                                   * ACPI PM1 Control block Address
                                   */
  unsigned short   AcpiPmTmrBlkAddr;     /**< AcpiPmTmrBlkAddr  00-00-25
                                   *   @par
                                   * ACPI PM timer block Address
                                   */
  unsigned short   CpuControlBlkAddr;    /**< CpuControlBlkAddr  00-00-26
                                   *   @par
                                   * ACPI CPU control block Address
                                   */
  unsigned short   AcpiGpe0BlkAddr;      /**< AcpiGpe0BlkAddr  00-00-27
                                   *   @par
                                   * ACPI GPE0 block Address
                                   */
  unsigned short   SmiCmdPortAddr;       /**< SmiCmdPortAddr  00-00-28
                                   *   @par
                                   * SMI command port Address
                                   */
  unsigned short   AcpiPmaCntBlkAddr;    /**< AcpiPmaCntBlkAddr  00-00-29
                                   *   @par
                                   * ACPI PMA Control block Address
                                   */
  unsigned int   HpetBase;             /**< HpetBase  00-00-30
                                   *   @par
                                   * HPET Base address
                                   */
  unsigned int   SataIDESsid;          /**< SataIDESsid  00-00-31
                                   *   @par
                                   * SATA IDE mode SSID
                                   */
  unsigned int   SataRAIDSsid;         /**< SataRAIDSsid  00-00-32
                                   *   @par
                                   *  SATA RAID mode SSID
                                   */
  unsigned int   SataRAID5Ssid;        /**< SataRAID5Ssid  00-00-33
                                   *   @par
                                   * SATA RAID5 mode SSID
                                   */
  unsigned int   SataAHCISsid;         /**< SataAHCISsid  00-00-34
                                   *   @par
                                   * SATA AHCI mode SSID
                                   */
  unsigned int   OhciSsid;             /**< OhciSsid  00-00-35
                                   *   @par
                                   * OHCI Controller SSID
                                   */
  unsigned int   EhciSsid;             /**< EhciSsid  00-00-36
                                   *   @par
                                   * EHCI Controller SSID
                                   */
  unsigned int   Ohci4Ssid;            /**< Ohci4Ssid  00-00-37
                                   *   @par
                                   * OHCI4 Controller SSID (Force USB 1.1 mode)
                                   */
  unsigned int   SmbusSsid;            /**< SmbusSsid  00-00-38
                                   *   @par
                                   *  SMBUS controller SSID
                                   */
  unsigned int   IdeSsid;              /**< IdeSsid  00-00-39
                                   *   @par
                                   * IDE (Sata) controller SSID
                                   */
  unsigned int   AzaliaSsid;           /**< AzaliaSsid  00-00-40
                                   *   @par
                                   * HD Audio controller SSID
                                   */
  unsigned int   LpcSsid;              /**< LpcSsid  00-00-41
                                   *   @par
                                   * LPC controller SSID
                                   */
  unsigned int   PCIBSsid;             /**< PCIBSsid  00-00-42
                                   *   @par
                                   * PCIB controller SSID
                                   */
} BUILDPARAM;

/**
 * The Hwm temprature parameter  for CIMx Module. *
 */
typedef struct _HWM_temp_par_struct {
  unsigned short At;            ///< At
  unsigned short Ct;          ///< Ct
  unsigned char Mode;             ///< Mode BIT0:HiRatio BIT1:HiCurrent
} HWM_temp_par_struct;

/**
 * The Hwm current struct  for CIMx Module. *
 */
typedef struct _HWM_cur_struct {
  unsigned short fanSpeed[5];            ///< fanSpeed
  unsigned short temperature[5];          ///< temperature
  unsigned short voltage[8];             ///< voltage
} HWM_cur_struct;

/**
 * The Hwm control struct  for CIMx Module.
 *
 */
typedef struct _HWM_fan_ctl_struct {
  unsigned char InputControl_reg00;       ///< InputControl_reg00
  unsigned char Control_reg01;            ///< Control_reg01
  unsigned char Freq_reg02;               ///< Freq_reg02
  unsigned char LowDuty_reg03;            ///< LowDuty_reg03
  unsigned char MedDuty_reg04;            ///< MedDuty_reg04
  unsigned char Multiplier_reg05;         ///< Multiplier_reg05
  unsigned short LowTemp_reg06;           ///< LowTemp_reg06
  unsigned short MedTemp_reg08;           ///< MedTemp_reg08
  unsigned short HighTemp_reg0A;          ///< HighTemp_reg0A
  unsigned char LinearRange_reg0C;        ///< LinearRange_reg0C
  unsigned char LinearHoldCount_reg0D;    ///< LinearHoldCount_reg0D
} HWM_fan_ctl_struct;

/**
 * The Hwm struct  for CIMx Module. *
 */
typedef struct _HWM_struct {
  unsigned char hwmEnable;            ///< hwmEnable
  unsigned int hwmControl;                    /**< hwmControl
                                        *   @par
                                        * HWM control configuration
                                        *    @li <b>0</b> - HWM is Enabled
                                        *    @li <b>1</b> - IMC is Enabled
                                        */
  unsigned char fanSampleFreqDiv;            ///< fanSampleFreqDiv
  unsigned char hwmSbtsiAutoPoll;            ///< hwmSbtsiAutoPoll
  unsigned char hwmSbtsiAutoPollStarted;            ///< hwmSbtsiAutoPollStarted
  unsigned char fanLinearEnhanceEn;          ///< fanLinearEnhanceEn
  unsigned char fanLinearHoldFix;            ///< fanLinearHoldFix
  unsigned char fanLinearRangeOutLimit;      ///< fanLinearRangeOutLimit
  unsigned char hwmSbtsiMode;            ///< hwmSbtsiMode
  unsigned short hwmCalibrationFactor;            ///< hwmEfuseFactor
  HWM_cur_struct hwmCurrent;            ///< hwmCurrent
  HWM_cur_struct hwmCurrentRaw;            ///< hwmCurrentRaw
  HWM_temp_par_struct hwmTempPar[5];          ///< hwmTempPar
  HWM_fan_ctl_struct hwmFanControl[5];             ///< hwmFanControl
  HWM_fan_ctl_struct hwmFanControlCooked[5];             ///< hwmFanControlCooked
} HWM_struct;

/**
 * The IMC struct  for CIMx Module. *
 */
typedef struct _IMC_struct {
  unsigned char imcEnable;            ///< imcEnable
  unsigned char imcEnabled;            ///< imcEnabled
  unsigned char imcSureBootTimer;            ///< imcSureBootTimer
} IMC_struct;

/**
 * The EC fan MSGREG struct  for CIMx Module. *
 */
typedef struct _EC_struct {
  unsigned char  MSGFun81zone0MSGREG0;  ///<Thermal zone
  unsigned char  MSGFun81zone0MSGREG1;  ///<Thermal zone
  unsigned char  MSGFun81zone0MSGREG2;  ///<Thermal zone control byte 1
  unsigned char  MSGFun81zone0MSGREG3;  ///<Thermal zone control byte 2
  unsigned char  MSGFun81zone0MSGREG4;  ///<Bit[3:0] - Thermal diode offset adjustment in degrees Celsius.
  unsigned char  MSGFun81zone0MSGREG5;  ///<Hysteresis inforamtion
  unsigned char  MSGFun81zone0MSGREG6;  ///<SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032
  unsigned char  MSGFun81zone0MSGREG7;  ///<Bit[1:0]: 0 - 2,  SMBUS bus number where the SMBUS based temperature sensor is located.
  unsigned char  MSGFun81zone0MSGREG8;  ///< @todo Style_Analyzer: Add Doxygen comments to struct entry
  unsigned char  MSGFun81zone0MSGREG9;  ///< @todo Style_Analyzer: Add Doxygen comments to struct entry

  //EC LDN9 funtion 81 zone 1
  unsigned char  MSGFun81zone1MSGREG0;  ///<Thermal zone
  unsigned char  MSGFun81zone1MSGREG1;  ///<Thermal zone
  unsigned char  MSGFun81zone1MSGREG2;  ///<Thermal zone control byte 1
  unsigned char  MSGFun81zone1MSGREG3;  ///<Thermal zone control byte 2
  unsigned char  MSGFun81zone1MSGREG4;  ///<Bit[3:0] - Thermal diode offset adjustment in degrees Celsius.
  unsigned char  MSGFun81zone1MSGREG5;  ///<Hysteresis inforamtion
  unsigned char  MSGFun81zone1MSGREG6;  ///<SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032
  unsigned char  MSGFun81zone1MSGREG7;  ///<Bit[1:0]: 0 - 2,  SMBUS bus number where the SMBUS based temperature sensor is located.
  unsigned char  MSGFun81zone1MSGREG8;  ///< @todo Style_Analyzer: Add Doxygen comments to struct entry
  unsigned char  MSGFun81zone1MSGREG9;  ///< @todo Style_Analyzer: Add Doxygen comments to struct entry

  //EC LDN9 funtion 81 zone 2
  unsigned char  MSGFun81zone2MSGREG0;  ///<Thermal zone
  unsigned char  MSGFun81zone2MSGREG1;  ///<Thermal zone
  unsigned char  MSGFun81zone2MSGREG2;  ///<Thermal zone control byte 1
  unsigned char  MSGFun81zone2MSGREG3;  ///<Thermal zone control byte 2
  unsigned char  MSGFun81zone2MSGREG4;  ///<Bit[3:0] - Thermal diode offset adjustment in degrees Celsius.
  unsigned char  MSGFun81zone2MSGREG5;  ///<Hysteresis inforamtion
  unsigned char  MSGFun81zone2MSGREG6;  ///<SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032
  unsigned char  MSGFun81zone2MSGREG7;  ///<Bit[1:0]: 0 - 2,  SMBUS bus number where the SMBUS based temperature sensor is located.
  unsigned char  MSGFun81zone2MSGREG8;  ///< @todo Style_Analyzer: Add Doxygen comments to struct entry
  unsigned char  MSGFun81zone2MSGREG9;  ///< @todo Style_Analyzer: Add Doxygen comments to struct entry

  //EC LDN9 funtion 81 zone 3
  unsigned char  MSGFun81zone3MSGREG0;  ///<Thermal zone
  unsigned char  MSGFun81zone3MSGREG1;  ///<Thermal zone
  unsigned char  MSGFun81zone3MSGREG2;  ///<Thermal zone control byte 1
  unsigned char  MSGFun81zone3MSGREG3;  ///<Thermal zone control byte 2
  unsigned char  MSGFun81zone3MSGREG4;  ///<Bit[3:0] - Thermal diode offset adjustment in degrees Celsius.
  unsigned char  MSGFun81zone3MSGREG5;  ///<Hysteresis inforamtion
  unsigned char  MSGFun81zone3MSGREG6;  ///<SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032
  unsigned char  MSGFun81zone3MSGREG7;  ///<Bit[1:0]: 0 - 2,  SMBUS bus number where the SMBUS based temperature sensor is located.
  unsigned char  MSGFun81zone3MSGREG8;  ///< @todo Style_Analyzer: Add Doxygen comments to struct entry
  unsigned char  MSGFun81zone3MSGREG9;  ///< @todo Style_Analyzer: Add Doxygen comments to struct entry

  //EC LDN9 funtion 83 zone 0
  unsigned char  MSGFun83zone0MSGREG0;  ///<Thermal zone
  unsigned char  MSGFun83zone0MSGREG1;  ///<Thermal zone
  unsigned char  MSGFun83zone0MSGREG2;  ///<_AC0
  unsigned char  MSGFun83zone0MSGREG3;  ///<_AC1
  unsigned char  MSGFun83zone0MSGREG4;  ///<_AC2
  unsigned char  MSGFun83zone0MSGREG5;  ///<_AC3
  unsigned char  MSGFun83zone0MSGREG6;  ///<_AC4
  unsigned char  MSGFun83zone0MSGREG7;  ///<_AC5
  unsigned char  MSGFun83zone0MSGREG8;  ///<_AC6
  unsigned char  MSGFun83zone0MSGREG9;  ///<_AC7
  unsigned char  MSGFun83zone0MSGREGA;  ///<_CRT
  unsigned char  MSGFun83zone0MSGREGB;  ///<_PSV

  //EC LDN9 funtion 83 zone 1
  unsigned char  MSGFun83zone1MSGREG0;  ///<Thermal zone
  unsigned char  MSGFun83zone1MSGREG1;  ///<Thermal zone
  unsigned char  MSGFun83zone1MSGREG2;  ///<_AC0
  unsigned char  MSGFun83zone1MSGREG3;  ///<_AC1
  unsigned char  MSGFun83zone1MSGREG4;  ///<_AC2
  unsigned char  MSGFun83zone1MSGREG5;  ///<_AC3
  unsigned char  MSGFun83zone1MSGREG6;  ///<_AC4
  unsigned char  MSGFun83zone1MSGREG7;  ///<_AC5
  unsigned char  MSGFun83zone1MSGREG8;  ///<_AC6
  unsigned char  MSGFun83zone1MSGREG9;  ///<_AC7
  unsigned char  MSGFun83zone1MSGREGA;  ///<_CRT
  unsigned char  MSGFun83zone1MSGREGB;  ///<_PSV

  //EC LDN9 funtion 83 zone 2
  unsigned char  MSGFun83zone2MSGREG0;  ///<Thermal zone
  unsigned char  MSGFun83zone2MSGREG1;  ///<Thermal zone
  unsigned char  MSGFun83zone2MSGREG2;  ///<_AC0
  unsigned char  MSGFun83zone2MSGREG3;  ///<_AC1
  unsigned char  MSGFun83zone2MSGREG4;  ///<_AC2
  unsigned char  MSGFun83zone2MSGREG5;  ///<_AC3
  unsigned char  MSGFun83zone2MSGREG6;  ///<_AC4
  unsigned char  MSGFun83zone2MSGREG7;  ///<_AC5
  unsigned char  MSGFun83zone2MSGREG8;  ///<_AC6
  unsigned char  MSGFun83zone2MSGREG9;  ///<_AC7
  unsigned char  MSGFun83zone2MSGREGA;  ///<_CRT
  unsigned char  MSGFun83zone2MSGREGB;  ///<_PSV

  //EC LDN9 funtion 83 zone 3
  unsigned char  MSGFun83zone3MSGREG0;  ///<Thermal zone
  unsigned char  MSGFun83zone3MSGREG1;  ///<Thermal zone
  unsigned char  MSGFun83zone3MSGREG2;  ///<_AC0
  unsigned char  MSGFun83zone3MSGREG3;  ///<_AC1
  unsigned char  MSGFun83zone3MSGREG4;  ///<_AC2
  unsigned char  MSGFun83zone3MSGREG5;  ///<_AC3
  unsigned char  MSGFun83zone3MSGREG6;  ///<_AC4
  unsigned char  MSGFun83zone3MSGREG7;  ///<_AC5
  unsigned char  MSGFun83zone3MSGREG8;  ///<_AC6
  unsigned char  MSGFun83zone3MSGREG9;  ///<_AC7
  unsigned char  MSGFun83zone3MSGREGA;  ///<_CRT
  unsigned char  MSGFun83zone3MSGREGB;  ///<_PSV

  //EC LDN9 funtion 85 zone 0
  unsigned char  MSGFun85zone0MSGREG0;  ///<Thermal zone
  unsigned char  MSGFun85zone0MSGREG1;  ///<Thermal zone
  unsigned char  MSGFun85zone0MSGREG2;  ///<AL0 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone0MSGREG3;  ///<AL1 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone0MSGREG4;  ///<AL2 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone0MSGREG5;  ///<AL3 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone0MSGREG6;  ///<AL4 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone0MSGREG7;  ///<AL5 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone0MSGREG8;  ///<AL6 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone0MSGREG9;  ///<AL7 PWM level in percentage (0 - 100%)

  //EC LDN9 funtion 85 zone 1
  unsigned char  MSGFun85zone1MSGREG0;  ///<Thermal zone
  unsigned char  MSGFun85zone1MSGREG1;  ///<Thermal zone
  unsigned char  MSGFun85zone1MSGREG2;  ///<AL0 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone1MSGREG3;  ///<AL1 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone1MSGREG4;  ///<AL2 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone1MSGREG5;  ///<AL3 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone1MSGREG6;  ///<AL4 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone1MSGREG7;  ///<AL5 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone1MSGREG8;  ///<AL6 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone1MSGREG9;  ///<AL7 PWM level in percentage (0 - 100%)

  //EC LDN9 funtion 85 zone 2
  unsigned char  MSGFun85zone2MSGREG0;  ///<Thermal zone
  unsigned char  MSGFun85zone2MSGREG1;  ///<Thermal zone
  unsigned char  MSGFun85zone2MSGREG2;  ///<AL0 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone2MSGREG3;  ///<AL1 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone2MSGREG4;  ///<AL2 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone2MSGREG5;  ///<AL3 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone2MSGREG6;  ///<AL4 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone2MSGREG7;  ///<AL5 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone2MSGREG8;  ///<AL6 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone2MSGREG9;  ///<AL7 PWM level in percentage (0 - 100%)

  //EC LDN9 funtion 85 zone 3
  unsigned char  MSGFun85zone3MSGREG0;  ///<Thermal zone
  unsigned char  MSGFun85zone3MSGREG1;  ///<Thermal zone
  unsigned char  MSGFun85zone3MSGREG2;  ///<AL0 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone3MSGREG3;  ///<AL1 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone3MSGREG4;  ///<AL2 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone3MSGREG5;  ///<AL3 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone3MSGREG6;  ///<AL4 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone3MSGREG7;  ///<AL5 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone3MSGREG8;  ///<AL6 PWM level in percentage (0 - 100%)
  unsigned char  MSGFun85zone3MSGREG9;  ///<AL7 PWM level in percentage (0 - 100%)

  //EC LDN9 funtion 89 TEMPIN channel 0
  unsigned char  MSGFun89zone0MSGREG0;  ///<Thermal zone
  unsigned char  MSGFun89zone0MSGREG1;  ///<Thermal zone
  unsigned char  MSGFun89zone0MSGREG2;  ///<At DWORD bit 0-7
  unsigned char  MSGFun89zone0MSGREG3;  ///<At DWORD bit 15-8
  unsigned char  MSGFun89zone0MSGREG4;  ///<At DWORD bit 23-16
  unsigned char  MSGFun89zone0MSGREG5;  ///<At DWORD bit 31-24
  unsigned char  MSGFun89zone0MSGREG6;  ///<Ct DWORD bit 0-7
  unsigned char  MSGFun89zone0MSGREG7;  ///<Ct DWORD bit 15-8
  unsigned char  MSGFun89zone0MSGREG8;  ///<Ct DWORD bit 23-16
  unsigned char  MSGFun89zone0MSGREG9;  ///<Ct DWORD bit 31-24
  unsigned char  MSGFun89zone0MSGREGA;  ///<Mode bit 0-7

  //EC LDN9 funtion 89 TEMPIN channel 1
  unsigned char  MSGFun89zone1MSGREG0;  ///<Thermal zone
  unsigned char  MSGFun89zone1MSGREG1;  ///<Thermal zone
  unsigned char  MSGFun89zone1MSGREG2;  ///<At DWORD bit 0-7
  unsigned char  MSGFun89zone1MSGREG3;  ///<At DWORD bit 15-8
  unsigned char  MSGFun89zone1MSGREG4;  ///<At DWORD bit 23-16
  unsigned char  MSGFun89zone1MSGREG5;  ///<At DWORD bit 31-24
  unsigned char  MSGFun89zone1MSGREG6;  ///<Ct DWORD bit 0-7
  unsigned char  MSGFun89zone1MSGREG7;  ///<Ct DWORD bit 15-8
  unsigned char  MSGFun89zone1MSGREG8;  ///<Ct DWORD bit 23-16
  unsigned char  MSGFun89zone1MSGREG9;  ///<Ct DWORD bit 31-24
  unsigned char  MSGFun89zone1MSGREGA;  ///<Mode bit 0-7

  //EC LDN9 funtion 89 TEMPIN channel 2
  unsigned char  MSGFun89zone2MSGREG0;  ///<Thermal zone
  unsigned char  MSGFun89zone2MSGREG1;  ///<Thermal zone
  unsigned char  MSGFun89zone2MSGREG2;  ///<At DWORD bit 0-7
  unsigned char  MSGFun89zone2MSGREG3;  ///<At DWORD bit 15-8
  unsigned char  MSGFun89zone2MSGREG4;  ///<At DWORD bit 23-16
  unsigned char  MSGFun89zone2MSGREG5;  ///<At DWORD bit 31-24
  unsigned char  MSGFun89zone2MSGREG6;  ///<Ct DWORD bit 0-7
  unsigned char  MSGFun89zone2MSGREG7;  ///<Ct DWORD bit 15-8
  unsigned char  MSGFun89zone2MSGREG8;  ///<Ct DWORD bit 23-16
  unsigned char  MSGFun89zone2MSGREG9;  ///<Ct DWORD bit 31-24
  unsigned char  MSGFun89zone2MSGREGA;  ///<Mode bit 0-7

  //EC LDN9 funtion 89 TEMPIN channel 3
  unsigned char  MSGFun89zone3MSGREG0;  ///<Thermal zone
  unsigned char  MSGFun89zone3MSGREG1;  ///<Thermal zone
  unsigned char  MSGFun89zone3MSGREG2;  ///<At DWORD bit 0-7
  unsigned char  MSGFun89zone3MSGREG3;  ///<At DWORD bit 15-8
  unsigned char  MSGFun89zone3MSGREG4;  ///<At DWORD bit 23-16
  unsigned char  MSGFun89zone3MSGREG5;  ///<At DWORD bit 31-24
  unsigned char  MSGFun89zone3MSGREG6;  ///<Ct DWORD bit 0-7
  unsigned char  MSGFun89zone3MSGREG7;  ///<Ct DWORD bit 15-8
  unsigned char  MSGFun89zone3MSGREG8;  ///<Ct DWORD bit 23-16
  unsigned char  MSGFun89zone3MSGREG9;  ///<Ct DWORD bit 31-24
  unsigned char  MSGFun89zone3MSGREGA;  ///<Mode bit 0-7

  // FLAG for Fun83/85/89 support
  unsigned short IMCFUNSupportBitMap;   ///< Bit0=81FunZone0 support(1=On;0=Off); bit1-3=81FunZone1-Zone3;Bit4-7=83FunZone0-Zone3;Bit8-11=85FunZone0-Zone3;Bit11-15=89FunZone0-Zone3;
} EC_struct;

/** SBGPPPORTCONFIG - Southbridge GPP port config structure */
typedef struct {
  unsigned char   PortPresent;     /**< Port connection
                                   *   @par
                                   *    @li <b>0</b> - Port doesn't have slot. No need to train the link
                                   *    @li <b>1</b> - Port connection defined and needs to be trained
                                   */
  unsigned char   PortDetected;    /**< Link training status
                                   *   @par
                                   *    @li <b>0</b> - EP not detected
                                   *    @li <b>1</b> - EP detected
                                   */
  unsigned char   PortIsGen2;      /**< Port link speed configuration
                                   *   @par
                                   *    @li <b>00</b> - Auto
                                   *    @li <b>01</b> - Forced GEN1
                                   *    @li <b>10</b> - Forced GEN2
                                   *    @li <b>11</b> - Reserved
                                   */

  unsigned char   PortHotPlug;     /**< Support hot plug?
                                   *   @par
                                   *    @li <b>0</b> - No support
                                   *    @li <b>1</b> - support
                                   */
} SBGPPPORTCONFIG;

/** CODECENTRY - Southbridge HD Audio OEM Codec structure */
typedef struct _CODECENTRY {
/** Nid - Reserved ?? */
  unsigned char Nid;
/** Byte40 - Reserved ?? */
  unsigned int Byte40;
} CODECENTRY;

/** CODECTBLLIST - Southbridge HD Audio Codec table list */
typedef struct _CODECTBLLIST {
/** CodecID - Codec ID */
  unsigned int   CodecID;
/** CodecTablePtr - Codec table pointer */
  CODECENTRY* CodecTablePtr;
} CODECTBLLIST;

/** Sata Controller structure */
typedef struct _SATAST {
  unsigned char   SataModeReg;               /** SataModeReg - Reserved 00-01-00*/
  unsigned char   SataController;            /**< SataController 00-01-01
                                        *   @par
                                        * Sata Controller
                                        *    @li <b>0</b> - disable
                                        *    @li <b>1</b> - enable
                                        */
  unsigned char   SataIdeCombMdPriSecOpt;    /**< SataIdeCombMdPriSecOpt - Reserved 00-01-02*/
  unsigned char   SataSetMaxGen2;            /**< SataSetMaxGen2 00-01-03
                                        *   @par
                                        * Sata Controller Set to Max Gen2 mode
                                        *    @li <b>0</b> - disable
                                        *    @li <b>1</b> - enable
                                        */
  unsigned char   SataIdeCombinedMode;       /**< SataIdeCombinedMode 00-01-04
                                        *   @par
                                        * Sata IDE Controller set to Combined Mode
                                        *    @li <b>0</b> - disable
                                        *    @li <b>1</b> - enable
                                        */
/** SataClkMode - Reserved 00-01-05 */
  unsigned char   SataClkMode;              // 4:7
} SATAST;

/** _USBST Controller structure
 *
 * Usb Ohci1 Contoller is defined at BIT0
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 18 Func0) *
 * Usb Ehci1 Contoller is defined at BIT1
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 18 Func2) *
 * Usb Ohci2 Contoller is defined at BIT2
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 19 Func0) *
 * Usb Ehci2 Contoller is defined at BIT3
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 19 Func2) *
 * Usb Ohci3 Contoller is defined at BIT4
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 22 Func0) *
 * Usb Ehci3 Contoller is defined at BIT5
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 22 Func2) *
 * Usb Ohci4 Contoller is defined at BIT6
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 20 Func5) *
 */
typedef struct _USBST {
  unsigned char   UsbModeReg;               /** UsbModeReg - Reserved */
  unsigned char  Ohci1;                      ///< Ohci0 controller - 0:disable, 1:enable
  unsigned char  Ehci1;                      ///< Ehci1 controller - 0:disable, 1:enable
  unsigned char  Ohci2;                      ///< Ohci2 controller - 0:disable, 1:enable
  unsigned char  Ehci2;                      ///< Ehci2 controller - 0:disable, 1:enable
  unsigned char  Ohci3;                      ///< Ohci3 controller - 0:disable, 1:enable
  unsigned char  Ehci3;                      ///< Ehci3 controller - 0:disable, 1:enable
  unsigned char  Ohci4;                      ///< Ohci4 controller - 0:disable, 1:enable
} USBST;


/** SATAPORTST - SATA PORT structure
 *
 */
typedef struct _SATAPORTST {
  unsigned char    SataPortPowerReg;         /** SataPortPowerReg - Reserved */
  unsigned char  PORT0;                      ///< PORT0 - 0:disable, 1:enable
  unsigned char  PORT1;                      ///< PORT1 - 0:disable, 1:enable
  unsigned char  PORT2;                      ///< PORT2 - 0:disable, 1:enable
  unsigned char  PORT3;                      ///< PORT3 - 0:disable, 1:enable
  unsigned char  PORT4;                      ///< PORT4 - 0:disable, 1:enable
  unsigned char  PORT5;                      ///< PORT5 - 0:disable, 1:enable
  unsigned char  PORT6;                      ///< PORT6 - 0:disable, 1:enable
  unsigned char  PORT7;                      ///< PORT7 - 0:disable, 1:enable
} SATAPORTST;


///< _SATAPORTMD - Force Each PORT to GEN1/GEN2 mode
typedef struct _SATAPORTMD {
  unsigned short   SataPortMode;             /** SataPortMode - Reserved */
  unsigned short  PORT0;                     ///< PORT0 - set BIT0 to GEN1, BIT1 - PORT0 set to GEN2
  unsigned short  PORT1;                     ///< PORT1 - set BIT2 to GEN1, BIT3 - PORT1 set to GEN2
  unsigned short  PORT2;                     ///< PORT2 - set BIT4 to GEN1, BIT5 - PORT2 set to GEN2
  unsigned short  PORT3;                     ///< PORT3 - set BIT6 to GEN1, BIT7 - PORT3 set to GEN2
  unsigned short  PORT4;                     ///< PORT4 - set BIT8 to GEN1, BIT9 - PORT4 set to GEN2
  unsigned short  PORT5;                     ///< PORT5 - set BIT10 to GEN1, BIT11 - PORT5 set to GEN2
  unsigned short  PORT6;                     ///< PORT6 - set BIT12 to GEN1, BIT13 - PORT6 set to GEN2
  unsigned short  PORT7;                     ///< PORT7 - set BIT14 to GEN1, BIT15 - PORT7 set to GEN2
} SATAPORTMD;

/**
 * _AZALIAPIN - HID Azalia or GPIO define structure.
 *
 */
typedef struct _AZALIAPIN {
  unsigned char   AzaliaSdinPin;             ///< @todo Style_Analyzer: Add Doxygen comments to struct entry
  unsigned char   AzaliaSdin0;               /**< AzaliaSdin0
                                        *   @par
                                        * SDIN0 is defined at BIT0 & BIT1
                                        *    @li <b>00</b> - GPIO PIN
                                        *    @li <b>10</b> - As a Azalia SDIN pin
                                        */
  unsigned char   AzaliaSdin1;               /**< AzaliaSdin1
                                        *   @par
                                        * SDIN0 is defined at BIT2 & BIT3
                                        *    @li <b>00</b> - GPIO PIN
                                        *    @li <b>10</b> - As a Azalia SDIN pin
                                        */
  unsigned char   AzaliaSdin2;               /**< AzaliaSdin2
                                        *   @par
                                        * SDIN0 is defined at BIT4 & BIT5
                                        *    @li <b>00</b> - GPIO PIN
                                        *    @li <b>10</b> - As a Azalia SDIN pin
                                        */
  unsigned char   AzaliaSdin3;               /**< AzaliaSdin3
                                        *   @par
                                        * SDIN0 is defined at BIT6 & BIT7
                                        *    @li <b>00</b> - GPIO PIN
                                        *    @li <b>10</b> - As a Azalia SDIN pin
                                        */
} AZALIAPIN;

/** AMDSBCFG - Southbridge CIMx configuration structure (Main) */
typedef struct _AMDSBCFG {
/** StdHeader - Standard header for all AGESA/CIMx services. */
  AMD_CONFIG_PARAMS   StdHeader;

/** BuildParameters - The STATIC platform information for CIMx Module. 00-00-00 */
  BUILDPARAM BuildParameters;
                                      //offset 90 bytes (32-121)
  //MsgXchgBiosCimx                   //offset 8 bytes (122-129)
  // SATA Configuration

  union /**< union - Reserved 00-01-00 */
  {  /**< SATAMODE - Sata Controller structure */
/** SataModeReg - Reserved */
    unsigned char   SataModeReg;
/** SataMode - Reserved */
    SATAST  SataMode;
  } SATAMODE;
/** S3Resume - Flag of ACPI S3 Resume. 00-02 Internal USED */
  unsigned char   S3Resume;                   // 8
/** RebootRequired - Flag of Reboot system is required. 00-03 Internal USED (retired) */
  unsigned char   RebootRequired;             // 9
/** SbSpiSpeedSupport - Reserved 00-04 (retired)*/
  unsigned char   SbSpiSpeedSupport;   // 10
/**< SpreadSpectrum 00-05-00
 *   @par
 *   Spread Spectrum function
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  unsigned char   SpreadSpectrum;             // 11
/** NbSbGen2 - Reserved 00-06-00 */
  unsigned char   NbSbGen2;                   // 12 00-07
  unsigned char   GppGen2;                    // 13 00-08
  unsigned char   GppMemWrImprove;            // 14 00-09
  unsigned char   S4Resume;                   // 15 00-10 Internal
  unsigned char   FchVariant;                 //
/**< SataClass - SATA Controller mode [2:0] 00-11
 *   @par
 *    @li <b>000</b> - Native IDE mode
 *    @li <b>001</b> - RAID mode
 *    @li <b>010</b> - AHCI mode
 *    @li <b>011</b> - Legacy IDE mode
 *    @li <b>100</b> - IDE->AHCI mode
 *    @li <b>101</b> - AHCI mode as 7804 ID (AMD driver)
 *    @li <b>110</b> - IDE->AHCI mode as 7804 ID (AMD driver)
 */
  unsigned char   SataClass;                 // 2:0
/**< SataIdeMode - Sata IDE Controller mode 00-12
 *   @par
 *    @li <b>0</b> - Legacy IDE mode
 *    @li <b>1</b> - Native IDE mode
 */
  unsigned char   SataIdeMode;               // 3
  unsigned char   sdConfig;        /**< sdMode 00-13
                                   *   @par
                                   *    SD Configuration
                                   *    @li <b>00</b> - Disabled
                                   *    @li <b>00</b> - AMDA,  set 24,18,16,  default
                                   *    @li <b>01</b> - DMA clear 24, 16, set 18
                                   *    @li <b>10</b> - PIO clear 24,18,16
                                   *
                                   */
  unsigned char   sdSpeed;        /**< sdSpeed 00-14
                                   *   @par
                                   *    SD Configuration
                                   *    @li <b>0</b> - Low speed clear 17
                                   *    @li <b>1</b> - High speed, set 17, default
                                   *
                                   */
  unsigned char   sdBitwidth;        /**< sdBitwidth 00-15
                                   *   @par
                                   *    SD Configuration
                                   *    @li <b>0</b> - 32BIT clear 23
                                   *    @li <b>1</b> - 64BIT, set 23,default
                                   *
                                   */
/**< SataDisUnusedIdePChannel - Disable Unused IDE Primary Channel 00-16
 *   @par
 *    @li <b>0</b> - Channel Enable
 *    @li <b>1</b> - Channel Disable
 */
  unsigned char   SataDisUnusedIdePChannel;
/**< SataDisUnusedIdeSChannel - Disable Unused IDE Secondary Channel 00-17
 *   @par
 *    @li <b>0</b> - Channel Enable
 *    @li <b>1</b> - Channel Disable
 */
  unsigned char   SataDisUnusedIdeSChannel;
/**< IdeDisUnusedIdePChannel - Disable Unused IDE Primary Channel 00-18
 *   @par
 *    @li <b>0</b> - Channel Enable
 *    @li <b>1</b> - Channel Disable
 */
  unsigned char   IdeDisUnusedIdePChannel;
/**< IdeDisUnusedIdeSChannel - Disable Unused IDE IDE Secondary Channel 00-19
 *   @par
 *    @li <b>0</b> - Channel Enable
 *    @li <b>1</b> - Channel Disable
 */
  unsigned char   IdeDisUnusedIdeSChannel;

/**<  SataEspPort - SATA port is external accessible on a signal only connector (eSATA:)  00-20
 *   @par
 *    @li <b> BIT0 </b> - PORT0 set as ESP port
 *    @li <b> BIT1 </b> - PORT1 set as ESP port
 *    @li <b> BIT2 </b> - PORT2 set as ESP port
 *    @li <b> BIT3 </b> - PORT3 set as ESP port
 *    @li <b> BIT4 </b> - PORT4 set as ESP port
 *    @li <b> BIT5 </b> - PORT5 set as ESP port
 *    @li <b> BIT6 </b> - PORT6 set as ESP port
 *    @li <b> BIT7 </b> - PORT7 set as ESP port
 */
  union /**< union - Reserved */
  { /**< SATAPORT - SATA Port Structure */
  /** SataPortReg - Reserved */
    unsigned char    SataPortReg;
    SATAPORTST SataEspPort;           // 23:16
  } SATAESPPORT;

  union /**< union - Reserved  00-21*/
  { /**< SATAPORT - SATA Port Structure */
  /** SataPortPowerReg - Reserved */
    unsigned char    SataPortPowerReg;
    SATAPORTST SataPortPower;         // 31:24
  } SATAPORTPOWER;

  // SATA Debug Option                //offset 4 bytes (130-133)

  union /**< union - Reserved  00-22*/
  {
/**< SataPortMode - Force Each PORT to GEN1/GEN2 mode
 *   @par
 *    @li <b> 0 </b> Auto for each PORTs
 *    @li <b> BIT0 = 1</b> - PORT0 set to GEN1
 *    @li <b> BIT1 = 1</b> - PORT0 set to GEN2
 *    @li <b> BIT2 = 1</b> - PORT1 set to GEN1
 *    @li <b> BIT3 = 1</b> - PORT1 set to GEN2
 *    @li <b> BIT4 = 1</b> - PORT2 set to GEN1
 *    @li <b> BIT5 = 1</b> - PORT2 set to GEN2
 *    @li <b> BIT6 = 1</b> - PORT3 set to GEN1
 *    @li <b> BIT7 = 1</b> - PORT3 set to GEN2
 *    @li <b> BIT8 = 1</b> - PORT4 set to GEN1
 *    @li <b> BIT9 = 1</b> - PORT4 set to GEN2
 *    @li <b> BIT10 = 1</b> - PORT5 set to GEN1
 *    @li <b> BIT11 = 1</b> - PORT5 set to GEN2
 *    @li <b> BIT12 = 1</b> - PORT6 set to GEN1
 *    @li <b> BIT13 = 1</b> - PORT6 set to GEN2
 *    @li <b> BIT14 = 1</b> - PORT7 set to GEN1
 *    @li <b> BIT15 = 1</b> - PORT7 set to GEN2
 */
    unsigned short   SataPortMode;               //15:0
    SATAPORTMD SataPortMd;
  } SATAPORTMODE;
/** SATAClkSelOpt - Reserved  00-23*/
  unsigned char   SATAClkSelOpt;              //19:16  *Removed from coding side
/** SataAggrLinkPmCap - Reserved  00-24*/
  unsigned char   SataAggrLinkPmCap;          //20, 0:OFF   1:ON
/** SataPortMultCap - Reserved  00-25*/
  unsigned char   SataPortMultCap;            //21, 0:OFF   1:ON
/** SataClkAutoOff - Reserved  00-26*/
  unsigned char   SataClkAutoOff;             //22, AutoClockOff 0:Disabled, 1:Enabled
/** SataPscCap - Reserved  00-27*/
  unsigned char   SataPscCap;                 //23, 1:Enable PSC capability, 0:Disable PSC capability
/** BIOSOSHandoff - Reserved  00-28*/
  unsigned char   BIOSOSHandoff;              //24
/** SataFisBasedSwitching - Reserved  00-29*/
  unsigned char   SataFisBasedSwitching;      //25
/** SataCccSupport - Reserved  00-30*/
  unsigned char   SataCccSupport;             //26
/** SataSscCap - Reserved  00-31*/
  unsigned char   SataSscCap;                 //27, 1:Enable SSC capability, 0:Disable SSC capability
/** SataMsiCapability - Reserved  00-32*/
  unsigned char   SataMsiCapability;          //28  0:Hidden 1:Visible
/** SataForceRaid - Reserved  00-33*/
  unsigned char   SataForceRaid;              //29  0:No function 1:Force RAID
/** SataInternal100Spread - Reserved  00-34*/
  unsigned char   SataInternal100Spread;      //30

  // SATA Debug Option (2)          //offset 4 bytes (134-137)

/** SataTargetSupport8Device - Reserved  00-35*/
  unsigned char   SataTargetSupport8Device;   // 0
/** SataDisableGenericMode - Reserved  00-36*/
  unsigned char   SataDisableGenericMode;     // 1
/** SataAhciEnclosureManagement - Reserved  00-37 */
  unsigned char   SataAhciEnclosureManagement; // 2
/** SataSgpio0 - Reserved  00-38*/
  unsigned char   SataSgpio0;                 // 3
/** SataSgpio1 - Reserved  00-39*/
  unsigned char   SataSgpio1;                 // 4
/** SataPhyPllShutDown - Reserved  00-40*/
  unsigned char   SataPhyPllShutDown;         // 5
/** SataHotRemoveEnh - Reserved */
  unsigned char   SataHotRemoveEnh;           // 6
/**< SataHotRemovelEnh for each ports - Hot-removal enhancementEnable for portN
 *   @par
 *    @li <b> BIT0 </b> - PORT0 set as HotRemovelEnh port
 *    @li <b> BIT1 </b> - PORT1 set as HotRemovelEnh port
 *    @li <b> BIT2 </b> - PORT2 set as HotRemovelEnh port
 *    @li <b> BIT3 </b> - PORT3 set as HotRemovelEnh port
 *    @li <b> BIT4 </b> - PORT4 set as HotRemovelEnh port
 *    @li <b> BIT5 </b> - PORT5 set as HotRemovelEnh port
 *    @li <b> BIT6 </b> - PORT6 set as HotRemovelEnh port
 *    @li <b> BIT7 </b> - PORT7 set as HotRemovelEnh port
 */
  union /**< union - Reserved  00-41*/
  { /**< SATAPORT - SATA Port Structure */
  /** SataHotRemovelEnh - Reserved */    // 31:24
    unsigned char    SataHotRemovelEnh:8;
    SATAPORTST SataHotRemoveEnhPort;
  } SATAHOTREMOVALENH;

//
// USB Configuration                //offset 4 bytes (134-137)
//

/** USBDeviceConfig - USB Controller Configuration
 *
 * Usb Ohci1 Contoller is defined at BIT0
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 18 Func0) *
 * Usb Ehci1 Contoller is defined at BIT1
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 18 Func2) *
 * Usb Ohci2 Contoller is defined at BIT2
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 19 Func0) *
 * Usb Ehci2 Contoller is defined at BIT3
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 19 Func2) *
 * Usb Ohci3 Contoller is defined at BIT4
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 22 Func0) *
 * Usb Ehci3 Contoller is defined at BIT5
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 22 Func2) *
 * Usb Ohci4 Contoller is defined at BIT6
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 20 Func5) *
 */

  union /**< union - Reserved */
  {  /**< USBMODE - USB Controller structure */
/** SataModeReg - Reserved  00-42*/
    unsigned char   UsbModeReg;
/** SataMode - Reserved */
    USBST  UsbMode;
  } USBMODE;

/**< GecConfig  00-43
 *   @par
 *   InChip Gbit NIC
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  unsigned char   GecConfig;                 //8
/**< IrConfig  00-44
 *   @par
 *   Ir Controller
 *    @li <b>00 </b> - disable
 *    @li <b>01 </b> - Rx and Tx0
 *    @li <b>10 </b> - Rx and Tx1
 *    @li <b>11 </b> - Rx and both Tx0,Tx1
 */
  unsigned char   IrConfig;                  //9:10
  unsigned char   XhciSwitch;                 //11  00-45

  //Azalia Configuration

/**< AzaliaController - Azalia Controller Configuration  00-46
 *   @par
 *   Azalia Controller [0-1]
 *    @li <b>0</b> - Auto   : Detect Azalia controller automatically.
 *    @li <b>1</b> - Diable : Disable Azalia controller.
 *    @li <b>2</b> - Enable : Enable Azalia controller.
 */
  unsigned char   AzaliaController;          //17:16
/**< AzaliaPinCfg - Azalia Controller SDIN pin Configuration  00-47
 *   @par
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  unsigned char   AzaliaPinCfg;              //18
/**< AzaliaFrontPanel - Azalia Controller Front Panel Configuration  00-48
 *   @par
 *   Support Front Panel configuration
 *    @li <b>0</b> - Auto
 *    @li <b>1</b> - disable
 *    @li <b>2</b> - enable
 */
  unsigned char   AzaliaFrontPanel;          //20:19
/**< FrontPanelDetected - Force Azalia Controller Front Panel Configuration  00-49
 *   @par
 *   Force Front Panel configuration
 *    @li <b>0</b> - Not Detected
 *    @li <b>1</b> - Detected
 */
  unsigned char   FrontPanelDetected;        //21
/**< AzaliaSnoop - Azalia Controller Snoop feature Configuration  00-59
 *   @par
 *   Azalia Controller Snoop feature Configuration
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  unsigned char   AzaliaSnoop;               //22
/** AzaliaDummy - Reserved */
  unsigned char   AzaliaDummy;               //23

  union
  {
/**< AzaliaSdinPin - Azalia Controller SDIN pin Configuration  00-51
 *
 * SDIN0 is defined at BIT0 & BIT1
 *   - 00: GPIO PIN
 *   - 01: Reserved
 *   - 10: As a Azalia SDIN pin
 *
 * SDIN1 is defined at BIT2 & BIT3
 *      * Config same as SDIN0
 * SDIN2 is defined at BIT4 & BIT5
 *      * Config same as SDIN0
 * SDIN3 is defined at BIT6 & BIT7
 *      * Config same as SDIN0
 */
    unsigned char   AzaliaSdinPin;
    AZALIAPIN  AzaliaConfig;
  } AZALIACONFIG;

/** AZOEMTBL - Azalia Controller OEM Codec Table Pointer  00-52
 *
 */
  union
  {
    PLACEHOLDER  PlaceHolder;
    CODECTBLLIST*  pAzaliaOemCodecTablePtr;  //offset 4 bytes (138-141)
  } AZOEMTBL;

/** AZOEMFPTBL - Azalia Controller Front Panel OEM Table Pointer  00-53
 *
 */
  union
  {
    PLACEHOLDER  PlaceHolder;
    void*   pAzaliaOemFpCodecTablePtr;       //offset 4 bytes (142-145)
  } AZOEMFPTBL;

  //Miscellaneous Configuration      //offset 4 bytes (146-149)
/** AnyHT200MhzLink - Reserved  00-54 */
  unsigned char   AnyHT200MhzLink;             //0
/**< HpetTimer - South Bridge Hpet Timer Configuration  00-56
 *   @par
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  unsigned char   HpetTimer;                 //1
/**< PciClks - PCI Slot Clock Control  00-57
 *   @par
 * PCI SLOT 0 define at BIT0
 *   - 00: disable
 *   - 01: enable
 *
 * PCI SLOT 1 define at BIT1
 *      * Config same as PCI SLOT0
 * PCI SLOT 2 define at BIT2
 *      * Config same as PCI SLOT0
 * PCI SLOT 3 define at BIT3
 *      * Config same as PCI SLOT0
 * PCI SLOT 4 define at BIT4
 *      * Config same as PCI SLOT0
 */
  unsigned char   PciClks;                   //2:6
/** MiscReserved1 - Reserved */
  unsigned char   MiscReserved1;             //9:7, Reserved
/** MobilePowerSavings - Debug function Reserved  00-58*/
  unsigned char   MobilePowerSavings;        //11, 0:Disable, 1:Enable Power saving features especially for Mobile platform
/** ExternalRTCClock - Debug function Reserved  00-59*/
  unsigned char   ExternalRTCClock;          //12, 0:Don't Shut Off, 1:Shut Off, external RTC clock
/** NativePcieSupport - Debug function Reserved  00-60*/
  unsigned char   NativePcieSupport;         //13, 0:Disabled, 1:Enable
/** FlashPinConfig - Debug function Reserved  00-61*/
  unsigned char   FlashPinConfig;            //14, 0:desktop mode 1:mobile mode
/** UsbPhyPowerDown - Debug function Reserved  00-62*/
  unsigned char   UsbPhyPowerDown;           //15
/** PcibClkStopOverride - Debug function Reserved  00-63*/
  unsigned short   PcibClkStopOverride;      //25:16
/**< HpetMsiDis - South Bridge HPET MSI Configuration  00-64 (Retired)
 *   @par
 *    @li <b>1</b> - disable
 *    @li <b>0</b> - enable
 */
  unsigned char   HpetMsiDis;                //26
/**< ResetCpuOnSyncFlood - Rest CPU on Sync Flood  00-65 (Retired)
 *   @par
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  unsigned char   ResetCpuOnSyncFlood;       //27

  //DebugOptions                     //offset 4 bytes (150-153)
/** PcibAutoClkCtr - Debug function Reserved  00-66*/
  unsigned char   PcibAutoClkCtr;

/**< OEMPROGTBL - ACPI MMIO register setting table OEM override  00-67
 *   @par
 *   OEM table for customer override ACPI MMIO register in their code.
 */
  union
  {
    PLACEHOLDER OemProgrammingTablePtr;  //offset 4 bytes (154-157)
    void   *OemProgrammingTablePtr_Ptr;
  } OEMPROGTBL;

  //Gpp Configuration                   //offset 24 bytes total (158-181)  00-68
  union {
    unsigned int           PORTCFG32;
    SBGPPPORTCONFIG  PortCfg;
  } PORTCONFIG[4];          //offset 16 bytes

  unsigned char           GppLinkConfig;       /**< GPP_LINK_CONFIG = PCIE_GPP_Enable[3:0]  00-69
                                         * @li  <b>0000</b> - Port ABCD -> 4:0:0:0
                                         * @li  <b>0001</b> - N/A
                                         * @li  <b>0010</b> - Port ABCD -> 2:2:0:0
                                         * @li  <b>0011</b> - Port ABCD -> 2:1:1:0
                                         * @li  <b>0100</b> - Port ABCD -> 1:1:1:1
                                         */
  unsigned char   GppFoundGfxDev;            ///< 3:0 If port A-D (mapped to bit [3:0]) has GFX EP detected  00-70
  unsigned char   GppGen2Strap;              ///< 4   LC_GEN2_EN_STRAP  00-71 (internal used)
  unsigned char   GppFunctionEnable;         ///< 5  00-72
  unsigned char   GppUnhidePorts;            ///< 6  00-73
  unsigned char   AlinkPhyPllPowerDown;      ///< 7  00-74
  unsigned char   GppPortAspm;               /**< 9:8 ASPM state for all GPP ports  00-75
                                         * @li   <b>01</b> - Disabled
                                         * @li   <b>01</b> - L0s
                                         * @li   <b>10</b> - L1
                                         * @li   <b>11</b> - L0s + L1
                                         */
  unsigned char   GppLaneReversal;           ///< 10  00-76
  unsigned char   GppPhyPllPowerDown;        ///< 11  00-77
  unsigned char   GppDynamicPowerSaving;     ///< 12  00-78
  unsigned char   PcieAER;                   ///< 13 Advanced Error Report: 0/1-disable/enable  00-79
/** UmiDynamicSpeedChange - Reserved */
  unsigned char   PcieRAS;                   // 14  00-80
  unsigned char   GppHardwareDowngrade;      // 20:18: 0:Disable, 1-4: portA-D  00-83
  unsigned char   GppToggleReset;            // 21  00-84
  unsigned char   sdbEnable;                 // 22  00-85

  //TempMMIO                            //offset 4 bytes (190-193)  00-86
  unsigned int   TempMMIO;

  // DebugOption2
  unsigned char   GecPhyStatus;              //  00-86
  unsigned char   GecDebugOptionDummy;       //  00-87
  unsigned char   SBGecPwr;                  //  00-88
  unsigned char   SBGecDebugBus;             //  00-89
  unsigned char   SbPcieOrderRule;           //  00-90
  unsigned char   AcDcMsg;                   //  00-91
  unsigned char   TimerTickTrack;            //  00-92
  unsigned char   ClockInterruptTag;         //  00-93
  unsigned char   OhciTrafficHanding;        //  00-94
  unsigned char   EhciTrafficHanding;        //  00-95
  unsigned char   FusionMsgCMultiCore;       //  00-95A
  unsigned char   FusionMsgCStage;           //  00-96
  unsigned char   ALinkClkGateOff;           //  00-97
  unsigned char   BLinkClkGateOff;           //  00-98
  unsigned char   Sdb;                       //  00-99
  unsigned char   SlowSpeedABlinkClock;      //  01-00
  unsigned char   MtC1eEnable;               //  01-00
  union
  {
    PLACEHOLDER DynamicGecRomAddressPtr;  //offset 4 bytes (194-201)  01-01
    void   *DynamicGecRomAddress_Ptr;
  } DYNAMICGECROM;
//  unsigned int   UmiCommon;                   //offset 4 bytes (202-205)
/** AbGppClockGating - Reserved */
  unsigned char   AbClockGating;   // 01-02
  unsigned char   GppClockGating;   // 01-03

//  unsigned int   UmiDebug;

/** L1TimerOverwrite - Reserved */
  unsigned char   L1TimerOverwrite;     // 01-04
/** UmiLinkWidth - Reserved */
  unsigned char   UmiLinkWidth; // 4:3  01-05
/** UmiDynamicSpeedChange - Reserved */
  unsigned char   UmiDynamicSpeedChange;                 // 01-06
/** PcieRefClockOverclocking - Reserved */
  unsigned char   PcieRefClockOverclocking;                 // 9:6 01-07
/** PcieRefClockOverclocking - Reserved */
  unsigned char   SbAlinkGppTxDriverStrength;                 // 12:10 01-08
  unsigned char   PwrFailShadow;               /**< PwrFailShadow = PM_Reg: 5Bh [3:0] 01-09
                                         * @li  <b>00</b> - Always off
                                         * @li  <b>01</b> - Always on
                                         * @li  <b>10</b> - Always off
                                         * @li  <b>11</b> - Use previous
                                         */
  unsigned char   StressResetMode;               /**< StressResetMode 01-10
                                         * @li  <b>00</b> - Disabed
                                         * @li  <b>01</b> - Io Write 0x64 with 0xfe
                                         * @li  <b>10</b> - Io Write 0xcf9 with 0x06
                                         * @li  <b>11</b> - Io Write 0xcf9 with 0x0e
                                         */
  EC_struct Pecstruct;  // 01-11 (Internal USED)
  HWM_struct hwm;       // 01-12
  IMC_struct imc;       // 01-13
  unsigned char    Cg2Pll;      // 01-14
  unsigned char    ClockRun;      // 01-15
  unsigned char   Debug_Reg00;                 // 01-16
  unsigned char   Debug_Reg01;                 // 01-17
  unsigned char   Debug_Reg02;                 // 01-18
  unsigned char   Debug_Reg03;                 // 01-19
  unsigned char   Debug_Reg04;                 // 01-20
  unsigned char   Debug_Reg05;                 // 01-21
  unsigned char   EhciDataCacheDis;            // 01-22
  unsigned char   EfuseByte [0x20];
  unsigned char   EfuseRemainder ;
  unsigned char   EfuseSum ;
  unsigned char   L1ImmediateAck;
} AMDSBCFG;

/** SMMSERVICESTRUC- Southbridge SMI service structure */
typedef  struct   _SMMSERVICESTRUC {
/** enableRegNum - Reserved */
  unsigned char enableRegNum;
/** enableBit - Reserved */
  unsigned char enableBit;
/** statusRegNum - Reserved */
  unsigned char statusRegNum;
/** statusBit - Reserved */
  unsigned char statusBit;
/** *debugMessage- Reserved */
  signed char *debugMessage;
/** serviceRoutine - Reserved */
  SMM_SERVICE_ROUTINE  serviceRoutine;
} SMMSERVICESTRUC;

#ifndef _NB_REG8MASK_

/**
 *  - Byte Register R/W structure
 *
 */
  typedef struct _Reg8Mask {
/** bRegIndex - Reserved */
    unsigned char bRegIndex;
/** bANDMask - Reserved */
    unsigned char bANDMask;
/** bORMask - Reserved */
    unsigned char bORMask;
  } REG8MASK;
#endif

/**
 *  - SATA Phy setting structure
 *
 */
typedef struct _SATAPHYSETTING {
/** wPhyCoreControl - Reserved */
  unsigned short wPhyCoreControl;
/** dwPhyFineTune - Reserved */
  unsigned int dwPhyFineTune;
} SATAPHYSETTING;

/**
 * _ABTblEntry - AB link register table R/W structure
 *
 */
typedef struct _ABTblEntry {
  /** regType  : AB Register Type (ABCFG, AXCFG and so on) */
  unsigned char    regType;
  /** regIndex : AB Register Index */
  unsigned int   regIndex;
  /** regMask  : AB Register Mask */
  unsigned int   regMask;
  /** regData  : AB Register Data */
  unsigned int   regData;
} ABTBLENTRY;

/**
 * _AcpiRegWrite - ACPI MMIO register R/W structure
 *
 */
typedef struct _AcpiRegWrite {
  /** MmioBase     : Index of Southbridge block (For instance GPIO_BASE:0x01 SMI_BASE:0x02) */
  unsigned char MmioBase;
  /** MmioReg      : Register index */
  unsigned char MmioReg;
  /** DataANDMask  : AND Register Data */
  unsigned char DataANDMask;
  /** DataOrMask   : Or Register Data */
  unsigned char DataOrMask;
} AcpiRegWrite;

/**
 * PCI_ADDRESS - PCI access structure
 *
 */
#define PCI_ADDRESS(bus, dev, func, reg) \
(unsigned int) ( (((unsigned int)bus) << 24) + (((unsigned int)dev) << 19) + (((unsigned int)func) << 16) + ((unsigned int)reg) )

/**
 * CIM_STATUS - CIMx module function return code
 */
typedef unsigned int    CIM_STATUS;
/**
 * CIM_SUCCESS - Executed without error
 */
#define CIM_SUCCESS 0x00000000
/**
 * CIM_ERROR - call error
 */
#define CIM_ERROR 0x80000000
/**
 * CIM_UNSUPPORTED - function does not support
 */
#define CIM_UNSUPPORTED 0x80000001

#ifndef  LEGACY_BIOS_SUPPORT
  #pragma pack (pop)
#endif

/**
 * DISABLED - Define disable in module
 */
#define DISABLED   0
/**
 * ENABLED - Define enable in module
 */
#define ENABLED    1

/// PCIE Reset Block
typedef enum {
  NbBlock,                        ///< Reset for NB PCIE
  SbBlock                         ///< Reset for FCH GPP
} RESET_BLOCK;

/// PCIE Reset Operation
typedef enum {
  DeassertReset,                  ///< Deassert reset
  AssertReset                     ///< Assert reset
} RESET_OP;

/**
 * SB_GPIO_INIT_ENTRY
 *
 */
typedef struct _SB_GPIO_INIT_ENTRY {
  unsigned char GpioPin;                  ///< Gpio Pin
  unsigned char GpioMux;                  ///< Gpio Mux
  unsigned char GpioOutEnB;               ///< Gpio Out EnB
  unsigned char GpioOut;                  ///< Gpio Out
} SB_GPIO_INIT_ENTRY;

/**
 * SB_GPIO_CONTROL_ENTRY
 *
 */
typedef struct _SB_GPIO_CONTROL_ENTRY {
  unsigned char GpioPin;                  ///< Gpio Pin
  unsigned char GpioControl;              ///< Gpio Control
} SB_GPIO_CONTROL_ENTRY;

/**
 * SB_SPREAD_SPECTRUM_ENTRY
 *
 */
typedef struct _SB_SPREAD_SPECTRUM_ENTRY {
  unsigned int P_40_25;                  ///< P_10_25_24
  unsigned int P_18_15_5;              ///< P_10_25_24
  unsigned int P_18_19_16;              ///< P_10_25_24
  unsigned int P_10_23_8;              ///< P_10_25_24
  unsigned int P_10_7_0;              ///< P_10_25_24
  unsigned int P_1C_5_0;              ///< P_10_25_24
  unsigned int P_08_31_28;              ///< P_10_25_24
  unsigned int P_08_7;              ///< P_10_25_24
  unsigned int P_08_8;              ///< P_10_25_24
  unsigned int P_10_25_24;              ///< P_10_25_24
} SB_SPREAD_SPECTRUM_ENTRY;
#endif // _AMD_SBTYPE_H_
