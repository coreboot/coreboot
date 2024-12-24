/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <bootmem.h>
#include <cbmem.h>
#include <commonlib/bsd/mem_chip_info.h>
#include <soc/dramc_info.h>
#include <soc/dramc_soc.h>
#include <symbols.h>

void reserve_buffer_for_dramc(void)
{
	const struct mem_chip_info *mc = cbmem_find(CBMEM_ID_MEM_CHIP_INFO);
	int i;
	const uint32_t reserved_size = 64 * KiB;
	uint64_t cbmem_top_addr = cbmem_top();
	uint64_t reserved_addr;
	uint64_t rank_size_sum = 0;
	uint64_t rank_size[RANK_MAX] = {0};

	assert(mc);

	for (i = 0; i < mc->num_entries; i++) {
		const struct mem_chip_entry *e = &mc->entries[i];
		assert(e->rank < RANK_MAX);
		rank_size[e->rank] += mem_chip_info_entry_density_bytes(e);
	}

	for (i = 0; i < RANK_MAX; i++) {
		if (rank_size[i] == 0)
			continue;
		rank_size_sum += rank_size[i];
		assert(rank_size_sum >= reserved_size);

		reserved_addr = (uint64_t)_dram + rank_size_sum - reserved_size;
		/* Reserved range must not overlap with CBMEM. */
		assert(reserved_addr >= cbmem_top_addr);
		bootmem_add_range(reserved_addr, reserved_size, BM_MEM_RESERVED);
	}
}
