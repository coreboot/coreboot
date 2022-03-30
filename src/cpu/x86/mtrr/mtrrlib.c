/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <console/console.h>

/* Get first available variable MTRR.
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

void set_var_mtrr(
	unsigned int reg, unsigned int base, unsigned int size, unsigned int type)
{
	/* Bit 32-35 of MTRRphysMask should be set to 1 */
	/* FIXME: It only support 4G less range */
	msr_t basem, maskm;

	if (!IS_POWER_OF_2(size))
		printk(BIOS_ERR, "MTRR Error: size %#x is not a power of two\n", size);
	if (size < 4 * KiB)
		printk(BIOS_ERR, "MTRR Error: size %#x smaller than 4KiB\n", size);
	if (base % size != 0)
		printk(BIOS_ERR, "MTRR Error: base %#x must be aligned to size %#x\n", base,
		       size);

	basem.lo = base | type;
	basem.hi = 0;
	wrmsr(MTRR_PHYS_BASE(reg), basem);
	maskm.lo = ~(size - 1) | MTRR_PHYS_MASK_VALID;
	maskm.hi = (1 << (cpu_phys_address_size() - 32)) - 1;
	wrmsr(MTRR_PHYS_MASK(reg), maskm);
}

void clear_all_var_mtrr(void)
{
	msr_t mtrr = {0, 0};
	int vcnt;
	int i;

	vcnt = get_var_mtrr_count();

	for (i = 0; i < vcnt; i++) {
		wrmsr(MTRR_PHYS_MASK(i), mtrr);
		wrmsr(MTRR_PHYS_BASE(i), mtrr);
	}
}
