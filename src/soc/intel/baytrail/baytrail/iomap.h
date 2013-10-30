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


/* Memory Mapped IO bases. */

#define PMC_BASE_ADDRESS		0xfed03000
#define IO_BASE_ADDRESS			0xfed0c000
#define ILB_BASE_ADDRESS		0xfed08000
#define SPI_BASE_ADDRESS		0xfed01000
#define MPHY_BASE_ADDRESS		0xfef00000
#define PUNIT_BASE_ADDRESS		0xfed05000
#define RCBA_BASE_ADDRESS		0xfed1c000
#define HPET_BASE_ADDRESS		0xfed00000

/* IO Port base */
#define ACPI_BASE_ADDRESS		0x0400
#define GPIO_BASE_ADDRESS		0x0500
#define SMBUS_BASE_ADDRESS		0xefa0

#endif /* _BAYTRAIL_IOMAP_H_ */
