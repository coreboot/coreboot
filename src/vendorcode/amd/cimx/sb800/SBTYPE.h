
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

#ifndef _AMD_SBTYPE_H_
#define _AMD_SBTYPE_H_

#pragma pack (push, 1)

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
  unsigned int   BiosSize:3;           /**< BiosSize
                                   *   @par
                                   *   BIOSSize [2.0] - BIOS Image Size
                                   *    @li <b>0</b> - 1M
                                   *    @li <b>1</b> - 2M
                                   *    @li <b>3</b> - 4M
                                   *    @li <b>7</b> - 8M
                                   *  In SB800, default ROM size is 1M Bytes, if your platform ROM bigger then 1M
                                   *  you have to set the ROM size outside CIMx module and before AGESA module get call
                                   *
                                   */
  unsigned int   LegacyFree:1;         /**< LegacyFree
                                   *   @par
                                   * Config Southbridge CIMx module for Legacy Free Mode
                                   */
  unsigned int   SpiSpeed:2;           /**< SpiSpeed
                                   *   @par
                                   *    SPI Speed [1.0] - the clock speed for non-fast read command
                                   *    @li <b>00</b> - 66Mhz
                                   *    @li <b>01</b> - 33Mhz
                                   *    @li <b>10</b> - 22Mhz
                                   *    @li <b>11</b> - 16.5Mhz
                                   *
                                   */
  unsigned int   ImcEnableOverWrite:2; /**< ImcEnableOverWrite
                                   *   @par
                                   *    Imc Enable OverWrite
                                   *    @li <b>00</b> - by default strapping
                                   *    @li <b>01</b> - On
                                   *    @li <b>10</b> - Off
                                   *
                                   */
  unsigned int   SpiFastReadEnable:1;  /**< SpiFastReadEnable
                                   *   @par
                                   *    @li <b>00</b> - Disable SPI Fast Read Function
                                   *    @li <b>01</b> - Enable SPI Fast Read Function
                                   */
  unsigned int   SpiFastReadSpeed:2;   /**< SpiFastReadSpeed
                                   *   @par
                                   *    @li <b>00</b> - 66Mhz
                                   *    @li <b>01</b> - 33Mhz
                                   *    @li <b>10</b> - 22Mhz
                                   *    @li <b>11</b> - 16.5Mhz
                                   */
  unsigned int   SpreadSpectrumType:1;   /**< SpreadSpectrumType
                                   *   @par
                                   *    @li <b>0</b> - Spread Spectrum for normal platform
                                   *    @li <b>1</b> - Spread Spectrum for Ontario platform
                                   */
/** Dummy0 - Reserved */
  unsigned int   Dummy0:4;
  unsigned int   EcKbd:1;              /**< EcKbd
                                   *   @par
                                   *   EcKbd [16] - Platform use EC (as SIO) or SIO chip for PS/2 Keyboard and Mouse
                                   *    @li <b>0</b> - Use SIO PS/2 function.
                                   *    @li <b>1</b> - Use EC PS/2 function instead of SIO PS/2 function. **
                                   *    @li <b>**</b> When set 1, EC function have to enable, otherwise, CIMx treat as legacy-free system.
                                   */
/** EcChannel0 - Reserved */
  unsigned int   EcChannel0:1;
/** UsbMsi - Reserved */
  unsigned int   UsbMsi:1;
/** HdAudioMsi - Reserved */
  unsigned int   HdAudioMsi:1;
/** LpcMsi - Reserved */
  unsigned int   LpcMsi:1;
/** PcibMsi - Reserved */
  unsigned int   PcibMsi:1;
/** AbMsi - Reserved */
  unsigned int   AbMsi:1;
/** Dummy1 - Reserved */
  unsigned int   Dummy1:9;

  unsigned int   Smbus0BaseAddress;    /**< Smbus0BaseAddress
                                   *   @par
                                   * Smbus BASE Address
                                   */
  unsigned int   Smbus1BaseAddress;    /**< Smbus1BaseAddress
                                   *   @par
                                   * Smbus1 (ASF) BASE Address
                                   */
  unsigned int   SioPmeBaseAddress;    /**< SioPmeBaseAddress
                                   *   @par
                                   * SIO PME BASE Address
                                   */
  unsigned int   SioHwmBaseAddress; /**< SioHwmBaseAddress
                                      *   @par
                                      * SIO HWM BASE Address
                                      */
  unsigned int   WatchDogTimerBase;    /**< WatchDogTimerBase
                                   *   @par
                                   *  Watch Dog Timer Address
                                   */
  unsigned int   GecShadowRomBase;     /**< GecShadowRomBase
                                   *   @par
                                   * GEC (NIC) SHADOWROM BASE Address
                                   */
  unsigned int   SpiRomBaseAddress;    /**< SpiRomBaseAddress
                                   *   @par
                                   * SPI ROM BASE Address
                                   */
  unsigned short   AcpiPm1EvtBlkAddr;    /**< AcpiPm1EvtBlkAddr
                                   *   @par
                                   * ACPI PM1 event block Address
                                   */
  unsigned short   AcpiPm1CntBlkAddr;    /**< AcpiPm1CntBlkAddr
                                   *   @par
                                   * ACPI PM1 Control block Address
                                   */
  unsigned short   AcpiPmTmrBlkAddr;     /**< AcpiPmTmrBlkAddr
                                   *   @par
                                   * ACPI PM timer block Address
                                   */
  unsigned short   CpuControlBlkAddr;    /**< CpuControlBlkAddr
                                   *   @par
                                   * ACPI CPU control block Address
                                   */
  unsigned short   AcpiGpe0BlkAddr;      /**< AcpiGpe0BlkAddr
                                   *   @par
                                   * ACPI GPE0 block Address
                                   */
  unsigned short   SmiCmdPortAddr;       /**< SmiCmdPortAddr
                                   *   @par
                                   * SMI command port Address
                                   */
  unsigned short   AcpiPmaCntBlkAddr;    /**< AcpiPmaCntBlkAddr
                                   *   @par
                                   * ACPI PMA Control block Address
                                   */
  unsigned int   HpetBase;             /**< HpetBase
                                   *   @par
                                   * HPET Base address
                                   */
  unsigned int   SataIDESsid;          /**< SataIDESsid
                                   *   @par
                                   * SATA IDE mode SSID
                                   */
  unsigned int   SataRAIDSsid;         /**< SataRAIDSsid
                                   *   @par
                                   *  SATA RAID mode SSID
                                   */
  unsigned int   SataRAID5Ssid;        /**< SataRAID5Ssid
                                   *   @par
                                   * SATA RAID5 mode SSID
                                   */
  unsigned int   SataAHCISsid;         /**< SataAHCISsid
                                   *   @par
                                   * SATA AHCI mode SSID
                                   */
  unsigned int   OhciSsid;             /**< OhciSsid
                                   *   @par
                                   * OHCI Controller SSID
                                   */
  unsigned int   EhciSsid;             /**< EhciSsid
                                   *   @par
                                   * EHCI Controller SSID
                                   */
  unsigned int   Ohci4Ssid;            /**< Ohci4Ssid
                                   *   @par
                                   * OHCI4 Controller SSID (Force USB 1.1 mode)
                                   */
  unsigned int   SmbusSsid;            /**< SmbusSsid
                                   *   @par
                                   *  SMBUS controller SSID
                                   */
  unsigned int   IdeSsid;              /**< IdeSsid
                                   *   @par
                                   * IDE (Sata) controller SSID
                                   */
  unsigned int   AzaliaSsid;           /**< AzaliaSsid
                                   *   @par
                                   * HD Audio controller SSID
                                   */
  unsigned int   LpcSsid;              /**< LpcSsid
                                   *   @par
                                   * LPC controller SSID
                                   */
  unsigned int   PCIBSsid;             /**< PCIBSsid
                                   *   @par
                                   * PCIB controller SSID
                                   */
} BUILDPARAM;

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
  unsigned short IMCFUNSupportBitMap;	 /// Bit0=81FunZone0 support(1=On;0=Off); bit1-3=81FunZone1-Zone3;Bit4-7=83FunZone0-Zone3;Bit8-11=85FunZone0-Zone3;Bit11-15=89FunZone0-Zone3;
} EC_struct;
/** SBGPPPORTCONFIG - Southbridge GPP port config structure */
typedef struct {
  unsigned int   PortPresent:1;     /**< Port connection
                                   *   @par
                                   *    @li <b>0</b> - Port doesn't have slot. No need to train the link
                                   *    @li <b>1</b> - Port connection defined and needs to be trained
                                   */
  unsigned int   PortDetected:1;    /**< Link training status
                                   *   @par
                                   *    @li <b>0</b> - EP not detected
                                   *    @li <b>1</b> - EP detected
                                   */
  unsigned int   PortIsGen2:2;      /**< Port link speed configuration
                                   *   @par
                                   *    @li <b>00</b> - Auto
                                   *    @li <b>01</b> - Forced GEN1
                                   *    @li <b>10</b> - Forced GEN2
                                   *    @li <b>11</b> - Reserved
                                   */

  unsigned int   PortHotPlug:1;     /**< Support hot plug?
                                   *   @par
                                   *    @li <b>0</b> - No support
                                   *    @li <b>1</b> - support
                                   */
/** PortMisc - Reserved */
  unsigned int   PortMisc:27;
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
  unsigned char   SataController:1;            /**< SataController
                                        *   @par
                                        * Sata Controller
                                        *    @li <b>0</b> - disable
                                        *    @li <b>1</b> - enable
                                        */
  unsigned char   SataIdeCombMdPriSecOpt:1;    /**< SataIdeCombMdPriSecOpt - Reserved */
  unsigned char   SataSetMaxGen2:1;            /**< SataSetMaxGen2
                                        *   @par
                                        * Sata Controller Set to Max Gen2 mode
                                        *    @li <b>0</b> - disable
                                        *    @li <b>1</b> - enable
                                        */
  unsigned char   SataIdeCombinedMode:1;       /**< SataIdeCombinedMode
                                        *   @par
                                        * Sata IDE Controller set to Combined Mode
                                        *    @li <b>0</b> - enable
                                        *    @li <b>1</b> - disable
                                        */
/** SATARefClkSel - Reserved */
  unsigned char   SATARefClkSel:2;              // 4:5
/** SATARefDivSel - Reserved */
  unsigned char   SATARefDivSel:2;              // 6:7
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
  unsigned char  Ohci1:1;                      ///< Ohci0 controller - 0:disable, 1:enable
  unsigned char  Ehci1:1;                      ///< Ehci1 controller - 0:disable, 1:enable
  unsigned char  Ohci2:1;                      ///< Ohci2 controller - 0:disable, 1:enable
  unsigned char  Ehci2:1;                      ///< Ehci2 controller - 0:disable, 1:enable
  unsigned char  Ohci3:1;                      ///< Ohci3 controller - 0:disable, 1:enable
  unsigned char  Ehci3:1;                      ///< Ehci3 controller - 0:disable, 1:enable
  unsigned char  Ohci4:1;                      ///< Ohci4 controller - 0:disable, 1:enable
  unsigned char  UTemp:1;                      ///< Reserved
} USBST;

/**
 * _AZALIAPIN - HID Azalia or GPIO define structure.
 *
 */
typedef struct _AZALIAPIN {
  unsigned char   AzaliaSdin0:2;               /**< AzaliaSdin0
                                        *   @par
                                        * SDIN0 is defined at BIT0 & BIT1
                                        *    @li <b>00</b> - GPIO PIN
                                        *    @li <b>10</b> - As a Azalia SDIN pin
                                        */
  unsigned char   AzaliaSdin1:2;               /**< AzaliaSdin1
                                        *   @par
                                        * SDIN0 is defined at BIT2 & BIT3
                                        *    @li <b>00</b> - GPIO PIN
                                        *    @li <b>10</b> - As a Azalia SDIN pin
                                        */
  unsigned char   AzaliaSdin2:2;               /**< AzaliaSdin2
                                        *   @par
                                        * SDIN0 is defined at BIT4 & BIT5
                                        *    @li <b>00</b> - GPIO PIN
                                        *    @li <b>10</b> - As a Azalia SDIN pin
                                        */
  unsigned char   AzaliaSdin3:2;               /**< AzaliaSdin3
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

/** BuildParameters - The STATIC platform information for CIMx Module. */
  BUILDPARAM BuildParameters;
                                      //offset 90 bytes (32-121)
  //MsgXchgBiosCimx                   //offset 4 bytes (122-125)
  // SATA Configuration

  union /**< union - Reserved */
  {  /**< SATAMODE - Sata Controller structure */
/** SataModeReg - Reserved */
    unsigned char   SataModeReg;
/** SataMode - Reserved */
    SATAST  SataMode;
  } SATAMODE;
/** S3Resume - Flag of ACPI S3 Resume. */
  unsigned char   S3Resume:1;                   // 8
/** RebootRequired - Flag of Reboot system is required. */
  unsigned char   RebootRequired:1;             // 9
/** SbSpiSpeedSupport - Reserved */
  unsigned char   SbSpiSpeedSupport:1;   // 10
/**< SpreadSpectrum
 *   @par
 *   Spread Spectrum function
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  unsigned char   SpreadSpectrum:1;             // 11
/** NbSbGen2 - Reserved */
  unsigned char   NbSbGen2:1;                   // 12
  unsigned char   GppGen2:1;                    // 13
  unsigned char   GppMemWrImprove:1;            // 14
/** MsgXchgBiosCimxReserved - Reserved */
  unsigned char   MsgXchgBiosCimxReserved:1;    // 15  (BB USED)
/**< SataClass - SATA Controller mode [16:18]
 *   @par
 *    @li <b>000</b> - Native IDE mode
 *    @li <b>001</b> - RAID mode
 *    @li <b>010</b> - AHCI mode
 *    @li <b>011</b> - Legacy IDE mode
 *    @li <b>100</b> - IDE->AHCI mode
 *    @li <b>101</b> - AHCI mode as 4394 ID (AMD driver)
 *    @li <b>110</b> - IDE->AHCI mode as 4394 ID (AMD driver)
 */
  unsigned short   SataClass:3;                 // 16:18
/**< Sata IDE Controller mode
 *   @par
 *    @li <b>0</b> - Legacy IDE mode
 *    @li <b>1</b> - Native IDE mode
 */
  unsigned short   SataIdeMode:1;               // 19
/**<  SataEspPort - SATA port is external accessible on a signal only connector (eSATA:)
 *   @par
 *    @li <b> BIT0 </b> - PORT0 set as ESP port
 *    @li <b> BIT1 </b> - PORT1 set as ESP port
 *    @li <b> BIT2 </b> - PORT2 set as ESP port
 *    @li <b> BIT3 </b> - PORT3 set as ESP port
 *    @li <b> BIT4 </b> - PORT4 set as ESP port
 *    @li <b> BIT5 </b> - PORT5 set as ESP port
 */
  unsigned short   SataEspPort:6;               // 20:25
/** SataPortPower - Reserved */
  unsigned short   SataPortPower:6;             // 31:26

  // SATA Debug Option                //offset 4 bytes (126-129)

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
 */
  unsigned int   SataPortMode:12;             //11:0
/** SATAClkSelOpt - Reserved */
  unsigned int   SATAClkSelOpt:4;             // Removed from coding side
/** SataAggrLinkPmCap - Reserved */
  unsigned int   SataAggrLinkPmCap:1;          //16, 0:OFF   1:ON
/** SataPortMultCap - Reserved */
  unsigned int   SataPortMultCap:1;            //17, 0:OFF   1:ON
/** SataClkAutoOff - Reserved */
  unsigned int   SataClkAutoOff:1;             //18, AutoClockOff 0:Disabled, 1:Enabled
/** SataPscCap - Reserved */
  unsigned int   SataPscCap:1;                 //19, 0:Enable PSC capability, 1:Disable PSC capability
/** BIOSOSHandoff - Reserved */
  unsigned int   BIOSOSHandoff:1;              //20
/** SataFisBasedSwitching - Reserved */
  unsigned int   SataFisBasedSwitching:1;      //21
/** SataCccSupport - Reserved */
  unsigned int   SataCccSupport:1;             //22
/** SataSscCap - Reserved */
  unsigned int   SataSscCap:1;                 //23, 0:Enable SSC capability, 1:Disable SSC capability
/** SataMsiCapability - Reserved */
  unsigned int   SataMsiCapability:1;          //24  0:Hidden 1:Visible. This feature is disabled per RPR, but remains the interface.
/** SataForceRaid - Reserved */
  unsigned int   SataForceRaid:1;              //25  0:No function 1:Force RAID
/** SataDebugDummy - Reserved */
  unsigned int   SataDebugDummy:6;             //31:26
//
// USB Configuration                //offset 4 bytes (130-133)
//

/** USBDeviceConfig - USB Controller Configuration
 *
 * - Usb Ohci1 Contoller is defined at BIT0
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 18 Func0) *
 * - Usb Ehci1 Contoller is defined at BIT1
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 18 Func2) *
 * - Usb Ohci2 Contoller is defined at BIT2
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 19 Func0) *
 * - Usb Ehci2 Contoller is defined at BIT3
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 19 Func2) *
 * - Usb Ohci3 Contoller is defined at BIT4
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 22 Func0) *
 * - Usb Ehci3 Contoller is defined at BIT5
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 22 Func2) *
 * - Usb Ohci4 Contoller is defined at BIT6
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 20 Func5) *
 */
  union /**< union - Reserved */
  {  /**< USBMODE - USB Controller structure */
/** SataModeReg - Reserved */
    unsigned char   UsbModeReg;
/** SataMode - Reserved */
    USBST  UsbMode;
  } USBMODE;
/*!
 */

/**< GecConfig
 *   @par
 *   InChip Gbit NIC
 *    @li <b>1</b> - disable
 *    @li <b>0</b> - enable
 */
  unsigned char   GecConfig:1;                 //8

/**< IrConfig
 *   @par
 *   Ir Controller setting
 *    @li <b>00 </b> - disable
 *    @li <b>01 </b> - Rx and Tx0
 *    @li <b>10 </b> - Rx and Tx1
 *    @li <b>11 </b> - Rx and both Tx0,Tx1
 */
  unsigned char   IrConfig:2;                  //9:10

/** GecDummy - Reserved */
  unsigned char   GecDummy:5;                  //15:11

  //Azalia Configuration

/**< AzaliaController - Azalia Controller Configuration
 *   @par
 *   Azalia Controller [0-1]
 *    @li <b>0</b> - Auto   : Detect Azalia controller automatically.
 *    @li <b>1</b> - Diable : Disable Azalia controller.
 *    @li <b>2</b> - Enable : Enable Azalia controller.
 */
  unsigned char   AzaliaController:2;          //17:16
/**< AzaliaPinCfg - Azalia Controller SDIN pin Configuration
 *   @par
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  unsigned char   AzaliaPinCfg:1;              //18
/**< AzaliaFrontPanel - Azalia Controller Front Panel Configuration
 *   @par
 *   Support Front Panel configuration
 *    @li <b>0</b> - Auto
 *    @li <b>1</b> - disable
 *    @li <b>2</b> - enable
 */
  unsigned char   AzaliaFrontPanel:2;          //20:19
/**< FrontPanelDetected - Force Azalia Controller Front Panel Configuration
 *   @par
 *   Force Front Panel configuration
 *    @li <b>0</b> - Not Detected
 *    @li <b>1</b> - Detected
 */
  unsigned char   FrontPanelDetected:1;        //21
/**< AzaliaSnoop - Azalia Controller Snoop feature Configuration
 *   @par
 *   Azalia Controller Snoop feature Configuration
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  unsigned char   AzaliaSnoop:1;               //22
/** AzaliaDummy - Reserved */
  unsigned char   AzaliaDummy:1;               //23

  union
  {
/**< AzaliaSdinPin - Azalia Controller SDIN pin Configuration
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

/** AZOEMTBL - Azalia Controller OEM Codec Table Pointer
 *
 */
  union
  {
    PLACEHOLDER  PlaceHolder;
    CODECTBLLIST*  pAzaliaOemCodecTablePtr;  //offset 4 bytes (134-137)
  } AZOEMTBL;

/** AZOEMFPTBL - Azalia Controller Front Panel OEM Table Pointer
 *
 */
  union
  {
    PLACEHOLDER  PlaceHolder;
    void*   pAzaliaOemFpCodecTablePtr;       //offset 4 bytes (138-141)
  } AZOEMFPTBL;

  //Miscellaneous Configuration      //offset 4 bytes (142-145)
/** AnyHT200MhzLink - Reserved */
  unsigned int   AnyHT200MhzLink:1;             //0
/**< HpetTimer - South Bridge Hpet Timer Configuration
 *   @par
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  unsigned int   HpetTimer:1;                 //1
/**< PciClks - PCI Slot Clock Control
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
  unsigned int   PciClks:5;                   //2:6
/** MiscReserved1 - Reserved */
  unsigned int   MiscReserved1:4;             //9:7, Reserved
/** MobilePowerSavings - Debug function Reserved */
  unsigned int   MobilePowerSavings:1;        //11, 0:Disable, 1:Enable Power saving features especially for Mobile platform
/** MiscDummy1 - Debug function Reserved */
  unsigned int   MiscDummy1:1;
/** NativePcieSupport - Debug function Reserved */
  unsigned int   NativePcieSupport:1;         //13, 0:Enable, 1:Disabled
/** FlashPinConfig - Debug function Reserved */
  unsigned int   FlashPinConfig:1;            //14, 0:desktop mode 1:mobile mode
/** UsbPhyPowerDown - Debug function Reserved */
  unsigned int   UsbPhyPowerDown:1;           //15
/** PcibClkStopOverride - Debug function Reserved */
  unsigned int   PcibClkStopOverride:10;      //25:16
/**< HpetMsiDis - South Bridge HPET MSI Configuration
 *   @par
 *    @li <b>1</b> - disable
 *    @li <b>0</b> - enable
 */
  unsigned int   HpetMsiDis:1;                //26
/**< ResetCpuOnSyncFlood - Rest CPU on Sync Flood
 *   @par
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  unsigned int   ResetCpuOnSyncFlood:1;       //27
/**< LdtStpDisable - LdtStp# output disable
 *   @par
 *    @li <b>0</b> - LdtStp# output enable
 *    @li <b>1</b> - LdtStp# output disable
 */
  unsigned int   LdtStpDisable:1;             //28
/**< MTC1e - Message Triggered C1e
 *   @par
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  unsigned int   MTC1e:1;                     //29
/** MiscDummy - Reserved */
  unsigned int   MiscDummy:2;                 //31:30
  unsigned int   SioHwmPortEnable:1;          // Enable SuperIO HWM access via LPC

  //DebugOptions                     //offset 4 bytes (146-149)
/** PcibAutoClkCtrlLow - Debug function Reserved */
  unsigned int   PcibAutoClkCtrlLow:16;
/** PcibAutoClkCtrlHigh - Debug function Reserved */
  unsigned int   PcibAutoClkCtrlHigh:16;

/**< OEMPROGTBL - ACPI MMIO register setting table OEM override
 *   @par
 *   OEM table for customer override ACPI MMIO register in their code.
 */
  union
  {
    PLACEHOLDER OemProgrammingTablePtr;  //offset 4 bytes (150-153)
    void   *OemProgrammingTablePtr_Ptr;
  } OEMPROGTBL;

  //Gpp Configuration                   //offset 24 bytes total (154-177)
  union {
    unsigned int           PORTCFG32;
    SBGPPPORTCONFIG  PortCfg;
  } PORTCONFIG[MAX_GPP_PORTS];          //offset 16 bytes

  unsigned int           GppLinkConfig;       // GPP_LINK_CONFIG = PCIE_GPP_Enable[3:0]
                                        //   0000 - Port ABCD -> 4:0:0:0
                                        //   0001 - N/A
                                        //   0010 - Port ABCD -> 2:2:0:0
                                        //   0011 - Port ABCD -> 2:1:1:0
                                        //   0100 - Port ABCD -> 1:1:1:1
                                        //
  unsigned int   GppFoundGfxDev:4;            //3:0 If port A-D (mapped to bit [3:0]) has GFX EP detected
  unsigned int   CoreGen2Enable:1;            //4
  unsigned int   GppFunctionEnable:1;         //5
  unsigned int   GppUnhidePorts:1;            //6
  unsigned int   AlinkPhyPllPowerDown:1;      //7
  unsigned int   GppConfigDummy1:2;           //9:8
  unsigned int   GppLaneReversal:1;           //10
  unsigned int   GppPhyPllPowerDown:1;        //11
  unsigned int   GppCompliance :1;            //12
  unsigned int   GppPortAspm:8;               //20:13 ASPM state for GPP ports, 14:13 for port0, ..., 20:19 for port3
                                        //  00 - Disabled
                                        //  01 - L0s
                                        //  10 - L1
                                        //  11 - L0s + L1
                                        //
  unsigned int   GppConfigDummy:11;           //31:21

  //TempMMIO                   //offset 4 bytes (178-181)
  unsigned int   TempMMIO;

  // DebugOption2
  unsigned int   GecPhyStatus:1;
  unsigned int   GecDebugOptionDummy:7;
  unsigned int   SBGecPwr:2;
  unsigned int   SBGecDebugBus:1;
  unsigned int   DebugOption2Dummy1:1;
  unsigned int   DebugOption2Dummy2:1;
  unsigned int   SbPcieOrderRule:1;
  unsigned int   SbUsbPll:1;
  unsigned int   AcDcMsg:1;
  unsigned int   TimerTickTrack:1;
  unsigned int   ClockInterruptTag:1;
  unsigned int   OhciTrafficHanding:1;
  unsigned int   EhciTrafficHanding:1;
  unsigned int   FusionMsgCMultiCore:1;
  unsigned int   FusionMsgCStage:1;
/**< UsbRxMode - CG PLL multiplier for USB Rx 1.1 mode
 *   @par
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  unsigned int   UsbRxMode:1;
  unsigned int   DebugOption2Dummy3:9;        //

  union
  {
    PLACEHOLDER DynamicGecRomAddressPtr;  //offset 4 bytes (182-185)
    void   *DynamicGecRomAddress_Ptr;
  } DYNAMICGECROM;
  EC_struct Pecstruct;
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
  /** MmioBase     : Index of Soubridge block (For instence GPIO_BASE:0x01 SMI_BASE:0x02) */
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

#pragma pack (pop)

/**
 * CIMX_OPTION_DISABLED - Define disable in module
 */
#define CIMX_OPTION_DISABLED   0
/**
 * CIMX_OPTION_ENABLED - Define enable in module
 */
#define CIMX_OPTION_ENABLED    1

/**
 * SATA_IDE_COMBINE_ENABLE  -Define  Enable Combined Mode
 */
#define SATA_IDE_COMBINE_ENABLE   0

/**
 * SATA_IDE_COMBINE_DISABLE  -Define  Disable Combined Mode
 */
#define SATA_IDE_COMBINE_DISABLE  1

// mov al, code
// out 80h, al
// jmp $

/**
 * DBG_STOP - define a debug point
 */
#define DBG_STOP __asm _emit 0xEB __asm _emit 0xFE

/**
 * STOP_CODE - define a debug point
 *    Warning: AL gets destroyed!
 */
#define STOP_CODE (code) __asm __emit 0xB0 __asm __emit code __asm __emit 0xE6 \
        __asm __emit 0x80 __asm _emit 0xEB __asm _emit 0xFE

#endif // _AMD_SBTYPE_H_
