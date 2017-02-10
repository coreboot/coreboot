/*
 * This file is part of the coreboot project.
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

#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>

/* Get first available variable MTRR.
 * Returns var# if available, else returns -1.
 */
int get_free_var_mtrr(void)
{
	msr_t msr, maskm;
	int vcnt;
	int i;

	/* Read MTRRCap and get vcnt - variable memory type ranges. */
	msr = rdmsr(MTRR_CAP_MSR);
	vcnt = msr.lo & 0xff;

	/* Identify the first var mtrr which is not valid. */
	for (i = 0; i < vcnt; i++) {
		maskm = rdmsr(MTRR_PHYS_MASK(i));
		if ((maskm.lo & MTRR_PHYS_MASK_VALID) == 0)
			return i;
	}

	/* No free var mtrr. */
	return -1;
}

#ifdef __ROMCC__
static
#endif
void set_var_mtrr(
	unsigned reg, unsigned base, unsigned size, unsigned type)
{
	/* Bit Bit 32-35 of MTRRphysMask should be set to 1 */
	/* FIXME: It only support 4G less range */
	msr_t basem, maskm;
	basem.lo = base | type;
	basem.hi = 0;
	wrmsr(MTRR_PHYS_BASE(reg), basem);
	maskm.lo = ~(size - 1) | MTRR_PHYS_MASK_VALID;
	maskm.hi = (1 << (CONFIG_CPU_ADDR_BITS - 32)) - 1;
	wrmsr(MTRR_PHYS_MASK(reg), maskm);
}
