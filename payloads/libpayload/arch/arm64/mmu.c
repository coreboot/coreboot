/*
 *
 * Copyright 2014 Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <arch/mmu.h>
#include <arch/lib_helpers.h>
#include <arch/cache.h>

/* Maximum number of XLAT Tables available based on ttb buffer size */
static unsigned int max_tables;
/* Address of ttb buffer */
static uint64_t *xlat_addr;

static int free_idx;
static uint8_t ttb_buffer[TTB_DEFAULT_SIZE] __attribute__((aligned(GRANULE_SIZE)));

static const char * const tag_to_string[] = {
	[TYPE_NORMAL_MEM] = "normal",
	[TYPE_DEV_MEM] = "device",
	[TYPE_DMA_MEM] = "uncached",
};

/*
 * The usedmem_ranges is used to describe all the memory ranges that are
 * actually used by payload i.e. _start -> _end in linker script and the
 * coreboot tables. This is required for two purposes:
 * 1) During the pre_sysinfo_scan_mmu_setup, these are the only ranges
 * initialized in the page table as we do not know the entire memory map.
 * 2) During the post_sysinfo_scan_mmu_setup, these ranges are used to check if
 * the DMA buffer is being placed in a sane location and does not overlap any of
 * the used mem ranges.
 */
static struct mmu_ranges usedmem_ranges;

static void __attribute__((noreturn)) mmu_error(void)
{
	halt();
}

/* Func : get_block_attr
 * Desc : Get block descriptor attributes based on the value of tag in memrange
 * region
 */
static uint64_t get_block_attr(unsigned long tag)
{
	uint64_t attr;

	/* We should be in EL2(which is non-secure only) or EL1(non-secure) */
	attr = BLOCK_NS;

	/* Assuming whole memory is read-write */
	attr |= BLOCK_AP_RW;

	attr |= BLOCK_ACCESS;

	switch (tag) {

	case TYPE_NORMAL_MEM:
		attr |= BLOCK_SH_INNER_SHAREABLE;
		attr |= (BLOCK_INDEX_MEM_NORMAL << BLOCK_INDEX_SHIFT);
		break;
	case TYPE_DEV_MEM:
		attr |= BLOCK_INDEX_MEM_DEV_NGNRNE << BLOCK_INDEX_SHIFT;
		attr |= BLOCK_XN;
		break;
	case TYPE_DMA_MEM:
		attr |= BLOCK_INDEX_MEM_NORMAL_NC << BLOCK_INDEX_SHIFT;
		break;
	}

	return attr;
}

/* Func : table_desc_valid
 * Desc : Check if a table entry contains valid desc
 */
static uint64_t table_desc_valid(uint64_t desc)
{
	return((desc & TABLE_DESC) == TABLE_DESC);
}

/* Func : setup_new_table
 * Desc : Get next free table from TTB and set it up to match old parent entry.
 */
static uint64_t *setup_new_table(uint64_t desc, size_t xlat_size)
{
	uint64_t *new, *entry;

	assert(free_idx < max_tables);

	new = (uint64_t*)((unsigned char *)xlat_addr + free_idx * GRANULE_SIZE);
	free_idx++;

	if (!desc) {
		memset(new, 0, GRANULE_SIZE);
	} else {
		/* Can reuse old parent entry, but may need to adjust type. */
		if (xlat_size == L3_XLAT_SIZE)
			desc |= PAGE_DESC;

		for (entry = new; (u8 *)entry < (u8 *)new + GRANULE_SIZE;
		     entry++, desc += xlat_size)
			*entry = desc;
	}

	return new;
}

/* Func : get_table_from_desc
 * Desc : Get next level table address from table descriptor
 */
static uint64_t *get_table_from_desc(uint64_t desc)
{
	uint64_t *ptr = (uint64_t*)(desc & XLAT_TABLE_MASK);
	return ptr;
}

/* Func: get_next_level_table
 * Desc: Check if the table entry is a valid descriptor. If not, initialize new
 * table, update the entry and return the table addr. If valid, return the addr.
 */
static uint64_t *get_next_level_table(uint64_t *ptr, size_t xlat_size)
{
	uint64_t desc = *ptr;

	if (!table_desc_valid(desc)) {
		uint64_t *new_table = setup_new_table(desc, xlat_size);
		desc = ((uint64_t)new_table) | TABLE_DESC;
		*ptr = desc;
	}
	return get_table_from_desc(desc);
}

/* Func : init_xlat_table
 * Desc : Given a base address and size, it identifies the indices within
 * different level XLAT tables which map the given base addr. Similar to table
 * walk, except that all invalid entries during the walk are updated
 * accordingly. On success, it returns the size of the block/page addressed by
 * the final table.
 */
static uint64_t init_xlat_table(uint64_t base_addr,
				uint64_t size,
				uint64_t tag)
{
	uint64_t l0_index = (base_addr & L0_ADDR_MASK) >> L0_ADDR_SHIFT;
	uint64_t l1_index = (base_addr & L1_ADDR_MASK) >> L1_ADDR_SHIFT;
	uint64_t l2_index = (base_addr & L2_ADDR_MASK) >> L2_ADDR_SHIFT;
	uint64_t l3_index = (base_addr & L3_ADDR_MASK) >> L3_ADDR_SHIFT;
	uint64_t *table = xlat_addr;
	uint64_t desc;
	uint64_t attr = get_block_attr(tag);

	/* L0 entry stores a table descriptor (doesn't support blocks) */
	table = get_next_level_table(&table[l0_index], L1_XLAT_SIZE);

	/* L1 table lookup */
	if ((size >= L1_XLAT_SIZE) &&
	    IS_ALIGNED(base_addr, (1UL << L1_ADDR_SHIFT))) {
			/* If block address is aligned and size is greater than
			 * or equal to size addressed by each L1 entry, we can
			 * directly store a block desc */
			desc = base_addr | BLOCK_DESC | attr;
			table[l1_index] = desc;
			/* L2 lookup is not required */
			return L1_XLAT_SIZE;
	}

	/* L1 entry stores a table descriptor */
	table = get_next_level_table(&table[l1_index], L2_XLAT_SIZE);

	/* L2 table lookup */
	if ((size >= L2_XLAT_SIZE) &&
	    IS_ALIGNED(base_addr, (1UL << L2_ADDR_SHIFT))) {
		/* If block address is aligned and size is greater than
		 * or equal to size addressed by each L2 entry, we can
		 * directly store a block desc */
		desc = base_addr | BLOCK_DESC | attr;
		table[l2_index] = desc;
		/* L3 lookup is not required */
		return L2_XLAT_SIZE;
	}

	/* L2 entry stores a table descriptor */
	table = get_next_level_table(&table[l2_index], L3_XLAT_SIZE);

	/* L3 table lookup */
	desc = base_addr | PAGE_DESC | attr;
	table[l3_index] = desc;
	return L3_XLAT_SIZE;
}

/* Func : sanity_check
 * Desc : Check address/size alignment of a table or page.
 */
static void sanity_check(uint64_t addr, uint64_t size)
{
	assert(!(addr & GRANULE_SIZE_MASK) &&
	       !(size & GRANULE_SIZE_MASK) &&
	       (addr + size < (1UL << BITS_PER_VA)) &&
	       size >= GRANULE_SIZE);
}

/* Func : mmu_config_range
 * Desc : This function repeatedly calls init_xlat_table with the base
 * address. Based on size returned from init_xlat_table, base_addr is updated
 * and subsequent calls are made for initializing the xlat table until the whole
 * region is initialized.
 */
void mmu_config_range(void *start, size_t size, uint64_t tag)
{
	uint64_t base_addr = (uintptr_t)start;
	uint64_t temp_size = size;

	assert(tag < ARRAY_SIZE(tag_to_string));
	printf("Libpayload: ARM64 MMU: Mapping address range [%p:%p) as %s\n",
	       start, start + size, tag_to_string[tag]);
	sanity_check(base_addr, temp_size);

	while (temp_size)
		temp_size -= init_xlat_table(base_addr + (size - temp_size),
					     temp_size, tag);

	/* ARMv8 MMUs snoop L1 data cache, no need to flush it. */
	dsb();
	tlbiall_el2();
	dsb();
	isb();
}

/* Func : mmu_init
 * Desc : Initialize mmu based on the mmu_memrange passed. ttb_buffer is used as
 * the base address for xlat tables. TTB_DEFAULT_SIZE defines the max number of
 * tables that can be used
 * Assuming that memory 0-4GiB is device memory.
 */
uint64_t mmu_init(struct mmu_ranges *mmu_ranges)
{
	int i = 0;

	xlat_addr = (uint64_t *)&ttb_buffer;

	memset((void*)xlat_addr, 0, GRANULE_SIZE);
	max_tables = (TTB_DEFAULT_SIZE >> GRANULE_SIZE_SHIFT);
	free_idx = 1;

	printf("Libpayload ARM64: TTB_BUFFER: %p Max Tables: %d\n",
	       (void*)xlat_addr, max_tables);

	/*
	 * To keep things simple we start with mapping the entire base 4GB as
	 * device memory. This accommodates various architectures' default
	 * settings (for instance rk3399 mmio starts at 0xf8000000); it is
	 * fine tuned (e.g. mapping DRAM areas as write-back) later in the
	 * boot process.
	 */
	mmu_config_range(NULL, 0x100000000, TYPE_DEV_MEM);

	for (; i < mmu_ranges->used; i++)
		mmu_config_range((void *)mmu_ranges->entries[i].base,
				 mmu_ranges->entries[i].size,
				 mmu_ranges->entries[i].type);

	printf("Libpayload ARM64: MMU init done\n");
	return 0;
}

static uint32_t is_mmu_enabled(void)
{
	uint32_t sctlr;

	sctlr = raw_read_sctlr_el2();

	return (sctlr & SCTLR_M);
}

/*
 * Func: mmu_enable
 * Desc: Initialize MAIR, TCR, TTBR and enable MMU by setting appropriate bits
 * in SCTLR
 */
void mmu_enable(void)
{
	uint32_t sctlr;

	/* Initialize MAIR indices */
	raw_write_mair_el2(MAIR_ATTRIBUTES);

	/* Invalidate TLBs */
	tlbiall_el2();

	/* Initialize TCR flags */
	raw_write_tcr_el2(TCR_TOSZ | TCR_IRGN0_NM_WBWAC | TCR_ORGN0_NM_WBWAC |
			      TCR_SH0_IS | TCR_TG0_4KB | TCR_PS_256TB |
			      TCR_TBI_USED);

	/* Initialize TTBR */
	raw_write_ttbr0_el2((uintptr_t)xlat_addr);

	/* Ensure system register writes are committed before enabling MMU */
	isb();

	/* Enable MMU */
	sctlr = raw_read_sctlr_el2();
	sctlr |= SCTLR_C | SCTLR_M | SCTLR_I;
	raw_write_sctlr_el2(sctlr);

	isb();

	if(is_mmu_enabled())
		printf("ARM64: MMU enable done\n");
	else
		printf("ARM64: MMU enable failed\n");
}

/*
 * Func: mmu_add_memrange
 * Desc: Adds a new memory range
 */
static struct mmu_memrange *mmu_add_memrange(struct mmu_ranges *r,
					     uint64_t base, uint64_t size,
					     uint64_t type)
{
	struct mmu_memrange *curr = NULL;
	int i = r->used;

	if (i < ARRAY_SIZE(r->entries)) {
		curr = &r->entries[i];
		curr->base = base;
		curr->size = size;
		curr->type = type;

		r->used = i + 1;
	}

	return curr;
}

/* Structure to define properties of new memrange request */
struct mmu_new_range_prop {
	/* Type of memrange */
	uint64_t type;
	/* Size of the range */
	uint64_t size;
	/*
	 * If any restrictions on the max addr limit(This addr is exclusive for
	 * the range), else 0
	 */
	uint64_t lim_excl;
	/* If any restrictions on alignment of the range base, else 0 */
	uint64_t align;
	/*
	 * Function to test whether selected range is fine.
	 * NULL=any range is fine
	 * Return value 1=valid range, 0=otherwise
	 */
	int (*is_valid_range)(uint64_t, uint64_t);
	/* From what type of source range should this range be extracted */
	uint64_t src_type;
};

/*
 * Func: mmu_is_range_free
 * Desc: We need to ensure that the new range being allocated doesn't overlap
 * with any used memory range. Basically:
 * 1. Memory ranges used by the payload (usedmem_ranges)
 * 2. Any area that falls below _end symbol in linker script (Kernel needs to be
 * loaded in lower areas of memory, So, the payload linker script can have
 * kernel memory below _start and _end. Thus, we want to make sure we do not
 * step in those areas as well.
 * Returns: 1 on success, 0 on error
 * ASSUMPTION: All the memory used by payload resides below the program
 * proper. If there is any memory used above the _end symbol, then it should be
 * marked as used memory in usedmem_ranges during the presysinfo_scan.
 */
static int mmu_is_range_free(uint64_t r_base,
			     uint64_t r_end)
{
	uint64_t payload_end = (uint64_t)&_end;
	uint64_t i;
	struct mmu_memrange *r = &usedmem_ranges.entries[0];

	/* Allocate memranges only above payload */
	if ((r_base <= payload_end) || (r_end <= payload_end))
		return 0;

	for (i = 0; i < usedmem_ranges.used; i++) {
		uint64_t start = r[i].base;
		uint64_t end = start + r[i].size;

		if ((start < r_end) && (end > r_base))
			return 0;
	}

	return 1;
}

/*
 * Func: mmu_get_new_range
 * Desc: Add a requested new memrange. We take as input set of all memranges and
 * a structure to define the new memrange properties i.e. its type, size,
 * max_addr it can grow upto, alignment restrictions, source type to take range
 * from and finally a function pointer to check if the chosen range is valid.
 */
static struct mmu_memrange *mmu_get_new_range(struct mmu_ranges *mmu_ranges,
					      struct mmu_new_range_prop *new)
{
	int i = 0;
	struct mmu_memrange *r = &mmu_ranges->entries[0];

	if (new->size == 0) {
		printf("MMU Error: Invalid range size\n");
		return NULL;
	}

	for (; i < mmu_ranges->used; i++) {

		if ((r[i].type != new->src_type) ||
		    (r[i].size < new->size) ||
		    (new->lim_excl && (r[i].base >= new->lim_excl)))
			continue;

		uint64_t base_addr;
		uint64_t range_end_addr = r[i].base + r[i].size;
		uint64_t end_addr = range_end_addr;

		/* Make sure we do not go above max if it is non-zero */
		if (new->lim_excl && (end_addr >= new->lim_excl))
			end_addr = new->lim_excl;

		while (1) {
			/*
			 * In case of alignment requirement,
			 * if end_addr is aligned, then base_addr will be too.
			 */
			if (new->align)
				end_addr = ALIGN_DOWN(end_addr, new->align);

			base_addr = end_addr - new->size;

			if (base_addr < r[i].base)
				break;

			/*
			 * If the selected range is not used and valid for the
			 * user, move ahead with it
			 */
			if (mmu_is_range_free(base_addr, end_addr) &&
			    ((new->is_valid_range == NULL) ||
			     new->is_valid_range(base_addr, end_addr)))
				break;

			/* Drop to the next address. */
			end_addr -= 1;
		}

		if (base_addr < r[i].base)
			continue;

		if (end_addr != range_end_addr) {
			/* Add a new memrange since we split up one
			 * range crossing the 4GiB boundary or doing an
			 * ALIGN_DOWN on end_addr.
			 */
			r[i].size -= (range_end_addr - end_addr);
			if (mmu_add_memrange(mmu_ranges, end_addr,
					     range_end_addr - end_addr,
					     r[i].type) == NULL)
				mmu_error();
		}

		if (r[i].size == new->size) {
			r[i].type = new->type;
			return &r[i];
		}

		r[i].size -= new->size;

		r = mmu_add_memrange(mmu_ranges, base_addr, new->size,
				     new->type);

		if (r == NULL)
			mmu_error();

		return r;
	}

	/* Should never reach here if everything went fine */
	printf("ARM64 ERROR: No region allocated\n");
	return NULL;
}

/*
 * Func: mmu_alloc_range
 * Desc: Call get_new_range to get a new memrange which is unused and mark it as
 * used to avoid same range being allocated for different purposes.
 */
static struct mmu_memrange *mmu_alloc_range(struct mmu_ranges *mmu_ranges,
					    struct mmu_new_range_prop *p)
{
	struct mmu_memrange *r = mmu_get_new_range(mmu_ranges, p);

	if (r == NULL)
		return NULL;

	/*
	 * Mark this memrange as used memory. Important since function
	 * can be called multiple times and we do not want to reuse some
	 * range already allocated.
	 */
	if (mmu_add_memrange(&usedmem_ranges, r->base, r->size, r->type)
	    == NULL)
		mmu_error();

	return r;
}

/*
 * Func: mmu_add_dma_range
 * Desc: Add a memrange for dma operations. This is special because we want to
 * initialize this memory as non-cacheable. We have a constraint that the DMA
 * buffer should be below 4GiB(32-bit only). So, we lookup a TYPE_NORMAL_MEM
 * from the lowest available addresses and align it to page size i.e. 64KiB.
 */
static struct mmu_memrange *mmu_add_dma_range(struct mmu_ranges *mmu_ranges)
{
	struct mmu_new_range_prop prop;

	prop.type = TYPE_DMA_MEM;
	/* DMA_DEFAULT_SIZE is multiple of GRANULE_SIZE */
	assert((DMA_DEFAULT_SIZE % GRANULE_SIZE) == 0);
	prop.size = DMA_DEFAULT_SIZE;
	prop.lim_excl = (uint64_t)CONFIG_LP_DMA_LIM_EXCL * MiB;
	prop.align = GRANULE_SIZE;
	prop.is_valid_range = NULL;
	prop.src_type = TYPE_NORMAL_MEM;

	return mmu_alloc_range(mmu_ranges, &prop);
}

static struct mmu_memrange *_mmu_add_fb_range(
		uint32_t size,
		struct mmu_ranges *mmu_ranges)
{
	struct mmu_new_range_prop prop;

	prop.type = TYPE_DMA_MEM;

	/* make sure to allocate a size of multiple of GRANULE_SIZE */
	size = ALIGN_UP(size, GRANULE_SIZE);
	prop.size = size;
	prop.lim_excl = MIN_64_BIT_ADDR;
	prop.align = MB_SIZE;
	prop.is_valid_range = NULL;
	prop.src_type = TYPE_NORMAL_MEM;

	return mmu_alloc_range(mmu_ranges, &prop);
}

/*
 * Func: mmu_extract_ranges
 * Desc: Assumption is that coreboot tables have memranges in sorted
 * order. So, if there is an opportunity to combine ranges, we do that as
 * well. Memranges are initialized for both CB_MEM_RAM and CB_MEM_TABLE as
 * TYPE_NORMAL_MEM.
 */
static void mmu_extract_ranges(struct memrange *cb_ranges,
			       uint64_t ncb,
			       struct mmu_ranges *mmu_ranges)
{
	int i = 0;
	struct mmu_memrange *prev_range = NULL;

	/* Extract memory ranges to be mapped */
	for (; i < ncb; i++) {
		switch (cb_ranges[i].type) {
		case CB_MEM_RAM:
		case CB_MEM_TABLE:
			if (prev_range && (prev_range->base + prev_range->size
					   == cb_ranges[i].base)) {
				prev_range->size += cb_ranges[i].size;
			} else {
				prev_range = mmu_add_memrange(mmu_ranges,
							      cb_ranges[i].base,
							      cb_ranges[i].size,
							      TYPE_NORMAL_MEM);
				if (prev_range == NULL)
					mmu_error();
			}
			break;
		default:
			break;
		}
	}
}

static void mmu_add_fb_range(struct mmu_ranges *mmu_ranges)
{
	struct mmu_memrange *fb_range;
	struct cb_framebuffer *framebuffer = &lib_sysinfo.framebuffer;
	uint32_t fb_size;

	/* Check whether framebuffer is needed */
	fb_size = framebuffer->bytes_per_line * framebuffer->y_resolution;
	if (!fb_size)
		return;

	/* framebuffer address has been set already, so just add it as DMA */
	if (framebuffer->physical_address) {
		if (mmu_add_memrange(mmu_ranges,
		    framebuffer->physical_address,
		    fb_size,
		    TYPE_DMA_MEM) == NULL)
			mmu_error();
		return;
	}

	/* Allocate framebuffer */
	fb_range = _mmu_add_fb_range(fb_size, mmu_ranges);
	if (fb_range == NULL)
		mmu_error();

	framebuffer->physical_address = fb_range->base;
}

/*
 * Func: mmu_init_ranges
 * Desc: Initialize mmu_memranges based on the memranges obtained from coreboot
 * tables. Also, initialize dma memrange and xlat_addr for ttb buffer.
 */
struct mmu_memrange *mmu_init_ranges_from_sysinfo(struct memrange *cb_ranges,
						  uint64_t ncb,
						  struct mmu_ranges *mmu_ranges)
{
	struct mmu_memrange *dma_range;

	/* Initialize mmu_ranges to contain no entries. */
	mmu_ranges->used = 0;

	/* Extract ranges from memrange in lib_sysinfo */
	mmu_extract_ranges(cb_ranges, ncb, mmu_ranges);

	/* Get a range for dma */
	dma_range = mmu_add_dma_range(mmu_ranges);

	/* Get a range for framebuffer */
	mmu_add_fb_range(mmu_ranges);

	if (dma_range == NULL)
		mmu_error();

	return dma_range;
}

/*
 * Func: mmu_presysinfo_memory_used
 * Desc: Initializes all the memory used for presysinfo page table
 * initialization and enabling of MMU. All these ranges are stored in
 * usedmem_ranges. usedmem_ranges plays an important role in selecting the dma
 * buffer as well since we check the dma buffer range against the used memory
 * ranges to prevent any overstepping.
 */
void mmu_presysinfo_memory_used(uint64_t base, uint64_t size)
{
	uint64_t range_base;

	range_base = ALIGN_DOWN(base, GRANULE_SIZE);

	size += (base - range_base);
	size = ALIGN_UP(size, GRANULE_SIZE);

	mmu_add_memrange(&usedmem_ranges, range_base, size, TYPE_NORMAL_MEM);
}

void mmu_presysinfo_enable(void)
{
	mmu_init(&usedmem_ranges);
	mmu_enable();
}

const struct mmu_ranges *mmu_get_used_ranges(void)
{
	return &usedmem_ranges;
}
