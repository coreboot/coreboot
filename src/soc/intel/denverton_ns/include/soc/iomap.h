/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2014-2017 Intel Corporation.
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
 */

#ifndef _DENVERTON_NS_IOMAP_H_
#define _DENVERTON_NS_IOMAP_H_

/*
 * Memory Mapped IO bases.
 */

/* Northbridge BARs */
#define DEFAULT_PCIEXBAR CONFIG_MMCONF_BASE_ADDRESS /* 4 KB per PCIe device */
#define DEFAULT_MCHBAR 0xfed10000		    /* 16 KB */

/* Southbridge internal device IO BARs (Set to match FSP settings) */
#define DEFAULT_PMBASE 0x1800
#define DEFAULT_ACPI_BASE DEFAULT_PMBASE
#define ACPI_BASE_ADDRESS DEFAULT_PMBASE
#define DEFAULT_TCO_BASE 0x400

/* Southbridge internal device MEM BARs (Set to match FSP settings) */
#define DEFAULT_PCR_BASE 0xfd000000
#define DEFAULT_PWRM_BASE 0xfe000000
#define DEFAULT_HPET_ADDR CONFIG_HPET_ADDRESS
#define DEFAULT_SPI_BASE 0xfed01000

#endif /* _DENVERTON_NS_IOMAP_H_ */
