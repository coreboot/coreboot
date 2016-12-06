/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corporation.
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

#include <arch/cpu.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <soc/intel/common/util.h>
#include <stddef.h>

uint32_t soc_get_variable_mtrr_count(uint64_t *msr)
{
	union {
		uint64_t u64;
		msr_t s;
	} mtrrcap;

	mtrrcap.s = rdmsr(MTRR_CAP_MSR);
	if (msr != NULL)
		*msr = mtrrcap.u64;
	return mtrrcap.u64 & MTRR_CAP_VCNT;
}

static const char *soc_display_mtrr_type(uint32_t type)
{
	switch (type) {
	default: return "reserved";
	case 0: return "UC";
	case 1: return "WC";
	case 4: return "WT";
	case 5: return "WP";
	case 6: return "WB";
	case 7: return "UC-";
	}
}

static void soc_display_mtrr_fixed_types(uint64_t msr,
	uint32_t starting_address, uint32_t memory_size)
{
	uint32_t base_address;
	uint32_t index;
	uint32_t next_address;
	uint32_t next_type;
	uint32_t type;

	type = msr & MTRR_DEF_TYPE_MASK;
	base_address = starting_address;
	next_address = base_address;
	for (index = 0; index < 64; index += 8) {
		next_address = starting_address + (memory_size *
			((index >> 3) + 1));
		next_type = (msr >> index) & MTRR_DEF_TYPE_MASK;
		if (next_type != type) {
			printk(BIOS_DEBUG, "    0x%08x - 0x%08x: %s\n",
				base_address, next_address - 1,
				soc_display_mtrr_type(type));
			base_address = next_address;
			type = next_type;
		}
	}
	if (base_address != next_address)
		printk(BIOS_DEBUG, "    0x%08x - 0x%08x: %s\n",
			base_address, next_address - 1,
			soc_display_mtrr_type(type));
}

static void soc_display_4k_mtrr(uint32_t msr_reg, uint32_t starting_address,
	const char *name)
{
	union {
		uint64_t u64;
		msr_t s;
	} msr;

	msr.s = rdmsr(msr_reg);
	printk(BIOS_DEBUG, "0x%016llx: %s\n", msr.u64, name);
	soc_display_mtrr_fixed_types(msr.u64, starting_address, 0x1000);
}

static void soc_display_16k_mtrr(uint32_t msr_reg, uint32_t starting_address,
	const char *name)
{
	union {
		uint64_t u64;
		msr_t s;
	} msr;

	msr.s = rdmsr(msr_reg);
	printk(BIOS_DEBUG, "0x%016llx: %s\n", msr.u64, name);
	soc_display_mtrr_fixed_types(msr.u64, starting_address, 0x4000);
}

static void soc_display_64k_mtrr(void)
{
	union {
		uint64_t u64;
		msr_t s;
	} msr;

	msr.s = rdmsr(MTRR_FIX_64K_00000);
	printk(BIOS_DEBUG, "0x%016llx: IA32_MTRR_FIX64K_00000\n", msr.u64);
	soc_display_mtrr_fixed_types(msr.u64, 0, 0x10000);
}

static uint32_t soc_display_mtrrcap(void)
{
	uint64_t msr;
	uint32_t variable_mtrrs;

	variable_mtrrs = soc_get_variable_mtrr_count(&msr);
	printk(BIOS_DEBUG,
		"0x%016llx: IA32_MTRRCAP: %s%s%s%d variable MTRRs\n",
		msr,
		(msr & MTRR_CAP_SMRR) ? "SMRR, " : "",
		(msr & MTRR_CAP_WC) ? "WC, " : "",
		(msr & MTRR_CAP_FIX) ? "FIX, " : "",
		variable_mtrrs);
	return variable_mtrrs;
}

static void soc_display_mtrr_def_type(void)
{
	union {
		uint64_t u64;
		msr_t s;
	} msr;

	msr.s = rdmsr(MTRR_DEF_TYPE_MSR);
	printk(BIOS_DEBUG, "0x%016llx: IA32_MTRR_DEF_TYPE:%s%s %s\n",
		msr.u64,
		(msr.u64 & MTRR_DEF_TYPE_EN) ? " E," : "",
		(msr.u64 & MTRR_DEF_TYPE_FIX_EN) ? " FE," : "",
		soc_display_mtrr_type((uint32_t)(msr.u64 &
			MTRR_DEF_TYPE_MASK)));
}

static void soc_display_variable_mtrr(uint32_t msr_reg, int index,
	uint64_t address_mask)
{
	uint64_t base_address;
	uint64_t length;
	uint64_t mask;
	union {
		uint64_t u64;
		msr_t s;
	} msr_a;
	union {
		uint64_t u64;
		msr_t s;
	} msr_m;

	msr_a.s = rdmsr(msr_reg);
	msr_m.s = rdmsr(msr_reg + 1);
	if (msr_m.u64 & MTRR_PHYS_MASK_VALID) {
		base_address = (msr_a.u64 & 0xfffffffffffff000ULL)
			& address_mask;
		printk(BIOS_DEBUG,
			"0x%016llx: PHYBASE%d: Address = 0x%016llx, %s\n",
			msr_a.u64, index, base_address,
			soc_display_mtrr_type(msr_a.u64 & MTRR_DEF_TYPE_MASK));
		mask = (msr_m.u64 & 0xfffffffffffff000ULL) & address_mask;
		length = (~mask & address_mask) + 1;
		printk(BIOS_DEBUG,
			"0x%016llx: PHYMASK%d: Length  = 0x%016llx, Valid\n",
			msr_m.u64, index, length);
	} else {
		printk(BIOS_DEBUG, "0x%016llx: PHYBASE%d\n", msr_a.u64, index);
		printk(BIOS_DEBUG, "0x%016llx: PHYMASK%d: Disabled\n",
			msr_m.u64, index);
	}
}

asmlinkage void soc_display_mtrrs(void)
{
	if (IS_ENABLED(CONFIG_DISPLAY_MTRRS)) {
		uint32_t address_bits;
		uint64_t address_mask;
		int i;
		int variable_mtrrs;

		/* Display the fixed MTRRs */
		variable_mtrrs = soc_display_mtrrcap();
		soc_display_mtrr_def_type();
		soc_display_64k_mtrr();
		soc_display_16k_mtrr(MTRR_FIX_16K_80000, 0x80000,
			"IA32_MTRR_FIX16K_80000");
		soc_display_16k_mtrr(MTRR_FIX_16K_A0000, 0xa0000,
			"IA32_MTRR_FIX16K_A0000");
		soc_display_4k_mtrr(MTRR_FIX_4K_C0000, 0xc0000,
			"IA32_MTRR_FIX4K_C0000");
		soc_display_4k_mtrr(MTRR_FIX_4K_C8000, 0xc8000,
			"IA32_MTRR_FIX4K_C8000");
		soc_display_4k_mtrr(MTRR_FIX_4K_D0000, 0xd0000,
			"IA32_MTRR_FIX4K_D0000");
		soc_display_4k_mtrr(MTRR_FIX_4K_D8000, 0xd8000,
			"IA32_MTRR_FIX4K_D8000");
		soc_display_4k_mtrr(MTRR_FIX_4K_E0000, 0xe0000,
			"IA32_MTRR_FIX4K_E0000");
		soc_display_4k_mtrr(MTRR_FIX_4K_E8000, 0xe8000,
			"IA32_MTRR_FIX4K_E8000");
		soc_display_4k_mtrr(MTRR_FIX_4K_F0000, 0xf0000,
			"IA32_MTRR_FIX4K_F0000");
		soc_display_4k_mtrr(MTRR_FIX_4K_F8000, 0xf8000,
			"IA32_MTRR_FIX4K_F8000");
		address_bits = cpu_phys_address_size();
		address_mask = (1ULL << address_bits) - 1;

		/* Display the variable MTRRs */
		for (i = 0; i < variable_mtrrs; i++)
			soc_display_variable_mtrr(MTRR_PHYS_BASE(i), i,
				address_mask);
	}
}
