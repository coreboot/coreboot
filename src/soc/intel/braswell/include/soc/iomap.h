/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_IOMAP_H_
#define _SOC_IOMAP_H_

/*
 * Memory Mapped IO bases.
 */

/* Transactions in this range will abort */
#define ABORT_BASE_ADDRESS		0xfeb00000
#define ABORT_BASE_SIZE			0x00100000

/* Power Management Controller */
#define PMC_BASE_ADDRESS		0xfed03000
#define PMC_BASE_SIZE			0x400

/* IO Memory */
#define IO_BASE_ADDRESS			0xfed80000
#define IO_BASE_SIZE			0x40000
#define COMMUNITY_OFFSET_GPSOUTHWEST            0x00000
#define COMMUNITY_OFFSET_GPNORTH                0x08000
#define COMMUNITY_OFFSET_GPEAST                 0x10000
#define COMMUNITY_OFFSET_GPSOUTHEAST            0x18000

/* Intel Legacy Block */
#define ILB_BASE_ADDRESS		0xfed08000
#define ILB_BASE_SIZE			0x2000

/* SPI Bus */
#define SPI_BASE_ADDRESS		0xfed01000
#define SPI_BASE_SIZE			0x400

/* MODPHY */
#define MPHY_BASE_ADDRESS		0xfea00000
#define MPHY_BASE_SIZE			0x100000

/* Power Management Unit */
#define PUNIT_BASE_ADDRESS		0xfed06000
#define PUNIT_BASE_SIZE			0x800

/* Root Complex Base Address */
#define RCBA_BASE_ADDRESS		0xfed1c000
#define RCBA_BASE_SIZE			0x400

/* High Performance Event Timer */
#define HPET_BASE_ADDRESS		0xfed00000
#define HPET_BASE_SIZE			0x400

/* Temporary Base Address */
#define TEMP_BASE_ADDRESS		0xfd000000

/*
 * IO Port bases.
 */
#define ACPI_BASE_ADDRESS		0x0400
#define ACPI_BASE_SIZE			0x80

#define GPIO_BASE_ADDRESS		0x0500
#define GPIO_BASE_SIZE			0x100

#define SMBUS_BASE_ADDRESS		0xefa0

#ifndef __ACPI__
#include <stdint.h>

/* Read Top of Low Memory (BMBOUND) */
uint32_t nc_read_top_of_low_memory(void);
#endif

#endif /* _SOC_IOMAP_H_ */
