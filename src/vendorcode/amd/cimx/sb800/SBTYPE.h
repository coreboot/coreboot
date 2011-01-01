
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
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
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
typedef UINT32 (*SBCIM_HOOK_ENTRY) (UINT32 Param1, UINT32 Param2, void* pConfig);
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
  UINT32   BiosSize:3;           /**< BiosSize
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
  UINT32   LegacyFree:1;         /**< LegacyFree
                                   *   @par
                                   * Config Southbridge CIMx module for Legacy Free Mode
                                   */
  UINT32   SpiSpeed:2;           /**< SpiSpeed
                                   *   @par
                                   *    SPI Speed [1.0] - the clock speed for non-fast read command
                                   *    @li <b>00</b> - 66Mhz
                                   *    @li <b>01</b> - 33Mhz
                                   *    @li <b>10</b> - 22Mhz
                                   *    @li <b>11</b> - 16.5Mhz
                                   *
                                   */
  UINT32   ImcEnableOverWrite:2; /**< ImcEnableOverWrite
                                   *   @par
                                   *    Imc Enable OverWrite
                                   *    @li <b>00</b> - by default strapping
                                   *    @li <b>01</b> - On
                                   *    @li <b>10</b> - Off
                                   *
                                   */
  UINT32   SpiFastReadEnable:1;  /**< SpiFastReadEnable
                                   *   @par
                                   *    @li <b>00</b> - Disable SPI Fast Read Function
                                   *    @li <b>01</b> - Enable SPI Fast Read Function
                                   */
  UINT32   SpiFastReadSpeed:2;   /**< SpiFastReadSpeed
                                   *   @par
                                   *    @li <b>00</b> - 66Mhz
                                   *    @li <b>01</b> - 33Mhz
                                   *    @li <b>10</b> - 22Mhz
                                   *    @li <b>11</b> - 16.5Mhz
                                   */
  UINT32   SpreadSpectrumType:1;   /**< SpreadSpectrumType
                                   *   @par
                                   *    @li <b>0</b> - Spread Spectrum for normal platform
                                   *    @li <b>1</b> - Spread Spectrum for Ontario platform
                                   */
/** Dummy0 - Reserved */
  UINT32   Dummy0:4;
  UINT32   EcKbd:1;              /**< EcKbd
                                   *   @par
                                   *   EcKbd [16] - Platform use EC (as SIO) or SIO chip for PS/2 Keyboard and Mouse
                                   *    @li <b>0</b> - Use SIO PS/2 function.
                                   *    @li <b>1</b> - Use EC PS/2 function instead of SIO PS/2 function. **
                                   *    @li <b>**</b> When set 1, EC function have to enable, otherwise, CIMx treat as legacy-free system.
                                   */
/** EcChannel0 - Reserved */
  UINT32   EcChannel0:1;
/** UsbMsi - Reserved */
  UINT32   UsbMsi:1;
/** HdAudioMsi - Reserved */
  UINT32   HdAudioMsi:1;
/** LpcMsi - Reserved */
  UINT32   LpcMsi:1;
/** PcibMsi - Reserved */
  UINT32   PcibMsi:1;
/** AbMsi - Reserved */
  UINT32   AbMsi:1;
/** Dummy1 - Reserved */
  UINT32   Dummy1:9;

  UINT32   Smbus0BaseAddress;    /**< Smbus0BaseAddress
                                   *   @par
                                   * Smbus BASE Address
                                   */
  UINT32   Smbus1BaseAddress;    /**< Smbus1BaseAddress
                                   *   @par
                                   * Smbus1 (ASF) BASE Address
                                   */
  UINT32   SioPmeBaseAddress;    /**< SioPmeBaseAddress
                                   *   @par
                                   * SIO PME BASE Address
                                   */
  UINT32   WatchDogTimerBase;    /**< WatchDogTimerBase
                                   *   @par
                                   *  Watch Dog Timer Address
                                   */
  UINT32   GecShadowRomBase;     /**< GecShadowRomBase
                                   *   @par
                                   * GEC (NIC) SHADOWROM BASE Address
                                   */
  UINT32   SpiRomBaseAddress;    /**< SpiRomBaseAddress
                                   *   @par
                                   * SPI ROM BASE Address
                                   */
  UINT16   AcpiPm1EvtBlkAddr;    /**< AcpiPm1EvtBlkAddr
                                   *   @par
                                   * ACPI PM1 event block Address
                                   */
  UINT16   AcpiPm1CntBlkAddr;    /**< AcpiPm1CntBlkAddr
                                   *   @par
                                   * ACPI PM1 Control block Address
                                   */
  UINT16   AcpiPmTmrBlkAddr;     /**< AcpiPmTmrBlkAddr
                                   *   @par
                                   * ACPI PM timer block Address
                                   */
  UINT16   CpuControlBlkAddr;    /**< CpuControlBlkAddr
                                   *   @par
                                   * ACPI CPU control block Address
                                   */
  UINT16   AcpiGpe0BlkAddr;      /**< AcpiGpe0BlkAddr
                                   *   @par
                                   * ACPI GPE0 block Address
                                   */
  UINT16   SmiCmdPortAddr;       /**< SmiCmdPortAddr
                                   *   @par
                                   * SMI command port Address
                                   */
  UINT16   AcpiPmaCntBlkAddr;    /**< AcpiPmaCntBlkAddr
                                   *   @par
                                   * ACPI PMA Control block Address
                                   */
  UINT32   HpetBase;             /**< HpetBase
                                   *   @par
                                   * HPET Base address
                                   */
  UINT32   SataIDESsid;          /**< SataIDESsid
                                   *   @par
                                   * SATA IDE mode SSID
                                   */
  UINT32   SataRAIDSsid;         /**< SataRAIDSsid
                                   *   @par
                                   *  SATA RAID mode SSID
                                   */
  UINT32   SataRAID5Ssid;        /**< SataRAID5Ssid
                                   *   @par
                                   * SATA RAID5 mode SSID
                                   */
  UINT32   SataAHCISsid;         /**< SataAHCISsid
                                   *   @par
                                   * SATA AHCI mode SSID
                                   */
  UINT32   OhciSsid;             /**< OhciSsid
                                   *   @par
                                   * OHCI Controller SSID
                                   */
  UINT32   EhciSsid;             /**< EhciSsid
                                   *   @par
                                   * EHCI Controller SSID
                                   */
  UINT32   Ohci4Ssid;            /**< Ohci4Ssid
                                   *   @par
                                   * OHCI4 Controller SSID (Force USB 1.1 mode)
                                   */
  UINT32   SmbusSsid;            /**< SmbusSsid
                                   *   @par
                                   *  SMBUS controller SSID
                                   */
  UINT32   IdeSsid;              /**< IdeSsid
                                   *   @par
                                   * IDE (Sata) controller SSID
                                   */
  UINT32   AzaliaSsid;           /**< AzaliaSsid
                                   *   @par
                                   * HD Audio controller SSID
                                   */
  UINT32   LpcSsid;              /**< LpcSsid
                                   *   @par
                                   * LPC controller SSID
                                   */
  UINT32   PCIBSsid;             /**< PCIBSsid
                                   *   @par
                                   * PCIB controller SSID
                                   */
} BUILDPARAM;

/**
 * The EC fan MSGREG struct  for CIMx Module. *
 */
typedef struct _EC_struct {
  UINT8  MSGFun81zone0MSGREG0;  ///<Thermal zone
  UINT8  MSGFun81zone0MSGREG1;  ///<Thermal zone
  UINT8  MSGFun81zone0MSGREG2;  ///<Thermal zone control byte 1
  UINT8  MSGFun81zone0MSGREG3;  ///<Thermal zone control byte 2
  UINT8  MSGFun81zone0MSGREG4;  ///<Bit[3:0] - Thermal diode offset adjustment in degrees Celsius.
  UINT8  MSGFun81zone0MSGREG5;  ///<Hysteresis inforamtion
  UINT8  MSGFun81zone0MSGREG6;  ///<SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032
  UINT8  MSGFun81zone0MSGREG7;  ///<Bit[1:0]: 0 - 2,  SMBUS bus number where the SMBUS based temperature sensor is located.
  UINT8  MSGFun81zone0MSGREG8;  ///< @todo Style_Analyzer: Add Doxygen comments to struct entry
  UINT8  MSGFun81zone0MSGREG9;  ///< @todo Style_Analyzer: Add Doxygen comments to struct entry

  //EC LDN9 funtion 81 zone 1
  UINT8  MSGFun81zone1MSGREG0;  ///<Thermal zone
  UINT8  MSGFun81zone1MSGREG1;  ///<Thermal zone
  UINT8  MSGFun81zone1MSGREG2;  ///<Thermal zone control byte 1
  UINT8  MSGFun81zone1MSGREG3;  ///<Thermal zone control byte 2
  UINT8  MSGFun81zone1MSGREG4;  ///<Bit[3:0] - Thermal diode offset adjustment in degrees Celsius.
  UINT8  MSGFun81zone1MSGREG5;  ///<Hysteresis inforamtion
  UINT8  MSGFun81zone1MSGREG6;  ///<SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032
  UINT8  MSGFun81zone1MSGREG7;  ///<Bit[1:0]: 0 - 2,  SMBUS bus number where the SMBUS based temperature sensor is located.
  UINT8  MSGFun81zone1MSGREG8;  ///< @todo Style_Analyzer: Add Doxygen comments to struct entry
  UINT8  MSGFun81zone1MSGREG9;  ///< @todo Style_Analyzer: Add Doxygen comments to struct entry

  //EC LDN9 funtion 81 zone 2
  UINT8  MSGFun81zone2MSGREG0;  ///<Thermal zone
  UINT8  MSGFun81zone2MSGREG1;  ///<Thermal zone
  UINT8  MSGFun81zone2MSGREG2;  ///<Thermal zone control byte 1
  UINT8  MSGFun81zone2MSGREG3;  ///<Thermal zone control byte 2
  UINT8  MSGFun81zone2MSGREG4;  ///<Bit[3:0] - Thermal diode offset adjustment in degrees Celsius.
  UINT8  MSGFun81zone2MSGREG5;  ///<Hysteresis inforamtion
  UINT8  MSGFun81zone2MSGREG6;  ///<SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032
  UINT8  MSGFun81zone2MSGREG7;  ///<Bit[1:0]: 0 - 2,  SMBUS bus number where the SMBUS based temperature sensor is located.
  UINT8  MSGFun81zone2MSGREG8;  ///< @todo Style_Analyzer: Add Doxygen comments to struct entry
  UINT8  MSGFun81zone2MSGREG9;  ///< @todo Style_Analyzer: Add Doxygen comments to struct entry

  //EC LDN9 funtion 81 zone 3
  UINT8  MSGFun81zone3MSGREG0;  ///<Thermal zone
  UINT8  MSGFun81zone3MSGREG1;  ///<Thermal zone
  UINT8  MSGFun81zone3MSGREG2;  ///<Thermal zone control byte 1
  UINT8  MSGFun81zone3MSGREG3;  ///<Thermal zone control byte 2
  UINT8  MSGFun81zone3MSGREG4;  ///<Bit[3:0] - Thermal diode offset adjustment in degrees Celsius.
  UINT8  MSGFun81zone3MSGREG5;  ///<Hysteresis inforamtion
  UINT8  MSGFun81zone3MSGREG6;  ///<SMBUS Address for SMBUS based temperature sensor such as SB-TSI and ADM1032
  UINT8  MSGFun81zone3MSGREG7;  ///<Bit[1:0]: 0 - 2,  SMBUS bus number where the SMBUS based temperature sensor is located.
  UINT8  MSGFun81zone3MSGREG8;  ///< @todo Style_Analyzer: Add Doxygen comments to struct entry
  UINT8  MSGFun81zone3MSGREG9;  ///< @todo Style_Analyzer: Add Doxygen comments to struct entry

  //EC LDN9 funtion 83 zone 0
  UINT8  MSGFun83zone0MSGREG0;  ///<Thermal zone
  UINT8  MSGFun83zone0MSGREG1;  ///<Thermal zone
  UINT8  MSGFun83zone0MSGREG2;  ///<_AC0
  UINT8  MSGFun83zone0MSGREG3;  ///<_AC1
  UINT8  MSGFun83zone0MSGREG4;  ///<_AC2
  UINT8  MSGFun83zone0MSGREG5;  ///<_AC3
  UINT8  MSGFun83zone0MSGREG6;  ///<_AC4
  UINT8  MSGFun83zone0MSGREG7;  ///<_AC5
  UINT8  MSGFun83zone0MSGREG8;  ///<_AC6
  UINT8  MSGFun83zone0MSGREG9;  ///<_AC7
  UINT8  MSGFun83zone0MSGREGA;  ///<_CRT
  UINT8  MSGFun83zone0MSGREGB;  ///<_PSV

  //EC LDN9 funtion 83 zone 1
  UINT8  MSGFun83zone1MSGREG0;  ///<Thermal zone
  UINT8  MSGFun83zone1MSGREG1;  ///<Thermal zone
  UINT8  MSGFun83zone1MSGREG2;  ///<_AC0
  UINT8  MSGFun83zone1MSGREG3;  ///<_AC1
  UINT8  MSGFun83zone1MSGREG4;  ///<_AC2
  UINT8  MSGFun83zone1MSGREG5;  ///<_AC3
  UINT8  MSGFun83zone1MSGREG6;  ///<_AC4
  UINT8  MSGFun83zone1MSGREG7;  ///<_AC5
  UINT8  MSGFun83zone1MSGREG8;  ///<_AC6
  UINT8  MSGFun83zone1MSGREG9;  ///<_AC7
  UINT8  MSGFun83zone1MSGREGA;  ///<_CRT
  UINT8  MSGFun83zone1MSGREGB;  ///<_PSV

  //EC LDN9 funtion 83 zone 2
  UINT8  MSGFun83zone2MSGREG0;  ///<Thermal zone
  UINT8  MSGFun83zone2MSGREG1;  ///<Thermal zone
  UINT8  MSGFun83zone2MSGREG2;  ///<_AC0
  UINT8  MSGFun83zone2MSGREG3;  ///<_AC1
  UINT8  MSGFun83zone2MSGREG4;  ///<_AC2
  UINT8  MSGFun83zone2MSGREG5;  ///<_AC3
  UINT8  MSGFun83zone2MSGREG6;  ///<_AC4
  UINT8  MSGFun83zone2MSGREG7;  ///<_AC5
  UINT8  MSGFun83zone2MSGREG8;  ///<_AC6
  UINT8  MSGFun83zone2MSGREG9;  ///<_AC7
  UINT8  MSGFun83zone2MSGREGA;  ///<_CRT
  UINT8  MSGFun83zone2MSGREGB;  ///<_PSV

  //EC LDN9 funtion 83 zone 3
  UINT8  MSGFun83zone3MSGREG0;  ///<Thermal zone
  UINT8  MSGFun83zone3MSGREG1;  ///<Thermal zone
  UINT8  MSGFun83zone3MSGREG2;  ///<_AC0
  UINT8  MSGFun83zone3MSGREG3;  ///<_AC1
  UINT8  MSGFun83zone3MSGREG4;  ///<_AC2
  UINT8  MSGFun83zone3MSGREG5;  ///<_AC3
  UINT8  MSGFun83zone3MSGREG6;  ///<_AC4
  UINT8  MSGFun83zone3MSGREG7;  ///<_AC5
  UINT8  MSGFun83zone3MSGREG8;  ///<_AC6
  UINT8  MSGFun83zone3MSGREG9;  ///<_AC7
  UINT8  MSGFun83zone3MSGREGA;  ///<_CRT
  UINT8  MSGFun83zone3MSGREGB;  ///<_PSV

  //EC LDN9 funtion 85 zone 0
  UINT8  MSGFun85zone0MSGREG0;  ///<Thermal zone
  UINT8  MSGFun85zone0MSGREG1;  ///<Thermal zone
  UINT8  MSGFun85zone0MSGREG2;  ///<AL0 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone0MSGREG3;  ///<AL1 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone0MSGREG4;  ///<AL2 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone0MSGREG5;  ///<AL3 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone0MSGREG6;  ///<AL4 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone0MSGREG7;  ///<AL5 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone0MSGREG8;  ///<AL6 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone0MSGREG9;  ///<AL7 PWM level in percentage (0 - 100%)

  //EC LDN9 funtion 85 zone 1
  UINT8  MSGFun85zone1MSGREG0;  ///<Thermal zone
  UINT8  MSGFun85zone1MSGREG1;  ///<Thermal zone
  UINT8  MSGFun85zone1MSGREG2;  ///<AL0 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone1MSGREG3;  ///<AL1 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone1MSGREG4;  ///<AL2 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone1MSGREG5;  ///<AL3 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone1MSGREG6;  ///<AL4 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone1MSGREG7;  ///<AL5 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone1MSGREG8;  ///<AL6 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone1MSGREG9;  ///<AL7 PWM level in percentage (0 - 100%)

  //EC LDN9 funtion 85 zone 2
  UINT8  MSGFun85zone2MSGREG0;  ///<Thermal zone
  UINT8  MSGFun85zone2MSGREG1;  ///<Thermal zone
  UINT8  MSGFun85zone2MSGREG2;  ///<AL0 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone2MSGREG3;  ///<AL1 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone2MSGREG4;  ///<AL2 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone2MSGREG5;  ///<AL3 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone2MSGREG6;  ///<AL4 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone2MSGREG7;  ///<AL5 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone2MSGREG8;  ///<AL6 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone2MSGREG9;  ///<AL7 PWM level in percentage (0 - 100%)

  //EC LDN9 funtion 85 zone 3
  UINT8  MSGFun85zone3MSGREG0;  ///<Thermal zone
  UINT8  MSGFun85zone3MSGREG1;  ///<Thermal zone
  UINT8  MSGFun85zone3MSGREG2;  ///<AL0 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone3MSGREG3;  ///<AL1 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone3MSGREG4;  ///<AL2 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone3MSGREG5;  ///<AL3 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone3MSGREG6;  ///<AL4 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone3MSGREG7;  ///<AL5 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone3MSGREG8;  ///<AL6 PWM level in percentage (0 - 100%)
  UINT8  MSGFun85zone3MSGREG9;  ///<AL7 PWM level in percentage (0 - 100%)

  //EC LDN9 funtion 89 TEMPIN channel 0
  UINT8  MSGFun89zone0MSGREG0;  ///<Thermal zone
  UINT8  MSGFun89zone0MSGREG1;  ///<Thermal zone
  UINT8  MSGFun89zone0MSGREG2;  ///<At DWORD bit 0-7
  UINT8  MSGFun89zone0MSGREG3;  ///<At DWORD bit 15-8
  UINT8  MSGFun89zone0MSGREG4;  ///<At DWORD bit 23-16
  UINT8  MSGFun89zone0MSGREG5;  ///<At DWORD bit 31-24
  UINT8  MSGFun89zone0MSGREG6;  ///<Ct DWORD bit 0-7
  UINT8  MSGFun89zone0MSGREG7;  ///<Ct DWORD bit 15-8
  UINT8  MSGFun89zone0MSGREG8;  ///<Ct DWORD bit 23-16
  UINT8  MSGFun89zone0MSGREG9;  ///<Ct DWORD bit 31-24
  UINT8  MSGFun89zone0MSGREGA;  ///<Mode bit 0-7

  //EC LDN9 funtion 89 TEMPIN channel 1
  UINT8  MSGFun89zone1MSGREG0;  ///<Thermal zone
  UINT8  MSGFun89zone1MSGREG1;  ///<Thermal zone
  UINT8  MSGFun89zone1MSGREG2;  ///<At DWORD bit 0-7
  UINT8  MSGFun89zone1MSGREG3;  ///<At DWORD bit 15-8
  UINT8  MSGFun89zone1MSGREG4;  ///<At DWORD bit 23-16
  UINT8  MSGFun89zone1MSGREG5;  ///<At DWORD bit 31-24
  UINT8  MSGFun89zone1MSGREG6;  ///<Ct DWORD bit 0-7
  UINT8  MSGFun89zone1MSGREG7;  ///<Ct DWORD bit 15-8
  UINT8  MSGFun89zone1MSGREG8;  ///<Ct DWORD bit 23-16
  UINT8  MSGFun89zone1MSGREG9;  ///<Ct DWORD bit 31-24
  UINT8  MSGFun89zone1MSGREGA;  ///<Mode bit 0-7

  //EC LDN9 funtion 89 TEMPIN channel 2
  UINT8  MSGFun89zone2MSGREG0;  ///<Thermal zone
  UINT8  MSGFun89zone2MSGREG1;  ///<Thermal zone
  UINT8  MSGFun89zone2MSGREG2;  ///<At DWORD bit 0-7
  UINT8  MSGFun89zone2MSGREG3;  ///<At DWORD bit 15-8
  UINT8  MSGFun89zone2MSGREG4;  ///<At DWORD bit 23-16
  UINT8  MSGFun89zone2MSGREG5;  ///<At DWORD bit 31-24
  UINT8  MSGFun89zone2MSGREG6;  ///<Ct DWORD bit 0-7
  UINT8  MSGFun89zone2MSGREG7;  ///<Ct DWORD bit 15-8
  UINT8  MSGFun89zone2MSGREG8;  ///<Ct DWORD bit 23-16
  UINT8  MSGFun89zone2MSGREG9;  ///<Ct DWORD bit 31-24
  UINT8  MSGFun89zone2MSGREGA;  ///<Mode bit 0-7

  //EC LDN9 funtion 89 TEMPIN channel 3
  UINT8  MSGFun89zone3MSGREG0;  ///<Thermal zone
  UINT8  MSGFun89zone3MSGREG1;  ///<Thermal zone
  UINT8  MSGFun89zone3MSGREG2;  ///<At DWORD bit 0-7
  UINT8  MSGFun89zone3MSGREG3;  ///<At DWORD bit 15-8
  UINT8  MSGFun89zone3MSGREG4;  ///<At DWORD bit 23-16
  UINT8  MSGFun89zone3MSGREG5;  ///<At DWORD bit 31-24
  UINT8  MSGFun89zone3MSGREG6;  ///<Ct DWORD bit 0-7
  UINT8  MSGFun89zone3MSGREG7;  ///<Ct DWORD bit 15-8
  UINT8  MSGFun89zone3MSGREG8;  ///<Ct DWORD bit 23-16
  UINT8  MSGFun89zone3MSGREG9;  ///<Ct DWORD bit 31-24
  UINT8  MSGFun89zone3MSGREGA;  ///<Mode bit 0-7

  // FLAG for Fun83/85/89 support
  UINT16 IMCFUNSupportBitMap;	 /// Bit0=81FunZone0 support(1=On;0=Off); bit1-3=81FunZone1-Zone3;Bit4-7=83FunZone0-Zone3;Bit8-11=85FunZone0-Zone3;Bit11-15=89FunZone0-Zone3;
} EC_struct;
/** SBGPPPORTCONFIG - Southbridge GPP port config structure */
typedef struct {
  UINT32   PortPresent:1;     /**< Port connection
                                   *   @par
                                   *    @li <b>0</b> - Port doesn't have slot. No need to train the link
                                   *    @li <b>1</b> - Port connection defined and needs to be trained
                                   */
  UINT32   PortDetected:1;    /**< Link training status
                                   *   @par
                                   *    @li <b>0</b> - EP not detected
                                   *    @li <b>1</b> - EP detected
                                   */
  UINT32   PortIsGen2:2;      /**< Port link speed configuration
                                   *   @par
                                   *    @li <b>00</b> - Auto
                                   *    @li <b>01</b> - Forced GEN1
                                   *    @li <b>10</b> - Forced GEN2
                                   *    @li <b>11</b> - Reserved
                                   */

  UINT32   PortHotPlug:1;     /**< Support hot plug?
                                   *   @par
                                   *    @li <b>0</b> - No support
                                   *    @li <b>1</b> - support
                                   */
/** PortMisc - Reserved */
  UINT32   PortMisc:27;
} SBGPPPORTCONFIG;

/** CODECENTRY - Southbridge HD Audio OEM Codec structure */
typedef struct _CODECENTRY {
/** Nid - Reserved ?? */
  UINT8 Nid;
/** Byte40 - Reserved ?? */
  UINT32 Byte40;
} CODECENTRY;

/** CODECTBLLIST - Southbridge HD Audio Codec table list */
typedef struct _CODECTBLLIST {
/** CodecID - Codec ID */
  UINT32   CodecID;
/** CodecTablePtr - Codec table pointer */
  CODECENTRY* CodecTablePtr;
} CODECTBLLIST;

/** Sata Controller structure */
typedef struct _SATAST {
  UINT8   SataController:1;            /**< SataController
                                        *   @par
                                        * Sata Controller
                                        *    @li <b>0</b> - disable
                                        *    @li <b>1</b> - enable
                                        */
  UINT8   SataIdeCombMdPriSecOpt:1;    /**< SataIdeCombMdPriSecOpt - Reserved */
  UINT8   SataSetMaxGen2:1;            /**< SataSetMaxGen2
                                        *   @par
                                        * Sata Controller Set to Max Gen2 mode
                                        *    @li <b>0</b> - disable
                                        *    @li <b>1</b> - enable
                                        */
  UINT8   SataIdeCombinedMode:1;       /**< SataIdeCombinedMode
                                        *   @par
                                        * Sata IDE Controller set to Combined Mode
                                        *    @li <b>0</b> - enable
                                        *    @li <b>1</b> - disable
                                        */
/** SATARefClkSel - Reserved */
  UINT8   SATARefClkSel:2;              // 4:5
/** SATARefDivSel - Reserved */
  UINT8   SATARefDivSel:2;              // 6:7
} SATAST;

/** _USBST Controller structure
 *
 * Usb Ohci1 Contoller is define at BIT0
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 18 Func0) *
 * Usb Ehci1 Contoller is define at BIT1
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 18 Func2) *
 * Usb Ohci2 Contoller is define at BIT2
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 19 Func0) *
 * Usb Ehci2 Contoller is define at BIT3
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 19 Func2) *
 * Usb Ohci3 Contoller is define at BIT4
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 22 Func0) *
 * Usb Ehci3 Contoller is define at BIT5
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 22 Func2) *
 * Usb Ohci4 Contoller is define at BIT6
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 20 Func5) *
 */
typedef struct _USBST {
  UINT8  Ohci1:1;                      ///< Ohci0 controller - 0:disable, 1:enable
  UINT8  Ehci1:1;                      ///< Ehci1 controller - 0:disable, 1:enable
  UINT8  Ohci2:1;                      ///< Ohci2 controller - 0:disable, 1:enable
  UINT8  Ehci2:1;                      ///< Ehci2 controller - 0:disable, 1:enable
  UINT8  Ohci3:1;                      ///< Ohci3 controller - 0:disable, 1:enable
  UINT8  Ehci3:1;                      ///< Ehci3 controller - 0:disable, 1:enable
  UINT8  Ohci4:1;                      ///< Ohci4 controller - 0:disable, 1:enable
  UINT8  UTemp:1;                      ///< Reserved
} USBST;

/**
 * _AZALIAPIN - HID Azalia or GPIO define structure.
 *
 */
typedef struct _AZALIAPIN {
  UINT8   AzaliaSdin0:2;               /**< AzaliaSdin0
                                        *   @par
                                        * SDIN0 is define at BIT0 & BIT1
                                        *    @li <b>00</b> - GPIO PIN
                                        *    @li <b>10</b> - As a Azalia SDIN pin
                                        */
  UINT8   AzaliaSdin1:2;               /**< AzaliaSdin1
                                        *   @par
                                        * SDIN0 is define at BIT2 & BIT3
                                        *    @li <b>00</b> - GPIO PIN
                                        *    @li <b>10</b> - As a Azalia SDIN pin
                                        */
  UINT8   AzaliaSdin2:2;               /**< AzaliaSdin2
                                        *   @par
                                        * SDIN0 is define at BIT4 & BIT5
                                        *    @li <b>00</b> - GPIO PIN
                                        *    @li <b>10</b> - As a Azalia SDIN pin
                                        */
  UINT8   AzaliaSdin3:2;               /**< AzaliaSdin3
                                        *   @par
                                        * SDIN0 is define at BIT6 & BIT7
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
    UINT8   SataModeReg;
/** SataMode - Reserved */
    SATAST  SataMode;
  } SATAMODE;
/** S3Resume - Flag of ACPI S3 Resume. */
  UINT8   S3Resume:1;                   // 8
/** RebootRequired - Flag of Reboot system is required. */
  UINT8   RebootRequired:1;             // 9
/** SbSpiSpeedSupport - Reserved */
  UINT8   SbSpiSpeedSupport:1;   // 10
/**< SpreadSpectrum
 *   @par
 *   Spread Spectrum function
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  UINT8   SpreadSpectrum:1;             // 11
/** NbSbGen2 - Reserved */
  UINT8   NbSbGen2:1;                   // 12
  UINT8   GppGen2:1;                    // 13
  UINT8   GppMemWrImprove:1;            // 14
/** MsgXchgBiosCimxReserved - Reserved */
  UINT8   MsgXchgBiosCimxReserved:1;    // 15  (BB USED)
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
  UINT16   SataClass:3;                 // 16:18
/**< Sata IDE Controller mode
 *   @par
 *    @li <b>0</b> - Legacy IDE mode
 *    @li <b>1</b> - Native IDE mode
 */
  UINT16   SataIdeMode:1;               // 19
/**<  SataEspPort - SATA port is external accessible on a signal only connector (eSATA:)
 *   @par
 *    @li <b> BIT0 </b> - PORT0 set as ESP port
 *    @li <b> BIT1 </b> - PORT1 set as ESP port
 *    @li <b> BIT2 </b> - PORT2 set as ESP port
 *    @li <b> BIT3 </b> - PORT3 set as ESP port
 *    @li <b> BIT4 </b> - PORT4 set as ESP port
 *    @li <b> BIT5 </b> - PORT5 set as ESP port
 */
  UINT16   SataEspPort:6;               // 20:25
/** SataPortPower - Reserved */
  UINT16   SataPortPower:6;             // 31:26

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
  UINT32   SataPortMode:12;             //11:0
/** SATAClkSelOpt - Reserved */
  UINT32   SATAClkSelOpt:4;             // Removed from coding side
/** SataAggrLinkPmCap - Reserved */
  UINT32   SataAggrLinkPmCap:1;          //16, 0:OFF   1:ON
/** SataPortMultCap - Reserved */
  UINT32   SataPortMultCap:1;            //17, 0:OFF   1:ON
/** SataClkAutoOff - Reserved */
  UINT32   SataClkAutoOff:1;             //18, AutoClockOff 0:Disabled, 1:Enabled
/** SataPscCap - Reserved */
  UINT32   SataPscCap:1;                 //19, 0:Enable PSC capability, 1:Disable PSC capability
/** BIOSOSHandoff - Reserved */
  UINT32   BIOSOSHandoff:1;              //20
/** SataFisBasedSwitching - Reserved */
  UINT32   SataFisBasedSwitching:1;      //21
/** SataCccSupport - Reserved */
  UINT32   SataCccSupport:1;             //22
/** SataSscCap - Reserved */
  UINT32   SataSscCap:1;                 //23, 0:Enable SSC capability, 1:Disable SSC capability
/** SataMsiCapability - Reserved */
  UINT32   SataMsiCapability:1;          //24  0:Hidden 1:Visible. This feature is disabled per RPR, but remains the interface.
/** SataForceRaid - Reserved */
  UINT32   SataForceRaid:1;              //25  0:No function 1:Force RAID
/** SataDebugDummy - Reserved */
  UINT32   SataDebugDummy:6;             //31:26
//
// USB Configuration                //offset 4 bytes (130-133)
//

/** USBDeviceConfig - USB Controller Configuration
 *
 * - Usb Ohci1 Contoller is define at BIT0
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 18 Func0) *
 * - Usb Ehci1 Contoller is define at BIT1
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 18 Func2) *
 * - Usb Ohci2 Contoller is define at BIT2
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 19 Func0) *
 * - Usb Ehci2 Contoller is define at BIT3
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 19 Func2) *
 * - Usb Ohci3 Contoller is define at BIT4
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 22 Func0) *
 * - Usb Ehci3 Contoller is define at BIT5
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 22 Func2) *
 * - Usb Ohci4 Contoller is define at BIT6
 *   - 0:disable 1:enable
 *                             (Bus 0 Dev 20 Func5) *
 */
  union /**< union - Reserved */
  {  /**< USBMODE - USB Controller structure */
/** SataModeReg - Reserved */
    UINT8   UsbModeReg;
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
  UINT8   GecConfig:1;                 //8

/**< IrConfig
 *   @par
 *   Ir Controller setting
 *    @li <b>00 </b> - disable
 *    @li <b>01 </b> - Rx and Tx0
 *    @li <b>10 </b> - Rx and Tx1
 *    @li <b>11 </b> - Rx and both Tx0,Tx1
 */
  UINT8   IrConfig:2;                  //9:10

/** GecDummy - Reserved */
  UINT8   GecDummy:5;                  //15:11

  //Azalia Configuration

/**< AzaliaController - Azalia Controller Configuration
 *   @par
 *   Azalia Controller [0-1]
 *    @li <b>0</b> - Auto   : Detect Azalia controller automatically.
 *    @li <b>1</b> - Diable : Disable Azalia controller.
 *    @li <b>2</b> - Enable : Enable Azalia controller.
 */
  UINT8   AzaliaController:2;          //17:16
/**< AzaliaPinCfg - Azalia Controller SDIN pin Configuration
 *   @par
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  UINT8   AzaliaPinCfg:1;              //18
/**< AzaliaFrontPanel - Azalia Controller Front Panel Configuration
 *   @par
 *   Support Front Panel configuration
 *    @li <b>0</b> - Auto
 *    @li <b>1</b> - disable
 *    @li <b>2</b> - enable
 */
  UINT8   AzaliaFrontPanel:2;          //20:19
/**< FrontPanelDetected - Force Azalia Controller Front Panel Configuration
 *   @par
 *   Force Front Panel configuration
 *    @li <b>0</b> - Not Detected
 *    @li <b>1</b> - Detected
 */
  UINT8   FrontPanelDetected:1;        //21
/**< AzaliaSnoop - Azalia Controller Snoop feature Configuration
 *   @par
 *   Azalia Controller Snoop feature Configuration
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  UINT8   AzaliaSnoop:1;               //22
/** AzaliaDummy - Reserved */
  UINT8   AzaliaDummy:1;               //23

  union
  {
/**< AzaliaSdinPin - Azalia Controller SDIN pin Configuration
 *
 * SDIN0 is define at BIT0 & BIT1
 *   - 00: GPIO PIN
 *   - 01: Reserved
 *   - 10: As a Azalia SDIN pin
 *
 * SDIN1 is define at BIT2 & BIT3
 *      * Config same as SDIN0
 * SDIN2 is define at BIT4 & BIT5
 *      * Config same as SDIN0
 * SDIN3 is define at BIT6 & BIT7
 *      * Config same as SDIN0
 */
    UINT8   AzaliaSdinPin;
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
    VOID*   pAzaliaOemFpCodecTablePtr;       //offset 4 bytes (138-141)
  } AZOEMFPTBL;

  //Miscellaneous Configuration      //offset 4 bytes (142-145)
/** AnyHT200MhzLink - Reserved */
  UINT32   AnyHT200MhzLink:1;             //0
/**< HpetTimer - South Bridge Hpet Timer Configuration
 *   @par
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  UINT32   HpetTimer:1;                 //1
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
  UINT32   PciClks:5;                   //2:6
/** MiscReserved1 - Reserved */
  UINT32   MiscReserved1:4;             //9:7, Reserved
/** MobilePowerSavings - Debug function Reserved */
  UINT32   MobilePowerSavings:1;        //11, 0:Disable, 1:Enable Power saving features especially for Mobile platform
/** MiscDummy1 - Debug function Reserved */
  UINT32   MiscDummy1:1;
/** NativePcieSupport - Debug function Reserved */
  UINT32   NativePcieSupport:1;         //13, 0:Enable, 1:Disabled
/** FlashPinConfig - Debug function Reserved */
  UINT32   FlashPinConfig:1;            //14, 0:desktop mode 1:mobile mode
/** UsbPhyPowerDown - Debug function Reserved */
  UINT32   UsbPhyPowerDown:1;           //15
/** PcibClkStopOverride - Debug function Reserved */
  UINT32   PcibClkStopOverride:10;      //25:16
/**< HpetMsiDis - South Bridge HPET MSI Configuration
 *   @par
 *    @li <b>1</b> - disable
 *    @li <b>0</b> - enable
 */
  UINT32   HpetMsiDis:1;                //26
/**< ResetCpuOnSyncFlood - Rest CPU on Sync Flood
 *   @par
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  UINT32   ResetCpuOnSyncFlood:1;       //27
/**< LdtStpDisable - LdtStp# output disable
 *   @par
 *    @li <b>0</b> - LdtStp# output enable
 *    @li <b>1</b> - LdtStp# output disable
 */
  UINT32   LdtStpDisable:1;             //28
/**< MTC1e - Message Triggered C1e
 *   @par
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  UINT32   MTC1e:1;                     //29
/** MiscDummy - Reserved */
  UINT32   MiscDummy:2;                 //31:30

  //DebugOptions                     //offset 4 bytes (146-149)
/** PcibAutoClkCtrlLow - Debug function Reserved */
  UINT32   PcibAutoClkCtrlLow:16;
/** PcibAutoClkCtrlHigh - Debug function Reserved */
  UINT32   PcibAutoClkCtrlHigh:16;

/**< OEMPROGTBL - ACPI MMIO register setting table OEM override
 *   @par
 *   OEM table for customer override ACPI MMIO register in their code.
 */
  union
  {
    PLACEHOLDER OemProgrammingTablePtr;  //offset 4 bytes (150-153)
    VOID   *OemProgrammingTablePtr_Ptr;
  } OEMPROGTBL;

  //Gpp Configuration                   //offset 24 bytes total (154-177)
  union {
    UINT32           PORTCFG32;
    SBGPPPORTCONFIG  PortCfg;
  } PORTCONFIG[MAX_GPP_PORTS];          //offset 16 bytes

  UINT32           GppLinkConfig;       // GPP_LINK_CONFIG = PCIE_GPP_Enable[3:0]
                                        //   0000 - Port ABCD -> 4:0:0:0
                                        //   0001 - N/A
                                        //   0010 - Port ABCD -> 2:2:0:0
                                        //   0011 - Port ABCD -> 2:1:1:0
                                        //   0100 - Port ABCD -> 1:1:1:1
                                        //
  UINT32   GppFoundGfxDev:4;            //3:0 If port A-D (mapped to bit [3:0]) has GFX EP detected
  UINT32   CoreGen2Enable:1;            //4
  UINT32   GppFunctionEnable:1;         //5
  UINT32   GppUnhidePorts:1;            //6
  UINT32   AlinkPhyPllPowerDown:1;      //7
  UINT32   GppConfigDummy1:2;           //9:8
  UINT32   GppLaneReversal:1;           //10
  UINT32   GppPhyPllPowerDown:1;        //11
  UINT32   GppCompliance :1;            //12
  UINT32   GppPortAspm:8;               //20:13 ASPM state for GPP ports, 14:13 for port0, ..., 20:19 for port3
                                        //  00 - Disabled
                                        //  01 - L0s
                                        //  10 - L1
                                        //  11 - L0s + L1
                                        //
  UINT32   GppConfigDummy:11;           //31:21

  //TempMMIO                   //offset 4 bytes (178-181)
  UINT32   TempMMIO;

  // DebugOption2
  UINT32   GecPhyStatus:1;
  UINT32   GecDebugOptionDummy:7;
  UINT32   SBGecPwr:2;
  UINT32   SBGecDebugBus:1;
  UINT32   DebugOption2Dummy1:1;
  UINT32   DebugOption2Dummy2:1;
  UINT32   SbPcieOrderRule:1;
  UINT32   SbUsbPll:1;
  UINT32   AcDcMsg:1;
  UINT32   TimerTickTrack:1;
  UINT32   ClockInterruptTag:1;
  UINT32   OhciTrafficHanding:1;
  UINT32   EhciTrafficHanding:1;
  UINT32   FusionMsgCMultiCore:1;
  UINT32   FusionMsgCStage:1;
/**< UsbRxMode - CG PLL multiplier for USB Rx 1.1 mode
 *   @par
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
  UINT32   UsbRxMode:1;
  UINT32   DebugOption2Dummy3:9;        //

  union
  {
    PLACEHOLDER DynamicGecRomAddressPtr;  //offset 4 bytes (182-185)
    VOID   *DynamicGecRomAddress_Ptr;
  } DYNAMICGECROM;
  EC_struct Pecstruct;
} AMDSBCFG;

/** SMMSERVICESTRUC- Southbridge SMI service structure */
typedef  struct   _SMMSERVICESTRUC {
/** enableRegNum - Reserved */
  UINT8 enableRegNum;
/** enableBit - Reserved */
  UINT8 enableBit;
/** statusRegNum - Reserved */
  UINT8 statusRegNum;
/** statusBit - Reserved */
  UINT8 statusBit;
/** *debugMessage- Reserved */
  CHAR8 *debugMessage;
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
    UINT8 bRegIndex;
/** bANDMask - Reserved */
    UINT8 bANDMask;
/** bORMask - Reserved */
    UINT8 bORMask;
  } REG8MASK;
#endif

/**
 *  - SATA Phy setting structure
 *
 */
typedef struct _SATAPHYSETTING {
/** wPhyCoreControl - Reserved */
  UINT16 wPhyCoreControl;
/** dwPhyFineTune - Reserved */
  UINT32 dwPhyFineTune;
} SATAPHYSETTING;

/**
 * _ABTblEntry - AB link register table R/W structure
 *
 */
typedef struct _ABTblEntry {
  /** regType  : AB Register Type (ABCFG, AXCFG and so on) */
  UINT8    regType;
  /** regIndex : AB Register Index */
  UINT32   regIndex;
  /** regMask  : AB Register Mask */
  UINT32   regMask;
  /** regData  : AB Register Data */
  UINT32   regData;
} ABTBLENTRY;

/**
 * _AcpiRegWrite - ACPI MMIO register R/W structure
 *
 */
typedef struct _AcpiRegWrite {
  /** MmioBase     : Index of Soubridge block (For instence GPIO_BASE:0x01 SMI_BASE:0x02) */
  UINT8 MmioBase;
  /** MmioReg      : Register index */
  UINT8 MmioReg;
  /** DataANDMask  : AND Register Data */
  UINT8 DataANDMask;
  /** DataOrMask   : Or Register Data */
  UINT8 DataOrMask;
} AcpiRegWrite;

/**
 * PCI_ADDRESS - PCI access structure
 *
 */
#define PCI_ADDRESS(bus, dev, func, reg) \
(UINT32) ( (((UINT32)bus) << 24) + (((UINT32)dev) << 19) + (((UINT32)func) << 16) + ((UINT32)reg) )

/**
 * CIM_STATUS - CIMx module function return code
 */
typedef UINT32    CIM_STATUS;
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
 * DISABLED - Define disable in module
 */
#define DISABLED   0
/**
 * ENABLED - Define enable in module
 */
#define ENABLED    1

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
