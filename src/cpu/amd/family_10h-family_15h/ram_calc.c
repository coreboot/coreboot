/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>

#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>

#include <cbmem.h>

#include "ram_calc.h"

static inline uint8_t is_fam15h(void)
{
	uint8_t fam15h = 0;
	uint32_t family;

	family = cpuid_eax(0x80000001);
	family = ((family & 0xf00000) >> 16) | ((family & 0xf00) >> 8);

	if (family >= 0x6f)
		/* Family 15h or later */
		fam15h = 1;

	return fam15h;
}

uint64_t get_uma_memory_size(uint64_t topmem)
{
	uint64_t uma_size = 0;
	if (IS_ENABLED(CONFIG_GFXUMA)) {
		/* refer to UMA Size Consideration in 780 BDG. */
		if (topmem >= 0x40000000)	/* 1GB and above system memory */
			uma_size = 0x10000000;	/* 256M recommended UMA */

		else if (topmem >= 0x20000000)	/* 512M - 1023M system memory */
			uma_size = 0x8000000;	/* 128M recommended UMA */

		else if (topmem >= 0x10000000)	/* 256M - 511M system memory */
			uma_size = 0x4000000;	/* 64M recommended UMA */
	}

	return uma_size;
}

uint64_t get_cc6_memory_size()
{
	uint8_t enable_cc6;

	uint64_t cc6_size = 0;

	if (is_fam15h()) {
		enable_cc6 = 0;

#ifdef __PRE_RAM__
		if (pci_read_config32(PCI_DEV(0, 0x18, 2), 0x118) & (0x1 << 18))
			enable_cc6 = 1;
#else
		struct device *dct_dev = dev_find_slot(0, PCI_DEVFN(0x18, 2));
		if (pci_read_config32(dct_dev, 0x118) & (0x1 << 18))
			enable_cc6 = 1;
#endif

		if (enable_cc6) {
			/* Preserve the maximum possible CC6 save region
			 * This needs to be kept in sync with
			 * amdfam10_domain_read_resources() in northbridge.c
			 */
			cc6_size = 0x8000000;
		}
	}

	return cc6_size;
}

void *cbmem_top(void)
{
	uint32_t topmem = rdmsr(TOP_MEM).lo;

	return (void *) topmem - get_uma_memory_size(topmem) - get_cc6_memory_size();
}
