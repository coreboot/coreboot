/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
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

#ifndef _SOC_APOLLOLAKE_IOMAP_H_
#define _SOC_APOLLOLAKE_IOMAP_H_

#define P2SB_BAR			CONFIG_IOSF_BASE_ADDRESS
#define MCH_BASE_ADDR			0xfed10000
#define MCH_BASE_SIZE			(32 * KiB)

#define ACPI_PMIO_BASE			0x400
#define  R_ACPI_PM1_TMR			0x8

/* Accesses to these BARs are hardcoded in FSP */
#define PMC_BAR0			0xfe042000
#define PMC_BAR1			0xfe044000

#endif /* _SOC_APOLLOLAKE_IOMAP_H_ */
