/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SOC_INTEL_COMMON_BLOCK_SA_H
#define SOC_INTEL_COMMON_BLOCK_SA_H

/* Device 0:0.0 PCI configuration space */

#define MCHBAR          0x48
#define PCIEXBAR        0x60
#define  PCIEXBAR_LENGTH_64MB       2
#define  PCIEXBAR_LENGTH_128MB      1
#define  PCIEXBAR_LENGTH_256MB      0
#define  PCIEXBAR_PCIEXBAREN        (1 << 0)
#define GGC             0x50

#define TOUUD           0xa8    /* Top of Upper Usable DRAM */
#define BDSM            0xb0    /* Base Data Stolen Memory */
#define BGSM            0xb4    /* Base GTT Stolen Memory */
#define TSEG            0xb8    /* TSEG base */
#define TOLUD           0xbc    /* Top of Low Used Memory */

void bootblock_systemagent_early_init(void);

#endif	/* SOC_INTEL_COMMON_BLOCK_SA_H */
