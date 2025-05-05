/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <console/console.h>
#include <commonlib/bsd/helpers.h>
#include <types.h>

/*
 * Get first available variable MTRR.
 * Returns var# if available, else returns -1.
 */
int get_free_var_mtrr(void)
{
	msr_t maskm;
	int vcnt;
	int i;

	vcnt = get_var_mtrr_count();

	/* Identify the first var mtrr which is not valid. */
	for (i = 0; i < vcnt; i++) {
		maskm = rdmsr(MTRR_PHYS_MASK(i));
		if ((maskm.lo & MTRR_PHYS_MASK_VALID) == 0)
			return i;
	}

	/* No free var mtrr. */
	return -1;
}

/*
 * Sets the specified variable MTRR (as per index) with the
 * given base, size, and memory type.
 */
void set_var_mtrr(
	unsigned int index, unsigned int base, unsigned int size,
	unsigned int type)
{
	/* Bit 32-35 of MTRRphysMask should be set to 1 */
	/* FIXME: It only support 4G less range */
	msr_t basem, maskm;

	if (type == MTRR_TYPE_WRBACK && !is_cache_sets_power_of_two() && ENV_CACHE_AS_RAM)
		printk(BIOS_ERR, "MTRR Error: Type %x may not be supported due to NEM limitation\n",
			 type);
	if (!IS_POWER_OF_2(size))
		printk(BIOS_ERR, "MTRR Error: size %#x is not a power of two\n", size);
	if (size < 4 * KiB)
		printk(BIOS_ERR, "MTRR Error: size %#x smaller than 4KiB\n", size);
	if (base % size != 0)
		printk(BIOS_ERR, "MTRR Error: base %#x must be aligned to size %#x\n", base,
		       size);

	basem.lo = base | type;
	basem.hi = 0;
	wrmsr(MTRR_PHYS_BASE(index), basem);
	maskm.lo = ~(size - 1) | MTRR_PHYS_MASK_VALID;
	maskm.hi = (1 << (cpu_phys_address_size() - 32)) - 1;
	wrmsr(MTRR_PHYS_MASK(index), maskm);
}

/* Disables the variable MTRR at the given index by clearing its base and mask MSRs. */
void clear_var_mtrr(int index)
{
	msr_t msr = { .lo = 0, .hi = 0 };

	wrmsr(MTRR_PHYS_BASE(index), msr);
	wrmsr(MTRR_PHYS_MASK(index), msr);
}

/*
 * Acquires a free variable MTRR, configures it with the given base, size, and type.
 * Returns the MTRR index if successful (>=0), or an error code (<0) if acquisition failed.
 */
int acquire_and_configure_mtrr(unsigned int base, unsigned int size, unsigned int type)
{
	int index = get_free_var_mtrr();

	if (index >= 0)
		set_var_mtrr(index, base, size, type);

	return index;
}
