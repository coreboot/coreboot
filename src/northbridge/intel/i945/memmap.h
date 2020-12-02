/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef NORTHBRIDGE_INTEL_I945_MEMMAP_H
#define NORTHBRIDGE_INTEL_I945_MEMMAP_H

/* Northbridge BARs */
#define DEFAULT_X60BAR		0xfed13000
#ifndef __ACPI__
#define DEFAULT_MCHBAR		((u8 *)0xfed14000)	/* 16 KB */
#define DEFAULT_DMIBAR		((u8 *)0xfed18000)	/* 4 KB */
#else
#define DEFAULT_MCHBAR		0xfed14000	/* 16 KB */
#define DEFAULT_DMIBAR		0xfed18000	/* 4 KB */
#endif
#define DEFAULT_EPBAR		0xfed19000	/* 4 KB */

#endif
