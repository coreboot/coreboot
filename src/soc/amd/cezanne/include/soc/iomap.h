/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_CEZANNE_IOMAP_H
#define AMD_CEZANNE_IOMAP_H

/* MMIO Ranges */
/* FCH AL2AHB Registers */
#define ALINK_AHB_ADDRESS		0xfedc0000

#define APU_DMAC0_BASE			0xfedc7000
#define APU_DMAC1_BASE			0xfedc8000
#define APU_UART0_BASE			0xfedc9000
#define APU_UART1_BASE			0xfedca000

#define FLASH_BASE_ADDR			((0xffffffff - CONFIG_ROM_SIZE) + 1)

/* I/O Ranges */
#define NCP_ERR				0x00f0
#define ACPI_IO_BASE		0x0400
#define  ACPI_PM_EVT_BLK		(ACPI_IO_BASE + 0x00)
#define   ACPI_PM1_STS			(ACPI_PM_EVT_BLK + 0x00)
#define   ACPI_PM1_EN			(ACPI_PM_EVT_BLK + 0x02)
#define  ACPI_PM1_CNT_BLK		(ACPI_IO_BASE + 0x04)
#define  ACPI_PM_TMR_BLK		(ACPI_IO_BASE + 0x08)
#define  ACPI_CPU_CONTROL		(ACPI_IO_BASE + 0x10)
#define  ACPI_GPE0_BLK			(ACPI_IO_BASE + 0x20)
#define   ACPI_GPE0_STS			(ACPI_GPE0_BLK + 0x00)
#define   ACPI_GPE0_EN			(ACPI_GPE0_BLK + 0x04)
#define SMB_BASE_ADDR			0x0b00

#endif /* AMD_CEZANNE_IOMAP_H */
