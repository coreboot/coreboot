/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_TURIN_POC_IOMAP_H
#define AMD_TURIN_POC_IOMAP_H

#define I2C_MASTER_DEV_COUNT		6
#define I2C_PERIPHERAL_DEV_COUNT	0
#define I2C_CTRLR_COUNT			(I2C_MASTER_DEV_COUNT + I2C_PERIPHERAL_DEV_COUNT)
#define I3C_CTRLR_COUNT			4

#define SPI_BASE_ADDRESS		0xfec10000

/* @Todo : Check these values for Turin */

/* I/O Ranges */
#define ACPI_IO_BASE			0x0400
#define  ACPI_CSTATE_CONTROL		(ACPI_IO_BASE + 0x10)

/* FCH AL2AHB Registers */
#define ALINK_AHB_ADDRESS		0xfedc0000

#define APU_I2C0_BASE			0xfedc2000
#define APU_I2C1_BASE			0xfedc3000
#define APU_I2C2_BASE			0xfedc4000
#define APU_I2C3_BASE			0xfedc5000
#define APU_I2C4_BASE			0xfedc6000
#define APU_I2C5_BASE			0xfedcb000

#define APU_UART0_BASE			0xfedc9000
#define APU_UART1_BASE			0xfedca000
#define APU_UART2_BASE			0xfedce000

#define APU_I3C0_BASE			0xfedd2000
#define APU_I3C1_BASE			0xfedd3000
#define APU_I3C2_BASE			0xfedd4000
#define APU_I3C3_BASE			0xfedd6000

#endif /* AMD_TURIN_POC_IOMAP_H */
