/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * mtrr.c: setting MTRR to decent values for cache initialization on P6
 * Derived from intel_set_mtrr in intel_subr.c and mtrr.c in linux kernel
 *
 * Reference: Intel Architecture Software Developer's Manual, Volume 3: System
 * Programming
 */

#include <stddef.h>
#include <string.h>
#include <bootstate.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <memrange.h>
#include <cpu/amd/mtrr.h>
#include <assert.h>
#if CONFIG(X86_AMD_FIXED_MTRRS)
#define MTRR_FIXED_WRBACK_BITS (MTRR_READ_MEM | MTRR_WRITE_MEM)
#else
#define MTRR_FIXED_WRBACK_BITS 0
#endif

#define MIN_MTRRS	8

/*
 * Static storage size for variable MTRRs. It's sized sufficiently large to
 * handle different types of CPUs. Empirically, 16 variable MTRRs has not
 * yet been observed.
 */
#define NUM_MTRR_STATIC_STORAGE 16

static int total_mtrrs;

static void detect_var_mtrrs(void)
{
	total_mtrrs = get_var_mtrr_count();

	if (total_mtrrs > NUM_MTRR_STATIC_STORAGE) {
		printk(BIOS_WARNING,
			"MTRRs detected (%d) > NUM_MTRR_STATIC_STORAGE (%d)\n",
			total_mtrrs, NUM_MTRR_STATIC_STORAGE);
		total_mtrrs = NUM_MTRR_STATIC_STORAGE;
	}
}

void enable_fixed_mtrr(void)
{
	msr_t msr;

	msr = rdmsr(MTRR_DEF_TYPE_MSR);
	msr.lo |= MTRR_DEF_TYPE_EN | MTRR_DEF_TYPE_FIX_EN;
	wrmsr(MTRR_DEF_TYPE_MSR, msr);
}

void fixed_mtrrs_expose_amd_rwdram(void)
{
	msr_t syscfg;

	if (!CONFIG(X86_AMD_FIXED_MTRRS))
		return;

	syscfg = rdmsr(SYSCFG_MSR);
	syscfg.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, syscfg);
}

void fixed_mtrrs_hide_amd_rwdram(void)
{
	msr_t syscfg;

	if (!CONFIG(X86_AMD_FIXED_MTRRS))
		return;

	syscfg = rdmsr(SYSCFG_MSR);
	syscfg.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, syscfg);
}

static void enable_var_mtrr(unsigned char deftype)
{
	msr_t msr;

	msr = rdmsr(MTRR_DEF_TYPE_MSR);
	msr.lo &= ~0xff;
	msr.lo |= MTRR_DEF_TYPE_EN | deftype;
	wrmsr(MTRR_DEF_TYPE_MSR, msr);
}

#define MTRR_VERBOSE_LEVEL BIOS_NEVER

/* MTRRs are at a 4KiB granularity. */
#define RANGE_SHIFT 12
#define ADDR_SHIFT_TO_RANGE_SHIFT(x) \
	(((x) > RANGE_SHIFT) ? ((x) - RANGE_SHIFT) : RANGE_SHIFT)
#define PHYS_TO_RANGE_ADDR(x) ((x) >> RANGE_SHIFT)
#define RANGE_TO_PHYS_ADDR(x) (((resource_t)(x)) << RANGE_SHIFT)
#define NUM_FIXED_MTRRS (NUM_FIXED_RANGES / RANGES_PER_FIXED_MTRR)

/* Helpful constants. */
#define RANGE_1MB PHYS_TO_RANGE_ADDR(1ULL << 20)
#define RANGE_4GB (1ULL << (ADDR_SHIFT_TO_RANGE_SHIFT(32)))

#define MTRR_ALGO_SHIFT (8)
#define MTRR_TAG_MASK ((1 << MTRR_ALGO_SHIFT) - 1)

static inline uint64_t range_entry_base_mtrr_addr(struct range_entry *r)
{
	return PHYS_TO_RANGE_ADDR(range_entry_base(r));
}

static inline uint64_t range_entry_end_mtrr_addr(struct range_entry *r)
{
	return PHYS_TO_RANGE_ADDR(range_entry_end(r));
}

static inline int range_entry_mtrr_type(struct range_entry *r)
{
	return range_entry_tag(r) & MTRR_TAG_MASK;
}

static int filter_vga_wrcomb(struct device *dev, struct resource *res)
{
	/* Only handle PCI devices. */
	if (dev->path.type != DEVICE_PATH_PCI)
		return 0;

	/* Only handle VGA class devices. */
	if (((dev->class >> 8) != PCI_CLASS_DISPLAY_VGA))
		return 0;

	/* Add resource as write-combining in the address space. */
	return 1;
}

static void print_physical_address_space(const struct memranges *addr_space,
					const char *identifier)
{
	const struct range_entry *r;

	if (identifier)
		printk(BIOS_DEBUG, "MTRR: %s Physical address space:\n",
			identifier);
	else
		printk(BIOS_DEBUG, "MTRR: Physical address space:\n");

	memranges_each_entry(r, addr_space)
		printk(BIOS_DEBUG,
		       "0x%016llx - 0x%016llx size 0x%08llx type %ld\n",
		       range_entry_base(r), range_entry_end(r) - 1,
		       range_entry_size(r), range_entry_tag(r));
}

static struct memranges *get_physical_address_space(void)
{
	static struct memranges *addr_space;
	static struct memranges addr_space_storage;

	/* In order to handle some chipsets not being able to pre-determine
	 *  uncacheable ranges, such as graphics memory, at resource insertion
	 * time remove uncacheable regions from the cacheable ones. */
	if (addr_space == NULL) {
		unsigned long mask;
		unsigned long match;

		addr_space = &addr_space_storage;

		mask = IORESOURCE_CACHEABLE;
		/* Collect cacheable and uncacheable address ranges. The
		 * uncacheable regions take precedence over the  cacheable
		 * regions. */
		memranges_init(addr_space, mask, mask, MTRR_TYPE_WRBACK);
		memranges_add_resources(addr_space, mask, 0,
					MTRR_TYPE_UNCACHEABLE);

		/* Handle any write combining resources. Only prefetchable
		 * resources are appropriate for this MTRR type. */
		match = IORESOURCE_PREFETCH;
		mask |= match;
		memranges_add_resources_filter(addr_space, mask, match,
					MTRR_TYPE_WRCOMB, filter_vga_wrcomb);

		/* The address space below 4GiB is special. It needs to be
		 * covered entirely by range entries so that MTRR calculations
		 * can be properly done for the full 32-bit address space.
		 * Therefore, ensure holes are filled up to 4GiB as
		 * uncacheable */
		memranges_fill_holes_up_to(addr_space,
					   RANGE_TO_PHYS_ADDR(RANGE_4GB),
					   MTRR_TYPE_UNCACHEABLE);

		print_physical_address_space(addr_space, NULL);
	}

	return addr_space;
}

/* Fixed MTRR descriptor. This structure defines the step size and begin
 * and end (exclusive) address covered by a set of fixed MTRR MSRs.
 * It also describes the offset in byte intervals to store the calculated MTRR
 * type in an array. */
struct fixed_mtrr_desc {
	uint32_t begin;
	uint32_t end;
	uint32_t step;
	int range_index;
	int msr_index_base;
};

/* Shared MTRR calculations. Can be reused by APs. */
static uint8_t fixed_mtrr_types[NUM_FIXED_RANGES];

/* Fixed MTRR descriptors. */
static const struct fixed_mtrr_desc fixed_mtrr_desc[] = {
	{ PHYS_TO_RANGE_ADDR(0x000000), PHYS_TO_RANGE_ADDR(0x080000),
	  PHYS_TO_RANGE_ADDR(64 * 1024), 0, MTRR_FIX_64K_00000 },
	{ PHYS_TO_RANGE_ADDR(0x080000), PHYS_TO_RANGE_ADDR(0x0C0000),
	  PHYS_TO_RANGE_ADDR(16 * 1024), 8, MTRR_FIX_16K_80000 },
	{ PHYS_TO_RANGE_ADDR(0x0C0000), PHYS_TO_RANGE_ADDR(0x100000),
	  PHYS_TO_RANGE_ADDR(4 * 1024), 24, MTRR_FIX_4K_C0000 },
};

static void calc_fixed_mtrrs(void)
{
	static int fixed_mtrr_types_initialized;
	struct memranges *phys_addr_space;
	struct range_entry *r;
	const struct fixed_mtrr_desc *desc;
	const struct fixed_mtrr_desc *last_desc;
	uint32_t begin;
	uint32_t end;
	int type_index;

	if (fixed_mtrr_types_initialized)
		return;

	phys_addr_space = get_physical_address_space();

	/* Set all fixed ranges to uncacheable first. */
	memset(&fixed_mtrr_types[0], MTRR_TYPE_UNCACHEABLE, NUM_FIXED_RANGES);

	desc = &fixed_mtrr_desc[0];
	last_desc = &fixed_mtrr_desc[ARRAY_SIZE(fixed_mtrr_desc) - 1];

	memranges_each_entry(r, phys_addr_space) {
		begin = range_entry_base_mtrr_addr(r);
		end = range_entry_end_mtrr_addr(r);

		if (begin >= last_desc->end)
			break;

		if (end > last_desc->end)
			end = last_desc->end;

		/* Get to the correct fixed mtrr descriptor. */
		while (begin >= desc->end)
			desc++;

		type_index = desc->range_index;
		type_index += (begin - desc->begin) / desc->step;

		while (begin != end) {
			unsigned char type;

			type = range_entry_tag(r);
			printk(MTRR_VERBOSE_LEVEL,
			       "MTRR addr 0x%x-0x%x set to %d type @ %d\n",
			       begin, begin + desc->step - 1, type, type_index);
			if (type == MTRR_TYPE_WRBACK)
				type |= MTRR_FIXED_WRBACK_BITS;
			fixed_mtrr_types[type_index] = type;
			type_index++;
			begin += desc->step;
			if (begin == desc->end)
				desc++;
		}
	}
	fixed_mtrr_types_initialized = 1;
}

static void commit_fixed_mtrrs(void)
{
	int i;
	int j;
	int msr_num;
	int type_index;
	/* 8 ranges per msr. */
	msr_t fixed_msrs[NUM_FIXED_MTRRS];
	unsigned long msr_index[NUM_FIXED_MTRRS];

	fixed_mtrrs_expose_amd_rwdram();

	memset(&fixed_msrs, 0, sizeof(fixed_msrs));

	msr_num = 0;
	type_index = 0;
	for (i = 0; i < ARRAY_SIZE(fixed_mtrr_desc); i++) {
		const struct fixed_mtrr_desc *desc;
		int num_ranges;

		desc = &fixed_mtrr_desc[i];
		num_ranges = (desc->end - desc->begin) / desc->step;
		for (j = 0; j < num_ranges; j += RANGES_PER_FIXED_MTRR) {
			msr_index[msr_num] = desc->msr_index_base +
				(j / RANGES_PER_FIXED_MTRR);
			fixed_msrs[msr_num].lo |=
				fixed_mtrr_types[type_index++] << 0;
			fixed_msrs[msr_num].lo |=
				fixed_mtrr_types[type_index++] << 8;
			fixed_msrs[msr_num].lo |=
				fixed_mtrr_types[type_index++] << 16;
			fixed_msrs[msr_num].lo |=
				fixed_mtrr_types[type_index++] << 24;
			fixed_msrs[msr_num].hi |=
				fixed_mtrr_types[type_index++] << 0;
			fixed_msrs[msr_num].hi |=
				fixed_mtrr_types[type_index++] << 8;
			fixed_msrs[msr_num].hi |=
				fixed_mtrr_types[type_index++] << 16;
			fixed_msrs[msr_num].hi |=
				fixed_mtrr_types[type_index++] << 24;
			msr_num++;
		}
	}

	/* Ensure that both arrays were fully initialized */
	ASSERT(msr_num == NUM_FIXED_MTRRS)

	for (i = 0; i < ARRAY_SIZE(fixed_msrs); i++)
		printk(BIOS_DEBUG, "MTRR: Fixed MSR 0x%lx 0x%08x%08x\n",
		       msr_index[i], fixed_msrs[i].hi, fixed_msrs[i].lo);

	disable_cache();
	for (i = 0; i < ARRAY_SIZE(fixed_msrs); i++)
		wrmsr(msr_index[i], fixed_msrs[i]);
	enable_cache();
	fixed_mtrrs_hide_amd_rwdram();

}

void x86_setup_fixed_mtrrs_no_enable(void)
{
	calc_fixed_mtrrs();
	commit_fixed_mtrrs();
}

void x86_setup_fixed_mtrrs(void)
{
	x86_setup_fixed_mtrrs_no_enable();

	printk(BIOS_SPEW, "call enable_fixed_mtrr()\n");
	enable_fixed_mtrr();
}

struct var_mtrr_regs {
	msr_t base;
	msr_t mask;
};

struct var_mtrr_solution {
	int mtrr_default_type;
	int num_used;
	struct var_mtrr_regs regs[NUM_MTRR_STATIC_STORAGE];
};

/* Global storage for variable MTRR solution. */
static struct var_mtrr_solution mtrr_global_solution;

struct var_mtrr_state {
	struct memranges *addr_space;
	int above4gb;
	int address_bits;
	int prepare_msrs;
	int mtrr_index;
	int def_mtrr_type;
	struct var_mtrr_regs *regs;
};

static void clear_var_mtrr(int index)
{
	msr_t msr = { .lo = 0, .hi = 0 };

	wrmsr(MTRR_PHYS_BASE(index), msr);
	wrmsr(MTRR_PHYS_MASK(index), msr);
}

static int get_os_reserved_mtrrs(void)
{
	return CONFIG(RESERVE_MTRRS_FOR_OS) ? 2 : 0;
}

static void prep_var_mtrr(struct var_mtrr_state *var_state,
			  uint64_t base, uint64_t size, int mtrr_type)
{
	struct var_mtrr_regs *regs;
	resource_t rbase;
	resource_t rsize;
	resource_t mask;

	if (var_state->mtrr_index >= total_mtrrs) {
		printk(BIOS_ERR, "Not enough MTRRs available! MTRR index is %d with %d MTRRs in total.\n",
		       var_state->mtrr_index, total_mtrrs);
		return;
	}

	/*
	 * If desired, 2 variable MTRRs are attempted to be saved for the OS to
	 * use. However, it's more important to try to map the full address
	 * space properly.
	 */
	if (var_state->mtrr_index >= total_mtrrs - get_os_reserved_mtrrs())
		printk(BIOS_WARNING, "Taking a reserved OS MTRR.\n");

	rbase = base;
	rsize = size;

	rbase = RANGE_TO_PHYS_ADDR(rbase);
	rsize = RANGE_TO_PHYS_ADDR(rsize);
	rsize = -rsize;

	mask = (1ULL << var_state->address_bits) - 1;
	rsize = rsize & mask;

	printk(BIOS_DEBUG, "MTRR: %d base 0x%016llx mask 0x%016llx type %d\n",
	       var_state->mtrr_index, rbase, rsize, mtrr_type);

	regs = &var_state->regs[var_state->mtrr_index];

	regs->base.lo = rbase;
	regs->base.lo |= mtrr_type;
	regs->base.hi = rbase >> 32;

	regs->mask.lo = rsize;
	regs->mask.lo |= MTRR_PHYS_MASK_VALID;
	regs->mask.hi = rsize >> 32;
}

/*
 * fls64: find least significant bit set in a 64-bit word
 * As samples, fls64(0x0) = 64; fls64(0x4400) = 10;
 * fls64(0x40400000000) = 34.
 */
static uint32_t fls64(uint64_t x)
{
	uint32_t lo = (uint32_t)x;
	if (lo)
		return fls(lo);
	uint32_t hi = x >> 32;
	return fls(hi) + 32;
}

/*
 * fms64: find most significant bit set in a 64-bit word
 * As samples, fms64(0x0) = 0; fms64(0x4400) = 14;
 * fms64(0x40400000000) = 42.
 */
static uint32_t fms64(uint64_t x)
{
	uint32_t hi = (uint32_t)(x >> 32);
	if (!hi)
		return fms((uint32_t)x);
	return fms(hi) + 32;
}

static void calc_var_mtrr_range(struct var_mtrr_state *var_state,
				uint64_t base, uint64_t size, int mtrr_type)
{
	while (size != 0) {
		uint32_t addr_lsb;
		uint32_t size_msb;
		uint64_t mtrr_size;

		addr_lsb = fls64(base);
		size_msb = fms64(size);

		/* All MTRR entries need to have their base aligned to the mask
		 * size. The maximum size is calculated by a function of the
		 * min base bit set and maximum size bit set. */
		if (addr_lsb > size_msb)
			mtrr_size = 1ULL << size_msb;
		else
			mtrr_size = 1ULL << addr_lsb;

		if (var_state->prepare_msrs)
			prep_var_mtrr(var_state, base, mtrr_size, mtrr_type);

		size -= mtrr_size;
		base += mtrr_size;
		var_state->mtrr_index++;
	}
}

static uint64_t optimize_var_mtrr_hole(const uint64_t base,
				       const uint64_t hole,
				       const uint64_t limit,
				       const int carve_hole)
{
	/*
	 * With default type UC, we can potentially optimize a WB
	 * range with unaligned upper end, by aligning it up and
	 * carving the added "hole" out again.
	 *
	 * To optimize the upper end of the hole, we will test
	 * how many MTRRs calc_var_mtrr_range() will spend for any
	 * alignment of the hole's upper end.
	 *
	 * We take four parameters, the lower end of the WB range
	 * `base`, upper end of the WB range as start of the `hole`,
	 * a `limit` how far we may align the upper end of the hole
	 * up and a flag `carve_hole` whether we should count MTRRs
	 * for carving the hole out. We return the optimal upper end
	 * for the hole (which may be the same as the end of the WB
	 * range in case we don't gain anything by aligning up).
	 */

	const int dont_care = 0;
	struct var_mtrr_state var_state = { 0, };

	unsigned int align, best_count;
	uint32_t best_end = hole;

	/* calculate MTRR count for the WB range alone (w/o a hole) */
	calc_var_mtrr_range(&var_state, base, hole - base, dont_care);
	best_count = var_state.mtrr_index;
	var_state.mtrr_index = 0;

	for (align = fls(hole) + 1; align <= fms(hole); ++align) {
		const uint64_t hole_end = ALIGN_UP((uint64_t)hole, 1 << align);
		if (hole_end > limit)
			break;

		/* calculate MTRR count for this alignment */
		calc_var_mtrr_range(
			&var_state, base, hole_end - base, dont_care);
		if (carve_hole)
			calc_var_mtrr_range(
				&var_state, hole, hole_end - hole, dont_care);

		if (var_state.mtrr_index < best_count) {
			best_count = var_state.mtrr_index;
			best_end = hole_end;
		}
		var_state.mtrr_index = 0;
	}

	return best_end;
}

static void calc_var_mtrrs_with_hole(struct var_mtrr_state *var_state,
				     struct range_entry *r)
{
	uint64_t a1, a2, b1, b2;
	int mtrr_type, carve_hole;

	/*
	 * Determine MTRRs based on the following algorithm for the given entry:
	 * +------------------+ b2 = ALIGN_UP(end)
	 * |  0 or more bytes | <-- hole is carved out between b1 and b2
	 * +------------------+ a2 = b1 = original end
	 * |                  |
	 * +------------------+ a1 = begin
	 *
	 * Thus, there are up to 2 sub-ranges to configure variable MTRRs for.
	 */
	mtrr_type = range_entry_mtrr_type(r);

	a1 = range_entry_base_mtrr_addr(r);
	a2 = range_entry_end_mtrr_addr(r);

	/* The end address is within the first 1MiB. The fixed MTRRs take
	 * precedence over the variable ones. Therefore this range
	 * can be ignored. */
	if (a2 <= RANGE_1MB)
		return;

	/* Again, the fixed MTRRs take precedence so the beginning
	 * of the range can be set to 0 if it starts at or below 1MiB. */
	if (a1 <= RANGE_1MB)
		a1 = 0;

	/* If the range starts above 4GiB the processing is done. */
	if (!var_state->above4gb && a1 >= RANGE_4GB)
		return;

	/* Clip the upper address to 4GiB if addresses above 4GiB
	 * are not being processed. */
	if (!var_state->above4gb && a2 > RANGE_4GB)
		a2 = RANGE_4GB;

	b1 = a2;
	b2 = a2;
	carve_hole = 0;

	/* We only consider WB type ranges for hole-carving. */
	if (mtrr_type == MTRR_TYPE_WRBACK) {
		struct range_entry *next;
		uint64_t b2_limit;
		/*
		 * Depending on the type of the next range, there are three
		 * different situations to handle:
		 *
		 * 1. WB range is last in address space:
		 *    Aligning up, up to the next power of 2, may gain us
		 *    something.
		 *
		 * 2. The next range is of type UC:
		 *    We may align up, up to the _end_ of the next range. If
		 *    there is a gap between the current and the next range,
		 *    it would have been covered by the default type UC anyway.
		 *
		 * 3. The next range is not of type UC:
		 *    We may align up, up to the _base_ of the next range. This
		 *    may either be the end of the current range (if the next
		 *    range follows immediately) or the end of the gap between
		 *    the ranges.
		 */
		next = memranges_next_entry(var_state->addr_space, r);
		if (next == NULL) {
			b2_limit = ALIGN_UP((uint64_t)b1, 1 << fms(b1));
			/* If it's the last range above 4GiB, we won't carve
			   the hole out. If an OS wanted to move MMIO there,
			   it would have to override the MTRR setting using
			   PAT just like it would with WB as default type. */
			carve_hole = a1 < RANGE_4GB;
		} else if (range_entry_mtrr_type(next)
				== MTRR_TYPE_UNCACHEABLE) {
			b2_limit = range_entry_end_mtrr_addr(next);
			carve_hole = 1;
		} else {
			b2_limit = range_entry_base_mtrr_addr(next);
			carve_hole = 1;
		}
		b2 = optimize_var_mtrr_hole(a1, b1, b2_limit, carve_hole);
	}

	calc_var_mtrr_range(var_state, a1, b2 - a1, mtrr_type);
	if (carve_hole && b2 != b1) {
		calc_var_mtrr_range(var_state, b1, b2 - b1,
				    MTRR_TYPE_UNCACHEABLE);
	}
}

static void __calc_var_mtrrs(struct memranges *addr_space,
			     int above4gb, int address_bits,
			     int *num_def_wb_mtrrs, int *num_def_uc_mtrrs)
{
	int wb_deftype_count;
	int uc_deftype_count;
	struct range_entry *r;
	struct var_mtrr_state var_state;

	/* The default MTRR cacheability type is determined by calculating
	 * the number of MTRRs required for each MTRR type as if it was the
	 * default. */
	var_state.addr_space = addr_space;
	var_state.above4gb = above4gb;
	var_state.address_bits = address_bits;
	var_state.prepare_msrs = 0;

	wb_deftype_count = 0;
	uc_deftype_count = 0;

	/*
	 * For each range do 2 calculations:
	 *   1. UC as default type with possible holes at top of range.
	 *   2. WB as default.
	 * The lowest count is then used as default after totaling all
	 * MTRRs. UC takes precedence in the MTRR architecture. There-
	 * fore, only holes can be used when the type of the region is
	 * MTRR_TYPE_WRBACK with MTRR_TYPE_UNCACHEABLE as the default
	 * type.
	 */
	memranges_each_entry(r, var_state.addr_space) {
		int mtrr_type;

		mtrr_type = range_entry_mtrr_type(r);

		if (mtrr_type != MTRR_TYPE_UNCACHEABLE) {
			var_state.mtrr_index = 0;
			var_state.def_mtrr_type = MTRR_TYPE_UNCACHEABLE;
			calc_var_mtrrs_with_hole(&var_state, r);
			uc_deftype_count += var_state.mtrr_index;
		}

		if (mtrr_type != MTRR_TYPE_WRBACK) {
			var_state.mtrr_index = 0;
			var_state.def_mtrr_type = MTRR_TYPE_WRBACK;
			calc_var_mtrrs_with_hole(&var_state, r);
			wb_deftype_count += var_state.mtrr_index;
		}
	}

	*num_def_wb_mtrrs = wb_deftype_count;
	*num_def_uc_mtrrs = uc_deftype_count;
}

static int calc_var_mtrrs(struct memranges *addr_space,
			  int above4gb, int address_bits)
{
	int wb_deftype_count = 0;
	int uc_deftype_count = 0;

	__calc_var_mtrrs(addr_space, above4gb, address_bits, &wb_deftype_count,
			 &uc_deftype_count);

	const int bios_mtrrs = total_mtrrs - get_os_reserved_mtrrs();
	if (wb_deftype_count > bios_mtrrs && uc_deftype_count > bios_mtrrs) {
		printk(BIOS_DEBUG, "MTRR: Removing WRCOMB type. "
		       "WB/UC MTRR counts: %d/%d > %d.\n",
		       wb_deftype_count, uc_deftype_count, bios_mtrrs);
		memranges_update_tag(addr_space, MTRR_TYPE_WRCOMB,
				     MTRR_TYPE_UNCACHEABLE);
		__calc_var_mtrrs(addr_space, above4gb, address_bits,
				 &wb_deftype_count, &uc_deftype_count);
	}

	printk(BIOS_DEBUG, "MTRR: default type WB/UC MTRR counts: %d/%d.\n",
	       wb_deftype_count, uc_deftype_count);

	if (wb_deftype_count < uc_deftype_count) {
		printk(BIOS_DEBUG, "MTRR: WB selected as default type.\n");
		return MTRR_TYPE_WRBACK;
	}
	printk(BIOS_DEBUG, "MTRR: UC selected as default type.\n");
	return MTRR_TYPE_UNCACHEABLE;
}

static void prepare_var_mtrrs(struct memranges *addr_space, int def_type,
				int above4gb, int address_bits,
				struct var_mtrr_solution *sol)
{
	struct range_entry *r;
	struct var_mtrr_state var_state;

	var_state.addr_space = addr_space;
	var_state.above4gb = above4gb;
	var_state.address_bits = address_bits;
	/* Prepare the MSRs. */
	var_state.prepare_msrs = 1;
	var_state.mtrr_index = 0;
	var_state.def_mtrr_type = def_type;
	var_state.regs = &sol->regs[0];

	memranges_each_entry(r, var_state.addr_space) {
		if (range_entry_mtrr_type(r) == def_type)
			continue;
		calc_var_mtrrs_with_hole(&var_state, r);
	}

	/* Update the solution. */
	sol->num_used = var_state.mtrr_index;
}

static int commit_var_mtrrs(const struct var_mtrr_solution *sol)
{
	int i;

	if (sol->num_used > total_mtrrs) {
		printk(BIOS_WARNING, "Not enough MTRRs: %d vs %d\n",
			sol->num_used, total_mtrrs);
		return -1;
	}

	/* Write out the variable MTRRs. */
	disable_cache();
	for (i = 0; i < sol->num_used; i++) {
		wrmsr(MTRR_PHYS_BASE(i), sol->regs[i].base);
		wrmsr(MTRR_PHYS_MASK(i), sol->regs[i].mask);
	}
	/* Clear the ones that are unused. */
	for (; i < total_mtrrs; i++)
		clear_var_mtrr(i);
	enable_var_mtrr(sol->mtrr_default_type);
	enable_cache();

	return 0;
}

void x86_setup_var_mtrrs(unsigned int address_bits, unsigned int above4gb)
{
	static struct var_mtrr_solution *sol = NULL;
	struct memranges *addr_space;

	addr_space = get_physical_address_space();

	if (sol == NULL) {
		sol = &mtrr_global_solution;
		sol->mtrr_default_type =
			calc_var_mtrrs(addr_space, !!above4gb, address_bits);
		prepare_var_mtrrs(addr_space, sol->mtrr_default_type,
				  !!above4gb, address_bits, sol);
	}

	commit_var_mtrrs(sol);
}

static void _x86_setup_mtrrs(unsigned int above4gb)
{
	int address_size;

	x86_setup_fixed_mtrrs();
	address_size = cpu_phys_address_size();
	printk(BIOS_DEBUG, "CPU physical address size: %d bits\n",
		address_size);
	x86_setup_var_mtrrs(address_size, above4gb);
}

void x86_setup_mtrrs(void)
{
	/* Without detect, assume the minimum */
	total_mtrrs = MIN_MTRRS;
	/* Always handle addresses above 4GiB. */
	_x86_setup_mtrrs(1);
}

void x86_setup_mtrrs_with_detect(void)
{
	detect_var_mtrrs();
	/* Always handle addresses above 4GiB. */
	_x86_setup_mtrrs(1);
}

void x86_setup_mtrrs_with_detect_no_above_4gb(void)
{
	detect_var_mtrrs();
	_x86_setup_mtrrs(0);
}

void x86_mtrr_check(void)
{
	/* Only Pentium Pro and later have MTRR */
	msr_t msr;
	printk(BIOS_DEBUG, "\nMTRR check\n");

	msr = rdmsr(MTRR_DEF_TYPE_MSR);

	printk(BIOS_DEBUG, "Fixed MTRRs   : ");
	if (msr.lo & MTRR_DEF_TYPE_FIX_EN)
		printk(BIOS_DEBUG, "Enabled\n");
	else
		printk(BIOS_DEBUG, "Disabled\n");

	printk(BIOS_DEBUG, "Variable MTRRs: ");
	if (msr.lo & MTRR_DEF_TYPE_EN)
		printk(BIOS_DEBUG, "Enabled\n");
	else
		printk(BIOS_DEBUG, "Disabled\n");

	printk(BIOS_DEBUG, "\n");

	post_code(0x93);
}

static bool put_back_original_solution;

void need_restore_mtrr(void)
{
	put_back_original_solution = true;
}

void mtrr_use_temp_range(uintptr_t begin, size_t size, int type)
{
	const struct range_entry *r;
	const struct memranges *orig;
	struct var_mtrr_solution sol;
	struct memranges addr_space;
	const int above4gb = 1; /* Cover above 4GiB by default. */
	int address_bits;

	/* Make a copy of the original address space and tweak it with the
	 * provided range. */
	memranges_init_empty(&addr_space, NULL, 0);
	orig = get_physical_address_space();
	memranges_each_entry(r, orig) {
		unsigned long tag = range_entry_tag(r);

		/* Remove any write combining MTRRs from the temporary
		 * solution as it just fragments the address space. */
		if (tag == MTRR_TYPE_WRCOMB)
			tag = MTRR_TYPE_UNCACHEABLE;

		memranges_insert(&addr_space, range_entry_base(r),
				range_entry_size(r), tag);
	}

	/* Place new range into the address space. */
	memranges_insert(&addr_space, begin, size, type);

	print_physical_address_space(&addr_space, "TEMPORARY");

	/* Calculate a new solution with the updated address space. */
	address_bits = cpu_phys_address_size();
	memset(&sol, 0, sizeof(sol));
	sol.mtrr_default_type =
		calc_var_mtrrs(&addr_space, above4gb, address_bits);
	prepare_var_mtrrs(&addr_space, sol.mtrr_default_type,
				above4gb, address_bits, &sol);

	if (commit_var_mtrrs(&sol) < 0)
		printk(BIOS_WARNING, "Unable to insert temporary MTRR range: 0x%016llx - 0x%016llx size 0x%08llx type %d\n",
			(long long)begin, (long long)begin + size - 1,
			(long long)size, type);
	else
		need_restore_mtrr();

	memranges_teardown(&addr_space);
}

static void remove_temp_solution(void *unused)
{
	if (put_back_original_solution)
		commit_var_mtrrs(&mtrr_global_solution);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, remove_temp_solution, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, remove_temp_solution, NULL);
