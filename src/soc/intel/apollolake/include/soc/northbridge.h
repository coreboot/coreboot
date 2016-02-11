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

#endif /* _SOC_APOLLOLAKE_NORTHBRIDGE_H_ */
