/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_PICASSO_IOMAP_H
#define AMD_PICASSO_IOMAP_H

/* MMIO Ranges */
/* IO_APIC_ADDR defined in arch/x86	0xfec00000 */
#define GNB_IO_APIC_ADDR		0xfec01000
#define SPI_BASE_ADDRESS		0xfec10000

#if CONFIG(HPET_ADDRESS_OVERRIDE)
#error HPET address override is not allowed and must be fixed at 0xfed00000
#endif
#define HPET_BASE_ADDRESS		0xfed00000

/* FCH AL2AHB Registers */
#define ALINK_AHB_ADDRESS		0xfedc0000
#define AL2AHB_CONTROL_CLK_OFFSET	0x10
#define   AL2AHB_CLK_GATE_EN		(1 << 1)
#define AL2AHB_CONTROL_HCLK_OFFSET	0x30
#define   AL2AHB_HCLK_GATE_EN		(1 << 1)

/* Reserved				0xfecd1000-0xfedc3fff */

/*
 * Picasso/Dali have I2C0 and I2C1 wired to the Sensor Fusion Hub (SFH/MP2).
 * The controllers are not directly accessible via the x86.
 *
 * On Pollock, I2C0 and I2C1 are routed to the x86 domain, but unfortunately the
 * interrupts weren't. This effectively makes the I2C controllers useless, so we
 * pretend they don't exist.
 *
 * We want the device tree numbering to match the I2C numbers, so we allocate
 * I2C0 and I2C1 even though they are not functional.
 */
#define I2C_MASTER_DEV_COUNT		4
#define I2C_MASTER_START_INDEX		2
#define I2C_SLAVE_DEV_COUNT		1

#define APU_I2C2_BASE			0xfedc4000
#define APU_I2C3_BASE			0xfedc5000
#define APU_I2C4_BASE			0xfedc6000

/* I2C parameters for lpc_read_resources */
#define I2C_BASE_ADDRESS		APU_I2C2_BASE
#define I2C_DEVICE_SIZE			0x00001000
#define I2C_DEVICE_COUNT		(I2C_MASTER_DEV_COUNT \
					 - I2C_MASTER_START_INDEX)

#define APU_DMAC0_BASE			0xfedc7000
#define APU_DMAC1_BASE			0xfedc8000
#define APU_UART0_BASE			0xfedc9000
#define APU_UART1_BASE			0xfedca000
/* Reserved				0xfedcb000 */
#define APU_DMAC2_BASE			0xfedcc000
#define APU_DMAC3_BASE			0xfedcd000
#define APU_UART2_BASE			0xfedce000
#define APU_UART3_BASE			0xfedcf000
/* Reserved				0xfedd0000-0xfedd4fff */
#define APU_EMMC_BASE			0xfedd5000
#define APU_EMMC_CONFIG_BASE		0xfedd5800

#define FLASH_BASE_ADDR			((0xffffffff - CONFIG_ROM_SIZE) + 1)

/* I/O Ranges */
#define ACPI_SMI_CTL_PORT		0xb2
#define PICASSO_ACPI_IO_BASE	CONFIG_PICASSO_ACPI_IO_BASE
#define  ACPI_PM_EVT_BLK	(PICASSO_ACPI_IO_BASE + 0x00)     /* 4 bytes */
#define  ACPI_PM1_STS		(ACPI_PM_EVT_BLK + 0x00)	  /* 2 bytes */
#define  ACPI_PM1_EN		(ACPI_PM_EVT_BLK + 0x02)	  /* 2 bytes */
#define  ACPI_PM1_CNT_BLK	(PICASSO_ACPI_IO_BASE + 0x04)     /* 2 bytes */
#define  ACPI_PM_TMR_BLK	(PICASSO_ACPI_IO_BASE + 0x08)     /* 4 bytes */
#define  ACPI_CPU_CONTROL	(PICASSO_ACPI_IO_BASE + 0x13)
/* doc says 0x14 for GPE0_BLK but FT5 only works with 0x20 */
#define  ACPI_GPE0_BLK		(PICASSO_ACPI_IO_BASE + 0x20)     /* 8 bytes */
#define  ACPI_GPE0_STS		(ACPI_GPE0_BLK + 0x00)		  /* 4 bytes */
#define  ACPI_GPE0_EN		(ACPI_GPE0_BLK + 0x04)		  /* 4 bytes */
#define NCP_ERR				0xf0
#define SMB_BASE_ADDR			0xb00
#define PM2_INDEX			0xcd0
#define PM2_DATA			0xcd1
#define BIOSRAM_INDEX			0xcd4
#define BIOSRAM_DATA			0xcd5
#define AB_INDX				0xcd8
#define AB_DATA				(AB_INDX+4)
#define SYS_RESET			0xcf9

#endif /* AMD_PICASSO_IOMAP_H */
