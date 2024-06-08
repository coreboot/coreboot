/* SPDX-License-Identifier: GPL-2.0-only */

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
struct pmpcfg {
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
};

/* This variable is used to record which entries have been used. */
static uintptr_t pmp_entry_used_mask;

/* The architectural spec says that up to 16 PMP entries are
 * available.
 * "Up to 16 PMP entries are supported. If any PMP  entries are
 *  implemented, then all PMP CSRs must be implemented,
 *  but all PMP CSR fields are WARL and may be hardwired to zero."
 */
int pmp_entries_num(void)
{
	return 16;
}

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
		printk(BIOS_INFO, "%s(%d, %lx) = %lx\n", __func__, idx, cfg, read_csr(pmpcfg0));
		break;
	case 1:
		old = read_csr(pmpcfg2);
		new = (old & ~((uintptr_t)0xff << shift))
			| ((cfg & 0xff) << shift);
		write_csr(pmpcfg2, new);
		printk(BIOS_INFO, "%s(%d, %lx) = %lx\n", __func__, idx, cfg, read_csr(pmpcfg2));
		break;
	}
#endif
	if (read_pmpcfg(idx) != cfg) {
		printk(BIOS_WARNING, "%s: PMPcfg%d: Wrote %lx, read %lx\n", __func__, idx, cfg, read_pmpcfg(idx));
		die("PMPcfg write failed");
	}
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

	printk(BIOS_INFO, "%s(%d, %lx) = %lx\n", __func__, idx, val, read_pmpaddr(idx));
	/* The PMP is not required to return what we wrote. On some SoC, many bits are cleared. */
	if (read_pmpaddr(idx) != val) {
		printk(BIOS_WARNING, "%s: PMPaddr%d: Wrote %lx, read %lx\n", __func__,
				idx, val, read_pmpaddr(idx));
	}
}

/* Generate a PMP configuration for all memory */
static void generate_pmp_all(struct pmpcfg *p)
{
	p->cfg = PMP_NAPOT | PMP_R | PMP_W | PMP_X;
	p->previous_address = 0;
	p->address = (uintptr_t) -1;
}

/* Generate a PMP configuration of type NA4/NAPOT */
static void generate_pmp_napot(struct pmpcfg *p, uintptr_t base, uintptr_t size, u8 flags)
{
	flags = flags & (PMP_R | PMP_W | PMP_X | PMP_L);
	p->cfg = flags | (size > GRANULE ? PMP_NAPOT : PMP_NA4);
	p->previous_address = 0;
	p->address = (base + (size / 2 - 1));
}

/* Generate a PMP configuration of type TOR */
static void generate_pmp_range(struct pmpcfg *p, uintptr_t base, uintptr_t size, u8 flags)
{
	flags = flags & (PMP_R | PMP_W | PMP_X | PMP_L);
	p->cfg = flags | PMP_TOR;
	p->previous_address = base;
	p->address = (base + size);
}

/*
 * Generate a PMP configuration.
 * reminder: base and size are 34 bit numbers on RV32.
 */
static int generate_pmp(struct pmpcfg *p, u64 base, u64 size, u8 flags)
{
	/* Convert the byte address and byte size to units of 32-bit words */
	uintptr_t b = (uintptr_t) base >> PMP_SHIFT, s = (uintptr_t) size >> PMP_SHIFT;
#if __riscv_xlen == 32
	/* verify that base + size fits in 34 bits */
	if ((base + size - 1) >> 34) {
		printk(BIOS_EMERG, "%s: base (%llx) + size (%llx) - 1 is more than 34 bits\n",
				__func__, base, size);
		return 1;
	}
#endif
	/*  if base is -1, that means "match all" */
	if (base == (u64)-1) {
		generate_pmp_all(p);
	} else if (IS_POWER_OF_2(size) && (size >= 4) && ((base & (size - 1)) == 0)) {
		generate_pmp_napot(p, b, s, flags);
	} else {
		generate_pmp_range(p, b, s, flags);
	}
	return 0;
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

/* prints the pmp regions by reading the PMP address and configuration registers */
void print_pmp_regions(void)
{
	uintptr_t prev_pmpaddr = 0;
	uintptr_t base = 0;
	uintptr_t size = 0;
	const char *mode;
	for (int i = 0; i < pmp_entries_num(); i++) {
		uintptr_t pmpcfg = read_pmpcfg(i);
		uintptr_t pmpaddr = read_pmpaddr(i);
		if ((pmpcfg & PMP_A) == 0) {
			continue; // PMP entry is disabled
		} else if (pmpcfg & PMP_NA4) {
			base = pmpaddr << PMP_SHIFT;
			size = 4;
			mode = "NA4";
		} else if (pmpcfg & PMP_NAPOT) {
			unsigned int count_trailing_ones = 0;
			base = pmpaddr;
			while (base) {
				if ((base & 1) == 0)
					break; // we got a zero
				count_trailing_ones++;
				base >>= 1;
			}
			size = 8 << count_trailing_ones;
			base = (pmpaddr & ~((2 << count_trailing_ones) - 1)) >> PMP_SHIFT;
			mode = "NAPOT";
		} else if (pmpcfg & PMP_TOR) {
			base = pmpaddr;
			size = pmpaddr - prev_pmpaddr;
			mode = "TOR";
		}

		printk(BIOS_DEBUG, "base: 0x%lx, size: 0x%lx, perm: %c%c%c, mode: %s, locked: %d\n",
			base, size,
			(pmpcfg & PMP_R) ? 'r' : ' ',
			(pmpcfg & PMP_W) ? 'w' : ' ',
			(pmpcfg & PMP_X) ? 'x' : ' ',
			mode,
			(pmpcfg & PMP_L) ? 1 : 0);

		prev_pmpaddr = pmpaddr;
	}
}

/* reset PMP setting */
void reset_pmp(void)
{
	for (int i = 0; i < pmp_entries_num(); i++) {
		if (read_pmpcfg(i) & PMP_L)
			die("Some PMP configurations are locked and cannot be reset!");
		write_pmpcfg(i, 0);
		write_pmpaddr(i, 0);
	}
}

/*
 * set up PMP record
 * Why are these u64 and not uintptr_t?
 * because, per the spec:
 * The Sv32 page-based virtual-memory scheme described in Section 4.3
 * supports 34-bit physical addresses for RV32, so the PMP scheme must
 * support addresses wider than XLEN for RV32.
 * Yes, in RV32, these are 34-bit numbers.
 * Rather than require every future user of these to remember that,
 * this ABI is 64 bits.
 * generate_pmp will check for out of range values.
 */
void setup_pmp(u64 base, u64 size, u8 flags)
{
	struct pmpcfg p;
	int is_range, n;

	if (generate_pmp(&p, base, size, flags))
		return;

	is_range = ((p.cfg & PMP_A) == PMP_TOR);

	n = find_empty_pmp_entry(is_range);

	/*
	 * NOTE! you MUST write the cfg register first, or on (e.g.)
	 * the SiFive FU740, it will not take all the bits.
	 * This is different than QEMU. NASTY!
	 */
	write_pmpcfg(n, p.cfg);

	write_pmpaddr(n, p.address);
	if (is_range)
		write_pmpaddr(n - 1, p.previous_address);

	mask_pmp_entry_used(n);
	if (is_range)
		mask_pmp_entry_used(n - 1);
}

/*
 * close_pmp will "close" the pmp.
 * This consists of adding the "match every address" entry.
 * This should be the last pmp function that is called.
 * Because we can not be certain that there is not some reason for it
 * NOT to be last, we do not check -- perhaps, later, a check would
 * make sense, but, for now, we do not check.
 * If previous code has used up all pmp entries, print a warning
 * and continue.
 * The huge constant for the memory size may seem a bit odd here.
 * Recall that PMP is to protect a *limited* number of M mode
 * memory ranges from S and U modes. Therefore, the last range
 * entry should cover all possible addresses, up to
 * an architectural limit. It is entirely acceptable
 * for it to cover memory that does not exist -- PMP
 * protects M mode, nothing more.
 * Think of this range as the final catch-all else
 * in an if-then-else.
  */
void close_pmp(void)
{
	setup_pmp((u64)-1, 0, PMP_R|PMP_W|PMP_X);
}
