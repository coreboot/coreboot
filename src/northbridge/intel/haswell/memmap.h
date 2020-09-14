/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __NORTHBRIDGE_INTEL_HASWELL_MEMMAP_H__
#define __NORTHBRIDGE_INTEL_HASWELL_MEMMAP_H__

/* Intel Enhanced Debug region */
#define IED_SIZE	CONFIG_IED_REGION_SIZE

/* Northbridge BARs */
#define DEFAULT_MCHBAR		0xfed10000		/* 16 KB */
#define DEFAULT_DMIBAR		0xfed18000		/* 4 KB */
#define DEFAULT_EPBAR		0xfed19000		/* 4 KB */

#define GFXVT_BASE_ADDRESS	0xfed90000ULL
#define GFXVT_BASE_SIZE		0x1000

#define VTVC0_BASE_ADDRESS	0xfed91000ULL
#define VTVC0_BASE_SIZE		0x1000

#endif /* __NORTHBRIDGE_INTEL_HASWELL_MEMMAP_H__ */
