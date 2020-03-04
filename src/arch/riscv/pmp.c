/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/encoding.h>
#include <stdint.h>
#include <arch/pmp.h>
#include <console/console.h>
#include <commonlib/helpers.h>

#define GRANULE		(1 << PMP_SHIFT)

/*
 * This structure is used to temporarily record PMP
 * configuration information.
 */
typedef struct {
	/* used to record the value of pmpcfg[i] */
	uintptr_t cfg;
	/*
	 * When generating a TOR type configuration,
	 * the previous entry needs to record the starting address.
	 * used to record the value of pmpaddr[i - 1]
	 */
	uintptr_t previous_address;
	/* used to record the value of pmpaddr[i] */
	uintptr_t address;
} pmpcfg_t;

/* This variable is used to record which entries have been used. */
static uintptr_t pmp_entry_used_mask;

/* helper function used to read pmpcfg[idx] */
static uintptr_t read_pmpcfg(int idx)
{
#if __riscv_xlen == 32
	int shift = 8 * (idx & 3);
	switch (idx >> 2) {
	case 0:
		return (read_csr(pmpcfg0) >> shift) & 0xff;
	case 1:
		return (read_csr(pmpcfg1) >> shift) & 0xff;
	case 2:
		return (read_csr(pmpcfg2) >> shift) & 0xff;
	case 3:
		return (read_csr(pmpcfg3) >> shift) & 0xff;
	}
#elif __riscv_xlen == 64
	int shift = 8 * (idx & 7);
	switch (idx >> 3) {
	case 0:
		return (read_csr(pmpcfg0) >> shift) & 0xff;
	case 1:
		return (read_csr(pmpcfg2) >> shift) & 0xff;
	}
#endif
	return -1;
}

/* helper function used to write pmpcfg[idx] */
static void write_pmpcfg(int idx, uintptr_t cfg)
{
	uintptr_t old;
	uintptr_t new;
#if __riscv_xlen == 32
	int shift = 8 * (idx & 3);
	switch (idx >> 2) {
	case 0:
		old = read_csr(pmpcfg0);
		new = (old & ~((uintptr_t)0xff << shift))
			| ((cfg & 0xff) << shift);
		write_csr(pmpcfg0, new);
		break;
	case 1:
		old = read_csr(pmpcfg1);
		new = (old & ~((uintptr_t)0xff << shift))
			| ((cfg & 0xff) << shift);
		write_csr(pmpcfg1, new);
		break;
	case 2:
		old = read_csr(pmpcfg2);
		new = (old & ~((uintptr_t)0xff << shift))
			| ((cfg & 0xff) << shift);
		write_csr(pmpcfg2, new);
		break;
	case 3:
		old = read_csr(pmpcfg3);
		new = (old & ~((uintptr_t)0xff << shift))
			| ((cfg & 0xff) << shift);
		write_csr(pmpcfg3, new);
		break;
	}
#elif __riscv_xlen == 64
	int shift = 8 * (idx & 7);
	switch (idx >> 3) {
	case 0:
		old = read_csr(pmpcfg0);
		new = (old & ~((uintptr_t)0xff << shift))
			| ((cfg & 0xff) << shift);
		write_csr(pmpcfg0, new);
		break;
	case 1:
		old = read_csr(pmpcfg2);
		new = (old & ~((uintptr_t)0xff << shift))
			| ((cfg & 0xff) << shift);
		write_csr(pmpcfg2, new);
		break;
	}
#endif
	if (read_pmpcfg(idx) != cfg)
		die("write pmpcfg failure!");
}

/* helper function used to read pmpaddr[idx] */
static uintptr_t read_pmpaddr(int idx)
{
	switch (idx) {
	case 0:
		return read_csr(pmpaddr0);
	case 1:
		return read_csr(pmpaddr1);
	case 2:
		return read_csr(pmpaddr2);
	case 3:
		return read_csr(pmpaddr3);
	case 4:
		return read_csr(pmpaddr4);
	case 5:
		return read_csr(pmpaddr5);
	case 6:
		return read_csr(pmpaddr6);
	case 7:
		return read_csr(pmpaddr7);
	case 8:
		return read_csr(pmpaddr8);
	case 9:
		return read_csr(pmpaddr9);
	case 10:
		return read_csr(pmpaddr10);
	case 11:
		return read_csr(pmpaddr11);
	case 12:
		return read_csr(pmpaddr12);
	case 13:
		return read_csr(pmpaddr13);
	case 14:
		return read_csr(pmpaddr14);
	case 15:
		return read_csr(pmpaddr15);
	}
	return -1;
}

/* helper function used to write pmpaddr[idx] */
static void write_pmpaddr(int idx, uintptr_t val)
{
	switch (idx) {
	case 0:
		write_csr(pmpaddr0, val);
		break;
	case 1:
		write_csr(pmpaddr1, val);
		break;
	case 2:
		write_csr(pmpaddr2, val);
		break;
	case 3:
		write_csr(pmpaddr3, val);
		break;
	case 4:
		write_csr(pmpaddr4, val);
		break;
	case 5:
		write_csr(pmpaddr5, val);
		break;
	case 6:
		write_csr(pmpaddr6, val);
		break;
	case 7:
		write_csr(pmpaddr7, val);
		break;
	case 8:
		write_csr(pmpaddr8, val);
		break;
	case 9:
		write_csr(pmpaddr9, val);
		break;
	case 10:
		write_csr(pmpaddr10, val);
		break;
	case 11:
		write_csr(pmpaddr11, val);
		break;
	case 12:
		write_csr(pmpaddr12, val);
		break;
	case 13:
		write_csr(pmpaddr13, val);
		break;
	case 14:
		write_csr(pmpaddr14, val);
		break;
	case 15:
		write_csr(pmpaddr15, val);
		break;
	}
	if (read_pmpaddr(idx) != val)
		die("write pmpaddr failure");
}

/* Generate a PMP configuration of type NA4/NAPOT */
static pmpcfg_t generate_pmp_napot(
		uintptr_t base, uintptr_t size, uintptr_t flags)
{
	pmpcfg_t p;
	flags = flags & (PMP_R | PMP_W | PMP_X | PMP_L);
	p.cfg = flags | (size > GRANULE ? PMP_NAPOT : PMP_NA4);
	p.previous_address = 0;
	p.address = (base + (size / 2 - 1)) >> PMP_SHIFT;
	return p;
}

/* Generate a PMP configuration of type TOR */
static pmpcfg_t generate_pmp_range(
		uintptr_t base, uintptr_t size, uintptr_t flags)
{
	pmpcfg_t p;
	flags = flags & (PMP_R | PMP_W | PMP_X | PMP_L);
	p.cfg = flags | PMP_TOR;
	p.previous_address = base >> PMP_SHIFT;
	p.address = (base + size) >> PMP_SHIFT;
	return p;
}

/* Generate a PMP configuration */
static pmpcfg_t generate_pmp(uintptr_t base, uintptr_t size, uintptr_t flags)
{
	if (IS_POWER_OF_2(size) && (size >= 4) && ((base & (size - 1)) == 0))
		return generate_pmp_napot(base, size, flags);
	else
		return generate_pmp_range(base, size, flags);
}

/*
 * find empty PMP entry by type
 * TOR type configuration requires two consecutive PMP entries,
 * others requires one.
 */
static int find_empty_pmp_entry(int is_range)
{
	int free_entries = 0;
	for (int i = 0; i < pmp_entries_num(); i++) {
		if (pmp_entry_used_mask & (1 << i))
			free_entries = 0;
		else
			free_entries++;
		if (is_range && (free_entries == 2))
			return i;
		if (!is_range && (free_entries == 1))
			return i;
	}
	die("Too many PMP configurations, no free entries can be used!");
	return -1;
}

/*
 * mark PMP entry has be used
 * this function need be used with find_entry_pmp_entry
 *
 *   n = find_empty_pmp_entry(is_range)
 *   ... // PMP set operate
 *   mask_pmp_entry_used(n);
 */
static void mask_pmp_entry_used(int idx)
{
	pmp_entry_used_mask |= 1 << idx;
}

/* reset PMP setting */
void reset_pmp(void)
{
	for (int i = 0; i < pmp_entries_num(); i++) {
		if (read_pmpcfg(i) & PMP_L)
			die("Some PMP configurations are locked "
					"and cannot be reset!");
		write_pmpcfg(i, 0);
		write_pmpaddr(i, 0);
	}
}

/* set up PMP record */
void setup_pmp(uintptr_t base, uintptr_t size, uintptr_t flags)
{
	pmpcfg_t p;
	int is_range, n;

	p = generate_pmp(base, size, flags);
	is_range = ((p.cfg & PMP_A) == PMP_TOR);

	n = find_empty_pmp_entry(is_range);

	write_pmpaddr(n, p.address);
	if (is_range)
		write_pmpaddr(n - 1, p.previous_address);
	write_pmpcfg(n, p.cfg);

	mask_pmp_entry_used(n);
	if (is_range)
		mask_pmp_entry_used(n - 1);
}
