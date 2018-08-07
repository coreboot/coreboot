/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#ifndef _PLATFORM_CFG_H_
#define _PLATFORM_CFG_H_

#include <stdint.h>


/**
 * @def BIOS_SIZE
 *      BIOS_SIZE_{1,2,4,8,16}M
 *
 * In Hudson-2, default ROM size is 1M Bytes, if your platform ROM
 * bigger than 1M you have to set the ROM size outside CIMx module and
 * before AGESA module get call.
 */
#ifndef BIOS_SIZE
#define BIOS_SIZE ((CONFIG_COREBOOT_ROMSIZE_KB >> 10) - 1)
#endif /* BIOS_SIZE */

/**
 *   @section SBCIMx_LEGACY_FREE SBCIMx_LEGACY_FREE
 *    @li <b>1</b> - Legacy free enable
 *    @li <b>0</b> - Legacy free disable
 */
#ifndef SBCIMx_LEGACY_FREE
  #define SBCIMx_LEGACY_FREE        0
#endif

/**
 *   @section SpiSpeed
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#ifndef SBCIMX_SPI_SPEED
  #define SBCIMX_SPI_SPEED 0
#endif

/**
 *   @section SpiFastSpeed
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#ifndef SBCIMX_SPI_FASTSPEED
  #define SBCIMX_SPI_FASTSPEED 0
#endif

/**
 *   @section SpiMode
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#ifndef SBCIMX_SPI_MODE
  #define SBCIMX_SPI_MODE 0
#endif

/**
 *   @section SpiBurstWrite
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#ifndef SBCIMX_SPI_BURST_WRITE
  #define SBCIMX_SPI_BURST_WRITE 0
#endif

/**
 *   @section INCHIP_EC_KBD INCHIP_EC_KBD
 *    @li <b>0</b> - Use SIO PS/2 function.
 *    @li <b>1</b> - Use EC PS/2 function.
 */
#ifndef INCHIP_EC_KBD
  #define INCHIP_EC_KBD             0
#endif

/**
 *   @section INCHIP_EC_CHANNEL10 INCHIP_EC_CHANNEL10
 *    @li <b>0</b> - EC controller NOT support Channel10
 *    @li <b>1</b> - EC controller support Channel10.
 */
#ifndef INCHIP_EC_CHANNEL10
  #define INCHIP_EC_CHANNEL10       1
#endif

/**
 *   @section Smbus0BaseAddress
 */
// #ifndef SMBUS0_BASE_ADDRESS
//  #define SMBUS0_BASE_ADDRESS 0xB00
// #endif

/**
 *   @section Smbus1BaseAddress
 */
// #ifndef SMBUS1_BASE_ADDRESS
//   #define SMBUS1_BASE_ADDRESS 0xB21
// #endif

/**
 *   @section SioPmeBaseAddress
 */
// #ifndef SIO_PME_BASE_ADDRESS
//   #define SIO_PME_BASE_ADDRESS 0xE00
// #endif

/**
 *   @section WatchDogTimerBase
 */
// #ifndef WATCHDOG_TIMER_BASE_ADDRESS
//   #define WATCHDOG_TIMER_BASE_ADDRESS IO_APIC_ADDR
// #endif

/**
 *   @section GecShadowRomAddress
 */
#ifndef GEC_ROM_SHADOW_ADDRESS
  #define GEC_ROM_SHADOW_ADDRESS 0xFED61000
#endif

/**
 *   @section SpiRomBaseAddress
 */
// #ifndef SPI_BASE_ADDRESS
//   #define SPI_BASE_ADDRESS 0xFEC10000
// #endif

/**
 *   @section AcpiPm1EvtBlkAddr
 */
// #ifndef PM1_EVT_BLK_ADDRESS
//   #define PM1_EVT_BLK_ADDRESS 0x400
// #endif

/**
 *   @section AcpiPm1CntBlkAddr
 */
// #ifndef PM1_CNT_BLK_ADDRESS
//   #define PM1_CNT_BLK_ADDRESS 0x404
// #endif

/**
 *   @section AcpiPmTmrBlkAddr
 */
// #ifndef PM1_TMR_BLK_ADDRESS
//   #define PM1_TMR_BLK_ADDRESS 0x408
// #endif

/**
 *   @section CpuControlBlkAddr
 */
// #ifndef CPU_CNT_BLK_ADDRESS
//   #define CPU_CNT_BLK_ADDRESS 0x410
// #endif

/**
 *   @section AcpiGpe0BlkAddr
 */
// #ifndef GPE0_BLK_ADDRESS
//   #define GPE0_BLK_ADDRESS 0x420
// #endif

/**
 *   @section SmiCmdPortAddr
 */
// #ifndef SMI_CMD_PORT
//   #define SMI_CMD_PORT 0xB0
// #endif

/**
 *   @section AcpiPmaCntBlkAddr
 */
// #ifndef ACPI_PMA_CNT_BLK_ADDRESS
//   #define ACPI_PMA_CNT_BLK_ADDRESS 0xFE00
// #endif

/**
 *   @section InChipSataController
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#ifndef INCHIP_SATA_CONTROLLER
  #define INCHIP_SATA_CONTROLLER    1
#endif

/**
 *  @section SataIdeCombModeChannel
 *    @li <b>0</b> - Primary
 *    @li <b>1</b> - Secondary<TD></TD>
 *  Sata Controller set as primary or
 *      secondary while Combined Mode is enabled
 */
#ifndef SATA_COMBINE_MODE_CHANNEL
  #define SATA_COMBINE_MODE_CHANNEL 0
#endif

/**
 *   @section SataSetMaxGen2
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 *    SataController Set to Max Gen2 mode
 */
#ifndef SATA_MAX_GEN2_MODE
  #define SATA_MAX_GEN2_MODE        0
#endif

/**
 *   @section SataCombineMode
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 *    Sata IDE Controller set to Combined Mode
 */
#ifndef SATA_COMBINE_MODE
  #define SATA_COMBINE_MODE         0
#endif

#define SATA_CLK_RESERVED           9

/**
 *   @section NbSbGen2
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#ifndef NB_SB_GEN2
  #define NB_SB_GEN2 1
#endif

/**
 *   @section SataInternal100Spread
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#ifndef INCHIP_SATA_INTERNAL_100_SPREAD
  #define INCHIP_SATA_INTERNAL_100_SPREAD     0
#endif

/**
 *   @section Cg2Pll
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#ifndef INCHIP_CG2_PLL
  #define INCHIP_CG2_PLL 0
#endif




/**
 *   @section SpreadSpectrum
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 *    Spread Spectrum function
 */
#define INCHIP_SPREAD_SPECTRUM     1

/**
 *   @section INCHIP_USB_CINFIG INCHIP_USB_CINFIG
 *
 * - Usb Ohci1 Controller is define at BIT0
 *    0:Disable 1:Enable
 *                             (Bus 0 Dev 18 Func0)
 * - Usb Ehci1 Controller is define at BIT1
 *    0:Disable 1:Enable
 *                             (Bus 0 Dev 18 Func2)
 * - Usb Ohci2 Controller is define at BIT2
 *    0:Disable 1:Enable
 *                             (Bus 0 Dev 19 Func0)
 * - Usb Ehci2 Controller is define at BIT3
 *    0:Disable 1:Enable
 *                             (Bus 0 Dev 19 Func2)
 * - Usb Ohci3 Controller is define at BIT4
 *    0:Disable 1:Enable
 *                             (Bus 0 Dev 22 Func0)
 * - Usb Ehci3 Controller is define at BIT5
 *    0:Disable 1:Enable
 *                             (Bus 0 Dev 22 Func2)
 * - Usb Ohci4 Controller is define at BIT6
 *    0:Disable 1:Enable
 *                             (Bus 0 Dev 20 Func5)
 */
#define INCHIP_USB_CINFIG          0x7F
#define INCHIP_USB_OHCI1_CINFIG    0x01
#define INCHIP_USB_OHCI2_CINFIG    0x01
#if IS_ENABLED(CONFIG_ONBOARD_USB30)
#define INCHIP_USB_OHCI3_CINFIG    0x00
#else
#define INCHIP_USB_OHCI3_CINFIG    0x01
#endif
#define INCHIP_USB_OHCI4_CINFIG    0x01
#define INCHIP_USB_EHCI1_CINFIG    0x01
#define INCHIP_USB_EHCI2_CINFIG    0x01
#define INCHIP_USB_EHCI3_CINFIG    0x01

/**
 *   @section INCHIP_SATA_MODE INCHIP_SATA_MODE
 *    @li <b>000</b> - Native IDE mode
 *    @li <b>001</b> - RAID mode
 *    @li <b>010</b> - AHCI mode
 *    @li <b>011</b> - Legacy IDE mode
 *    @li <b>100</b> - IDE->AHCI mode
 *    @li <b>101</b> - AHCI mode as 7804 ID (AMD driver)
 *    @li <b>110</b> - IDE->AHCI mode as 7804 ID (AMD driver)
 */
#define INCHIP_SATA_MODE           0

/**
 *   @section INCHIP_IDE_MODE INCHIP_IDE_MODE
 *    @li <b>0</b> - Legacy IDE mode
 *    @li <b>1</b> - Native IDE mode<TD></TD>
 *     ** DO NOT ALLOW SATA & IDE use same mode **
 */
#define INCHIP_IDE_MODE            1

#define SATA_PORT_MULT_CAP_RESERVED  1

/**
 *   @section INCHIP_AZALIA_CONTROLLER INCHIP_AZALIA_CONTROLLER
 *    @li <b>0</b> - Auto   : Detect Azalia controller automatically.
 *    @li <b>1</b> - Disable : Disable Azalia controller.
 *    @li <b>2</b> - Enable : Enable Azalia controller.
 */
#define INCHIP_AZALIA_CONTROLLER   2
#define AZALIA_AUTO				0
#define AZALIA_DISABLE			1
#define AZALIA_ENABLE			2

/**
 *   @section INCHIP_AZALIA_PIN_CONFIG INCHIP_AZALIA_PIN_CONFIG
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable
 */
#define INCHIP_AZALIA_PIN_CONFIG   1

/**
 *   @section AZALIA_PIN_CONFIG AZALIA_PIN_CONFIG
 *
 * SDIN0 is define at BIT0 & BIT1
 *   - 00: GPIO PIN
 *   - 01: Reserved
 *   - 10: As a Azalia SDIN pin<TD></TD>
 * SDIN1 is define at BIT2 & BIT3
 *   - 00: GPIO PIN
 *   - 01: Reserved
 *   - 10: As a Azalia SDIN pin<TD></TD>
 * SDIN2 is define at BIT4 & BIT5
 *   - 00: GPIO PIN
 *   - 01: Reserved
 *   - 10: As a Azalia SDIN pin<TD></TD>
 * SDIN3 is define at BIT6 & BIT7
 *   - 00: GPIO PIN
 *   - 01: Reserved
 *   - 10: As a Azalia SDIN pin
 */
#define AZALIA_PIN_CONFIG          0x2A

/**
 *   @section AzaliaSnoop
 *    @li <b>0</b> - disable
 *    @li <b>1</b> - enable *
 */
#define INCHIP_AZALIA_SNOOP        0x01

/**
 *   @section NCHIP_GEC_CONTROLLER
 *    @li <b>0</b> - Enable *
 *    @li <b>1</b> - Disable
 */
#define INCHIP_GEC_CONTROLLER      0x00

/**
 *   @section SB_HPET_TIMER SB_HPET_TIMER
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SB_HPET_TIMER              1

/**
 *   @section SB_GPP_CONTROLLER SB_GPP_CONTROLLER
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SB_GPP_CONTROLLER          1

/**
 *   @section GPP_LINK_CONFIG GPP_LINK_CONFIG
 *    @li  <b>0000</b> - Port ABCD -> 4:0:0:0
 *    @li  <b>0001</b> - N/A
 *    @li  <b>0010</b> - Port ABCD -> 2:2:0:0
 *    @li  <b>0011</b> - Port ABCD -> 2:1:1:0
 *    @li  <b>0100</b> - Port ABCD -> 1:1:1:1
 */
#define GPP_LINK_CONFIG          4

/**
 *   @section SB_GPP_PORT0 SB_GPP_PORT0
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SB_GPP_PORT0               1

/**
 *   @section SB_GPP_PORT1 SB_GPP_PORT1
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SB_GPP_PORT1               1

/**
 *   @section SB_GPP_PORT2 SB_GPP_PORT2
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SB_GPP_PORT2               1

/**
 *   @section SB_GPP_PORT3 SB_GPP_PORT3
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SB_GPP_PORT3               1

/**
 *   @section SB_IR_CONTROLLER
 *    @li <b>00 </b> - disable
 *    @li <b>01 </b> - Rx and Tx0
 *    @li <b>10 </b> - Rx and Tx1
 *    @li <b>11 </b> - Rx and both Tx0,Tx1
 */
#define SB_IR_CONTROLLER           3

/**
 *   @section INCHIP_USB_PHY_POWER_DOWN
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_USB_PHY_POWER_DOWN  0

/**
 *   @section INCHIP_NATIVE_PCIE_SUPPOORT
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_NATIVE_PCIE_SUPPOORT 1

/**
 *   @section INCHIP_NB_SB_GEN2
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_NB_SB_GEN2           1

/**
 *   @section INCHIP_GPP_GEN2
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_GPP_GEN2             1

/**
 *   @section INCHIP_GPP_MEMORY_WRITE_IMPROVE
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_GPP_MEMORY_WRITE_IMPROVE  1

/**
 *   @section INCHIP_GEC_PHY_STATUS
 *    @li <b>0</b> - Gb PHY Mode *
 *    @li <b>1</b> - 100/10 PHY Mode
 */
#define INCHIP_GEC_PHY_STATUS       0

/**
 *   @section INCHIP_GEC_POWER_POLICY
 *    @li <b>0</b> - S3/S5
 *    @li <b>1</b> - S5
 *    @li <b>2</b> - S3
 *    @li <b>3</b> - Never power down *
 */
#define INCHIP_GEC_POWER_POLICY     3

/**
 *   @section INCHIP_GEC_DEBUGBUS
 *    @li <b>0</b> - Disable *
 *    @li <b>1</b> - Enable
 */
#define INCHIP_GEC_DEBUGBUS         0

/**
 *   @section SATA_MAX_GEN2_MODE SATA_MAX_GEN2_MODE
 *    @li <b>0</b> - Disable *
 *    @li <b>1</b> - Enable
 *    SataController Set to Max Gen2 mode
 */
#define SATA_MAX_GEN2_MODE        0

/**
 *   @section INCHIP_SATA_AGGR_LINK_PM_CAP
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable *
 *    SataController Set to aggressive link PM capability
 */
#define INCHIP_SATA_AGGR_LINK_PM_CAP  0

/**
 *   @section INCHIP_SATA_PORT_MULT_CAP
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable *
 *    SataController Set to Port Multiple capability
 */
#define INCHIP_SATA_PORT_MULT_CAP     1

/**
 *   @section INCHIP_SATA_PSC_CAP
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable *
*/
#define INCHIP_SATA_PSC_CAP           0

/**
 *   @section INCHIP_SATA_SSC_CAP
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable *
 */
#define INCHIP_SATA_SSC_CAP           0

/**
 *   @section INCHIP_SATA_CLK_AUTO_OFF
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable *
 */
#define INCHIP_SATA_CLK_AUTO_OFF      1

/**
 *   @section INCHIP_SATA_FIS_BASE_SW
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable *
 */
#define INCHIP_SATA_FIS_BASE_SW       1

/**
 *   @section INCHIP_SATA_CCC_SUPPORT
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable *
 */
#define INCHIP_SATA_CCC_SUPPORT       1

/**
 *   @section INCHIP_SATA_MSI_CAP
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable *
 */
#define INCHIP_SATA_MSI_CAP           1

/**
 *   @section CIMXSB_SATA_TARGET_8DEVICE_CAP
 *    @li <b>0</b> - Disable *
 *    @li <b>1</b> - Enable
 */
#define CIMXSB_SATA_TARGET_8DEVICE_CAP 0

/**
 *   @section SATA_DISABLE_GENERIC_MODE
 *    @li <b>0</b> - Disable *
 *    @li <b>1</b> - Enable
 */
#define SATA_DISABLE_GENERIC_MODE_CAP 0

/**
 *   @section SATA_AHCI_ENCLOSURE_CAP
 *    @li <b>0</b> - Disable *
 *    @li <b>1</b> - Enable
 */
#define SATA_AHCI_ENCLOSURE_CAP 0

/**
 *   @section SataForceRaid (RISD5 mode)
 *    @li <b>0</b> - Disable *
 *    @li <b>1</b> - Enable
 */
#define INCHIP_SATA_FORCE_RAID5    0

/**
 *   @section SATA_GPIO_0_CAP
 *    @li <b>0</b> - Disable *
 *    @li <b>1</b> - Enable
 */
#define SATA_GPIO_0_CAP 0

/**
 *   @section SATA_GPIO_1_CAP
 *    @li <b>0</b> - Disable *
 *    @li <b>1</b> - Enable
 */
#define SATA_GPIO_1_CAP 0

/**
 *   @section SataPhyPllShutDown
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable *
 */
#define SATA_PHY_PLL_SHUTDOWN 1

/**
 *   @section ImcEnableOverWrite
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define IMC_ENABLE_OVER_WRITE 0

/**
 *   @section UsbMsi
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define USB_MSI 0

/**
 *   @section HdAudioMsi
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define HDAUDIO_MSI 0

/**
 *   @section LpcMsi
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define LPC_MSI 0

/**
 *   @section PcibMsi
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define PCIB_MSI 0

/**
 *   @section AbMsi
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define AB_MSI 0

/**
 *   @section GecShadowRomBase
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable *
 */
#define GEC_SHADOWROM_BASE 0xFED61000

/**
 *   @section SataController
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable *
 */
#define SATA_CONTROLLER 1

/**
 *   @section SataIdeCombMdPriSecOpt
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_IDE_COMBMD_PRISEC_OPT 0

/**
 *   @section SataIdeCombinedMode
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_IDECOMBINED_MODE 0

/**
 *   @section sdConfig
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable *
 */
#define SB_SD_CONFIG 1

/**
 *   @section sdSpeed
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable *
 */
#define SB_SD_SPEED 1

/**
 *   @section sdBitwidth
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable *
 */
#define SB_SD_BITWIDTH 1

/**
 *   @section SataDisUnusedIdePChannel
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_DISUNUSED_IDE_P_CHANNEL 0

/**
 *   @section SataDisUnusedIdeSChannel
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_DISUNUSED_IDE_S_CHANNEL 0

/**
 *   @section IdeDisUnusedIdePChannel
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define IDE_DISUNUSED_IDE_P_CHANNEL 0

/**
 *   @section IdeDisUnusedIdeSChannel
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define IDE_DISUNUSED_IDE_S_CHANNEL 0

/**
 *   @section SataEspPort0
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_ESP_PORT0 0

/**
 *   @section  SataEspPort1
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_ESP_PORT1 0

/**
 *   @section  SataEspPort2
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_ESP_PORT2 0

/**
 *   @section  SataEspPort3
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_ESP_PORT3 0

/**
 *   @section  SataEspPort4
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_ESP_PORT4 0

/**
 *   @section  SataEspPort5
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_ESP_PORT5 0

/**
 *   @section  SataEspPort6
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_ESP_PORT6 0

/**
 *   @section  SataEspPort7
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_ESP_PORT7 0

/**
 *   @section  SataPortPower0
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_PORT_POWER_PORT0 0

/**
 *   @section  SataPortPower1
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_PORT_POWER_PORT1 0

/**
 *   @section  SataPortPower2
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_PORT_POWER_PORT2 0

/**
 *   @section  SataPortPower3
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_PORT_POWER_PORT3 0

/**
 *   @section  SataPortPower4
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_PORT_POWER_PORT4 0

/**
 *   @section  SataPortPower5
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_PORT_POWER_PORT5 0

/**
 *   @section  SataPortPower6
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_PORT_POWER_PORT6 0

/**
 *   @section  SataPortPower7
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_PORT_POWER_PORT7 0

/**
 *   @section  SataPortMd0
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_PORTMODE_PORT0 3

/**
 *   @section  SataPortMd1
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_PORTMODE_PORT1 3

/**
 *   @section  SataPortMd2
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_PORTMODE_PORT2 3

/**
 *   @section  SataPortMd3
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_PORTMODE_PORT3 3

/**
 *   @section  SataPortMd4
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_PORTMODE_PORT4 0

/**
 *   @section  SataPortMd5
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_PORTMODE_PORT5 0

/**
 *   @section  SataPortMd6
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_PORTMODE_PORT6 0

/**
 *   @section  SataPortMd7
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_PORTMODE_PORT7 0

/**
 *   @section  SataHotRemovelEnh0
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_HOTREMOVEL_ENH_PORT0 0

/**
 *   @section  SataHotRemovelEnh1
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_HOTREMOVEL_ENH_PORT1 0

/**
 *   @section  SataHotRemovelEnh2
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_HOTREMOVEL_ENH_PORT2 0

/**
 *   @section  SataHotRemovelEnh3
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_HOTREMOVEL_ENH_PORT3 0

/**
 *   @section  SataHotRemovelEnh4
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_HOTREMOVEL_ENH_PORT4 0

/**
 *   @section  SataHotRemovelEnh5
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_HOTREMOVEL_ENH_PORT5 0

/**
 *   @section  SataHotRemovelEnh6
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_HOTREMOVEL_ENH_PORT6 0

/**
 *   @section  SataHotRemovelEnh7
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define SATA_HOTREMOVEL_ENH_PORT7 0

/**
 *   @section  XhciSwitch
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#if IS_ENABLED(CONFIG_ONBOARD_USB30)
	#define SB_XHCI_SWITCH 0
#else
#define SB_XHCI_SWITCH 1
#endif

/**
 *   @section  FrontPanelDetected
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_FRONT_PANEL_DETECTED 0

/**
 *   @section  AnyHT200MhzLink
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_ANY_HT_200MHZ_LINK 0

/**
 *   @section  PcibClkStopOverride
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_PCIB_CLK_STOP_OVERRIDE 0

/**
 *   @section  GppLinkConfig
 * @li  <b>0000</b> - Port ABCD -> 4:0:0:0
 * @li  <b>0001</b> - N/A
 * @li  <b>0010</b> - Port ABCD -> 2:2:0:0
 * @li  <b>0011</b> - Port ABCD -> 2:1:1:0
 * @li  <b>0100</b> - Port ABCD -> 1:1:1:1
 */
#define INCHIP_GPP_LINK_CONFIG 4

/**
 *   @section  GppUnhidePorts
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_GPP_UNHIDE_PORTS 0

/**
 *   @section  GppPortAspm
 * @li   <b>01</b> - Disabled
 * @li   <b>01</b> - L0s
 * @li   <b>10</b> - L1
 * @li   <b>11</b> - L0s + L1
 */
#define INCHIP_GPP_PORT_ASPM 3

/**
 *   @section  GppLaneReversal
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_GPP_LANEREVERSAL 0

/**
 *   @section  AlinkPhyPllPowerDown
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_ALINK_PHY_PLL_POWER_DOWN 1

/**
 *   @section  GppPhyPllPowerDown
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_GPP_PHY_PLL_POWER_DOWN 1

/**
 *   @section  GppDynamicPowerSaving
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_GPP_DYNAMIC_POWER_SAVING 1

/**
 *   @section  PcieAER
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_PCIE_AER 0

/**
 *   @section  PcieRAS
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_PCIE_RAS 0

/**
 *   @section  GppHardwareDowngrade
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_GPP_HARDWARE_DOWNGRADE 0

/**
 *   @section  GppToggleReset
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_GPP_TOGGLE_RESET 0

/**
 *   @section  SbPcieOrderRule
 *    @li <b>00</b> - Disable
 *    @li <b>01</b> - Rule 1
 *    @li <b>10</b> - Rule 2
 */
#define INCHIP_SB_PCIE_ORDER_RULE 2

/**
 *   @section  AcDcMsg
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_ACDC_MSG 0

/**
 *   @section  TimerTickTrack
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_TIMER_TICK_TRACK 1

/**
 *   @section  ClockInterruptTag
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_CLOCK_INTERRUPT_TAG 1

/**
 *   @section  OhciTrafficHanding
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_OHCI_TRAFFIC_HANDING 0

/**
 *   @section  EhciTrafficHanding
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_EHCI_TRAFFIC_HANDING 0

/**
 *   @section  FusionMsgCMultiCore
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_FUSION_MSGC_MULTICORE 0

/**
 *   @section  FusionMsgCStage
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_FUSION_MSGC_STAGE 0

/**
 *   @section  ALinkClkGateOff
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_ALINK_CLK_GATE_OFF 0

/**
 *   @section  BLinkClkGateOff
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_BLINK_CLK_GATE_OFF 0

/**
 *   @section  SlowSpeedABlinkClock
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_SLOW_SPEED_ABLINK_CLOCK 0

/**
 *   @section  AbClockGating
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_AB_CLOCK_GATING 1

/**
 *   @section  GppClockGating
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_GPP_CLOCK_GATING 1

/**
 *   @section  L1TimerOverwrite
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_L1_TIMER_OVERWRITE 0

/**
 *   @section  UmiDynamicSpeedChange
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_UMI_DYNAMIC_SPEED_CHANGE 0

/**
 *   @section  SbAlinkGppTxDriverStrength
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_ALINK_GPP_TX_DRV_STRENGTH 0

/**
 *   @section  StressResetMode
 *    @li <b>0</b> - Disable
 *    @li <b>1</b> - Enable
 */
#define INCHIP_STRESS_RESET_MODE 0

#ifndef SB_PCI_CLOCK_RESERVED
  #define SB_PCI_CLOCK_RESERVED      0x0 //according to CIMx change 0x1F
#endif

/**
 * @brief South Bridge CIMx configuration
 *
 */
void sb900_cimx_config(AMDSBCFG *sb_cfg);
void SbPowerOnInit_Config(AMDSBCFG *sb_cfg);

/**
 * @brief Entry point of Southbridge CIMx callout
 *
 * prototype UINT32 (*SBCIM_HOOK_ENTRY)(UINT32 Param1, UINT32 Param2, void* pConfig)
 *
 * @param[in] func    Southbridge CIMx Function ID.
 * @param[in] data    Southbridge Input Data.
 * @param[in] config  Southbridge configuration structure pointer.
 *
 */
u32 sb900_callout_entry(u32 func, u32 data, void* config);

// definition for function in gpio.c
void gpioEarlyInit (void);

#endif
