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

#ifndef BIOS_SIZE
  #define BIOS_SIZE                      0x04   //04 - 1MB
#endif
#ifndef LEGACY_FREE
	#define LEGACY_FREE                    0x00
#endif
//#define ACPI_SLEEP_TRAP                0x01
//#define SPREAD_SPECTRUM_EPROM_LOAD     0x01

/**
 * Module Specific Defines for platform BIOS
 *
 */

/**
 * PCIEX_BASE_ADDRESS - Define PCIE base address
 *
 * @param[Option]     MOVE_PCIEBAR_TO_F0000000 Set PCIe base address to 0xF7000000
 */
#ifdef  MOVE_PCIEBAR_TO_F0000000
  #define PCIEX_BASE_ADDRESS           0xF7000000
#else
  #define PCIEX_BASE_ADDRESS           CONFIG_MMCONF_BASE_ADDRESS
#endif

/**
 * SMBUS0_BASE_ADDRESS - Smbus base address
 *
 */
#ifndef SMBUS0_BASE_ADDRESS
  #define SMBUS0_BASE_ADDRESS          0xB00
#endif

/**
 * SMBUS1_BASE_ADDRESS - Smbus1 (ASF) base address
 *
 */
#ifndef SMBUS1_BASE_ADDRESS
  #define SMBUS1_BASE_ADDRESS          0xB20
#endif

/**
 * GEC_BASE_ADDRESS - Gec Shadow ROM base address
 *
 */
#ifndef GEC_BASE_ADDRESS
  #define GEC_BASE_ADDRESS          0xFED61000
#endif


/**
 * SIO_PME_BASE_ADDRESS - Super IO PME base address
 *
 */
#ifndef SIO_PME_BASE_ADDRESS
  #define SIO_PME_BASE_ADDRESS         0xE00
#endif

/**
 * SIO_HWM_BASE_ADDRESS - Super IO HWM base address
 *
 */
#ifndef SIO_HWM_BASE_ADDRESS
  #define SIO_HWM_BASE_ADDRESS         0x290
#endif

/**
 * SPI_BASE_ADDRESS - SPI controller (ROM) base address
 *
 */
#ifndef SPI_BASE_ADDRESS
  #define SPI_BASE_ADDRESS             0xFEC10000
#endif

/**
 * WATCHDOG_TIMER_BASE_ADDRESS - WATCHDOG timer base address
 *
 */
#ifndef WATCHDOG_TIMER_BASE_ADDRESS
  #define WATCHDOG_TIMER_BASE_ADDRESS  0xFEC000F0        // Watchdog Timer Base Address
#endif

/**
 * HPET_BASE_ADDRESS - HPET base address
 *
 */
#ifndef HPET_BASE_ADDRESS
  #define HPET_BASE_ADDRESS            0xFED00000        // HPET Base address
#endif

/**
 * ALT_ADDR_400 - For some BIOS codebases which use 0x400 as ACPI base address
 *
 */
#ifdef ALT_ADDR_400
  #define ACPI_BLK_BASE                0x400
#else
  #define ACPI_BLK_BASE                0x800
#endif

#define PM1_STATUS_OFFSET              0x00
#define PM1_ENABLE_OFFSET              0x02
#define PM1_CONTROL_OFFSET             0x04
#define PM_TIMER_OFFSET                0x08
#define CPU_CONTROL_OFFSET             0x10
#define EVENT_STATUS_OFFSET            0x20
#define EVENT_ENABLE_OFFSET            0x24

/**
 * PM1_EVT_BLK_ADDRESS - ACPI power management Event Block base address
 *
 */
#define PM1_EVT_BLK_ADDRESS           ACPI_BLK_BASE + PM1_STATUS_OFFSET     // AcpiPm1EvtBlkAddr

/**
 * PM1_CNT_BLK_ADDRESS - ACPI power management Control block base address
 *
 */
#define PM1_CNT_BLK_ADDRESS           ACPI_BLK_BASE + PM1_CONTROL_OFFSET    // AcpiPm1CntBlkAddr

/**
 * PM1_TMR_BLK_ADDRESS - ACPI power management Timer block base address
 *
 */
#define PM1_TMR_BLK_ADDRESS           ACPI_BLK_BASE + PM_TIMER_OFFSET       // AcpiPmTmrBlkAddr

/**
 * CPU_CNT_BLK_ADDRESS - ACPI power management CPU Control block base address
 *
 */
#define CPU_CNT_BLK_ADDRESS           ACPI_BLK_BASE + CPU_CONTROL_OFFSET    // CpuControlBlkAddr

/**
 * GPE0_BLK_ADDRESS - ACPI power management General Purpose Event block base address
 *
 */
#define GPE0_BLK_ADDRESS              ACPI_BLK_BASE + EVENT_STATUS_OFFSET   // AcpiGpe0BlkAddr

/**
 * SMI_CMD_PORT - ACPI SMI Command block base address
 *
 */
#define SMI_CMD_PORT                  0xB0              // SmiCmdPortAddr

/**
 * ACPI_PMA_CNT_BLK_ADDRESS - ACPI power management additional control block base address
 *
 */
#define ACPI_PMA_CNT_BLK_ADDRESS      0xFE00            // AcpiPmaCntBlkAddr

/**
 * SATA_IDE_MODE_SSID - Sata controller IDE mode SSID.
 *    Define value for SSID while SATA controller set to IDE mode.
 */
#ifndef SATA_IDE_MODE_SSID
  #define SATA_IDE_MODE_SSID           0x43901002
#endif

/**
 * SATA_RAID_MODE_SSID - Sata controller RAID mode SSID.
 *    Define value for SSID while SATA controller set to RAID mode.
 */
#ifndef SATA_RAID_MODE_SSID
  #define SATA_RAID_MODE_SSID          0x43921002
#endif

/**
 * SATA_RAID5_MODE_SSID - Sata controller RAID5 mode SSID.
 *    Define value for SSID while SATA controller set to RAID5 mode.
 */
#ifndef SATA_RAID5_MODE_SSID
  #define SATA_RAID5_MODE_SSID         0x43931002
#endif

/**
 * SATA_AHCI_MODE_SSID - Sata controller AHCI mode SSID.
 *    Define value for SSID while SATA controller set to AHCI mode.
 */
#ifndef SATA_AHCI_SSID
  #define SATA_AHCI_SSID               0x43911002
#endif

/**
 * OHCI_SSID - All SB OHCI controllers SSID value.
 *
 */
#ifndef OHCI_SSID
  #define OHCI_SSID                    0x43971002
#endif

/**
 * EHCI_SSID - All SB EHCI controllers SSID value.
 *
 */
#ifndef EHCI_SSID
  #define EHCI_SSID                    0x43961002
#endif

/**
 * OHCI4_SSID - OHCI (USB 1.1 mode *HW force) controllers SSID value.
 *
 */
#ifndef OHCI4_SSID
  #define OHCI4_SSID                   0x43991002
#endif

/**
 * SMBUS_SSID - Smbus controller (South Bridge device 0x14 function 0) SSID value.
 *
 */
#ifndef SMBUS_SSID
  #define SMBUS_SSID                   0x43851002
#endif

/**
 * IDE_SSID - SATA IDE controller (South Bridge device 0x14 function 1) SSID value.
 *
 */
#ifndef IDE_SSID
  #define IDE_SSID                     0x439C1002
#endif

/**
 * AZALIA_SSID - AZALIA controller (South Bridge device 0x14 function 2) SSID value.
 *
 */
#ifndef AZALIA_SSID
  #define AZALIA_SSID                  0x43831002
#endif

/**
 * LPC_SSID - LPC controller (South Bridge device 0x14 function 3) SSID value.
 *
 */
#ifndef LPC_SSID
  #define LPC_SSID                     0x439D1002
#endif

/**
 * PCIB_SSID - PCIB controller (South Bridge device 0x14 function 4) SSID value.
 *
 */
#ifndef PCIB_SSID
  #define PCIB_SSID                    0x43841002
#endif

/**
 * USB_PLL_Voltage - CG2 Clock voltage setting.
 *
 */
#ifndef USB_PLL_Voltage
  #define USB_PLL_Voltage              0x10
#endif

/**
 * Spread_Spectrum_Type
 *
 *  - 0 : Normal platform
 *  - 1 : Ontario platform
 */
#ifndef Spread_Spectrum_Type
  #define Spread_Spectrum_Type         0x00
#endif

/**
 *    Imc Enable OverWrite
 *    2 - by default strapping
 *    1 - On
 *    0 - Off
 */
#ifndef IMC_ENABLE_OVER_WRITE
  #define IMC_ENABLE_OVER_WRITE        0x02
#endif
