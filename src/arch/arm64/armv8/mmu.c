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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <memrange.h>
#include <arch/mmu.h>
#include <arch/lib_helpers.h>
#include <arch/cache.h>

/* Maximum number of XLAT Tables available based on ttb buffer size */
static unsigned int max_tables;
/* Address of ttb buffer */
static uint64_t *xlat_addr;
static int free_idx;

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
		if (tag & MA_MEM_NC)
			attr |= BLOCK_INDEX_MEM_NORMAL_NC << BLOCK_INDEX_SHIFT;
		else
			attr |= BLOCK_INDEX_MEM_NORMAL << BLOCK_INDEX_SHIFT;
	} else {
		attr |= BLOCK_INDEX_MEM_DEV_NGNRNE << BLOCK_INDEX_SHIFT;
	}

	return attr;
}

/* Func : get_index_from_addr
 * Desc : Get index into table at a given level using appropriate bits from the
 * base address
 */
static uint64_t get_index_from_addr(uint64_t addr, uint8_t level)
{
	uint64_t mask = level_to_addr_mask[level-1];
	uint8_t shift = level_to_addr_shift[level-1];

	return ((addr & mask) >> shift);
}

/* Func : table_desc_valid
 * Desc : Check if a table entry contains valid desc
 */
static uint64_t table_desc_valid(uint64_t desc)
{
	return((desc & TABLE_DESC) == TABLE_DESC);
}

/* Func : get_new_table
 * Desc : Return the next free XLAT table from ttb buffer
 */
static uint64_t *get_new_table(void)
{
	uint64_t *new;

	if (free_idx >= max_tables) {
		return NULL;
	}

	new = (uint64_t*)((unsigned char *)xlat_addr + free_idx * GRANULE_SIZE);
	free_idx++;

	memset(new, 0, GRANULE_SIZE);

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
 * Desc: Check if the table entry is a valid descriptor. If not, allocate new
 * table, update the entry and return the table addr. If valid, return the addr
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

/* Func : init_xlat_table
 * Desc : Given a base address and size, it identifies the indices within
 * different level XLAT tables which map the given base addr. Similar to table
 * walk, except that all invalid entries during the walk are updated
 * accordingly. On success, it returns the size of the block/page addressed by
 * the final table
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

	/* L1 table lookup */
	/* If VA has bits more than 41, lookup starts at L1 */
	if (l1_index) {
		table = get_next_level_table(&table[l1_index]);
		if (!table)
			return 0;
	}

	/* L2 table lookup */
	/* If lookup was performed at L1, L2 table addr is obtained from L1 desc
	   else, lookup starts at ttbr address */
	if (!l3_index && (size >= L2_XLAT_SIZE)) {
		/* If block address is aligned and size is greater than or equal
		   to 512MiB i.e. size addressed by each L2 entry, we can
		   directly store a block desc */
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

/* Func : sanity_check
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

/* Func : init_mmap_entry
 * Desc : For each mmap entry, this function calls init_xlat_table with the base
 * address. Based on size returned from init_xlat_table, base_addr is updated
 * and subsequent calls are made for initializing the xlat table until the whole
 * region is initialized.
 */
static void init_mmap_entry(struct range_entry *r)
{
	uint64_t base_addr = range_entry_base(r);
	uint64_t size      = range_entry_size(r);
	uint64_t tag       = range_entry_tag(r);
	uint64_t temp_size = size;

	while (temp_size) {
		uint64_t ret;

		if (sanity_check(base_addr,temp_size)) {
			return;
		}

		ret = init_xlat_table(base_addr + (size - temp_size),
				      temp_size,tag);

		if (ret == 0)
			return;

		temp_size -= ret;
	}
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

	if (sanity_check((uint64_t)ttb_buffer, ttb_size)) {
		return;
	}

	memset((void*)ttb_buffer, 0, GRANULE_SIZE);
	max_tables = (ttb_size >> GRANULE_SIZE_SHIFT);
	xlat_addr = ttb_buffer;
	free_idx = 1;

	memranges_each_entry(mmap_entry, mmap_ranges) {
		init_mmap_entry(mmap_entry);
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
			  TCR_SH0_IS | TCR_TG0_64KB | TCR_PS_64GB |
			  TCR_TBI_USED);

	/* Initialize TTBR */
	raw_write_ttbr0_el3((uintptr_t)xlat_addr);

	/* Ensure all translation table writes are committed before enabling MMU */
	dsb();
	isb();

	/* Enable MMU */
	sctlr = raw_read_sctlr_el3();
	sctlr |= SCTLR_C | SCTLR_M | SCTLR_I;
	raw_write_sctlr_el3(sctlr);

	isb();
}
