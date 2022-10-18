/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_MORGANA_IOMAP_H
#define AMD_MORGANA_IOMAP_H

#define I2C_MASTER_DEV_COUNT		4
#define I2C_MASTER_START_INDEX		0
#define I2C_PERIPHERAL_DEV_COUNT	0 /* TODO: Only master for now. */
#define I2C_CTRLR_COUNT			(I2C_MASTER_DEV_COUNT + I2C_PERIPHERAL_DEV_COUNT)

#if ENV_X86

/* MMIO Ranges */
/* IO_APIC_ADDR defined in arch/x86	0xfec00000 */
#define GNB_IO_APIC_ADDR		0xfec01000
#define SPI_BASE_ADDRESS		0xfec10000

/* FCH AL2AHB Registers */
#define ALINK_AHB_ADDRESS		0xfedc0000

#define APU_I2C0_BASE			0xfedc2000
#define APU_I2C1_BASE			0xfedc3000
#define APU_I2C2_BASE			0xfedc4000
#define APU_I2C3_BASE			0xfedc5000

#define APU_DMAC0_BASE			0xfedc7000
#define APU_DMAC1_BASE			0xfedc8000
#define APU_UART0_BASE			0xfedc9000
#define APU_UART1_BASE			0xfedca000
#define APU_DMAC2_BASE			0xfedcc000
#define APU_DMAC3_BASE			0xfedcd000
#define APU_UART2_BASE			0xfedce000
#define APU_UART3_BASE			0xfedcf000
#define APU_DMAC4_BASE			0xfedd0000
#define APU_UART4_BASE			0xfedd1000

#define APU_EMMC_BASE			0xfedd5000
#define APU_EMMC_CONFIG_BASE		0xfedd5800

#endif /* ENV_X86 */

#define FLASH_BASE_ADDR			((0xffffffff - CONFIG_ROM_SIZE) + 1)

/* I/O Ranges */
#define ACPI_IO_BASE			0x0400
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

#endif /* AMD_MORGANA_IOMAP_H */
