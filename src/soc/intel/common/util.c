/*
 * This file is part of the coreboot project.
 *
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/cpu.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <soc/intel/common/util.h>
#include <stddef.h>

void soc_display_upd_value(const char *name, uint32_t size, uint64_t old,
	uint64_t new)
{
	if (old == new) {
		switch (size) {
		case 1:
			printk(BIOS_SPEW, "  0x%02llx: %s\n", new, name);
			break;

		case 2:
			printk(BIOS_SPEW, "  0x%04llx: %s\n", new, name);
			break;

		case 4:
			printk(BIOS_SPEW, "  0x%08llx: %s\n", new, name);
			break;

		case 8:
			printk(BIOS_SPEW, "  0x%016llx: %s\n", new, name);
			break;
		}
	} else {
		switch (size) {
		case 1:
			printk(BIOS_SPEW, "  0x%02llx --> 0x%02llx: %s\n", old,
				new, name);
			break;

		case 2:
			printk(BIOS_SPEW, "  0x%04llx --> 0x%04llx: %s\n", old,
				new, name);
			break;

		case 4:
			printk(BIOS_SPEW, "  0x%08llx --> 0x%08llx: %s\n", old,
				new, name);
			break;

		case 8:
			printk(BIOS_SPEW, "  0x%016llx --> 0x%016llx: %s\n",
				old, new, name);
			break;
		}
	}
}

uint32_t soc_get_variable_mtrr_count(uint64_t *msr)
{
	union {
		uint64_t u64;
		msr_t s;
	} mttrcap;

	mttrcap.s = rdmsr(MTRRcap_MSR);
	if (msr != NULL)
		*msr = mttrcap.u64;
	return mttrcap.u64 & MTRRcapVcnt;
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

	type = msr & MTRRdefTypeType;
	base_address = starting_address;
	next_address = base_address;
	for (index = 0; index < 64; index += 8) {
		next_address = starting_address + (memory_size *
			((index >> 3) + 1));
		next_type = (msr >> index) & MTRRdefTypeType;
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

	msr.s = rdmsr(MTRRfix64K_00000_MSR);
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
		(msr & MTRRcapSmrr) ? "SMRR, " : "",
		(msr & MTRRcapWc) ? "WC, " : "",
		(msr & MTRRcapFix) ? "FIX, " : "",
		variable_mtrrs);
	return variable_mtrrs;
}

static void soc_display_mtrr_def_type(void)
{
	union {
		uint64_t u64;
		msr_t s;
	} msr;

	msr.s = rdmsr(MTRRdefType_MSR);
	printk(BIOS_DEBUG, "0x%016llx: IA32_MTRR_DEF_TYPE:%s%s %s\n",
		msr.u64,
		(msr.u64 & MTRRdefTypeEn) ? " E," : "",
		(msr.u64 & MTRRdefTypeFixEn) ? " FE," : "",
		soc_display_mtrr_type((uint32_t)(msr.u64 & MTRRdefTypeType)));
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
	if (msr_m.u64 & MTRRphysMaskValid) {
		base_address = (msr_a.u64 & 0xfffffffffffff000ULL)
			& address_mask;
		printk(BIOS_DEBUG,
			"0x%016llx: PHYBASE%d: Address = 0x%016llx, %s\n",
			msr_a.u64, index, base_address,
			soc_display_mtrr_type(msr_a.u64 & MTRRdefTypeType));
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
		soc_display_16k_mtrr(MTRRfix16K_80000_MSR, 0x80000,
			"IA32_MTRR_FIX16K_80000");
		soc_display_16k_mtrr(MTRRfix16K_A0000_MSR, 0xa0000,
			"IA32_MTRR_FIX16K_A0000");
		soc_display_4k_mtrr(MTRRfix4K_C0000_MSR, 0xc0000,
			"IA32_MTRR_FIX4K_C0000");
		soc_display_4k_mtrr(MTRRfix4K_C8000_MSR, 0xc8000,
			"IA32_MTRR_FIX4K_C8000");
		soc_display_4k_mtrr(MTRRfix4K_D0000_MSR, 0xd0000,
			"IA32_MTRR_FIX4K_D0000");
		soc_display_4k_mtrr(MTRRfix4K_D8000_MSR, 0xd8000,
			"IA32_MTRR_FIX4K_D8000");
		soc_display_4k_mtrr(MTRRfix4K_E0000_MSR, 0xe0000,
			"IA32_MTRR_FIX4K_E0000");
		soc_display_4k_mtrr(MTRRfix4K_E8000_MSR, 0xe8000,
			"IA32_MTRR_FIX4K_E8000");
		soc_display_4k_mtrr(MTRRfix4K_F0000_MSR, 0xf0000,
			"IA32_MTRR_FIX4K_F0000");
		soc_display_4k_mtrr(MTRRfix4K_F8000_MSR, 0xf8000,
			"IA32_MTRR_FIX4K_F8000");
		address_bits = cpu_phys_address_size();
		address_mask = (1ULL << address_bits) - 1;

		/* Display the variable MTRRs */
		for (i = 0; i < variable_mtrrs; i++)
			soc_display_variable_mtrr(MTRRphysBase_MSR(i), i,
				address_mask);
	}
}
