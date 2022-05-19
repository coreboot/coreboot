/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_APOLLOLAKE_IOMAP_H_
#define _SOC_APOLLOLAKE_IOMAP_H_

#include <commonlib/helpers.h>

#define P2SB_BAR			CONFIG_PCR_BASE_ADDRESS
#define P2SB_SIZE			(16 * MiB)
#define MCH_BASE_ADDRESS		0xfed10000
#define MCH_BASE_SIZE			(32 * KiB)

#define ACPI_BASE_ADDRESS		0x400
#define ACPI_BASE_SIZE			0x100
#define R_ACPI_PM1_TMR			0x8

#define TCO_BASE_ADDRESS	(ACPI_BASE_ADDRESS + 0x60)
#define TCO_BASE_SIZE		0x20

/* CST Range (R/W) IO port block size */
#define PMG_IO_BASE_CST_RNG_BLK_SIZE	0x5
/* ACPI PMIO Offset to C-state register*/
#define ACPI_PMIO_CST_REG	(ACPI_BASE_ADDRESS + 0x14)

/* Accesses to these BARs are hardcoded in FSP */
#define PCH_PWRM_BASE_ADDRESS		0xfe042000
#define PMC_BAR1			0xfe044000
#define PMC_BAR0_SIZE			(8 * KiB)

#define SRAM_BASE_0			0xfe900000
#define SRAM_SIZE_0			(8 * KiB)
#define SRAM_BASE_2			0xfe902000
#define SRAM_SIZE_2			(4 * KiB)
#define SHARED_SRAM_BASE		0xfffe0000
#define SHARED_SRAM_SIZE		(128 * KiB)

#define HECI1_BASE_ADDRESS		0xfed1a000
#define PSF3_BASE_ADDRESS		0x1e00

/* Temporary BAR for SPI until PCI enumeration assigns a BAR in ramstage. */
#define SPI_BASE_ADDRESS		0xfe010000
#define EARLY_GSPI_BASE_ADDRESS		0xfe011000

/* Temporary BAR for early I2C bus access */
#define EARLY_I2C_BASE_ADDRESS		0xfe020000
#define EARLY_I2C_BASE(x)	(EARLY_I2C_BASE_ADDRESS + (0x1000 * (x)))

#endif /* _SOC_APOLLOLAKE_IOMAP_H_ */
