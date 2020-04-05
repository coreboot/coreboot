/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _SOC_IOMAP_H_
#define _SOC_IOMAP_H_

#include <commonlib/helpers.h>

/*
 * Memory-mapped I/O registers.
 */
#define PCH_PRESERVED_BASE_ADDRESS      0xfc800000
#define PCH_PRESERVED_BASE_SIZE 0x02000000

#define EARLY_I2C_BASE_ADDRESS	0xfe040000
#define EARLY_I2C_BASE(x)	(EARLY_I2C_BASE_ADDRESS + (0x1000 * (x)))

#define MCH_BASE_ADDRESS	0xfed10000
#define MCH_BASE_SIZE		0x8000

#define DMI_BASE_ADDRESS	0xfed18000
#define DMI_BASE_SIZE		0x1000

#define EP_BASE_ADDRESS		0xfed19000
#define EP_BASE_SIZE		0x1000

#define EDRAM_BASE_ADDRESS	0xfed80000
#define EDRAM_BASE_SIZE		0x4000

#define GDXC_BASE_ADDRESS	0xfed84000
#define GDXC_BASE_SIZE		0x1000

#define GFXVT_BASE_ADDRESS	0xfed90000
#define GFXVT_BASE_SIZE		0x1000

#define VTVC0_BASE_ADDRESS	0xfed91000
#define VTVC0_BASE_SIZE		0x1000

#define HPET_BASE_ADDRESS	0xfed00000

#define PCH_PWRM_BASE_ADDRESS	0xfe000000
#define PCH_PWRM_BASE_SIZE	0x10000

#define SPI_BASE_ADDRESS	0xfe010000
#define EARLY_GSPI_BASE_ADDRESS 0xfe011000

#define GPIO_BASE_SIZE		0x10000

#define HECI1_BASE_ADDRESS	0xfed1a000

#define THERMAL_BASE_ADDRESS	0xfe600000

#define VTD_BASE_ADDRESS	0xFED90000
#define VTD_BASE_SIZE		0x00004000

/* CPU Trace reserved memory size */
#define GDXC_MOT_MEMORY_SIZE	(96*MiB)
#define GDXC_IOT_MEMORY_SIZE	(32*MiB)
#define PSMI_BUFFER_AREA_SIZE	(64*MiB)

/* PTT registers */
#define PTT_TXT_BASE_ADDRESS	0xfed30800
#define PTT_PRESENT		0x00070000

#define ABOVE_4GB_MEM_BASE_ADDRESS	(128ULL * GiB)
#define ABOVE_4GB_MEM_BASE_SIZE	(64ULL * GiB)

/*
 * I/O port address space
 */
#define SMBUS_BASE_ADDRESS	0x0efa0
#define SMBUS_BASE_SIZE		0x20

#define ACPI_BASE_ADDRESS	0x1800
#define ACPI_BASE_SIZE		0x100

#define TCO_BASE_ADDRESS	0x400
#define TCO_BASE_SIZE		0x20

#define P2SB_BAR		CONFIG_PCR_BASE_ADDRESS
#define P2SB_SIZE		(16 * MiB)

#endif
