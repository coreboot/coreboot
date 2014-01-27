/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _BAYTRAIL_IOMAP_H_
#define _BAYTRAIL_IOMAP_H_


/*
 * Memory Mapped IO bases.
 */

/* PCI Configuration Space */
#define MCFG_BASE_ADDRESS		CONFIG_MMCONF_BASE_ADDRESS
#define MCFG_BASE_SIZE			0x10000000

/* Transactions in this range will abort */
#define ABORT_BASE_ADDRESS		0xfeb00000
#define ABORT_BASE_SIZE			0x00100000

/* Power Management Controller */
#define PMC_BASE_ADDRESS		0xfed03000
#define PMC_BASE_SIZE			0x400

/* IO Memory */
#define IO_BASE_ADDRESS			0xfed0c000
#define  IO_BASE_OFFSET_GPSCORE		0x0000
#define  IO_BASE_OFFSET_GPNCORE		0x1000
#define  IO_BASE_OFFSET_GPSSUS		0x2000
#define IO_BASE_SIZE			0x4000

/* Intel Legacy Block */
#define ILB_BASE_ADDRESS		0xfed08000
#define ILB_BASE_SIZE			0x400

/* SPI Bus */
#define SPI_BASE_ADDRESS		0xfed01000
#define SPI_BASE_SIZE			0x400

/* MODPHY */
#define MPHY_BASE_ADDRESS		0xfef00000
#define MPHY_BASE_SIZE			0x100000

/* Power Management Unit */
#define PUNIT_BASE_ADDRESS		0xfed05000
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
/* Read Top of Low Memory (BMBOUND) */
uint32_t nc_read_top_of_low_memory(void);
#endif

#endif /* _BAYTRAIL_IOMAP_H_ */
