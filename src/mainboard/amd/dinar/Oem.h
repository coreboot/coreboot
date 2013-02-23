/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _AMD_SB_CIMx_OEM_H_
#define _AMD_SB_CIMx_OEM_H_

#define MOVE_PCIEBAR_TO_F0000000

#define LEGACY_FREE                                     0x00

/**
 * PCIEX_BASE_ADDRESS - Define PCIE base address
 *
 * @param[Option]     MOVE_PCIEBAR_TO_F0000000 Set PCIe base address to 0xF7000000
 */
#ifdef  MOVE_PCIEBAR_TO_F0000000
#define PCIEX_BASE_ADDRESS           0xF8000000
#else
#define PCIEX_BASE_ADDRESS           0xE0000000
#endif


#define SMBUS0_BASE_ADDRESS                     0xB00
#define SMBUS1_BASE_ADDRESS                     0xB20
#define SIO_PME_BASE_ADDRESS            0xE00
#define SPI_BASE_ADDRESS                        0xFEC10000

#define WATCHDOG_TIMER_BASE_ADDRESS     0xFEC000F0                      // Watchdog Timer Base Address
#define HPET_BASE_ADDRESS                       0xFED00000                      // HPET Base address

#define PM1_EVT_BLK_ADDRESS                     0x800                           //      AcpiPm1EvtBlkAddr;
#define PM1_CNT_BLK_ADDRESS                     0x804                           //      AcpiPm1CntBlkAddr;
#define PM1_TMR_BLK_ADDRESS                     0x808                           //      AcpiPmTmrBlkAddr;
#define CPU_CNT_BLK_ADDRESS                     0x810                           //      CpuControlBlkAddr;
#define GPE0_BLK_ADDRESS                        0x820                           //  AcpiGpe0BlkAddr;
#define SMI_CMD_PORT                            0xB0                            //      SmiCmdPortAddr;
#define ACPI_PMA_CNT_BLK_ADDRESS        0xFE00                          //      AcpiPmaCntBlkAddr;

#define EC_LDN5_MAILBOX_ADDRESS         0x550
#define EC_LDN5_IRQ                                     0x05
#define EC_LDN9_MAILBOX_ADDRESS         0x3E

#define SATA_IDE_MODE_SSID                      0x43901002
#define SATA_RAID_MODE_SSID                     0x43921002
#define SATA_RAID5_MODE_SSID            0x43931002
#define SATA_AHCI_SSID                          0x43911002
#define OHCI0_SSID                                      0x43971002
#define OHCI1_SSID                                      0x43981002
#define EHCI0_SSID                                      0x43961002
#define OHCI2_SSID                                      0x43971002
#define OHCI3_SSID                                      0x43981002
#define EHCI1_SSID                                      0x43961002
#define OHCI4_SSID                                      0x43991002

#define SMBUS_SSID                                      0x43851002
#define IDE_SSID                                        0x439C1002
#define AZALIA_SSID                                     0x43831002
#define LPC_SSID                                        0x439D1002
#define P2P_SSID                                        0x43841002

#define RESERVED_VALUE                          0x00

#endif //ifndef _AMD_SB_CIMx_OEM_H_
