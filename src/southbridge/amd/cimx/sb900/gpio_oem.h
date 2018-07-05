/*
 * This file is part of the coreboot project.
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

#ifndef _GPIO_OEM_H_
#define _GPIO_OEM_H_

/* Hudson-2 ACPI PmIO Space Define */
#define SB_ACPI_BASE_ADDRESS              0x0400
#define VACPI_MMIO_BASE  ((u8 *)0xFED80000)
#define SB_CFG_BASE     0x000   // DWORD
#define GPIO_BASE       0x100   // BYTE
#define SMI_BASE        0x200   // DWORD
#define PMIO_BASE       0x300   // DWORD
#define PMIO2_BASE      0x400   // BYTE
#define BIOS_RAM_BASE   0x500   // BYTE
#define CMOS_RAM_BASE   0x600   // BYTE
#define CMOS_BASE       0x700   // BYTE
#define ASF_BASE        0x900   // DWORD
#define SMBUS_BASE      0xA00   // DWORD
#define WATCHDOG_BASE   0xB00   // ??
#define HPET_BASE       0xC00   // DWORD
#define IOMUX_BASE      0xD00   // BYTE
#define MISC_BASE       0xE00
#define SERIAL_DEBUG_BASE  0x1000
#define GFX_DAC_BASE       0x1400
#define CEC_BASE           0x1800
#define XHCI_BASE          0x1C00
#define ACPI_SMI_DATA_PORT                0xB1
#define R_SB_ACPI_PM1_STATUS              0x00
#define R_SB_ACPI_PM1_ENABLE              0x02
#define R_SB_ACPI_PM_CONTROL              0x04
#define R_SB_ACPI_EVENT_STATUS            0x20
#define R_SB_ACPI_EVENT_ENABLE            0x24
#define   B_PWR_BTN_STATUS                BIT8
#define   B_WAKEUP_STATUS                 BIT15
#define   B_SCI_EN                        BIT0
#define SB_PM_INDEX_PORT                  0xCD6
#define SB_PM_DATA_PORT                   0xCD7
#define SB_PMIOA_REG24          0x24        //  AcpiMmioEn
#define MmioAddress( BaseAddr, Register ) \
  ( (UINTN)BaseAddr + \
    (UINTN)(Register) \
  )
#define Mmio32Ptr( BaseAddr, Register ) \
  ( (volatile UINT32 *)MmioAddress( BaseAddr, Register ) )
#define Mmio32( BaseAddr, Register ) \
  *Mmio32Ptr( BaseAddr, Register )

#define SB_GPIO_REG01   1
#define SB_GPIO_REG02   2
#define SB_GPIO_REG15   15
#define SB_GPIO_REG24   24
#define SB_GPIO_REG25   25
#define SB_GPIO_REG27   27

#endif
