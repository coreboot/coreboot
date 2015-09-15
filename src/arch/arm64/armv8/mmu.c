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

#include <console/console.h>
#include <memrange.h>
#include <arch/mmu.h>
#include <arch/lib_helpers.h>
#include <arch/cache.h>

/* Maximum number of XLAT Tables available based on ttb buffer size */
static unsigned int max_tables;
/* Address of ttb buffer */
static uint64_t *xlat_addr;
static int free_idx;

static void print_tag(int level, uint64_t tag)
{
	printk(level, tag & MA_MEM_NC ? "non-cacheable | " :
					"    cacheable | ");
	printk(level, tag & MA_RO ?	"read-only  | " :
					"read-write | ");
	printk(level, tag & MA_NS ?	"non-secure | " :
					"    secure | ");
	printk(level, tag & MA_MEM ?	"normal\n" :
					"device\n");
}

/* Func : get_block_attr
 * Desc : Get block descriptor attributes based on the value of tag in memrange
 * region
 */
static uint64_t get_block_attr(unsigned long tag)
{
	uint64_t attr;

	attr = (tag & MA_NS)? BLOCK_NS : 0;
	attr |= (tag & MA_RO)? BLOCK_AP_RO : BLOCK_AP_RW;
	attr |= BLOCK_ACCESS;

	if (tag & MA_MEM) {
		attr |= BLOCK_SH_INNER_SHAREABLE;
		if (tag & MA_MEM_NC)
			attr |= BLOCK_INDEX_MEM_NORMAL_NC << BLOCK_INDEX_SHIFT;
		else
			attr |= BLOCK_INDEX_MEM_NORMAL << BLOCK_INDEX_SHIFT;
	} else {
		attr |= BLOCK_INDEX_MEM_DEV_NGNRNE << BLOCK_INDEX_SHIFT;
		attr |= BLOCK_XN;
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
 * table, update the entry and return the table addr. If valid, return the addr
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
	uint64_t l1_index = (base_addr & L1_ADDR_MASK) >> L1_ADDR_SHIFT;
	uint64_t l2_index = (base_addr & L2_ADDR_MASK) >> L2_ADDR_SHIFT;
	uint64_t l3_index = (base_addr & L3_ADDR_MASK) >> L3_ADDR_SHIFT;
	uint64_t *table = xlat_addr;
	uint64_t desc;
	uint64_t attr = get_block_attr(tag);

	/* L1 table lookup
	 * If VA has bits more than L2 can resolve, lookup starts at L1
	 * Assumption: we don't need L0 table in coreboot */
	if (BITS_PER_VA > L1_ADDR_SHIFT) {
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
		table = get_next_level_table(&table[l1_index], L2_XLAT_SIZE);
	}

	/* L2 table lookup
	 * If lookup was performed at L1, L2 table addr is obtained from L1 desc
	 * else, lookup starts at ttbr address */
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

	if (!IS_ENABLED(CONFIG_SMP)) {
		printk(BIOS_INFO, "Mapping address range [%p:%p) as ",
		       start, start + size);
		print_tag(BIOS_INFO, tag);
	}

	sanity_check(base_addr, temp_size);

	while (temp_size)
		temp_size -= init_xlat_table(base_addr + (size - temp_size),
					     temp_size, tag);

	/* ARMv8 MMUs snoop L1 data cache, no need to flush it. */
	dsb();
	tlbiall_current();
	dsb();
	isb();
}

/* Func : mmu_init
 * Desc : Initialize mmu based on the mmap_ranges passed. ttb_buffer is used as
 * the base address for xlat tables. ttb_size defines the max number of tables
 * that can be used
 */
void mmu_init(struct memranges *mmap_ranges,
	      uint64_t *ttb_buffer,
	      uint64_t ttb_size)
{
	struct range_entry *mmap_entry;

	sanity_check((uint64_t)ttb_buffer, ttb_size);

	memset((void*)ttb_buffer, 0, GRANULE_SIZE);
	max_tables = (ttb_size >> GRANULE_SIZE_SHIFT);
	xlat_addr = ttb_buffer;
	free_idx = 1;

	if (mmap_ranges)
		memranges_each_entry(mmap_entry, mmap_ranges) {
			mmu_config_range((void *)range_entry_base(mmap_entry),
					 range_entry_size(mmap_entry),
					 range_entry_tag(mmap_entry));
		}
}

void mmu_enable(void)
{
	uint32_t sctlr;

	/* Initialize MAIR indices */
	raw_write_mair_el3(MAIR_ATTRIBUTES);

	/* Invalidate TLBs */
	tlbiall_el3();

	/* Initialize TCR flags */
	raw_write_tcr_el3(TCR_TOSZ | TCR_IRGN0_NM_WBWAC | TCR_ORGN0_NM_WBWAC |
			  TCR_SH0_IS | TCR_TG0_4KB | TCR_PS_64GB |
			  TCR_TBI_USED);

	/* Initialize TTBR */
	raw_write_ttbr0_el3((uintptr_t)xlat_addr);

	/* Ensure system register writes are committed before enabling MMU */
	isb();

	/* Enable MMU */
	sctlr = raw_read_sctlr_el3();
	sctlr |= SCTLR_C | SCTLR_M | SCTLR_I;
	raw_write_sctlr_el3(sctlr);

	isb();
}
