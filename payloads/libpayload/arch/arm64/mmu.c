/*
 * This file is part of the coreboot project.
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

static const uint64_t level_to_addr_mask[] = {
	L1_ADDR_MASK,
	L2_ADDR_MASK,
	L3_ADDR_MASK,
};

static const uint64_t level_to_addr_shift[] = {
	L1_ADDR_SHIFT,
	L2_ADDR_SHIFT,
	L3_ADDR_SHIFT,
};

static void __attribute__((noreturn)) mmu_error(void)
{
	halt();
}

/*
 * Func : get_block_attr
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
		attr |= (BLOCK_INDEX_MEM_NORMAL << BLOCK_INDEX_SHIFT);
		break;
	case TYPE_DEV_MEM:
		attr |= BLOCK_INDEX_MEM_DEV_NGNRNE << BLOCK_INDEX_SHIFT;
		break;
	case TYPE_DMA_MEM:
		attr |= BLOCK_INDEX_MEM_NORMAL_NC << BLOCK_INDEX_SHIFT;
		break;
	}

	return attr;
}

/*
 * Func : get_index_from_addr
 * Desc : Get index into table at a given level using appropriate bits from the
 * base address
 */
static uint64_t get_index_from_addr(uint64_t addr, uint8_t level)
{
	uint64_t mask = level_to_addr_mask[level-1];
	uint8_t shift = level_to_addr_shift[level-1];

	return ((addr & mask) >> shift);
}

/*
 * Func : table_desc_valid
 * Desc : Check if a table entry contains valid desc
 */
static uint64_t table_desc_valid(uint64_t desc)
{
	return((desc & TABLE_DESC) == TABLE_DESC);
}

/*
 * Func : get_new_table
 * Desc : Return the next free XLAT table from ttb buffer
 */
static uint64_t *get_new_table(void)
{
	uint64_t *new;

	if (free_idx >= max_tables) {
		printf("ARM64 MMU: No free table\n");
		return NULL;
	}

	new = (uint64_t*)((unsigned char *)xlat_addr + free_idx * GRANULE_SIZE);
	free_idx++;

	memset(new, 0, GRANULE_SIZE);

	return new;
}

/*
 * Func : get_table_from_desc
 * Desc : Get next level table address from table descriptor
 */
static uint64_t *get_table_from_desc(uint64_t desc)
{
	uint64_t *ptr = (uint64_t*)(desc & XLAT_TABLE_MASK);
	return ptr;
}

/*
 * Func: get_next_level_table
 * Desc: Check if the table entry is a valid descriptor. If not, allocate new
 * table, update the entry and return the table addr. If valid, return the addr.
 */
static uint64_t *get_next_level_table(uint64_t *ptr)
{
	uint64_t desc = *ptr;

	if (!table_desc_valid(desc)) {
		uint64_t *new_table = get_new_table();
		if (new_table == NULL)
			return NULL;
		desc = ((uint64_t)new_table) | TABLE_DESC;
		*ptr = desc;
	}
	return get_table_from_desc(desc);
}

/*
 * Func : init_xlat_table
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
	uint64_t l1_index = get_index_from_addr(base_addr,1);
	uint64_t l2_index = get_index_from_addr(base_addr,2);
	uint64_t l3_index = get_index_from_addr(base_addr,3);
	uint64_t *table = xlat_addr;
	uint64_t desc;
	uint64_t attr = get_block_attr(tag);

	/*
	 * L1 table lookup
	 * If VA has bits more than 41, lookup starts at L1
	 */
	if (l1_index) {
		table = get_next_level_table(&table[l1_index]);
		if (!table)
			return 0;
	}

	/*
	 * L2 table lookup
	 * If lookup was performed at L1, L2 table addr is obtained from L1 desc
	 * else, lookup starts at ttbr address
	 */
	if (!l3_index && (size >= L2_XLAT_SIZE)) {
		/*
		 * If block address is aligned and size is greater than or equal
		 * to 512MiB i.e. size addressed by each L2 entry, we can
		 * directly store a block desc
		 */
		desc = base_addr | BLOCK_DESC | attr;
		table[l2_index] = desc;
		/* L3 lookup is not required */
		return L2_XLAT_SIZE;
	} else {
		/* L2 entry stores a table descriptor */
		table = get_next_level_table(&table[l2_index]);
		if (!table)
			return 0;
	}

	/* L3 table lookup */
	desc = base_addr | PAGE_DESC | attr;
	table[l3_index] = desc;
	return L3_XLAT_SIZE;
}

/*
 * Func : sanity_check
 * Desc : Check if the address is aligned and size is atleast the granule size
 */
static uint64_t sanity_check(uint64_t addr,
			     uint64_t size)
{
	/* Address should be atleast 64 KiB aligned */
	if (addr & GRANULE_SIZE_MASK)
		return 1;

	/* Size should be atleast granule size */
	if (size < GRANULE_SIZE)
		return 1;

	return 0;
}

/*
 * Func : init_mmap_entry
 * Desc : For each mmap entry, this function calls init_xlat_table with the base
 * address. Based on size returned from init_xlat_table, base_addr is updated
 * and subsequent calls are made for initializing the xlat table until the whole
 * region is initialized.
 */
static void init_mmap_entry(struct mmu_memrange *r)
{
	uint64_t base_addr = r->base;
	uint64_t size	   = r->size;
	uint64_t tag	   = r->type;
	uint64_t temp_size = size;

	while (temp_size) {
		uint64_t ret;

		if (sanity_check(base_addr,temp_size)) {
			printf("Libpayload: ARM64 MMU: sanity check failed\n");
			return;
		}

		ret = init_xlat_table(base_addr + (size - temp_size),
				      temp_size, tag);

		if (ret == 0)
			return;

		temp_size -= ret;
	}
}

/*
 * Func : mmu_init
 * Desc : Initialize mmu based on the mmu_memrange passed. ttb_buffer is used as
 * the base address for xlat tables. TTB_DEFAULT_SIZE defines the max number of
 * tables that can be used
 * Assuming that memory 0-2GiB is device memory.
 */
uint64_t mmu_init(struct mmu_ranges *mmu_ranges)
{
	struct mmu_memrange devrange = { 0, 0x80000000, TYPE_DEV_MEM };

	int i = 0;

	xlat_addr = (uint64_t *)&ttb_buffer;

	memset((void*)xlat_addr, 0, GRANULE_SIZE);
	max_tables = (TTB_DEFAULT_SIZE >> GRANULE_SIZE_SHIFT);
	free_idx = 1;

	printf("Libpayload ARM64: TTB_BUFFER: 0x%p Max Tables: %d\n",
	       (void*)xlat_addr, max_tables);

	init_mmap_entry(&devrange);

	for (; i < mmu_ranges->used; i++) {
		init_mmap_entry(&mmu_ranges->entries[i]);
	}

	printf("Libpayload ARM64: MMU init done\n");
	return 0;
}

static uint32_t is_mmu_enabled(void)
{
	uint32_t sctlr;

	sctlr = raw_read_sctlr_current();

	return (sctlr & SCTLR_M);
}

/*
 * Func: mmu_disable
 * Desc: Invalidate caches and disable mmu
 */
void mmu_disable(void)
{
	uint32_t el = get_current_el();
	uint32_t sctlr;

	sctlr = raw_read_sctlr(el);
	sctlr &= ~(SCTLR_C | SCTLR_M | SCTLR_I);

	tlbiall_current();
	dcache_clean_invalidate_all();

	dsb();
	isb();

	raw_write_sctlr(sctlr, el);

	dcache_clean_invalidate_all();
	dsb();
	isb();
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
	raw_write_mair_current(MAIR_ATTRIBUTES);

	/* Invalidate TLBs */
	tlbiall_current();

	/* Initialize TCR flags */
	raw_write_tcr_current(TCR_TOSZ | TCR_IRGN0_NM_WBWAC | TCR_ORGN0_NM_WBWAC |
			      TCR_SH0_IS | TCR_TG0_64KB | TCR_PS_64GB |
			      TCR_TBI_USED);

	/* Initialize TTBR */
	raw_write_ttbr0_current((uintptr_t)xlat_addr);

	/* Ensure all translation table writes are committed before enabling MMU */
	dsb();
	isb();

	/* Enable MMU */
	sctlr = raw_read_sctlr_current();
	sctlr |= SCTLR_C | SCTLR_M | SCTLR_I;
	raw_write_sctlr_current(sctlr);

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
 * Desc: We need to ensure that the new range being allocated doesnt overlap
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

		if (((r_base >= start) && (r_base <= end)) ||
		    ((r_end >= start) && (r_end <= end)))
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
	static struct cb_framebuffer modified_fb;
	struct cb_framebuffer *framebuffer = lib_sysinfo.framebuffer;
	uint32_t fb_size;

	/*
	 * Check whether framebuffer is needed
	 * or framebuffer address has been set already
	 */
	if (framebuffer == NULL)
		return;
	if (framebuffer->physical_address)
		return;
	fb_size = framebuffer->bytes_per_line * framebuffer->y_resolution;
	if (!fb_size)
		return;

	/* Allocate framebuffer */
	fb_range = _mmu_add_fb_range(fb_size, mmu_ranges);
	if (fb_range == NULL)
		mmu_error();

	/*
	 * Set framebuffer address. However, one needs to use a freshly
	 * allocated framebuffer structure because the one in the coreboot
	 * table is part of a checksum calculation. Therefore, one cannot
	 * modify a field without recomputing the necessary checksum
	 * calcuation.
	 */
	modified_fb = *framebuffer;
	modified_fb.physical_address = fb_range->base;
	lib_sysinfo.framebuffer = &modified_fb;
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
