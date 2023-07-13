/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_GENOA_IOMAP_H
#define AMD_GENOA_IOMAP_H

#define SPI_BASE_ADDRESS		0xfec10000

/* @Todo : Check these values for Genoa */

/* I/O Ranges */
#define ACPI_IO_BASE			0x0400
#define  ACPI_CSTATE_CONTROL		(ACPI_IO_BASE + 0x10)

/* FCH AL2AHB Registers */
#define ALINK_AHB_ADDRESS		0xfedc0000

#define APU_I2C0_BASE			0xfedc2000
#define APU_I2C1_BASE			0xfedc3000
#define APU_I2C2_BASE			0xfedc4000
#define APU_I2C3_BASE			0xfedc5000

#define APU_UART0_BASE			0xfedc9000
#define APU_UART1_BASE			0xfedca000
#define APU_UART2_BASE			0xfedce000

#endif /* AMD_GENOA_IOMAP_H */
