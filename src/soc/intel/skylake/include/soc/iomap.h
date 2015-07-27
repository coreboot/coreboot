/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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
 * Foundation, Inc.
 */

#ifndef _SOC_IOMAP_H_
#define _SOC_IOMAP_H_

#define MCFG_BASE_ADDRESS	CONFIG_MMCONF_BASE_ADDRESS
#define MCFG_BASE_SIZE		0x4000000

#define PCH_PCR_BASE_ADDRESS	0xfd000000
#define PCH_BCR_BASE_SIZE	0x1000000

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

/* TODO: need to remove RCBA code after ASL clean up */
#define RCBA_BASE_ADDRESS	0xfed1c000
#define RCBA_BASE_SIZE		0x4000

#define HPET_BASE_ADDRESS	0xfed00000

#define PCH_PWRM_BASE_ADDRESS	0xfe000000
#define PCH_PWRM_BASE_SIZE	0x10000

#define ACPI_BASE_ADDRESS	0x1800
#define ACPI_BASE_SIZE		0x100

#define GPIO_BASE_ADDRESS	0x1400
#define GPIO_BASE_SIZE		0x400

#define SMBUS_BASE_ADDRESS	0x0400
#define SMBUS_BASE_SIZE		0x10

#endif
