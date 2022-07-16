/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_STONEYRIDGE_IOMAP_H
#define AMD_STONEYRIDGE_IOMAP_H

/* MMIO Ranges */
#define PSP_MAILBOX_BAR3_BASE		0xf0a00000
#define SPI_BASE_ADDRESS		0xfec10000
#define IO_APIC2_ADDR			0xfec20000

#define ALINK_AHB_ADDRESS		0xfedc0000

/* I2C fixed address */
#define APU_I2C0_BASE			0xfedc2000
#define APU_I2C1_BASE			0xfedc3000
#define APU_I2C2_BASE			0xfedc4000
#define APU_I2C3_BASE			0xfedc5000

#define APU_UART0_BASE			0xfedc6000
#define APU_UART1_BASE			0xfedc8000

#define FLASH_BASE_ADDR			((0xffffffff - CONFIG_ROM_SIZE) + 1)

/* I/O Ranges */
#define ACPI_IO_BASE			0x400
#define  ACPI_PM_EVT_BLK		(ACPI_IO_BASE + 0x00)		/* 4 bytes */
#define  ACPI_PM1_STS			(ACPI_PM_EVT_BLK + 0x00)	/* 2 bytes */
#define  ACPI_PM1_EN			(ACPI_PM_EVT_BLK + 0x02)	/* 2 bytes */
#define  ACPI_PM1_CNT_BLK		(ACPI_IO_BASE + 0x04)		/* 2 bytes */
#define  ACPI_CPU_CONTROL		(ACPI_IO_BASE + 0x08)		/* 6 bytes */
#define  ACPI_GPE0_BLK			(ACPI_IO_BASE + 0x10)		/* 8 bytes */
#define  ACPI_GPE0_STS			(ACPI_GPE0_BLK + 0x00)		/* 4 bytes */
#define  ACPI_GPE0_EN			(ACPI_GPE0_BLK + 0x04)		/* 4 bytes */
#define  ACPI_PM_TMR_BLK		(ACPI_IO_BASE + 0x18)		/* 4 bytes */
#define SMB_BASE_ADDR			0xb00
#define PM2_INDEX			0xcd0
#define PM2_DATA			0xcd1
#define BIOSRAM_INDEX			0xcd4
#define BIOSRAM_DATA			0xcd5
#define AB_INDX				0xcd8
#define AB_DATA				(AB_INDX + 4)

#endif /* AMD_STONEYRIDGE_IOMAP_H */
