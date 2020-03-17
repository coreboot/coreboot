/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_PMC_H_
#define _SOC_PMC_H_

/* PCI Configuration Space (D31:F2): PMC */
#define PMC_ACPI_CNT            0x44

#define  SCI_IRQ_SEL            (7 << 0)
#define  SCIS_IRQ9              0
#define  SCIS_IRQ10             1
#define  SCIS_IRQ11             2
#define  SCIS_IRQ20             4
#define  SCIS_IRQ21             5
#define  SCIS_IRQ22             6
#define  SCIS_IRQ23             7

#define SCI_IRQ_ADJUST          0

#endif
