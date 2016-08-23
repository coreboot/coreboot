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

#ifndef _SOC_APOLLOLAKE_NORTHBRIDGE_H_
#define _SOC_APOLLOLAKE_NORTHBRIDGE_H_

#define MCHBAR		0x48
#define PCIEXBAR	0x60
#define PCIEX_SIZE	(256 * MiB)

#define BDSM		0xb0	/* Base Data Stolen Memory */
#define BGSM		0xb4	/* Base GTT Stolen Memory */
#define TSEG		0xb8	/* TSEG base */
#define TOLUD		0xbc	/* Top of Low Used Memory */
#define TOUUD		0xa8	/* Top of Upper Usable DRAM */

/* IMR registers are found under MCHBAR. */
#define MCHBAR_IMR0BASE		0x6870
#define MCHBAR_IMR0MASK		0x6874
#define MCH_IMR_PITCH		0x20
#define MCH_NUM_IMRS		20

/* RAPL Package Power Limit register under MCHBAR. */
#define MCHBAR_RAPL_PPL		0x70A8

#endif /* _SOC_APOLLOLAKE_NORTHBRIDGE_H_ */
